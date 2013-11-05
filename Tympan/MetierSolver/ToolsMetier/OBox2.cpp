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
	_repere = ORepere3D();
	_min = OPoint3D(0, 0, 0);
	_max = OPoint3D(0, 0, 0);
	_center = OPoint3D(0, 0, 0);
	_A = OPoint3D(0, 0, 0);
	_B = OPoint3D(0, 0, 0);
	_C = OPoint3D(0, 0, 0);
	_D = OPoint3D(0, 0, 0);
	_E = OPoint3D(0, 0, 0);
	_F = OPoint3D(0, 0, 0);
	_G = OPoint3D(0, 0, 0);
	_H = OPoint3D(0, 0, 0);
	_length = 0;
	_height = 0;
	_width = 0;
}

OBox2::OBox2(const OBox2& box)
{
	_min = box._min;
	_max = box._max;
	_repere = box._repere;
	_center = box._center;
	_A = box._A;
	_B = box._B;
	_C = box._C;
	_D = box._D;
	_E = box._E;
	_F = box._F;
	_G = box._G;
	_H = box._H;
	_length = box._length;
	_height = box._height;
	_width = box._width;
}

OBox2::OBox2(const OBox& box, const ORepere3D& repere)
{
    _min = box._min;
    _max = box._max;
    _repere = repere;
	_center = OPoint3D(box._max._x/2, box._max._y/2, box._max._z/2);
	_length = box._max._x;
	_height = box._max._z;
	_width = box._max._y;
	_A = box._min;
	_B = OPoint3D(0, _width, 0);
	_C = OPoint3D(_length, _width, 0);
	_D = OPoint3D(_length, 0, 0);
	_E = OPoint3D(_length, 0, _height);
	_F = OPoint3D(0, 0, _height);
	_G = OPoint3D(0, _width, _height);
	_H = box._max;
}

OBox2::OBox2(const OBox2& box, const ORepere3D& repere, const OPoint3D& centre)
{
    _min = box._min;
    _max = box._max;
    _repere = repere;
	_center = OPoint3D(centre._x, centre._y, centre._z);
	_length = box._max._x;
	_height = box._max._z;
	_width = box._max._y;
	_A = box._A;
	_B = box._B;
	_C = box._C;
	_D = box._D;
	_E = box._E;
	_F = box._F;
	_G = box._G;
	_H = box._H;
}

