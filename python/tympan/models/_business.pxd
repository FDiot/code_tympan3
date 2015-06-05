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

cdef extern from "Tympan/models/business/TYElement.h" namespace "TYElement":
    TYElement* getInstance(OGenID)

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
    TYUserSourcePonctuelle* downcast_user_source_ponctuelle "downcast<TYUserSourcePonctuelle>"(TYElement *)
    TYMaillage* downcast_maillage "downcast<TYMaillage>"(TYElement *)
    TYPointControl* downcast_point_control "downcast<TYPointControl>"(TYElement *)
    TYPointCalcul* downcast_point_calcul "downcast<TYPointCalcul>"(TYElement *)
    TYCourbeNiveau* downcast_courbe_niveau "downcast<TYCourbeNiveau>"(TYElement*)
    TYPlanEau* downcast_plan_eau "downcast<TYPlanEau>"(TYElement*)
    TYTerrain* downcast_terrain "downcast<TYTerrain>"(TYElement*)
    TYSiteNode* downcast_sitenode "downcast<TYSiteNode>"(TYElement*)
    TYSol* downcast_sol "downcast<TYSol>"(TYElement*)
    TYAcousticSurface* downcast_acoustic_surface "downcast<TYAcousticSurface>"(TYElement*)
    TYAcousticSurfaceNode* downcast_acoustic_surface_node "downcast<TYAcousticSurfaceNode>"(TYElement*)
    TYAcousticVolume* downcast_acoustic_volume "downcast<TYAcousticVolume>"(TYElement*)

cdef extern from "Tympan/models/business/acoustic/TYDirectivity.h":
    TYComputedDirectivity* downcast_computed_directivity "downcast_tydirectivity<TYComputedDirectivity>"(TYDirectivity *)

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
        cppmap[TYElem_ptr, vector[SmartPtr[TYGeometryNode]]]& getMapEmetteurSrcs()
        SmartPtr[TYPointCalcul] getRecepteur(const int& idx)
        void setResultMatrix(tycommon.SpectrumMatrix matrix)
        tycommon.SpectrumMatrix& getResultMatrix()
        void setSources(cppmap[TYElem_ptr, int])
        void addRecepteur(TYPointCalcul* pRecepteur)
        int getIndexRecepteur(TYPointCalcul* pRecepteur)

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
        TYComputedDirectivity(tycommon.OVector3D& vec, int type, double size)
        double SpecificSize
        DirectivityType Type

    cdef cppclass TYUserDefinedDirectivity(TYDirectivity, TYElement):
        TYUserDefinedDirectivity()
        TYUserDefinedDirectivity(tycommon.OVector3D& vec)

cdef extern from "Tympan/models/business/acoustic/TYSource.h":
    cdef cppclass TYSource(TYElement):
        TYSpectre* getSpectre() const

cdef extern from "Tympan/models/business/acoustic/TYSourcePonctuelle.h":
    cdef cppclass TYSourcePonctuelle(TYSource):
        SmartPtr[TYPoint] getPos()
        TYDirectivity* getDirectivity() const

cdef extern from "Tympan/models/business/acoustic/TYUserSourcePonctuelle.h":
    cdef cppclass TYUserSourcePonctuelle(TYSourcePonctuelle):
        pass

cdef extern from "Tympan/models/business/infrastructure/TYSiteNode.h":
    cdef cppclass TYSiteNode (TYElement):
        void getChilds (vector[SmartPtr[TYElement]] &elts, bool recursive)
        void getListFaces(vector[SmartPtr[TYGeometryNode]]& tabFaces,
                          unsigned int& nbFaceInfra,
                          vector[bool]& EstUnIndexDeFaceEcran)
        SmartPtr[TYTopographie] getTopographie()
        SmartPtr[TYInfrastructure] getInfrastructure()
        bool updateAltimetrie()
        void updateAltiInfra()
        void updateAcoustique(const bool& force)
        void update(const bool& force)
        TYProjet* getProjet()
        const double getAltiEmprise() const
        const vector[SmartPtr[TYGeometryNode]]& getListSiteNode() const
        bool getUseEmpriseAsCrbNiv() const
        void getFacesOnGround(cppmap[OGenID, deque[tycommon.OPoint3D]]& tabContours) const
        void uuid2tysol(const deque[string]& material_ids, deque[SmartPtr[TYSol]]& materials)

cdef extern from "Tympan/models/business/infrastructure/TYInfrastructure.h":
    cdef cppclass TYInfrastructure (TYElement):
        void getAllSrcs(const TYCalcul* pCalcul, cppmap[TYElem_ptr,
                        vector[SmartPtr[TYGeometryNode]]]& mapElementSrcs)
        vector[SmartPtr[TYElement]] getTabElemNOk()

