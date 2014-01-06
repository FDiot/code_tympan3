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

#include "OBox2.h"
#include <math.h>

OBox2::OBox2()
{
    _min._x = _min._y = _min._z = _max._x = _max._y = _max._z = 0;
    _repere = ORepere3D();
}

OBox2::OBox2(const OBox2& box)
{
    _min = box._min;
    _max = box._max;
    _repere = box._repere;
	_A = _min;
	_H = _max;
}

OBox2::OBox2(const OBox& box, const ORepere3D& repere)
{
    _min = box._min;
    _max = box._max;
    _repere = repere;
	_A = _min;
	_H = _max;
}

OBox2::OBox2(const OCoord3D& min, const OCoord3D& max, const ORepere3D& repere)
{
    _min = min;
    _max = max;
    _repere = repere;
	_A = _min;
	_H = _max;
}

OBox2::OBox2(const ORepere3D repere, const OCoord3D& A, const OCoord3D& B, const OCoord3D& C, 
			const OCoord3D& D, const OCoord3D& E, const OCoord3D& F, const OCoord3D& G, 
			const OCoord3D& H)
{
	_repere = repere;
	_center = repere._origin;
	_A = A;
	_B = B;
	_C = C;
	_D = D;
	_E = E;
	_F = F;
	_G = G;
	_H = H;
	_min = _A;
	_max = _H;
}

OBox2& OBox2::operator=(const OBox2& box)
{
    if (this != &box)
    {
        _min = box._min;
        _max = box._max;
        _repere = box._repere;
    }
    return *this;
}

bool OBox2::operator==(const OBox2& box) const
{
    if (this != &box)
    {
        if (_min != box._min) { return false; }
        if (_max != box._max) { return false; }
        if (_repere != box._repere) { return false; }
    }
    return true;
}

OPoint3D OBox2::BoxCoord(int N) const
{
	switch(N)
	{
		case(0):
			return _center;
		case(1):
			return _A;
		case(2):
			return _B;
		case(3):
			return _C;
		case(4):
			return _D;
		case(5):
			return _E;
		case(6):
			return _F;
		case(7):
			return _G;
		case(8):
			return _H;
		default:
			OPoint3D zero = OPoint3D(0.0, 0.0, 0.0);
			return zero;
	}
}

OVector3D OBox2::GetRotationOzOy(double alpha, double theta, OVector3D V)
{
	double cosA = cos(alpha);
	double sinA = sin(alpha);
	double cosB = cos(theta);
	double sinB = sin(theta);
	OVector3D Vfinal;
	// Computes the calculations
	Vfinal._x = cosA*cosB*V._x - sinA*V._y - cosA*sinB*V._z;
	Vfinal._y = sinA*cosB*V._x + cosA*V._y - sinA*sinB*V._z;
	Vfinal._z = sinB*V._x + cosB*V._z;
	return Vfinal;	
}

void OBox2::BoxRotationOzOy(double alpha, double theta)
{
	// We create vectors from the box center to the corners
	OVector3D OA = OVector3D(_center, _A);
	OVector3D OB = OVector3D(_center, _B);
	OVector3D OC = OVector3D(_center, _C);
	OVector3D OD = OVector3D(_center, _D);
	OVector3D OE = OVector3D(_center, _E);
	OVector3D OF = OVector3D(_center, _F);
	OVector3D OG = OVector3D(_center, _G);
	OVector3D OH = OVector3D(_center, _H);

	// These vectors are modified by two rotations
	OVector3D OAf = GetRotationOzOy(alpha, theta, OA);
	OVector3D OBf = GetRotationOzOy(alpha, theta, OB);
	OVector3D OCf = GetRotationOzOy(alpha, theta, OC);
	OVector3D ODf = GetRotationOzOy(alpha, theta, OD);
	OVector3D OEf = GetRotationOzOy(alpha, theta, OE);
	OVector3D OFf = GetRotationOzOy(alpha, theta, OF);
	OVector3D OGf = GetRotationOzOy(alpha, theta, OG);
	OVector3D OHf = GetRotationOzOy(alpha, theta, OH);

	//It changes the box properties
	_A.setCoords(OAf._x, OAf._y, OAf._z);
	_B.setCoords(OBf._x, OBf._y, OBf._z);
	_C.setCoords(OCf._x, OCf._y, OCf._z);
	_D.setCoords(ODf._x, ODf._y, ODf._z);
	_E.setCoords(OEf._x, OEf._y, OEf._z);
	_F.setCoords(OFf._x, OFf._y, OFf._z);
	_G.setCoords(OGf._x, OGf._y, OGf._z);
	_H.setCoords(OHf._x, OHf._y, OHf._z);
	_min = _A;
	_max = _H;
}

