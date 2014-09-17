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

#include <string>
#include <fstream>

#include "Tympan/solvers/AcousticRaytracer/global.h"
#include "TYANIME3DSetup.h"

namespace ANIME3DSetup
{
	double getParam(char* ligne)
	{
		std::string str(ligne);
		int delim = str.find(';');
		str.erase(delim);
		if (delim == -1) { return 1.0E-32f; }
		const char* bidon = str.c_str();
		return std::atof(bidon);
	}

    void exec()
    {
        // Init values
        initGlobalValues();

   		// Load parameters from file (csv)
		loadParameters();
    }

	void initGlobalValues()
	{
	////////////////////////////
	// Meteo
	////////////////////////////

		globalAtmosPressure = 101300.;			// [METEO] Atmospheric pressure in pascal
		globalAtmosTemperature = 20.;			// [METEO] Temperature in degres celsius
		globalAtmosHygrometry = 60.;			// [METEO] Hygrometry percent
		globalAnalyticC0 = 340.;				// [METEO] initial (default) sound speed
		globalWindDirection = 0.;				// [METEO] Wind direction 0 means from north to south
		globalAnalyticGradC = 0.;				// [METEO] Vertical temperature gradient
		globalAnalyticGradV = 0.;				// [METEO] Vertical wind speed gradient

	////////////////////////////
	// Acoustic ray tracer
	////////////////////////////

		globalRayTracingOrder = 0;				// [ACOUSTICRAYTRACER] Ray tracing order propagation (0=From, 1=from receptor, 2=auto)
		globalDiscretization = 2;				// [ACOUSTICRAYTRACER] Sampler model 0=random, 1=uniform v1, 2= uniform v2, 3=horizontal
		globalNbRaysPerSource = 100000;			// [ACOUSTICRAYTRACER] Number of rays per source
		globalMaxLength = 5000;					// [ACOUSTICRAYTRACER] Maximum ray length in meter
		globalSizeReceiver = 2.;				// [ACOUSTICRAYTRACER] Receptor radius in meter
		globalAccelerator = 3;					// [ACOUSTICRAYTRACER] Accelerating structure parameter (0=brut force, 1=grid, 2=BVH, 3=KDTree)
		globalMaxTreeDepth = 12;				// [ACOUSTICRAYTRACER] Maximal depth search for BVH or KDTree
		globalAngleDiffMin = 5.;				// [ACOUSTICRAYTRACER] Minimum dihedral angle to add a diffraction cylinder
		globalCylindreThick = 0.3f;				// [ACOUSTICRAYTRACER] Diffraction ridge size in meter
		globalMaxProfondeur = 10;				// [ACOUSTICRAYTRACER] Maximum events number for a ray
		globalUseSol = 0;						// [ACOUSTICRAYTRACER] Allow ground reflections
		globalMaxReflexion = 0;					// [ACOUSTICRAYTRACER] Maximum reflections events for a ray
		globalMaxDiffraction = 2;				// [ACOUSTICRAYTRACER] Maximum diffraction events for a ray
		globalDiffractionUseRandomSampler = 0;	// [ACOUSTICRAYTRACER] Use random sampler instead of regular distribution
		globalNbRayWithDiffraction = 0;			// [ACOUSTICRAYTRACER] Number of ray thrown after diffraction (<0 = depends of sources, 0 = distance filter, >0 = forced)
		globalDiffractionDropDownNbRays = 1;	// [ACOUSTICRAYTRACER] Drop down number of rays thrown after a diffraction
		globalDiffractionFilterRayAtCreation = 0;	// [ACOUSTICRAYTRACER] Do not create rays outside the correct angle
		globalUsePathDifValidation = 0;			// [ACOUSTICRAYTRACER] Allow use of path length difference validation
		globalMaxPathDifference = 25.;			// [ACOUSTICRAYTRACER] Maximum path length difference in meter (25 meters for 25 dB, 8 meters for 20 dB)
		globalDiffractionUseDistanceAsFilter = 1;	// [ACOUSTICRAYTRACER] Allow suppressing rays passing to far from the ridge
		globalKeepDebugRay = 0;					// [ACOUSTICRAYTRACER] Keep invalid rays
		globalUsePostFilters = 1;				// [ACOUSTICRAYTRACER] Use of post-filters

	////////////////////////////
	// Targeting parameters
	////////////////////////////

		globalEnableTargets = 0;				// [ACOUSTICRAYTRACER] Use targeting
		globalSampleGround2D = 0;				// [ACOUSTICRAYTRACER] Sample ground in 2D
		globalEnableFullTargets = 0;			// [ACOUSTICRAYTRACER] Set target search after a diffuse event
		globalTargetsDensity = 0.1f;				// [ACOUSTICRAYTRACER] Sampling density for interesting areas

	////////////////////////////
	// AnalyticRayTracer
	////////////////////////////

		globalCurveRaySampler = 1;				// [ANALYTICRAYTRACER] Sampler model 1=horizontal, 2=vertical, 3=uniform v1, 4=uniform v2
		globalInitialAngleTheta = 0.;			// [ANALYTICRAYTRACER] Start vertical angle (theta)
		globalFinalAngleTheta = 0.;				// [ANALYTICRAYTRACER] Final vertical angle (theta)
		globalInitialAnglePhi = 0.;				// [ANALYTICRAYTRACER] Start horizontal angle (phi)
		globalFinalAnglePhi = 360.;				// [ANALYTICRAYTRACER] Final horizontal angle (phi)
		globalAnalyticNbRay = 20;				// [ANALYTICRAYTRACER] Number of rays per source
		globalAnalyticTMax = 10.;				// [ANALYTICRAYTRACER] Propagation time in second
		globalAnalyticH = 0.1;					// [ANALYTICRAYTRACER] Time step in second
		globalAnalyticDMax = 3000;				// [ANALYTICRAYTRACER] Maximum length propagation

	////////////////////////////
	// Geometric transformer
	///////////////////////////

		globalAnalyticTypeTransfo = 1;			// [GEOM_TRANSFORMER] Transformation method (1 is the only [good] response)
		globalMeshRefinementValue = 0.;			// [GEOM_TRANSFORMER] Altimetry refinement parameter
		globalRestitModifiedGeom = 0;			// [GEOM_TRANSFORMER] Restore modified altimetry after computing

	/////////////////////////////
	// Preprocessing
	/////////////////////////////

		globalMinSRDistance = 0.3f;				// [PREPROCESSING] Source-receptor minimal distance in meters

	/////////////////////////////
	// Default Solver
	/////////////////////////////

		globalUseRealGround = 1;				// [DEFAULTSOLVER] Use of real ground (0) or totally reflective ground (1)
		globalUseVegetation = 0;				// [DEFAULTSOLVER] Takes vegetation into account
		globalUseScreen = 1;					// [DEFAULTSOLVER] Takes screens into account
		globalUseLateralDiffraction = 1;		// [DEFAULTSOLVER] Lateral diffractions computing (if screens on)
		globalUseReflection = 1;				// [DEFAULTSOLVER] Takes reflections in account (first order only)
		globalPropaConditions = 0;				// [DEFAULTSOLVER] Propagation conditions (non refracting / downward conditions (ISO 9613))
		globalH1parameter = 10.;				// [DEFAULTSOLVER] H1 parameter (ISO 9613 downward conditions)
		globalModSummation = 0;					// [DEFAULTSOLVER] Energetic (p² summation) or interference (p summation)

	//////////////////////////////
	// ANIME3D Solver
	/////////////////////////////

		globalUseMeteo = 0;						// [ANIME3D] Takes meteo in account
		globalOverSampleD = 0;					// [ANIME3D] Rays oversampling rate (if meteo -see above-)
		globalUseFresnelArea = 0;				// [ANIME3D] Use Fresnel area
		globalAnime3DSigma = 0.;				// [ANIME3D] Value of relative uncertainty
		globalAnime3DForceC = 1;				// [ANIME3D] Force "C" parameter

	////////////////////////////////
	// Mode DEBUG
	////////////////////////////////

		globalDebugUseCloseEventSelector = true;			// [DEBUG] allow use of closeEventSelector
		globalDebugUseDiffractionAngleSelector = true;	// [DEBUG] allow use of diffraction angle selector
		globalDebugUseDiffractionPathSelector = true;		// [DEBUG] allow use of diffraction path length selector
		globalDebugUseFermatSelector = true;				// [DEBUG] allow use of Fermat selector
		globalDebugUseFaceSelector = true;				// [DEBUG] allow use of (doubled) faces selector
	}

