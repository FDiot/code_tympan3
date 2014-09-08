/**
 * \brief This file provides class for solver configuration
 */

#ifndef TY_MS_CONFIG
#define TY_MS_CONFIG

#include "Tympan/core/smartptr.h"

namespace tympan
{

class SolverConfiguration;
typedef SmartPtr<SolverConfiguration> LPSolverConfiguration;


class SolverConfiguration : public IRefCount
{
public:

    double AtmosPressure;
    double AtmosTemperature;
    double AtmosHygrometry;
    double AnalyticC0;
    double WindDirection;
    double AnalyticGradC;
    double AnalyticGradV;

    int RayTracingOrder;
    int Discretization;
    int NbRaysPerSource;
    float MaxLength;
    float SizeReceiver;
    int Accelerator;
    int MaxTreeDepth;
    float AngleDiffMin;
    float CylindreThick;
    int MaxProfondeur;
    bool UseSol;
    int MaxReflexion;
    int MaxDiffraction;
    bool DiffractionUseRandomSampler;
    int NbRayWithDiffraction;
    bool DiffractionDropDownNbRays;
    bool DiffractionFilterRayAtCreation;
    bool UsePathDifValidation;
    float MaxPathDifference;
    bool DiffractionUseDistanceAsFilter;
    bool KeepDebugRay;
    bool UsePostFilters;

    //bool EnableTargets;
    //float SampleGround2D;
    bool EnableFullTargets;
    float TargetsDensity;


    //int CurveRaySampler;
    float InitialAngleTheta;
    //float FinalAngleTheta;
    float InitialAnglePhi;
    //float FinalAnglePhi;
    int AnalyticNbRay;
    double AnalyticTMax;
    double AnalyticH;
    double AnalyticDMax;

    int AnalyticTypeTransfo;
    //float MeshRefinementValue;
    //bool RestitModifiedGeom;

    //float MinSRDistance;

    //bool UseRealGround;
    //bool UseVegetation;
    //bool UseScreen;
    //bool UseLateralDiffraction;
    //bool UseReflection;
    //bool PropaConditions;
    //float H1parameter;
    //bool ModSummation;

    bool UseMeteo;
    float OverSampleD;
    bool UseFresnelArea;
    float Anime3DSigma;
    int Anime3DForceC;

    bool DebugUseCloseEventSelector;
    bool DebugUseDiffractionAngleSelector;
    bool DebugUseDiffractionPathSelector;
    bool DebugUseFermatSelector;
    bool DebugUseFaceSelector;

    SolverConfiguration();
    ~SolverConfiguration();
    static LPSolverConfiguration get();

private:

    // singleton
    static LPSolverConfiguration _pInstance;
};

} // namespace tympan


#endif // TY_MS_CONFIG
