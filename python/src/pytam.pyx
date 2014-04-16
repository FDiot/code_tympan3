from libcpp cimport bool
from libcpp.vector cimport vector
cimport pytam

def loadsolver(foldername, Computation comp):
    load_solver(foldername, comp.thisptr.getRealPointer())

cdef class ProblemModel:
    cdef AcousticProblemModel thisptr
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
    cdef AcousticResultModel thisptr
    def __cinit__(self):
        pass

cdef class SolverModelBuilder:
    cdef SolverDataModelBuilder *thisptr
    def __cinit__(self, ProblemModel model):
        self.thisptr = new SolverDataModelBuilder (model.thisptr)
    def fill_problem(self, Site site):
        self.thisptr.walkTroughtSite(site.thisptr)

cdef class ElementArray:
    cdef vector[SmartPtr[TYElement]] thisptr
    def __cinit__(self):
        pass

cdef class Element:
    cdef SmartPtr[TYElement] thisptr
    def __cinit__(self):
        self.thisptr = SmartPtr[TYElement](new TYElement())
    def name(self):
        return self.thisptr.getRealPointer().getName().toStdString()

cdef class Site:
    cdef SmartPtr[TYSiteNode] thisptr
    def __cinit__(self):
        self.thisptr = SmartPtr[TYSiteNode](new TYSiteNode())
    def childs(self):
        # Retrieves the childs of the site (LPTYElements) into the "childs" vector.
        cdef vector[SmartPtr[TYElement]] childs
        self.thisptr.getRealPointer().getChilds(childs, False)
        # Constructs a python list and appends to it the LPTYElement objects
        # previously wrapped into Elements objects
        pylist = []
        cdef Element child
        for i in range(childs.size()):
            child = Element.__new__(Element)
            child.thisptr = childs[i]
            pylist.append(child)
        return pylist

cdef class Computation:
    cdef SmartPtr[TYCalcul] thisptr
    def __cinit__(self):
        self.thisptr = SmartPtr[TYCalcul](new TYCalcul())
    def go(self):
        return self.thisptr.getRealPointer().go()
    def problem(self):
        cdef ProblemModel problem = ProblemModel.__new__(ProblemModel)
        problem.thisptr = self.thisptr.getRealPointer()._acousticProblem
        return problem
    def result(self):
        cdef ResultModel result = ResultModel.__new__(ResultModel)
        result.thisptr = self.thisptr.getRealPointer()._acousticResult
        return result

cdef class Project:
    cdef SmartPtr[TYProjet] thisptr
    def __cinit__(self):
        self.thisptr = SmartPtr[TYProjet](new TYProjet())
    # Constructs a Computation python object and sets the wrapped instance
    # with the Computation retrieved from the C++ LPTYProject.
    def current_computation(self):
        cdef Computation comp = Computation.__new__(Computation)
        comp.thisptr = self.thisptr.getRealPointer().getCurrentCalcul()
        return comp
    def site(self):
        cdef Site site =  Site.__new__(Site)
        site.thisptr = self.thisptr.getRealPointer().getSite()
        return site
    @staticmethod
    def from_xml(filepath):
        project = Project()
        load_project_from_file(filepath, project.thisptr)
        return project
    def to_xml(self, filepath):
        save_project(filepath, self.thisptr)
    @classmethod
    def safeDownCast(self, Element el):
        project = Project()
        pproj = project.thisptr.getRealPointer()
        pproj = safeDownCast(el.thisptr.getRealPointer())
        return project
