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


#include <cassert>

#include "OPlan.h"
#include "OGeometrie.h"

OPlan::OPlan():
    _a(0.0),
    _b(0.0),
    _c(0.0),
    _d(0.0)
{
    // Do not call this : update_explicit_repr();
}

OPlan::OPlan(const OPlan& plan)
{
    *this = plan;
    update_explicit_repr();
}

OPlan::OPlan(double a, double b, double c, double d):
    _a(a),
    _b(b),
    _c(c),
    _d(d)
{
    update_explicit_repr();
}

OPlan::OPlan(const OPoint3D& pt1, const OPoint3D& pt2, const OPoint3D& pt3)
{
    set(pt1, pt2, pt3);
}

OPlan::OPlan(const OPoint3D& pt, const OVector3D& normale)
{
    set(pt, normale);
}

OPlan::~OPlan()
{
}

OPlan& OPlan::operator=(const OPlan& plan)
{
    if (this != &plan)
    {
        _a = plan._a;
        _b = plan._b;
        _c = plan._c;
        _d = plan._d;
    }
    return *this;
    update_explicit_repr();
}

bool OPlan::operator==(const OPlan& plan) const
{
    if (this != &plan)
    {
        if (_a != plan._a) { return false; }
        if (_b != plan._b) { return false; }
        if (_c != plan._c) { return false; }
        if (_d != plan._d) { return false; }
    }
    return true;
}

bool OPlan::operator!=(const OPlan& plan) const
{
    return !operator==(plan);
}

void OPlan::set(double a, double b, double c, double d)
{
    _a = a;
    _b = b;
    _c = c;
    _d = d;
    update_explicit_repr();
}

void OPlan::set(const OPoint3D& pt1, const OPoint3D& pt2, const OPoint3D& pt3)
{
    _a = pt1._y * (pt2._z - pt3._z) + pt2._y * (pt3._z - pt1._z) + pt3._y * (pt1._z - pt2._z);
    _b = pt1._z * (pt2._x - pt3._x) + pt2._z * (pt3._x - pt1._x) + pt3._z * (pt1._x - pt2._x);
    _c = pt1._x * (pt2._y - pt3._y) + pt2._x * (pt3._y - pt1._y) + pt3._x * (pt1._y - pt2._y);
    _d = - (pt1._x * (pt2._y * pt3._z - pt3._y * pt2._z) +
            pt2._x * (pt3._y * pt1._z - pt1._y * pt3._z) +
            pt3._x * (pt1._y * pt2._z - pt2._y * pt1._z));
    update_explicit_repr();
}

void OPlan::set(const OPoint3D& pt, const OVector3D& normale)
{
    _a = normale._x;
    _b = normale._y;
    _c = normale._z;
    _d = -normale.scalar(pt);
    update_explicit_repr();
}

bool OPlan::is_valid()
{ return _a!=0 || _b!=0 || _c!=0; }

#define ___XBH_VERSION
#ifdef ___XBH_VERSION

int OPlan::intersectsSegment(const OPoint3D& pt1, const OPoint3D& pt2, OPoint3D& ptIntersec) const
{
    int res = INTERS_NULLE;

    double xSeg = pt2._x - pt1._x;
    double ySeg = pt2._y - pt1._y;
    double zSeg = pt2._z - pt1._z;

    double ps = xSeg * _a + ySeg * _b + zSeg * _c;
    double ps1 = pt1._x * _a + pt1._y * _b + pt1._z * _c;
    double  t;

    if (ps != 0)
    {
        // La droite par laquelle passe le segment coupe le plan...
        t = -(ps1 + _d) / ps;

        // ...mais est-ce que le segment lui-meme coupe le plan ?
        if ((t >= 0) && (t <= 1))
        {
            ptIntersec._x = pt1._x + t * (pt2._x - pt1._x);
            ptIntersec._y = pt1._y + t * (pt2._y - pt1._y);
            ptIntersec._z = pt1._z + t * (pt2._z - pt1._z);

            ptIntersec._x =  ABS(ptIntersec._x) > 1E-6 ? ptIntersec._x : 0.00;
            ptIntersec._y =  ABS(ptIntersec._y) > 1E-6 ? ptIntersec._y : 0.00;
            ptIntersec._z =  ABS(ptIntersec._z) > 1E-6 ? ptIntersec._z : 0.00;

            res = INTERS_OUI;
        }
    }
    else    // Le segment est parallele au plan
    {
        // Est-il dans le plan ?

        double z = ps1 + _d;

        if (ABS(z) < EPSILON_PRECIS)
        {
            res = INTERS_CONFONDU;
        }
    }

    return res;
}

#else

int OPlan::intersectsSegment(const OPoint3D& pt1, const OPoint3D& pt2, OPoint3D& ptIntersec) const
{
    int res = INTERS_NULLE;

    OVector3D n(_a, _b, _c);
    OVector3D vecPt1(pt1);
    OVector3D vecPt2(pt2);
    OVector3D vecSeg = vecPt2 - vecPt1;

    double ps = vecSeg.scalar(n);
    double  t;

    if (ps != 0)
    {
        // La droite par laquelle passe le segment coupe le plan...
        t = -(vecPt1.scalar(n) + _d) / ps;

        // ...mais est-ce que le segment lui-meme coupe le plan ?
        if ((t >= 0) && (t <= 1))
        {
            ptIntersec._x = pt1._x + t * (pt2._x - pt1._x);
            ptIntersec._y = pt1._y + t * (pt2._y - pt1._y);
            ptIntersec._z = pt1._z + t * (pt2._z - pt1._z);

            ptIntersec._x =  ABS(ptIntersec._x) > 1E-6 ? ptIntersec._x : 0.00;
            ptIntersec._y =  ABS(ptIntersec._y) > 1E-6 ? ptIntersec._y : 0.00;
            ptIntersec._z =  ABS(ptIntersec._z) > 1E-6 ? ptIntersec._z : 0.00;
            res = INTERS_OUI;
        }
    }
    else    // Le segment est parallele au plan
    {
        // Est-il dans le plan ?

        double z = vecPt1.scalar(n) + _d;

        if (ABS(z) < EPSILON_PRECIS)
        {
            res = INTERS_CONFONDU;
        }
    }

    return res;
}
#endif //___XBH_VERSION