OBox2::OBox2(const OCoord3D& min, const OCoord3D& max, const ORepere3D& repere)
{
    _min = min;
    _max = max;
    _repere = repere;
	_center = OPoint3D(_max._x/2, _max._y/2, _max._z/2);
	_length = _max._x/2;
	_height = _max._z/2;
	_width = _max._y/2;
	_A = _min;
	_B = OPoint3D(0, _width, 0);
	_C = OPoint3D(_length, _width, 0);
	_D = OPoint3D(_length, 0, 0);
	_E = OPoint3D(_length, 0, _height);
	_F = OPoint3D(0, 0, _height);
	_G = OPoint3D(0, _width, _height);
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
		_A = box._A;
		_B = box._B;
		_C = box._C;
		_D = box._D;
		_E = box._E;
		_F = box._F;
		_G = box._G;
		_H = box._H;
		_center = box._center;
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
		if (_A != box._min) { return false; }
		if (_B != OPoint3D(box._min._x, box._max._y, box._min._z)) { return false; }
		if (_C != OPoint3D(box._max._x, box._max._y, box._min._z)) { return false; }
		if (_D != OPoint3D(box._max._x, box._min._y, box._min._z)) { return false; }
		if (_E != OPoint3D(box._max._x, box._min._y, box._max._z)) { return false; }
		if (_F != OPoint3D(box._min._x, box._min._y, box._max._z)) { return false; }
		if (_G != OPoint3D(box._min._x, box._max._y, box._max._z)) { return false; }
		if (_H != box._max) { return false; }
		if (_center != OPoint3D(box._max._x/2, box._max._y/2, box._max._z/2)) { return false; }
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
	Vfinal._z = +sinB*V._x + cosB*V._z;
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
	_A.setCoords(OAf._x + _center._x, OAf._y + _center._y, OAf._z + _center._z);
	_B.setCoords(OBf._x + _center._x, OBf._y + _center._y, OBf._z + _center._z);
	_C.setCoords(OCf._x + _center._x, OCf._y + _center._y, OCf._z + _center._z);
	_D.setCoords(ODf._x + _center._x, ODf._y + _center._y, ODf._z + _center._z);
	_E.setCoords(OEf._x + _center._x, OEf._y + _center._y, OEf._z + _center._z);
	_F.setCoords(OFf._x + _center._x, OFf._y + _center._y, OFf._z + _center._z);
	_G.setCoords(OGf._x + _center._x, OGf._y + _center._y, OGf._z + _center._z);
	_H.setCoords(OHf._x + _center._x, OHf._y + _center._y, OHf._z + _center._z);
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
	_center._x += vectorTranslate._x;
	_center._y += vectorTranslate._y;
	_center._z += vectorTranslate._z;
    _A._x += vectorTranslate._x;
    _A._y += vectorTranslate._y;
    _A._z += vectorTranslate._z;
    _H._x += vectorTranslate._x;
    _H._y += vectorTranslate._y;
    _H._z += vectorTranslate._z;
    _B._x += vectorTranslate._x;
    _B._y += vectorTranslate._y;
    _B._z += vectorTranslate._z;
	_C._x += vectorTranslate._x;
    _C._y += vectorTranslate._y;
    _C._z += vectorTranslate._z;
    _D._x += vectorTranslate._x;
    _D._y += vectorTranslate._y;
    _D._z += vectorTranslate._z;
    _E._x += vectorTranslate._x;
    _E._y += vectorTranslate._y;
    _E._z += vectorTranslate._z;
    _F._x += vectorTranslate._x;
    _F._y += vectorTranslate._y;
    _F._z += vectorTranslate._z;
	_G._x += vectorTranslate._x;
    _G._y += vectorTranslate._y;
    _G._z += vectorTranslate._z;
}

OBox2 OBox2::boxRotation(const OPoint3D& O, const OPoint3D& P2)
{
	OBox2 box, boxIn = *this;

    //3/ Changement de repere pour faire pivoter la boite
	// The aim here is to make a rotation of the bounding box
	// We want its main axis to be aligned with Vector v2 i.e. [OPsuiv].
	// We have to keep in mind that axes are defined from the bottom left corner
	// of the box.
	// This step is a bit tricky : one has to keep in mind in
	// which plane(s) the rotation(s) occur(s).
	// 3D rotation : 2 rotations around two different axes 
	// (pay attention to get the right ones!);
	// 2D rotation : only one movement => define the right axis.
	// Let x1, y1, z1 and x2, y2, z2 be the vectors after the first rotation
	// and the second one, respectively expressed in the new basis B1 and B2. 
	// Let xf, yf, zf be the final vectors given to the box, i.e. x2,y2 and z2
	// in B0.

	// Let I be the point at coordinate(L, l/2, l/2)
	// This point will be useful to create a vector from the box centre to I
	// This is a way of creating an axis within the box in order to compute 
	// the angle between the box main axis and vector v2
	OPoint3D I = OPoint3D(this->_H._x, O._y, O._z);

	// Let v1 be the vector between the box centre (O) and I
	// Let v2 be the vector between the box centre (O) and Psuiv
	OVector3D v1 = OVector3D(O,I);
    OVector3D v2 = OVector3D(O,P2);

	v2.normalize();
	v1.normalize();

	if (v1._z == 0 && v2._z == 0)
	{
		// When v1 and v2 are in xOy plane only
		return this->rotInXOYOnly(v1, v2, O, P2);
	}
	else if (v1._y == 0 && v2._y == 0)
	{
		// When v1 and v2 are in xOz plane only
		return this->rotInXOZOnly(v1, v2, O, P2);
	}
	else
	{
		// When it's a 3d rotation, things get a little more "funny".
		return this->rot3D(v1, v2, O, P2);
	}

	return box;
}

