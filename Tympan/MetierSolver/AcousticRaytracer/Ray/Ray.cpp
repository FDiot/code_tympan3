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

#include <cassert>
#include <vector>
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/mathlib.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Acoustic/Event.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Acoustic/Recepteur.h"
#include "Ray.h"

void Ray::computeLongueur()
{
    if (source == NULL)
    {
        std::cerr << "Erreur : rayon sans source dans computeLongueur" << endl;
        return;
    }

    longueur = 0;
    switch (events.size())
    {
        case 0:

            if (recepteur == NULL)
            {
                longueur = 0;
                return;
            }
            else
            {
                vec3 posSource = vec3(source->getPosition());
                vec3 posRecep = vec3(((Recepteur*)(recepteur))->getPosition());
                longueur = posSource.distance(posRecep);
                return;
            }
            break;

        case 1:
            if (recepteur == NULL)
            {
                vec3 posSource = vec3(source->getPosition());
                vec3 posEvent = vec3(events.at(0)->getPosition());
                longueur = posSource.distance(posEvent);
                return;
            }
            else
            {
                vec3 posSource = vec3(source->getPosition());
                vec3 posEvent = vec3(events.at(0)->getPosition());
                vec3 posRecep = vec3(((Recepteur*)(recepteur))->getPosition());
                longueur = posSource.distance(posEvent) + posEvent.distance(posRecep);
                return;
            }
            break;

        default:
            longueur = 0;
            vec3 posSource = vec3(source->getPosition());
            vec3 posCurrentEvent = vec3(events.at(0)->getPosition());
            vec3 posNextEvent = vec3(events.at(1)->getPosition());
            longueur += posSource.distance(posCurrentEvent);
            for (unsigned int i = 0; i < events.size() - 1; i++)
            {
                posCurrentEvent = vec3(events.at(i)->getPosition());
                posNextEvent = vec3(events.at(i + 1)->getPosition());
                longueur += posCurrentEvent.distance(posNextEvent);
            }
            if (recepteur != NULL)
            {
                vec3 posLastEvent = vec3(events.back()->getPosition());
                vec3 posRecepteur = vec3(((Recepteur*)(recepteur))->getPosition());
                longueur += posLastEvent.distance(posRecepteur);
            }
            return;
            break;
    }
}

decimal Ray::computeTrueLength(vec3& closestPoint)
{
    if (source == NULL || recepteur == NULL)
    {
        std::cerr << "Erreur : rayon without source or receptor computeTrueLength" << endl;
        return 0.;
    }

    longueur = 0.;
    vec3 posSource = source->getPosition();
    vec3 posRecep = static_cast<Recepteur*>(recepteur)->getPosition();
    vec3 posLastEvent;
    vec3 current(0., 0., 0.), previous(0., 0., 0.);
    switch (events.size())
    {
        case 0: // Chemin direct source-recepteur
            closestPoint = posRecep.closestPointOnLine(posSource, finalPosition);
            longueur = posSource.distance(closestPoint);
            return longueur;
            break;

        default:
            longueur = 0;
            previous = source->getPosition();

            // Compute distance from source to the last event
            for (std::vector< QSharedPointer<Event> > :: iterator iter = events.begin(); iter != events.end(); ++iter)
            {
                current = (*iter)->getPosition();
                longueur += current.distance(previous);
                previous = current;
            }

            // Compute distance from the last event to the nearest point from receptor
            posLastEvent = vec3(events.back()->getPosition());
            closestPoint = posRecep.closestPointOnLine(posLastEvent, finalPosition);
            longueur += closestPoint.distance(posLastEvent);
            return longueur;

            break;
    }

    return 0.;
}

