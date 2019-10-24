"""business models for Code_TYMPAN, headers
"""
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp.deque cimport deque
from libcpp cimport bool
from libcpp.map cimport map as cppmap

from tympan._core cimport QString, SmartPtr, OGenID, SolverInterface
from tympan.models cimport _common as tycommon
from tympan.models cimport _solver as tysolver


cdef extern from "Tympan/models/business/xml_project_util.h" namespace "tympan":
    SmartPtr[TYProjet] load_project(const char * filename) except +
    void save_project(const char * filename, SmartPtr[TYProjet] &) except +
    vector[SmartPtr[TYElement]] load_elements(const char * filename) except +

cdef extern from "Tympan/models/business/init_registry.h" namespace "tympan":
    void init_registry()

cdef extern from "Tympan/core/smartptr.h":
    cdef cppclass IRefCount:
        int getRefCount() const
        int incRef()

cdef extern from "Tympan/models/business/TYElement.h":
    cdef cppclass TYElement(IRefCount):
        QString getName()
        void setName(string name)
        const char * getClassName() const
        TYElement * getParent()
        OGenID getID()
        void regenerateID()
        void setIsAcousticModified(bool isModified)
        bool deepCopy(const TYElement * pOther, bool copyId)

cdef extern from "Tympan/models/business/TYMaillage.h":
    cdef cppclass TYMaillage:
        bool etat()
        const vector[SmartPtr[TYPointCalcul]] & getPtsCalcul() const
        void exportCSV(const string & fileName)
        int getDataType()

cdef extern from "Tympan/models/business/TYMaillage.h" namespace "TYMaillage":
    cdef enum MaillageState:
        Actif
        Inactif

cdef extern from "Tympan/models/business/TYElement.h" namespace "TYElement":
    TYElement * getInstance(OGenID)

cdef extern from "Tympan/models/business/TYElement.h":
    TYAcousticSurface * downcast_acoustic_surface "downcast<TYAcousticSurface>"(TYElement *)
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
    TYSourcePonctuelle * downcast_source_ponctuelle "downcast<TYSourcePonctuelle>"(TYElement *)
    TYUserSourcePonctuelle * downcast_user_source_ponctuelle "downcast<TYUserSourcePonctuelle>"(TYElement *)
    TYMaillage * downcast_maillage "downcast<TYMaillage>"(TYElement *)
    TYPointControl * downcast_point_control "downcast<TYPointControl>"(TYElement *)
    TYPointCalcul * downcast_point_calcul "downcast<TYPointCalcul>"(TYElement *)
    TYCourbeNiveau * downcast_courbe_niveau "downcast<TYCourbeNiveau>"(TYElement * )
    TYPlanEau * downcast_plan_eau "downcast<TYPlanEau>"(TYElement * )
    TYTerrain * downcast_terrain "downcast<TYTerrain>"(TYElement * )
    TYSiteNode * downcast_sitenode "downcast<TYSiteNode>"(TYElement * )
    TYSol * downcast_sol "downcast<TYSol>"(TYElement * )
    TYCalcul * downcast_calcul "downcast<TYCalcul>"(TYElement * )
    TYAcousticSurface * downcast_acoustic_surface "downcast<TYAcousticSurface>"(TYElement * )
    TYAcousticSurfaceNode * downcast_acoustic_surface_node "downcast<TYAcousticSurfaceNode>"(TYElement * )
    TYAcousticVolume * downcast_acoustic_volume "downcast<TYAcousticVolume>"(TYElement * )
    TYMachine * downcast_machine "downcast<TYMachine>"(TYElement * )
    TYBatiment * downcast_batiment "downcast<TYBatiment>"(TYElement * )

cdef extern from "Tympan/models/business/acoustic/TYDirectivity.h":
    TYComputedDirectivity * downcast_computed_directivity "downcast_tydirectivity<TYComputedDirectivity>"(TYDirectivity *)

