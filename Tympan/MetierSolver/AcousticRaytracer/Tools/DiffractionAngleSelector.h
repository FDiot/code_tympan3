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

template<typename T>
class DiffractionAngleSelector : public Selector<T>
{

public :
    DiffractionAngleSelector() : Selector<T>() {}
    virtual Selector<T>* Copy()
    {
        DiffractionAngleSelector* newSelector = new DiffractionAngleSelector();
        newSelector->setIsDeletable(this->deletable);
        return newSelector;
    }

    virtual SELECTOR_RESPOND canBeInserted(T* r, unsigned long long& replace)
    {
        vector<QSharedPointer<Event> >* events = r->getEvents();

        if ( (events->size() == 0) || (r->getDiff() == 0) ) { return SELECTOR_ACCEPT; }
		
		vec3 beginPos = r->getSource()->getPosition();
		vec3 endPos = static_cast<Recepteur*> (r->getRecepteur())->getPosition();
		vec3 currentPos, nextPos, normale, from, to;
		Diffraction *pDiff = NULL;

		vector<QSharedPointer<Event> >::iterator iter = events->begin();
		do
		{
			if ( (*iter)->getType() != DIFFRACTION ) { iter++; continue; }

			currentPos = (*iter)->getPosition();

			pDiff = dynamic_cast<Diffraction*>( (*iter).data() );
			normale = pDiff->getRepere().getU();

			from = (currentPos - beginPos);
			from.normalize();

			if ( (iter+1) != events->end() )
			{
				nextPos = (*(iter+1)).data()->getPosition();
			}
			else
			{
				nextPos = static_cast<Recepteur*> (r->getRecepteur())->getPosition();
			}

			to = (currentPos - nextPos);
			to.normalize();

			if ( (from * normale) < 0. || (to * normale) < 0. ) { return SELECTOR_REJECT; }

			beginPos = currentPos;
			iter++;
		}
		while( iter != events->end() );

		return SELECTOR_ACCEPT;
    }

	virtual void insert(T* r, unsigned long long& replace) { return; }

    virtual bool insertWithTest(T* r)
    {
        vector<QSharedPointer<Event> >* events = r->getEvents();

        if ( (events->size() == 0) || (r->getDiff() == 0) ) { return true; }
		
		vec3 beginPos = r->getSource()->getPosition();
		vec3 endPos = static_cast<Recepteur*> (r->getRecepteur())->getPosition();
		vec3 currentPos, nextPos, normale, from, to;
		Diffraction *pDiff = NULL;

		vector<QSharedPointer<Event> >::iterator iter = events->begin();
		do
		{
			if ( (*iter)->getType() != DIFFRACTION ) { iter++; continue; }

			currentPos = (*iter)->getPosition();

			pDiff = dynamic_cast<Diffraction*>( (*iter).data() );
			normale = pDiff->getRepere().getU();

			from = (currentPos - beginPos);
			from.normalize();

			if ( (iter+1) != events->end() )
			{
				nextPos = (*(iter+1)).data()->getPosition();
			}
			else
			{
				nextPos = static_cast<Recepteur*> (r->getRecepteur())->getPosition();
			}

			to = (currentPos - nextPos);
			to.normalize();

			if ( (from * normale) > 0. && (to * normale) > 0. ) { return false; }

			beginPos = currentPos;
			iter++;
		}
		while( iter != events->end() );

		return true;
    }
};

#endif //DIFFRACTION_ANGLE_SELECTOR
