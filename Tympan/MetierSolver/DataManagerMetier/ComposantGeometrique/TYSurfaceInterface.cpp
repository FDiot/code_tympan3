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



#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/MetierSolver/ToolsMetier/OSegment3D.h"


int TYSurfaceInterface::intersects(const TYSurfaceInterface* pSurf, OSegment3D& seg) const
{
    if (!pSurf) { return INTERS_NULLE; }

    int res = INTERS_NULLE;
    int i = 0;
    bool ptAFind = false, ptBFind = false;
    OPoint3D ptTmp;

    // Contour 2
    TYTabPoint3D contour2 = pSurf->getOContour();
    size_t nbPts = contour2.size();

    // Pour chaque segment composant le contour 2
    for (i = 0; i < nbPts; i++)
    {
        // On chercher les points d'intersection entre les segments du contour 2 et cette surface
        if (this->intersects(OSegment3D(contour2[i], contour2[(i + 1) % nbPts]), ptTmp) != INTERS_NULLE)
        {
            if (!ptAFind)
            {
                // Un 1er point a ete trouve
                seg._ptA = ptTmp;
                ptAFind = true;
            }
            else if (!ptBFind)
            {
                // Un 2eme point a ete trouve
                seg._ptB = ptTmp;
                ptBFind = true;
            }
        }
    }

    // Contour 1
    TYTabPoint3D contour1 = getOContour();
    nbPts = contour1.size();

    // Pour chaque segment composant le contour 2
    for (i = 0; i < nbPts; i++)
    {
        // On chercher les points d'intersection entre les segments du contour 1 et la surface a tester
        if (pSurf->intersects(OSegment3D(contour1[i], contour1[(i + 1) % nbPts]), ptTmp) != INTERS_NULLE)
        {
            if (!ptAFind)
            {
                // Un 1er point a ete trouve
                seg._ptA = ptTmp;
                ptAFind = true;
            }
            else if (!ptBFind)
            {
                // Un 2eme point a ete trouve
                seg._ptB = ptTmp;
                ptBFind = true;
            }
        }
    }

    // Les 2 points ont ete trouve ?
    if (ptBFind && ptBFind)
    {
        // Nous avons donc notre segment intersection
        res = INTERS_OUI;
    }

    return res;
}

int TYSurfaceInterface::intersectsPlan(const OPlan& plan, OSegment3D& seg) const
{
    int res = INTERS_NULLE;
    bool ptAFind = false, ptBFind = false;
    OPoint3D ptIntersec;

    // Contour
    TYTabPoint3D contour = this->getOContour();
    size_t nbPts = contour.size();

    // Pour chaque segment composant le contour
    for (size_t i = 0; i < nbPts; i++)
    {
        if (plan.intersectsSegment(contour[i], contour[(i + 1) % nbPts], ptIntersec) == INTERS_OUI)
        {
            if (!ptAFind)
            {
                // Un 1er point a ete trouve
                seg._ptA = ptIntersec;
                ptAFind = true;
            }
            else if (!ptBFind)
            {
                // Un 2eme point a ete trouve
                if (ptIntersec == seg._ptA) { continue ; } // Cas ou le plan passe exactement par un point
                seg._ptB = ptIntersec;
                ptBFind = true;

                // Si on a trouve point B c'est qu'on a trouve point A on peut sortir victorieusement
                res = INTERS_OUI;
                break;
            }
        }
    }

    return res;
}


void TYSurfaceInterface::exportMesh(
    std::deque<OPoint3D>& points,
    std::deque<OTriangle>& triangles,
    const TYGeometryNode& geonode) const
{
    assert(false && "OPROTO does not support pure virtual methods");
}