decimal Ray::computePertinentLength(vec3& closestPoint)
{
    if (source == NULL || recepteur == NULL)
    {
        std::cerr << "Erreur : rayon without source or receptor computeTrueLength" << endl;
        return 0.;
    }

    decimal pertinent_length = 0.;
    vec3 posRecep = static_cast<Recepteur*>(recepteur)->getPosition();
    vec3 posLastEvent;
    vec3 current(0., 0., 0.), previous(0., 0., 0.);

    Source* s = NULL;
    Event* e = NULL;
    Base* beginEvent = getLastPertinentEvent();

    switch (events.size())
    {
        case 0: // Chemin direct source-recepteur
            // needs only computing of full (true) length
            return computeTrueLength(closestPoint);
            break;

        default:
            s = dynamic_cast<Source*>(beginEvent);

            // Compute distance from source to the last event
            if (s)
            {
                // needs only computing of full (true) length
                return computeTrueLength(closestPoint);
            }
            else
            {
                e = dynamic_cast<Event*>(beginEvent);
                if (e)
                {
                    // compute legth from end of vector
                    std::vector< QSharedPointer<Event> >::reverse_iterator rit = events.rbegin();
                    previous = (*rit)->getPosition();
                    while ((rit != events.rend()) && ((*rit).data() != e))
                    {
                        rit++;
                        current = (*rit)->getPosition();
                        pertinent_length += current.distance(previous);
                        previous = current;
                    }

                    // Compute distance from the last event to the nearest point from receptor
                    posLastEvent = vec3(events.back()->getPosition());
                    closestPoint = posRecep.closestPointOnLine(posLastEvent, finalPosition);
                    return (pertinent_length += closestPoint.distance(posLastEvent));
                }
            }

            break;
    }

    return 0.;
}

Base* Ray::getLastPertinentEvent()
{
    Base* res = (Base*) source;

    for (std::vector< QSharedPointer<Event> > :: iterator iter = events.begin(); iter != events.end(); ++iter)
    {
        if ((*iter)->getType() == DIFFRACTION) { res = (Base*)((*iter).data()); }
    }

    return res;
}

float Ray::distanceSourceRecepteur()
{
    vec3 pos_s = source->getPosition();
    vec3 pos_r = static_cast<Recepteur*>(recepteur)->getPosition();

    if (events.size() < 1)  // Rayon avec aucun evenement
    {
        return sqrt((pos_s.x - pos_r.x) * (pos_s.x - pos_r.x) + (pos_s.y - pos_r.y) * (pos_s.y - pos_r.y));
    }

    else if (events.size() == 1) // Rayon avec 1 evenement
    {
        vec3 impact = events.at(0)->getPosition();
        return sqrt((pos_s.x - impact.x) * (pos_s.x - impact.x) + (pos_s.y - impact.y) * (pos_s.y - impact.y)) +
               sqrt((pos_r.x - impact.x) * (pos_r.x - impact.x) + (pos_r.y - impact.y) * (pos_r.y - impact.y));
    }
    else // Rayon avec plusieurs evenements
    {
        float distance = 0.0;
        vec3 impact, impact2;

        for (unsigned int i = 0; i < events.size() - 1; i++)
        {
            impact = events.at(i)->getPosition();
            impact2 = events.at(i + 1)->getPosition();
            distance += sqrt((impact.x - impact2.x) * (impact.x - impact2.x) + (impact.y - impact2.y) * (impact.y - impact2.y));
        }
        impact = events.at(0)->getPosition();
        distance += sqrt((pos_s.x - impact.x) * (pos_s.x - impact.x) + (pos_s.y - impact.y) * (pos_s.y - impact.y));

        impact = events.at(events.size() - 1)->getPosition();
        distance += sqrt((pos_r.x - impact.x) * (pos_r.x - impact.x) + (pos_r.y - impact.y) * (pos_r.y - impact.y));

        return distance;
    }
}

std::vector<unsigned int> Ray::getFaceHistory()
{
    vector<unsigned int> result;
    result.push_back(source->getId());
    for (unsigned int i = 0; i < events.size(); i++)
    {
        result.push_back(events.at(i)->getShape()->getFaceId());
    }

    result.push_back(((Recepteur*)recepteur)->getId());
    return result;
}

std::vector<unsigned int> Ray::getPrimitiveHistory()
{
    vector<unsigned int> result;
    result.push_back(source->getId());
    for (unsigned int i = 0; i < events.size(); i++)
    {
        result.push_back(events.at(i)->getShape()->getPrimitiveId());
    }

    result.push_back(((Recepteur*)recepteur)->getId());
    return result;
}

signature Ray::getSignature(const typeevent& typeEv)
{
    bitSet SR = getSRBitSet(source->getId(), (static_cast<Recepteur*>(recepteur))->getId());
    bitSet SD = getEventsBitSet(typeEv);

    return std::make_pair(SR, SD);
}

bitSet Ray::getEventsBitSet(const typeevent& typeEv)
{
    bitSet SD = 0;
    for (size_t i = 0; i < events.size(); i++)
    {
        SD = SD << 1;

        if (events.at(i)->getType() == typeEv) { SD++; }
    }

    return SD;
}
