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
 
#include <iostream>
#include <fstream>
#include <cstdlib>

#include "Tympan/MetierSolver/AcousticRaytracer/Acoustic/ValidRay.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Acoustic/PostTreatment.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/FaceSelector.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/LengthSelector.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/DiffractionSelector.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/ReflectionSelector.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/CloseEventSelector.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/FermatSelector.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/DiffractionPathSelector.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/DiffractionAngleSelector.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/SelectorManager.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/Logger.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Acoustic/Solver.h"

#include "TYANIME3DRayTracerSetup.h"

void TYANIME3DRayTracerSetup::initGlobalValues()
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
	globalCylindreThick = 0.3;				// [ACOUSTICRAYTRACER] Diffraction ridge size in meter
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
	globalTargetsDensity = 0.1;				// [ACOUSTICRAYTRACER] Sampling density for interesting areas

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

	globalMinSRDistance = 0.3;				// [PREPROCESSING] Source-receptor minimal distance in meters

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
	globalAnime3DForceC = 1.;				// [ANIME3D] Force "C" parameter

////////////////////////////////
// Mode DEBUG
////////////////////////////////

	globalDebugUseCloseEventSelector = true;			// [DEBUG] allow use of closeEventSelector
	globalDebugUseDiffractionAngleSelector = true;	// [DEBUG] allow use of diffraction angle selector
	globalDebugUseDiffractionPathSelector = true;		// [DEBUG] allow use of diffraction path length selector
	globalDebugUseFermatSelector = true;				// [DEBUG] allow use of Fermat selector
	globalDebugUseFaceSelector = true;				// [DEBUG] allow use of (doubled) faces selector


    // Chargement des parametres de calcul
    loadParameters();
}

