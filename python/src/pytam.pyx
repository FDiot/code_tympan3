from libcpp cimport bool
from libcpp.vector cimport vector
from libcpp.deque cimport deque
from cython.operator cimport dereference as deref, preincrement as inc
import cython
import numpy as np
from cython.view cimport array as cyarray


class NullCppObject(Exception):
    """
    The referenced C++ object is NULL
    """

@cython.locals(comp=Computation)
def loadsolver(foldername, comp):
    load_solver(foldername, comp.thisptr.getRealPointer())


cdef class ProblemModel:
    thisptr = cython.declare(cython.pointer(AcousticProblemModel))

    def __cinit__(self):
        self.thisptr = NULL

    def npoints(self):
        if self.thisptr == NULL:
            raise NullCppObject()
        return self.thisptr.npoints()

    def ntriangles(self):
        if self.thisptr == NULL:
            raise NullCppObject()
        return self.thisptr.ntriangles()

    def nmaterials(self):
        if self.thisptr == NULL:
            raise NullCppObject()
        return self.thisptr.nmaterials()

    @property
    def nsources(self):
        if self.thisptr == NULL:
            raise NullCppObject()
        return self.thisptr.nsources()

    def export_triangular_mesh(self):
        if self.thisptr == NULL:
            raise NullCppObject()
        nb_elts = cython.declare(cython.uint)
        actri = cython.declare(cython.pointer(AcousticTriangle))
        nb_elts = self.thisptr.ntriangles()
        triangles = np.empty([nb_elts, 3])
        for i in range(nb_elts):
            actri = cython.address(self.thisptr.triangle(i))
            triangles[i] = [actri.n[0], actri.n[1], actri.n[2]]
        point = cython.declare(cython.pointer(OPoint3D))
        nb_elts = self.thisptr.npoints()
        nodes = np.empty([nb_elts, 3])
        for i in range(nb_elts):
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
        self.process_altimetry(site)
        self.process_infrastructure(site)
        self.build_sources(site, comp)

    @cython.locals(site=Site, comp=Computation)
    def build_sources(self, site, comp):
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
                sources.push_back(sources_of_elt[i])
            inc(its)
        nsources = sources.size()
        pelt = cython.declare(cython.pointer(TYElement))
        psource = cython.declare(cython.pointer(TYSourcePonctuelle))
        ppoint = cython.declare(cython.pointer(TYPoint))
        for i in xrange(nsources):
            pelt = sources[i].getRealPointer().getElement()
            psource = downcast_source_ponctuelle(pelt)
            ppoint = psource.getPos().getRealPointer()
            self.model.make_source(ppoint[0], psource.getSpectre()[0])

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
        for i in range(nb_building_faces):
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
            pmat = self.model.make_material( pbuildmat.getName().toStdString(),
                                            spectre)
            actri = cython.declare(cython.pointer(AcousticTriangle))
            # Set the UUID of the site element and the material of the surface
            for i in range(triangles.size()):
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
            assert (deref(itt)._A == points[deref(itt)._p1])
            assert (deref(itt)._B == points[deref(itt)._p2])
            assert (deref(itt)._C == points[deref(itt)._p3])
            # Add the triangle
            self.model.make_triangle(map_to_model_node_idx[deref(itt)._p1],
                                     map_to_model_node_idx[deref(itt)._p2],
                                     map_to_model_node_idx[deref(itt)._p3])
            inc(itt)

    @cython.locals(site=Site)
    def process_altimetry(self, site):
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
        for i in range(triangles.size()):
            actri = cython.address(self.model.triangle(i))
            psol = materials[i].getRealPointer()
            pmat = self.model.make_material(psol.getName().toStdString(),
                                            psol.getResistivite())
            actri.made_of = pmat


cdef class ElementArray:
    thisptr = cython.declare(vector[SmartPtr[TYElement]])
    def __cinit__(self):
        pass


cdef class Element:
    thisptr = cython.declare(SmartPtr[TYElement])

    def __cinit__(self):
        self.thisptr = SmartPtr[TYElement]()

    def name(self):
        if self.thisptr.getRealPointer() == NULL:
            raise NullCppObject()
        return self.thisptr.getRealPointer().getName().toStdString()