# This is because it seems unsupported to declare a map containing pointers
# http://trac.cython.org/cython_trac/ticket/793
# http://grokbase.com/t/gg/cython-users/12b61wdpnm/std-pair-of-two-pointers-in-cython-problem
ctypedef (TYElement * ) TYElem_ptr

cdef extern from "Tympan/models/business/TYResultat.h":
    cdef cppclass TYResultat(TYElement):
        bool operator == (const TYResultat & other) const
        size_t getNbOfRecepteurs() const
        size_t getNbOfSources() const
        tycommon.OSpectre getSpectre(const int & indexRecepteur, const int & indexSource) const
        tycommon.OSpectre getSpectre(TYElement * pRecepteur, TYElement * pSource)
        cppmap[TYElem_ptr, vector[SmartPtr[TYGeometryNode]]
               ] & getMapEmetteurSrcs()
        SmartPtr[TYPointCalcul] getRecepteur(const int & idx)
        void setResultMatrix(tycommon.SpectrumMatrix matrix)
        tycommon.SpectrumMatrix & getResultMatrix()
        void setSources(cppmap[TYElem_ptr, int])
        void addRecepteur(TYElement * pRecepteur)
        void addSource(TYElement * pSource)
        int getIndexRecepteur(TYPointCalcul * pRecepteur)
        vector[SmartPtr[TYElement]] getSources()
        vector[SmartPtr[TYElement]] getReceptors()
        bool setSpectre(TYElement * pRecepteur, TYElement * pSource, tycommon.OSpectre & Spectre)
        void buildMatrix()
        void setHideLW(bool isHidden)

cdef extern from "Tympan/models/business/acoustic/TYDirectivity.h" namespace "TYComputedDirectivity":
    cdef enum DirectivityType:
        Surface
        Baffled
        Chimney

cdef extern from "Tympan/models/business/acoustic/TYDirectivity.h":
    cdef cppclass TYDirectivity(TYElement):
        TYDirectivity()
        tycommon.OVector3D DirectivityVector

    cdef cppclass TYComputedDirectivity(TYDirectivity, TYElement):
        TYComputedDirectivity()
        TYComputedDirectivity(tycommon.OVector3D & vec, int type, double size)
        double SpecificSize
        DirectivityType Type

    cdef cppclass TYUserDefinedDirectivity(TYDirectivity, TYElement):
        TYUserDefinedDirectivity()
        TYUserDefinedDirectivity(tycommon.OVector3D & vec)

cdef extern from "Tympan/models/business/acoustic/TYSource.h":
    cdef cppclass TYSource(TYElement):
        TYSpectre * getSpectre() const
        void setSpectre(const TYSpectre * spectrum)

cdef extern from "Tympan/models/business/acoustic/TYSourcePonctuelle.h":
    cdef cppclass TYSourcePonctuelle(TYSource):
        SmartPtr[TYPoint] getPos()
        void setPos(const SmartPtr[TYPoint] pPos)
        double getHauteur()
        void setHauteur(double hauteur)
        TYDirectivity * getDirectivity() const

cdef extern from "Tympan/models/business/acoustic/TYUserSourcePonctuelle.h":
    cdef cppclass TYUserSourcePonctuelle(TYSourcePonctuelle):
        TYUserSourcePonctuelle()
        TYSpectre* getCurrentSpectre()
        void updateCurrentRegime()


