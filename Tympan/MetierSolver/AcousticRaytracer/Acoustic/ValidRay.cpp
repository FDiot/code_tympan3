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

    vec3 impact = r->position + r->direction * inter->t;
    Cylindre* cylindre = (Cylindre*)(inter->p);
    vec3 realImpact = impact.closestPointOnSegment(cylindre->getVertices()->at(cylindre->getLocalVertices()->at(0)), cylindre->getVertices()->at(cylindre->getLocalVertices()->at(1)));

    vec3 from = realImpact - r->position;
    from.normalize();

    Diffraction* newEvent = new Diffraction(realImpact, from, (Cylindre*)(inter->p));
    vec3 newDir;
    if (newEvent->getResponse(newDir))
    {
        r->position = realImpact;
        r->direction = newDir;
        r->events.push_back(QSharedPointer<Event>(newEvent));
        //          newEvent->setNbResponseLeft(200);
        newEvent->setNbResponseLeft(globalNbRayWithDiffraction);
        r->nbDiffraction += 1;

        return true;
    }
    return false;
}