bool TYANIME3DRayTracerSetup::loadParameters()
{
    bool bRes = true;
    const char fileName[33] = "universalSolverParameterFile.txt";

    // Ouverture du fichier
    ifstream params(fileName, ios::in);

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
    if (params.getline(ligne, 132)) { globalRayTracingOrder = getParam(ligne); }

	// [ACOUSTICRAYTRACER] Sampler model 0=random, 1=uniform v1, 2= uniform v2, 3=horizontal
    if (params.getline(ligne, 132)) { globalDiscretization = getParam(ligne); }

	// [ACOUSTICRAYTRACER] Number of rays per source
    if (params.getline(ligne, 132)) { globalNbRaysPerSource = getParam(ligne); }

	// [ACOUSTICRAYTRACER] Maximum ray length in meter
    if (params.getline(ligne, 132)) { globalMaxLength = getParam(ligne); }

	// [ACOUSTICRAYTRACER] Receptor radius in meter
    if (params.getline(ligne, 132)) { globalSizeReceiver = getParam(ligne); }

	// [ACOUSTICRAYTRACER] Accelerating structure parameter (0=brut force, 1=grid, 2=BVH, 3=KDTree)
    if (params.getline(ligne, 132)) { globalAccelerator = getParam(ligne); }

	// [ACOUSTICRAYTRACER] Maximal depth search for BVH or KDTree
    if (params.getline(ligne, 132)) { globalMaxTreeDepth = getParam(ligne); }

	// [ACOUSTICRAYTRACER] Minimum dihedral angle to add a diffraction cylinder
    if (params.getline(ligne, 132)) { globalAngleDiffMin = getParam(ligne); }

	// [ACOUSTICRAYTRACER] Diffraction ridge size in meter
    if (params.getline(ligne, 132)) { globalCylindreThick = getParam(ligne); }

	// [ACOUSTICRAYTRACER] Maximum events number for a ray
    if (params.getline(ligne, 132)) { globalMaxProfondeur = getParam(ligne); }

	// [ACOUSTICRAYTRACER] Allow ground reflections
    if (params.getline(ligne, 132)) { globalUseSol = getParam(ligne); }

	// [ACOUSTICRAYTRACER] Maximum reflections events for a ray
    if (params.getline(ligne, 132)) { globalMaxReflexion = getParam(ligne); }

	// [ACOUSTICRAYTRACER] Maximum diffraction events for a ray
    if (params.getline(ligne, 132)) { globalMaxDiffraction = getParam(ligne); }

	// [ACOUSTICRAYTRACER] Use random sampler instead of regular distribution 
	if (params.getline(ligne, 132)) { globalDiffractionUseRandomSampler = getParam(ligne); }

	// [ACOUSTICRAYTRACER] Number of ray thrown after diffraction (<0 = depends of sources, 0 = distance filter, >0 = forced)
    if (params.getline(ligne, 132)) { globalNbRayWithDiffraction = getParam(ligne); }

	// [ACOUSTICRAYTRACER] Drop down number of rays thrown after a diffraction
	if (params.getline(ligne, 132)) { globalDiffractionDropDownNbRays = getParam(ligne); }

	// [ACOUSTICRAYTRACER] Do not create rays outside the correct angle
	if (params.getline(ligne, 132)) { globalDiffractionFilterRayAtCreation = getParam(ligne); }

	// [ACOUSTICRAYTRACER] Allow use of path length difference validation
    if (params.getline(ligne, 132)) { globalUsePathDifValidation = getParam(ligne); }

	// [ACOUSTICRAYTRACER] Maximum path length difference in meter (25 meters for 25 dB, 8 meters for 20 dB)
    if (params.getline(ligne, 132)) { globalMaxPathDifference = getParam(ligne); }

	// [ACOUSTICRAYTRACER] Allow suppressing rays passing to far from the ridge
	if (params.getline(ligne, 132)) { globalDiffractionUseDistanceAsFilter = getParam(ligne); }

	// [ACOUSTICRAYTRACER] Keep invalid rays
    if (params.getline(ligne, 132)) { globalKeepDebugRay = getParam(ligne); }

	// [ACOUSTICRAYTRACER] Use of post-filters
    if (params.getline(ligne, 132)) { globalUsePostFilters = getParam(ligne); }	
	
////////////////////////////
// Targeting parameters
////////////////////////////

	// [ACOUSTICRAYTRACER] Use targeting
    if (params.getline(ligne, 132)) { globalEnableTargets = getParam(ligne); }

	// [ACOUSTICRAYTRACER] Sample ground in 2D
    if (params.getline(ligne, 132)) { globalSampleGround2D = getParam(ligne); }

	// [ACOUSTICRAYTRACER] Set target search after a diffuse event
    if (params.getline(ligne, 132)) { globalEnableFullTargets = getParam(ligne); }

	// [ACOUSTICRAYTRACER] Sampling density for interesting areas
    if (params.getline(ligne, 132)) { globalTargetsDensity = getParam(ligne); }

////////////////////////////
// AnalyticRayTracer
////////////////////////////

	// [ANALYTICRAYTRACER] Sampler model 1=horizontal, 2=vertical, 3=uniform v1, 4=uniform v2
    if (params.getline(ligne, 132)) { globalCurveRaySampler = getParam(ligne); }

	// [ANALYTICRAYTRACER] Start vertical angle (theta)
    if (params.getline(ligne, 132)) { globalInitialAngleTheta = getParam(ligne); }

	// [ANALYTICRAYTRACER] Final vertical angle (theta)
    if (params.getline(ligne, 132)) { globalFinalAngleTheta = getParam(ligne); }

	// [ANALYTICRAYTRACER] Start horizontal angle (phi)
    if (params.getline(ligne, 132)) { globalInitialAnglePhi = getParam(ligne); }

	// [ANALYTICRAYTRACER] Final horizontal angle (phi)
    if (params.getline(ligne, 132)) { globalFinalAnglePhi = getParam(ligne); }

	// [ANALYTICRAYTRACER] Number of rays per source
    if (params.getline(ligne, 132)) { globalAnalyticNbRay = getParam(ligne); }

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
    if (params.getline(ligne, 132)) { globalAnalyticTypeTransfo = getParam(ligne); }

	// [GEOM_TRANSFORMER] Altimetry refinement parameter
    if (params.getline(ligne, 132)) { globalMeshRefinementValue = getParam(ligne); }

	// [GEOM_TRANSFORMER] Restore modified altimetry after computing
    if (params.getline(ligne, 132)) { globalRestitModifiedGeom = getParam(ligne); }


/////////////////////////////
// Preprocessing
/////////////////////////////

	// [PREPROCESSING] Source-receptor minimal distance in meters
    if (params.getline(ligne, 132)) { globalMinSRDistance = getParam(ligne); }


/////////////////////////////
// Default Solver
/////////////////////////////

	// [DEFAULTSOLVER] Use of real ground (0) or totally reflective ground (1)
    if (params.getline(ligne, 132)) { globalUseRealGround = getParam(ligne); }

	// [DEFAULTSOLVER] Takes vegetation into account
    if (params.getline(ligne, 132)) { globalUseVegetation = getParam(ligne); }

	// [DEFAULTSOLVER] Takes screens into account
    if (params.getline(ligne, 132)) { globalUseScreen = getParam(ligne); }

	// [DEFAULTSOLVER] Lateral diffractions computing (if screens on)
    if (params.getline(ligne, 132)) { globalUseLateralDiffraction = getParam(ligne); }

	// [DEFAULTSOLVER] Takes reflections in account (first order only)
    if (params.getline(ligne, 132)) { globalUseReflection = getParam(ligne); }

	// [DEFAULTSOLVER] Propagation conditions (non refracting / downward conditions (ISO 9613))
    if (params.getline(ligne, 132)) { globalPropaConditions = getParam(ligne); }

	// [DEFAULTSOLVER] H1 parameter (ISO 9613 downward conditions)
    if (params.getline(ligne, 132)) { globalH1parameter = getParam(ligne); }

	// [DEFAULTSOLVER] Energetic (p² summation) or interference (p summation)
    if (params.getline(ligne, 132)) { globalModSummation = getParam(ligne); }


//////////////////////////////
// ANIME3D Solver
/////////////////////////////

	// [ANIME3D] Takes meteo in account
    if (params.getline(ligne, 132)) { globalUseMeteo = getParam(ligne); }

	// [ANIME3D] Rays oversampling rate (if meteo -see above-)
    if (params.getline(ligne, 132)) { globalOverSampleD = getParam(ligne); }

	// [ANIME3D] Use Fresnel area
    if (params.getline(ligne, 132)) { globalUseFresnelArea = getParam(ligne); }

	// [ANIME3D] Value of relative uncertainty
    if (params.getline(ligne, 132)) { globalAnime3DSigma = getParam(ligne); }

	// [ANIME3D] Force "C" parameter
    if (params.getline(ligne, 132)) { globalAnime3DForceC = getParam(ligne); }

////////////////////////////////
// Mode DEBUG
////////////////////////////////

	// [DEBUG] allow use of closeEventSelector
	if (params.getline(ligne, 132)) { globalDebugUseCloseEventSelector = getParam(ligne); }			
	
	// [DEBUG] allow use of diffraction angle selector
	if (params.getline(ligne, 132)) { globalDebugUseDiffractionAngleSelector = getParam(ligne); }	
	
	// [DEBUG] allow use of diffraction path length selector
	if (params.getline(ligne, 132)) { globalDebugUseDiffractionPathSelector = getParam(ligne); }		
	
	// [DEBUG] allow use of Fermat selector
	if (params.getline(ligne, 132)) { globalDebugUseFermatSelector = getParam(ligne); }				
	
	// [DEBUG] allow use of (doubled) faces selector
	if (params.getline(ligne, 132)) { globalDebugUseFaceSelector = getParam(ligne); }				


	params.close();

    return bRes;
}