cdef extern from "Tympan/models/business/infrastructure/TYSiteNode.h":
    cdef cppclass TYSiteNode(TYElement):
        void getChilds(vector[SmartPtr[TYElement]] & elts, bool recursive)
        void getListFaces(vector[SmartPtr[TYGeometryNode]] & tabFaces,
                          unsigned int & nbFaceInfra,
                          vector[bool] & EstUnIndexDeFaceEcran)
        SmartPtr[TYTopographie] getTopographie()
        SmartPtr[TYInfrastructure] getInfrastructure()
        bool updateAltimetrie()
        void updateAltiInfra()
        void updateAcoustique(const bool & force)
        void update(const bool & force)
        TYProjet * getProjet()
        const double getAltiEmprise() const
        void setAltiEmprise(const double & altiEmprise)
        const vector[SmartPtr[TYGeometryNode]] & getListSiteNode() const
        bool getUseEmpriseAsCrbNiv() const
        void setUseEmpriseAsCrbNiv(bool b)
        void getFacesOnGround(cppmap[OGenID, deque[deque[tycommon.OPoint3D]]] & tabContours) const
        void uuid2tysol(const deque[string] & material_ids, deque[SmartPtr[TYSol]] & materials)
        bool addSiteNode(SmartPtr[TYGeometryNode] pSiteNodeGeoNode)

        int getSIGType()
        void setSIGType(int SIGType)
        double getSIG_X()
        void setSIG_X(const double & SIG_X)
        double getSIG_Y()
        void setSIG_Y(const double & SIG_Y)
        double getSIG_OFFSET()
        void setSIG_OFFSET(const double & SIG_OFFSET)

cdef extern from "Tympan/models/business/geoacoustic/TYAcousticVolumeNode.h":
    cdef cppclass TYAcousticVolumeNode(TYElement):
        pass

cdef extern from "Tympan/models/business/infrastructure/TYMachine.h":
    cdef cppclass TYMachine(TYAcousticVolumeNode):
        TYMachine()
        vector[TYPoint] & sommets()
        TYSpectre& getCurrentSpectre()

cdef extern from "Tympan/models/business/infrastructure/TYBatiment.h":
    cdef cppclass TYBatiment(TYAcousticVolumeNode):
        TYBatiment()
        bool getIsRayonnant()
        vector[TYPoint] & sommets()
        TYSpectre& getCurrentSpectre()

cdef extern from "Tympan/models/business/infrastructure/TYInfrastructure.h":
    cdef cppclass TYInfrastructure(TYElement):
        void getAllSrcs(const TYCalcul * pCalcul, cppmap[TYElem_ptr,
                                                         vector[SmartPtr[TYGeometryNode]]] & mapElementSrcs)
        vector[SmartPtr[TYElement]] getTabElemNOk()
        bool addSrc(SmartPtr[TYUserSourcePonctuelle] pSrc)
        bool addSrc(SmartPtr[TYGeometryNode] pSrcGeoNode)
        bool addMachine(SmartPtr[TYGeometryNode] pMachineGeoNode)
        bool addBatiment(SmartPtr[TYGeometryNode] pBatimentGeoNode)
        vector[SmartPtr[TYGeometryNode]] & getListBatiment()
        vector[SmartPtr[TYGeometryNode]] & getListMachine()
        vector[SmartPtr[TYGeometryNode]] & getSrcs()

cdef extern from "Tympan/models/business/TYCalcul.h":
    cdef cppclass TYCalcul(TYElement):
        TYCalcul()
        SmartPtr[TYResultat] getResultat()
        void getAllSources(cppmap[TYElem_ptr, vector[SmartPtr[TYGeometryNode]]] & mapElementSrcs,
                           vector[SmartPtr[TYGeometryNode]])
        vector[SmartPtr[TYRay]] & getTabRays()
        void goPostprocessing()
        void setSolverId(const OGenID & iD)
        const OGenID getSolverId()
        bool addPtCtrlToResult(SmartPtr[TYPointControl] pPoint)
        void setSpectre(const TYPointCalcul * pPoint, TYSpectre * pSpectre)
        bool addMaillage(TYMaillage * pMaillage)
        vector[SmartPtr[TYSpectre]] * getSpectrumDatas(TYMaillage * pMaillage)
        QString solverParams

cdef extern from "Tympan/models/business/TYCalcul.h":
    TYElement * downcast_Element "downcast<TYElement>"(TYCalcul *)

cdef extern from "Tympan/models/business/TYPluginManager.h" namespace "tympan":
    OGenID solver_id(string name) except +
    void load_solvers(const char * name)

