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

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/models/business/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#if TY_USE_IHM
#include "Tympan/gui/gl/TYRayGraphic.h"
#endif

TY_EXT_GRAPHIC_INST(TYRay);

#include "TYRay.h"

TYRay::TYRay() : acoustic_path()
{
    _source = NULL;
    _recepteur = NULL;
}

TYRay::TYRay(const TYRay& ray)
{
    *this = ray;
}

TYRay::~TYRay()
{
}

TYRay& TYRay::operator=(const TYRay& other)
{
    acoustic_path::operator=(other);
    _source = other._source;
    _recepteur = other._recepteur;

    return *this;
}

bool TYRay::deepCopy(TYRay* other)
{
    assert(other);

    acoustic_path::deepCopy(other);

    _source = other->_source;
    _recepteur = other->_recepteur;

    return true;
}
