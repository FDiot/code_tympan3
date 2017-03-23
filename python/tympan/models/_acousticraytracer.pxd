"""Acoustic Ray Tracer from Code_TYMPAN, headers
"""
from libcpp cimport bool
from libcpp.string cimport string
from libcpp.deque cimport deque
from libcpp.vector cimport vector
from libcpp.list cimport list

cdef extern from "boost/shared_ptr.hpp" namespace "boost":
    cdef cppclass shared_ptr[T]:
        shared_ptr(T*)
        shared_ptr()
        T *get()

cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Geometry/mathlib.h":
    cdef cppclass base_vec3[T]:
        base_vec3()
        base_vec3(T,T,T)

cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Base.h":
    cdef cppclass Base:
        string getName()
        void setName(const string &_name)

cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Accelerator/LeafTreatment.h" namespace "leafTreatment":
    cdef enum treatment "leafTreatment::treatment":
        FIRST          
        ALL_BEFORE_TRIANGLE
        ALL_BEFORE_VISIBLE
        ALL

cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Acoustic/Event.h":
    cdef cppclass Event(Base):
        pass
    cdef enum typeevent:
        SPECULARREFLEXION
        DIFFRACTION
        NOTHING
                
cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Geometry/Shape.h":
    cdef cppclass Shape(Base):
        pass
    cdef struct Intersection:
        pass
    
cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Geometry/Sphere.h":
    cdef cppclass Sphere(Shape):
        void setRadius(float)
        void setPosition(base_vec3[float]&)

cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Geometry/Sampler.h":
    cdef cppclass Sampler:
        Sampler()
cdef class cySampler:        
    cdef Sampler* thisptr
    
cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Geometry/Latitude2DSampler.h":
    cdef cppclass Latitude2DSampler(Sampler):
        Latitude2DSampler(int)
        void setStartPhi(float)
        void setEndPhi(float)
        void setStartTheta(float)
        void setEndTheta(float)
cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Geometry/Longitude2DSampler.h":
    cdef cppclass Longitude2DSampler(Sampler):
        Longitude2DSampler(int)

cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Geometry/RandomSphericSampler.h":
    cdef cppclass RandomSphericSampler(Sampler):
        RandomSphericSampler(int)

cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Geometry/UniformSphericSampler.h":
    cdef cppclass UniformSphericSampler(Sampler):
        UniformSphericSampler(int)
        int getRealNbRays()

cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Geometry/UniformSphericSampler2.h":
    cdef cppclass UniformSphericSampler2(Sampler):
        UniformSphericSampler2(int)
        int getRealNbRays()

cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Engine/Engine.h":
    cdef cppclass Engine:
        Engine()
        Scene* getScene()
        void setScene(Scene*)
        Solver* getSolver()
        void setSolver(Solver*)
        
cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Engine/DefaultEngine.h":
    cdef cppclass DefaultEngine(Engine):
        pass
    
cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Engine/ParallelDefaultEngine.h":
    cdef cppclass ParallelDefaultEngine(Engine):
        pass
    
cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Ray/Ray.h":
    cdef cppclass Ray(Base):
        float computeEventsSequenceLength()
        void computeLongueur()
        float computeTrueLength (base_vec3[float]&, base_vec3[float]&, base_vec3[float]&)
        float computePertinentLength (base_vec3[float]&, base_vec3[float]&, base_vec3[float]&)
        Base* getLastPertinentEventOrSource(typeevent)   
        base_vec3[float] computeLocalOrigin(Base*)
        double getLongueur() 
        int getDiff()
        int getReflex()
        int getNbEvents()
        vector[shared_ptr[Event]]* getEvents()
        vector[int] getFaceHistory()
        vector[int] getPrimitiveHistory()
        Source* getSource()
        void* getRecepteur()
        float getThickness(float&, bool)
        float getSolidAngle(bool&)
    
cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Acoustic/Solver.h":
    cdef cppclass Solver:
        Solver()
        bool postTreatmentScene(Scene*, vector[Source]&, vector[Recepteur]&)
        bool valideIntersection(Ray*, Intersection*)
        bool valideRayon(Ray*)
        bool invalidRayon(Ray*)
        deque[Ray*]* getValidRays()
        deque[Ray*]* getDebugRays()
        void finish()
        void clean()
        bool loadParameters()  
cdef class cySolver:        
    cdef Solver* thisptr

cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Acoustic/Spectre.h":
    cdef cppclass Spectre:
        Spectre()
        int getSizeSpectre()
        void getFrequencies(vector[int]&)
        int getFrequencie(int)
        void getPowers(vector[float]&)
        void getPowers(float*)
        float getPower(int)
        void addFrequencie(int, float)

cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Acoustic/Source.h":
    cdef cppclass Source(Base):
        Source()
        base_vec3[float] getPosition()
        void setPosition(base_vec3[float])
        Spectre getSpectre()
        void setSpectre(Spectre&)
        Sampler* getSampler()
        void setSampler(Sampler*)
        int getId()
        void setId(int)
        bool getDirection(base_vec3[float]&)
        void setInitialRayCount(int)
        void setPosition(base_vec3[float]*)
cdef class cySource:
    cdef Source thisptr

cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Acoustic/Recepteur.h":
    cdef cppclass Recepteur(Sphere):
        Recepteur()
        Recepteur(base_vec3[float]&)
cdef class cyRecepteur:
    cdef Recepteur thisptr

cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Accelerator/Accelerator.h":
    cdef cppclass Accelerator:
        Accelerator()
        bool build()
        treatment getIntersectionChoice()
        void setIntersectionChoice (treatment)
        float traverse(Ray*, list[Intersection]&)
    
cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Accelerator/BruteForceAccelerator.h":
    cdef cppclass BruteForceAccelerator(Accelerator):
        pass

cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Accelerator/BvhAccelerator.h":
    cdef cppclass BvhAccelerator(Accelerator):
        pass

cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Accelerator/GridAccelerator.h":
    cdef cppclass GridAccelerator(Accelerator):
        pass

cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Accelerator/KdtreeAccelerator.h":
    cdef cppclass KdtreeAccelerator(Accelerator):
        pass

cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Acoustic/Material.h":
    cdef cppclass Material(Base):
        pass

cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Geometry/BBox.h":
    cdef cppclass BBox:
        pass
    
cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Geometry/Scene.h":
    cdef cppclass Scene(Base):
        void clean()
        void addShape(Shape*)
        BBox getGlobalBox()
        Accelerator* getAccelerator()
        bool addVertex(base_vec3[float]&, int)
        Shape* addTriangle(int, int, int, Material*, bool)
        void addBuilding (base_vec3[float], base_vec3[float], Material*)
        bool finish(int accelerator_id, treatment _intersectionChoice)
        void export_to_ply(string)
        void import_from_ply(string)
                    
cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Engine/Simulation.h":
    cdef cppclass Simulation(Base):
        Simulation()
        AcousticRaytracerConfiguration* getConfiguration()
        Scene* getScene()
        Scene* get_receptors_landscape()
        void addSource(const Source& s) 
        vector[Source]& getSources()    
        void addRecepteur(const Recepteur& r)
        vector[Recepteur]& getRecepteurs()    
        void setSolver(Solver*)
        Solver* getSolver()
        void setEngine()
        bool launchSimulation()    
        void clean()
cdef class cySimulation:
    cdef shared_ptr[Simulation] thisptr
    cdef cyAcousticRaytracerConfiguration configuration
        
cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Engine/AcousticRaytracerConfiguration.h":
    cdef cppclass AcousticRaytracerConfiguration:
        unsigned int NbRaysPerSource
        unsigned int Discretization
        unsigned int Accelerator
        unsigned int MaxTreeDepth
        unsigned int NbRayWithDiffraction
        unsigned int MaxProfondeur
        unsigned int MaxReflexion
        unsigned int MaxDiffraction
        double MaxLength
        double SizeReceiver
        double AngleDiffMin
        double CylindreThick
        double MaxPathDifference
        double InitialAnglePhi
        double FinalAnglePhi
        double InitialAngleTheta
        double FinalAngleTheta
        bool UseSol
        bool UsePathDifValidation
        bool DiffractionFilterRayAtCreation
        bool DiffractionUseDistanceAsFilter
        bool DiffractionUseRandomSampler
        bool DiffractionDropDownNbRays
        bool UsePostFilter
        bool DebugUseCloseEventPostFilter
        bool DebugUseDiffractionAngleSelector
        bool DebugUseDiffractionPathSelector
        bool DebugUseFermatSelector
        bool DebugUseFaceSelector
        bool KeepDebugRay
        
cdef class cyAcousticRaytracerConfiguration:
    cdef AcousticRaytracerConfiguration* thisptr        