from libcpp cimport bool
from libcpp.vector cimport vector
from libcpp.deque cimport deque
from cython.operator cimport dereference as deref, preincrement as inc
import cython
import numpy as np
from cython.view cimport array as cyarray

@cython.locals(comp=Computation)
def loadsolver(foldername, comp):
    """ Load a solver plugin (looked for in the 'foldername' folder) that will
        be used to compute 'comp'
    """
    load_solver(foldername, comp.thisptr.getRealPointer())

def init_tympan_registry():
    """ Trigger the registration of Tympan business classes (TY* classes).
        It is necessary to do it before playing with Tympan library (just after
        pytam import)
    """
    init_registry()

cdef class ProblemModel:
    thisptr = cython.declare(cython.pointer(AcousticProblemModel))

    def __cinit__(self):
        self.thisptr = NULL

    def npoints(self):
        """ Return the number of mesh nodes contained in the acoustic problem
            model
        """
        assert(self.thisptr != NULL)
        return self.thisptr.npoints()

    def ntriangles(self):
        """ Return the number of mesh triangles contained in the acoustic problem
            model
        """
        assert(self.thisptr != NULL)
        return self.thisptr.ntriangles()

    def nmaterials(self):
        """ Return the number of acoustic materials contained in the acoustic
            problem model
        """
        assert(self.thisptr != NULL)
        return self.thisptr.nmaterials()

    @property
    def nsources(self):
        """ Return the number of acoustic sources involved in the acoustic
            problem model
        """
        assert(self.thisptr != NULL)
        return self.thisptr.nsources()

    def source(self, idx):
        """ Return the acoustic source (Source object) of index 'idx'
        """
        assert(self.thisptr != NULL)
        source = Source()
        source.thisptr = cython.address(self.thisptr.source(idx))
        return source

    def export_triangular_mesh(self):
        """ Build a triangular mesh from the acoustic problem model.
            Return two nparrays:
                * 'nodes': an array of nodes (of dimension 'npoints'X3), where
                each line stands for a node and contains 3 coordinates)
                * 'triangles': an array of triangles (of dimension 'ntriangles'X3),
                where each line stands for a triangle and contains the indices of
                its 3 vertices in the 'nodes' array.
        """
        assert(self.thisptr != NULL)
        nb_elts = cython.declare(cython.uint)
        actri = cython.declare(cython.pointer(AcousticTriangle))
        nb_elts = self.thisptr.ntriangles()
        triangles = np.empty([nb_elts, 3])
        for i in xrange(nb_elts):
            actri = cython.address(self.thisptr.triangle(i))
            triangles[i] = [actri.n[0], actri.n[1], actri.n[2]]
        point = cython.declare(cython.pointer(OPoint3D))
        nb_elts = self.thisptr.npoints()
        nodes = np.empty([nb_elts, 3])
        for i in xrange(nb_elts):
            point = cython.address(self.thisptr.node(i))
            nodes[i] = [point._x, point._y, point._z]
        return (nodes, triangles)


cdef class ResultModel:
    thisptr = cython.declare(cython.pointer(AcousticResultModel))
    def __cinit__(self):
        self.thisptr = NULL


