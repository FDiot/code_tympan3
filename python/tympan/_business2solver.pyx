import numpy as np
import cython as cy
from cython.operator cimport dereference as deref, preincrement as inc
from libcpp cimport bool
from libcpp.map cimport map
from libcpp.deque cimport deque
from libcpp.vector cimport vector
from libcpp.string cimport string

from tympan._core cimport shared_ptr
from tympan.models cimport _business as tybusiness
from tympan.models cimport _solver as tysolver
from tympan.models cimport _common as tycommon


cdef business2microsource(map[tybusiness.TYElem_ptr, vector[SmartPtr[tybusiness.TYGeometryNode]]] map_sources):
    """Business2MicroSource wrapping a map (TYElement*, SmartPtr[TYGeometryNode])
    """
    b2ms = Business2MicroSource()
    b2ms.map_sources = map_sources
    return b2ms

def load_computation_solver(foldername, tybusiness.Computation comp):
    """Load a solver plugin (from 'foldername' folder) to compute 'comp'"""
    solver = cy.declare(tysolver.Solver)
    solver = tysolver.Solver()
    solver.thisptr = load_solver(foldername, comp.thisptr.getRealPointer().getSolverId());
    return solver

cdef id_str(tybusiness.TYElement* elt):
    """Return 'elt' unique ID as a string"""
    return elt.getID().toString().toStdString()

