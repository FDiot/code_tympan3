from libcpp cimport bool
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.deque cimport deque
from libcpp.map cimport map

cdef extern from "Tympan/Tools/OSmartPtr.h":
    cdef cppclass SmartPtr[T]:
        SmartPtr()
        SmartPtr(T *)
        T *getRealPointer()

cdef extern from "boost/shared_ptr.hpp" namespace "boost":
    cdef cppclass shared_ptr[T]:
        T *get()

cdef extern from "memory" namespace "std":
    cdef cppclass unique_ptr[T]:
        T *get()

cdef extern from "Tympan/MetierSolver/DataManagerMetier/xml_project_util.hpp" namespace "tympan":
    SmartPtr[TYProjet] load_project(const char *filename) except +
    void save_project(const char *filename, const SmartPtr[TYProjet] &) except +

cdef extern from "Tympan/Tools/OGenID.h":
    cdef cppclass OGenID:
        OGenID(const OGenID& other)
        OGenID()

cdef extern from "python/include/Loader.hpp" namespace "tympan":
   void load_solver(const char *foldername, TYCalcul *calcul)

cdef extern from "Qt/qstring.h":
    cdef cppclass QString:
        string toStdString()

cdef extern from "Tympan/MetierSolver/SolverDataModel/acoustic_problem_model.hpp" namespace "tympan":
    cdef cppclass AcousticProblemModel:
        size_t npoints()
        size_t ntriangles()
        size_t nmaterials()
        size_t nsources()
        AcousticTriangle &triangle(size_t tri_idx)
        OPoint3D& node(size_t idx)
        shared_ptr[AcousticMaterialBase] make_material(const string& name, double resistivity)
        shared_ptr[AcousticMaterialBase] make_material(const string& name, const OSpectre& spectrum)
        size_t make_triangle(size_t n1, size_t n2, size_t n3)
        size_t make_node(const OPoint3D&)
        size_t make_source(const OPoint3D& point_, const OSpectre& spectrum_)

cdef extern from "Tympan/MetierSolver/SolverDataModel/acoustic_result_model.hpp" namespace "tympan":
    cdef cppclass AcousticResultModel:
        pass

cdef extern from "Tympan/MetierSolver/DataManagerCore/TYElement.h":
    cdef cppclass TYElement:
        QString getName()
        const char* getClassName() const
        TYElement* getParent()
        OGenID getID()

cdef extern from "Tympan/MetierSolver/DataManagerCore/TYElement.h":
    TYAcousticSurface* downcast_acoustic_surface "downcast<TYAcousticSurface>"(TYElement *)
    # Occurences of downcast_acoustic_surface in Cython will be directly
    # translated into downcast<TYAcousticSurface> in C++.
    # This is compatible with cython 0.19. Templates are supported from cython
    # 0.20. This code thus sticks to cython 0.19.
    # It was tried to upgrade to cython 0.20 and wrap the template this way
    # but it didn't work:
    #    "T* downcast[T](TYElement * elt)"
    # As a matter of fact, the T* return type caused a compilation error
    #  ("'T' is not a type identifier")
    # However "T max[T](T a, T b)" is actually supported in cython 0.20.
    TYSourcePonctuelle* downcast_source_ponctuelle "downcast<TYSourcePonctuelle>"(TYElement *)

# This is because it seems unsupported to declare a map containing pointers
# http://trac.cython.org/cython_trac/ticket/793
# http://grokbase.com/t/gg/cython-users/12b61wdpnm/std-pair-of-two-pointers-in-cython-problem
ctypedef (TYElement *) TYElem_ptr

cdef extern from "Tympan/MetierSolver/DataManagerMetier/Commun/TYResultat.h":
    cdef cppclass TYResultat(TYElement):
        bool operator==(const TYResultat& other) const
        size_t getNbOfRecepteurs() const
        size_t getNbOfSources() const
        OSpectre getSpectre(const int& indexRecepteur, const int& indexSource) const
        map[TYElem_ptr, vector[SmartPtr[TYGeometryNode]]]& getMapEmetteurSrcs()

cdef extern from "Tympan/MetierSolver/CommonTools/OSpectre.h":
    const unsigned int TY_SPECTRE_DEFAULT_NB_ELMT
    cdef enum TYSpectreType:
        SPECTRE_TYPE_ATT
        SPECTRE_TYPE_ABSO
        SPECTRE_TYPE_LW
        SPECTRE_TYPE_LP
        SPECTRE_TYPE_AUTRE
    cdef cppclass OSpectre:
        OSpectre()
        bool operator==(const OSpectre& other) const
        bool operator!=(const OSpectre& other) const
        OSpectre toDB()
        void setType(TYSpectreType type)
        double * getTabValReel()
        unsigned int getNbValues() const

