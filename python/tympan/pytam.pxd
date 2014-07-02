from libcpp cimport bool
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.deque cimport deque
from libcpp.map cimport map

cdef extern from "Tympan/core/smartptr.h":
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
    void save_project(const char *filename, SmartPtr[TYProjet] &) except +

cdef extern from "Tympan/core/idgen.h":
    cdef cppclass OGenID:
        OGenID(const OGenID& other)
        OGenID()

cdef extern from "Tympan/MetierSolver/DataManagerMetier/init_registry.h" namespace "tympan":
    void init_registry()

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
        size_t nreceptors()
        AcousticTriangle &triangle(size_t tri_idx)
        AcousticSource& source(size_t idx)
        AcousticReceptor& receptor(size_t idx)
        OPoint3D& node(size_t idx)
        shared_ptr[AcousticMaterialBase] make_material(const string& name, double resistivity)
        shared_ptr[AcousticMaterialBase] make_material(const string& name, const OSpectre& spectrum)
        size_t make_triangle(size_t n1, size_t n2, size_t n3)
        size_t make_node(const OPoint3D&)
        size_t make_source(const OPoint3D& point_, const OSpectre& spectrum_)
        size_t make_receptor(const OPoint3D& point_)

cdef extern from "Tympan/MetierSolver/SolverDataModel/acoustic_result_model.hpp" namespace "tympan":
    cdef cppclass AcousticResultModel:
        pass

cdef extern from "Tympan/MetierSolver/DataManagerCore/TYElement.h":
    cdef cppclass TYElement:
        QString getName()
        const char* getClassName() const
        TYElement* getParent()
        OGenID getID()

cdef extern from "Tympan/MetierSolver/DataManagerMetier/Commun/TYMaillage.h":
    cdef cppclass TYMaillage:
        int getState()
        const vector[SmartPtr[TYPointCalcul]]& getPtsCalcul() const

cdef extern from "Tympan/MetierSolver/DataManagerMetier/Commun/TYMaillage.h" namespace "TYMaillage":
    cdef enum MaillageState:
        Actif
        Inactif

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
    TYMaillage* downcast_maillage "downcast<TYMaillage>"(TYElement *)
    TYPointControl* downcast_point_control "downcast<TYPointControl>"(TYElement *)

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
        SmartPtr[TYPointCalcul] getRecepteur(const int& idx)

cdef extern from "Tympan/models/common/spectre.h":
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

cdef extern from "Tympan/MetierSolver/SolverDataModel/data_model_common.hpp":
    cdef cppclass BaseEntity:
        pass

cdef extern from "Tympan/MetierSolver/SolverDataModel/entities.hpp" namespace "tympan":
    cdef cppclass AcousticSource(BaseEntity):
        OPoint3D position
        OSpectre spectrum

cdef extern from "Tympan/MetierSolver/SolverDataModel/entities.hpp" namespace "tympan":
    cdef cppclass AcousticReceptor(BaseEntity):
        OPoint3D position

cdef extern from "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSource.h":
    cdef cppclass TYSource(TYElement):
        TYSpectre* getSpectre() const

cdef extern from "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSourcePonctuelle.h":
    cdef cppclass TYSourcePonctuelle(TYSource):
        SmartPtr[TYPoint] getPos()

cdef extern from "Tympan/MetierSolver/DataManagerMetier/EltMateriaux/TYAtmosphere.h":
    cdef cppclass TYAtmosphere (TYElement):
        pass

