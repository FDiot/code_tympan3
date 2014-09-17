/*
 * Copyright (C) <2012> <EDF-R&D> <FRANCE>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef GLOBAL_H
#define GLOBAL_H

////////////////////////////
// Meteo
////////////////////////////

extern double globalAtmosPressure;					// [METEO] Atmospheric pressure in pascal
extern double globalAtmosTemperature;				// [METEO] Temperature in degres celsius
extern double globalAtmosHygrometry;				// [METEO] Hygrometry percent
extern double globalAnalyticC0;						// [METEO] initial (default) sound speed
extern double globalWindDirection;					// [METEO] Wind direction 0 means from north to south
extern double globalAnalyticGradC;					// [METEO] Vertical temperature gradient
extern double globalAnalyticGradV;					// [METEO] Vertical wind speed gradient

////////////////////////////
// Acoustic ray tracer
////////////////////////////

extern int globalRayTracingOrder;					// [ACOUSTICRAYTRACER] Ray tracing order propagation (0=From, 1=from receptor, 2=auto)
extern int globalDiscretization;					// [ACOUSTICRAYTRACER] Sampler model 0=random, 1=uniform v1, 2= uniform v2, 3=horizontal
extern int globalNbRaysPerSource;					// [ACOUSTICRAYTRACER] Number of rays per source
extern float globalMaxLength;						// [ACOUSTICRAYTRACER] Maximum ray length in meter
extern float globalSizeReceiver;					// [ACOUSTICRAYTRACER] Receptor radius in meter
extern int globalAccelerator;						// [ACOUSTICRAYTRACER] Accelerating structure parameter (0=brut force, 1=grid, 2=BVH, 3=KDTree)
extern int globalMaxTreeDepth;						// [ACOUSTICRAYTRACER] Maximal depth search for BVH or KDTree
extern float globalAngleDiffMin;					// [ACOUSTICRAYTRACER] Minimum dihedral angle to add a diffraction cylinder
extern float globalCylindreThick;					// [ACOUSTICRAYTRACER] Diffraction ridge size in meter
extern int globalMaxProfondeur;						// [ACOUSTICRAYTRACER] Maximum events number for a ray
extern bool globalUseSol;							// [ACOUSTICRAYTRACER] Allow ground reflections
extern int globalMaxReflexion;						// [ACOUSTICRAYTRACER] Maximum reflections events for a ray
extern int globalMaxDiffraction;					// [ACOUSTICRAYTRACER] Maximum diffraction events for a ray
extern bool globalDiffractionUseRandomSampler;		// [ACOUSTICRAYTRACER] Use random sampler instead of regular distribution 
extern int globalNbRayWithDiffraction;				// [ACOUSTICRAYTRACER] Number of ray thrown after diffraction (0 = based on source sampling, >0 = forced)
extern bool globalDiffractionDropDownNbRays;		// [ACOUSTICRAYTRACER] Drop down number of rays thrown after a diffraction
extern bool globalDiffractionFilterRayAtCreation;	// [ACOUSTICRAYTRACER] Do not create rays outside the correct angle
extern bool globalUsePathDifValidation;				// [ACOUSTICRAYTRACER] Allow use of path length difference validation
extern float globalMaxPathDifference;				// [ACOUSTICRAYTRACER] Maximum path length difference in meter (25 meters for 25 dB, 8 meters for 20 dB)
extern bool globalDiffractionUseDistanceAsFilter;	// [ACOUSTICRAYTRACER] Allow suppressing rays passing to far from the ridge
extern bool globalKeepDebugRay;						// [ACOUSTICRAYTRACER] Keep invalid rays
extern bool globalUsePostFilters;					// [ACOUSTICRAYTRACER] Use of post-filters

////////////////////////////
// Targeting parameters
////////////////////////////

extern bool globalEnableTargets;					// [ACOUSTICRAYTRACER] Use targeting
extern float globalSampleGround2D;					// [ACOUSTICRAYTRACER] Sample ground in 2D
extern bool globalEnableFullTargets;				// [ACOUSTICRAYTRACER] Set target search after a diffuse event
extern float globalTargetsDensity;					// [ACOUSTICRAYTRACER] Sampling density for interesting areas

////////////////////////////
// AnalyticRayTracer
////////////////////////////

extern int globalCurveRaySampler;					// [ANALYTICRAYTRACER] Sampler model 1=horizontal, 2=vertical, 3=uniform v1, 4=uniform v2
extern float globalInitialAngleTheta;				// [ANALYTICRAYTRACER] Start vertical angle (theta)
extern float globalFinalAngleTheta;					// [ANALYTICRAYTRACER] Final vertical angle (theta)
extern float globalInitialAnglePhi;					// [ANALYTICRAYTRACER] Start horizontal angle (phi)
extern float globalFinalAnglePhi;					// [ANALYTICRAYTRACER] Final horizontal angle (phi)
extern int globalAnalyticNbRay;						// [ANALYTICRAYTRACER] Number of rays per source
extern double globalAnalyticTMax;					// [ANALYTICRAYTRACER] Propagation time in second
extern double globalAnalyticH;						// [ANALYTICRAYTRACER] Time step in second
extern double globalAnalyticDMax;					// [ANALYTICRAYTRACER] Maximum length propagation

////////////////////////////
// Geometric transformer
///////////////////////////

extern int globalAnalyticTypeTransfo;				// [GEOM_TRANSFORMER] Transformation method (1 is the only [good] response)
extern float globalMeshRefinementValue;				// [GEOM_TRANSFORMER] Altimetry refinement parameter
extern bool globalRestitModifiedGeom;				// [GEOM_TRANSFORMER] Restore modified altimetry after computing

/////////////////////////////
// Preprocessing
/////////////////////////////

extern float globalMinSRDistance;					// [PREPROCESSING] Source-receptor minimal distance in meters

/////////////////////////////
// Default Solver
/////////////////////////////

extern bool globalUseRealGround;					// [DEFAULTSOLVER] Use of real ground (0) or totally reflective ground (1)
extern bool globalUseVegetation;					// [DEFAULTSOLVER] Takes vegetation into account
extern bool globalUseScreen;						// [DEFAULTSOLVER] Takes screens into account
extern bool globalUseLateralDiffraction;			// [DEFAULTSOLVER] Lateral diffractions computing (if screens on)
extern bool globalUseReflection;					// [DEFAULTSOLVER] Takes reflections in account (first order only)
extern bool globalPropaConditions;					// [DEFAULTSOLVER] Propagation conditions (non refracting / downward conditions (ISO 9613))
extern float globalH1parameter ;					// [DEFAULTSOLVER] H1 parameter (ISO 9613 downward conditions)
extern bool globalModSummation;						// [DEFAULTSOLVER] Energetic (p² summation) or interference (p summation)

//////////////////////////////
// ANIME3D Solver
/////////////////////////////

extern bool globalUseMeteo;							// [ANIME3D] Takes meteo in account
extern float globalOverSampleD;						// [ANIME3D] Rays oversampling rate (if meteo -see above-)
extern bool globalUseFresnelArea;					// [ANIME3D] Use Fresnel area
extern float globalAnime3DSigma;					// [ANIME3D] Value of relative uncertainty
extern int globalAnime3DForceC;					// [ANIME3D] Force "C" parameter

////////////////////////////////
// Mode DEBUG
////////////////////////////////
extern bool globalDebugUseCloseEventSelector;		// [DEBUG] allow use of closeEventSelector
extern bool globalDebugUseDiffractionAngleSelector;	// [DEBUG] allow use of diffraction angle selector
extern bool globalDebugUseDiffractionPathSelector;	// [DEBUG] allow use of diffraction path length selector
extern bool globalDebugUseFermatSelector;			// [DEBUG] allow use of Fermat selector
extern bool globalDebugUseFaceSelector;				// [DEBUG] allow use of (doubled) faces selector

#endif //GLOBAL_H
