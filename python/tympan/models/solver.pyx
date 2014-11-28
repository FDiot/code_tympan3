"""THIS FILE IS GENERATED, DON'T EDIT IT"""
"""solver models for Code_TYMPAN
"""

import cython as cy

import numpy as np
cimport numpy as np

from tympan.models cimport common as tycommon
from tympan.core cimport unique_ptr, shared_ptr

cdef class ProblemModel:

    def __cinit__(self):
        self.thisptr = shared_ptr[AcousticProblemModel](new AcousticProblemModel())

    @property
    def npoints(self):
        """ Return the number of mesh nodes contained in the acoustic problem
            model
        """
        assert self.thisptr.get() != NULL
        return self.thisptr.get().npoints()

    @property
    def ntriangles(self):
        """ Return the number of mesh triangles contained in the acoustic problem
            model
        """
        assert self.thisptr.get() != NULL
        return self.thisptr.get().ntriangles()

    @property
    def nmaterials(self):
        """ Return the number of acoustic materials contained in the acoustic
            problem model
        """
        assert self.thisptr.get() != NULL
        return self.thisptr.get().nmaterials()

    def add_source(self, position, spectrum_values, shift):
        """ position is a (x, y, z) tuple; spectrum_values is a np array containing the
        DB values of a power spectrum, shift is the number of the first frequence for which
        a DB value is given (frequences start at 16 Hz and end at 16000Hz, 31 values in total).
        Add the corresponding acoustic source to the solver model (the acoustic
        source is created with a SphericalFaceDirectivity)
        """
        # position
        pos = cy.declare(tycommon.OPoint3D)
        pos = tycommon.OPoint3D(position[0], position[1], position[2])
        # spectrum
        nb_val = spectrum_values.shape[0]
        cdef np.ndarray[double, ndim=1, mode="c"] vals = spectrum_values
        spec = cy.declare(tycommon.OSpectre)
        spec = tycommon.OSpectre(&vals[0], nb_val, shift)
        spec.setEtat(tycommon.SPECTRE_ETAT_DB)
        spec.setType(tycommon.SPECTRE_TYPE_LW)
        # directivity
        pdirectivity = cy.declare(cy.pointer(SourceDirectivityInterface))
        pdirectivity = new SphericalSourceDirectivity()
        source_idx = self.thisptr.get().make_source(pos, spec.toGPhy(), pdirectivity)
        return source_idx

    @property
    def nsources(self):
        """ Return the number of acoustic sources involved in the acoustic
            problem model
        """
        assert self.thisptr.get() != NULL
        return self.thisptr.get().nsources()

    @property
    def nreceptors(self):
        """ Return the number of acoustic receptors involved in the acoustic
            problem model
        """
        assert self.thisptr.get() != NULL
        return self.thisptr.get().nreceptors()

    def source(self, idx):
        """ Return the acoustic source (SolverSource object) of index 'idx'
        """
        assert self.thisptr.get() != NULL
        source = SolverSource()
        source.thisptr = cy.address(self.thisptr.get().source(idx))
        return source

    @property
    def sources(self):
        """ Return all the acoustic sources of the model
        """
        assert self.thisptr.get() != NULL
        sources = []
        for i in xrange(self.nsources):
            source = SolverSource()
            source.thisptr = cy.address(self.thisptr.get().source(i))
            sources.append(source)
        return sources

    def receptor(self, idx):
        """ Return the acoustic receptor (SolverReceptor object) of index 'idx'
        """
        assert self.thisptr.get() != NULL
        receptor = SolverReceptor()
        receptor.thisptr = cy.address(self.thisptr.get().receptor(idx))
        return receptor

    @property
    def receptors(self):
        """ Return all the acoustic receptors of the model
        """
        assert self.thisptr.get() != NULL
        receptors = []
        for i in xrange(self.nreceptors):
            receptor = SolverReceptor()
            receptor.thisptr = cy.address(self.thisptr.get().receptor(i))
            receptors.append(receptor)
        return receptors

    def export_triangular_mesh(self):
        """ Build a triangular mesh from the acoustic problem model.
            Return two nparrays:
                * 'nodes': an array of nodes (of dimension 'npoints'X3), where
                each line stands for a node and contains 3 coordinates)
                * 'triangles': an array of triangles (of dimension 'ntriangles'X3),
                where each line stands for a triangle and contains the indices of
                its 3 vertices in the 'nodes' array.
        """
        assert self.thisptr.get() != NULL
        nb_elts = cy.declare(cy.uint)
        actri = cy.declare(cy.pointer(AcousticTriangle))
        nb_elts = self.thisptr.get().ntriangles()
        triangles = np.empty([nb_elts, 3], dtype=int)
        for i in xrange(nb_elts):
            actri = cy.address(self.thisptr.get().triangle(i))
            triangles[i] = [actri.n[0], actri.n[1], actri.n[2]]
        point = cy.declare(cy.pointer(tycommon.OPoint3D))
        nb_elts = self.thisptr.get().npoints()
        nodes = np.empty([nb_elts, 3])
        for i in xrange(nb_elts):
            point = cy.address(self.thisptr.get().node(i))
            nodes[i] = [point._x, point._y, point._z]
        return (nodes, triangles)


