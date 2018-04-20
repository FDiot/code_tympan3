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

#ifndef DIFFRACTION_PATH_SELECTOR
#define DIFFRACTION_PATH_SELECTOR

#include "Selector.h"

/*!
 * \brief : Rejects rays if the cumulative length added by the diffractions events in comparison to 
 * the length of the direct path between reflections (i.e, ignoring diffractions) exceeds some threshold
 */
template<typename T>
class DiffractionPathSelector : public Selector<T>
{
public :
	/// Constructor
    DiffractionPathSelector(double _maximumDelta = 8) : Selector<T>() { maximumDelta = _maximumDelta; }
    virtual Selector<T>* Copy()
    {
        DiffractionPathSelector* newSelector = new DiffractionPathSelector(maximumDelta);
        newSelector->setIsDeletable(this->deletable);
        return newSelector;
    }

    virtual SELECTOR_RESPOND canBeInserted(T* r, unsigned long long& replace)
    {
        vector< std::shared_ptr<Event> > *events = r->getEvents();
		if (events->size() == 0) { return SELECTOR_ACCEPT; }


		decimal cumul_delta = 0.;	 //Cumulative distance added by the diffractions
		decimal cumul_distance = 0.; //Cumulative distance since last reflection

		bool notLast = true;
		Recepteur *pRecep = static_cast<Recepteur*>( r->getRecepteur() );
		Source *pSource = r->getSource();

		vec3 origin = pRecep->getPosition();
		vec3 current_pos = origin;

		// Iterate other the list of events in REVERSE order
		vector< std::shared_ptr<Event> >::reverse_iterator rit = events->rbegin();
		while(rit != events->rend())
		{
			cumul_distance += (*rit)->getPosition().distance(current_pos);
			//if current event is a reflection
			if ( (*rit)->getType() == SPECULARREFLEXION ) 
			{
				//accumulate the difference between :
				//	- the cumulative distance since last reflection 
				//	and
				//	- the direct distance between the current position and the last reflection
				//(Note: the difference between cumul_distance and the direct distance is zero if no diffraction as been encountered)
				decimal direct_distance= origin.distance( (*rit)->getPosition());
				cumul_delta += ( cumul_distance - direct_distance );
				
				//Reset cumul_distance
				cumul_distance = 0;

				//Save the position of the current reflection
				origin = (*rit)->getPosition();
				notLast = false;
			}
			//if the current event is a diffraction 
			else
			{
				
				notLast = true;
			}

			current_pos = (*rit)->getPosition(); 
			rit ++;
		};

		//if the first event is a diffraction finish the computation with the source 
		if (notLast) 
		{ 
			//Accumulate the distance from the current position to the source
			cumul_distance += current_pos.distance( pSource->getPosition() );
			//Computer the direct distance between the source and the first reflection
			decimal direct_distance= origin.distance( pSource->getPosition());
			//Compute the difference between the cumulative and the direct distances
			cumul_delta += ( cumul_distance - direct_distance );
		}
 
 
        if ( cumul_delta > maximumDelta )
        {
			return SELECTOR_REJECT;
        }

        return SELECTOR_ACCEPT;
    }
    /// Select the ray
    virtual void insert(T* r, unsigned long long& replace) { return; }

    virtual bool insertWithTest(T* r)
    {
        vector< std::shared_ptr<Event> > *events = r->getEvents();
		
		if (events->size() == 0) { return true; }

		decimal cumul_delta = 0.;		//Cumulative distance added by the diffractions
		decimal cumul_distance = 0.; 	//Cumulative distance since last reflection
		bool notLast = true;
		
		Recepteur *pRecep = static_cast<Recepteur*>( r->getRecepteur() );
		Source *pSource = r->getSource();

		vec3 origin = pRecep->getPosition();
		vec3 current_pos = origin;

		// Iterate other the list of events in REVERSE order
		vector< std::shared_ptr<Event> >::reverse_iterator rit = events->rbegin();
		while(rit != events->rend())
		{
			cumul_distance += (*rit)->getPosition().distance(current_pos);
			//if current event is a reflection
			if ( (*rit)->getType() == SPECULARREFLEXION ) 
			{
				//accumulate the difference between :
				//	- the cumulative distance since last reflection 
				//	and
				//	- the direct distance between the current position and the last reflection
				//(Note: the difference between cumul_distance and the direct distance is zero if no diffraction as been encountered)
				decimal direct_distance= origin.distance( (*rit)->getPosition());
				cumul_delta += ( cumul_distance - direct_distance );
				
				//Reset cumul_distance
				cumul_distance = 0;

				//Save the position of the current reflection
				origin = (*rit)->getPosition();
				notLast = false;
			}
			//if the current event is a diffraction 
			else
			{
				
				notLast = true;
			}

			current_pos = (*rit)->getPosition(); 
			rit ++;
		};

		//if the first event is a diffraction finish the computation with the source 
		if (notLast) 
		{ 
			//Accumulate the distance from the current position to the source
			cumul_distance += current_pos.distance( pSource->getPosition() );
			//Computer the direct distance between the source and the first reflection
			decimal direct_distance= origin.distance( pSource->getPosition());
			//Compute the difference between the cumulative and the direct distances
			cumul_delta += ( cumul_distance - direct_distance );
		}
 
        if ( cumul_delta > maximumDelta )
        {
			return false;
        }

        return true;
    }
    /**
    * \brief Get maximumDelta
    */  
    double getMaximumDelta() { return maximumDelta; }

    /**
    * \brief Set maximumDelta
    */  
    void setMaximumDelta(double _maximumDelta) { this->maximumDelta = _maximumDelta; }


protected:
    double maximumDelta; //!< Maximal path length difference between rays produced by diffraction
};

#endif // DIFFRACTION_PATH_SELECTOR
