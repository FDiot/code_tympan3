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




#include <math.h>
#include "OGeometrie.h"
#include "OSegment3D.h"



OSegment3D::OSegment3D()
{
    _ptA._x = 0; _ptA._y = 0; _ptA._z = 0;
    _ptB._x = 0; _ptB._y = 1; _ptB._z = 0;
}

OSegment3D::OSegment3D(const OSegment3D& other)
{
    *this = other;
}

OSegment3D::OSegment3D(const OPoint3D& ptA, const OPoint3D& ptB)
{
    _ptA = ptA;
    _ptB = ptB;
}

OSegment3D::~OSegment3D()
{
}

OSegment3D& OSegment3D::operator=(const OSegment3D& other)
{
    if (this != &other)
    {
        _ptA = other._ptA;
        _ptB = other._ptB;
    }
    return *this;
}

bool OSegment3D::operator==(const OSegment3D& other) const
{
    if (this != &other)
    {
        if (_ptA != other._ptA) { return false; }
        if (_ptB != other._ptB) { return false; }
    }
    return true;
}

bool OSegment3D::operator!=(const OSegment3D& other) const
{
    return !operator==(other);
}


OSegment3D OSegment3D::operator*(const OMatrix& matrix) const
{
    return OSegment3D(matrix * _ptA, matrix * _ptB);
}

double OSegment3D::longueur() const
{
    return OVector3D(_ptA, _ptB).norme();
}

int OSegment3D::symetrieOf(const OPoint3D& pt, OPoint3D& ptSym) const
{
    int res = 0;

    // Calcul le pt image a ptSym (pt source) par rapport a la droite definie passant
    // par ce segment
    double k = OGeometrie::symPointDroite(_ptA, _ptB, pt, ptSym);

    // Le coefficient k indique si le segment [Point source / Point image] coupe
    // ce segment, pour cela il faut : 0 < k < 1.
    if ((0.0 <= k) && (k <= 1.0))
    {
        res = 1;
    }

    return res;
}

int OSegment3D::projection(const OPoint3D& pt, OPoint3D& ptProj, double seuilConfondus) const
{
    int res = 0;
    OPoint3D ptSym;

    res = symetrieOf(pt, ptSym);
    res = res && intersects(OSegment3D(pt, ptSym), ptProj, seuilConfondus);

    return res;
}

int OSegment3D::intersects(const OSegment3D& seg, OPoint3D& pt, double seuilConfondus) const
{
    int res = INTERS_NULLE;

    OPoint3D ptA, ptB;
    double mua, mub;

    // Calcul le segment le plus court entre les 2 segments dont on recherche l'eventuelle intersection
    if (OGeometrie::shortestSegBetween2Lines(this->_ptA, this->_ptB, seg._ptA, seg._ptB, ptA, ptB, &mua, &mub))
    {
        // Les coefs mua et mub doivent etre compris entre 0 et 1 pour que les points constituant
        // le segment trouve se trouve sur les segments respectifs a tester
        if ((0.0 <= mua) && (mua <= 1.0) && (0.0 <= mub) && (mub <= 1.0))
        {
            // La norme du segment trouve doit etre inferieur a un seuil pour que l'on puisse admettre
            // que les 2 points constituant ce segment sont quasi confondus
            OVector3D vecSeg(ptA, ptB);

            if (vecSeg.norme() <= seuilConfondus)
            {
                res = INTERS_OUI;

                // Arbitrairement on prend le 1er point du segment trouve comme point d'intersection.
                pt = ptA;
            }
        }
    }

    return res;
}

OPoint3D OSegment3D::centreOf() const
{
    double x, y, z;

    x = ((_ptB._x - _ptA._x) / 2.0) + _ptA._x;
    y = ((_ptB._y - _ptA._y) / 2.0) + _ptA._y;
    z = ((_ptB._z - _ptA._z) / 2.0) + _ptA._z;

    return OPoint3D(x, y, z);
}

OPoint3D OSegment3D::centerOfCurvedPath(const double& R) const
{
    OPoint3D point;
    OPoint3D milieu = centreOf() ;
    double demiLongueur = longueur() / 2;
    double d, x, y, z, q;
    x = milieu._x; y = milieu._y; z = milieu._z;

    if (R > demiLongueur)
    {
        d = sqrt(R * R - demiLongueur * demiLongueur);

        if (_ptA._z != _ptB._z)
        {
            d = d * SIGNE(_ptA._z - _ptB._z);
            q = (_ptB._y - _ptA._y) / (_ptB._z - _ptA._z);
            y = milieu._y + d / sqrt(1 + q * q);
        }
        z = milieu._z + sqrt(ABS(d * d - (y - milieu._y) * (y - milieu._y)));
    }

    point.setCoords(x, y, z);
    return point;
}

double OSegment3D::lengthOfCurvedPath(const double& R)
{
    // Longueur du cote du triangle rectangle
    double cote = longueur() / 2.0;
    // Angle d'ouverture du segment
    double angle = 2 * asin(cote / R);

    return angle * R;
}

OSegment3D OSegment3D::swap() const
{
    OSegment3D seg(*this) ;
    OPoint3D PT = seg._ptA;
    seg._ptA = seg._ptB;
    seg._ptB = PT;

    return seg;
}
