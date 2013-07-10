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
#include "Tympan/MetierSolver/AcousticRaytracer/Acoustic/event.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Acoustic/Recepteur.h"
#include "Ray.h"

void Ray::computeLongueur()
{
    if (source == NULL)
    {
        std::cerr << "Erreur : rayon sans source dans computeLongueur" << endl;
        return;
    }
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
	// The source id is stored in 12 bits, receptor is stored in 20 bits
	unsigned int SR = source->getId(), SD = 0;
	assert(SR < 4096);
	SR = SR << 20;
	unsigned int R = (static_cast<Recepteur*>(recepteur))->getId();
	assert(R < 1048576);

	SR += R;

	for (unsigned int i=0; i<events.size(); i++)
	{
		SD = SD << 1;

		if ( events.at(i)->getType() == DIFFRACTION) { SD++; }
	}

	if (typeEv == SPECULARREFLEXION) 
	{
		SD = core_mathlib::buildComplementaryBitSet(getNbEvents(), SD);
	}

	return std::make_pair(SR, SD);
}
