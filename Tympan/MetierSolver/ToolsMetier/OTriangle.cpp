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

#include "OTriangle.h"
#include "OPoint3D.h"
#include <cmath>
#include <limits>
#include <cassert>

OPoint3D OTriangle::* OTriangle::vertices_m_ptr[3] = {&OTriangle::_A, &OTriangle::_B, &OTriangle::_C};
int OTriangle::* OTriangle::indices_m_ptr[3] = {&OTriangle::_p1, &OTriangle::_p2, &OTriangle::_p3};

OPoint3D& OTriangle::vertex(unsigned i)
{
	assert( i<3 && "Triangle vertex indices are 0, 1 or 2");
	return this->*vertices_m_ptr[i];
}

const OPoint3D& OTriangle::vertex(unsigned i) const
{
	assert( i<3 && "Triangle vertex indices are 0, 1 or 2");
	return this->*vertices_m_ptr[i];
}

int OTriangle::index(unsigned i) const
{
	assert( i<3 && "Triangle vertex indices are 0, 1 or 2");
	return this->*indices_m_ptr[i];
}

int& OTriangle::index(unsigned i)
{
	assert( i<3 && "Triangle vertex indices are 0, 1 or 2");
	return this->*indices_m_ptr[i];
}

OTriangle::OTriangle(int p1, int p2, int p3)
{
    _p1 = p1;
    _p2 = p2;
    _p3 = p3;
    // Insert signaling NaN to protect against used of uninitialized coordinates
    const double NaN = std::numeric_limits<double>::signaling_NaN();
    const OPoint3D notAPoint(NaN, NaN, NaN);
    _A = notAPoint;
    _B = notAPoint;
    _C = notAPoint;
}

OTriangle::OTriangle(OPoint3D A, OPoint3D B, OPoint3D C)
{
    _A = A;
    _B = B;
    _C = C;
     // Hope this segfault if used for indexing an array (i.e. should not create
     // a \c OTriangle from OPoint3D without setting _p1, _p2 & _p3.
    _p1 = _p2 = _p3 = std::numeric_limits<unsigned>::max();
}

OTriangle::~OTriangle()
{
}

double OTriangle::getSurface()
{
	/*
    // TODO : recuperation des sommets

    OPoint3D A, B, C; //sommets A, B, C du triangle
    double xA, yA, zA, xB, yB, zB, xC, yC, zC;

    xA = _A._x; yA = _A._y; zA = _A._z;
    xB = _B._x; yB = _B._y; zB = _B._z;
    xC = _C._x; yC = _C._y; zC = _C._z;

    A = OPoint3D(xA, yA, zA);
    B = OPoint3D(xB, yB, zB);
    C = OPoint3D(xC, yC, zC);
	*/

    // calcul des cotes :
    double c1 = _A.distFrom(_B);
    double c2 = _B.distFrom(_C);
    double c3 = _C.distFrom(_A);

    double s = (c1 + c2 + c3) * 0.5; // demie somme des cotes du triangle

    // air d'un triangle de cotes a, b , c (formule de Heron)
    return sqrt(s * (s - c1) * (s - c2) * (s - c3));
}
