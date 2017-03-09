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
#include "Tympan/models/common/mathlib.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Cylindre.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Acoustic/Event.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Acoustic/Recepteur.h"
#include "Ray.h"

void Ray::computeLongueur()
{
    if (source == NULL)
    {
        std::cerr << "Error : ray without source or receptor computeLongueur" << endl;
        return;
    }

    longueur = 0;
    // Sum distance between each events (starting from the source). Add the distance between
    // last event and the receptor if any.
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

decimal Ray::computeEventsSequenceLength()
{
    // Sum distance between each event
	decimal length = 0;

	if ( events.size() == 0 ) { return 0; }

	std::vector< std::shared_ptr<Event> > :: iterator iter = events.begin();

	vec3 previous = (*iter)->getPosition();
	vec3 current(0., 0., 0.);

	iter++;
	while( iter != events.end() )
	{
		current = (*iter)->getPosition();
		length += current.distance(previous);
		previous = current;
		iter++;
	};

	return length;
}

decimal Ray::computeTrueLength( const vec3& ref, const vec3& lastPos, vec3& closestPoint )
{
    if (source == NULL)
    {
        std::cerr << "Error : ray without source or receptor computeTrueLength" << endl;
        return 0.;
    }

	decimal length = 0.;
    vec3 posSource = source->getPosition();
    vec3 posLastEvent;
    switch (events.size())
    {
        case 0: // Chemin direct source-recepteur
			closestPoint = ref.closestPointOnLine(posSource, lastPos);
            return posSource.distance(closestPoint);
            break;

        default:
			// Distance from source to first event
            length = source->getPosition().distance( events.front()->getPosition() );

			// Add length of events sequence
			length += computeEventsSequenceLength();

			// Compute distance from the last event to the nearest point from ref
			posLastEvent = events.back()->getPosition();
			closestPoint = ref.closestPointOnLine(posLastEvent, lastPos);
			return length += closestPoint.distance(posLastEvent);

            break;
    }

    return 0.;
}

decimal Ray::computePertinentLength(const vec3& ref, const vec3& lastPos, vec3& closestPoint)
{
    if (source == NULL)
    {
        std::cerr << "Error : ray without source or receptor computePertinentLength" << endl;
        return 0.;
    }

    decimal pertinent_length = 0.;

    vec3 posLastEvent;
    vec3 current(0., 0., 0.), previous(0., 0., 0.);

    Source* s = NULL;
    Event* e = NULL;
	Base *beginEvent = getLastPertinentEventOrSource();

    switch (events.size())
    {
        case 0: // Chemin direct source-recepteur
            // needs only computing of full (true) length
            return computeTrueLength(ref, lastPos, closestPoint);
            break;

        default:
            s = dynamic_cast<Source*>(beginEvent);

            // Compute distance from source to the last event
            if (s)
            {
                // needs only computing of full (true) length
				return computeTrueLength(ref, lastPos, closestPoint);
            }
            else
            {
                e = dynamic_cast<Event*>(beginEvent);
                if (e)
                {
                    // compute length from end of vector
                    std::vector< std::shared_ptr<Event> >::reverse_iterator rit = events.rbegin();
                    previous = (*rit)->getPosition();
                    while ((rit != events.rend()) && ((*rit).get() != e))
                    {
                        rit++;
                        current = (*rit)->getPosition();
                        pertinent_length += current.distance(previous);
                        previous = current;
                    }

                    // Compute distance from the last event to the nearest point from receptor
                    posLastEvent = vec3(events.back()->getPosition());
					closestPoint = ref.closestPointOnLine(posLastEvent, lastPos);
                    return (pertinent_length += closestPoint.distance(posLastEvent));
                }
            }

            break;
    }

    return 0.;
}

Base* Ray::getLastPertinentEventOrSource(typeevent evType)
{
    Base* res = (Base*) source;

    for (std::vector< std::shared_ptr<Event> > :: iterator iter = events.begin(); iter != events.end(); ++iter)
    {
		if ( (*iter)->getType() == evType ) { res = (Base*) ( (*iter).get() ); }
    }

    return res;
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

decimal Ray::getThickness( const decimal& distance, bool diffraction)
{
	decimal angle = getSolidAngle( diffraction );

	if ( diffraction )
	{
		return distance * angle;
	}

	return 2. * distance * sqrt( angle / M_PI );
}

decimal Ray::getSolidAngle( bool &diffraction)
{
	unsigned int nb_rays = source->getInitialRayCount();

	if (diffraction)
	{
		Base *last = getLastPertinentEventOrSource();
		Event *e = dynamic_cast<Event*>(last);

		if ( e && ( e->getType() == DIFFRACTION ) )
		{
			return dynamic_cast<Diffraction*>(e)->getAngle() * M_2PI / e->getInitialNbResponseLeft();
		}
		else // else is done to be explicit
		{
			diffraction = false;
		}
	}

	return M_4PI / static_cast<decimal>(nb_rays);
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
