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

template<typename T>
class FermatSelector : public Selector<T>
{
public :
    FermatSelector() : Selector<T>() {}
    virtual Selector<T>* Copy()
    {
        FermatSelector* newSelector = new FermatSelector();
        newSelector->setIsDeletable(this->deletable);
        return newSelector;
    }

    virtual SELECTOR_RESPOND canBeInserted(T* r, unsigned long long& replace)
    {
		vec3 receptorPos( static_cast<Recepteur*> (r->recepteur)->getPosition() );
		vec3 finalPos( r->finalPosition );

		vec3 closestPoint;
		decimal trueLength = r->computeTrueLength(receptorPos, finalPos, closestPoint);

		decimal epaisseur = r->getThickness( trueLength, false );
		decimal closestDistance = static_cast<Recepteur*> ( r->getRecepteur() )->getPosition().distance(closestPoint);
		if ( closestDistance >= ( epaisseur/2. * 1.05 ) ) // Ajout de 5 % de marge supplémentaire 
		{
			return SELECTOR_REJECT;
		}

		return SELECTOR_ACCEPT;
	}

    virtual void insert(T* r, unsigned long long& replace) { return; }

    virtual bool insertWithTest(T* r)
    {
		vec3 receptorPos( static_cast<Recepteur*> (r->recepteur)->getPosition() );
		vec3 finalPos( r->finalPosition );

		vec3 closestPoint;
		decimal trueLength = r->computeTrueLength(receptorPos, finalPos, closestPoint);

		decimal epaisseur = r->getThickness( trueLength, false );
		decimal closestDistance = static_cast<Recepteur*> ( r->getRecepteur() )->getPosition().distance(closestPoint);
		if ( closestDistance >= ( epaisseur/2. * 1.05 ) ) // Ajout de 5 % de marge supplémentaire 
		{
			return false;
		}

		return true;
    }

protected:
}; // FERMAT_SELECTOR

#endif
