from libcpp cimport bool
from libcpp.vector cimport vector
from libcpp.deque cimport deque
from cython.operator cimport dereference as deref, preincrement as inc
import cython

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

    def nbpoints(self):
        if self.thisptr == NULL:
            raise NullCppObject()
        return self.thisptr.num_points()

    def nbtriangles(self):
        if self.thisptr == NULL:
            raise NullCppObject()
        return self.thisptr.num_triangles()

    def nbmaterials(self):
        if self.thisptr == NULL:
            raise NullCppObject()
        return self.thisptr.num_materials()

    def export_triangles(self, filename):
        if self.thisptr == NULL:
            raise NullCppObject()
        self.thisptr.export_triangles_soup(filename)


cdef class ResultModel:
    thisptr = cython.declare(cython.pointer(AcousticResultModel))
    def __cinit__(self):
        self.thisptr = NULL


cdef class SolverModelBuilder:
    thisptr = cython.declare(cython.pointer(SolverDataModelBuilder))
    model = cython.declare(cython.pointer(AcousticProblemModel))

    @cython.locals(model=ProblemModel)
    def __cinit__(self, model):
        self.model = model.thisptr
        self.thisptr = new SolverDataModelBuilder (self.model[0])

    @cython.locals(site=Site)
    def fill_problem(self, site):
        self.process_altimetry(site)
        self.process_infrastructure(site)

    def problem_model(self):
        problem = ProblemModel()
        problem.thisptr = self.model
        return problem

    @cython.locals(site=Site)
    def process_infrastructure(self, site):
        is_screen_face_idx = cython.declare(vector[bool])
        face_list = cython.declare(vector[SmartPtr[TYGeometryNode]])
        pelt = cython.declare(cython.pointer(TYElement))
        geonode = cython.declare(TYGeometryNode)
        nb_building_faces = cython.declare(cython.uint)
        nb_building_faces = 0
        psurf = cython.declare(cython.pointer(TYAcousticSurface))
        site.thisptr.getRealPointer().getListFaces(face_list, nb_building_faces,
                           is_screen_face_idx)
        element_uid = cython.declare(cython.pointer(UuidAdapter))
        points = cython.declare(deque[OPoint3D])
        triangles = cython.declare(deque[OTriangle])
        for i in range(nb_building_faces):
            pelt = face_list[i].getRealPointer().getElement()
            psurf = safeDownCast(pelt)
            # 'face_list' can contain topography elements. Not relevant here.
            if psurf == NULL:
                continue
            geonode = (face_list[i].getRealPointer())[0]
            # Get the uid for the site element bearing the current acoustic surface
            element_uid = new UuidAdapter(psurf.getParent().getID())
            # Use the triangulating interface of TYSurfaceInterface to get triangles
            # and convert them to Nodes and AcousticTriangles (beware of mapping
            # TYPoints to Node in the correct way.)
            psurf.exportMesh(points, triangles, geonode)
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
                actri = self.model.ptriangle(i)
                actri.uuid = (element_uid)[0].getUuid()
                actri.made_of = pmat
            points.clear()
            triangles.clear()

    @cython.cfunc
    @cython.locals(points=deque[OPoint3D], triangles=deque[OTriangle])
    def process_mesh(self, points, triangles):
        map_to_model_node_idx = cython.declare(vector[size_t])
        map_to_model_node_idx = vector[size_t] (points.size())
        itp = cython.declare(deque[OPoint3D].iterator)
        itp = points.begin()
        i = 0
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
        element_uid = cython.declare(cython.pointer(UuidAdapter))
        element_uid = new UuidAdapter(ptopo.getID())
        ptopo.exportMesh(points, triangles, cython.address(materials))
        self.process_mesh(points, triangles)
        # make material
        actri = cython.declare(cython.pointer(AcousticTriangle))
        psol = cython.declare(cython.pointer(TYSol))
        pmat = cython.declare(shared_ptr[AcousticMaterialBase])
        for i in range(triangles.size()):
            # XXX find a way to use triangle() instead of ptriangle()
            actri = self.model.ptriangle(i)
            actri.uuid = element_uid[0].getUuid()
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


cdef class Computation:
    thisptr = cython.declare(SmartPtr[TYCalcul])

    def __cinit__(self):
        self.thisptr = SmartPtr[TYCalcul]()

    def go(self):
        if self.thisptr.getRealPointer() == NULL:
            raise NullCppObject()
        return self.thisptr.getRealPointer().go()

    def problem(self):
        if self.thisptr.getRealPointer() == NULL:
            raise NullCppObject()
        problem = ProblemModel()
        problem.thisptr = cython.address(self.thisptr.getRealPointer()._acousticProblem)
        return problem

    def result(self):
        if self.thisptr.getRealPointer() == NULL:
            raise NullCppObject()
        result = ResultModel()
        result.thisptr = cython.address(self.thisptr.getRealPointer()._acousticResult)
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
        # TODO see how to convert potential C++ exception into python ones
        project.thisptr = load_project(filepath)
        if  project.thisptr.getRealPointer() != NULL:
            return project
        else:
            raise Exception("Project could not be loaded from %s" % filepath)

    def to_xml(self, filepath):
        if self.thisptr.getRealPointer() != NULL:
            save_project(filepath, self.thisptr)
