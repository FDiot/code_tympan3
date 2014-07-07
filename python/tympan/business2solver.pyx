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


cdef business2microsource(map[tybusiness.TYElem_ptr, vector[SmartPtr[tybusiness.TYGeometryNode]]] map_sources):
    """ factory function: return a Business2MicroSource (python object) from a
        map (TYElement*, SmartPtr[TYGeometryNode]) (cpp lib)
    """
    b2ms = Business2MicroSource()
    b2ms.map_sources = map_sources
    return b2ms

def loadsolver(foldername, tybusiness.Computation comp):
    """ Load a solver plugin (looked for in the 'foldername' folder) that will
        be used to compute 'comp'
    """
    load_solver(foldername, comp.thisptr.getRealPointer())


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
        infra = cy.declare(cy.pointer(tybusiness.TYInfrastructure))
        infra = site.thisptr.getRealPointer().getInfrastructure().getRealPointer()
        map_elt_srcs = cy.declare(map[tybusiness.TYElem_ptr, vector[SmartPtr[tybusiness.TYGeometryNode]]])
        infra.getAllSrcs(comp.thisptr.getRealPointer(), map_elt_srcs)
        sources = cy.declare(vector[SmartPtr[tybusiness.TYGeometryNode]])
        sources_of_elt = cy.declare(vector[SmartPtr[tybusiness.TYGeometryNode]])
        its = cy.declare(map[tybusiness.TYElem_ptr, vector[SmartPtr[tybusiness.TYGeometryNode]]].iterator)
        its = map_elt_srcs.begin()
        while its != map_elt_srcs.end():
            sources_of_elt = deref(its).second
            nsources = sources_of_elt.size()
            for i in xrange(nsources):
                if sources_of_elt[i].getRealPointer() != NULL:
                    sources.push_back(sources_of_elt[i])
            inc(its)
        nsources = sources.size()
        pelt = cy.declare(cy.pointer(tybusiness.TYElement))
        psource = cy.declare(cy.pointer(tybusiness.TYSourcePonctuelle))
        ppoint = cy.declare(cy.pointer(tybusiness.TYPoint))
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
            self.model.make_source(ppoint[0], psource.getSpectre()[0])

    @cy.locals(site=tybusiness.Site, comp=tybusiness.Computation)
    def build_receptors(self, site, comp):
        """ Retrieve the mesh points (TYPointCalcul, TYPointControl) used in the
            current computation (the active ones), build the acoustic receptors
            using their position and add them to the acoustic problem model.
        """
        project = cy.declare(cy.pointer(tybusiness.TYProjet))
        project = site.thisptr.getRealPointer().getProjet()
        # First add isolated receptors to the acoustic problem model
        control_points = cy.declare(vector[SmartPtr[tybusiness.TYPointControl]])
        control_points = project.getPointsControl()
        n_ctrl_pts = control_points.size()
        for i in xrange(n_ctrl_pts):
            # if control point state == active (with respect to the current computation)
            if control_points[i].getRealPointer().getEtat(comp.thisptr.getRealPointer()):
                # inheritance: TYPointControl > TYPointCalcul > TYPoint > tycommon.OPoint3D > OCoord3D
                # call to tycommon.OPoint3D copy constructor to record control point coordinates
                self.model.make_receptor((control_points[i].getRealPointer())[0])
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
                    self.model.make_receptor((mesh_points[j].getRealPointer())[0])

    @cy.locals(site=tybusiness.Site)
    def process_infrastructure(self, site):
        """ Set a few 'geometric' entities such as nodes
            Create geometric entities, fill dedicated container and relate them
            according to the relation definitions.
        """
        is_screen_face_idx = cy.declare(vector[bool])
        face_list = cy.declare(vector[SmartPtr[tybusiness.TYGeometryNode]])
        pelt = cy.declare(cy.pointer(tybusiness.TYElement))
        nb_building_faces = cy.declare(cy.uint)
        nb_building_faces = 0
        psurf = cy.declare(cy.pointer(tybusiness.TYAcousticSurface))
        site.thisptr.getRealPointer().getListFaces(face_list, nb_building_faces,
                           is_screen_face_idx)
        points = cy.declare(deque[tycommon.OPoint3D])
        triangles = cy.declare(deque[tycommon.OTriangle])
        for i in xrange(nb_building_faces):
            pelt = face_list[i].getRealPointer().getElement()
            psurf = tybusiness.downcast_acoustic_surface(pelt)
            # 'face_list' can contain topography elements. Not relevant here.
            if psurf == NULL:
                continue
            # Use the triangulating interface of TYSurfaceInterface to get triangles
            # and convert them to Nodes and AcousticTriangles (beware of mapping
            # TYPoints to Node in the correct way.)
            # !! Here we must not give exportMesh the geonode face_list[i] but
            # build a new one to avoid constructing the triangles in a local
            # scale
            psurf.exportMesh(points, triangles, tybusiness.TYGeometryNode(psurf))
            (nodes_idx, tgles_idx) = self.process_mesh(points, triangles)
            # Get the building material for the surface
            pbuildmat = cy.declare(cy.pointer(tybusiness.TYMateriauConstruction))
            pbuildmat = psurf.getMateriau().getRealPointer()
            spectre = cy.declare(tybusiness.TYSpectre)
            spectre = pbuildmat.getSpectreAbso()
            pmat = cy.declare(shared_ptr[tysolver.AcousticMaterialBase])
            pmat = self.model.make_material(pbuildmat.getName().toStdString(),
                                            spectre)
            actri = cy.declare(cy.pointer(tysolver.AcousticTriangle))
            # Set the UUID of the site element and the material of the surface
            for i in xrange(tgles_idx.size):
                actri = cy.address(self.model.triangle(tgles_idx[i]))
                actri.made_of = pmat
            points.clear()
            triangles.clear()

    @cy.cfunc
    @cy.locals(points=deque[tycommon.OPoint3D], triangles=deque[tycommon.OTriangle])
    def process_mesh(self, points, triangles):
        """ Create nodes and acoustic triangles in the model to represent the
        mesh given in argument.
        The mesh must be given as a list of 'Point3D' python objects ('points')
        and a list of 'Triangle' python objects ('triangles')
        Returns 2 np arrays containing the indices of these nodes and triangles
        in the model once created.
        """
        map_to_model_node_idx = np.empty(points.size())
        itp = cy.declare(deque[tycommon.OPoint3D].iterator)
        itp = points.begin()
        i = 0
        # Create all nodes related to the triangles
        while itp != points.end():
            # Add the points
            map_to_model_node_idx[i] = self.model.make_node(deref(itp))
            i = i+1
            inc(itp)
        map_to_model_tgle_idx = np.empty(triangles.size())
        itt = cy.declare(deque[tycommon.OTriangle].iterator)
        itt = triangles.begin()
        i = 0
        while  itt != triangles.end():
            # Assert consistency of the tycommon.OPoint3D given in the mesh
            assert deref(itt).checkConsistencyWrtPointsTab(points), deref(itt).reportInconsistencyWrtPointsTab(points)
            # Add the triangle
            map_to_model_tgle_idx[i] = self.model.make_triangle(
                map_to_model_node_idx[deref(itt)._p1],
                map_to_model_node_idx[deref(itt)._p2],
                map_to_model_node_idx[deref(itt)._p3])
            i = i+1
            inc(itt)
        return (map_to_model_node_idx, map_to_model_tgle_idx)

    @cy.locals(site=tybusiness.Site)
    def process_altimetry(self, site):
        """ Call Tympan methods to make a mesh (points, triangles, materials)
            out of the site altimetry. Read and export this mesh to the
            acoustic problem model (see also process_mesh), converting the data
            in basic classes 'understandable' by the solvers (see entities.hpp).
        """
        points = cy.declare(deque[tycommon.OPoint3D])
        triangles = cy.declare(deque[tycommon.OTriangle])
        materials = cy.declare(deque[SmartPtr[tybusiness.TYSol]])
        ptopo = cy.declare(cy.pointer(tybusiness.TYTopographie))
        ptopo = site.thisptr.getRealPointer().getTopographie().getRealPointer()
        ptopo.exportMesh(points, triangles, cy.address(materials))
        (nodes_idx, tgles_idx) = self.process_mesh(points, triangles)
        # make material
        actri = cy.declare(cy.pointer(tysolver.AcousticTriangle))
        psol = cy.declare(cy.pointer(tybusiness.TYSol))
        pmat = cy.declare(shared_ptr[tysolver.AcousticMaterialBase])
        # Set the material of each triangle
        for i in xrange(tgles_idx.size):
            actri = cy.address(self.model.triangle(tgles_idx[i]))
            psol = materials[i].getRealPointer()
            pmat = self.model.make_material(psol.getName().toStdString(),
                                            psol.getResistivite())
            actri.made_of = pmat