OBox2 OBox2::rotInXOYOnly(const OVector3D& v1, const OVector3D& v2, const OPoint3D& O, const OPoint3D& P2)
{
	OBox2 box = *this;
	OVector3D x0 = v1;

	// Creates a basis with 3 vectors x0, y0 and z0.
	// x0 -> x2, y0 -> y2 and z0 remains the same.
	OVector3D y0 = OCoord3D(0.0,1.0,0.0);
	OVector3D z0 = OCoord3D(0.0,0.0,1.0);
	double alpha = v1.angle(v2);
	OVector3D x2 = x0.getRotationOzOy(alpha, 0.0);
	OVector3D y2 = y0.getRotationOzOy(alpha, 0.0);
	x2.normalize();
	y2.normalize();

	// Final step : New ORepere3D
	ORepere3D r = ORepere3D(O, x2, y2, z0);
	box.BoxRotationOzOy(alpha, 0.0);
	
	return OBox2(box, r, O);
}

OBox2 OBox2::rotInXOZOnly(const OVector3D& v1, const OVector3D& v2, const OPoint3D& O, const OPoint3D& P2)
{
	OBox2 box = *this;
	OVector3D x0 = v1;

	// Creates a basis with 3 vectors x0, y0 and z0.
	// x0 -> x2, z0 -> z2 and y0 remains the same.
	OVector3D y0 = OCoord3D(0.0,1.0,0.0);
	OVector3D z0 = OCoord3D(0.0,0.0,1.0);
	double theta = v1.angle(v2);
	OVector3D x2 = x0.getRotationOzOy(0.0, theta);
	OVector3D z2 = z0.getRotationOzOy(0.0, theta);
	x2.normalize();
	z2.normalize();

	// Final step : New ORepere3D
	ORepere3D r = ORepere3D(O, x2, y0, z2); 
	box._center._x = O._x;
	box._center._y = O._y;
	box._center._z = O._z;
	box.BoxRotationOzOy(0.0, theta);
	
	return OBox2(box, r, O);
}

OBox2 OBox2::rot3D(const OVector3D& v1, const OVector3D& v2, const OPoint3D& O, const OPoint3D& P2)
{
	// 2 steps needed : always a first rotation around Z axis to get Ox
	// just "under" v2 and then a rotation around Y depending on the angle.
	// Second step : rotate vectors x' and z' around Oy axis : x'' is now
	// aligned with vector v2.
	// Let's call them x0, x1, and x2.
	// Between the two rotations, (x1, y1, z1) have to become a new basis B1.

	OBox2 box = *this;
	OVector3D x0 = v1;

	// First step is to perform a rotation around Oz axis : 
	// Vectors x and y will change and become x' and y', respectively.
	OVector3D y0 = OCoord3D(0.0,1.0,0.0);
	OVector3D z0 = OCoord3D(0.0,0.0,1.0);

	// Construction du projete en supposant un sol plat
	const OCoord3D proj = OCoord3D(P2._x, P2._y, 0.0);

	// Vector from the bb center to proj
	OVector3D O2H = OVector3D(O,proj);
	double alpha = v1.angle(O2H);

	OVector3D x1b0 = x0.getRotationOz(alpha);
	OVector3D y1b0 = y0.getRotationOz(alpha);

	// This step defines x1, y1 and z1 as a new basis B1.
	OVector3D x1b1 = x1b0.getRotationOzBase2(alpha);
	OVector3D y1b1 = y1b0.getRotationOzBase2(alpha);
	x1b1.normalize();
	y1b1.normalize();
	
	// Second step
	// Returns the angle necessary to perform the rotation around OY axis
	double theta = O2H.angle(v2);

	OVector3D x2b1 = x1b1.getRotationOy(theta);
	OVector3D z2b1 = z0.getRotationOy(theta);

	// x2 , y2 and z2 in B0
	OVector3D x2b0 = x0.getRotationOzOy(alpha, theta);
	OVector3D z2b0 = z0.getRotationOzOy(0.0, theta);

	// Final step : New ORepere3D
	ORepere3D r = ORepere3D(O,x2b0 ,y1b0, z2b0); 
	box.BoxRotationOzOy(alpha, theta);
	    
	return OBox2(box, r, O);
}