cdef class ResultModel:

    def __cinit__(self):
        self.thisptr = shared_ptr[AcousticResultModel](new AcousticResultModel())

    def spectrum(self, id_receptor, id_source):
        spec = cy.declare(tycommon.OSpectre)
        spec = self.thisptr.get().get_data().element(id_receptor, id_source)
        return tycommon.ospectre2spectrum(spec)


cdef class Solver:

    @cy.locals(model=ProblemModel)
    @cy.returns((bool, ResultModel))
    def solve_problem(self, model):
        """Solve the acoustic problem by calling the underlying
        SolverInterface::solve() C++ method.

        Raises a RuntimeError in case of computation failure.
        """
        result = ResultModel()
        if not self.thisptr.solve(model.thisptr.get()[0],
                                  result.thisptr.get()[0]):
            raise RuntimeError(
                'Computation failed (C++ SolverInterface::solve() method '
                'returned false)')
        return result

    def purge(self):
        self.thisptr.purge()


cdef class SolverSource:
    thisptr = cy.declare(cy.pointer(AcousticSource))

    def __cinit__(self):
        self.thisptr = NULL

    @property
    def position(self):
        """ Return the acoustic source position (as a 'Point3D' object)
        """
        assert self.thisptr != NULL
        return tycommon.opoint3d2point3d(self.thisptr.position)

    @property
    def spectrum(self):
        """ Return the acoustic spectrum of the Source (dB scale, power spectrum)
        """
        assert self.thisptr != NULL
        return tycommon.ospectre2spectrum(self.thisptr.spectrum)


cdef class SolverReceptor:
    thisptr = cy.declare(cy.pointer(AcousticReceptor))

    def __cinit__(self):
        self.thisptr = NULL

    @property
    def position(self):
        """ Return the acoustic source position (as a 'Point3D' object)
        """
        assert self.thisptr != NULL
        return tycommon.opoint3d2point3d(self.thisptr.position)


