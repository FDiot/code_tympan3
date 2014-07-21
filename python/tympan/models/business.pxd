"""business models for Code_TYMPAN, headers
"""
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.deque cimport deque
from libcpp cimport bool
from libcpp.map cimport map

from tympan.core cimport unique_ptr, QString, SmartPtr, OGenID
from tympan.models cimport common as tycommon
from tympan.models cimport solver as tysolver


cdef extern from "Tympan/models/business/xml_project_util.h" namespace "tympan":
    SmartPtr[TYProjet] load_project(const char *filename) except +
    void save_project(const char *filename, SmartPtr[TYProjet] &) except +

cdef extern from "Tympan/models/business/init_registry.h" namespace "tympan":
    void init_registry()

cdef extern from "Tympan/core/smartptr.h":
    cdef cppclass IRefCount:
        int getRefCount() const
        int incRef()

cdef extern from "Tympan/models/business/TYElement.h":
    cdef cppclass TYElement(IRefCount):
        QString getName()
        const char* getClassName() const
        TYElement* getParent()
        OGenID getID()
        void setIsAcousticModified(bool isModified)

cdef extern from "Tympan/models/business/TYMaillage.h":
    cdef cppclass TYMaillage:
        int getState()
        const vector[SmartPtr[TYPointCalcul]]& getPtsCalcul() const

cdef extern from "Tympan/models/business/TYMaillage.h" namespace "TYMaillage":
    cdef enum MaillageState:
        Actif
        Inactif

cdef extern from "Tympan/models/business/TYElement.h":
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

cdef extern from "Tympan/models/business/TYResultat.h":
    cdef cppclass TYResultat(TYElement):
        bool operator==(const TYResultat& other) const
        size_t getNbOfRecepteurs() const
        size_t getNbOfSources() const
        tycommon.OSpectre getSpectre(const int& indexRecepteur, const int& indexSource) const
        map[TYElem_ptr, vector[SmartPtr[TYGeometryNode]]]& getMapEmetteurSrcs()
        SmartPtr[TYPointCalcul] getRecepteur(const int& idx)
        void setResultMatrix(tysolver.SpectrumMatrix matrix)

cdef extern from "Tympan/models/business/acoustic/TYSource.h":
    cdef cppclass TYSource(TYElement):
        TYSpectre* getSpectre() const

cdef extern from "Tympan/models/business/acoustic/TYSourcePonctuelle.h":
    cdef cppclass TYSourcePonctuelle(TYSource):
        SmartPtr[TYPoint] getPos()

cdef extern from "Tympan/models/business/material/TYAtmosphere.h":
    cdef cppclass TYAtmosphere (TYElement):
        pass

cdef extern from "Tympan/models/business/infrastructure/TYSiteNode.h":
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

cdef extern from "Tympan/models/business/infrastructure/TYInfrastructure.h":
    cdef cppclass TYInfrastructure (TYElement):
        void getAllSrcs(const TYCalcul* pCalcul, map[TYElem_ptr,
                        vector[SmartPtr[TYGeometryNode]]]& mapElementSrcs)

cdef extern from "Tympan/models/business/TYCalcul.h":
    cdef cppclass TYCalcul (TYElement):
        bool go()
        unique_ptr[tysolver.AcousticProblemModel] _acousticProblem
        unique_ptr[tysolver.AcousticResultModel]  _acousticResult
        SmartPtr[TYResultat] getResultat()
        void getAllSources(map[TYElem_ptr, vector[SmartPtr[TYGeometryNode]]]& mapElementSrcs,
                      vector[SmartPtr[TYGeometryNode]])
        SmartPtr[TYAtmosphere] getAtmosphere()
        void selectActivePoint(SmartPtr[TYSiteNode] pSite)
        const vector[SmartPtr[TYGeometryNode]] getMaillages() const
        void setNbThread(unsigned int nbThread)
        void goPostprocessing()

