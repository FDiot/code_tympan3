"""THIS FILE IS GENERATED, DON'T EDIT IT"""
"""solver models for Code_TYMPAN
"""
from itertools import zip_longest

import cython as cy
import numpy as np
cimport numpy as np
from libcpp.deque cimport deque

from tympan.models cimport _common as tycommon
from tympan._core cimport unique_ptr, shared_ptr


cdef class ProblemModel:
    """Solver model"""

    def __cinit__(self):
        self.thisptr = shared_ptr[AcousticProblemModel](new AcousticProblemModel())

    @property
    def npoints(self):
        """Return the number of mesh nodes of the model"""
        assert self.thisptr.get() != NULL
        return self.thisptr.get().npoints()

    @property
    def ntriangles(self):
        """Return the number of mesh triangles of the model"""
        assert self.thisptr.get() != NULL
        return self.thisptr.get().ntriangles()

    @property
    def nmaterials(self):
        """Return the number of acoustic materials of the model"""
        assert self.thisptr.get() != NULL
        return self.thisptr.get().nmaterials()

    def add_node(self, x, y, z):
        """Add a node of double coordinates (x, y, z) to the model"""
        assert self.thisptr.get() != NULL
        node = cy.declare(tycommon.OPoint3D, tycommon.OPoint3D(x, y, z))
        return self.thisptr.get().make_node(node)

    def add_triangle(self, n1, n2, n3, material_info=None):
        """Add a triangle of node indices (n1, n2, n3) to the model"""
        assert self.thisptr.get() != NULL
        tri_idx = self.thisptr.get().make_triangle(n1, n2, n3)
        if material_info is not None:
            self._set_triangle_material(tri_idx, *material_info)
        return tri_idx

    def _set_triangle_material(self, idx, name, resistivity, deviation,
                               length):
        """Associate a material with specified properties to an acoustic
        triangle at `idx`.
        """
        actri = cy.declare(cy.pointer(AcousticTriangle),
                           cy.address(self.thisptr.get().triangle(idx)))
        material = cy.declare(shared_ptr[AcousticMaterialBase],
                              self.thisptr.get().make_material(
                                  name, resistivity, deviation, length))
        actri.made_of = material

    def add_mesh(self, points, triangles, materials=()):
        """Add a mesh to the solver model.

        :param points: sequence of `Point3d`
        :param triangles: sequence of `Triangle`

        :return: (nodes, triangles) Numpy arrays with the indices of these
            nodes and triangles in the model once created.
        """
        nodes = np.array([self.add_node(p.x, p.y, p.z) for p in points])
        triangles = np.array([self.add_triangle(nodes[t.p1], nodes[t.p2], nodes[t.p3], m)
                              for t, m in zip_longest(triangles, materials)])
        return nodes, triangles

    @cy.locals(spectrum_values=np.ndarray)
    def _add_source(self, position, spectrum_values, shift):
        """Add an acoustic source to the model"""
        # position
        pos = cy.declare(tycommon.OPoint3D,
                         tycommon.OPoint3D(position[0], position[1], position[2]))
        # spectrum
        nb_val = spectrum_values.shape[0]
        spec = cy.declare(tycommon.OSpectre,
                          tycommon.OSpectre(<double *> spectrum_values.data, nb_val, shift))
        spec.setEtat(tycommon.SPECTRE_ETAT_DB)
        spec.setType(tycommon.SPECTRE_TYPE_LW)
        # directivity
        pdirectivity = cy.declare(cy.pointer(SourceDirectivityInterface),
                                  new SphericalSourceDirectivity())
        source_idx = self.thisptr.get().make_source(pos, spec.toGPhy(), pdirectivity)
        return source_idx

    @property
    def nsources(self):
        """Return the number of acoustic sources involved in the model"""
        assert self.thisptr.get() != NULL
        return self.thisptr.get().nsources()

    @property
    def nreceptors(self):
        """Return the number of acoustic receptors involved in the model"""
        assert self.thisptr.get() != NULL
        return self.thisptr.get().nreceptors()

    def source(self, idx):
        """Return the acoustic source (SolverSource object) of index 'idx'"""
        assert self.thisptr.get() != NULL
        source = SolverSource()
        source.thisptr = cy.address(self.thisptr.get().source(idx))
        return source

    @property
    def sources(self):
        """Return all the acoustic sources of the model"""
        assert self.thisptr.get() != NULL
        sources = []
        for i in xrange(self.nsources):
            source = SolverSource()
            source.thisptr = cy.address(self.thisptr.get().source(i))
            sources.append(source)
        return sources

    def receptor(self, idx):
        """Return the acoustic receptor (SolverReceptor object) of index 'idx'
        """
        assert self.thisptr.get() != NULL
        receptor = SolverReceptor()
        receptor.thisptr = cy.address(self.thisptr.get().receptor(idx))
        return receptor

    @property
    def receptors(self):
        """Return all the acoustic receptors of the model"""
        assert self.thisptr.get() != NULL
        receptors = []
        for i in xrange(self.nreceptors):
            receptor = SolverReceptor()
            receptor.thisptr = cy.address(self.thisptr.get().receptor(i))
            receptors.append(receptor)
        return receptors

    @property
    def triangles(self):
        """All the acoustic triangles of the model"""
        assert self.thisptr.get() != NULL
        triangles = []
        for i in xrange(self.ntriangles):
            triangle = MeshTriangle()
            triangle.thisptr = cy.address(self.thisptr.get().triangle(i))
            triangles.append(triangle)
        return triangles

    @cy.locals(source=tycommon.Point3D, receptor=tycommon.Point3D)
    def fresnel_zone_intersection(self, l, h, source, receptor):
        """Return the indices of the acoustic triangles of the model that are intersected by
        the non iso-oriented box `box`

        The box is an approximation for the Fresnel zone between a source and a receptor
        """
        triangles = cy.declare(deque[AcousticTriangle], self.thisptr.get().triangles())
        nodes = cy.declare(deque[tycommon.OPoint3D], self.thisptr.get().nodes())
        intersected = scene_volume_intersection(triangles, nodes, l, h, source.thisobj,
                                                receptor.thisobj)
        return [idx for idx in intersected]

    def _export_triangular_mesh(self):
        """Build a triangular mesh from the acoustic problem model"""
        assert self.thisptr.get() != NULL
        actri = cy.declare(cy.pointer(AcousticTriangle))
        nb_elts = cy.declare(cy.uint, self.thisptr.get().ntriangles())
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

    def node_coords(self, idx):
        """Return a tuple with the 3D coordinates for the node of id 'idx'"""
        assert self.thisptr.get() != NULL
        _node = cy.declare(tycommon.OPoint3D, self.thisptr.get().node(idx))
        return _node._x, _node._y, _node._z


