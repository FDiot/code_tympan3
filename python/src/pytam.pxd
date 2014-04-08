from libcpp cimport bool
from libcpp.string cimport string
from libcpp.vector cimport vector

# SmartPtr wrapping
cdef extern from "Tympan/Tools/OSmartPtr.h":
    cdef cppclass SmartPtr[T]:
        SmartPtr()
        SmartPtr(T *)
        T *getRealPointer()

cdef extern from "Tympan/MetierSolver/DataManagerMetier/xml_project_util.hpp" namespace "tympan":
   bool load_project(const char *filename, SmartPtr[TYProjet] &)
   bool save_project(const char *filename, const SmartPtr[TYProjet] &)

cdef extern from "python/include/Loader.hpp" namespace "tympan":
   void load_solver(const char *foldername, TYCalcul *calcul)

cdef extern from "Qt/qstring.h":
    cdef cppclass QString:
        string toStdString()

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
        void processAltimetry(SmartPtr[TYSiteNode] site_ptr)
        void setAcousticTriangle(const TYGeometryNode& acoust_surf_geo)
cdef extern from "Tympan/MetierSolver/DataManagerCore/TYElement.h":
    cdef cppclass TYElement:
        QString getName()
        const char* getClassName() const

cdef extern from "Tympan/MetierSolver/DataManagerMetier/Site/TYSiteNode.h":
    cdef cppclass TYSiteNode:
        void getChilds (vector[SmartPtr[TYElement]] &elts, bool recursive)
        void update(const bool& force)
        void getListFaces(vector[SmartPtr[TYGeometryNode]]& tabFaces,
                          unsigned int& nbFaceInfra,
                          vector[bool]& EstUnIndexDeFaceEcran)

cdef extern from "Tympan/MetierSolver/DataManagerMetier/Commun/TYCalcul.h":
    cdef cppclass TYCalcul:
        bool go()
        AcousticProblemModel _acousticProblem
        AcousticResultModel _acousticResult

cdef extern from "Tympan/MetierSolver/DataManagerMetier/Commun/TYProjet.h":
    cdef cppclass TYProjet:
        SmartPtr[TYCalcul] getCurrentCalcul()
        SmartPtr[TYSiteNode] getSite()

cdef extern from "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticSurface.h":
    cdef cppclass TYAcousticSurface:
        pass

cdef extern from "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticSurface.h" namespace "TYAcousticSurface":
    TYAcousticSurface * safeDownCast(TYElement *)

cdef extern from "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYGeometryNode.h":
    cdef cppclass TYGeometryNode:
        TYElement* getElement()

