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
 
#include "global.h"

////////////////////////////
// Meteo
////////////////////////////

double globalAtmosPressure;			// [METEO] Atmospheric pressure in pascal
double globalAtmosTemperature;		// [METEO] Temperature in degres celsius
double globalAtmosHygrometry;		// [METEO] Hygrometry percent
double globalAnalyticC0;				// [METEO] initial (default) sound speed
double globalWindDirection;			// [METEO] Wind direction 0 means from north to south
double globalAnalyticGradC;			// [METEO] Vertical temperature gradient
double globalAnalyticGradV;			// [METEO] Vertical wind speed gradient

////////////////////////////
// Acoustic ray tracer
////////////////////////////

int globalRayTracingOrder;			// [ACOUSTICRAYTRACER] Ray tracing order propagation (0=From, 1=from receptor, 2=auto)
int globalDiscretization;			// [ACOUSTICRAYTRACER] Sampler model 0=random, 1=uniform v1, 2= uniform v2, 3=horizontal
int globalNbRaysPerSource;			// [ACOUSTICRAYTRACER] Number of rays per source
float globalMaxLength;				// [ACOUSTICRAYTRACER] Maximum ray length in meter
float globalSizeReceiver;			// [ACOUSTICRAYTRACER] Receptor radius in meter
int globalAccelerator;				// [ACOUSTICRAYTRACER] Accelerating structure parameter (0=brut force, 1=grid, 2=BVH, 3=KDTree)
int globalMaxTreeDepth;				// [ACOUSTICRAYTRACER] Maximal depth search for BVH or KDTree
float globalAngleDiffMin;			// [ACOUSTICRAYTRACER] Minimum dihedral angle to add a diffraction cylinder
float globalCylindreThick;			// [ACOUSTICRAYTRACER] Diffraction ridge size in meter
int globalMaxProfondeur;				// [ACOUSTICRAYTRACER] Maximum events number for a ray
bool globalUseSol;					// [ACOUSTICRAYTRACER] Allow ground reflections
int globalMaxReflexion;				// [ACOUSTICRAYTRACER] Maximum reflections events for a ray
int globalMaxDiffraction;			// [ACOUSTICRAYTRACER] Maximum diffraction events for a ray
bool globalDiffractionUseRandomSampler;		// [ACOUSTICRAYTRACER] Use random sampler instead of regular distribution 
int globalNbRayWithDiffraction;		// [ACOUSTICRAYTRACER] Number of ray thrown after diffraction (<0 = depends of sources, 0 = distance filter, >0 = forced)
bool globalDiffractionDropDownNbRays;		// [ACOUSTICRAYTRACER] Drop down number of rays thrown after a diffraction
bool globalUsePathDifValidation;		// [ACOUSTICRAYTRACER] Allow use of path length difference validation
bool globalDiffractionUseDistanceAsFilter;	// [ACOUSTICRAYTRACER] Allow suppressing rays passing to far from the ridge
float globalMaxPathDifference;		// [ACOUSTICRAYTRACER] Maximum path length difference in meter (25 meters for 25 dB, 8 meters for 20 dB)
bool globalKeepDebugRay;				// [ACOUSTICRAYTRACER] Keep invalid rays
bool globalEnableTargets;			// [ACOUSTICRAYTRACER] Use targeting
float globalSampleGround2D;			// [ACOUSTICRAYTRACER] Sample ground in 2D
bool globalEnableFullTargets;		// [ACOUSTICRAYTRACER] Set target search after a diffuse event
float globalTargetsDensity;			// [ACOUSTICRAYTRACER] Sampling density for interesting areas
bool globalUsePostFilters;			// [ACOUSTICRAYTRACER] Use of post-filters

////////////////////////////
// AnalyticRayTracer
////////////////////////////

int globalCurveRaySampler;			// [ANALYTICRAYTRACER] Sampler model 1=horizontal, 2=vertical, 3=uniform v1, 4=uniform v2
float globalInitialAngleTheta;		// [ANALYTICRAYTRACER] Start vertical angle (theta)
float globalFinalAngleTheta;			// [ANALYTICRAYTRACER] Final vertical angle (theta)
float globalInitialAnglePhi;			// [ANALYTICRAYTRACER] Start horizontal angle (phi)
float globalFinalAnglePhi;			// [ANALYTICRAYTRACER] Final horizontal angle (phi)
int globalAnalyticNbRay;				// [ANALYTICRAYTRACER] Number of rays per source
double globalAnalyticTMax;			// [ANALYTICRAYTRACER] Propagation time in second
double globalAnalyticH;				// [ANALYTICRAYTRACER] Time step in second
double globalAnalyticDMax;			// [ANALYTICRAYTRACER] Maximum length propagation

////////////////////////////
// Geometric transformer
///////////////////////////

int globalAnalyticTypeTransfo;		// [GEOM_TRANSFORMER] Transformation method (1 is the only [good] response)
float globalMeshRefinementValue;	// [GEOM_TRANSFORMER] Altimetry refinement parameter
bool globalRestitModifiedGeom;		// [GEOM_TRANSFORMER] Restore modified altimetry after computing

/////////////////////////////
// Preprocessing
/////////////////////////////

float globalMinSRDistance;			// [PREPROCESSING] Source-receptor minimal distance in meters

/////////////////////////////
// Default Solver
/////////////////////////////

bool globalUseRealGround;			// [DEFAULTSOLVER] Use of real ground (0) or totally reflective ground (1)
bool globalUseVegetation;			// [DEFAULTSOLVER] Takes vegetation into account
bool globalUseScreen;				// [DEFAULTSOLVER] Takes screens into account
bool globalUseLateralDiffraction;	// [DEFAULTSOLVER] Lateral diffractions computing (if screens on)
bool globalUseReflection;			// [DEFAULTSOLVER] Takes reflections in account (first order only)
bool globalPropaConditions;			// [DEFAULTSOLVER] Propagation conditions (non refracting / downward conditions (ISO 9613))
float globalH1parameter ;			// [DEFAULTSOLVER] H1 parameter (ISO 9613 downward conditions)
bool globalModSummation;			// [DEFAULTSOLVER] Energetic (p² summation) or interference (p summation)

//////////////////////////////
// ANIME3D Solver
/////////////////////////////

bool globalUseMeteo;				// [ANIME3D] Takes meteo in account
float globalOverSampleD;			// [ANIME3D] Rays oversampling rate (if meteo -see above-)
bool globalUseFresnelArea;			// [ANIME3D] Use Fresnel area
float globalAnime3DSigma;			// [ANIME3D] Value of relative uncertainty
float globalAnime3DForceC;			// [ANIME3D] Force "C" parameter
