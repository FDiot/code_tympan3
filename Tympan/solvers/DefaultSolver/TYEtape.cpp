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


#include "TYEtape.h"


TYEtape::TYEtape(const TYEtape& other)
{
    *this = other;
}

TYEtape::~TYEtape()
{
}

TYEtape& TYEtape::operator=(const TYEtape& other)
{
    if (this != &other)
    {
        _pt = other._pt;
        _type = other._type;
        _spectrum = other._spectrum;
    }
    return *this;
}

bool TYEtape::operator==(const TYEtape& other) const
{
    if (this != &other)
    {
        if (_pt != other._pt) { return false; }
        if (_type != other._type) { return false; }
        if (_spectrum != other._spectrum) { return false; }
    }
    return true;
}

bool TYEtape::operator!=(const TYEtape& other) const
{
    return !operator==(other);
}

acoustic_event* TYEtape::asEvent() const
{
    acoustic_event *returned_event = new acoustic_event();
    returned_event->pos = _pt;
    returned_event->type = _type;
    return returned_event;
}