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

OBox2::OBox2() :    OBox(),
    _repere(ORepere3D()),
    _center(0.0, 0.0, 0.0),
    _A(0.0, 0.0, 0.0),
    _B(0.0, 0.0, 0.0),
    _C(0.0, 0.0, 0.0),
    _D(0.0, 0.0, 0.0),
    _E(0.0, 0.0, 0.0),
    _F(0.0, 0.0, 0.0),
    _G(0.0, 0.0, 0.0),
    _H(0.0, 0.0, 0.0),
    _length(0.0),
    _height(0.0),
    _width(0.0)
{
}

OBox2::OBox2(const OBox2& box)
{
    *this = box;
}

OBox2::OBox2(const OBox& box)
{
    _repere = ORepere3D();

    _min = box._min;
    _max = box._max;

    double minX = _min._x, minY = _min._y, minZ = _min._z;
    double maxX = _max._x, maxY = _max._y, maxZ = _max._z;

    _center = OPoint3D((maxX - minX) / 2.0 + minX,
                       (maxY - minY) / 2.0 + minY,
                       (maxZ - minZ) / 2.0 + minZ);

    _length = maxX - minX;
    _width  = maxY - minY;
    _height = maxZ - minZ;

    _A = OPoint3D(minX, minY, minZ);
    _B = OPoint3D(minX, maxY, minZ);
    _C = OPoint3D(maxX, maxY, minZ);
    _D = OPoint3D(maxX, minY, minZ);
    _E = OPoint3D(maxX, minY, maxZ);
    _F = OPoint3D(minX, minY, maxZ);
    _G = OPoint3D(minX, maxY, maxZ);
    _H = OPoint3D(maxX, maxY, maxZ);

    _repere._origin = _A;
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

OBox2::OBox2(const double& length, const double& width, const double& height) :
    _length(length),
    _height(width),
    _width(height),
    _center(OPoint3D(0.0, 0.0, 0.0))
{
    double  minX = -_length / 2.0,
            minY = -_width / 2.0,
            minZ = -_height / 2.0,
            maxX = minX + _length,
            maxY = minY + _width,
            maxZ = minZ + _height;

    _A = _min = OPoint3D(minX, minY, minZ);
    _B =        OPoint3D(minX, maxY, minZ);
    _C =        OPoint3D(maxX, maxY, minZ);
    _D =        OPoint3D(maxX, minY, minZ);
    _E =        OPoint3D(maxX, minY, maxZ);
    _F =        OPoint3D(minX, minY, maxZ);
    _G =        OPoint3D(minX, maxY, maxZ);
    _H = _max = OPoint3D(maxX, maxY, maxZ);
}

OBox2& OBox2::operator=(const OBox2& box)
{
    if (this != &box)
    {
        OBox::operator=(box);
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

    return *this;
}


bool OBox2::operator==(const OBox2& box) const
{
    if (this != &box)
    {
        if (_min != box._min) { return false; }
        if (_max != box._max) { return false; }
        if (_repere != box._repere) { return false; }
        if (_center != box._center) { return false; }
        if (_A != box._A) { return false; }
        if (_B != box._B) { return false; }
        if (_C != box._C) { return false; }
        if (_D != box._D) { return false; }
        if (_E != box._E) { return false; }
        if (_F != box._F) { return false; }
        if (_G != box._G) { return false; }
        if (_H != box._H) { return false; }
        if (_length != box._length) { return false; }
        if (_height != box._height) { return false; }
        if (_width != box._width) { return false; }
    }
    return true;
}

OPoint3D OBox2::BoxCoord(int N) const
{
    switch (N)
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

void OBox2::Translate(const OVector3D& vect)
{
    _center = _center + vect;
    _A = _A + vect;
    _B = _B + vect;
    _C = _C + vect;
    _D = _D + vect;
    _E = _E + vect;
    _F = _F + vect;
    _G = _G + vect;
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
    OVector3D v1 = OVector3D(O, I);
    OVector3D v2 = OVector3D(O, P2);

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

void OBox2::BoxRotationOzOy(double alpha, double theta)
{
    // We create vectors from the box center to the corners
    OVector3D OA(_center, _A);
    OVector3D OB(_center, _B);
    OVector3D OC(_center, _C);
    OVector3D OD(_center, _D);
    OVector3D OE(_center, _E);
    OVector3D OF(_center, _F);
    OVector3D OG(_center, _G);
    OVector3D OH(_center, _H);

    // These vectors are modified by two rotations
    OVector3D OAf = OA.getRotationOzOy(alpha, theta);
    OVector3D OBf = OB.getRotationOzOy(alpha, theta);
    OVector3D OCf = OC.getRotationOzOy(alpha, theta);
    OVector3D ODf = OD.getRotationOzOy(alpha, theta);
    OVector3D OEf = OE.getRotationOzOy(alpha, theta);
    OVector3D OFf = OF.getRotationOzOy(alpha, theta);
    OVector3D OGf = OG.getRotationOzOy(alpha, theta);
    OVector3D OHf = OH.getRotationOzOy(alpha, theta);

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

OBox2 OBox2::rotInXOYOnly(const OVector3D& v1, const OVector3D& v2, const OPoint3D& O, const OPoint3D& P2)
{
    OBox2 box = *this;
    OVector3D x0 = v1;

    // Creates a basis with 3 vectors x0, y0 and z0.
    // x0 -> x2, y0 -> y2 and z0 remains the same.
    OVector3D y0 = OCoord3D(0.0, 1.0, 0.0);
    OVector3D z0 = OCoord3D(0.0, 0.0, 1.0);
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
    OVector3D y0 = OCoord3D(0.0, 1.0, 0.0);
    OVector3D z0 = OCoord3D(0.0, 0.0, 1.0);
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
    OVector3D y0 = OCoord3D(0.0, 1.0, 0.0);
    OVector3D z0 = OCoord3D(0.0, 0.0, 1.0);

    // Construction du projete en supposant un sol plat
    const OCoord3D proj = OCoord3D(P2._x, P2._y, 0.0);

    // Vector from the bb center to proj
    OVector3D O2H = OVector3D(O, proj);
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
    ORepere3D r = ORepere3D(O, x2b0 , y1b0, z2b0);
    box.BoxRotationOzOy(alpha, theta);

    return OBox2(box, r, O);
}

void OBox2::moveAndRotate(const OPoint3D& origin, const OVector3D& vec)
{
    _center = origin;
    _repere = ORepere3D(origin, vec);

    OMatrix mat = _repere.asMatrix();

    _A = mat * _A;
    _B = mat * _B;
    _C = mat * _C;
    _D = mat * _D;
    _E = mat * _E;
    _F = mat * _F;
    _G = mat * _G;
    _H = mat * _H;
}


