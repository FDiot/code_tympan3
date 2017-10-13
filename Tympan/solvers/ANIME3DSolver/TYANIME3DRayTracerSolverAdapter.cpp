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

#include "Tympan/models/solver/config.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Acoustic/ValidRay.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Acoustic/PostTreatment.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Tools/FaceSelector.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Tools/LengthSelector.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Tools/DiffractionSelector.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Tools/ReflectionSelector.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Tools/CloseEventSelector.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Tools/FermatSelector.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Tools/DiffractionPathSelector.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Tools/DiffractionAngleSelector.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Tools/CleanerSelector.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Tools/SelectorManager.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Tools/Logger.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Acoustic/Solver.h"

#include "TYANIME3DRayTracerSolverAdapter.h"

bool TYANIME3DRayTracerSolverAdapter::postTreatmentScene(Scene* scene, std::vector<Source>& sources, std::vector<Recepteur>& recepteurs)
{
    tympan::LPSolverConfiguration config = tympan::SolverConfiguration::get();
	selectorManagerValidation.addSelector( new CleanerSelector<Ray>() );
	selectorManagerValidation.addSelector( new LengthSelector<Ray>(config->MaxLength) );

	if (config->UsePostFilters)
	{
        if (config->DebugUseCloseEventSelector) { selectorManagerValidation.addSelector( new CloseEventSelector<Ray>() ); }
        if (config->DebugUseDiffractionAngleSelector) { selectorManagerValidation.addSelector(new DiffractionAngleSelector<Ray>()); }
        if (config->DebugUseDiffractionPathSelector) { selectorManagerValidation.addSelector(new DiffractionPathSelector<Ray>(config->MaxPathDifference)); }
        if (config->DebugUseFermatSelector) { selectorManagerValidation.addSelector( new FermatSelector<Ray>() ); }
        if (config->DebugUseFaceSelector) { selectorManagerValidation.addSelector( new FaceSelector<Ray>(HISTORY_PRIMITIVE) ); }
	}

	selectorManagerIntersection.addSelector( new DiffractionSelector<Ray>(config->MaxDiffraction) );
    selectorManagerIntersection.addSelector( new ReflectionSelector<Ray>(config->MaxReflexion, config->UseSol) );

    // Ajoute des cylindres sur les arretes diffractantes
    PostTreatment::constructEdge(scene);

    return true;
}

bool TYANIME3DRayTracerSolverAdapter::valideIntersection(Ray* r, Intersection* inter)
{
    tympan::LPSolverConfiguration config = tympan::SolverConfiguration::get();
    if (r->events.size() > static_cast<unsigned int>(config->MaxProfondeur)) { return false; }

    bool isValid = false;

    // cas d'un triangle
    if ( ( inter->forme == TRIANGLE ) &&
		 ( r->nbReflexion < static_cast<unsigned int>(config->MaxReflexion) ) &&
		 !( !config->UseSol && inter->p->isSol() ) )
    {
        isValid = ValidRay::validTriangleWithSpecularReflexion(r, inter);
    }

    // cas du cylindre (arrete de diffraction)
    else if (inter->forme == CYLINDRE && r->nbDiffraction < static_cast<unsigned int>(config->MaxDiffraction))
    {
        isValid = ValidRay::validCylindreWithDiffraction(r, inter);
    }

#ifdef _ALLOW_TARGETING_
    if (isValid && config->EnableFullTargets) { ValidRay::appendDirectionToEvent(r->events.back(), targetManager); }
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
    if (!tympan::SolverConfiguration::get()->KeepDebugRay)
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