cdef extern from "Tympan/models/business/TYProjet.h":
    cdef cppclass TYProjet(TYElement):
        TYProjet()
        SmartPtr[TYCalcul] getCurrentCalcul()
        void setCurrentCalcul(SmartPtr[TYCalcul] pCurCalcul)
        void addCalcul(SmartPtr[TYCalcul] pCalcul)
        void selectActivePoint(SmartPtr[TYSiteNode] pSite)
        SmartPtr[TYSiteNode] getSite()
        bool updateAltiRecepteurs(const TYAltimetrie * pAlti)
        vector[SmartPtr[TYPointControl]] & getPointsControl()
        vector[SmartPtr[TYCalcul]] & getListCalcul()
        vector[SmartPtr[TYGeometryNode]] & getMaillages()
        bool addPointControl(SmartPtr[TYPointControl] pPointControl)

cdef extern from "Tympan/models/business/topography/TYAltimetrie.h":
    cdef cppclass TYAltimetrie(TYElement):
        void plugBackTriangulation(const deque[tycommon.OPoint3D] & points,
                                   const deque[tycommon.OTriangle] & triangles,
                                   const deque[SmartPtr[TYSol]] & materials)

cdef extern from "Tympan/models/business/geoacoustic/TYAcousticSurface.h":
    cdef cppclass TYAcousticSurface(TYElement):
        void exportMesh(deque[tycommon.OPoint3D] & , deque[tycommon.OTriangle] & , TYGeometryNode & )
        SmartPtr[TYMateriauConstruction] getMateriau()
        bool getIsRayonnant()

cdef extern from "Tympan/models/business/geoacoustic/TYAcousticVolume.h":
    cdef cppclass TYAcousticVolume(TYElement):
        pass

cdef extern from "Tympan/models/business/geoacoustic/TYAcousticSurfaceNode.h":
    cdef cppclass TYAcousticSurfaceNode(TYElement):
        pass

cdef extern from "Tympan/models/business/geometry/TYGeometryNode.h":
    cdef cppclass TYGeometryNode(TYElement):
        TYGeometryNode()
        TYGeometryNode(TYElement * )
        TYElement * getElement()
        void setElement(TYElement * pElt)
        tycommon.OMatrix getMatrix()
        void setMatrix(const tycommon.OMatrix & matrix)
        void setPosition(const tycommon.OPoint3D & pos)
        void setRotation(const tycommon.OPoint3D & rot)
        void setHauteur(const double & hauteur)
        tycommon.OPoint3D position()
        tycommon.OPoint3D rotation()
        double getHauteur()


cdef extern from "Tympan/models/business/geometry/TYPoint.h":
    cdef cppclass TYPoint(TYElement, tycommon.OPoint3D):
        TYPoint(bool PutInInstanceList=false)
        TYPoint(const TYPoint & other, bool PutInInstanceList=false)
        tycommon.OPoint3D * downcast_opoint()

cdef extern from "Tympan/models/business/TYPointCalcul.h":
    cdef cppclass TYPointCalcul(TYPoint):
        bool etat(TYCalcul * pCalcul)
        void setSpectre(const TYSpectre & spectre, TYCalcul * pCalcul)
        TYSpectre * getSpectre(TYCalcul * pCalcul)
        double getValA()

cdef extern from "Tympan/models/business/TYPointControl.h":
    cdef cppclass TYPointControl(TYPointCalcul):
        TYPointControl()
        TYPointControl(const TYPoint & other)
        void setHauteur(double hauteur)

cdef extern from "Tympan/models/business/TYRay.h":
    cdef cppclass TYRay(TYElement, tycommon.acoustic_path):
        TYRay(tycommon.acoustic_path & ap)

cdef extern from "Tympan/models/business/TYRay.h":
    cdef SmartPtr[TYRay] build_ray(const tycommon.acoustic_path & ap)

cdef extern from "Tympan/models/business/topography/TYCourbeNiveau.h":
    cdef cppclass TYCourbeNiveau(TYElement):
        TYCourbeNiveau(const vector[TYPoint] & pts, double alt)
        const vector[TYPoint] & getListPoints() const
        double getAltitude() const

