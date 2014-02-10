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

#include "Geometry/mathlib.h"
#include "Geometry/Cylindre.h"
#include "Acoustic/Event.h"
#include "Ray/Ray.h"
#include "PostFilter.h"
#include "CloseEventPostFilter.h"

using namespace std;

unsigned int closeEventPostFilter::Process()
{
    unsigned int nbSuppress = 0;
    bool bstatus = true;

    vector<Ray*>::iterator it = _tabRay->begin();
    while (it != _tabRay->end())
    {
        bstatus = true;

        vector<QSharedPointer<Event> >* events = (*it)->getEvents();

        if (events->size() < 2) { it++; continue; }

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
                    sh1 = static_cast<Cylindre*>(ev1->getShape())->getFirstShape();
                    sh2 = static_cast<Cylindre*>(ev1->getShape())->getSecondShape();
                    sh3 = ev2->getShape();
                }
                else
                {
                    sh1 = static_cast<Cylindre*>(ev2->getShape())->getFirstShape();
                    sh2 = static_cast<Cylindre*>(ev2->getShape())->getSecondShape();
                    sh3 = ev1->getShape();
                }

                if ((sh3 == sh1) || (sh3 == sh2))
                {
                    bstatus = false;
                    break;
                }
            }
        }

        if (!bstatus)
        {
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