cdef extern from "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSource.h":
    cdef cppclass TYSource(TYElement):
        TYSpectre* getSpectre() const

cdef extern from "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSourcePonctuelle.h":
    cdef cppclass TYSourcePonctuelle(TYSource):
        SmartPtr[TYPoint] getPos()

cdef extern from "Tympan/MetierSolver/DataManagerMetier/Site/TYSiteNode.h":
    cdef cppclass TYSiteNode (TYElement):
        void getChilds (vector[SmartPtr[TYElement]] &elts, bool recursive)
        void update(const bool& force)
        void getListFaces(vector[SmartPtr[TYGeometryNode]]& tabFaces,
                          unsigned int& nbFaceInfra,
                          vector[bool]& EstUnIndexDeFaceEcran)
        SmartPtr[TYTopographie] getTopographie()
        SmartPtr[TYInfrastructure] getInfrastructure()

cdef extern from "Tympan/MetierSolver/DataManagerMetier/Site/TYInfrastructure.h":
    cdef cppclass TYInfrastructure (TYElement):
        void getAllSrcs(const TYCalcul* pCalcul, map[TYElem_ptr,
                        vector[SmartPtr[TYGeometryNode]]]& mapElementSrcs)

cdef extern from "Tympan/MetierSolver/DataManagerMetier/Commun/TYCalcul.h":
    cdef cppclass TYCalcul (TYElement):
        bool go()
        unique_ptr[AcousticProblemModel] _acousticProblem
        unique_ptr[AcousticResultModel]  _acousticResult
        SmartPtr[TYResultat] getResultat()
        void getAllSources(map[TYElem_ptr, vector[SmartPtr[TYGeometryNode]]]& mapElementSrcs,
                      vector[SmartPtr[TYGeometryNode]])

cdef extern from "Tympan/MetierSolver/DataManagerMetier/Commun/TYProjet.h":
    cdef cppclass TYProjet (TYElement):
        SmartPtr[TYCalcul] getCurrentCalcul()
        SmartPtr[TYSiteNode] getSite()

cdef extern from "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticSurface.h":
    cdef cppclass TYAcousticSurface (TYElement):
        void exportMesh(deque[OPoint3D] &, deque[OTriangle] &, TYGeometryNode &)
        SmartPtr[TYMateriauConstruction] getMateriau()

cdef extern from "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYGeometryNode.h":
    cdef cppclass TYGeometryNode (TYElement):
        TYElement* getElement()
        TYGeometryNode(TYElement *)

cdef extern from "Tympan/MetierSolver/CommonTools/OCoord3D.h":
    cdef cppclass OCoord3D:
        double _x
        double _y
        double _z
        double _value[3]
        bool operator==(const OCoord3D& coord)

cdef extern from "Tympan/MetierSolver/CommonTools/OPoint3D.h":
    cdef cppclass OPoint3D (OCoord3D):
        pass

cdef extern from "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYPoint.h":
    cdef cppclass TYPoint(TYElement, OPoint3D):
        pass

cdef extern from "Tympan/MetierSolver/CommonTools/OTriangle.h":
    cdef cppclass OTriangle:
        OPoint3D _A
        OPoint3D _B
        OPoint3D _C
        int _p1
        int _p2
        int _p3

cdef extern from "Tympan/MetierSolver/DataManagerMetier/EltMateriaux/TYSol.h":
    cdef cppclass TYSol (TYElement):
        double getResistivite()

cdef extern from "Tympan/MetierSolver/DataManagerMetier/Site/TYTopographie.h":
    cdef cppclass TYTopographie (TYElement):
        void exportMesh(deque[OPoint3D] &, deque[OTriangle] &, deque[SmartPtr[TYSol]] *)

cdef extern from "Tympan/MetierSolver/SolverDataModel/entities.hpp" namespace "tympan":
    cdef cppclass AcousticTriangle:
        shared_ptr[AcousticMaterialBase] made_of
        size_t n[3]
    cdef cppclass AcousticMaterialBase:
        pass

cdef extern from "Tympan/MetierSolver/DataManagerMetier/EltMateriaux/TYMateriauConstruction.h":
    cdef cppclass TYMateriauConstruction (TYElement):
        TYSpectre& getSpectreAbso()

cdef extern from "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSpectre.h":
    cdef cppclass TYSpectre (TYElement, OSpectre):
        pass
