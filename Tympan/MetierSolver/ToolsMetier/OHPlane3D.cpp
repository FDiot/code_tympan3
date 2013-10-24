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




#include "OHPlane3D.h"
#include "OGeometrie.h"

#define dot(u,v)   ((u)._x * (v)._x + (u)._y * (v)._y + (u)._z * (v)._z)

OHPlane3D::OHPlane3D()
{
}

OHPlane3D::OHPlane3D(const OHPlane3D& Plane)
{
    _N = Plane._N;
    _p = Plane._p;
    _O = Plane._O;
}

OHPlane3D::OHPlane3D(const OVector3D& normal, const OPoint3D& origin)
{
    _N = normal;
    _O = origin;
    _p = -dot(_N, _O);
}

OHPlane3D::~OHPlane3D()
{
}

void OHPlane3D::set(const OVector3D& normal, const OPoint3D& origin)
{
    _N = normal;
    _O = origin;
    _p = -dot(_N, _O);
}

OHPlane3D& OHPlane3D::operator=(const OHPlane3D& Plane)
{
    _N = Plane._N;
    _p = Plane._p;
    _O = Plane._O;
    return *this;
}

bool OHPlane3D::operator==(const OHPlane3D& Plane) const
{
    bool res = true;

    res = res && (_N == Plane._N);
    res = res && (_p == Plane._p);

    return res;
}

bool OHPlane3D::operator!=(const OHPlane3D& Plane) const
{
    return !operator==(Plane);
}

int OHPlane3D::intersects(const OSegment3D& seg, OPoint3D& ptIntersec) const
{
    double t;
    return intersects(seg._ptA, seg._ptB, ptIntersec, t);
}

/**
 * For a detailed description of the algorithm used by this function refer to
 * the following link:
 *
 * Intersections of Lines, Segments and Planes (2D and 3D)
 * <http://geometryalgorithms.com/Archive/algorithm_0104/algorithm_0104B.htm>
 *
 */
int OHPlane3D::intersects(const OPoint3D& pt1, const OPoint3D& pt2, OPoint3D& ptIntersec, double& t) const
{
    OVector3D    u(pt1, pt2);
    OVector3D    w(_O, pt1);

    double     D = dot(_N, u);
    double     N = -dot(_N, w);

    if (ABS(D) < EPSILON_PRECIS)        // segment is parallel to plane
    {
        if (ABS(N) < EPSILON_PRECIS)   // segment lies in plane
        {
            return 2;
        }
        else
        {
            return 0;    // no intersection
        }
    }
    // they are not parallel
    // compute intersect param
    double sI = N / D;
    if ((sI < 0.0f) || (sI > 1.0f))
    {
        return 0;    // no intersection
    }

    ptIntersec = OVector3D(pt1) + sI * u;             // compute segment intersect point
    t = sI;

    return 1;
}
