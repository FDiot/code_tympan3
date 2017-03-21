"""Acoustic Ray Tracer from Code_TYMPAN, headers
"""
from libcpp cimport bool
from libcpp.string cimport string

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
            
cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Geometry/Shape.h":
    cdef cppclass Shape(Base):
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

cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Acoustic/Solver.h":
    cdef cppclass Solver:
        Solver()
cdef class cySolver:        
    cdef Solver* thisptr
               
cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Acoustic/Source.h":
    cdef cppclass Source(Base):
        Source()
        void setSampler(Sampler*)
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
        pass
    
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

cdef extern from "Tympan/geometric_methods/AcousticRaytracer/Geometry/Scene.h":
    cdef cppclass Scene(Base):
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
        void addRecepteur(const Recepteur& r)
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