cdef class ResultModel:
    """Results of a computation ran by a solver on a solver model"""

    def __cinit__(self):
        self.thisptr = shared_ptr[AcousticResultModel](new AcousticResultModel())

    def spectrum(self, id_receptor, id_source):
        """Return the power spectrum received by a receptor from a source
        """
        spec = cy.declare(tycommon.OSpectre,
                          self.thisptr.get().get_data().element(id_receptor, id_source))
        return tycommon.ospectre2spectrum(spec)

    @property
    def nreceptors(self):
        return self.thisptr.get().get_data().nb_receptors()

    @property
    def nsources(self):
        return self.thisptr.get().get_data().nb_sources()

    def spectra(self):
        """Return a 3D numpy array containing the result spectra per receptor and per source
        (dimensions: (nreceptors, nsources, 31)). If no sources or no receptors, return None
        """
        if self.nsources == 0 or self.nreceptors == 0:
            return None
        _spectra = np.empty((self.nreceptors, self.nsources, 31))
        for rec in xrange(self.nreceptors):
            _spectra[rec, :, :]  =  np.vstack(self.spectrum(rec, source).values
                                              for source in xrange(self.nsources))
        return _spectra

    def combined_spectra(self):
        """Return a 2D numpy array of combined sources spectra values per receptor (or None if no
        receptors)"""
        if self.nreceptors == 0:
            return None
        if self.nsources == 0:
            return np.zeros((self.nreceptors, 31))
        return self.spectra().sum(axis=1)


