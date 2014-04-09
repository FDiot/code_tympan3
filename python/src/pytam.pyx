from libcpp cimport bool
from libcpp.vector cimport vector
from libcpp.deque cimport deque
from cython.operator cimport dereference as deref, preincrement as inc

class NullCppObject(Exception):
    """
    The referenced C++ object is NULL
    """

def loadsolver(foldername, Computation comp):
    load_solver(foldername, comp.thisptr.getRealPointer())

cdef class ProblemModel:
    cdef AcousticProblemModel *thisptr
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
    cdef AcousticResultModel *thisptr
    def __cinit__(self):
        pass

cdef class SolverModelBuilder:
    cdef SolverDataModelBuilder *thisptr
    cdef AcousticProblemModel *model
    def __cinit__(self):
        self.model = new AcousticProblemModel()
        self.thisptr = new SolverDataModelBuilder (self.model[0])
    def fill_problem(self, Site site):
        self.process_infrastructure(site)
        self.process_altimetry(site)
    def problem_model(self):
        problem = ProblemModel()
        problem.thisptr = self.model
        return problem
    def process_infrastructure(self, Site site):
        cdef vector[bool] is_screen_face_idx
        cdef vector[SmartPtr[TYGeometryNode]] face_list
        cdef TYElement *pelt
        cdef TYGeometryNode geonode
        cdef unsigned int nb_building_faces = 0
        site.thisptr.getRealPointer().getListFaces(face_list, nb_building_faces,
                           is_screen_face_idx)
        cdef TYAcousticSurface *pSurf = NULL
        for i in range(nb_building_faces):
            pelt = face_list[i].getRealPointer().getElement()
            pSurf = safeDownCast(pelt)
            if pSurf != NULL:
                geonode = (face_list[i].getRealPointer())[0]
                self.thisptr.setAcousticTriangle(geonode)
    def process_altimetry(self, Site site):
        cdef deque[OPoint3D] points
        cdef deque[OTriangle] triangles
        cdef deque[SmartPtr[TYSol]] materials
        cdef TYTopographie *ptopo = site.thisptr.getRealPointer().getTopographie().getRealPointer()
        cdef UuidAdapter *element_uid = new UuidAdapter(ptopo.getID())
        ptopo.exportMesh(points, triangles, &materials)
        # process mesh
        cdef size_t nbpoints = points.size()
        cdef vector[size_t] map_to_model_node_idx = vector[size_t] (nbpoints)
        cdef deque[OPoint3D].iterator itp = points.begin()
        cdef deque[OTriangle].iterator itt = triangles.begin()
        cdef unsigned int i = 0
        while itp != points.end():
            # Add the points
            map_to_model_node_idx[i] = self.model.make_node(deref(itp))
            i = i+1
            inc(itp)
        while  itt != triangles.end():
            # Assert consistency of the OPoint3D given in the mesh
            assert (deref(itt)._A == points[deref(itt)._p1])
            assert (deref(itt)._B == points[deref(itt)._p2])
            assert (deref(itt)._C == points[deref(itt)._p3])
            # Add the deref(itt)angle
            self.model.make_triangle(map_to_model_node_idx[deref(itt)._p1],
                                     map_to_model_node_idx[deref(itt)._p2],
                                     map_to_model_node_idx[deref(itt)._p3])
            inc(itt)
        # make material
        cdef AcousticTriangle *actri = NULL
        cdef TYSol *psol
        cdef shared_ptr[AcousticMaterialBase] pmat
        for i in range(triangles.size()):
            # XXX find a way to use triangle() instead of ptriangle()
            actri = self.model.ptriangle(i)
            actri.uuid = element_uid[0].getUuid()
            psol = materials[i].getRealPointer()
            pmat = self.model.make_material(psol.getName().toStdString(),
                                            psol.getResistivite())
            actri.made_of = pmat

cdef class ElementArray:
    cdef vector[SmartPtr[TYElement]] thisptr
    def __cinit__(self):
        pass

cdef class Element:
    cdef SmartPtr[TYElement] thisptr
    def __cinit__(self):
        self.thisptr = SmartPtr[TYElement]()
    def name(self):
        if self.thisptr.getRealPointer() == NULL:
            raise NullCppObject()
        return self.thisptr.getRealPointer().getName().toStdString()

cdef class Site:
    cdef SmartPtr[TYSiteNode] thisptr
    def __cinit__(self):
        self.thisptr = SmartPtr[TYSiteNode]()
    def childs(self):
        if self.thisptr.getRealPointer() == NULL:
            raise NullCppObject()
        cdef vector[SmartPtr[TYElement]] childs
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
    cdef SmartPtr[TYCalcul] thisptr
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
        problem.thisptr = &self.thisptr.getRealPointer()._acousticProblem
        return problem
    def result(self):
        if self.thisptr.getRealPointer() == NULL:
            raise NullCppObject()
        result = ResultModel()
        result.thisptr = &self.thisptr.getRealPointer()._acousticResult
        return result

cdef class Project:
    cdef SmartPtr[TYProjet] thisptr
    def __cinit__(self):
        self.thisptr = SmartPtr[TYProjet]()
    # Constructs a Computation python object and sets the wrapped instance
    # with the Computation retrieved from the C++ LPTYProject.
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
