import numpy as np
import cython as cy
from cython.operator cimport dereference as deref, preincrement as inc
from libcpp cimport bool
from libcpp.map cimport map
from libcpp.deque cimport deque
from libcpp.vector cimport vector
from libcpp.string cimport string

from tympan.core cimport shared_ptr
from tympan.models cimport business as tybusiness
from tympan.models cimport solver as tysolver
from tympan.models cimport common as tycommon


# business / solver mappings
# business receptors (control and mesh points) to solver receptors indices
bus2solv_receptors = cy.declare(map[cy.pointer(tybusiness.TYPointCalcul), size_t])
# solver receptors to business receptors
solv2bus_receptors = cy.declare(map[size_t, cy.pointer(tybusiness.TYPointCalcul)])
# business source to micro sources (both business model)
macro2micro_sources = cy.declare(map[tybusiness.TYElem_ptr,
                                     vector[SmartPtr[tybusiness.TYGeometryNode]]])
# business micro sources (TYSourcePonctuelle contained in TYGeometryNode) to solver source indices
bus2solv_sources = cy.declare(map[SmartPtr[tybusiness.TYGeometryNode], size_t])
# Receptors that are mesh points and therefore will be removed from the final
# result matrix after the solver computation:
to_be_removed_receptors = cy.declare(deque[size_t])


cdef business2microsource(map[tybusiness.TYElem_ptr, vector[SmartPtr[tybusiness.TYGeometryNode]]] map_sources):
    """ factory function: return a Business2MicroSource (python object) from a
        map (TYElement*, SmartPtr[TYGeometryNode]) (cpp lib)
    """
    b2ms = Business2MicroSource()
    b2ms.map_sources = map_sources
    return b2ms

def load_computation_solver(foldername, tybusiness.Computation comp):
    """ Load a solver plugin (looked for in the 'foldername' folder) that will
        be used to compute 'comp'
    """
    res = cy.declare(tycommon.SolverInterface)
    res = tycommon.SolverInterface()
    res.thisptr = load_solver(foldername, comp.thisptr.getRealPointer().getSolverId());
    return res