cdef extern from "Tympan/models/business/TYCalcul.h":
    cdef cppclass TYCalcul (TYElement):
        SmartPtr[TYResultat] getResultat()
        void getAllSources(cppmap[TYElem_ptr, vector[SmartPtr[TYGeometryNode]]]& mapElementSrcs,
                      vector[SmartPtr[TYGeometryNode]])
        void selectActivePoint(SmartPtr[TYSiteNode] pSite)
        const vector[SmartPtr[TYGeometryNode]] getMaillages() const
        vector[SmartPtr[TYRay]]& getTabRays()
        void goPostprocessing()
        const OGenID getSolverId()
        QString solverParams

cdef extern from "Tympan/models/business/TYProjet.h":
    cdef cppclass TYProjet (TYElement):
        SmartPtr[TYCalcul] getCurrentCalcul()
        SmartPtr[TYSiteNode] getSite()
        bool updateAltiRecepteurs(const TYAltimetrie* pAlti)
        vector[SmartPtr[TYPointControl]]& getPointsControl()

cdef extern from "Tympan/models/business/topography/TYAltimetrie.h":
    cdef cppclass TYAltimetrie (TYElement):
        void plugBackTriangulation(const deque[tycommon.OPoint3D]& points,
                                   const deque[tycommon.OTriangle]& triangles,
                                   const deque[SmartPtr[TYSol]]& materials)

cdef extern from "Tympan/models/business/geoacoustic/TYAcousticSurface.h":
    cdef cppclass TYAcousticSurface (TYElement):
        void exportMesh(deque[tycommon.OPoint3D] &, deque[tycommon.OTriangle] &, TYGeometryNode &)
        SmartPtr[TYMateriauConstruction] getMateriau()

cdef extern from "Tympan/models/business/geoacoustic/TYAcousticVolume.h":
    cdef cppclass TYAcousticVolume (TYElement):
        pass

cdef extern from "Tympan/models/business/geoacoustic/TYAcousticSurfaceNode.h":
    cdef cppclass TYAcousticSurfaceNode (TYElement):
        pass

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
        TYSpectre* getSpectre(TYCalcul* pCalcul)
        double getValA()

cdef extern from "Tympan/models/business/TYPointControl.h":
    cdef cppclass TYPointControl (TYPointCalcul):
        pass

cdef extern from "Tympan/models/business/TYRay.h":
    cdef cppclass TYRay (TYElement, tycommon.acoustic_path):
        TYRay(tycommon.acoustic_path& ap)

cdef extern from "Tympan/models/business/TYRay.h":
        cdef SmartPtr[TYRay] build_ray(const tycommon.acoustic_path& ap)

cdef extern from "Tympan/models/business/topography/TYCourbeNiveau.h":
    cdef cppclass TYCourbeNiveau(TYElement):
        const vector[TYPoint]& getListPoints() const
        double getAltitude() const
        TYCourbeNiveau(const vector[TYPoint]& pts, double alt)

cdef extern from "Tympan/models/business/topography/TYTerrain.h":
    cdef cppclass TYTerrain(TYElement):
        SmartPtr[TYSol] getSol()
        const vector[TYPoint]& getListPoints() const
        bool isVegetActive()
        SmartPtr[TYVegetation] getVegetation()

cdef extern from "Tympan/models/business/topography/TYPlanEau.h":
    cdef cppclass TYPlanEau(TYTerrain):
        SmartPtr[TYCourbeNiveau] getCrbNiv()

cdef extern from "Tympan/models/business/material/TYSol.h":
    cdef cppclass TYSol (TYElement):
        double getResistivite()
        double getEcartType()
        double getLongueur()

cdef extern from "Tympan/models/business/material/TYVegetation.h":
    cdef cppclass TYVegetation (TYElement):
        double getHauteur()
        bool getFoliageStatus()

cdef extern from "Tympan/models/business/infrastructure/TYTopographie.h":
    cdef cppclass TYTopographie (TYElement):
        void exportMesh(deque[tycommon.OPoint3D] &, deque[tycommon.OTriangle] &, deque[SmartPtr[TYSol]] &)
        SmartPtr[TYAltimetrie] getAltimetrie()
        void sortTerrainsBySurface()
        const vector[SmartPtr[TYGeometryNode]]& getListPlanEau() const
        const vector[SmartPtr[TYGeometryNode]]& getListTerrain() const
        const vector[SmartPtr[TYGeometryNode]]& getListCrbNiv() const
        const vector[TYPoint]& getEmprise() const

cdef extern from "Tympan/models/business/material/TYMateriauConstruction.h":
    cdef cppclass TYMateriauConstruction (TYElement):
        TYSpectre& getSpectreAbso()

cdef extern from "Tympan/models/business/acoustic/TYSpectre.h":
    cdef cppclass TYSpectre (TYElement, tycommon.OSpectre):
        TYSpectre(const tycommon.OSpectre& spectre)

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

cdef pointcalcul2receptor(SmartPtr[TYPointCalcul] ptcalc)
cdef typrojet2project(TYProjet* proj)
cdef tymateriauconstruction2material(SmartPtr[TYMateriauConstruction] mat)
cdef tysol2ground(SmartPtr[TYSol] grnd)
cdef find_volume_id(TYElement* elem)
cdef find_surface_node_id(TYElement* elem)
