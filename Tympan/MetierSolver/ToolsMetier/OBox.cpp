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
 */

#include "OBox.h"

OBox::OBox()
{
    _min._x = _min._y = _min._z = _max._x = _max._y = _max._z = 0;
}

OBox::OBox(const OBox& box)
{
    _min = box._min;
    _max = box._max;
}

OBox::OBox(const OCoord3D& min, const OCoord3D& max)
{
    _min = min;
    _max = max;
}

OBox::OBox(double x1, double y1, double z1, double x2, double y2, double z2)
{
    _min = OPoint3D(x1, y1, z1);
    _max = OPoint3D(x2, y2, z2);
}

OBox& OBox::operator=(const OBox& box)
{
    if (this != &box)
    {
        _min = box._min;
        _max = box._max;
    }
    return *this;
}

bool OBox::operator==(const OBox& box) const
{
    if (this != &box)
    {
        if (_min != box._min) { return false; }
        if (_max != box._max) { return false; }
    }
    return true;
}

bool OBox::isInside(const OPoint3D& pt) const
{
    if (pt._x < _min._x) { return false; }
    if (pt._x > _max._x) { return false; }
    if (pt._y < _min._y) { return false; }
    if (pt._y > _max._y) { return false; }
    if (pt._z < _min._z) { return false; }
    if (pt._z > _max._z) { return false; }
    return true;
}

bool OBox::isInside2D(const OPoint3D& pt) const
{
    if (pt._x < _min._x) { return false; }
    if (pt._x > _max._x) { return false; }
    if (pt._y < _min._y) { return false; }
    if (pt._y > _max._y) { return false; }
    return true;
}

bool OBox::isInContact(const OBox& box) const
{
    if (_max._x < box._min._x) { return false; }
    if (_min._x > box._max._x) { return false; }
    if (_max._y < box._min._y) { return false; }
    if (_min._y > box._max._y) { return false; }
    if (_max._z < box._min._z) { return false; }
    if (_min._z > box._max._z) { return false; }
    return true;
}

void OBox::Enlarge(const OPoint3D& pt)
{
    if (_min._x == 0 && _min._y == 0 &&  _min._z == 0 &&
        _max._x == 0 && _max._y == 0 &&  _max._z == 0)
    {
        _min._x = pt._x;
        _max._x = pt._x;
        _min._y = pt._y;
        _max._y = pt._y;
        _min._z = pt._z;
        _max._z = pt._z;
    }
    else
    {
        if (pt._x < _min._x) { _min._x = pt._x; }
        if (pt._x > _max._x) { _max._x = pt._x; }
        if (pt._y < _min._y) { _min._y = pt._y; }
        if (pt._y > _max._y) { _max._y = pt._y; }
        if (pt._z < _min._z) { _min._z = pt._z; }
        if (pt._z > _max._z) { _max._z = pt._z; }
    }
}

void OBox::Enlarge(float x, float y, float z)
{
    OPoint3D pt(x, y, z);
    Enlarge(pt);
}

void OBox::Enlarge(const OBox& box)
{
    if (box._min._x < _min._x) { _min._x = box._min._x; }
    if (box._min._y < _min._y) { _min._y = box._min._y; }
    if (box._min._z < _min._z) { _min._z = box._min._z; }
    if (box._max._x > _max._x) { _max._x = box._max._x; }
    if (box._max._y > _max._y) { _max._y = box._max._y; }
    if (box._max._z > _max._z) { _max._z = box._max._z; }
}

void OBox::Translate(const OPoint3D& vectorTranslate)
{
    _min._x += vectorTranslate._x;
    _min._y += vectorTranslate._y;
    _min._z += vectorTranslate._z;
    _max._x += vectorTranslate._x;
    _max._y += vectorTranslate._y;
    _max._z += vectorTranslate._z;
}