	bool loadParameters()
	{
		bool bRes = true;
		const char fileName[33] = "universalSolverParameterFile.txt";

		// Ouverture du fichier
		std::ifstream params(fileName, std::ios::in);

		if (!params) { return false; }

		char ligne[132];

	////////////////////////////
	// Meteo
	////////////////////////////

		// [METEO] Atmospheric pressure in pascal
		if (params.getline(ligne, 132)) { globalAtmosPressure = getParam(ligne); }

		// [METEO] Temperature in degres celsius
		if (params.getline(ligne, 132)) { globalAtmosTemperature = getParam(ligne); }

		// [METEO] Hygrometry percent
		if (params.getline(ligne, 132)) { globalAtmosHygrometry = getParam(ligne); }

		// [METEO] initial (default) sound speed
		if (params.getline(ligne, 132)) { globalAnalyticC0 = getParam(ligne); }

		// [METEO] Wind direction 0 means from north to south
		if (params.getline(ligne, 132)) { globalWindDirection = getParam(ligne); }

		// [METEO] Vertical temperature gradient
		if (params.getline(ligne, 132)) { globalAnalyticGradC = getParam(ligne); }

		// [METEO] Vertical wind speed gradient
		if (params.getline(ligne, 132)) { globalAnalyticGradV = getParam(ligne); }


	////////////////////////////
	// Acoustic ray tracer
	////////////////////////////

		// [ACOUSTICRAYTRACER] Ray tracing order propagation (0=From, 1=from receptor, 2=auto)
		if (params.getline(ligne, 132)) { globalRayTracingOrder = static_cast<int>( getParam(ligne) ); }

		// [ACOUSTICRAYTRACER] Sampler model 0=random, 1=uniform v1, 2= uniform v2, 3=horizontal
		if (params.getline(ligne, 132)) { globalDiscretization = static_cast<int>( getParam(ligne) ); }

		// [ACOUSTICRAYTRACER] Number of rays per source
		if (params.getline(ligne, 132)) { globalNbRaysPerSource = static_cast<int>( getParam(ligne) ); }

		// [ACOUSTICRAYTRACER] Maximum ray length in meter
		if (params.getline(ligne, 132)) { globalMaxLength = static_cast<float>( getParam(ligne) ); }

		// [ACOUSTICRAYTRACER] Receptor radius in meter
		if (params.getline(ligne, 132)) { globalSizeReceiver = static_cast<float>( getParam(ligne) ); }

		// [ACOUSTICRAYTRACER] Accelerating structure parameter (0=brut force, 1=grid, 2=BVH, 3=KDTree)
		if (params.getline(ligne, 132)) { globalAccelerator = static_cast<int>( getParam(ligne) ); }

		// [ACOUSTICRAYTRACER] Maximal depth search for BVH or KDTree
		if (params.getline(ligne, 132)) { globalMaxTreeDepth = static_cast<int>( getParam(ligne) ); }

		// [ACOUSTICRAYTRACER] Minimum dihedral angle to add a diffraction cylinder
		if (params.getline(ligne, 132)) { globalAngleDiffMin = static_cast<float>( getParam(ligne) ); }

		// [ACOUSTICRAYTRACER] Diffraction ridge size in meter
		if (params.getline(ligne, 132)) { globalCylindreThick = static_cast<float>( getParam(ligne) ); }

		// [ACOUSTICRAYTRACER] Maximum events number for a ray
		if (params.getline(ligne, 132)) { globalMaxProfondeur = static_cast<int>( getParam(ligne) ); }

		// [ACOUSTICRAYTRACER] Allow ground reflections
		if (params.getline(ligne, 132)) { globalUseSol = static_cast<bool>( getParam(ligne) ); }

		// [ACOUSTICRAYTRACER] Maximum reflections events for a ray
		if (params.getline(ligne, 132)) { globalMaxReflexion = static_cast<int>( getParam(ligne) ); }

		// [ACOUSTICRAYTRACER] Maximum diffraction events for a ray
		if (params.getline(ligne, 132)) { globalMaxDiffraction = static_cast<int>( getParam(ligne) ); }

		// [ACOUSTICRAYTRACER] Use random sampler instead of regular distribution
		if (params.getline(ligne, 132)) { globalDiffractionUseRandomSampler = static_cast<bool>( getParam(ligne) ); }

		// [ACOUSTICRAYTRACER] Number of ray thrown after diffraction (<0 = depends of sources, 0 = distance filter, >0 = forced)
		if (params.getline(ligne, 132)) { globalNbRayWithDiffraction = static_cast<int>( getParam(ligne) ); }

		// [ACOUSTICRAYTRACER] Drop down number of rays thrown after a diffraction
		if (params.getline(ligne, 132)) { globalDiffractionDropDownNbRays = static_cast<bool>( getParam(ligne) ); }

		// [ACOUSTICRAYTRACER] Do not create rays outside the correct angle
		if (params.getline(ligne, 132)) { globalDiffractionFilterRayAtCreation = static_cast<bool>( getParam(ligne) ); }

		// [ACOUSTICRAYTRACER] Allow use of path length difference validation
		if (params.getline(ligne, 132)) { globalUsePathDifValidation = static_cast<bool>( getParam(ligne) ); }

		// [ACOUSTICRAYTRACER] Maximum path length difference in meter (25 meters for 25 dB, 8 meters for 20 dB)
		if (params.getline(ligne, 132)) { globalMaxPathDifference = static_cast<float>( getParam(ligne) ); }

		// [ACOUSTICRAYTRACER] Allow suppressing rays passing to far from the ridge
		if (params.getline(ligne, 132)) { globalDiffractionUseDistanceAsFilter = static_cast<bool>( getParam(ligne) ); }

		// [ACOUSTICRAYTRACER] Keep invalid rays
		if (params.getline(ligne, 132)) { globalKeepDebugRay = static_cast<bool>( getParam(ligne) ); }

		// [ACOUSTICRAYTRACER] Use of post-filters
		if (params.getline(ligne, 132)) { globalUsePostFilters = static_cast<bool>( getParam(ligne) ); }

	////////////////////////////
	// Targeting parameters
	////////////////////////////

		// [ACOUSTICRAYTRACER] Use targeting
		if (params.getline(ligne, 132)) { globalEnableTargets = static_cast<bool>( getParam(ligne) ); }

		// [ACOUSTICRAYTRACER] Sample ground in 2D
		if (params.getline(ligne, 132)) { globalSampleGround2D = static_cast<float>( getParam(ligne) ); }

		// [ACOUSTICRAYTRACER] Set target search after a diffuse event
		if (params.getline(ligne, 132)) { globalEnableFullTargets = static_cast<bool>( getParam(ligne) ); }

		// [ACOUSTICRAYTRACER] Sampling density for interesting areas
		if (params.getline(ligne, 132)) { globalTargetsDensity = static_cast<float>( getParam(ligne) ); }

	////////////////////////////
	// AnalyticRayTracer
	////////////////////////////

		// [ANALYTICRAYTRACER] Sampler model 1=horizontal, 2=vertical, 3=uniform v1, 4=uniform v2
		if (params.getline(ligne, 132)) { globalCurveRaySampler = static_cast<int>( getParam(ligne) ); }

		// [ANALYTICRAYTRACER] Start vertical angle (theta)
		if (params.getline(ligne, 132)) { globalInitialAngleTheta = static_cast<float>( getParam(ligne) ); }

		// [ANALYTICRAYTRACER] Final vertical angle (theta)
		if (params.getline(ligne, 132)) { globalFinalAngleTheta = static_cast<float>( getParam(ligne) ); }

		// [ANALYTICRAYTRACER] Start horizontal angle (phi)
		if (params.getline(ligne, 132)) { globalInitialAnglePhi = static_cast<float>( getParam(ligne) ); }

		// [ANALYTICRAYTRACER] Final horizontal angle (phi)
		if (params.getline(ligne, 132)) { globalFinalAnglePhi = static_cast<float>( getParam(ligne) ); }

		// [ANALYTICRAYTRACER] Number of rays per source
		if (params.getline(ligne, 132)) { globalAnalyticNbRay = static_cast<int>( getParam(ligne) ); }

		// [ANALYTICRAYTRACER] Propagation time in second
		if (params.getline(ligne, 132)) { globalAnalyticTMax = getParam(ligne); }

		// [ANALYTICRAYTRACER] Time step in second
		if (params.getline(ligne, 132)) { globalAnalyticH = getParam(ligne); }

		// [ANALYTICRAYTRACER] Maximum length propagation
		if (params.getline(ligne, 132)) { globalAnalyticDMax = getParam(ligne); }


	////////////////////////////
	// Geometric transformer
	///////////////////////////

		// [GEOM_TRANSFORMER] Transformation method (1 is the only [good] response)
		if (params.getline(ligne, 132)) { globalAnalyticTypeTransfo = static_cast<int>( getParam(ligne) ); }

		// [GEOM_TRANSFORMER] Altimetry refinement parameter
		if (params.getline(ligne, 132)) { globalMeshRefinementValue = static_cast<float>( getParam(ligne) ); }

		// [GEOM_TRANSFORMER] Restore modified altimetry after computing
		if (params.getline(ligne, 132)) { globalRestitModifiedGeom = static_cast<bool>( getParam(ligne) ); }


	/////////////////////////////
	// Preprocessing
	/////////////////////////////

		// [PREPROCESSING] Source-receptor minimal distance in meters
		if (params.getline(ligne, 132)) { globalMinSRDistance = static_cast<float>( getParam(ligne) ); }


	/////////////////////////////
	// Default Solver
	/////////////////////////////

		// [DEFAULTSOLVER] Use of real ground (0) or totally reflective ground (1)
		if (params.getline(ligne, 132)) { globalUseRealGround = static_cast<bool>( getParam(ligne) ); }

		// [DEFAULTSOLVER] Takes vegetation into account
		if (params.getline(ligne, 132)) { globalUseVegetation = static_cast<bool>( getParam(ligne) ); }

		// [DEFAULTSOLVER] Takes screens into account
		if (params.getline(ligne, 132)) { globalUseScreen = static_cast<bool>( getParam(ligne) ); }

		// [DEFAULTSOLVER] Lateral diffractions computing (if screens on)
		if (params.getline(ligne, 132)) { globalUseLateralDiffraction = static_cast<bool>( getParam(ligne) ); }

		// [DEFAULTSOLVER] Takes reflections in account (first order only)
		if (params.getline(ligne, 132)) { globalUseReflection = static_cast<bool>( getParam(ligne) ); }

		// [DEFAULTSOLVER] Propagation conditions (non refracting / downward conditions (ISO 9613))
		if (params.getline(ligne, 132)) { globalPropaConditions = static_cast<bool>( getParam(ligne) ); }

		// [DEFAULTSOLVER] H1 parameter (ISO 9613 downward conditions)
		if (params.getline(ligne, 132)) { globalH1parameter = static_cast<float>( getParam(ligne) ); }

		// [DEFAULTSOLVER] Energetic (p² summation) or interference (p summation)
		if (params.getline(ligne, 132)) { globalModSummation = static_cast<bool>( getParam(ligne) ); }


	//////////////////////////////
	// ANIME3D Solver
	/////////////////////////////

		// [ANIME3D] Takes meteo in account
		if (params.getline(ligne, 132)) { globalUseMeteo = static_cast<bool>( getParam(ligne) ); }

		// [ANIME3D] Rays oversampling rate (if meteo -see above-)
		if (params.getline(ligne, 132)) { globalOverSampleD = static_cast<float>( getParam(ligne) ); }

		// [ANIME3D] Use Fresnel area
		if (params.getline(ligne, 132)) { globalUseFresnelArea = static_cast<bool>( getParam(ligne) ); }

		// [ANIME3D] Value of relative uncertainty
		if (params.getline(ligne, 132)) { globalAnime3DSigma = static_cast<float>( getParam(ligne) ); }

		// [ANIME3D] Force "C" parameter
		if (params.getline(ligne, 132)) { globalAnime3DForceC = static_cast<int>( getParam(ligne) ); }

	////////////////////////////////
	// Mode DEBUG
	////////////////////////////////

		// [DEBUG] allow use of closeEventSelector
		if (params.getline(ligne, 132)) { globalDebugUseCloseEventSelector = static_cast<bool>( getParam(ligne) ); }

		// [DEBUG] allow use of diffraction angle selector
		if (params.getline(ligne, 132)) { globalDebugUseDiffractionAngleSelector = static_cast<bool>( getParam(ligne) ); }

		// [DEBUG] allow use of diffraction path length selector
		if (params.getline(ligne, 132)) { globalDebugUseDiffractionPathSelector = static_cast<bool>( getParam(ligne) ); }

		// [DEBUG] allow use of Fermat selector
		if (params.getline(ligne, 132)) { globalDebugUseFermatSelector = static_cast<bool>( getParam(ligne) ); }

		// [DEBUG] allow use of (doubled) faces selector
		if (params.getline(ligne, 132)) { globalDebugUseFaceSelector = static_cast<bool>( getParam(ligne) ); }


		params.close();

		return bRes;
	}
} //ANIME3DSetup
