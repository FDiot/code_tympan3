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

#include "Engine/AcousticRaytracerConfiguration.h"
#include "Geometry/Cylindre.h"
#include "Geometry/Sphere.h"
#include "ValidRay.h"
#include "SpecularReflexion.h"
#include "Diffraction.h"
#include "DoNothing.h"

bool ValidRay::validRayWithDoNothingEvent(Ray *r, Intersection* inter)
{
	vec3 impact = r->getPosition() + r->getDirection() * inter->t;
	DoNothing *newEvent = new DoNothing(impact, r->getDirection(), inter->p);
	std::shared_ptr<Event> SPEv(newEvent);

    vec3 newDir;
	//Check if the event can provide a response (should always be true in the case of a newly created DoNothing event)
    if (newEvent->getResponse(newDir))
    {
		//update position of the ray and add the event to the list of events
        r->setPosition(impact);
		newDir.normalize();
        r->setDirection(newDir); //ray's direction = event's response
		r->addEvent(SPEv);
        return true;
	}

	return false;
}

bool ValidRay::validTriangleWithSpecularReflexion(Ray* r, Intersection* inter)
{
    vec3 impact = r->getPosition() + r->getDirection() * inter->t;
    vec3 normal = inter->p->getNormal(impact);
    
    //intersection is not valid if the ray goes in the same direction as the normal of the intersected shape
    if (normal.dot(r->getDirection()) > 0.) { return false; }


	if (AcousticRaytracerConfiguration::get()->UsePathDifValidation && !pathDiffValidationForReflection(r, impact))
	{
		return false;
	}

	SpecularReflexion* newEvent = new SpecularReflexion(impact, r->getDirection(), inter->p);
	std::shared_ptr<Event> SPEv(newEvent);

    vec3 newDir;

	//if the newly created event can provide a response
    if (newEvent->getResponse(newDir))
    {
		//update the ray's position, direction and add the new event to the ray's list of events
        r->setPosition( r->getPosition() + r->getDirection() * inter->t);
		newDir.normalize();
        r->setDirection( newDir ); //ray's direction = event's response
		r->addEvent(SPEv);
		r->setNbReflexion (r->getReflex()+1);
        return true;
    }

	//no response could be provided by the new event => intersection is not valid
    return false;
}

//Computes distance between impact and last event/source and adds it to the cumulDistance
void ValidRay::computeCumulDistance(Ray *r, const vec3& impact)
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

	r->setCumulDistance(r->getCumulDistance()  + previousPos.distance(impact) );
	
}


bool ValidRay::pathDiffValidationForReflection(Ray * r, const vec3& impact)
{
	vec3 lastReflexionPos = r->computeLocalOrigin( r->getLastPertinentEventOrSource(SPECULARREFLEXION));
	computeCumulDistance(r, impact);

	// Compute the difference between :
	//    -  the cumulative length since the last valid reflection
	//		and
	//    -  the euclidian distance between impact and the last reflection
	decimal delta= r->getCumulDistance() - impact.distance(lastReflexionPos);

	//Add the difference to the cumulative delta
	r->setCumulDelta (r->getCumulDelta()+delta) ;

	//Reset cumulDistance
	r->setCumulDistance( 0. );

	//If the path difference is below the MaxPathDifference threshold, return true, else return false
	return ( r->getCumulDelta() <= AcousticRaytracerConfiguration::get()->MaxPathDifference );
}

bool ValidRay::pathDiffValidationForDiffraction(Ray *r, const vec3& impact)
{
	vec3 lastReflexionPos = r->computeLocalOrigin( r->getLastPertinentEventOrSource(SPECULARREFLEXION) );
	computeCumulDistance(r, impact);

	// Compute the difference between :
	//    -  the cumulative length since the last valid reflection
	//		and
	//    -  the euclidian distance between impact and the last reflection
	decimal delta= r->getCumulDistance() - impact.distance(lastReflexionPos);

	//Sum of cumulDelta and delta (Note: cumulDelta is not modified)
	decimal currentCumulDelta = r->getCumulDelta() + delta;

	//If the path difference is below the MaxPathDifference threshold, return true, else return false
	return ( currentCumulDelta <= AcousticRaytracerConfiguration::get()->MaxPathDifference );
}