cdef extern from "Tympan/MetierSolver/DataManagerMetier/Site/TYSiteNode.h":
    cdef cppclass TYSiteNode (TYElement):
        void getChilds (vector[SmartPtr[TYElement]] &elts, bool recursive)
        void getListFaces(vector[SmartPtr[TYGeometryNode]]& tabFaces,
                          unsigned int& nbFaceInfra,
                          vector[bool]& EstUnIndexDeFaceEcran)
        SmartPtr[TYTopographie] getTopographie()
        SmartPtr[TYInfrastructure] getInfrastructure()
        void updateAltiInfra(const bool& force)
        void updateAcoustique(const bool& force)
        void update(const bool& force)
        void setAtmosphere(const SmartPtr[TYAtmosphere]& pAtmosphere)
        TYProjet* getProjet()

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
        SmartPtr[TYAtmosphere] getAtmosphere()
        void selectActivePoint(SmartPtr[TYSiteNode] pSite)
        const vector[SmartPtr[TYGeometryNode]] getMaillages() const
        void setNbThread(unsigned int nbThread)

cdef extern from "Tympan/MetierSolver/DataManagerMetier/Commun/TYProjet.h":
    cdef cppclass TYProjet (TYElement):
        SmartPtr[TYCalcul] getCurrentCalcul()
        SmartPtr[TYSiteNode] getSite()
        bool updateAltiRecepteurs(const TYAltimetrie* pAlti)
        vector[SmartPtr[TYPointControl]]& getPointsControl()

cdef extern from "Tympan/MetierSolver/DataManagerMetier/EltTopographique/TYAltimetrie.h":
    cdef cppclass TYAltimetrie (TYElement):
        pass

cdef extern from "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticSurface.h":
    cdef cppclass TYAcousticSurface (TYElement):
        void exportMesh(deque[OPoint3D] &, deque[OTriangle] &, TYGeometryNode &)
        SmartPtr[TYMateriauConstruction] getMateriau()

cdef extern from "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYGeometryNode.h":
    cdef cppclass TYGeometryNode (TYElement):
        TYElement* getElement()
        TYGeometryNode(TYElement *)
        OMatrix getMatrix()

cdef extern from "Tympan/models/common/3d.h":
    cdef cppclass OMatrix:
        pass
    OVector3D operator*(const OMatrix& mat, const OVector3D& vector)
    OPoint3D operator*(const OMatrix& mat, const OPoint3D& point)

    cdef cppclass OCoord3D:
        double _x
        double _y
        double _z
        double _value[3]
        bool operator==(const OCoord3D& coord)

    cdef cppclass OPoint3D (OCoord3D):
        pass

    cdef cppclass OVector3D (OCoord3D):
        pass

cdef extern from "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYPoint.h":
    cdef cppclass TYPoint(TYElement, OPoint3D):
        pass

cdef extern from "Tympan/MetierSolver/DataManagerMetier/Commun/TYPointCalcul.h":
    cdef cppclass TYPointCalcul (TYPoint):
        bool getEtat(TYCalcul* pCalcul)

cdef extern from "Tympan/MetierSolver/DataManagerMetier/Commun/TYPointControl.h":
    cdef cppclass TYPointControl (TYPointCalcul):
        pass

cdef extern from "Tympan/models/common/triangle.h":
    cdef cppclass OTriangle:
        OPoint3D _A
        OPoint3D _B
        OPoint3D _C
        int _p1
        int _p2
        int _p3

        bool checkConsistencyWrtPointsTab(const deque[OPoint3D]& points) const
        string reportInconsistencyWrtPointsTab(const deque[OPoint3D]& points)


cdef extern from "Tympan/MetierSolver/DataManagerMetier/EltMateriaux/TYSol.h":
    cdef cppclass TYSol (TYElement):
        double getResistivite()

cdef extern from "Tympan/MetierSolver/DataManagerMetier/Site/TYTopographie.h":
    cdef cppclass TYTopographie (TYElement):
        void exportMesh(deque[OPoint3D] &, deque[OTriangle] &, deque[SmartPtr[TYSol]] *)
        SmartPtr[TYAltimetrie] getAltimetrie()
        void sortTerrainsBySurface()

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

cdef extern from "Tympan/MetierSolver/CommonTools/prettyprint.hpp":
    cdef string OPoint3D_to_string "tympan_to_string<OPoint3D>"(const OPoint3D& o)