bool TYANIME3DRayTracerSetup::postTreatmentScene(Scene* scene, std::vector<Source>& sources, std::vector<Recepteur>& recepteurs)
{
	selectorManagerValidation.addSelector( new LengthSelector<Ray>(globalMaxLength) );

	if (globalUsePostFilters)
	{
#ifdef _DEBUG
		if (globalDebugUseCloseEventSelector)
#endif
		selectorManagerValidation.addSelector( new CloseEventSelector<Ray>() );
#ifdef _DEBUG
		if (globalDebugUseDiffractionAngleSelector)
#endif
		selectorManagerValidation.addSelector( new DiffractionAngleSelector<Ray>() );
#ifdef _DEBUG
		if (globalDebugUseDiffractionPathSelector)
#endif
		selectorManagerValidation.addSelector( new DiffractionPathSelector<Ray>(globalMaxPathDifference) );
#ifdef _DEBUG
		if (globalDebugUseFermatSelector)
#endif
		selectorManagerValidation.addSelector( new FermatSelector<Ray>() ); 
#ifdef _DEBUG
		if (globalDebugUseFaceSelector)
#endif
		selectorManagerValidation.addSelector( new FaceSelector<Ray>(HISTORY_PRIMITIVE) );
	}
 
	selectorManagerIntersection.addSelector( new DiffractionSelector<Ray>(globalMaxDiffraction) );
    selectorManagerIntersection.addSelector( new ReflectionSelector<Ray>(globalMaxReflexion, globalUseSol) );

    // Ajoute des cylindres sur les arretes diffractantes
    PostTreatment::constructEdge(scene);

    return true;
}