bool ValidRay::computeRealImpact(Ray *r, Intersection* inter, Cylindre *cylindre, vec3& impact)
{
	//First segment: 2 times the ray's incoming direction
	vec3 p1 = r->getPosition();
	vec3 p2 = r->getPosition() + r->getDirection() * inter->t * 2.;

	//Second segment: diffraction edge
	vec3 p3 = cylindre->getVertices()->at(cylindre->getLocalVertices()->at(0));
	vec3 p4 = cylindre->getVertices()->at(cylindre->getLocalVertices()->at(1));

	//Find the shortest segment S between the ray and the diffraction edge
	vec3 *pa = new vec3(); //extremity of S on the ray
	vec3 *pb = new vec3(); //extremity of S on the diffraction edge

	decimal *mua = new decimal(); //position of a along the ray
	decimal *mub = new decimal(); //position of b along the diffraction edge

	int res = LineLineIntersect(p1, p2, p3, p4, pa, pb, mua, mub);

	impact = *pb; // pb corrsponds to the impact of the ray on the diffraction edge

    delete pa;
    delete pb;
    delete mua;
    delete mub;

	return res;
}

bool ValidRay::isRayPassesNearRidge(Ray *r, const vec3& impact, const vec3& realImpact)
{
	vec3 closestPoint;
	//compute length from last reflection to the closest point on the ray from realImpact
	decimal length = r->computePertinentLength(realImpact, impact, closestPoint);

	//compute the thickness of the diffracted ray 
	decimal thick = r->getThickness(length, true);

	//compute the distance between the realImpact and the closestPoint 
	decimal closestDistance = realImpact.distance(closestPoint);

    //return true if the closest point on the ray from realImpact is within the thickness of the ray 
	return ( closestDistance <= ( thick / 2. ) );
}

bool ValidRay::validCylindreWithDiffraction(Ray* r, Intersection* inter)
{
    Cylindre* cylindre = (Cylindre*)(inter->p);

	vec3 impact = r->getPosition() + r->getDirection() * inter->t;

        AcousticRaytracerConfiguration* config = AcousticRaytracerConfiguration::get();
// Test if creating a new diffraction is allowed (depends on path length difference)
	if ( config->UsePathDifValidation && !pathDiffValidationForDiffraction(r, impact) )
	{
		return false;
	}

// Compute the real impact of the ray on the ridge
	// Define first segment
	vec3 realImpact;
	if ( !computeRealImpact(r, inter, cylindre, realImpact) ) { return false; }

// Validate the creation of event using distance from the ridge (if needed)
	if ( config->DiffractionUseDistanceAsFilter && !isRayPassesNearRidge(r, impact, realImpact) )
	{
		return ValidRay::validRayWithDoNothingEvent(r, inter);
	}

// Create a diffraction event
    vec3 from = realImpact - r->getPosition();
    from.normalize();
	Diffraction* newEvent = new Diffraction(realImpact, from, (Cylindre*)(inter->p));
	std::shared_ptr<Event> SPEv(newEvent);

// Define the number of rays to throw
	unsigned int diff_nb_rays = 0;
	if ( config->NbRayWithDiffraction > 0 )
	{
		diff_nb_rays = config->NbRayWithDiffraction;
	}
	else
	{
		diff_nb_rays = (unsigned int)(r->getSource()->getSampler()->computeDiffractionNbr((decimal)(M_PIDIV2 - newEvent->getAngle())));
	}

// Reduce (if needed) number of rays thrown depending on the diffraction order
	if ( config->DiffractionDropDownNbRays )
	{
		diff_nb_rays = (unsigned int)(diff_nb_rays / pow( static_cast<float>(2), static_cast<int>(r->getDiff())));
	}

	diff_nb_rays += 1; // DO NOT MOVE/CONCATENATE THIS OPERATION ON UPPER LINES !!!
    newEvent->setNbResponseLeft(diff_nb_rays); // Set the number of rays to throw

// Add the event to the list
	r->addEvent(SPEv);
	r->setNbDiffraction(r->getDiff()+1);
    vec3 newDir;
    if (newEvent->getResponse(newDir))
    {
        r->setPosition(realImpact);
        r->setDirection(newDir);
 
        return true;
    }

    return true;
}
#ifdef _ALLOW_TARGETING_
void ValidRay::appendDirectionToEvent(std::shared_ptr<Event> e, TargetManager& targets)
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
#endif // _ALLOW_TARGETING_
