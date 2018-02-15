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
#include "Geometry/mathlib.h"
#include "Geometry/Cylindre.h"
#include "Acoustic/Event.h"
#include "Acoustic/Recepteur.h"
#include "Ray.h"

void Ray::computeLongueur()
{
    if (source == NULL)
    {
        std::cerr << "Error : ray without source or receptor computeLongueur" << endl;
        return;
    }

    longueur = 0;

    if (events.size()==0) //No events
    {

            if (recepteur == NULL)
            {
                //If the ray has not reached a receptor, the length is 0
                longueur = 0;
                return;

            }else{
                
                //If the ray has reached a receptor, the length is the distance between the source dans the receptor
                vec3 posSource = vec3(source->getPosition());
                vec3 posRecep = vec3(((Recepteur*)(recepteur))->getPosition());
                longueur = posSource.distance(posRecep);
                return;
            }

    }else{ //Ray has some events
        
            longueur = 0;

            // Distance from source to first event
            longueur = source->getPosition().distance( events.front()->getPosition() );

            // Add length of events sequence
            longueur += computeEventsSequenceLength();

            // If the ray has reached a receptor, add the distance from the last event to the receptor
            if (recepteur != NULL)
            {
                vec3 posLastEvent = vec3(events.back()->getPosition());
                vec3 posRecepteur = vec3(((Recepteur*)(recepteur))->getPosition());
                longueur += posLastEvent.distance(posRecepteur);
            }
            return;
         
    }
}

decimal Ray::computeEventsSequenceLength()
{
    // Sum the distance between each event
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
        // Ray has no events
        case 0: 
            // Compute the projection of ref on the line passing by posSource and lastPos
			closestPoint = ref.closestPointOnLine(posSource, lastPos);

            // Compute the distance between the source and closestPoint
            return posSource.distance(closestPoint);
            break;

        // Ray has some events
        default:
			// Distance from source to first event
            length = source->getPosition().distance( events.front()->getPosition() );

			// Add length of events sequence
			length += computeEventsSequenceLength();

			// Find last event's position
			posLastEvent = events.back()->getPosition();

            // Compute the projection of ref on the line passing by posLastEvent and lastPos
			closestPoint = ref.closestPointOnLine(posLastEvent, lastPos);

            // Add the distance between closestPoint and the position of the last event to the length and return the trueLength
			return length += closestPoint.distance(posLastEvent);

            break;
    }

    return 0.;
}

decimal Ray::computePertinentLength(const vec3& ref, const vec3& lastPos, vec3& closestPoint)
{
    if (source == NULL)
    {
        std::cerr << "Error : ray without source (computePertinentLength)" << endl;
        return 0.;
    }

    decimal pertinent_length = 0.;

    vec3 posLastEvent;
    vec3 current(0., 0., 0.), previous(0., 0., 0.);

    Source* s = NULL;
    Event* e = NULL;

    // Get the last diffraction event, or source if theray has no diffraction event
	Base *pertinentEvent = getLastPertinentEventOrSource();

    switch (events.size())
    {
        case 0: 
            // if no the ray has no events, simply compute the full (true) length
            return computeTrueLength(ref, lastPos, closestPoint);
            break;

        default:
            s = dynamic_cast<Source*>(pertinentEvent);

            // if pertinentEvent is the source, simply compute the full (true) length
            if (s)
            {
               
				return computeTrueLength(ref, lastPos, closestPoint);
            }

            // if pertinentEvent is not the source
            else
            {

                // Recover the  pertinent event
                e = dynamic_cast<Event*>(pertinentEvent);

                if (e)
                {
                    
                    // Get the last event (begining of the reverse iterator)
                    std::vector< std::shared_ptr<Event> >::reverse_iterator rit = events.rbegin();
                    // Get the position of the lastEvent
                    previous = (*rit)->getPosition();

                    // Compute the length from the last event to pertinentEvent
                    while ((rit != events.rend()) && ((*rit).get() != e))
                    {
                        rit++;
                        current = (*rit)->getPosition();
                        pertinent_length += current.distance(previous);
                        previous = current;
                    }

                    // Get the position of last event
                    posLastEvent = vec3(events.back()->getPosition());
                    // Compute the projection of ref on the line passing by posLastEvent and lastPos
					closestPoint = ref.closestPointOnLine(posLastEvent, lastPos);

                    // Return the distance from the last event to the projection of ref
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
