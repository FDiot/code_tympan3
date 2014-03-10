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
	if (r->getReflex() >= static_cast<unsigned int>( globalMaxReflexion )) { return false; }

	if (inter->p->getMaterial()->isNatural) { return false; }
    
	vec3 impact = r->position + r->direction * inter->t;
    vec3 normale = inter->p->getNormal(impact);
    if (normale.dot(r->direction) > 0.) { return false; }

    SpecularReflexion* newEvent = new SpecularReflexion(impact, r->direction, inter->p);

    vec3 newDir;
    if (newEvent->getResponse(newDir))
    {
        r->position = r->position + r->direction * inter->t;
        //std::cout<<"Ancienne direction : ("<<r->direction.x<<","<<r->direction.y<<","<<r->direction.z<<")"<<std::endl;
        r->direction = newDir;
        r->direction.normalize();
        //std::cout<<"Nouvelle direction : ("<<r->direction.x<<","<<r->direction.y<<","<<r->direction.z<<")"<<std::endl;
        //std::cout<<"Nouvelle position : ("<<r->position.x<<","<<r->position.y<<","<<r->position.z<<")"<<std::endl;
        r->events.push_back(QSharedPointer<Event>(newEvent));
        r->nbReflexion += 1;
        //std::cout<<"Validation d'une reflexion"<<std::endl;
        return true;
    }

    delete newEvent;
    return false;
}

bool ValidRay::validCylindreWithDiffraction(Ray* r, Intersection* inter)
{
	if (r->getDiff() >= static_cast<unsigned int>( globalMaxDiffraction )) { return false; }

    Cylindre* cylindre = (Cylindre*)(inter->p);


#define _TEST_IMPACT
#ifdef _TEST_IMPACT

	vec3 impact = r->position + r->direction * inter->t;

	// Define first segment
	vec3 p1 = r->position;
	vec3 p2 = r->position + r->direction * inter->t * 2.;
	// Define second segment
	vec3 p3 = cylindre->getVertices()->at(cylindre->getLocalVertices()->at(0));
	vec3 p4 = cylindre->getVertices()->at(cylindre->getLocalVertices()->at(1));

	// shortest segment definition
	vec3 *pa = new vec3(), *pb = new vec3();
	decimal *mua = new decimal(), *mub = new decimal();
	int res = LineLineIntersect(p1, p2, p3, p4, pa, pb, mua, mub);
	if (!res) { return false; }

	vec3 realImpact = *pb;

	delete pa, pb, mua, mub; // Cleaning
	
	//vec3 realImpact = impact.closestPointOnLine(cylindre->getVertices()->at(cylindre->getLocalVertices()->at(0)), cylindre->getVertices()->at(cylindre->getLocalVertices()->at(1)));

#else

	vec3 impact = r->position + r->direction * inter->t.;

    vec3 realImpact = impact.closestPointOnSegment(cylindre->getVertices()->at(cylindre->getLocalVertices()->at(0)), cylindre->getVertices()->at(cylindre->getLocalVertices()->at(1)));

#endif

    vec3 from = realImpact - r->position;
    from.normalize();

    Diffraction* newEvent = new Diffraction(realImpact, from, (Cylindre*)(inter->p));

	if ( globalNbRayWithDiffraction > 0 )
	{
		newEvent->setNbResponseLeft(globalNbRayWithDiffraction+1); // Attempt to correct problem 
	}
	else if ( globalNbRayWithDiffraction == 0 )
	{
		vec3 closestPoint;

#ifdef _FJ_THICKNESS_
		decimal length = r->computePertinentLength(realImpact, impact, closestPoint);
#else
		decimal length = r->computeTrueLength(realImpact, impact, closestPoint);
#endif
		decimal thick = r->getThickness(length);
		decimal closestDistance = realImpact.distance(closestPoint);

		if ( closestDistance < ( thick / 2. ) ) 
		{
			unsigned int diff_nb_rays = r->getSource()->getSampler()->computeDiffractionNbr(M_PIDIV2 - newEvent->getAngle()) + 1;
			newEvent->setNbResponseLeft(diff_nb_rays);
		}
		else
		{
			return false;
		}
	}
	else
	{
		unsigned int diff_nb_rays = r->getSource()->getSampler()->computeDiffractionNbr(M_PIDIV2 - newEvent->getAngle()) + 1;
		newEvent->setNbResponseLeft(diff_nb_rays);
	}
    
	vec3 newDir;
    if (newEvent->getResponse(newDir))
    {
        r->position = realImpact;
        r->direction = newDir;
        r->events.push_back(QSharedPointer<Event>(newEvent));
        //          newEvent->setNbResponseLeft(200);
        r->nbDiffraction += 1;

        return true;
    }

    return false;
}
