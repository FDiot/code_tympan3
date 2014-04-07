from libcpp cimport bool
from libcpp.vector cimport vector

class NullCppObject(Exception):
    """
    The SmartPtr object is empty
    """

def loadsolver(foldername, Computation comp):
    load_solver(foldername, comp.thisptr.getRealPointer())

cdef class ProblemModel:
    cdef AcousticProblemModel *thisptr
    def __cinit__(self):
        pass
    def nbpoints(self):
        return self.thisptr.num_points()
    def nbtriangles(self):
        return self.thisptr.num_triangles()
    def nbmaterials(self):
        return self.thisptr.num_materials()
    def export_triangles(self, filename):
        self.thisptr.export_triangles_soup(filename)

cdef class ResultModel:
    cdef AcousticResultModel *thisptr
    def __cinit__(self):
        pass

cdef class SolverModelBuilder:
    cdef SolverDataModelBuilder *thisptr
    def __cinit__(self, ProblemModel model):
        self.thisptr = new SolverDataModelBuilder (model.thisptr[0])
    def fill_problem(self, Site site):
        self.thisptr.walkTroughtSite(site.thisptr)

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
        if load_project(filepath, project.thisptr):
            return project
        else:
            raise Exception("Project could not be loaded from %s" % filepath)
    def to_xml(self, filepath):
        if self.thisptr.getRealPointer() != NULL:
            save_project(filepath, self.thisptr)