cdef extern from "Tympan/models/business/TYProjet.h":
    cdef cppclass TYProjet (TYElement):
        SmartPtr[TYCalcul] getCurrentCalcul()
        SmartPtr[TYSiteNode] getSite()
        bool updateAltiRecepteurs(const TYAltimetrie* pAlti)
        vector[SmartPtr[TYPointControl]]& getPointsControl()

cdef extern from "Tympan/models/business/topography/TYAltimetrie.h":
    cdef cppclass TYAltimetrie (TYElement):
        pass

cdef extern from "Tympan/models/business/geoacoustic/TYAcousticSurface.h":
    cdef cppclass TYAcousticSurface (TYElement):
        void exportMesh(deque[tycommon.OPoint3D] &, deque[tycommon.OTriangle] &, TYGeometryNode &)
        SmartPtr[TYMateriauConstruction] getMateriau()

cdef extern from "Tympan/models/business/geometry/TYGeometryNode.h":
    cdef cppclass TYGeometryNode (TYElement):
        TYElement* getElement()
        TYGeometryNode(TYElement *)
        tycommon.OMatrix getMatrix()

cdef extern from "Tympan/models/business/geometry/TYPoint.h":
    cdef cppclass TYPoint(TYElement, tycommon.OPoint3D):
        pass

cdef extern from "Tympan/models/business/TYPointCalcul.h":
    cdef cppclass TYPointCalcul (TYPoint):
        bool getEtat(TYCalcul* pCalcul)
        void setSpectre(const TYSpectre& spectre, TYCalcul* pCalcul)

cdef extern from "Tympan/models/business/TYPointControl.h":
    cdef cppclass TYPointControl (TYPointCalcul):
        pass

cdef extern from "Tympan/models/business/topography/TYCourbeNiveau.h":
    cdef cppclass TYCourbeNiveau(TYElement):
        const vector[TYPoint]& getListPoints() const
        double getAltitude() const
        TYCourbeNiveau(const vector[TYPoint]& pts, double alt)

cdef extern from "Tympan/models/business/topography/TYTerrain.h":
    cdef cppclass TYTerrain(TYElement):
        SmartPtr[TYSol] getSol()
        const vector[TYPoint]& getListPoints() const

cdef extern from "Tympan/models/business/topography/TYPlanEau.h":
    cdef cppclass TYPlanEau(TYTerrain):
        SmartPtr[TYCourbeNiveau] getCrbNiv()

cdef extern from "Tympan/models/business/material/TYSol.h":
    cdef cppclass TYSol (TYElement):
        double getResistivite()

cdef extern from "Tympan/models/business/infrastructure/TYTopographie.h":
    cdef cppclass TYTopographie (TYElement):
        void exportMesh(deque[tycommon.OPoint3D] &, deque[tycommon.OTriangle] &, deque[SmartPtr[TYSol]] *)
        SmartPtr[TYAltimetrie] getAltimetrie()
        void sortTerrainsBySurface()

cdef extern from "Tympan/models/business/material/TYMateriauConstruction.h":
    cdef cppclass TYMateriauConstruction (TYElement):
        TYSpectre& getSpectreAbso()

cdef extern from "Tympan/models/business/acoustic/TYSpectre.h":
    cdef cppclass TYSpectre (TYElement, tycommon.OSpectre):
        TYSpectre(const tycommon.OSpectre& spectre)

cdef class Computation:
    cdef SmartPtr[TYCalcul] thisptr

cdef class Site:
    cdef SmartPtr[TYSiteNode] thisptr

cdef class Material:
    cdef SmartPtr[TYMateriauConstruction] thisptr

cdef class Ground:
    cdef SmartPtr[TYSol] thisptr

cdef pointcalcul2receptor(SmartPtr[TYPointCalcul] ptcalc)
cdef typrojet2project(TYProjet* proj)
cdef tymateriauconstruction2material(SmartPtr[TYMateriauConstruction] mat)
cdef tysol2ground(SmartPtr[TYSol] grnd)

