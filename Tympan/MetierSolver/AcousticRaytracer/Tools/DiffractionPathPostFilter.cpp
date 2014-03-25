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

#include <vector>
#include <deque>

#include "Geometry/mathlib.h"
#include "Geometry/Cylindre.h"
#include "Acoustic/Event.h"
#include "Ray/Ray.h"
#include "PostFilter.h"
#include "DiffractionPathPostFilter.h"
#include "Acoustic/Source.h"
#include "Acoustic/Recepteur.h"

using namespace std;

unsigned int diffractionPathPostFilter::Process()
{
    unsigned int nbSuppress = 0;
    bool bstatus = true;

    deque<Ray*>::iterator it = _tabRay->begin();
    while (it != _tabRay->end())
    {
        bstatus = true;

        vector< QSharedPointer<Event> > *events = (*it)->getEvents();
		if (events->size() == 0) { it++; continue; }


		decimal cumul_delta = 0.;
		decimal cumul_distance = 0.;
		bool notLast = true;
		Recepteur *pRecep = static_cast<Recepteur*>( (*it)->recepteur );
		Source *pSource = (*it)->source;

		vec3 origin = pRecep->getPosition();
		vec3 current_pos = origin;

		vector< QSharedPointer<Event> >::reverse_iterator rit = events->rbegin();
		while(rit != events->rend())
		{
			cumul_distance += (*rit)->getPosition().distance(current_pos);
			if ( (*rit)->getType() == SPECULARREFLEXION ) 
			{
				cumul_delta += ( cumul_distance - origin.distance( (*rit)->getPosition() ) );
				cumul_distance = 0;
				origin = (*rit)->getPosition();
				notLast = false;
			}
			else
			{
				notLast = true;
			}

			current_pos = (*rit)->getPosition(); 
			rit ++;
		};

		if (notLast) 
		{ 
			cumul_distance += current_pos.distance( pSource->getPosition() );
			cumul_delta += ( cumul_distance - origin.distance( pSource->getPosition() ) );
		}
 
        if ( cumul_delta > globalMaxPathDifference )
        {
			delete (*it);
			(*it) = NULL;
            it = _tabRay->erase(it);
            nbSuppress++;
        }
        else
        {
            it++;
        }
    };

    return nbSuppress;
}