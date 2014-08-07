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
bus2solv_sources = cy.declare(map[cy.pointer(tybusiness.TYSourcePonctuelle), deque[size_t]])

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

    def build_solver_problem(self):
        builder = SolverModelBuilder(self.solver_problem)
        builder.fill_problem(self.site, self.comp)

    def postprocessing(self):
        self.comp.thisptr.getRealPointer().goPostprocessing()



cdef class SolverModelBuilder:
    model = cy.declare(cy.pointer(tysolver.AcousticProblemModel))

    @cy.locals(model=tysolver.ProblemModel)
    def __cinit__(self, model):
        self.model = model.thisptr

    @cy.locals(site=tybusiness.Site, comp=tybusiness.Computation)
    def fill_problem(self, site, comp):
        """ Fill the acoustic problem model: build an acoustic mesh, sources
            and receptors.
        """
        self.process_altimetry(site)
        self.process_infrastructure(site)
        self.build_sources(site, comp)
        self.build_receptors(site, comp)

    @cy.locals(site=tybusiness.Site, comp=tybusiness.Computation)
    def build_sources(self, site, comp):
        """ Retrieve the sources from the site infrastructure (TYSourcePonctuelle),
            and make acoustic sources from them (using their position and their
            spectrum).
            Add these acoustic sources to the acoustic problem model.
        """
        emitter2sources = cy.declare(map[tybusiness.TYElem_ptr,
                                         vector[SmartPtr[tybusiness.TYGeometryNode]]])
        infra = cy.declare(cy.pointer(tybusiness.TYInfrastructure))
        infra = site.thisptr.getRealPointer().getInfrastructure().getRealPointer()
        infra.getAllSrcs(comp.thisptr.getRealPointer(), emitter2sources)
        sources = cy.declare(vector[SmartPtr[tybusiness.TYGeometryNode]])
        sources_of_elt = cy.declare(vector[SmartPtr[tybusiness.TYGeometryNode]])
        its = cy.declare(map[tybusiness.TYElem_ptr, vector[SmartPtr[tybusiness.TYGeometryNode]]].iterator)
        its = emitter2sources.begin()
        while its != emitter2sources.end():
            sources_of_elt = deref(its).second
            nbusiness_sources = sources_of_elt.size()
            for i in xrange(nbusiness_sources):
                if sources_of_elt[i].getRealPointer() != NULL:
                    sources.push_back(sources_of_elt[i])
            inc(its)
        nsources = sources.size()
        pelt = cy.declare(cy.pointer(tybusiness.TYElement))
        psource = cy.declare(cy.pointer(tybusiness.TYSourcePonctuelle))
        ppoint = cy.declare(cy.pointer(tybusiness.TYPoint))
        source_idx = cy.declare(size_t)
        # TYGeometryNode objects contain TYSourcePonctuelle objects as their element
        for i in xrange(nsources):
            pelt = sources[i].getRealPointer().getElement()
            psource = tybusiness.downcast_source_ponctuelle(pelt)
            ppoint = psource.getPos().getRealPointer()
            point3d  = cy.declare(tycommon.OPoint3D)
            point3d = tycommon.dot(sources[i].getRealPointer().getMatrix(), ppoint[0])
            ppoint[0]._x = point3d._x
            ppoint[0]._y = point3d._y
            ppoint[0]._z = point3d._z
            source_idx = self.model.make_source(ppoint[0], psource.getSpectre()[0])
            bus2solv_sources[psource].push_back(source_idx)

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
        for i in xrange(n_ctrl_pts):
            # if control point state == active (with respect to the current computation)
            if control_points[i].getRealPointer().getEtat(comp.thisptr.getRealPointer()):
                # inheritance: TYPointControl > TYPointCalcul > TYPoint > tycommon.OPoint3D > OCoord3D
                # call to tycommon.OPoint3D copy constructor to record control point coordinates
                rec_idx = self.model.make_receptor((control_points[i].getRealPointer())[0])
                bus2solv_receptors[control_points[i].getRealPointer()] = rec_idx
                solv2bus_receptors[rec_idx] = control_points[i].getRealPointer()
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