cdef class SolverModelBuilder:
    model = cython.declare(cython.pointer(AcousticProblemModel))

    @cython.locals(model=ProblemModel)
    def __cinit__(self, model):
        self.model = model.thisptr

    @cython.locals(site=Site, comp=Computation)
    def fill_problem(self, site, comp):
        """ Fill the acoustic problem model: build an acoustic mesh, sources
            and receptors.
        """
        self.process_altimetry(site)
        self.process_infrastructure(site)
        self.build_sources(site, comp)
        self.build_receptors(site, comp)

    @cython.locals(site=Site, comp=Computation)
    def build_sources(self, site, comp):
        """ Retrieve the sources from the site infrastructure (TYSourcePonctuelle),
            and make acoustic sources from them (using their position and their
            spectrum).
            Add these acoustic sources to the acoustic problem model.
        """
        infra = cython.declare(cython.pointer(TYInfrastructure))
        infra = site.thisptr.getRealPointer().getInfrastructure().getRealPointer()
        map_elt_srcs = cython.declare(map[TYElem_ptr, vector[SmartPtr[TYGeometryNode]]])
        infra.getAllSrcs(comp.thisptr.getRealPointer(), map_elt_srcs)
        sources = cython.declare(vector[SmartPtr[TYGeometryNode]])
        sources_of_elt = cython.declare(vector[SmartPtr[TYGeometryNode]])
        its = cython.declare(map[TYElem_ptr, vector[SmartPtr[TYGeometryNode]]].iterator)
        its = map_elt_srcs.begin()
        while its != map_elt_srcs.end():
            sources_of_elt = deref(its).second
            nsources = sources_of_elt.size()
            for i in xrange(nsources):
                if sources_of_elt[i].getRealPointer() != NULL:
                    sources.push_back(sources_of_elt[i])
            inc(its)
        nsources = sources.size()
        pelt = cython.declare(cython.pointer(TYElement))
        psource = cython.declare(cython.pointer(TYSourcePonctuelle))
        ppoint = cython.declare(cython.pointer(TYPoint))
        # TYGeometryNode objects contain TYSourcePonctuelle objects as their element
        for i in xrange(nsources):
            pelt = sources[i].getRealPointer().getElement()
            psource = downcast_source_ponctuelle(pelt)
            ppoint = psource.getPos().getRealPointer()
            self.model.make_source(ppoint[0], psource.getSpectre()[0])

    @cython.locals(site=Site, comp=Computation)
    def build_receptors(self, site, comp):
        """ Retrieve the mesh points (TYPointCalcul, TYPointControl) used in the
            current computation (the active ones), build the acoustic receptors
            using their position and add them to the acoustic problem model.
        """
        project = cython.declare(cython.pointer(TYProjet))
        project = site.thisptr.getRealPointer().getProjet()
        # First add isolated receptors to the acoustic problem model
        control_points = cython.declare(vector[SmartPtr[TYPointControl]])
        control_points = project.getPointsControl()
        n_ctrl_pts = control_points.size()
        for i in xrange(n_ctrl_pts):
            # if control point state == active (with respect to the current computation)
            if control_points[i].getRealPointer().getEtat(comp.thisptr.getRealPointer()):
                # inheritance: TYPointControl > TYPointCalcul > TYPoint > OPoint3D > OCoord3D
                # call to OPoint3D copy constructor to record control point coordinates
                self.model.make_receptor((control_points[i].getRealPointer())[0])
        # Then add mesh points to the acoustic problem model
        meshes = cython.declare(vector[SmartPtr[TYGeometryNode]])
        meshes = comp.thisptr.getRealPointer().getMaillages()
        mesh = cython.declare(cython.pointer(TYMaillage))
        mesh_points = cython.declare(vector[SmartPtr[TYPointCalcul]])
        nmeshes = meshes.size()
        for i in xrange(nmeshes):
            matrix = cython.declare(OMatrix)
            matrix = meshes[i].getRealPointer().getMatrix()
            mesh = downcast_maillage(meshes[i].getRealPointer().getElement())
            # mesh point must be active
            if mesh.getState() != Actif: # enum value from MaillageState (class TYMaillage)
                continue
            mesh_points = mesh.getPtsCalcul()
            n_mesh_points = mesh_points.size()
            for j in xrange(n_mesh_points):
                # if control point state == active (with respect to the current computation)
                if mesh_points[i].getRealPointer().getEtat(comp.thisptr.getRealPointer()):
                    # XXX here we should take into account the global position
                    # should we add the matrix to the AcousticReceptor class ?
                    # Will be clarified when writing the tests
                    self.model.make_receptor((mesh_points[i].getRealPointer())[0])

    @cython.locals(site=Site)
    def process_infrastructure(self, site):
        """ Set a few 'geometric' entities such as nodes
            Create geometric entities, fill dedicated container and relate them
            according to the relation definitions.
        """
        is_screen_face_idx = cython.declare(vector[bool])
        face_list = cython.declare(vector[SmartPtr[TYGeometryNode]])
        pelt = cython.declare(cython.pointer(TYElement))
        nb_building_faces = cython.declare(cython.uint)
        nb_building_faces = 0
        psurf = cython.declare(cython.pointer(TYAcousticSurface))
        site.thisptr.getRealPointer().getListFaces(face_list, nb_building_faces,
                           is_screen_face_idx)
        points = cython.declare(deque[OPoint3D])
        triangles = cython.declare(deque[OTriangle])
        for i in xrange(nb_building_faces):
            pelt = face_list[i].getRealPointer().getElement()
            psurf = downcast_acoustic_surface(pelt)
            # 'face_list' can contain topography elements. Not relevant here.
            if psurf == NULL:
                continue
            # Use the triangulating interface of TYSurfaceInterface to get triangles
            # and convert them to Nodes and AcousticTriangles (beware of mapping
            # TYPoints to Node in the correct way.)
            # !! Here we must not give exportMesh the geonode face_list[i] but
            # build a new one to avoid constructing the triangles in a local
            # scale
            psurf.exportMesh(points, triangles, TYGeometryNode(psurf))
            self.process_mesh(points, triangles)
            # Get the building material for the surface
            pbuildmat = cython.declare(cython.pointer(TYMateriauConstruction))
            pbuildmat = psurf.getMateriau().getRealPointer()
            spectre = cython.declare(TYSpectre)
            spectre = pbuildmat.getSpectreAbso()
            pmat = cython.declare(shared_ptr[AcousticMaterialBase])
            pmat = self.model.make_material(pbuildmat.getName().toStdString(),
                                            spectre)
            actri = cython.declare(cython.pointer(AcousticTriangle))
            # Set the UUID of the site element and the material of the surface
            for i in xrange(triangles.size()):
                actri = cython.address(self.model.triangle(i))
                actri.made_of = pmat
            points.clear()
            triangles.clear()

    @cython.cfunc
    @cython.locals(points=deque[OPoint3D], triangles=deque[OTriangle])
    def process_mesh(self, points, triangles):
        """ Create nodes and acoustic triangles in the model to represent the
            mesh given in argument.
        """
        map_to_model_node_idx = np.empty(points.size())
        itp = cython.declare(deque[OPoint3D].iterator)
        itp = points.begin()
        i = 0
        # Create all nodes related to the triangles
        while itp != points.end():
            # Add the points
            map_to_model_node_idx[i] = self.model.make_node(deref(itp))
            i = i+1
            inc(itp)
        itt = cython.declare(deque[OTriangle].iterator)
        itt = triangles.begin()
        while  itt != triangles.end():
            # Assert consistency of the OPoint3D given in the mesh
            assert deref(itt).checkConsistencyWrtPointsTab(points), deref(itt).reportInconsistencyWrtPointsTab(points)
            # Add the triangle
            self.model.make_triangle(map_to_model_node_idx[deref(itt)._p1],
                                     map_to_model_node_idx[deref(itt)._p2],
                                     map_to_model_node_idx[deref(itt)._p3])
            inc(itt)

    @cython.locals(site=Site)
    def process_altimetry(self, site):
        """ Call Tympan methods to make a mesh (points, triangles, materials)
            out of the site altimetry. Read and export this mesh to the
            acoustic problem model (see also process_mesh), converting the data
            in basic classes 'understandable' by the solvers (see entities.hpp).
        """
        points = cython.declare(deque[OPoint3D])
        triangles = cython.declare(deque[OTriangle])
        materials = cython.declare(deque[SmartPtr[TYSol]])
        ptopo = cython.declare(cython.pointer(TYTopographie))
        ptopo = site.thisptr.getRealPointer().getTopographie().getRealPointer()
        ptopo.exportMesh(points, triangles, cython.address(materials))
        self.process_mesh(points, triangles)
        # make material
        actri = cython.declare(cython.pointer(AcousticTriangle))
        psol = cython.declare(cython.pointer(TYSol))
        pmat = cython.declare(shared_ptr[AcousticMaterialBase])
        # Set the material of each triangle
        for i in xrange(triangles.size()):
            actri = cython.address(self.model.triangle(i))
            psol = materials[i].getRealPointer()
            pmat = self.model.make_material(psol.getName().toStdString(),
                                            psol.getResistivite())
            actri.made_of = pmat


