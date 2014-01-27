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

		// Pour chaque rayon
		for (unsigned int i = 0; i < _tabRay->size(); i++)
		{
			vec3 closestPoint;
			Ray *ray = _tabRay->at(i);
			decimal trueLength = ray->computeTrueLength(closestPoint);
			decimal epaisseur = ray->getThickness( trueLength, ray->getSolidAngle() );
			decimal closestDistance = static_cast<Recepteur*> ( ray->getRecepteur() )->getPosition().distance(closestPoint);
			if ( closestDistance > (epaisseur /2.) )
			{
				// TO DO : suppression du rayon
			}
		}

		return 0; 
	}

protected:

};

#endif //FERMAT_POST_FILTER_H