bool OBox2::isInside(const OPoint3D& pt) const
{
	OPoint3D point2;

	// OpM (OprimeM) is the vector from the box center to point p
	OVector3D OpM = OVector3D(pt._x - _repere._origin._x, pt._y - _repere._origin._y,
		pt._z - _repere._origin._z);

	// point2 has its coordinates in the other coordinates system (the box system)
	point2._x = OpM._x * _repere._vecI._x;
	point2._y = OpM._y * _repere._vecJ._y;
	point2._z = OpM._z * _repere._vecK._z;

	return OBox::isInside(point2);
}


bool OBox2::isInside2D(const OPoint3D& pt) const
{
	OPoint3D point2;

	// OpM (OprimeM) is the vector from the box center to point p
	OVector3D OpM = OVector3D(pt._x - _repere._origin._x, pt._y - _repere._origin._y,
		pt._z - _repere._origin._z);

	// point2 has its coordinates in the other coordinates system (the box system)
	point2._x = OpM._x * _repere._vecI._x;
	point2._y = OpM._y * _repere._vecJ._y;
	point2._z = OpM._z * _repere._vecK._z;
	
	return OBox::isInside2D(point2);
}

/*
bool OBox2::isInContact(const OBox2& box) const
{
    if (_max._x < box._min._x) return false;
    if (_min._x > box._max._x) return false;
    if (_max._y < box._min._y) return false;
    if (_min._y > box._max._y) return false;
    if (_max._z < box._min._z) return false;
    if (_min._z > box._max._z) return false;
    return true;
}

void OBox2::Enlarge(const OPoint3D& pt)
{
    if( _min._x == 0 && _min._y == 0 &&  _min._z == 0 &&
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
        if (pt._x < _min._x) _min._x = pt._x;
        if (pt._x > _max._x) _max._x = pt._x;
        if (pt._y < _min._y) _min._y = pt._y;
        if (pt._y > _max._y) _max._y = pt._y;
        if (pt._z < _min._z) _min._z = pt._z;
        if (pt._z > _max._z) _max._z = pt._z;
    }
}

void OBox2::Enlarge(float x, float y, float z)
{
    OPoint3D pt(x,y,z);
    Enlarge(pt);
}

void OBox2::Enlarge(const OBox2& box)
{
    if (box._min._x < _min._x) _min._x = box._min._x;
    if (box._min._y < _min._y) _min._y = box._min._y;
    if (box._min._z < _min._z) _min._z = box._min._z;
    if (box._max._x > _max._x) _max._x = box._max._x;
    if (box._max._y > _max._y) _max._y = box._max._y;
    if (box._max._z > _max._z) _max._z = box._max._z;
}
*/
void OBox2::Translate(const OPoint3D& vectorTranslate)
{
    _min._x += vectorTranslate._x;
    _min._y += vectorTranslate._y;
    _min._z += vectorTranslate._z;
    _max._x += vectorTranslate._x;
    _max._y += vectorTranslate._y;
    _max._z += vectorTranslate._z;
}
