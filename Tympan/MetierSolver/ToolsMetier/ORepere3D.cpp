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

bool ORepere3D::getMatChangeRep(OMatrix& matrix)
{
    bool res = false;

    double  normeI = _vecI.norme();
    double  normeJ = _vecJ.norme();
    double  normeK = _vecK.norme();

    // Init matrix
    matrix.unite();

    if ((normeI != 0.0) && (normeJ != 0.0) && (normeK != 0.0))
    {
        res = true;

        matrix._m[0][0] = _vecI._x / normeI;
        matrix._m[0][1] = _vecJ._x / normeJ;
        matrix._m[0][2] = _vecK._x / normeK;
        matrix._m[0][3] = _origin._x;

        matrix._m[1][0] = _vecI._y / normeI;
        matrix._m[1][1] = _vecJ._y / normeJ;
        matrix._m[1][2] = _vecK._y / normeK;
        matrix._m[1][3] = _origin._y;

        matrix._m[2][0] = _vecI._z / normeI;
        matrix._m[2][1] = _vecJ._z / normeJ;
        matrix._m[2][2] = _vecK._z / normeK;
        matrix._m[2][3] = _origin._z;
    }

    return res;
}
