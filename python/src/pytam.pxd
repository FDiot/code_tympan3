from libcpp cimport bool
from libcpp.string cimport string
from libcpp.vector cimport vector

# SmartPtr wrapping
cdef extern from "Tympan/Tools/OSmartPtr.h":
    cdef cppclass SmartPtr[T]:
        SmartPtr()
        SmartPtr(T *)
        T *getRealPointer()

cdef extern from "Tympan/Tools/OPrototype.h":
    cdef cppclass OPrototype:
        const char* getClassName() const
        OPrototype* safeDownCast(OPrototype* pObject)

# Additional C++ methods allowing to load a project and a solver
cdef extern from "Loader.hpp" namespace "tympan":
   void load_project_from_file(const char *filename, SmartPtr[TYProjet] &)
   void load_solver(const char *foldername, TYCalcul *calcul)
   void save_project(const char *filename, const SmartPtr[TYProjet] &)

cdef extern from "Qt/qstring.h":
    cdef cppclass QString:
        string toStdString()

cdef extern from "Tympan/MetierSolver/DataManagerCore/TYXMLManager.h":
    cdef cppclass TYXMLManager:
        TYXMLManager()
        int createDoc(QString docName, QString version)
        int load(const char *fileName, vector[SmartPtr[TYElement]] &eltCollection)
        int save(QString fileName)
        int addElement(TYElement *pElt)

cdef extern from "Tympan/MetierSolver/SolverDataModel/acoustic_problem_model.hpp" namespace "tympan":
    cdef cppclass AcousticProblemModel:
        size_t num_points()
        size_t num_triangles()
        size_t num_materials()
        void export_triangles_soup(string &filename)

cdef extern from "Tympan/MetierSolver/SolverDataModel/acoustic_result_model.hpp" namespace "tympan":
    cdef cppclass AcousticResultModel:
        pass

cdef extern from "Tympan/MetierSolver/DataExchange/SolverDataModelBuilder.hpp" namespace "tympan":
    cdef cppclass SolverDataModelBuilder:
        SolverDataModelBuilder(AcousticProblemModel &model)
        void walkTroughtSite(SmartPtr[TYSiteNode] site)

cdef extern from "Tympan/MetierSolver/DataManagerCore/TYElement.h":
    cdef cppclass TYElement:
        QString getName()
        const char* getClassName() const

cdef extern from "Tympan/MetierSolver/DataManagerMetier/Site/TYSiteNode.h":
    cdef cppclass TYSiteNode:
        void getChilds (vector[SmartPtr[TYElement]] &elts, bool recursive)
        void update(const bool& force)

cdef extern from "Tympan/MetierSolver/DataManagerMetier/Commun/TYCalcul.h":
    cdef cppclass TYCalcul:
        bool go()
        AcousticProblemModel _acousticProblem
        AcousticResultModel _acousticResult

cdef extern from "Tympan/MetierSolver/DataManagerMetier/Commun/TYProjet.h":
    cdef cppclass TYProjet:
        SmartPtr[TYCalcul] getCurrentCalcul()
        SmartPtr[TYSiteNode] getSite()

cdef extern from "Tympan/MetierSolver/DataManagerMetier/Commun/TYProjet.h" namespace "TYProjet":
    TYProjet * safeDownCast(TYElement *)
