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

#include "ORepere3D.h"
#include "OGeometrie.h"


ORepere3D::ORepere3D():
    _origin(0, 0, 0),
    _vecI(1, 0, 0),
    _vecJ(0, 1, 0),
    _vecK(0, 0, 1)
{
}

ORepere3D::ORepere3D(const ORepere3D& repere)
{
    *this = repere;
}

ORepere3D::ORepere3D(const OPoint3D& origin, const OVector3D& vecI, const OVector3D& vecJ, const OVector3D& vecK):
    _origin(origin),
    _vecI(vecI),
    _vecJ(vecJ),
    _vecK(vecK)
{
}

ORepere3D::ORepere3D(const OMatrix& matrix)
{
    set(matrix);
}

ORepere3D::~ORepere3D()
{
}

ORepere3D& ORepere3D::operator=(const ORepere3D& repere)
{
    if (this != &repere)
    {
        _origin = repere._origin;
        _vecI = repere._vecI;
        _vecJ = repere._vecJ;
        _vecK = repere._vecK;
    }
    return *this;
}

bool ORepere3D::operator==(const ORepere3D& repere) const
{
    if (this != &repere)
    {
        if (_origin != repere._origin) { return false; }
        if (_vecI != repere._vecI) { return false; }
        if (_vecJ != repere._vecJ) { return false; }
        if (_vecK != repere._vecK) { return false; }
    }
    return true;
}

bool ORepere3D::operator!=(const ORepere3D& repere) const
{
    return !operator==(repere);
}

void ORepere3D::set(const OPoint3D& origin, const OVector3D& vecI, const OVector3D& vecJ, const OVector3D& vecK)
{
    _origin = origin;
    _vecI = vecI;
    _vecJ = vecJ;
    _vecK = vecK;
}

void ORepere3D::set(const OMatrix& matrix)
{
    _vecI._x = matrix._m[0][0];
    _vecI._y = matrix._m[1][0];
    _vecI._z = matrix._m[2][0];
    _vecI.normalize();

    _vecJ._x = matrix._m[0][1];
    _vecJ._y = matrix._m[1][1];
    _vecJ._z = matrix._m[2][1];
    _vecJ.normalize();

    _vecK._x = matrix._m[0][2];
    _vecK._y = matrix._m[1][2];
    _vecK._z = matrix._m[2][2];
    _vecK.normalize();

    _origin._x = matrix._m[0][3];
    _origin._y = matrix._m[1][3];
    _origin._z = matrix._m[2][3];
}

void ORepere3D::normalize()
{
    _vecI.normalize();
    _vecJ.normalize();
    _vecK.normalize();
}

OMatrix ORepere3D::asMatrix()
{
    OMatrix matrix;

    ORepere3D rep(this);
    rep.normalize();

    matrix.unite();

    matrix._m[0][0] = rep._vecI._x;
    matrix._m[0][1] = rep._vecJ._x;
    matrix._m[0][2] = rep._vecK._x;
    matrix._m[0][3] = rep._origin._x;

    matrix._m[1][0] = rep._vecI._y;
    matrix._m[1][1] = rep._vecJ._y;
    matrix._m[1][2] = rep._vecK._y;
    matrix._m[1][3] = rep._origin._y;

    matrix._m[2][0] = rep._vecI._z;
    matrix._m[2][1] = rep._vecJ._z;
    matrix._m[2][2] = rep._vecK._z;
    matrix._m[2][3] = rep._origin._z;

    return matrix;
}