cdef class Configuration:
    thisptr = cy.declare(SmartPtr[SolverConfiguration])

    @staticmethod
    def get():
        config = Configuration()
        config.thisptr = get()
        return config

    def getAtmosPressure(self):
        return self.thisptr.getRealPointer().AtmosPressure
    def setAtmosPressure(self, value):
        self.thisptr.getRealPointer().AtmosPressure = value
    AtmosPressure = property(getAtmosPressure, setAtmosPressure)
    def getAtmosTemperature(self):
        return self.thisptr.getRealPointer().AtmosTemperature
    def setAtmosTemperature(self, value):
        self.thisptr.getRealPointer().AtmosTemperature = value
    AtmosTemperature = property(getAtmosTemperature, setAtmosTemperature)
    def getAtmosHygrometry(self):
        return self.thisptr.getRealPointer().AtmosHygrometry
    def setAtmosHygrometry(self, value):
        self.thisptr.getRealPointer().AtmosHygrometry = value
    AtmosHygrometry = property(getAtmosHygrometry, setAtmosHygrometry)
    def getAnalyticC0(self):
        return self.thisptr.getRealPointer().AnalyticC0
    def setAnalyticC0(self, value):
        self.thisptr.getRealPointer().AnalyticC0 = value
    AnalyticC0 = property(getAnalyticC0, setAnalyticC0)
    def getWindDirection(self):
        return self.thisptr.getRealPointer().WindDirection
    def setWindDirection(self, value):
        self.thisptr.getRealPointer().WindDirection = value
    WindDirection = property(getWindDirection, setWindDirection)
    def getAnalyticGradC(self):
        return self.thisptr.getRealPointer().AnalyticGradC
    def setAnalyticGradC(self, value):
        self.thisptr.getRealPointer().AnalyticGradC = value
    AnalyticGradC = property(getAnalyticGradC, setAnalyticGradC)
    def getAnalyticGradV(self):
        return self.thisptr.getRealPointer().AnalyticGradV
    def setAnalyticGradV(self, value):
        self.thisptr.getRealPointer().AnalyticGradV = value
    AnalyticGradV = property(getAnalyticGradV, setAnalyticGradV)
    def getRayTracingOrder(self):
        return self.thisptr.getRealPointer().RayTracingOrder
    def setRayTracingOrder(self, value):
        self.thisptr.getRealPointer().RayTracingOrder = value
    RayTracingOrder = property(getRayTracingOrder, setRayTracingOrder)
    def getDiscretization(self):
        return self.thisptr.getRealPointer().Discretization
    def setDiscretization(self, value):
        self.thisptr.getRealPointer().Discretization = value
    Discretization = property(getDiscretization, setDiscretization)
    def getNbRaysPerSource(self):
        return self.thisptr.getRealPointer().NbRaysPerSource
    def setNbRaysPerSource(self, value):
        self.thisptr.getRealPointer().NbRaysPerSource = value
    NbRaysPerSource = property(getNbRaysPerSource, setNbRaysPerSource)
    def getMaxLength(self):
        return self.thisptr.getRealPointer().MaxLength
    def setMaxLength(self, value):
        self.thisptr.getRealPointer().MaxLength = value
    MaxLength = property(getMaxLength, setMaxLength)
    def getSizeReceiver(self):
        return self.thisptr.getRealPointer().SizeReceiver
    def setSizeReceiver(self, value):
        self.thisptr.getRealPointer().SizeReceiver = value
    SizeReceiver = property(getSizeReceiver, setSizeReceiver)
    def getAccelerator(self):
        return self.thisptr.getRealPointer().Accelerator
    def setAccelerator(self, value):
        self.thisptr.getRealPointer().Accelerator = value
    Accelerator = property(getAccelerator, setAccelerator)
    def getMaxTreeDepth(self):
        return self.thisptr.getRealPointer().MaxTreeDepth
    def setMaxTreeDepth(self, value):
        self.thisptr.getRealPointer().MaxTreeDepth = value
    MaxTreeDepth = property(getMaxTreeDepth, setMaxTreeDepth)
    def getAngleDiffMin(self):
        return self.thisptr.getRealPointer().AngleDiffMin
    def setAngleDiffMin(self, value):
        self.thisptr.getRealPointer().AngleDiffMin = value
    AngleDiffMin = property(getAngleDiffMin, setAngleDiffMin)
    def getCylindreThick(self):
        return self.thisptr.getRealPointer().CylindreThick
    def setCylindreThick(self, value):
        self.thisptr.getRealPointer().CylindreThick = value
    CylindreThick = property(getCylindreThick, setCylindreThick)
    def getMaxProfondeur(self):
        return self.thisptr.getRealPointer().MaxProfondeur
    def setMaxProfondeur(self, value):
        self.thisptr.getRealPointer().MaxProfondeur = value
    MaxProfondeur = property(getMaxProfondeur, setMaxProfondeur)
    def getUseSol(self):
        return self.thisptr.getRealPointer().UseSol
    def setUseSol(self, value):
        self.thisptr.getRealPointer().UseSol = value
    UseSol = property(getUseSol, setUseSol)
    def getMaxReflexion(self):
        return self.thisptr.getRealPointer().MaxReflexion
    def setMaxReflexion(self, value):
        self.thisptr.getRealPointer().MaxReflexion = value
    MaxReflexion = property(getMaxReflexion, setMaxReflexion)
    def getMaxDiffraction(self):
        return self.thisptr.getRealPointer().MaxDiffraction
    def setMaxDiffraction(self, value):
        self.thisptr.getRealPointer().MaxDiffraction = value
    MaxDiffraction = property(getMaxDiffraction, setMaxDiffraction)
    def getDiffractionUseRandomSampler(self):
        return self.thisptr.getRealPointer().DiffractionUseRandomSampler
    def setDiffractionUseRandomSampler(self, value):
        self.thisptr.getRealPointer().DiffractionUseRandomSampler = value
    DiffractionUseRandomSampler = property(getDiffractionUseRandomSampler, setDiffractionUseRandomSampler)
    def getNbRayWithDiffraction(self):
        return self.thisptr.getRealPointer().NbRayWithDiffraction
    def setNbRayWithDiffraction(self, value):
        self.thisptr.getRealPointer().NbRayWithDiffraction = value
    NbRayWithDiffraction = property(getNbRayWithDiffraction, setNbRayWithDiffraction)
    def getDiffractionDropDownNbRays(self):
        return self.thisptr.getRealPointer().DiffractionDropDownNbRays
    def setDiffractionDropDownNbRays(self, value):
        self.thisptr.getRealPointer().DiffractionDropDownNbRays = value
    DiffractionDropDownNbRays = property(getDiffractionDropDownNbRays, setDiffractionDropDownNbRays)
    def getDiffractionFilterRayAtCreation(self):
        return self.thisptr.getRealPointer().DiffractionFilterRayAtCreation
    def setDiffractionFilterRayAtCreation(self, value):
        self.thisptr.getRealPointer().DiffractionFilterRayAtCreation = value
    DiffractionFilterRayAtCreation = property(getDiffractionFilterRayAtCreation, setDiffractionFilterRayAtCreation)
    def getUsePathDifValidation(self):
        return self.thisptr.getRealPointer().UsePathDifValidation
    def setUsePathDifValidation(self, value):
        self.thisptr.getRealPointer().UsePathDifValidation = value
    UsePathDifValidation = property(getUsePathDifValidation, setUsePathDifValidation)
    def getMaxPathDifference(self):
        return self.thisptr.getRealPointer().MaxPathDifference
    def setMaxPathDifference(self, value):
        self.thisptr.getRealPointer().MaxPathDifference = value
    MaxPathDifference = property(getMaxPathDifference, setMaxPathDifference)
    def getDiffractionUseDistanceAsFilter(self):
        return self.thisptr.getRealPointer().DiffractionUseDistanceAsFilter
    def setDiffractionUseDistanceAsFilter(self, value):
        self.thisptr.getRealPointer().DiffractionUseDistanceAsFilter = value
    DiffractionUseDistanceAsFilter = property(getDiffractionUseDistanceAsFilter, setDiffractionUseDistanceAsFilter)
    def getKeepDebugRay(self):
        return self.thisptr.getRealPointer().KeepDebugRay
    def setKeepDebugRay(self, value):
        self.thisptr.getRealPointer().KeepDebugRay = value
    KeepDebugRay = property(getKeepDebugRay, setKeepDebugRay)
    def getUsePostFilters(self):
        return self.thisptr.getRealPointer().UsePostFilters
    def setUsePostFilters(self, value):
        self.thisptr.getRealPointer().UsePostFilters = value
    UsePostFilters = property(getUsePostFilters, setUsePostFilters)
    def getEnableFullTargets(self):
        return self.thisptr.getRealPointer().EnableFullTargets
    def setEnableFullTargets(self, value):
        self.thisptr.getRealPointer().EnableFullTargets = value
    EnableFullTargets = property(getEnableFullTargets, setEnableFullTargets)
    def getTargetsDensity(self):
        return self.thisptr.getRealPointer().TargetsDensity
    def setTargetsDensity(self, value):
        self.thisptr.getRealPointer().TargetsDensity = value
    TargetsDensity = property(getTargetsDensity, setTargetsDensity)
    def getInitialAngleTheta(self):
        return self.thisptr.getRealPointer().InitialAngleTheta
    def setInitialAngleTheta(self, value):
        self.thisptr.getRealPointer().InitialAngleTheta = value
    InitialAngleTheta = property(getInitialAngleTheta, setInitialAngleTheta)
    def getInitialAnglePhi(self):
        return self.thisptr.getRealPointer().InitialAnglePhi
    def setInitialAnglePhi(self, value):
        self.thisptr.getRealPointer().InitialAnglePhi = value
    InitialAnglePhi = property(getInitialAnglePhi, setInitialAnglePhi)
    def getAnalyticNbRay(self):
        return self.thisptr.getRealPointer().AnalyticNbRay
    def setAnalyticNbRay(self, value):
        self.thisptr.getRealPointer().AnalyticNbRay = value
    AnalyticNbRay = property(getAnalyticNbRay, setAnalyticNbRay)
    def getAnalyticTMax(self):
        return self.thisptr.getRealPointer().AnalyticTMax
    def setAnalyticTMax(self, value):
        self.thisptr.getRealPointer().AnalyticTMax = value
    AnalyticTMax = property(getAnalyticTMax, setAnalyticTMax)
    def getAnalyticH(self):
        return self.thisptr.getRealPointer().AnalyticH
    def setAnalyticH(self, value):
        self.thisptr.getRealPointer().AnalyticH = value
    AnalyticH = property(getAnalyticH, setAnalyticH)
    def getAnalyticDMax(self):
        return self.thisptr.getRealPointer().AnalyticDMax
    def setAnalyticDMax(self, value):
        self.thisptr.getRealPointer().AnalyticDMax = value
    AnalyticDMax = property(getAnalyticDMax, setAnalyticDMax)
    def getAnalyticTypeTransfo(self):
        return self.thisptr.getRealPointer().AnalyticTypeTransfo
    def setAnalyticTypeTransfo(self, value):
        self.thisptr.getRealPointer().AnalyticTypeTransfo = value
    AnalyticTypeTransfo = property(getAnalyticTypeTransfo, setAnalyticTypeTransfo)
    def getNbThreads(self):
        return self.thisptr.getRealPointer().NbThreads
    def setNbThreads(self, value):
        self.thisptr.getRealPointer().NbThreads = value
    NbThreads = property(getNbThreads, setNbThreads)
    def getUseRealGround(self):
        return self.thisptr.getRealPointer().UseRealGround
    def setUseRealGround(self, value):
        self.thisptr.getRealPointer().UseRealGround = value
    UseRealGround = property(getUseRealGround, setUseRealGround)
    def getUseLateralDiffraction(self):
        return self.thisptr.getRealPointer().UseLateralDiffraction
    def setUseLateralDiffraction(self, value):
        self.thisptr.getRealPointer().UseLateralDiffraction = value
    UseLateralDiffraction = property(getUseLateralDiffraction, setUseLateralDiffraction)
    def getUseReflection(self):
        return self.thisptr.getRealPointer().UseReflection
    def setUseReflection(self, value):
        self.thisptr.getRealPointer().UseReflection = value
    UseReflection = property(getUseReflection, setUseReflection)
    def getPropaConditions(self):
        return self.thisptr.getRealPointer().PropaConditions
    def setPropaConditions(self, value):
        self.thisptr.getRealPointer().PropaConditions = value
    PropaConditions = property(getPropaConditions, setPropaConditions)
    def getH1parameter(self):
        return self.thisptr.getRealPointer().H1parameter
    def setH1parameter(self, value):
        self.thisptr.getRealPointer().H1parameter = value
    H1parameter = property(getH1parameter, setH1parameter)
    def getModSummation(self):
        return self.thisptr.getRealPointer().ModSummation
    def setModSummation(self, value):
        self.thisptr.getRealPointer().ModSummation = value
    ModSummation = property(getModSummation, setModSummation)
    def getUseMeteo(self):
        return self.thisptr.getRealPointer().UseMeteo
    def setUseMeteo(self, value):
        self.thisptr.getRealPointer().UseMeteo = value
    UseMeteo = property(getUseMeteo, setUseMeteo)
    def getOverSampleD(self):
        return self.thisptr.getRealPointer().OverSampleD
    def setOverSampleD(self, value):
        self.thisptr.getRealPointer().OverSampleD = value
    OverSampleD = property(getOverSampleD, setOverSampleD)
    def getUseFresnelArea(self):
        return self.thisptr.getRealPointer().UseFresnelArea
    def setUseFresnelArea(self, value):
        self.thisptr.getRealPointer().UseFresnelArea = value
    UseFresnelArea = property(getUseFresnelArea, setUseFresnelArea)
    def getAnime3DSigma(self):
        return self.thisptr.getRealPointer().Anime3DSigma
    def setAnime3DSigma(self, value):
        self.thisptr.getRealPointer().Anime3DSigma = value
    Anime3DSigma = property(getAnime3DSigma, setAnime3DSigma)
    def getAnime3DForceC(self):
        return self.thisptr.getRealPointer().Anime3DForceC
    def setAnime3DForceC(self, value):
        self.thisptr.getRealPointer().Anime3DForceC = value
    Anime3DForceC = property(getAnime3DForceC, setAnime3DForceC)
    def getDebugUseCloseEventSelector(self):
        return self.thisptr.getRealPointer().DebugUseCloseEventSelector
    def setDebugUseCloseEventSelector(self, value):
        self.thisptr.getRealPointer().DebugUseCloseEventSelector = value
    DebugUseCloseEventSelector = property(getDebugUseCloseEventSelector, setDebugUseCloseEventSelector)
    def getDebugUseDiffractionAngleSelector(self):
        return self.thisptr.getRealPointer().DebugUseDiffractionAngleSelector
    def setDebugUseDiffractionAngleSelector(self, value):
        self.thisptr.getRealPointer().DebugUseDiffractionAngleSelector = value
    DebugUseDiffractionAngleSelector = property(getDebugUseDiffractionAngleSelector, setDebugUseDiffractionAngleSelector)
    def getDebugUseDiffractionPathSelector(self):
        return self.thisptr.getRealPointer().DebugUseDiffractionPathSelector
    def setDebugUseDiffractionPathSelector(self, value):
        self.thisptr.getRealPointer().DebugUseDiffractionPathSelector = value
    DebugUseDiffractionPathSelector = property(getDebugUseDiffractionPathSelector, setDebugUseDiffractionPathSelector)
    def getDebugUseFermatSelector(self):
        return self.thisptr.getRealPointer().DebugUseFermatSelector
    def setDebugUseFermatSelector(self, value):
        self.thisptr.getRealPointer().DebugUseFermatSelector = value
    DebugUseFermatSelector = property(getDebugUseFermatSelector, setDebugUseFermatSelector)
    def getDebugUseFaceSelector(self):
        return self.thisptr.getRealPointer().DebugUseFaceSelector
    def setDebugUseFaceSelector(self, value):
        self.thisptr.getRealPointer().DebugUseFaceSelector = value
    DebugUseFaceSelector = property(getDebugUseFaceSelector, setDebugUseFaceSelector)