bool TYANIME3DRayTracerSetup::valideIntersection(Ray* r, Intersection* inter)
{
    if (r->events.size() > static_cast<unsigned int>(globalMaxProfondeur)) { return false; }

    bool isValid = false;

    // cas d'un triangle (sol)
    if ( ( inter->forme == TRIANGLE ) &&
		 ( r->nbReflexion < static_cast<unsigned int>(globalMaxReflexion) ) &&
		 !( !globalUseSol && inter->p->isSol() ) )
    {
        isValid = ValidRay::validTriangleWithSpecularReflexion(r, inter);
    }

    // cas du cylindre (arrete de diffraction)
    else if (inter->forme == CYLINDRE && r->nbDiffraction < static_cast<unsigned int>(globalMaxDiffraction))
    {
        isValid = ValidRay::validCylindreWithDiffraction(r, inter);
    }

#ifdef _ALLOW_TARGETING_
    if (isValid && globalEnableFullTargets) { ValidRay::appendDirectionToEvent(r->events.back(), targetManager); }
#endif //_ALLOW_TARGETING_

    return (isValid); //(isValid && selectorManagerIntersection.appendData(r));
}

bool TYANIME3DRayTracerSetup::valideRayon(Ray* r)
{
    selectorManagerValidation.appendData(r);
#ifdef _DEBUG
	if (selectorManagerValidation.getSelectedData().size() % 1000 == 0 )
	{
		std::cout << "Nombre de rayon valides = " << selectorManagerValidation.getSelectedData().size()<< std::endl;
	}

#endif
    return true;
}

bool TYANIME3DRayTracerSetup::invalidRayon(Ray* r)
{
    if (!globalKeepDebugRay)
    {
        delete r;
		r = NULL;
    }
    else
    {
        debug_rays.push_back(r);
    }

    return true;
}

void TYANIME3DRayTracerSetup::finish()
{
    std::map<unsigned long long int, Ray*> selectedData = selectorManagerValidation.getSelectedData();

    for (std::map<unsigned long long, Ray*>::iterator it = selectedData.begin(); it != selectedData.end(); it++)
    {
        valid_rays.push_back(it->second);
    }

	selectorManagerIntersection.reset();
	selectorManagerValidation.reset();

    return;
}