cdef class Business2SolverConverter:
    # Business model
    comp = cy.declare(tybusiness.Computation)
    site = cy.declare(tybusiness.Site)
    _nsources = cy.declare(int)
    _nreceptors = cy.declare(int)
    # transitional result matrix (from solver matrix to condensed business matrix)
    transitional_result_matrix = cy.declare(cy.pointer(tysolver.SpectrumMatrix))

    @cy.locals(comp=tybusiness.Computation, site=tybusiness.Site)
    def __cinit__(self, comp, site):
        self.comp = comp
        self.site = site

    @property
    def solver_problem(self):
        return self.comp.acoustic_problem

    @property
    def solver_result(self):
        return self.comp.acoustic_result

    @property
    def nsources(self):
        return self._nsources

    @property
    def nreceptors(self):
        return self._nreceptors

    def build_solver_problem(self):
        builder = SolverModelBuilder(self.solver_problem)
        builder.fill_problem(self.site, self.comp)
        self._nsources = builder.nsources
        self._nreceptors = builder.nreceptors

    def postprocessing(self):
        # Retrieve solver result matrix
        solver_result = cy.declare(cy.pointer(tysolver.AcousticResultModel))
        solver_result = self.comp.thisptr.getRealPointer()._acousticResult.get()
        solver_result_matrix = cy.declare(tysolver.SpectrumMatrix)
        solver_result_matrix = solver_result.get_data()
        self.transitional_result_matrix = new tysolver.SpectrumMatrix(solver_result_matrix)
        # update business receptors cumulative spectra
        self.update_business_receptors()
        self.remove_mesh_points_from_results()
        # condensate result matrix
        self.update_business_result_matrix()
        self.comp.thisptr.getRealPointer().goPostprocessing()
        # Clear intermediate data
        del self.transitional_result_matrix
        self.clear()

    def clear(self):
        bus2solv_receptors.clear()
        solv2bus_receptors.clear()
        macro2micro_sources.clear()
        bus2solv_sources.clear()
        to_be_removed_receptors.clear()

    def update_business_receptors(self):
        """ Once the acoustic problem has been solved, send back the acoustic results
        to the business receptors
        """
        # resize business result matrix with the number of enabled sources and receptors:
        business_result = cy.declare(tybusiness.Result)
        business_result = self.comp.result
        business_result_matrix = cy.declare(cy.pointer(tysolver.SpectrumMatrix))
        business_result_matrix = cy.address(business_result.thisptr.getRealPointer().getResultMatrix())
        business_result_matrix.resize(self.solver_problem.nreceptors, self.solver_problem.nsources)
        it = cy.declare(map[cy.pointer(tybusiness.TYPointCalcul), size_t].iterator)
        it = bus2solv_receptors.begin()
        while it != bus2solv_receptors.end():
            receptor = cy.declare(cy.pointer(tybusiness.TYPointCalcul))
            receptor = deref(it).first
            # retrieve receptor spectra
            spectra = cy.declare(vector[tycommon.OSpectre])
            spectra = self.transitional_result_matrix[0].by_receptor(deref(it).second)
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
            inc(it)
        busresult = cy.declare(cy.pointer(tybusiness.TYResultat))
        busresult = self.comp.thisptr.getRealPointer().getResultat().getRealPointer()
        busresult.setIsAcousticModified(False)

    def update_business_result_matrix(self):
        """ Condensate result matrix (originally contains 1 spectrum per pair
        (business receptor, micro source), will now contain 1 cumulative spectrum per
        pair (business receptor, business source)
        """
        busresult = cy.declare(cy.pointer(tybusiness.TYResultat))
        busresult = self.comp.thisptr.getRealPointer().getResultat().getRealPointer()
        result_sources = cy.declare(map[tybusiness.TYElem_ptr, int])
        condensate_matrix = cy.declare(tysolver.SpectrumMatrix)
        condensate_matrix.resize(bus2solv_receptors.size(), macro2micro_sources.size())
        rec_it = cy.declare(map[cy.pointer(tybusiness.TYPointCalcul), size_t].iterator)
        rec_it = bus2solv_receptors.begin()
        rec_counter = 0
        # Go through all the business receptors
        while rec_it != bus2solv_receptors.end():
            receptor = cy.declare(cy.pointer(tybusiness.TYPointCalcul))
            receptor = deref(rec_it).first
            busresult.addRecepteur(receptor)
            source_it = cy.declare(map[tybusiness.TYElem_ptr,
                                       vector[SmartPtr[tybusiness.TYGeometryNode]]].iterator)
            source_it = macro2micro_sources.begin()
            source_counter = 0
            # Go through all the business infrastructure sources
            while source_it != macro2micro_sources.end():
                valid_spectrum = True
                cumul_spectrum = cy.declare(tycommon.OSpectre)
                cumul_spectrum.setDefaultValue(0.0)
                subsources = cy.declare(vector[SmartPtr[tybusiness.TYGeometryNode]])
                subsources = deref(source_it).second
                # Go through all their business subsources
                for i in xrange(subsources.size()):
                    # Get solver result for this subsource
                    subsource_idx = bus2solv_sources[subsources[i]] # solver idx
                    cur_spectrum = cy.declare(tycommon.OSpectre)
                    cur_spectrum = self.transitional_result_matrix[0].element(
                        deref(rec_it).second, subsource_idx)
                    cumul_spectrum = cumul_spectrum.sum(cur_spectrum)
                    valid_spectrum &= cur_spectrum.isValid()
                cumul_spectrum.setValid(valid_spectrum)
                cumul_spectrum.setType(tycommon.SPECTRE_TYPE_LP)
                # The values retrieved from the solver are linear. We want to convert
                # them to DB.
                cumul_spectrum.setEtat(tycommon.SPECTRE_ETAT_LIN)
                cumul_spectrum = cumul_spectrum.toDB()
                condensate_matrix.setSpectre(rec_counter, source_counter, cumul_spectrum)
                result_sources[deref(source_it).first] = source_counter
                source_counter += 1
                inc(source_it)
            rec_counter += 1
            inc(rec_it)
        busresult.setResultMatrix(condensate_matrix)
        busresult.setSources(result_sources)

    def remove_mesh_points_from_results(self):
        """ In the final result matrix we don't keep mesh points (they were generated
        for the solver resolution but once computation is done we settle for a synthetic
        result
        """
        for i in xrange(to_be_removed_receptors.size()):
            self.transitional_result_matrix[0].clearReceptor(to_be_removed_receptors[i])
            remove_me = cy.declare(cy.pointer(tybusiness.TYPointCalcul))
            remove_me = solv2bus_receptors[to_be_removed_receptors[i]]
            bus2solv_receptors.erase(remove_me)
            solv2bus_receptors.erase(to_be_removed_receptors[i])