cdef class Solver:

    @cy.locals(model=ProblemModel)
    @cy.returns((bool, ResultModel))
    def solve_problem(self, model):
        """Run a computation based on the solver model given in argument

        Raises a RuntimeError in case of computation failure.
        """
        result = ResultModel()
        if not self.thisptr.solve(model.thisptr.get()[0],
                                  result.thisptr.get()[0], get()):
            raise RuntimeError(
                'Computation failed (C++ SolverInterface::solve() method '
                'returned false)')
        return result

    def purge(self):
        """Purge solver from its previous results"""
        self.thisptr.purge()


cdef class SolverSource:
    thisptr = cy.declare(cy.pointer(AcousticSource))

    def __cinit__(self):
        self.thisptr = NULL

    @property
    def position(self):
        """Return the acoustic source position (as a 'Point3D' object)"""
        assert self.thisptr != NULL
        return tycommon.opoint3d2point3d(self.thisptr.position)

    @property
    def spectrum(self):
        """Return the acoustic spectrum of the Source (dB scale, power spectrum)
        """
        assert self.thisptr != NULL
        return tycommon.ospectre2spectrum(self.thisptr.spectrum)

    @property
    def directivity_vector(self):
        """Source directivity vector in the global frame"""
        assert self.thisptr != NULL
        # Check the directivity of the source is a CommonFaceDirectivity directivity
        cf_dir = cy.declare(cy.pointer(CommonFaceDirectivity),
                            dynamic_cast_commonface_dir(self.thisptr.directivity))
        if cf_dir == NULL:
            raise ValueError("The directivity of this source has no support normal vector")
        # return its support normal vector
        return tycommon.ovector3d2vector3d(cf_dir.get_normal())

    def value(self, freq):
        """The spectrum value corresponding to the 'freq' frequency (linear, power)"""
        return self.thisptr.spectrum.getValueReal(freq)

    property volume_id:
        """identifier of the volume containing the source"""
        def __get__(self):
            assert self.thisptr != NULL
            return self.thisptr.volume_id.decode()
        def __set__(self, volume_id):
            assert self.thisptr != NULL
            self.thisptr.volume_id = volume_id.encode('utf-8')

    property face_id:
        """identifier of the face of the volume containing the source"""
        def __get__(self):
            assert self.thisptr != NULL
            return self.thisptr.face_id.decode()
        def __set__(self, face_id):
            assert self.thisptr != NULL
            self.thisptr.face_id = face_id.encode('utf-8')


cdef class SolverReceptor:
    thisptr = cy.declare(cy.pointer(AcousticReceptor))

    def __cinit__(self):
        self.thisptr = NULL

    @property
    def position(self):
        """Return the acoustic source position (as a 'Point3D' object)"""
        assert self.thisptr != NULL
        return tycommon.opoint3d2point3d(self.thisptr.position)


cdef class MeshTriangle:
    thisptr = cy.declare(cy.pointer(AcousticTriangle))

    def __cinit__(self):
        self.thisptr = NULL

    @property
    def volume_id(self):
        """Identifier of the volume containing the triangle"""
        assert self.thisptr != NULL
        if not self.thisptr.volume_id.empty():
            return self.thisptr.volume_id.decode()
        return None

    @property
    def nodes(self):
        """Tuple containing the ids of the 3 nodes of the triangle"""
        assert self.thisptr != NULL
        return self.thisptr.n[0], self.thisptr.n[1], self.thisptr.n[2]


