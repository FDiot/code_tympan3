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

#include "ValidRay.h"
#include "SpecularReflexion.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/Cylindre.h"
#include "Diffraction.h"
#include "Tympan/MetierSolver/AcousticRaytracer/global.h"

void ValidRay::appendDirectionToEvent(QSharedPointer<Event> e, TargetManager& targets)
{
    std::vector<vec3> &ponctualTargets = targets.getTargets();

    std::vector<vec3> acceptableTargets;
    for (unsigned int i = 0; i < ponctualTargets.size(); i++)
    {
        vec3 response = ponctualTargets.at(i) - e->getPosition();
        response.normalize();
        if (e->isAcceptableResponse(response))
        {
            acceptableTargets.push_back(response);
        }
    }

    if (acceptableTargets.size() < static_cast<unsigned int>(e->getNbResponseLeft()))
    {
        for (unsigned int i = 0; i < acceptableTargets.size(); i++)
        {
            e->appendTarget(acceptableTargets.at(i), true);
        }
    }
    else
    {
        for (unsigned int i = 0; i < static_cast<unsigned int>(e->getNbResponseLeft()); i++)
        {
            int index = rand() % e->getNbResponseLeft();
            e->appendTarget(acceptableTargets.at(index), true);
        }
    }
}

bool ValidRay::validTriangleWithSpecularReflexion(Ray* r, Intersection* inter)
{
	vec3 impact = r->position + r->direction * inter->t;
    vec3 normale = inter->p->getNormal(impact);
    if (normale.dot(r->direction) > 0.) { return false; }

	if (globalUsePathDifValidation) // Validation sur la différence de marche due aux diffractions
	{
		vec3 previousPos;
		if ( r->getEvents()->size() )
		{
			previousPos = r->getEvents()->back()->getPosition();
		}
		else
		{
			previousPos = r->getSource()->getPosition();
		}

		r->cumulDistance += previousPos.distance(impact);
		vec3 origin = r->computeLocalOrigin( r->getLastPertinentEventOrSource(SPECULARREFLEXION) );
		
		// We compute the true path length difference between actual position and the last reflection or source
		r->cumulDelta += ( r->cumulDistance - impact.distance(origin) ); 
		r->cumulDistance = 0.;

		if (r->cumulDelta > globalMaxPathDifference) { return false; }
	}    
	    
	SpecularReflexion* newEvent = new SpecularReflexion(impact, r->direction, inter->p);

    vec3 newDir;
    if (newEvent->getResponse(newDir))
    {
        r->position = r->position + r->direction * inter->t;
        r->direction = newDir;
        r->direction.normalize();
        r->events.push_back(QSharedPointer<Event>(newEvent));
        r->nbReflexion += 1;
        return true;
    }


	delete newEvent;
    return false;
}

bool ValidRay::isPathLengthDifferenceValid(Ray *r, const vec3& impact, const decimal& maxDiff)
{
		vec3 previousPos;
		if ( r->getEvents()->size() )
		{
			previousPos = r->getEvents()->back()->getPosition();
		}
		else
		{
			previousPos = r->getSource()->getPosition();
		}

		r->cumulDistance += previousPos.distance(impact);
		vec3 origin = r->computeLocalOrigin( r->getLastPertinentEventOrSource(SPECULARREFLEXION) );
		
		// We compute the true path length difference between actual position and the last reflection or source
		decimal currentCumulDelta = r->cumulDelta + ( r->cumulDistance - impact.distance(origin) ); 

		if ( currentCumulDelta > globalMaxPathDifference ) { return false; }

		return true;
}

bool ValidRay::computeRealImpact(Ray *r, Intersection* inter, Cylindre *cylindre, vec3& impact)
{
	vec3 p1 = r->position;
	vec3 p2 = r->position + r->direction * inter->t * 2.;
	// Define second segment
	vec3 p3 = cylindre->getVertices()->at(cylindre->getLocalVertices()->at(0));
	vec3 p4 = cylindre->getVertices()->at(cylindre->getLocalVertices()->at(1));

	// shortest segment definition
	vec3 *pa = new vec3(), *pb = new vec3();
	decimal *mua = new decimal(), *mub = new decimal();
	int res = LineLineIntersect(p1, p2, p3, p4, pa, pb, mua, mub);

	delete pa, pb, mua, mub; // Cleaning

	if (!res) { return false; }

	impact = *pb;

	return true;
}

bool ValidRay::isRayClosestFromRidge(Ray *r, const vec3& impact, const vec3& realImpact)
{
	vec3 closestPoint;
	decimal length = r->computePertinentLength(realImpact, impact, closestPoint);
	decimal thick = r->getThickness(length, true);
	decimal closestDistance = realImpact.distance(closestPoint);

	if ( closestDistance > ( thick / 2. ) ) { return false; }

	return true;
}

bool ValidRay::validCylindreWithDiffraction(Ray* r, Intersection* inter)
{
    Cylindre* cylindre = (Cylindre*)(inter->p);

	vec3 impact = r->position + r->direction * inter->t;

// Test if creating a new diffraction is allowed (depends on path length difference)
	if ( globalUsePathDifValidation && !isPathLengthDifferenceValid(r, impact, globalMaxPathDifference) )
	{
		return false;
	}   

// Compute real impact of the ray on the ridge
	// Define first segment
	vec3 realImpact;
	if ( !computeRealImpact(r, inter, cylindre, realImpact) ) { return false; }

// Valid creation of event using distance from the ridge (if needed)
	if ( globalDiffractionUseDistanceAsFilter && !isRayClosestFromRidge(r, impact, realImpact) ) { return false; }
	
// Create diffraction event
    vec3 from = realImpact - r->position;
    from.normalize();
	Diffraction* newEvent = new Diffraction(realImpact, from, (Cylindre*)(inter->p));
	QSharedPointer<Event> SPEv(newEvent);

// define the number of rays to throw
	unsigned int diff_nb_rays = 0;
	if ( globalNbRayWithDiffraction > 0 )
	{
		diff_nb_rays = globalNbRayWithDiffraction; 
	}
	else
	{
		diff_nb_rays = r->getSource()->getSampler()->computeDiffractionNbr(M_PIDIV2 - newEvent->getAngle());
	}

// Reduce (if needed) number of rays thrown depending on the diffraction order 
	if ( globalDiffractionDropDownNbRays )
	{
		diff_nb_rays = diff_nb_rays / pow( static_cast<float>(2), static_cast<int>(r->nbDiffraction) );
	}

	diff_nb_rays += 1; // DO NOT MOVE/CONCATENATE THIS OPERATION ON UPPER LINES !!!
    newEvent->setNbResponseLeft(diff_nb_rays); // Set the number of ray to throw

// Add the event to the list
	vec3 newDir;
    if (newEvent->getResponse(newDir))
    {
        r->position = realImpact;
        r->direction = newDir;
        r->events.push_back(SPEv);
        r->nbDiffraction += 1;

        return true;
    }

	return false;
}
