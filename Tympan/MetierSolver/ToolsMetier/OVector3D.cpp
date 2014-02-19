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




#include "OVector3D.h"
#include "OGeometrie.h"


OVector3D::OVector3D():
    OCoord3D()
{
}

OVector3D::OVector3D(const OVector3D& vector):
    OCoord3D(vector)
{
}

OVector3D::OVector3D(const OCoord3D& coord):
    OCoord3D(coord)
{
}

OVector3D::OVector3D(const OCoord3D& coordFrom, const OCoord3D& coordTo)
{
    OVector3D vecFrom(coordFrom);
    OVector3D vecTo(coordTo);

    *this = vecTo - vecFrom;
}

OVector3D::OVector3D(double x, double y, double z):
    OCoord3D(x, y, z)
{
}

OVector3D::~OVector3D()
{
}

void OVector3D::reset()
{
    _x = 0.0;
    _y = 0.0;
    _z = 0.0;
}

OVector3D& OVector3D::operator=(const OVector3D& vector)
{
    OCoord3D::operator=(vector);
    return *this;
}

bool OVector3D::operator==(const OVector3D& vector) const
{
    return OCoord3D::operator==(vector);
}

bool OVector3D::operator!=(const OVector3D& vector) const
{
    return !operator==(vector);
}

OVector3D OVector3D::operator+(const OVector3D& vector) const
{
    OVector3D vectorRet = *this;

    vectorRet._x = this->_x + vector._x;
    vectorRet._y = this->_y + vector._y;
    vectorRet._z = this->_z + vector._z;

    return vectorRet;
}

OVector3D OVector3D::operator-(const OVector3D& vector) const
{
    OVector3D vectorRet;

    vectorRet._x = this->_x - vector._x;
    vectorRet._y = this->_y - vector._y;
    vectorRet._z = this->_z - vector._z;

    return vectorRet;
}

OVector3D OVector3D::operator*(const OVector3D& vector) const
{
    OVector3D vectorRet;

    vectorRet._x = this->_x * vector._x;
    vectorRet._y = this->_y * vector._y;
    vectorRet._z = this->_z * vector._z;

    return vectorRet;
}

OVector3D OVector3D::operator*(const double a) const
{
    // Appel l'operateur (double) * (OVector3D)
    return (a * (*this));
}

OVector3D operator*(const double a, const OVector3D& vector)
{
    OVector3D vectorRet;

    vectorRet._x = vector._x * a;
    vectorRet._y = vector._y * a;
    vectorRet._z = vector._z * a;

    return vectorRet;
}

OVector3D OVector3D::cross(const OVector3D& vector) const
{
    OVector3D vectorRet;

    vectorRet._x = (this->_y * vector._z) - (this->_z * vector._y);
    vectorRet._y = (this->_z * vector._x) - (this->_x * vector._z);
    vectorRet._z = (this->_x * vector._y) - (this->_y * vector._x);

    return vectorRet;
}

void OVector3D::balance(double c1, const OVector3D& vector2, double c2)
{
    *this = *this * c1 + vector2 * c2;
}

double OVector3D::scalar(const OVector3D& vector) const
{
    return (this->_x * vector._x + this->_y * vector._y + this->_z * vector._z);
}

double OVector3D::norme() const
{
    return sqrt(_x * _x + _y * _y + _z * _z);
}

OVector3D OVector3D::normal(const OVector3D& vector2, const OVector3D& vector3) const
{
    return (vector2 - *this).cross(vector3 - *this);
}

void OVector3D::normalize()
{
    double n = norme();

    if (n > 0.0)
    {
        _x /= n;
        _y /= n;
        _z /= n;
    }
    else {
      throw exception();
    }
}

void OVector3D::invert()
{
    _x = - _x;
    _y = - _y;
    _z = - _z;
}

double OVector3D::angle(const OVector3D& vector) const
{
    double norme = (this->norme() * vector.norme());

    if (norme == 0.0) { return norme; }

    double cosVal = this->scalar(vector) / norme;

    return (acos(BORNE(cosVal, -1.0, 1.0)));
}
