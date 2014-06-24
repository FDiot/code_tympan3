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




#include "OGeometrie.h"

#define MAX(A,B) (((A)>(B))?(A):(B))
#define MIN(A,B) (((A)>(B))?(B):(A))
#define minP(A,B,C) ( (((A).C) > ((B).C)) ? (B) : (A) )
#define coeff_ang(PA,PB) ( ((PB)._y - (PA)._y) / ((PB)._x - (PA)._x) )

const double EPSILON_5 = 0.00001;

/*static*/ bool OGeometrie::intersDemiSegmentAvecSegment(const OPoint3D& ptS, const OPoint3D& ptA, const OPoint3D& ptB)
{
    double eps = 0.0;

    if (ptS._y == MAX(ptA._y, ptB._y) || ptS._y == MIN(ptA._y, ptB._y))
    {
        eps = EPSILON_5;
    }

    if ((ptS._y + eps) > MAX(ptA._y, ptB._y) || (ptS._y + eps) < MIN(ptA._y, ptB._y) || ptS._x > MAX(ptA._x, ptB._x))
    {
        return false;
    }

    if (ptS._x <= MIN(ptA._x, ptB._x))
    {
        return true;
    }

    double ca = (ptA._x != ptB._x) ? coeff_ang(ptA, ptB) : HUGE_VAL;
    OPoint3D my = minP(ptA, ptB, _y);
    double cp = (ptS._x - my._x) ? coeff_ang(my, ptS) : HUGE_VAL;

    return cp >= ca;
}

/*static*/ int OGeometrie::intersDroitesPoints(const OPoint3D& ptA, const OPoint3D& ptB, const OPoint3D& ptC, const OPoint3D& ptD, OPoint3D& ptI)
{
    int res = INTERS_NULLE;

    double k;
    double d = (ptC._x - ptD._x) * (ptA._y - ptB._y) - (ptC._y - ptD._y) * (ptA._x - ptB._x);

    k = 0;

    if (ABS(d) > 0.0)
    {
        k = ((ptA._x - ptD._x) * (ptA._y - ptB._y) - (ptA._y - ptD._y) * (ptA._x - ptB._x)) / d;

        res = INTERS_OUI;
    }
    else
    {
        d = (ptC._x - ptD._x) * (ptA._z - ptB._z) - (ptC._z - ptD._z) * (ptA._x - ptB._x);

        if (ABS(d) > 0.0)
        {
            k = ((ptA._x - ptD._x) * (ptA._z - ptB._z) - (ptA._z - ptD._z) * (ptA._x - ptB._x)) / d;

            res = INTERS_OUI;
        }
        else
        {
            d = (ptC._y - ptD._y) * (ptA._z - ptB._z) - (ptC._z - ptD._z) * (ptA._y - ptB._y);

            if (ABS(d) > 0.0)
            {
                k = ((ptA._y - ptD._y) * (ptA._z - ptB._z) - (ptA._z - ptD._z) * (ptA._y - ptB._y)) / d;

                res = INTERS_OUI;
            }
        }
    }

    if (res)
    {
        ptI._x = ptD._x + k * (ptC._x - ptD._x);
        ptI._y = ptD._y + k * (ptC._y - ptD._y);
        ptI._z = ptD._z + k * (ptC._z - ptD._z);
    }

    return res;
}

/*static*/ int OGeometrie::intersDroitesPointVecteur(const OPoint3D& ptA, const OVector3D& vecA, const OPoint3D& ptB, const OVector3D& vecB, OPoint3D& ptI)
{
    OVector3D vecA1, vecB1;

    vecA1 = OVector3D(ptA) + vecA;
    vecB1 = OVector3D(ptB) + vecB;

//    return intersDroitesPoints(ptA, OPoint3D(*(OCoord3D*)&vecA1), ptB, OPoint3D(*(OCoord3D*)&vecB1), ptI);
    return intersDroitesPoints(ptA, OPoint3D(vecA1), ptB, OPoint3D(vecB1), ptI);
}