cdef class Site:
    thisptr = cython.declare(SmartPtr[TYSiteNode])

    def __cinit__(self):
        self.thisptr = SmartPtr[TYSiteNode]()

    def childs(self):
        if self.thisptr.getRealPointer() == NULL:
            raise NullCppObject()
        childs = cython.declare(vector[SmartPtr[TYElement]])
        # Retrieves the childs of the site (LPTYElements) into the "childs" vector.
        self.thisptr.getRealPointer().getChilds(childs, False)
        # Constructs a python list and appends to it the LPTYElement objects
        # previously wrapped into Elements objects
        pylist = []
        for i in range(childs.size()):
            child = Element()
            child.thisptr = childs[i]
            pylist.append(child)
        return pylist


cdef class Result:
    thisptr = cython.declare(SmartPtr[TYResultat])

    def __cinit__(self):
        self.thisptr = SmartPtr[TYResultat]()

    @property
    def nsources(self):
        """ Returns the number of acoustic sources
        """
        if self.thisptr.getRealPointer() == NULL:
            raise NullCppObject()
        return self.thisptr.getRealPointer().getNbOfSources()

    @property
    def nreceivers(self):
        """ Returns the number of acoustic receivers
        """
        if self.thisptr.getRealPointer() == NULL:
            raise NullCppObject()
        return self.thisptr.getRealPointer().getNbOfRecepteurs()

    def spectrum(self, receiver, source):
        """ Returns the computed acoustic spectrum
        """
        if self.thisptr.getRealPointer() == NULL:
            raise NullCppObject()
        spec = Spectrum()
        spec.thisobj = self.thisptr.getRealPointer().getSpectre(receiver, source)
        return spec


cdef class Spectrum:
    thisobj = cython.declare(OSpectre)

    def __cinit__(self):
        pass

    @property
    def nvalues(self):
        """ Returns the number of values contained in the spectrum
        """
        return self.thisobj.getNbValues()

    @property
    def values(self):
        """ Returns the values of the spectrum
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


cdef class Computation:
    thisptr = cython.declare(SmartPtr[TYCalcul])

    def __cinit__(self):
        self.thisptr = SmartPtr[TYCalcul]()

    @property
    def result(self):
        """ Returns an acoustic result (business representation)
        """
        if self.thisptr.getRealPointer() == NULL:
            raise NullCppObject()
        res = Result()
        res.thisptr = self.thisptr.getRealPointer().getResultat()
        return res

    def go(self):
        """ Solves the current acoustic problem. A solver must be loaded.
        """
        if self.thisptr.getRealPointer() == NULL:
            raise NullCppObject()
        return self.thisptr.getRealPointer().go()

    def acoustic_problem(self):
        """ Returns an acoustic problem model (geometric representation as
        used by the solvers)
        """
        if self.thisptr.getRealPointer() == NULL:
            raise NullCppObject()
        problem = ProblemModel()
        problem.thisptr = self.thisptr.getRealPointer()._acousticProblem.get()
        return problem

    def acoustic_result(self):
        """ Returns an acoustic result model (geometric representation as used
        by the solvers)
        """
        if self.thisptr.getRealPointer() == NULL:
            raise NullCppObject()
        result = ResultModel()
        result.thisptr = self.thisptr.getRealPointer()._acousticResult.get()
        return result


cdef class Project:
    thisptr = cython.declare(SmartPtr[TYProjet])

    def __cinit__(self):
        self.thisptr = SmartPtr[TYProjet]()

    def current_computation(self):
        if self.thisptr.getRealPointer() == NULL:
            raise NullCppObject()
        comp = Computation()
        comp.thisptr = self.thisptr.getRealPointer().getCurrentCalcul()
        return comp

    def site(self):
        if self.thisptr.getRealPointer() == NULL:
            raise NullCppObject()
        site = Site()
        site.thisptr = self.thisptr.getRealPointer().getSite()
        return site

    @staticmethod
    def from_xml(filepath):
        project = Project()
        # if an exception is raised from the C++ code, it will be converted to
        # RuntimeError python exception. what() message should be preserved.
        # see http://docs.cython.org/src/userguide/wrapping_CPlusPlus.html#exceptions
        project.thisptr = load_project(filepath)
        return project

    def to_xml(self, filepath):
        if self.thisptr.getRealPointer() == NULL:
            raise ValueError ("Cannot export an empty project")
        # same thing as for load_project about the exception
        save_project(filepath, self.thisptr)
