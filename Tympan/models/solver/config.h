/**
 * \file config.h
 * \brief This file provides class for solver configuration
 */

#ifndef TY_MS_CONFIG
#define TY_MS_CONFIG

#include "Tympan/core/smartptr.h"

namespace tympan
{

class SolverConfiguration;
typedef SmartPtr<SolverConfiguration> LPSolverConfiguration;

/**
 * @brief Class to define a solver configuration (contains all the data needed by the different Tympan solvers)
 */
class SolverConfiguration : public IRefCount
{
public:

    double AtmosPressure;		//!< Atmospheric pressure [Pa]
    double AtmosTemperature;	//!< Atmospheric temperature [°C]
    double AtmosHygrometry;		//!< Atmospheric hygrometry
    double WindDirection;		//!< Wind direction
    double AnalyticGradC;		//!< Sound speed gradient
    double AnalyticGradV;		//!< Wind gradient

    int RayTracingOrder;		//!< Ray tracing order, with 0:S->R, 1:R->S, 2:Automatic (S->R if more sources than receptors, else R->S)
    int Discretization;			//!< Sampler choice with 0: RandomSphericSampler, 1: UniformSphericSampler, 2: UniformSphericSampler2, 3: Latitude2DSampler
    int NbRaysPerSource;		//!< Number of rays per source for the Sampler
    float MaxLength;			//!< LengthSelector Selector option (maximal length)
    float SizeReceiver;
    int Accelerator;			//!< Accelerator choice with 0: BruteForceAccelerator, 1: GridAccelerator, 2: BvhAccelerator, 3: KdtreeAccelerator
    int MaxTreeDepth;			//!< BvhAccelerator Accelerator option (Maximal tree depth)
    float AngleDiffMin;			//!< Minimal angle (other PI) between two face to allow building of a diffraction cylinder
    float CylindreThick;		//!< Diffraction cylinder diameter
    int MaxProfondeur;			//!< Maximal number of events for ray validation in ANIME3D solver
    bool UseSol;				//!< Flag to activate specular reflection on the ground
    int MaxReflexion;			//!< Maximal reflection events
    int MaxDiffraction;			//!< Maximal diffraction events
    bool DiffractionUseRandomSampler; //!< Flag to enable random (and not regular) sampling for diffraction
    int NbRayWithDiffraction;	//!< Number of rays to throw during diffraction
    bool DiffractionDropDownNbRays; //!< Flag to reduce the number of rays thrown depending on the diffraction order
    bool DiffractionFilterRayAtCreation; //!< Flag to filter the created rays during diffraction
    bool UsePathDifValidation;	//!< Flag to activate path validations during diffraction
    float MaxPathDifference;	//!< Parameter (by default 24) for path validations during diffraction
    bool DiffractionUseDistanceAsFilter; //!< Flag to use distance based filter during diffraction (distance from the ridge)
    bool KeepDebugRay;			//!< Flag to store rays into a debug_rays array after being invalidated
    bool UsePostFilters;		//!< Flag to use some specifics Selector

    int CurveRaySampler;		//!< Not used
    float InitialAngleTheta;	//!< Starting angle theta for the curve ray sampler
    float FinalAngleTheta;		//!< Ending angle theta for the curve ray sampler
    float InitialAnglePhi;		//!< Starting angle phi for the curve ray sampler
    float FinalAnglePhi;		//!< Ending angle phi for the curve ray sampler
    int AnalyticNbRay;			//!< Rays number to launch for the curve ray sampler
    double AnalyticTMax;		//!< The maximal propagation time for the curve ray sampler
    double AnalyticH;			//!< The propagation time step for the curve ray sampler
    double AnalyticDMax;		//!< The maximal distance for the curve ray sampler

    int AnalyticTypeTransfo;	//!< Type of geometry modifier used (only one for the moment: geometry_modifier_z_correction)
    float MeshElementSizeMax;	//!< Not used
    bool showScene;				//!< Flag to export Scene in order to visualize it

    float MinSRDistance;		//!< Not used
    int NbThreads;				//!< Number of threads used by TYSolver
    bool UseRealGround;			//!< Flag to model ground into the acoustic model
    //bool UseVegetation;
    bool UseScreen;				//!< Not used
    bool UseLateralDiffraction;	//!< Flag to compute horizontal paths in the acoustic path finder
    bool UseReflection;			//!< Flag to use reflection on vertical walls
    bool PropaConditions;		//!< Flag to check that reflections will be limited between source and receptors in the acoustic model
    float H1parameter;			//!< Multiplicative coefficient for the additional reflections if PropaConditions is true
    bool ModSummation;			//!< Flag to activate in the acoustic model a calculation with interference

    bool UseMeteo;				//!< Flag to use weather data in the calculation
    bool UseFresnelArea;		//!< Flag to use Fresnel area in the acoustic model for ANIME3D solver
    float Anime3DSigma;			//!< Relative uncertainty on the constant used in the coherence factor in the acoustic model
    int Anime3DForceC;			//!< In the acoustic model, define the way the C value is computed in the acoustic pressure
    bool Anime3DKeepRays;		//!< Flag to keep the rays in the results

    bool DebugUseCloseEventSelector;       //!< Flag to add the the CloseEventSelector filter
    bool DebugUseDiffractionAngleSelector; //!< Flag to add the DiffractionAngleSelector filter
    bool DebugUseDiffractionPathSelector;  //!< Flag to add the DiffractionPathSelector filter
    bool DebugUseFermatSelector;           //!< Flag to add the FermatSelector filter
    bool DebugUseFaceSelector;             //!< Flag to add the FaceSelector filter

    SolverConfiguration();  //!< Constructor
    ~SolverConfiguration(); //!< Destructor
    static LPSolverConfiguration get(); //!< Get the configuration
    static void set(LPSolverConfiguration config); //!< Set a configuration

private:

    // singleton
    static LPSolverConfiguration _pInstance;
};

} // namespace tympan



#endif // TY_MS_CONFIG
