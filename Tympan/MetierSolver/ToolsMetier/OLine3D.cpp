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




#include "OLine3D.h"
#include "OGeometrie.h"


OLine3D::OLine3D()
{
}

OLine3D::OLine3D(const OLine3D& Line)
{
    _Origin = Line._Origin;
    _Dir = Line._Dir;
}

OLine3D::OLine3D(const OCoord3D& origin, const OVector3D& dir)
{
    _Origin = origin;
    _Dir = dir;
}

OLine3D::~OLine3D()
{
}

OLine3D& OLine3D::operator=(const OLine3D& Line)
{
    _Origin = Line._Origin;
    _Dir = Line._Dir;
    return *this;
}

bool OLine3D::operator==(const OLine3D& Line) const
{
    bool res = true;

    res = res && (_Origin == Line._Origin);
    res = res && (_Dir == Line._Dir);

    return res;
}

bool OLine3D::operator!=(const OLine3D& Line) const
{
    return !operator==(Line);
}

int OLine3D::intersects(const OSegment3D& seg, OPoint3D& ptIntersec) const
{
    return intersects(seg._ptA, seg._ptB, ptIntersec);
}

int OLine3D::intersects(const OPoint3D& pt1, const OPoint3D& pt2, OPoint3D& ptIntersec) const
{
    return 0;
}

