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

#ifndef CLEANER_SELECTOR
#define CLEANER_SELECTOR

#include "Selector.h"
#include "Tympan/MetierSolver/AcousticRaytracer/global.h"

template<typename T>
class CleanerSelector : public Selector<T>
{

public :
    CleanerSelector() : Selector<T>()
    {
    }

    virtual Selector<T>* Copy()
    {
        CleanerSelector* newSelector = new CleanerSelector();
        newSelector->setIsDeletable(this->deletable);
        return newSelector;
    }

    virtual SELECTOR_RESPOND canBeInserted(T* r, unsigned long long& replace)
    {
        vector< QSharedPointer<Event> > *events = r->getEvents();
		if (events->size() == 0) { return SELECTOR_ACCEPT; }
		
		vector< QSharedPointer<Event> >::iterator it = events->begin();
		while(it != events->end())
		{
			if ( (*it)->getType() == NOTHING )
			{
				it  = events->erase(it);
				continue;
			}
			
			it++;
		}
		
        return SELECTOR_REPLACE;
    }

    virtual void insert(T* r) { return; }
    virtual bool insertWithTest(T* r)
    {
         vector< QSharedPointer<Event> > *events = r->getEvents();
		if (events->size() == 0) { return true; }
		
		vector< QSharedPointer<Event> >::iterator it = events->begin();
		while(it != events->end())
		{
			if ( (*it)->getType() == NOTHING )
			{
				it  = events->erase(it);
				continue;
			}
			
			it++;
		}
	
        return true;
    }

protected:
};

#endif //CLEANER_SELECTOR