cdef class Element:
    thisptr = cython.declare(SmartPtr[TYElement])

    def __cinit__(self):
        self.thisptr = SmartPtr[TYElement]()

    def name(self):
        """ Return the name of the element
        """
        assert(self.thisptr.getRealPointer() != NULL)
        return self.thisptr.getRealPointer().getName().toStdString()


cdef class Site:
    thisptr = cython.declare(SmartPtr[TYSiteNode])

    def __cinit__(self):
        self.thisptr = SmartPtr[TYSiteNode]()

    def childs(self):
        """ Return the direct childs of the Site (ie the elements it contains)
            as a python list. Not recursive.
        """
        assert(self.thisptr.getRealPointer() != NULL)
        childs = cython.declare(vector[SmartPtr[TYElement]])
        # Retrieves the childs of the site (LPTYElements) into the "childs" vector.
        self.thisptr.getRealPointer().getChilds(childs, False)
        # Constructs a python list and appends to it the LPTYElement objects
        # previously wrapped into Elements objects
        pylist = []
        for i in xrange(childs.size()):
            child = Element()
            child.thisptr = childs[i]
            pylist.append(child)
        return pylist

    def update(self):
        """ Updates a site:
            - "Sort" terrains contained in a topography
            - Updates the altimetry of the infrastructure elements
            - Updates the acoustic of the infrastructure elements
        """
        # This method actually leads to the dynamic allocation of an array of
        # TYGeometryNode that will be accessed by some other method(s) later on.
        # Not calling this method leads to a segmentation fault since the array
        # doesn't exist then.
        self.thisptr.getRealPointer().getTopographie().getRealPointer().sortTerrainsBySurface()
        self.thisptr.getRealPointer().updateAltiInfra(True)
        self.thisptr.getRealPointer().updateAcoustique(True)