cdef class Configuration:
    thisptr = cy.declare(SmartPtr[SolverConfiguration])

    @staticmethod
    def get():
        config = Configuration()
        config.thisptr = get()
        return config

    def getCylindreThick(self):
        return self.thisptr.getRealPointer().CylindreThick
    def setCylindreThick(self, value):
        self.thisptr.getRealPointer().CylindreThick = value
    CylindreThick = property(getCylindreThick, setCylindreThick)
    def getDiffractionDropDownNbRays(self):
        return self.thisptr.getRealPointer().DiffractionDropDownNbRays
    def setDiffractionDropDownNbRays(self, value):
        self.thisptr.getRealPointer().DiffractionDropDownNbRays = value
    DiffractionDropDownNbRays = property(getDiffractionDropDownNbRays, setDiffractionDropDownNbRays)
    def getKeepDebugRay(self):
        return self.thisptr.getRealPointer().KeepDebugRay
    def setKeepDebugRay(self, value):
        self.thisptr.getRealPointer().KeepDebugRay = value
    KeepDebugRay = property(getKeepDebugRay, setKeepDebugRay)
    def getMaxPathDifference(self):
        return self.thisptr.getRealPointer().MaxPathDifference
    def setMaxPathDifference(self, value):
        self.thisptr.getRealPointer().MaxPathDifference = value
    MaxPathDifference = property(getMaxPathDifference, setMaxPathDifference)
    def getSizeReceiver(self):
        return self.thisptr.getRealPointer().SizeReceiver
    def setSizeReceiver(self, value):
        self.thisptr.getRealPointer().SizeReceiver = value
    SizeReceiver = property(getSizeReceiver, setSizeReceiver)
    def getDiffractionFilterRayAtCreation(self):
        return self.thisptr.getRealPointer().DiffractionFilterRayAtCreation
    def setDiffractionFilterRayAtCreation(self, value):
        self.thisptr.getRealPointer().DiffractionFilterRayAtCreation = value
    DiffractionFilterRayAtCreation = property(getDiffractionFilterRayAtCreation, setDiffractionFilterRayAtCreation)
    def getMaxReflexion(self):
        return self.thisptr.getRealPointer().MaxReflexion
    def setMaxReflexion(self, value):
        self.thisptr.getRealPointer().MaxReflexion = value
    MaxReflexion = property(getMaxReflexion, setMaxReflexion)
    def getMaxProfondeur(self):
        return self.thisptr.getRealPointer().MaxProfondeur
    def setMaxProfondeur(self, value):
        self.thisptr.getRealPointer().MaxProfondeur = value
    MaxProfondeur = property(getMaxProfondeur, setMaxProfondeur)
    def getMaxDiffraction(self):
        return self.thisptr.getRealPointer().MaxDiffraction
    def setMaxDiffraction(self, value):
        self.thisptr.getRealPointer().MaxDiffraction = value
    MaxDiffraction = property(getMaxDiffraction, setMaxDiffraction)
    def getMaxTreeDepth(self):
        return self.thisptr.getRealPointer().MaxTreeDepth
    def setMaxTreeDepth(self, value):
        self.thisptr.getRealPointer().MaxTreeDepth = value
    MaxTreeDepth = property(getMaxTreeDepth, setMaxTreeDepth)
    def getUseSol(self):
        return self.thisptr.getRealPointer().UseSol
    def setUseSol(self, value):
        self.thisptr.getRealPointer().UseSol = value
    UseSol = property(getUseSol, setUseSol)
    def getDiffractionUseDistanceAsFilter(self):
        return self.thisptr.getRealPointer().DiffractionUseDistanceAsFilter
    def setDiffractionUseDistanceAsFilter(self, value):
        self.thisptr.getRealPointer().DiffractionUseDistanceAsFilter = value
    DiffractionUseDistanceAsFilter = property(getDiffractionUseDistanceAsFilter, setDiffractionUseDistanceAsFilter)
    def getRayTracingOrder(self):
        return self.thisptr.getRealPointer().RayTracingOrder
    def setRayTracingOrder(self, value):
        self.thisptr.getRealPointer().RayTracingOrder = value
    RayTracingOrder = property(getRayTracingOrder, setRayTracingOrder)
    def getDiffractionUseRandomSampler(self):
        return self.thisptr.getRealPointer().DiffractionUseRandomSampler
    def setDiffractionUseRandomSampler(self, value):
        self.thisptr.getRealPointer().DiffractionUseRandomSampler = value
    DiffractionUseRandomSampler = property(getDiffractionUseRandomSampler, setDiffractionUseRandomSampler)
    def getUsePathDifValidation(self):
        return self.thisptr.getRealPointer().UsePathDifValidation
    def setUsePathDifValidation(self, value):
        self.thisptr.getRealPointer().UsePathDifValidation = value
    UsePathDifValidation = property(getUsePathDifValidation, setUsePathDifValidation)
    def getDiscretization(self):
        return self.thisptr.getRealPointer().Discretization
    def setDiscretization(self, value):
        self.thisptr.getRealPointer().Discretization = value
    Discretization = property(getDiscretization, setDiscretization)
    def getAccelerator(self):
        return self.thisptr.getRealPointer().Accelerator
    def setAccelerator(self, value):
        self.thisptr.getRealPointer().Accelerator = value
    Accelerator = property(getAccelerator, setAccelerator)
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
    def getAngleDiffMin(self):
        return self.thisptr.getRealPointer().AngleDiffMin
    def setAngleDiffMin(self, value):
        self.thisptr.getRealPointer().AngleDiffMin = value
    AngleDiffMin = property(getAngleDiffMin, setAngleDiffMin)
    def getUsePostFilters(self):
        return self.thisptr.getRealPointer().UsePostFilters
    def setUsePostFilters(self, value):
        self.thisptr.getRealPointer().UsePostFilters = value
    UsePostFilters = property(getUsePostFilters, setUsePostFilters)
    def getNbRayWithDiffraction(self):
        return self.thisptr.getRealPointer().NbRayWithDiffraction
    def setNbRayWithDiffraction(self, value):
        self.thisptr.getRealPointer().NbRayWithDiffraction = value
    NbRayWithDiffraction = property(getNbRayWithDiffraction, setNbRayWithDiffraction)
    def getModSummation(self):
        return self.thisptr.getRealPointer().ModSummation
    def setModSummation(self, value):
        self.thisptr.getRealPointer().ModSummation = value
    ModSummation = property(getModSummation, setModSummation)
    def getUseLateralDiffraction(self):
        return self.thisptr.getRealPointer().UseLateralDiffraction
    def setUseLateralDiffraction(self, value):
        self.thisptr.getRealPointer().UseLateralDiffraction = value
    UseLateralDiffraction = property(getUseLateralDiffraction, setUseLateralDiffraction)
    def getUseRealGround(self):
        return self.thisptr.getRealPointer().UseRealGround
    def setUseRealGround(self, value):
        self.thisptr.getRealPointer().UseRealGround = value
    UseRealGround = property(getUseRealGround, setUseRealGround)
    def getNbThreads(self):
        return self.thisptr.getRealPointer().NbThreads
    def setNbThreads(self, value):
        self.thisptr.getRealPointer().NbThreads = value
    NbThreads = property(getNbThreads, setNbThreads)
    def getPropaConditions(self):
        return self.thisptr.getRealPointer().PropaConditions
    def setPropaConditions(self, value):
        self.thisptr.getRealPointer().PropaConditions = value
    PropaConditions = property(getPropaConditions, setPropaConditions)
    def getUseReflection(self):
        return self.thisptr.getRealPointer().UseReflection
    def setUseReflection(self, value):
        self.thisptr.getRealPointer().UseReflection = value
    UseReflection = property(getUseReflection, setUseReflection)
    def getUseScreen(self):
        return self.thisptr.getRealPointer().UseScreen
    def setUseScreen(self, value):
        self.thisptr.getRealPointer().UseScreen = value
    UseScreen = property(getUseScreen, setUseScreen)
    def getH1parameter(self):
        return self.thisptr.getRealPointer().H1parameter
    def setH1parameter(self, value):
        self.thisptr.getRealPointer().H1parameter = value
    H1parameter = property(getH1parameter, setH1parameter)
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
    def getAnalyticNbRay(self):
        return self.thisptr.getRealPointer().AnalyticNbRay
    def setAnalyticNbRay(self, value):
        self.thisptr.getRealPointer().AnalyticNbRay = value
    AnalyticNbRay = property(getAnalyticNbRay, setAnalyticNbRay)
    def getFinalAngleTheta(self):
        return self.thisptr.getRealPointer().FinalAngleTheta
    def setFinalAngleTheta(self, value):
        self.thisptr.getRealPointer().FinalAngleTheta = value
    FinalAngleTheta = property(getFinalAngleTheta, setFinalAngleTheta)
    def getAnalyticDMax(self):
        return self.thisptr.getRealPointer().AnalyticDMax
    def setAnalyticDMax(self, value):
        self.thisptr.getRealPointer().AnalyticDMax = value
    AnalyticDMax = property(getAnalyticDMax, setAnalyticDMax)
    def getInitialAnglePhi(self):
        return self.thisptr.getRealPointer().InitialAnglePhi
    def setInitialAnglePhi(self, value):
        self.thisptr.getRealPointer().InitialAnglePhi = value
    InitialAnglePhi = property(getInitialAnglePhi, setInitialAnglePhi)
    def getCurveRaySampler(self):
        return self.thisptr.getRealPointer().CurveRaySampler
    def setCurveRaySampler(self, value):
        self.thisptr.getRealPointer().CurveRaySampler = value
    CurveRaySampler = property(getCurveRaySampler, setCurveRaySampler)
    def getInitialAngleTheta(self):
        return self.thisptr.getRealPointer().InitialAngleTheta
    def setInitialAngleTheta(self, value):
        self.thisptr.getRealPointer().InitialAngleTheta = value
    InitialAngleTheta = property(getInitialAngleTheta, setInitialAngleTheta)
    def getFinalAnglePhi(self):
        return self.thisptr.getRealPointer().FinalAnglePhi
    def setFinalAnglePhi(self, value):
        self.thisptr.getRealPointer().FinalAnglePhi = value
    FinalAnglePhi = property(getFinalAnglePhi, setFinalAnglePhi)
    def getAtmosPressure(self):
        return self.thisptr.getRealPointer().AtmosPressure
    def setAtmosPressure(self, value):
        self.thisptr.getRealPointer().AtmosPressure = value
    AtmosPressure = property(getAtmosPressure, setAtmosPressure)
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
    def getAtmosHygrometry(self):
        return self.thisptr.getRealPointer().AtmosHygrometry
    def setAtmosHygrometry(self, value):
        self.thisptr.getRealPointer().AtmosHygrometry = value
    AtmosHygrometry = property(getAtmosHygrometry, setAtmosHygrometry)
    def getAtmosTemperature(self):
        return self.thisptr.getRealPointer().AtmosTemperature
    def setAtmosTemperature(self, value):
        self.thisptr.getRealPointer().AtmosTemperature = value
    AtmosTemperature = property(getAtmosTemperature, setAtmosTemperature)
    def getAnalyticGradV(self):
        return self.thisptr.getRealPointer().AnalyticGradV
    def setAnalyticGradV(self, value):
        self.thisptr.getRealPointer().AnalyticGradV = value
    AnalyticGradV = property(getAnalyticGradV, setAnalyticGradV)
    def getMinSRDistance(self):
        return self.thisptr.getRealPointer().MinSRDistance
    def setMinSRDistance(self, value):
        self.thisptr.getRealPointer().MinSRDistance = value
    MinSRDistance = property(getMinSRDistance, setMinSRDistance)
    def getMeshElementSizeMax(self):
        return self.thisptr.getRealPointer().MeshElementSizeMax
    def setMeshElementSizeMax(self, value):
        self.thisptr.getRealPointer().MeshElementSizeMax = value
    MeshElementSizeMax = property(getMeshElementSizeMax, setMeshElementSizeMax)
    def getshowScene(self):
        return self.thisptr.getRealPointer().showScene
    def setshowScene(self, value):
        self.thisptr.getRealPointer().showScene = value
    showScene = property(getshowScene, setshowScene)
    def getAnalyticTypeTransfo(self):
        return self.thisptr.getRealPointer().AnalyticTypeTransfo
    def setAnalyticTypeTransfo(self, value):
        self.thisptr.getRealPointer().AnalyticTypeTransfo = value
    AnalyticTypeTransfo = property(getAnalyticTypeTransfo, setAnalyticTypeTransfo)
    def getDebugUseDiffractionPathSelector(self):
        return self.thisptr.getRealPointer().DebugUseDiffractionPathSelector
    def setDebugUseDiffractionPathSelector(self, value):
        self.thisptr.getRealPointer().DebugUseDiffractionPathSelector = value
    DebugUseDiffractionPathSelector = property(getDebugUseDiffractionPathSelector, setDebugUseDiffractionPathSelector)
    def getDebugUseDiffractionAngleSelector(self):
        return self.thisptr.getRealPointer().DebugUseDiffractionAngleSelector
    def setDebugUseDiffractionAngleSelector(self, value):
        self.thisptr.getRealPointer().DebugUseDiffractionAngleSelector = value
    DebugUseDiffractionAngleSelector = property(getDebugUseDiffractionAngleSelector, setDebugUseDiffractionAngleSelector)
    def getDebugUseCloseEventSelector(self):
        return self.thisptr.getRealPointer().DebugUseCloseEventSelector
    def setDebugUseCloseEventSelector(self, value):
        self.thisptr.getRealPointer().DebugUseCloseEventSelector = value
    DebugUseCloseEventSelector = property(getDebugUseCloseEventSelector, setDebugUseCloseEventSelector)
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
    def getAnime3DSigma(self):
        return self.thisptr.getRealPointer().Anime3DSigma
    def setAnime3DSigma(self, value):
        self.thisptr.getRealPointer().Anime3DSigma = value
    Anime3DSigma = property(getAnime3DSigma, setAnime3DSigma)
    def getUseFresnelArea(self):
        return self.thisptr.getRealPointer().UseFresnelArea
    def setUseFresnelArea(self, value):
        self.thisptr.getRealPointer().UseFresnelArea = value
    UseFresnelArea = property(getUseFresnelArea, setUseFresnelArea)
    def getAnime3DForceC(self):
        return self.thisptr.getRealPointer().Anime3DForceC
    def setAnime3DForceC(self, value):
        self.thisptr.getRealPointer().Anime3DForceC = value
    Anime3DForceC = property(getAnime3DForceC, setAnime3DForceC)
    def getUseMeteo(self):
        return self.thisptr.getRealPointer().UseMeteo
    def setUseMeteo(self, value):
        self.thisptr.getRealPointer().UseMeteo = value
    UseMeteo = property(getUseMeteo, setUseMeteo)
    def getAnime3DKeepRays(self):
        return self.thisptr.getRealPointer().Anime3DKeepRays
    def setAnime3DKeepRays(self, value):
        self.thisptr.getRealPointer().Anime3DKeepRays = value
    Anime3DKeepRays = property(getAnime3DKeepRays, setAnime3DKeepRays)
