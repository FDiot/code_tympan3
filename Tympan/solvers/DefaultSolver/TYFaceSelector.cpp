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


#include "Tympan/models/business/geoacoustic/TYAcousticVolume.h"
#include "Tympan/core/defines.h"

#include "TYFaceSelector.h"
#include "TYSolver.h"

TYFaceSelector::TYFaceSelector(TYSolver& solver)
    : _solver(solver)
{

}

TYFaceSelector::~TYFaceSelector()
{

}

void TYFaceSelector::selectFaces(std::deque<TYSIntersection>& tabIntersect, const OSegment3D& rayon)
{
    int i;
    short XY = 0, XZ = 1;

    // Construction des plans de coupe
    TYSPlan plan[2];
    buildPlans(plan, rayon);

    size_t nbFaces = _solver.getTabPolygon().size();

    // Test des faces qui coupent le plan vertical
    for (i = 0; i < nbFaces; i++)
    {
        const TYStructSurfIntersect& SI = _solver.getTabPolygon()[i];

        if ((SI.tabPoint.size() == 0)) { continue; }

        // Plan vertical = 0 / Plan horizontal = 1
        TYSIntersection intersection;
        bool bVertical = CalculSegmentCoupe(SI, intersection, plan[0].pt1, plan[0].pt2, plan[0].pt3, 0);
        bool bHorizontal = CalculSegmentCoupe(SI, intersection, plan[1].pt1, plan[1].pt2, plan[1].pt3, 1);

        if (bVertical || bHorizontal) { tabIntersect.push_back(intersection); }
    }
}


bool TYFaceSelector::buildPlans(TYSPlan* plan, const OSegment3D& rayon)
{
    double planOffset = 10.0;
    TYSPlan tmpPlan;
    tmpPlan.pt1 = rayon._ptA;
    tmpPlan.pt2 = rayon._ptB;

    // Construction du plan vertical
    // 3eme point simplement en copiant le 1er et changeant l'alti
    tmpPlan.pt3 = tmpPlan.pt1;
    tmpPlan.pt3._z += planOffset;

    // Si A et B sont confondus (en 2D) sur le plan que l'on souhaite creer
    if ((tmpPlan.pt1._x == tmpPlan.pt2._x) && (tmpPlan.pt1._y == tmpPlan.pt2._y))
    {
        // On deplace le point 2 pour choisir un plan arbitraire valide
        tmpPlan.pt2._x += planOffset;
        tmpPlan.pt2._y += planOffset;
    }

    plan[0] = tmpPlan;

    // Construction du plan horizontal
    tmpPlan.pt1 = rayon._ptA;
    tmpPlan.pt2 = rayon._ptB;

    if (tmpPlan.pt2._x != tmpPlan.pt1._x)
    {
        tmpPlan.pt3._z = tmpPlan.pt1._z;
        tmpPlan.pt3._y = tmpPlan.pt1._y + planOffset;
        tmpPlan.pt3._x = ((tmpPlan.pt1._y - tmpPlan.pt2._y) * planOffset / (tmpPlan.pt2._x - tmpPlan.pt1._x)) + (tmpPlan.pt1._x);
    }
    else if (tmpPlan.pt1._y != tmpPlan.pt2._y)
    {
        tmpPlan.pt3._z = tmpPlan.pt1._z;
        tmpPlan.pt3._x = tmpPlan.pt1._x + planOffset;
        tmpPlan.pt3._y = ((tmpPlan.pt2._x - tmpPlan.pt1._x) * planOffset / (tmpPlan.pt1._y - tmpPlan.pt2._y)) + (tmpPlan.pt1._y);
    }
    else if (tmpPlan.pt1._z != tmpPlan.pt2._z)
    {
        tmpPlan.pt3._y = tmpPlan.pt1._y;
        tmpPlan.pt3._x = tmpPlan.pt1._x + planOffset;
        tmpPlan.pt3._z = ((tmpPlan.pt2._x - tmpPlan.pt1._x) * planOffset / (tmpPlan.pt1._z - tmpPlan.pt2._z)) + (tmpPlan.pt1._z);
    }
    else
    {
        return false;
    }


    plan[1] = tmpPlan;


    return true;
}

bool TYFaceSelector::CalculSegmentCoupe( const TYStructSurfIntersect& FaceCourante,
                                         TYSIntersection& Intersect,
                                         OPoint3D& pt1, OPoint3D& pt2, OPoint3D& pt3,
                                         const int& indice) const
{
    bool bRes = false;
    OSegment3D segInter;
    OPlan planRayon(pt1, pt2, pt3);
    if ( planRayon.intersectsSurface(FaceCourante.tabPoint, segInter) )
    {
        Intersect.bIntersect[indice] = true;
        Intersect.segInter[indice] = segInter;
        Intersect.isEcran = FaceCourante.isEcran;
        Intersect.isInfra = FaceCourante.isInfra;
        Intersect.material = FaceCourante.material;
        bRes = true;
    }
    else
    {
        Intersect.bIntersect[indice] = false;
    }
    return  bRes;
}