cdef class Result:
    thisptr = cython.declare(SmartPtr[TYResultat])

    def __cinit__(self):
        self.thisptr = SmartPtr[TYResultat]()

    @property
    def nsources(self):
        """ Returns the number of acoustic sources
        """
        assert(self.thisptr.getRealPointer() != NULL)
        return self.thisptr.getRealPointer().getNbOfSources()

    @property
    def nreceptors(self):
        """ Returns the number of acoustic receptors
        """
        assert(self.thisptr.getRealPointer() != NULL)
        return self.thisptr.getRealPointer().getNbOfRecepteurs()

    def receptor(self, index):
        """ Return the receptor of index 'index'
        """
        assert(self.thisptr.getRealPointer() != NULL)
        receptor = Receptor()
        receptor.thisptr = self.thisptr.getRealPointer().getRecepteur(index)
        return receptor

    def spectrum(self, receptor, source):
        """ Return the computed acoustic spectrum
        """
        assert(self.thisptr.getRealPointer() != NULL)
        spec = Spectrum()
        spec.thisobj = self.thisptr.getRealPointer().getSpectre(receptor, source)
        return spec


cdef class Receptor:
    thisptr = cython.declare(SmartPtr[TYPointCalcul])

    def __cinit__(self):
        pass

    def is_control_point(self):
        """ Return true if the receptor is a control point (that is, a kind of
            "smart" receptor), false otherwise
        """
        assert(self.thisptr.getRealPointer() != NULL)
        control_point = cython.declare(cython.pointer(TYPointControl))
        control_point = downcast_point_control(self.thisptr.getRealPointer())
        return (control_point != NULL)

cdef class Spectrum:
    thisobj = cython.declare(OSpectre)

    def __cinit__(self):
        pass

    @property
    def nvalues(self):
        """ Return the number of values contained in the spectrum
        """
        return self.thisobj.getNbValues()

    @property
    def values(self):
        """ Return the values of the spectrum
        """
        cdef cyarray cy_array = <double[:self.nvalues]> self.thisobj.getTabValReel()
        spec_val = np.array(cy_array, dtype=np.double)
        return spec_val

    def to_dB(self):
        """ Convert the spectrum to a dB scale
        """
        spectrum = Spectrum()
        spectrum.thisobj = self.thisobj.toDB()
        return spectrum

cdef class Point3D:
    thisobj = cython.declare(OPoint3D)

    def __init__(self):
        pass

    @property
    def x(self):
        return self.thisobj._x

    @property
    def y(self):
        return self.thisobj._y

    @property
    def z(self):
        return self.thisobj._z