/*static*/ double OGeometrie::symPointDroite(const OPoint3D& ptA, const OPoint3D& ptB, const OPoint3D& ptP, OPoint3D& ptI)
{
    // Determination du point M tel que :
    //  Vecteur PM perpendiculaire a Vecteur AB
    //  Vecteur AM colineaire a Vecteur AB

    OVector3D vecAB(ptA, ptB);
    OVector3D vecPtA(ptA);
    OVector3D vecPtP(ptP);
    double k;

    k = (ptP._x - ptA._x) * vecAB._x + (ptP._y - ptA._y) * vecAB._y + (ptP._z - ptA._z) * vecAB._z;
    k /= vecAB._x * vecAB._x + vecAB._y * vecAB._y + vecAB._z * vecAB._z;

    // Determination de P', image de P par rapport a M
    // P' = OA + AM + MP' = OA + AM + PM
    // Avec OA = ptA
    //      AM = k.AB
    //      PM = PA + kAB = (ptA - ptP) + kAB
    // Soit P' = ptA + kAB + ptA - ptP + kAB
    //         = 2(ptA + kAB) - ptP

    // This is evil ! What the hell has it been put here for ?
    //  *(OCoord3D*)&ptI = *(OCoord3D*) & (2 * (vecPtA + (k * vecAB)) - vecPtP);
    // Why be so complicated
    //    (OCoord3D)ptI = (OCoord3D)(2 * (vecPtA + (k * vecAB)) - vecPtP);
    // A very simple solution !
    ptI = 2 * (vecPtA + (k * vecAB)) - vecPtP;

    return k;
}

/*static*/ bool OGeometrie::pointInPolygonAngleSum(const OPoint3D& ptP, const OPoint3D* pts, int nbPts)
{
    int i;
    double m1m2;
    double angle;
    double anglesum = 0;
    double sign = 1;
    OVector3D vec1(ptP, pts[0]);
    OVector3D vec2;

    for (i = 0; i < nbPts; i++)
    {
        vec2 = OVector3D(ptP, pts[(i + 1) % nbPts]);

        m1m2 = vec1.norme() * vec2.norme();

        if (m1m2 <= EPSILON_6)
        {
            // We are on a node, consider this inside
            return true;
        }

        // Angle forme par les 2 vecteurs
        angle = acos((vec1.scalar(vec2)) / (m1m2));

        // Signe de l'angle
        sign = (vec1.cross(vec2)._z > 0) ? -1 : 1;

        anglesum += (angle * sign);

        vec1 = vec2;
    }

    // Le total des angles doit etre egal a 2Pi (a Epsilon pres)
    return ((ABS(anglesum) > (M_2PI - EPSILON_6)) && (ABS(anglesum) < (M_2PI + EPSILON_6)));
}


#if TY_USE_IHM
//in 2D
/*static*/ bool OGeometrie::pointInPolygonRayCasting(const OPoint3D& ptP, const OPoint3D* pts, int nbPts)
{
    int cross = 0, i;
    bool res;

    for (i = 0; i < nbPts - 1; i++)
    {
        if (intersDemiSegmentAvecSegment(ptP, pts[i], pts[i + 1]))
        {
            cross++;
        }
    }

    if (intersDemiSegmentAvecSegment(ptP, pts[i], pts[0]))
    {
        cross++;
    }

    res = cross % 2 != 0;

    return res;
}
#else // TY_USE_IHM
/*static*/ bool OGeometrie::pointInPolygonRayCasting(const OPoint3D& ptP, const OPoint3D* pts, int nbPts, const OVector3D& normal, const OBox& box)
{
    return false;
}
#endif // TY_USE_IHM

