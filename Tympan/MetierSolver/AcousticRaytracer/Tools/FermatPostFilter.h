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

#include <map>
#include "Tympan/MetierSolver/AcousticRaytracer/Ray/Ray.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Acoustic/Recepteur.h"

#ifndef FERMAT_POST_FILTER_H
#define FERMAT_POST_FILTER_H

class fermatPostFilter : public postFilter
{
public:
	fermatPostFilter(std::vector<Ray*> *tabRay) : postFilter(tabRay){}
	~fermatPostFilter() { _tabRay = NULL; }
	
	/*!
	 * \fn unsigned int Process();
	 * \brief apply a filter to the group of valid rays found by ray tracing
	 * \return number of rays suppressed
	 */
	virtual unsigned int Process() 
	{
		decimal ray_length = 0.0;
		unsigned int suppressed = 0;

		// Pour chaque rayon
		std::vector<Ray*>::iterator iter = _tabRay->begin();
		while( iter != _tabRay->end() )
		{
			vec3 closestPoint;
			Ray *ray = (*iter);

#ifdef _DEBUG
			size_t nbEvents = ray->getEvents()->size();
#endif

			vec3 receptorPos( static_cast<Recepteur*> (ray->recepteur)->getPosition() );
			vec3 finalPos( ray->finalPosition );
#ifdef _FJ_THICKNESS_
			decimal trueLength = ray->computePertinentLength(receptorPos, finalPos, closestPoint);
#else
			decimal trueLength = ray->computeTrueLength(receptorPos, finalPos, closestPoint);
#endif
			decimal epaisseur = ray->getThickness( trueLength);
			decimal closestDistance = static_cast<Recepteur*> ( ray->getRecepteur() )->getPosition().distance(closestPoint);
			if ( closestDistance >= ( epaisseur/2. * 1.05 ) ) 
			{
				iter = _tabRay->erase(iter);
				suppressed ++;
				continue;
			}

			iter++;
		}

// TEST
		// At this point similar rays are very close. Anyone is good enough 
		// to use in acoustic computing. So we decide to delete all duplicates
		// rays, using a map.
		map < vector<unsigned int>, Ray* > mapRays;
		for (unsigned int i = 0; i<_tabRay->size(); i++)
		{
			vector<unsigned int> tab = _tabRay->at(i)->getPrimitiveHistory();
			mapRays[tab] = _tabRay->at(i);
		}

		// Keep only rays on the map
		_tabRay->clear();
		map < vector<unsigned int>, Ray* >::iterator it;
		for (it = mapRays.begin(); it != mapRays.end(); ++it)
		{
			_tabRay->push_back( (*it).second );
		}

// END TEST
		return suppressed; 
	}

protected:

};

#endif //FERMAT_POST_FILTER_H