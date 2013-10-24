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




#include "OPoint3D.h"
#include "OVector3D.h"
#include <math.h>


OPoint3D::OPoint3D():
    OCoord3D()
{
}

OPoint3D::OPoint3D(const OPoint3D& pt):
    OCoord3D(pt)
{
}

OPoint3D::OPoint3D(const OCoord3D& coord):
    OCoord3D(coord)
{
}

OPoint3D::OPoint3D(double x, double y, double z):
    OCoord3D(x, y, z)
{
}

OPoint3D::OPoint3D(double v[]):
    OCoord3D(v[0], v[1], v[2])
{
}

OPoint3D::~OPoint3D()
{
}

double OPoint3D::distFrom(const OPoint3D& pt) const
{
    return OVector3D(*this, pt).norme();
}

double OPoint3D::dist2DFrom(const OPoint3D& pt) const
{
    return sqrt((_x - pt._x) * (_x - pt._x) + (_y - pt._y) * (_y - pt._y));
}

void OPoint3D::set(double x, double y, double z)
{
    OCoord3D::setCoords(x, y, z);
}