cdef class Business2SolverConverter:
    """Offer mappings between solver and business models

    Allows to build a solver model from a business site, and keeps business/solver
    mappings so as to make it possible to send back solver results to the
    business model after a solver computation.
    """
    comp = cy.declare(tybusiness.Computation)
    site = cy.declare(tybusiness.Site)
    # business receptor uuids (string) to solver receptor indices
    bus2solv_receptors = cy.declare(dict)
    # Receptors that are mesh points and therefore will be removed from the final
    # result matrix after the solver computation:
    to_be_removed_receptors = cy.declare(list)
    # business source uuids to solver source indices
    bus2solv_sources = cy.declare(dict)
    # macro source uuid to micro source uuid (both from business model)
    macro2micro_sources = cy.declare(dict)

    # transitional result matrix (from solver matrix to condensed business matrix)
    transitional_result_matrix = cy.declare(cy.pointer(tycommon.SpectrumMatrix))
    # maps an UUID to the corresponding TYElement*
    # XXX Can't use a dict because of the "Cannot convert 'Thing*' to Python object"
    # compilation error. One solution would be to use CObjects
    # (PyCObject_FromVoidPtr/PyCObject_AsVoidPtr) to add and get back the pointer.
    instances_mapping = cy.declare(map[string, cy.pointer(tybusiness.TYElement)])

    @cy.locals(comp=tybusiness.Computation, site=tybusiness.Site)
    def __cinit__(self, comp, site):
        self.comp = comp
        self.site = site
        self.bus2solv_receptors = dict()
        self.to_be_removed_receptors = []
        self.bus2solv_sources = dict()
        self.macro2micro_sources = dict()

    @cy.locals(model=tysolver.ProblemModel, result=tysolver.ResultModel)
    def postprocessing(self, model, result):
        """Post-process solver result to reinject them to the business result
        """
        # Retrieve solver result matrix
        solver_result_matrix = cy.declare(tycommon.SpectrumMatrix)
        solver_result_matrix = result.thisptr.get().get_data()
        self.transitional_result_matrix = new tycommon.SpectrumMatrix(solver_result_matrix)
        # update business receptors cumulative spectra
        self.update_business_receptors(model, result)
        self.remove_mesh_points_from_results()
        # condensate result matrix
        self.update_business_result_matrix()
        self.comp.thisptr.getRealPointer().goPostprocessing()
        # Clear intermediate data
        del self.transitional_result_matrix

    @cy.locals(model=tysolver.ProblemModel, result=tysolver.ResultModel)
    def update_business_receptors(self, model, result):
        """Update business receptor by cumulating spectra perceived from the sources

        Once the acoustic problem has been solved, send back the acoustic results
        to the business receptors
        """
        # resize business result matrix with the number of enabled sources and receptors:
        business_result = cy.declare(tybusiness.Result)
        business_result = self.comp.result
        business_result_matrix = cy.declare(cy.pointer(tycommon.SpectrumMatrix))
        business_result_matrix = cy.address(business_result.thisptr.getRealPointer().getResultMatrix())
        business_result_matrix.resize(model.nreceptors, model.nsources)
        for brec_id in self.bus2solv_receptors:
            receptor = cy.declare(cy.pointer(tybusiness.TYPointCalcul))
            receptor = tybusiness.downcast_point_calcul(
                deref(self.instances_mapping.find(brec_id)).second)
            # retrieve receptor spectra
            spectra = cy.declare(vector[tycommon.OSpectre])
            spectra = self.transitional_result_matrix[0].by_receptor(
                self.bus2solv_receptors[brec_id])
            # receptor cumulative spectrum
            cumul_spectrum = cy.declare(tycommon.OSpectre)
            cumul_spectrum.setDefaultValue(0.0)
            for i in xrange(spectra.size()):
                cumul_spectrum = cumul_spectrum.sum(spectra[i])
            cumul_spectrum.setType(tycommon.SPECTRE_TYPE_LP) # enum TYSpectreType from OSpectre
            # The values retrieved from the solver are linear. We want to convert
            # them to DB.
            cumul_spectrum.setEtat(tycommon.SPECTRE_ETAT_LIN)
            cumul_spectrum = cumul_spectrum.toDB()
            receptor.setSpectre(tybusiness.TYSpectre(cumul_spectrum),
                                self.comp.thisptr.getRealPointer())
        busresult = cy.declare(cy.pointer(tybusiness.TYResultat))
        busresult = self.comp.thisptr.getRealPointer().getResultat().getRealPointer()
        busresult.setIsAcousticModified(False)

    def update_business_result_matrix(self):
        """Condensate result matrix

        It originally contains 1 spectrum per pair (business receptor, micro source),
        and will now contain 1 cumulative spectrum per pair
        (business receptor, business source)
        """
        busresult = cy.declare(cy.pointer(tybusiness.TYResultat))
        busresult = self.comp.thisptr.getRealPointer().getResultat().getRealPointer()
        result_sources = cy.declare(map[tybusiness.TYElem_ptr, int])
        condensate_matrix = cy.declare(tycommon.SpectrumMatrix)
        condensate_matrix.resize(len(self.bus2solv_receptors), len(self.macro2micro_sources))
        rec_counter = 0
        # Go through all the business receptors
        for brec_id in self.bus2solv_receptors:
            receptor = cy.declare(cy.pointer(tybusiness.TYPointCalcul))
            receptor = tybusiness.downcast_point_calcul(
                deref(self.instances_mapping.find(brec_id)).second)
            busresult.addRecepteur(receptor)
            receptor_id = busresult.getIndexRecepteur(receptor)
            source_counter = 0
            # Go through all the business infrastructure sources
            for macro_source_id in self.macro2micro_sources:
                valid_spectrum = True
                cumul_spectrum = cy.declare(tycommon.OSpectre)
                cumul_spectrum.setDefaultValue(0.0)
                # Go through all their business subsources
                for micro_source_id in self.macro2micro_sources[macro_source_id]:
                    # Get solver result for this subsource
                    subsource_idx = self.bus2solv_sources[micro_source_id]
                    cur_spectrum = cy.declare(tycommon.OSpectre)
                    cur_spectrum = self.transitional_result_matrix[0].element(
                        self.bus2solv_receptors[brec_id], subsource_idx)
                    cumul_spectrum = cumul_spectrum.sum(cur_spectrum)
                    valid_spectrum &= cur_spectrum.isValid()
                cumul_spectrum.setValid(valid_spectrum)
                cumul_spectrum.setType(tycommon.SPECTRE_TYPE_LP)
                # The values retrieved from the solver are linear. We want to convert
                # them to DB.
                cumul_spectrum.setEtat(tycommon.SPECTRE_ETAT_LIN)
                cumul_spectrum = cumul_spectrum.toDB()
                condensate_matrix.setSpectre(receptor_id, source_counter, cumul_spectrum)
                macro_source = deref(self.instances_mapping.find(macro_source_id)).second
                result_sources[macro_source] = source_counter
                source_counter += 1
            rec_counter += 1
        busresult.setResultMatrix(condensate_matrix)
        busresult.setSources(result_sources)

    def remove_mesh_points_from_results(self):
        """Remove mesh points from the receptors in the result matrix

        In the final result matrix we don't keep mesh points (they were generated
        for the solver resolution but once computation is done we settle for a synthetic
        result
        """
        while self.to_be_removed_receptors:
            uuid = self.to_be_removed_receptors.pop()
            del self.bus2solv_receptors[uuid]

    @cy.locals(model=tysolver.ProblemModel)
    def build_mesh(self, model):
        """Build site mesh into the model

        Retrieve a mesh from business site topography and altimetry and inject it into
        the solver model.
        """
        self.process_altimetry(model, self.site)
        self.process_infrastructure(model, self.site)

    @cy.locals(model=tysolver.ProblemModel)
    def build_sources(self, model):
        return self._build_sources(model, self.site)

    @cy.locals(model=tysolver.ProblemModel, site=tybusiness.Site)
    def _build_sources(self, model, site):
        """Build acoustic sources into the model, given that of the site

        Retrieve the sources from the site infrastructure (TYSourcePonctuelle),
        and make acoustic sources from them (using their position and their
        spectrum).
        Add these acoustic sources to the acoustic problem model.
        """
        infra = cy.declare(cy.pointer(tybusiness.TYInfrastructure))
        infra = site.thisptr.getRealPointer().getInfrastructure().getRealPointer()
        # Retrieve all the infrastructure sources for the current site, each
        # one linked to a list of sub-sources
        infra_sources = cy.declare(map[tybusiness.TYElem_ptr,
                                     vector[SmartPtr[tybusiness.TYGeometryNode]]])
        infra.getAllSrcs(self.comp.thisptr.getRealPointer(), infra_sources)
        # Go through the sources of the current site and build solver sources accordingly
        sources = cy.declare(vector[SmartPtr[tybusiness.TYGeometryNode]])
        sources_of_elt = cy.declare(vector[SmartPtr[tybusiness.TYGeometryNode]])
        its = cy.declare(map[tybusiness.TYElem_ptr, vector[SmartPtr[tybusiness.TYGeometryNode]]].iterator)
        its = infra_sources.begin()
        business_src = cy.declare(tybusiness.TYElem_ptr)
        nb_sources = 0
        # For each business macro source (ex: machine, building...)
        while its != infra_sources.end():
            business_src = deref(its).first
            macro_source_id = id_str(business_src)
            sources_of_elt = deref(its).second
            self.macro2micro_sources[macro_source_id] = []
            nsubsources = sources_of_elt.size()
            # For each of the micro sources making the macro one
            for i in xrange(nsubsources):
                # TYGeometryNode objects contain TYSourcePonctuelle objects as their element
                if sources_of_elt[i].getRealPointer() != NULL:
                    # Get it
                    subsource_elt = cy.declare(cy.pointer(tybusiness.TYElement))
                    subsource_elt = sources_of_elt[i].getRealPointer().getElement()
                    subsource = cy.declare(cy.pointer(tybusiness.TYSourcePonctuelle))
                    subsource = tybusiness.downcast_source_ponctuelle(subsource_elt)
                    ppoint = subsource.getPos().getRealPointer()
                    point3d  = cy.declare(tycommon.OPoint3D)
                    # Site transform matrix * source transform matrix
                    globalmatrix = cy.declare(tycommon.OMatrix)
                    globalmatrix = site.matrix.dot(sources_of_elt[i].getRealPointer().getMatrix())
                    # Convert its position to the global frame
                    point3d = tycommon.dot(globalmatrix, ppoint[0])
                    ppoint[0]._x = point3d._x
                    ppoint[0]._y = point3d._y
                    ppoint[0]._z = point3d._z
                    # Directivity
                    # solver model directivity
                    pdirectivity = cy.declare(cy.pointer(tysolver.SourceDirectivityInterface))
                    # business model directivity
                    pbus_directivity = cy.declare(cy.pointer(tybusiness.TYDirectivity))
                    pbus_directivity = subsource.getDirectivity()
                    # Check if the acoustic source is a user-defined one
                    pusersource = cy.declare(cy.pointer(tybusiness.TYUserSourcePonctuelle))
                    pusersource = tybusiness.downcast_user_source_ponctuelle(subsource_elt)
                    if pusersource != NULL:
                        pdirectivity = new tysolver.SphericalSourceDirectivity()
                    else: #  it is a computed acoustic source
                        pcompdirectivity = cy.declare(cy.pointer(tybusiness.TYComputedDirectivity))
                        pcompdirectivity = tybusiness.downcast_computed_directivity(pbus_directivity)
                        if pcompdirectivity.Type == tybusiness.Surface:
                            pdirectivity = new tysolver.VolumeFaceDirectivity(
                                pcompdirectivity.DirectivityVector, pcompdirectivity.SpecificSize)
                        elif pcompdirectivity.Type == tybusiness.Baffled:
                            pdirectivity = new tysolver.BaffledFaceDirectivity(
                                pcompdirectivity.DirectivityVector, pcompdirectivity.SpecificSize)
                        else: # Chimney
                            pdirectivity = new tysolver.ChimneyFaceDirectivity(
                                pcompdirectivity.DirectivityVector, pcompdirectivity.SpecificSize)
                    # Add it to the solver model
                    source_idx = model.thisptr.get().make_source(ppoint[0], subsource.getSpectre()[0], pdirectivity)
                    # Record where it has been stored
                    self.bus2solv_sources[id_str(subsource_elt)] = source_idx
                    # Copy source mapping to macro2micro_sources
                    self.macro2micro_sources[macro_source_id].append(id_str(subsource))
                    self.instances_mapping[macro_source_id] = business_src
                    nb_sources += 1
            inc(its)
        # Recurse on subsites
        for subsite in site.subsites:
            nb_sources += self._build_sources(model, subsite)
        assert (model.thisptr.get().nsources() == nb_sources,
                (model.thisptr.get().nsources(), nb_sources))
        return nb_sources

    @cy.locals(model=tysolver.ProblemModel)
    def build_receptors(self, model):
        """Insert receptors into the solver model from the business project

        Retrieve the mesh points (TYPointCalcul, TYPointControl) used in the
        current computation (the active ones), build the acoustic receptors
        using their position and add them to the acoustic problem model.
        """
        project = cy.declare(cy.pointer(tybusiness.TYProjet))
        project = self.site.thisptr.getRealPointer().getProjet()
        # First add user-defined receptors to the acoustic problem model
        control_points = cy.declare(vector[SmartPtr[tybusiness.TYPointControl]])
        control_points = project.getPointsControl()
        n_ctrl_pts = control_points.size()
        rec_idx = cy.declare(size_t)
        nb_receptors = 0
        for i in xrange(n_ctrl_pts):
            # if control point state == active (with respect to the current computation)
            if control_points[i].getRealPointer().getEtat(self.comp.thisptr.getRealPointer()):
                # inheritance: TYPointControl > TYPointCalcul > TYPoint > tycommon.OPoint3D > OCoord3D
                # call to tycommon.OPoint3D copy constructor to record control point coordinates
                rec_idx = model.thisptr.get().make_receptor((control_points[i].getRealPointer())[0])
                rec_uuid = id_str(control_points[i].getRealPointer())
                self.bus2solv_receptors[rec_uuid] = rec_idx
                self.instances_mapping[rec_uuid] = control_points[i].getRealPointer()
                nb_receptors += 1
        # Then add mesh points to the acoustic problem model
        meshes = cy.declare(vector[SmartPtr[tybusiness.TYGeometryNode]])
        meshes = self.comp.thisptr.getRealPointer().getMaillages()
        mesh = cy.declare(cy.pointer(tybusiness.TYMaillage))
        mesh_points = cy.declare(vector[SmartPtr[tybusiness.TYPointCalcul]])
        nmeshes = meshes.size()
        for i in xrange(nmeshes):
            matrix = cy.declare(tycommon.OMatrix)
            matrix = meshes[i].getRealPointer().getMatrix()
            mesh = tybusiness.downcast_maillage(meshes[i].getRealPointer().getElement())
            # mesh point must be active
            if mesh.getState() != tybusiness.Actif: # enum value from MaillageState (class TYMaillage)
                continue
            mesh_points = mesh.getPtsCalcul()
            n_mesh_points = mesh_points.size()
            for j in xrange(n_mesh_points):
                # if control point state == active (with respect to the current computation)
                if mesh_points[j].getRealPointer().getEtat(self.comp.thisptr.getRealPointer()):
                    point3d  = cy.declare(tycommon.OPoint3D)
                    # Move receptor to a global scale
                    point3d = tycommon.dot(matrix, mesh_points[j].getRealPointer()[0])
                    mesh_points[j].getRealPointer()._x = point3d._x
                    mesh_points[j].getRealPointer()._y = point3d._y
                    mesh_points[j].getRealPointer()._z = point3d._z
                    rec_idx = model.thisptr.get().make_receptor((mesh_points[j].getRealPointer())[0])
                    rec_uuid = id_str(mesh_points[j].getRealPointer())
                    self.bus2solv_receptors[rec_uuid] = rec_idx
                    self.instances_mapping[rec_uuid] = mesh_points[j].getRealPointer()
                    # We won't keep mesh points in the final result matrix
                    self.to_be_removed_receptors.append(rec_uuid)
                    nb_receptors += 1
        assert (model.thisptr.get().nreceptors() == nb_receptors,
                (model.thisptr.get().nreceptors(), nb_receptors))
        return nb_receptors

    @cy.locals(model=tysolver.ProblemModel, site=tybusiness.Site)
    cdef process_altimetry(self, model, site):
        """Call Tympan methods to make a mesh out of the site altimetry.

        Read and export this mesh to the acoustic problem model
        (see also _process_mesh), converting the data in basic classes
        'understandable' by the solvers (see entities.hpp).
        """
        (points, triangles, grounds) = site.export_topo_mesh()
        (nodes_idx, tgles_idx) = self._process_mesh(model, points, triangles)
        # make material
        actri = cy.declare(cy.pointer(tysolver.AcousticTriangle))
        pmat = cy.declare(shared_ptr[tysolver.AcousticMaterialBase])
        # Set the material of each triangle
        for (i, ground) in enumerate(grounds):
            actri = cy.address(model.thisptr.get().triangle(tgles_idx[i]))
            _ground = cy.declare(tybusiness.Ground)
            _ground = ground
            grnd = cy.declare(SmartPtr[tybusiness.TYSol])
            grnd = _ground.thisptr
            mat_name = cy.declare(string)
            mat_name = grnd.getRealPointer().getName().toStdString()
            mat_res = cy.declare(double)
            mat_res = ground.resistivity
            pmat = model.thisptr.get().make_material(mat_name, mat_res)
            actri.made_of = pmat
        # Recurse on subsites
        for subsite in site.subsites:
            self.process_altimetry(model, subsite)

    @cy.locals(model=tysolver.ProblemModel, site=tybusiness.Site)
    def process_infrastructure(self, model, site):
        """Go through site infrastructure and enrich solver model accordingly

        Set a few 'geometric' entities such as nodes.
        Create geometric entities, fill dedicated container and relate them
        according to the relation definitions.
        """
        for surface in site.acoustic_surfaces:
            (points, triangles) = surface.export_mesh()
            (nodes_idx, tgles_idx) = self._process_mesh(model, points, triangles)
            # Get the building material for the surface
            pmat = cy.declare(shared_ptr[tysolver.AcousticMaterialBase])
            buildmat = cy.declare(tybusiness.Material)
            buildmat = surface.material
            mat_spec = cy.declare(tycommon.Spectrum)
            mat_spec = buildmat.spectrum
            mat_cspec = cy.declare(tycommon.OSpectreComplex)
            mat_cspec = tycommon.OSpectreComplex(mat_spec.thisobj)
            mat_name = cy.declare(string)
            mat_name = buildmat.name
            actri = cy.declare(cy.pointer(tysolver.AcousticTriangle))
            # Set the material of the surface
            for i in xrange(tgles_idx.size):
                pmat = model.thisptr.get().make_material(mat_name, mat_cspec)
                actri = cy.address(model.thisptr.get().triangle(tgles_idx[i]))
                actri.made_of = pmat
        # Recurse on subsites
        for subsite in site.subsites:
            self.process_infrastructure(model, subsite)

    @cy.locals(model=tysolver.ProblemModel)
    def _process_mesh(self, model, points, triangles):
        """Add a mesh to the solver model

        Create nodes and acoustic triangles in the model to represent the
        mesh given in argument.
        The mesh must be given as a list of 'Point3D' python objects ('points')
        and a list of 'Triangle' python objects ('triangles')
        Returns 2 np arrays containing the indices of these nodes and triangles
        in the model once created.
        """
        map_to_model_node_idx = np.empty(len(points))
        for (i, pt) in enumerate(points):
            node = cy.declare(tycommon.OPoint3D)
            node._x = pt.x
            node._y = pt.y
            node._z = pt.z
            map_to_model_node_idx[i] = model.thisptr.get().make_node(node)
        map_to_model_tgle_idx = np.empty(len(triangles))
        for (i, tri) in enumerate(triangles):
            map_to_model_tgle_idx[i] = model.thisptr.get().make_triangle(
                map_to_model_node_idx[tri.p1],
                map_to_model_node_idx[tri.p2],
                map_to_model_node_idx[tri.p3])
        return (map_to_model_node_idx, map_to_model_tgle_idx)

