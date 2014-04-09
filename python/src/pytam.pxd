from libcpp cimport bool
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.deque cimport deque

# SmartPtr wrapping
cdef extern from "Tympan/Tools/OSmartPtr.h":
    cdef cppclass SmartPtr[T]:
        SmartPtr()
        SmartPtr(T *)
        T *getRealPointer()

cdef extern from "boost/shared_ptr.hpp" namespace "boost":
    cdef cppclass shared_ptr[T]:
        T *get()

cdef extern from "Tympan/MetierSolver/DataManagerMetier/xml_project_util.hpp" namespace "tympan":
   SmartPtr[TYProjet] load_project(const char *filename)
   void save_project(const char *filename, const SmartPtr[TYProjet] &)

cdef extern from "Tympan/Tools/OGenID.h":
    cdef cppclass OGenID:
        pass

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
        AcousticTriangle &triangle(size_t tri_idx)
        AcousticTriangle *ptriangle(size_t tri_idx)
        shared_ptr[AcousticMaterialBase] make_material(const string& name, double resistivity)

cdef extern from "Tympan/MetierSolver/SolverDataModel/acoustic_result_model.hpp" namespace "tympan":
    cdef cppclass AcousticResultModel:
        pass

cdef extern from "Tympan/MetierSolver/DataExchange/SolverDataModelBuilder.hpp" namespace "tympan":
    cdef cppclass SolverDataModelBuilder:
        SolverDataModelBuilder(AcousticProblemModel &model)
        void setAcousticTriangle(const TYGeometryNode& acoust_surf_geo)
        void processMesh(const deque[OPoint3D] &points, const deque[OTriangle] &triangles)
    cdef cppclass UuidAdapter:
        UuidAdapter(const OGenID& rhs)
        binary_uuid getUuid()

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
        SmartPtr[TYTopographie] getTopographie()

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

cdef extern from "Tympan/MetierSolver/ToolsMetier/OPoint3D.h":
    cdef cppclass OPoint3D:
        pass

cdef extern from "Tympan/MetierSolver/ToolsMetier/OTriangle.h":
    cdef cppclass OTriangle:
        pass

cdef extern from "Tympan/MetierSolver/DataManagerMetier/EltMateriaux/TYSol.h":
    cdef cppclass TYSol:
        double getResistivite()
        # XXX here we should be able to use TYElement::getName() since TYSol
        # inherits from TYElement...
        QString getName()

cdef extern from "Tympan/MetierSolver/DataManagerMetier/Site/TYTopographie.h":
    cdef cppclass TYTopographie:
        const OGenID& getID()
        void exportMesh(deque[OPoint3D] &, deque[OTriangle] &, deque[SmartPtr[TYSol]] *)

cdef extern from "Tympan/MetierSolver/SolverDataModel/data_model_common.hpp" namespace "tympan":
    struct binary_uuid:
        pass

cdef extern from "Tympan/MetierSolver/SolverDataModel/entities.hpp" namespace "tympan":
    cdef cppclass AcousticTriangle:
        shared_ptr[AcousticMaterialBase] made_of
        binary_uuid uuid
    cdef cppclass AcousticMaterialBase:
        pass