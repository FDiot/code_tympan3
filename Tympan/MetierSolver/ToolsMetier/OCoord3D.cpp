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




#include<math.h>
#include "OCoord3D.h"
#include "OGeometrie.h"


OCoord3D::OCoord3D():
    _x(0),
    _y(0),
    _z(0)
{
}

OCoord3D::OCoord3D(double x, double y, double z):
    _x(x),
    _y(y),
    _z(z)
{
}

OCoord3D::OCoord3D(const OCoord3D& coord)
{
    *this = coord;
}

OCoord3D::~OCoord3D()
{
}

OCoord3D& OCoord3D::operator=(const OCoord3D& coord)
{
    if (this != &coord)
    {
        _x = coord._x;
        _y = coord._y;
        _z = coord._z;
    }
    return *this;
}

bool OCoord3D::operator==(const OCoord3D& coord) const
{
    if (this != &coord)
    {
        if (fabs(_x - coord._x) > EPSILON_6) { return false; }
        if (fabs(_y - coord._y) > EPSILON_6) { return false; }
        if (fabs(_z - coord._z) > EPSILON_6) { return false; }
    }
    return true;
}

bool OCoord3D::operator!=(const OCoord3D& coord) const
{
    return !operator==(coord);
}

void OCoord3D::setCoords(double x, double y, double z)
{
    _x = x;
    _y = y;
    _z = z;
}

void OCoord3D::setCoords(double coords[3])
{
    _x = coords[0];
    _y = coords[1];
    _z = coords[2];
}

void OCoord3D::getCoords(double coords[3])
{
    coords[0] = _x;
    coords[1] = _y;
    coords[2] = _z;
}

double* OCoord3D::getCoords()
{
    double* res = new double[3];

    res[0] = _x;
    res[1] = _y;
    res[2] = _z;

    return res;
}
