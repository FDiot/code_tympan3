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

#ifndef DIFFRACTION_ANGLE_SELECTOR
#define DIFFRACTION_ANGLE_SELECTOR

#include "Selector.h"
#include "Geometry/3d.h"

/*!
 * \brief : Select diffracted rays that are launched in the shadow zone of the obstacle (closed angle)
 *          Other are suppressed
 */

template<typename T>

class DiffractionAngleSelector : public Selector<T>
{
public :
	/// Constructor
    DiffractionAngleSelector() : Selector<T>() {}
    virtual Selector<T>* Copy()
    {
        DiffractionAngleSelector* newSelector = new DiffractionAngleSelector();
        newSelector->setIsDeletable(this->deletable);
        return newSelector;
    }

    virtual SELECTOR_RESPOND canBeInserted(T* r, unsigned long long& replace)
    {
        vector<std::shared_ptr<Event> >* events = r->getEvents();

        //The ray has is accepted if it has no diffraction event
        if ( (events->size() == 0) || (r->getDiff() == 0) ) { return SELECTOR_ACCEPT; }
		
		vec3 beginPos = r->getSource()->getPosition();
		vec3 currentPos, nextPos, N, N1, N2, W, From, To;

		Diffraction *pDiff = NULL;

		decimal F1 = 0., F2 = 0., T1 = 0., T2 = 0., FT = 0.;

		vector<std::shared_ptr<Event> >::iterator iter = events->begin();
		do
		{
			if ( (*iter)->getType() != DIFFRACTION ) 
			{ 
				beginPos = (*iter)->getPosition();
				iter++; 
				continue; 
			}

			currentPos = (*iter)->getPosition();

			pDiff = dynamic_cast<Diffraction*>( (*iter).get() );


			N1 = dynamic_cast<Cylindre*>(pDiff->getShape())->getFirstShape()->getNormal();
			N2 = dynamic_cast<Cylindre*>(pDiff->getShape())->getSecondShape()->getNormal();
			
			From = (currentPos - beginPos);
			From.normalize();
			
			if ( (iter+1) != events->end() )
			{
				nextPos = (*(iter+1)).get()->getPosition();
			}
			else
			{
				nextPos = static_cast<Recepteur*> (r->getRecepteur())->getPosition();
			}

			To = (nextPos - currentPos);
			To.normalize();

			FT = From * To;

			//Accept the ray if FROM and TO are colinear and point in the same direction
			if ( ( 1. - FT ) < EPSILON_4 ) { return SELECTOR_ACCEPT; } 

			//Reject the ray if FROM and TO point in opposite directions
			if ( FT < 0. ) { return SELECTOR_REJECT; }  

			F1 = From * N1;
			F2 = From * N2;

			//Reject the ray if its incoming direction is such that there is no shadow zone
			if ( (F1 * F2) > 0.) { return SELECTOR_REJECT; } 

			T1 = To * N1;
			T2 = To * N2;

			//Reject the ray if TO is not in the shadow zone of the obstacle
			if ( (F1 <= 0.) && ( (T1 > EPSILON_4 ) || ( (T2 - F2) > EPSILON_4 ) ) )
			{ 
				return SELECTOR_REJECT; 
			}

			if ( (F2 <= 0.) && ( ( T2 > EPSILON_4 ) || ( (T1 - F1) > EPSILON_4 ) ) )
			{ 
				return SELECTOR_REJECT; 
			}

			beginPos = currentPos;
			iter++;
		}
		while( iter != events->end() );

		return SELECTOR_ACCEPT;
    }
    /// Select the ray
	virtual void insert(T* r, unsigned long long& replace) { return; }

    virtual bool insertWithTest(T* r)
    {
        vector<std::shared_ptr<Event> >* events = r->getEvents();

        if ( (events->size() == 0) || (r->getDiff() == 0) ) { return true; }
		
		vec3 beginPos = r->getSource()->getPosition();
		vec3 currentPos, nextPos, N, N1, N2, W, From, To;

		Diffraction *pDiff = NULL;

		decimal F1 = 0., F2 = 0., T1 = 0., T2 = 0., FT = 0.;

		vector<std::shared_ptr<Event> >::iterator iter = events->begin();
		do
		{
			if ( (*iter)->getType() != DIFFRACTION ) 
			{ 
				beginPos = (*iter)->getPosition();
				iter++; 
				continue; 
			}

			currentPos = (*iter)->getPosition();

			pDiff = dynamic_cast<Diffraction*>( (*iter).get() );

			N1 = dynamic_cast<Cylindre*>(pDiff->getShape())->getFirstShape()->getNormal();
			N2 = dynamic_cast<Cylindre*>(pDiff->getShape())->getSecondShape()->getNormal();
			
			From = (currentPos - beginPos);
			From.normalize();
			
			if ( (iter+1) != events->end() )
			{
				nextPos = (*(iter+1)).get()->getPosition();
			}
			else
			{
				nextPos = static_cast<Recepteur*> (r->getRecepteur())->getPosition();
			}

			To = (nextPos - currentPos);
			To.normalize();

			FT = From * To;

			//Return true if FROM and TO are colinear and point in the same direction
			if ( ( 1. - FT ) < EPSILON_4 ) { return true; } 

			//Return false if FROM and TO point in opposite directions
			if ( FT < 0. ) { return false; }  

			F1 = From * N1;
			F2 = From * N2;

			//Return false if its incoming direction is such that there is no shadow zone
			if ( (F1 * F2) > 0.) { return false; } 

			T1 = To * N1;
			T2 = To * N2;

			//Return false if TO is not in the shadow zone of the obstacle
			if ( (F1 <= 0.) && ( (T1 > EPSILON_4 ) || ( (T2 - F2) > EPSILON_4 ) ) )
			{ 
				return false; 
			}

			if ( (F2 <= 0.) && ( ( T2 > EPSILON_4 ) || ( (T1 - F1) > EPSILON_4 ) ) )
			{ 
				return false; 
			}

			beginPos = currentPos;
			iter++;
		}
		while( iter != events->end() );

		return true;
	}

	/**
	* \brief Return the class type of the selector
	*/
	virtual const char* getSelectorName(){
		return typeid(this).name();
	}
};

#endif //DIFFRACTION_ANGLE_SELECTOR
