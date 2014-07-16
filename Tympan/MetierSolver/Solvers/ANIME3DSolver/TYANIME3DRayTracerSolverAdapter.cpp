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
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/CleanerSelector.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/SelectorManager.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/Logger.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Acoustic/Solver.h"

#include "TYANIME3DRayTracerSolverAdapter.h"

bool TYANIME3DRayTracerSolverAdapter::postTreatmentScene(Scene* scene, std::vector<Source>& sources, std::vector<Recepteur>& recepteurs)
{
	selectorManagerValidation.addSelector( new CleanerSelector<Ray>() );
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

bool TYANIME3DRayTracerSolverAdapter::valideIntersection(Ray* r, Intersection* inter)
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

bool TYANIME3DRayTracerSolverAdapter::valideRayon(Ray* r)
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

bool TYANIME3DRayTracerSolverAdapter::invalidRayon(Ray* r)
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

void TYANIME3DRayTracerSolverAdapter::finish()
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
