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
 
/*
 *
 */




#include "OSubject.h"


void OSubject::attach(OObserver* pObserver)
{
    if (pObserver)
    {
        _observers.push_back(pObserver);
    }
}

bool OSubject::detach(OObserver* pObserver)
{
    OTabPtrObserver::iterator ite;
    bool ret = false;

    if (pObserver)
    {
        for (ite = _observers.begin(); ite != _observers.end(); ite++)
        {
            if (*ite == pObserver)
            {
                _observers.erase(ite);
                ret = true;
                break;
            }
        }
    }

    return ret;
}

void OSubject::reset()
{
    _observers.clear();
}

void OSubject::notify()
{
    for (unsigned int i = 0; i < _observers.size(); i++)
    {
        _observers.at(i)->update();
    }
}