bool OPlan::isInPlan(const OPoint3D& pt)
{
    OVector3D n(_a, _b, _c);
    OVector3D vecPt1(pt);
    bool res = false;

    double z = vecPt1.scalar(n) + _d;

    if (ABS(z) < EPSILON_PRECIS)
    {
        res = true;
    }

    return res;
}

int OPlan::intersectsDroite(const OPoint3D& pt, const OVector3D& vector, OPoint3D& ptIntersec)
{
    int res = INTERS_NULLE;

    OVector3D   n(_a, _b, _c);
    OVector3D   vecPt(pt);
    double      ps;

    ps = vector.scalar(n);

    if (ABS(ps) > 0.0)
    {
        double t = -(vecPt.scalar(n) + _d) / ps;

        ptIntersec._x = pt._x + t * vector._x;
        ptIntersec._y = pt._y + t * vector._y;
        ptIntersec._z = pt._z + t * vector._z;

        res = INTERS_OUI;
    }

    return res;
}

int OPlan::intersectsPlan(const OPlan& plan, OVector3D& vectorIntersec)
{
    int res = INTERS_OUI;

    double  Dxy = _a * plan._b - plan._a * _b;
    double  Dyz = _b * plan._c - plan._b * _c;
    double  Dzx = _c * plan._a - plan._c * _a;

    if (ABS(Dxy) >= EPSILON_PRECIS)
    {
        vectorIntersec._x = Dyz / Dxy;
        vectorIntersec._y = Dzx / Dxy;
        vectorIntersec._z = 1.0;
    }
    else if (ABS(Dyz) >= EPSILON_PRECIS)
    {
        vectorIntersec._x = 1.0;
        vectorIntersec._y = Dzx / Dyz;
        vectorIntersec._z = Dxy / Dyz;
    }
    else if (ABS(Dzx) >= EPSILON_PRECIS)
    {
        vectorIntersec._x = Dyz / Dzx;
        vectorIntersec._y = 1.0;
        vectorIntersec._z = Dxy / Dzx;
    }
    else
    {
        vectorIntersec._x = vectorIntersec._y = vectorIntersec._z = 0;
        res = INTERS_CONFONDU;
    }

    if (res == INTERS_OUI)
    {
        // On norme, c'est plus propre...
        double norme = vectorIntersec.norme();

        if (norme != 0.0)
        {
            vectorIntersec._x /= norme;
            vectorIntersec._y /= norme;
            vectorIntersec._z /= norme;
        }
    }

    return res;
}

double OPlan::angle(const OPlan& plan)
{
    double cosAngle = (_a * plan._a + _b * plan._b + _c * plan._c) /
                      (OVector3D(_a, _b, _c).norme() * OVector3D(plan._a, plan._b, plan._c).norme());

    return acos(cosAngle);
}

double OPlan::distance(const OPoint3D& pt)
{
    return ((_a * pt._x + _b * pt._y + _c * pt._z + _d) / OVector3D(pt).norme());
}

bool OPlan::distancePlanParallel(const OPlan& plan, double& distance)
{
    if (!isParallel(plan)) { return false; }

    distance = ABS(_d - plan._d) / OVector3D(_a, _b, _c).norme();

    return true;
}

bool OPlan::isParallel(const OPlan& plan)
{
    if (isOrthogonal(plan)) { return false; }

    if (((_a / plan._a) == (_b / plan._b)) && ((_b / plan._b) == (_c / plan._c))) { return true; }

    return false;
}

bool OPlan::isOrthogonal(const OPlan& plan)
{
    if (((_a * plan._a) + (_b * plan._b) + (_c * plan._c)) == 0) { return true; }
    return false;
}

void OPlan::update_explicit_repr( OVector3D hint /* = OVector3D(1, 1, 1) */ )
{
    // We check the plane is valid
    // assert(is_valid()); // This is too strong, invalid planes are crated by TYRectangles e.g.
    if(!is_valid())
        return;

    // The origin of the plane is the projection on the plane of the 3D origin.
    OVector3D N(_a, _b, _c);
    N.normalize();
    // Need to ensure that the hint vector and the normal vectors are NOT colinear
    hint.normalize();
    if (N.dot(hint) > 0.9) // Test for near colinearity
    {
        hint = OVector3D(0, 1, 0);
        if (N.dot(hint) > 0.9) // Test for near colinearity
            hint = OVector3D(0, 1, 0);
    }
    // Solve l.N belonging to the plane to find an origin
    double l = - _d / ( _a*N._x + _b*N._y + _c*N._z);
    OPoint3D origin( N*l );
    // orthogonal component of the hint vector
    double h = hint.dot(N);
    OVector3D U(hint - h* N);
    U.normalize();
    assert( fabs(N.dot(U)) < 0.001 ) ; // Check validity of the construction
    OVector3D V( N.cross(U) );
    assert( fabs(V.norme()-1.0) < 0.01 ) ; // Check validity of the construction
    rframe.set(origin, U, V, N);
}
