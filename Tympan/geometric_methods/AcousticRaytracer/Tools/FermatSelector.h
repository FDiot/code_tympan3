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

#ifndef FERMAT_SELECTOR
#define FERMAT_SELECTOR

#include "Selector.h"

/*!
 * \brief : Rays can be seen as long cones that get thicker as their length increases based on their associated solidAngle.
 			This selector rejects rays if the receptor they hit does not lie in the thickness of the ray. 
            (depending on the number of rays launched by the source and the distance covered by the ray).
 */
template<typename T>
class FermatSelector : public Selector<T>
{
public :
	/// Constructor
    FermatSelector() : Selector<T>() {}
    virtual Selector<T>* Copy()
    {
        FermatSelector* newSelector = new FermatSelector();
        newSelector->setIsDeletable(this->deletable);
        return newSelector;
    }

    virtual SELECTOR_RESPOND canBeInserted(T* r, unsigned long long& replace)
    {
		vec3 receptorPos( static_cast<Recepteur*> (r->getRecepteur())->getPosition() );
		vec3 finalPos( r->getFinalPosition() );

		vec3 closestPoint;
		//find closest position (closestPoint) from the receptor on the segment receptorPos-finalPos and compute the trueLength (ray length from source to closestPoint)
		decimal trueLength = r->computeTrueLength(receptorPos, finalPos, closestPoint);

		//compute the thickness of the ray for the previsouly computed trueLength
		decimal epaisseur = r->getThickness( trueLength, false );

		//compute the distance from  the receptor and its closest point on the ray
		decimal closestDistance = static_cast<Recepteur*> ( r->getRecepteur() )->getPosition().distance(closestPoint);

		//ray should be rejected if the receptor is outside the cone of the ray (i.e thickness/2 <= closestsDistance)
		if ( closestDistance >= ( epaisseur/2. ) ) 
		{
			return SELECTOR_REJECT;
		}

		return SELECTOR_ACCEPT;
	}
    /// Keep the ray
    virtual void insert(T* r, unsigned long long& replace) { return; }

    virtual bool insertWithTest(T* r)
    {
		vec3 receptorPos( static_cast<Recepteur*> (r->getRecepteur())->getPosition() );
		vec3 finalPos( r->getFinalPosition() );

		vec3 closestPoint;
		//find closest position (closestPoint) from the receptor on the segment receptorPos-finalPos ray and compute the trueLength (ray length from source to closestPoint)
		decimal trueLength = r->computeTrueLength(receptorPos, finalPos, closestPoint);

		//compute the thickness of the ray for the previsouly computed trueLength
		decimal epaisseur = r->getThickness( trueLength, false );

		//compute the distance from  the receptor and its closest point on the ray
		decimal closestDistance = static_cast<Recepteur*> ( r->getRecepteur() )->getPosition().distance(closestPoint);

		//ray should be rejected if the receptor is outside the cone of the ray (i.e thickness/2 <= closestsDistance)
		if ( closestDistance >= ( epaisseur/2.) ) 
		{
			return false;
		}

		return true;
    }

protected:
}; // FERMAT_SELECTOR

#endif