cdef class SolverModelBuilder:
    model = cy.declare(cy.pointer(tysolver.AcousticProblemModel))
    _nsources = cy.declare(int)
    _nreceptors = cy.declare(int)

    @cy.locals(model=tysolver.ProblemModel)
    def __cinit__(self, model):
        self.model = model.thisptr

    @property
    def nsources(self):
        return self._nsources

    @property
    def nreceptors(self):
        return self._nreceptors

    @cy.locals(site=tybusiness.Site, comp=tybusiness.Computation)
    def fill_problem(self, site, comp):
        """ Fill the acoustic problem model: build an acoustic mesh, sources
            and receptors.
        """
        self.process_altimetry(site)
        self.process_infrastructure(site)
        self._nsources = self.build_sources(site, comp)
        self._nreceptors = self.build_receptors(site, comp)

    @cy.locals(site=tybusiness.Site, comp=tybusiness.Computation)
    def build_sources(self, site, comp):
        """ Retrieve the sources from the site infrastructure (TYSourcePonctuelle),
            and make acoustic sources from them (using their position and their
            spectrum).
            Add these acoustic sources to the acoustic problem model.
        """
        infra = cy.declare(cy.pointer(tybusiness.TYInfrastructure))
        infra = site.thisptr.getRealPointer().getInfrastructure().getRealPointer()
        # Retrieve all the infrastructure sources, each one linked to a list of
        # sub-sources
        infra.getAllSrcs(comp.thisptr.getRealPointer(), macro2micro_sources)
        sources = cy.declare(vector[SmartPtr[tybusiness.TYGeometryNode]])
        sources_of_elt = cy.declare(vector[SmartPtr[tybusiness.TYGeometryNode]])
        its = cy.declare(map[tybusiness.TYElem_ptr, vector[SmartPtr[tybusiness.TYGeometryNode]]].iterator)
        its = macro2micro_sources.begin()
        nb_sources = 0
        # For each business macro source (ex: machine, building...)
        while its != macro2micro_sources.end():
            sources_of_elt = deref(its).second
            macro_source = cy.declare(cy.pointer(tybusiness.TYElement))
            macro_source = deref(its).first
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
                    # Convert its position to the global frame
                    point3d = tycommon.dot(sources_of_elt[i].getRealPointer().getMatrix(),
                                           ppoint[0])
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
                    source_idx = self.model.make_source(ppoint[0], subsource.getSpectre()[0], pdirectivity)
                    # Record where it has been stored
                    bus2solv_sources[sources_of_elt[i]] = source_idx
                    nb_sources += 1
            inc(its)
        return nb_sources

    @cy.locals(site=tybusiness.Site, comp=tybusiness.Computation)
    def build_receptors(self, site, comp):
        """ Retrieve the mesh points (TYPointCalcul, TYPointControl) used in the
            current computation (the active ones), build the acoustic receptors
            using their position and add them to the acoustic problem model.
        """
        project = cy.declare(cy.pointer(tybusiness.TYProjet))
        project = site.thisptr.getRealPointer().getProjet()
        # First add user-defined receptors to the acoustic problem model
        control_points = cy.declare(vector[SmartPtr[tybusiness.TYPointControl]])
        control_points = project.getPointsControl()
        n_ctrl_pts = control_points.size()
        rec_idx = cy.declare(size_t)
        nb_receptors = 0
        for i in xrange(n_ctrl_pts):
            # if control point state == active (with respect to the current computation)
            if control_points[i].getRealPointer().getEtat(comp.thisptr.getRealPointer()):
                # inheritance: TYPointControl > TYPointCalcul > TYPoint > tycommon.OPoint3D > OCoord3D
                # call to tycommon.OPoint3D copy constructor to record control point coordinates
                rec_idx = self.model.make_receptor((control_points[i].getRealPointer())[0])
                bus2solv_receptors[control_points[i].getRealPointer()] = rec_idx
                solv2bus_receptors[rec_idx] = control_points[i].getRealPointer()
                nb_receptors += 1
        # Then add mesh points to the acoustic problem model
        meshes = cy.declare(vector[SmartPtr[tybusiness.TYGeometryNode]])
        meshes = comp.thisptr.getRealPointer().getMaillages()
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
                if mesh_points[j].getRealPointer().getEtat(comp.thisptr.getRealPointer()):
                    point3d  = cy.declare(tycommon.OPoint3D)
                    # Move receptor to a global scale
                    point3d = tycommon.dot(matrix, mesh_points[j].getRealPointer()[0])
                    mesh_points[j].getRealPointer()._x = point3d._x
                    mesh_points[j].getRealPointer()._y = point3d._y
                    mesh_points[j].getRealPointer()._z = point3d._z
                    rec_idx = self.model.make_receptor((mesh_points[j].getRealPointer())[0])
                    bus2solv_receptors[mesh_points[j].getRealPointer()] = rec_idx
                    solv2bus_receptors[rec_idx] = mesh_points[j].getRealPointer()
                    # We won't keep mesh points in the final result matrix
                    to_be_removed_receptors.push_back(rec_idx)
                    nb_receptors += 1
        return nb_receptors


    @cy.locals(site=tybusiness.Site)
    def process_infrastructure(self, site):
        """ Set a few 'geometric' entities such as nodes
            Create geometric entities, fill dedicated container and relate them
            according to the relation definitions.
        """
        for surface in site.acoustic_surfaces:
            (points, triangles) = surface.export_mesh()
            (nodes_idx, tgles_idx) = self.process_mesh(points, triangles)
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
                pmat = self.model.make_material(mat_name, mat_cspec)
                actri = cy.address(self.model.triangle(tgles_idx[i]))
                actri.made_of = pmat

    def process_mesh(self, points, triangles):
        """ Create nodes and acoustic triangles in the model to represent the
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
            map_to_model_node_idx[i] = self.model.make_node(node)
        map_to_model_tgle_idx = np.empty(len(triangles))
        for (i, tri) in enumerate(triangles):
            map_to_model_tgle_idx[i] = self.model.make_triangle(
                map_to_model_node_idx[tri.p1],
                map_to_model_node_idx[tri.p2],
                map_to_model_node_idx[tri.p3])
        return (map_to_model_node_idx, map_to_model_tgle_idx)


    @cy.locals(site=tybusiness.Site)
    def process_altimetry(self, site):
        """ Call Tympan methods to make a mesh (points, triangles, materials)
            out of the site altimetry. Read and export this mesh to the
            acoustic problem model (see also process_mesh), converting the data
            in basic classes 'understandable' by the solvers (see entities.hpp).
        """
        (points, triangles, grounds) = site.export_topo_mesh()
        (nodes_idx, tgles_idx) = self.process_mesh(points, triangles)
        # make material
        actri = cy.declare(cy.pointer(tysolver.AcousticTriangle))
        pmat = cy.declare(shared_ptr[tysolver.AcousticMaterialBase])
        # Set the material of each triangle
        for (i, ground) in enumerate(grounds):
            actri = cy.address(self.model.triangle(tgles_idx[i]))
            _ground = cy.declare(tybusiness.Ground)
            _ground = ground
            grnd = cy.declare(SmartPtr[tybusiness.TYSol])
            grnd = _ground.thisptr
            mat_name = cy.declare(string)
            mat_name = grnd.getRealPointer().getName().toStdString()
            mat_res = cy.declare(double)
            mat_res = ground.resistivity
            pmat = self.model.make_material(mat_name, mat_res)
            actri.made_of = pmat