/*static*/ bool OGeometrie::shortestSegBetween2Lines(const OPoint3D& pt1, const OPoint3D& pt2, const OPoint3D& pt3, const OPoint3D& pt4,
                                                     OPoint3D& ptA, OPoint3D& ptB, double* mua, double* mub)
{
    OPoint3D p13, p43, p21;
    double d1343, d4321, d1321, d4343, d2121;
    double numer, denom;

    p13._x = pt1._x - pt3._x;
    p13._y = pt1._y - pt3._y;
    p13._z = pt1._z - pt3._z;
    p43._x = pt4._x - pt3._x;
    p43._y = pt4._y - pt3._y;
    p43._z = pt4._z - pt3._z;

    if ((ABS(p43._x) < EPSILON_6) && (ABS(p43._y) < EPSILON_6) && (ABS(p43._z) < EPSILON_6))
    {
        return false;
    }

    p21._x = pt2._x - pt1._x;
    p21._y = pt2._y - pt1._y;
    p21._z = pt2._z - pt1._z;

    if ((ABS(p21._x) < EPSILON_6) && (ABS(p21._y) < EPSILON_6) && (ABS(p21._z) < EPSILON_6))
    {
        return false;
    }

    d1343 = p13._x * p43._x + p13._y * p43._y + p13._z * p43._z;
    d4321 = p43._x * p21._x + p43._y * p21._y + p43._z * p21._z;
    d1321 = p13._x * p21._x + p13._y * p21._y + p13._z * p21._z;
    d4343 = p43._x * p43._x + p43._y * p43._y + p43._z * p43._z;
    d2121 = p21._x * p21._x + p21._y * p21._y + p21._z * p21._z;

    denom = d2121 * d4343 - d4321 * d4321;

    if (ABS(denom) < EPSILON_6)
    {
        return false;
    }

    numer = d1343 * d4321 - d1321 * d4343;

    *mua = numer / denom;
    *mub = (d1343 + d4321 * (*mua)) / d4343;

    ptA._x = pt1._x + *mua * p21._x;
    ptA._y = pt1._y + *mua * p21._y;
    ptA._z = pt1._z + *mua * p21._z;
    ptB._x = pt3._x + *mub * p43._x;
    ptB._y = pt3._y + *mub * p43._y;
    ptB._z = pt3._z + *mub * p43._z;

    return true;
}

/*static*/ void OGeometrie::boundingBox(OPoint3D* pts, int nbPts, OPoint3D& ptMin, OPoint3D& ptMax)
{
    if (nbPts <= 0)
    {
        return;
    }

    // Init
    ptMax = ptMin = pts[0];

    for (int i = 0; i < nbPts; i++)
    {
        // Minimuns
        if (pts[i]._x <= ptMin._x)
        {
            ptMin._x = pts[i]._x;
        }
        if (pts[i]._y <= ptMin._y)
        {
            ptMin._y = pts[i]._y;
        }
        if (pts[i]._z <= ptMin._z)
        {
            ptMin._z = pts[i]._z;
        }
        // Maximums
        if (pts[i]._x >= ptMax._x)
        {
            ptMax._x = pts[i]._x;
        }
        if (pts[i]._y >= ptMax._y)
        {
            ptMax._y = pts[i]._y;
        }
        if (pts[i]._z >= ptMax._z)
        {
            ptMax._z = pts[i]._z;
        }
    }
}

/* static */ void OGeometrie::computeNormal(OPoint3D* pts, int nbPts, OVector3D& normal)
{
    float ax, ay, az, bx, by, bz;
    //  Because polygon may be concave, need to accumulate cross products to
    //  determine true normal.
    //
    //assert(nbPts>=3);
    OVector3D vec(0, 0, 0);
    OVector3D vecPt0; //set things up for loop
    OVector3D vecPt1(pts[0]);
    OVector3D vecPt2(pts[1]);

    for (int i = 2; i < nbPts; i++)
    {
        vecPt0 = vecPt1;
        vecPt1 = vecPt2;
        vecPt2 = OVector3D(pts[i]);

        // order is important!!! to maintain consistency with polygon vertex order
        ax = vecPt2._x - vecPt1._x; ay = vecPt2._y - vecPt1._y; az = vecPt2._z - vecPt1._z;
        bx = vecPt0._x - vecPt1._x; by = vecPt0._y - vecPt1._y; bz = vecPt0._z - vecPt1._z;

        vec._x += (ay * bz - az * by);
        vec._y += (az * bx - ax * bz);
        vec._z += (ax * by - ay * bx);
    }

    vec.normalize();

    normal = vec;
}