cdef class Source:
    thisptr = cython.declare(cython.pointer(AcousticSource))

    def __cinit__(self):
        pass

    @property
    def position(self):
        """ Return the acoustic source position (as a 'Point3D' object)
        """
        assert(self.thisptr != NULL)
        point = Point3D()
        point.thisobj = self.thisptr.position
        return point

    @property
    def spectrum(self):
        """ Return the acoustic spectrum of the Source (dB scale, power spectrum)
        """
        assert(self.thisptr != NULL)
        spectrum = Spectrum()
        spectrum.thisobj = self.thisptr.spectrum
        return spectrum


cdef class Computation:
    thisptr = cython.declare(SmartPtr[TYCalcul])

    def __cinit__(self):
        self.thisptr = SmartPtr[TYCalcul]()

    @property
    def result(self):
        """ Return an acoustic result (business representation)
        """
        assert(self.thisptr.getRealPointer() != NULL)
        res = Result()
        res.thisptr = self.thisptr.getRealPointer().getResultat()
        return res

    def go(self):
        """ Solve the current acoustic problem. A solver must be loaded.
        """
        assert(self.thisptr.getRealPointer() != NULL)
        return self.thisptr.getRealPointer().go()

    def set_nthread(self, nthread):
        """ Set the number of threads used by the default solver to compute the
            acoustic problem
        """
        assert(self.thisptr.getRealPointer() != NULL)
        self.thisptr.getRealPointer().setNbThread(nthread)

    @property
    def acoustic_problem(self):
        """ Return an acoustic problem model (geometric representation as
            used by the solvers)
        """
        assert(self.thisptr.getRealPointer() != NULL)
        problem = ProblemModel()
        problem.thisptr = self.thisptr.getRealPointer()._acousticProblem.get()
        return problem

    @property
    def acoustic_result(self):
        """ Returns an acoustic result model (geometric representation as used
        by the solvers)
        """
        assert(self.thisptr.getRealPointer() != NULL)
        result = ResultModel()
        result.thisptr = self.thisptr.getRealPointer()._acousticResult.get()
        return result


cdef class Project:
    thisptr = cython.declare(SmartPtr[TYProjet])

    def __cinit__(self):
        self.thisptr = SmartPtr[TYProjet]()

    def update_site(self):
        """ Update the project site (altimetry, atmosphere, inactive mesh points
            detection).
        """
        self.site.update()
        computation = cython.declare(cython.pointer(TYCalcul))
        computation = self.thisptr.getRealPointer().getCurrentCalcul().getRealPointer()
        atmosphere = cython.declare(SmartPtr[TYAtmosphere])
        atmosphere = computation.getAtmosphere()
        self.thisptr.getRealPointer().getSite().getRealPointer().setAtmosphere(atmosphere)
        # detect and deactivate the mesh points that are inside machines or buildings
        computation.selectActivePoint(self.thisptr.getRealPointer().getSite())

    def update_altimetry_on_receptors(self):
        """ Call Tympan method to update the acoustic receptors
            (TYPointControl objects) with regard to the current altimetry
        """
        site = cython.declare(cython.pointer(TYSiteNode))
        site = self.thisptr.getRealPointer().getSite().getRealPointer()
        alti = cython.declare(cython.pointer(TYAltimetrie))
        # Retrieve current altimetry from the site topography
        alti = site.getTopographie().getRealPointer().getAltimetrie().getRealPointer()
        self.thisptr.getRealPointer().updateAltiRecepteurs(alti)

    @property
    def current_computation(self):
        """ Return the project current computation
        """
        assert(self.thisptr.getRealPointer() != NULL)
        comp = Computation()
        comp.thisptr = self.thisptr.getRealPointer().getCurrentCalcul()
        return comp

    @property
    def site(self):
        """ Return the site considered in the project
        """
        assert(self.thisptr.getRealPointer() != NULL)
        site = Site()
        site.thisptr = self.thisptr.getRealPointer().getSite()
        return site

    @staticmethod
    def from_xml(filepath):
        """ Build a project (TYProject) from a xml file
        """
        project = Project()
        # if an exception is raised from the C++ code, it will be converted to
        # RuntimeError python exception. what() message should be preserved.
        # see http://docs.cython.org/src/userguide/wrapping_CPlusPlus.html#exceptions
        project.thisptr = load_project(filepath)
        return project

    def to_xml(self, filepath):
        """ Export an acoustic project to a XML file
        """
        assert(self.thisptr.getRealPointer() != NULL)
        # same thing as for load_project about the exception
        save_project(filepath, self.thisptr)