cdef extern from "Tympan/models/business/topography/TYTerrain.h":
    cdef cppclass TYTerrain(TYElement):
        SmartPtr[TYSol] getSol()
        const vector[TYPoint] & getListPoints() const
        bool isVegetActive()
        SmartPtr[TYVegetation] getVegetation()

cdef extern from "Tympan/models/business/topography/TYPlanEau.h":
    cdef cppclass TYPlanEau(TYTerrain):
        SmartPtr[TYCourbeNiveau] getCrbNiv()

cdef extern from "Tympan/models/business/material/TYSol.h":
    cdef cppclass TYSol(TYElement):
        double getResistivite()
        void setResistivite(double res)
        double getEpaisseur()
        double getEcartType()
        double getLongueur()

cdef extern from "Tympan/models/business/material/TYVegetation.h":
    cdef cppclass TYVegetation(TYElement):
        double getHauteur()
        bool getFoliageStatus()

cdef extern from "Tympan/models/business/infrastructure/TYTopographie.h":
    cdef cppclass TYTopographie(TYElement):
        void exportMesh(deque[tycommon.OPoint3D] & , deque[tycommon.OTriangle] & , deque[SmartPtr[TYSol]] & )
        SmartPtr[TYAltimetrie] getAltimetrie()
        void sortTerrainsBySurface()
        bool addCrbNiv(SmartPtr[TYCourbeNiveau] pCrbNivGeoNode)
        bool remCrbNiv(const SmartPtr[TYCourbeNiveau] pCrbNiv)
        void setEmprise(const vector[TYPoint] & pts, const bool & defTerrain)
        const vector[SmartPtr[TYGeometryNode]] & getListPlanEau() const
        const vector[SmartPtr[TYGeometryNode]] & getListTerrain() const
        const vector[SmartPtr[TYGeometryNode]] & getListCrbNiv() const
        const vector[TYPoint] & getEmprise() const

cdef extern from "Tympan/models/business/material/TYMateriauConstruction.h":
    cdef cppclass TYMateriauConstruction(TYElement):
        TYSpectre & getSpectreAbso()

cdef extern from "Tympan/models/business/acoustic/TYSpectre.h":
    cdef cppclass TYSpectre(TYElement, tycommon.OSpectre):
        TYSpectre(const tycommon.OSpectre & spectre)
        tycommon.OSpectre * downcast_ospectre()

cdef class Result:
    cdef SmartPtr[TYResultat] thisptr

cdef class Computation:
    cdef SmartPtr[TYCalcul] thisptr

cdef class Site:
    cdef SmartPtr[TYSiteNode] thisptr
    cdef tycommon.OMatrix matrix

cdef class Material:
    cdef SmartPtr[TYMateriauConstruction] thisptr

cdef class Ground:
    cdef SmartPtr[TYSol] thisptr

cdef class Vegetation:
    cdef SmartPtr[TYVegetation] thisptr

cdef class User_source:
    cdef SmartPtr[TYUserSourcePonctuelle] thisptr
    cdef SmartPtr[TYGeometryNode] thisgeonodeptr

cdef class Engine:
    cdef SmartPtr[TYMachine] thisptr
    cdef SmartPtr[TYGeometryNode] thisgeonodeptr

cdef class Building:
    cdef SmartPtr[TYBatiment] thisptr
    cdef SmartPtr[TYGeometryNode] thisgeonodeptr

cdef pointcalcul2receptor(SmartPtr[TYPointCalcul] ptcalc)
cdef typrojet2project(TYProjet * proj)
cdef tymateriauconstruction2material(SmartPtr[TYMateriauConstruction] mat)
cdef tysol2ground(SmartPtr[TYSol] grnd)
cdef find_volume_id(TYElement * elem)
cdef find_volume_name(TYElement * elem)
cdef find_surface_node_id(TYElement * elem)
cdef find_surface_node_name(TYElement * elem)
