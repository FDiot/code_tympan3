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

#ifndef CLOSE_EVENT_SELECTOR
#define CLOSE_EVENT_SELECTOR

#include "Tympan/solvers/AcousticRaytracer/Geometry/Cylindre.h"

#include "Selector.h"

/*!
 * \brief To reject a ray if two different events of this one occur on the same shape
 *        (for example a diffraction close to a reflection)
 */
template<typename T>
class CloseEventSelector : public Selector<T>
{
public :
	/// Constructor
    CloseEventSelector() : Selector<T>() {}
    virtual Selector<T>* Copy()
    {
        CloseEventSelector* newSelector = new CloseEventSelector();
        newSelector->setIsDeletable(this->deletable);
        return newSelector;
    }

    virtual SELECTOR_RESPOND canBeInserted(T* r, unsigned long long& replace)
    {
        vector<QSharedPointer<Event> >* events = r->getEvents();

        if (events->size() < 2) { return SELECTOR_ACCEPT; }

        for (unsigned int i = 0 ; i < events->size() - 1 ; i++)
        {
            QSharedPointer<Event> ev1 = events->at(i);
            QSharedPointer<Event> ev2 = events->at(i + 1);
            int type1 = events->at(i)->getType();
            int type2 = events->at(i + 1)->getType();
            Shape* sh1 = NULL, *sh2 = NULL, *sh3 = NULL;

            // if events type are different and occurs on same shape, the ray is suppressed
            // NB  : Diffraction event take into account two faces
            if ((type1 != type2))
            {
                if (type1 == DIFFRACTION)
                {
                    sh1 = dynamic_cast<Cylindre*>(ev1->getShape())->getFirstShape();
                    sh2 = dynamic_cast<Cylindre*>(ev1->getShape())->getSecondShape();
                    sh3 = ev2->getShape();
                }
                else
                {
                    sh1 = dynamic_cast<Cylindre*>(ev2->getShape())->getFirstShape();
                    sh2 = dynamic_cast<Cylindre*>(ev2->getShape())->getSecondShape();
                    sh3 = ev1->getShape();
                }

                if ((sh3 == sh1) || (sh3 == sh2)) { return SELECTOR_REJECT; }
            }
        }
		
        return SELECTOR_ACCEPT;
    }
	
    virtual bool insertWithTest(T* r)
    {
        vector<QSharedPointer<Event> >* events = r->getEvents();

        if (events->size() < 2) { return true; }

        for (unsigned int i = 0 ; i < events->size() - 1 ; i++)
        {
            QSharedPointer<Event> ev1 = events->at(i);
            QSharedPointer<Event> ev2 = events->at(i + 1);
            int type1 = events->at(i)->getType();
            int type2 = events->at(i + 1)->getType();
            Shape* sh1 = NULL, *sh2 = NULL, *sh3 = NULL;

            // if events type are different and occurs on same shape, the ray is suppressed
            // NB  : Diffraction event take into account two faces
            if ((type1 != type2))
            {
                if (type1 == DIFFRACTION)
                {
                    sh1 = dynamic_cast<Cylindre*>(ev1->getShape())->getFirstShape();
                    sh2 = dynamic_cast<Cylindre*>(ev1->getShape())->getSecondShape();
                    sh3 = ev2->getShape();
                }
                else
                {
                    sh1 = dynamic_cast<Cylindre*>(ev2->getShape())->getFirstShape();
                    sh2 = dynamic_cast<Cylindre*>(ev2->getShape())->getSecondShape();
                    sh3 = ev1->getShape();
                }

                if ((sh3 == sh1) || (sh3 == sh2)) { return false; }
            }
        }
		
        return true;
    }

protected:
};

#endif //CLOSE_EVENT_SELECTOR
