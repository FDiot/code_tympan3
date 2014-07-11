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

void TYFaceSelector::selectFaces(TYSIntersection* tabIntersect, const TYSourcePonctuelleGeoNode* pSrcGeoNode, const OSegment3D& rayon)
{
    int i;
    short XY = 0, XZ = 1;

    // Construction des plans de coupe
    TYSPlan plan[2];
    buildPlans(plan, rayon);

    // Recuperation de la source ponctuelle
    TYSourcePonctuelle* pSrc = dynamic_cast<TYSourcePonctuelle*>(pSrcGeoNode->getElement());

    TYAcousticSurfaceGeoNode* pSurfaceGeoNode = NULL;
    TYAcousticSurface* pSurface = NULL;

    // Recuperation de la taille  du tableau de faces
    size_t nbFaces = _solver.getTabPolygonSize();

    // Neutralisation des faces qui ne peuvent etre prises en compte (face portant la source)
    for (i = 0; i < nbFaces; i++)
    {
        tabIntersect[i].noIntersect = false;
        tabIntersect[i].bIntersect[0] = false;
        tabIntersect[i].bIntersect[1] = false;

        const TYStructSurfIntersect& SI = _solver.getTabPolygon()[i];

        // Seules les faces infra peuvent etre invalidees
        if ((!SI.isInfra) || (SI.tabPoint.size() == 0))
        {
            continue;
        }

        pSurfaceGeoNode = SI.pSurfGeoNode;
        pSurface = getActiveSurface(pSurfaceGeoNode, pSrc);

        if (pSurface == NULL)
        {
            tabIntersect[i].noIntersect = true;
        }
    }

    // Extension du segment source-recepteur (on se ramene a du 2D)
    OPoint3D pt1 = rayon._ptA;
    OPoint3D pt2 = rayon._ptB;

    //"Pente" XY
    // On cree les valeurs dX et dY car on va les reutiliser pour calculer la tangente par atan2
    double dX = pt2._x - pt1._x;
    double dY = pt2._y - pt1._y;

    dX = fabs(dX) < 1E-7 ? 1E-7 : dX; // Limitation de dy
    double A = dY / dX; // Pente de la droite source recepteur
    double B = pt1._y - (A * pt1._x);
    double alpha = atan2(dY, dX);
    double cosA = cos(alpha);
    double sinA = sin(alpha);

    // Construction du segment de test
    OSegment3D segTest1;
    segTest1._ptA._x = -10000;
    segTest1._ptA._y    = (A * segTest1._ptA._x) + B;
    segTest1._ptB._x = +10000;
    segTest1._ptB._y = (A * segTest1._ptB._x) + B;

    // Changement de repere pour construire un repere XZ
    // dans lequel l'axe source-recepteur projete au sol devient l'axe X
    double dX2 = sqrt((dX * dX) + (dY * dY));
    double dY2 = pt2._z - pt1._z;
    double A2 = dY2 / dX2;
    double B2 = pt1._z - (A2 * (pt1._x * cosA + pt1._y * sinA));

    // Construction du segment de test
    OSegment3D segTest2;
    segTest2._ptA._x = -10000;
    segTest2._ptA._y = (A2 * segTest2._ptA._x) + B2;
    segTest2._ptB._x = +10000;
    segTest2._ptB._y = (A2 * segTest2._ptB._x) + B2;

    // Test des faces qui coupent le plan vertical
    for (i = 0; i < nbFaces; i++)
    {
        if (tabIntersect[i].noIntersect)
        {
            continue;
        }

        const TYStructSurfIntersect& SI = _solver.getTabPolygon()[i];

        if ((SI.tabPoint.size() == 0)) { continue; }

        testIntersect2D(SI, tabIntersect[i], plan, segTest1, cosA, sinA, 0, XY);

        testIntersect2D(SI, tabIntersect[i], plan, segTest2, cosA, sinA, 1, XZ);
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

bool TYFaceSelector::testIntersect2D(const TYStructSurfIntersect& SI, TYSIntersection& Intersect, TYSPlan* plan, const OSegment3D& segTest, const double& cosA, const double& sinA, const short& indice, const short& XYZ)
{
    unsigned short i;
    int res = 0;

    // On commence par initialiser la TYStrucSurfIntersect
    Intersect.bIntersect[indice] = false;

    // Construction du segment a tester
    OPoint3D ptInter;
    OSegment3D seg;
    bool bInter = false;

    if (XYZ == 0) // Plan XY
    {
        seg._ptA._x = SI.tabPoint[0]._x;
        seg._ptA._y = SI.tabPoint[0]._y;

        for (i = 1; i < SI.tabPoint.size(); i++)
        {
            seg._ptB._x = SI.tabPoint[i]._x;
            seg._ptB._y = SI.tabPoint[i]._y;

            // On teste directement le segment constitue par les deux points
            res = seg.intersects(segTest, ptInter, TYSEUILCONFONDUS);

            if (res > 0)
            {
                // On calcule de suite le segment d'intersection
                bInter = CalculSegmentCoupe(SI, Intersect, plan[indice].pt1, plan[indice].pt2, plan[indice].pt3, indice);
                return bInter;
            }

            seg._ptA = seg._ptB;
        }
    }
    else // Plan XZ
    {
        seg._ptA._x = (SI.tabPoint[0]._x * cosA) + (SI.tabPoint[0]._y * sinA);
        seg._ptA._y = SI.tabPoint[0]._z;

        for (i = 1; i < SI.tabPoint.size(); i++)
        {
            seg._ptB._x = (SI.tabPoint[i]._x * cosA) + (SI.tabPoint[i]._y * sinA);
            seg._ptB._y = SI.tabPoint[i]._z;

            // On teste directement le segment constitue par les deux points
            res = seg.intersects(segTest, ptInter, TYSEUILCONFONDUS);

            if (res > 0)
            {
                // On calcule de suite le segment d'intersection
                bInter = CalculSegmentCoupe(SI, Intersect, plan[indice].pt1, plan[indice].pt2, plan[indice].pt3, indice);
                return bInter;
            }

            seg._ptA = seg._ptB;
        }
    }


    return false;
}


bool TYFaceSelector::CalculSegmentCoupe(const TYStructSurfIntersect& FaceCourante, TYSIntersection& Intersect, OPoint3D& pt1, OPoint3D& pt2, OPoint3D& pt3, const int& indice) const
{
    bool bRes = false;
    const OMatrix matrixinv = FaceCourante.matInv;
    TYAcousticSurfaceGeoNode* pSurfaceGeoNode = FaceCourante.pSurfGeoNode;

    TYAcousticSurface* pSurface = NULL;
    if (pSurfaceGeoNode) { pSurface = dynamic_cast<TYAcousticSurface*>(pSurfaceGeoNode->getElement()); }

    OSegment3D segInter;

    // Calcul de l'intersection de la face avec le plan si necessaire
    if (pSurface && FaceCourante.isInfra)
    {
        OPlan planRayon(matrixinv * pt1, matrixinv * pt2, matrixinv * pt3);
        if (pSurface->intersectsPlan(planRayon, segInter))
        {
            Intersect.bIntersect[indice] = true;
            Intersect.segInter[indice] = segInter * pSurfaceGeoNode->getMatrix();
            bRes = true;
        }
    }
    else
    {
        OPlan planRayon(pt1, pt2, pt3);
        if (pSurface->intersectsPlan(planRayon, segInter))
        {
            Intersect.bIntersect[indice] = true;
            Intersect.segInter[indice] = segInter;
            bRes = true;
        }
    }

    return  bRes;
}

TYAcousticSurface* TYFaceSelector::getActiveSurface(const TYAcousticSurfaceGeoNode* pSurfaceGeoNode, const TYSourcePonctuelle* pSrc) const
{
    if (!pSurfaceGeoNode || !pSrc) { return NULL; }
    TYAcousticSurface* pSurface = dynamic_cast<TYAcousticSurface*>(pSurfaceGeoNode->getElement());

    TYElement* pParentSurface = NULL;
    if (pSurface)
    {
        pParentSurface = pSurface->getParent();
    }
    else
    {
        return pSurface;
    }


    TYElement* pParentSrc = pSrc->getParent();

    TYAcousticVolume* pVolParentSurface = NULL;
    TYAcousticVolume* pVolParentSrc = NULL;

    // On ajoute un parametre de sortie de boucle i qui permet si on ne trouve pas de parent
    // de ne pas rentrer dans une boucle infinie. La limite est fixee a 12 car a priori,
    // aucun element ne peut avoir plus de 12 parents. Si on ne trouve pas le parent de la
    // surface on retourne la surface, si on ne trouve pas le parent de la source, on
    // retourne NULL.

    short i = 0;

    while (pParentSurface && !(pVolParentSurface = dynamic_cast<TYAcousticVolume*>(pParentSurface)) && (i++ < 12))
    {
        pParentSurface = pParentSurface->getParent();
    }

    if (!pParentSurface)
    {
        return pSurface;
    }

    i = 0;

    while (pParentSrc && !(pVolParentSrc = dynamic_cast<TYAcousticVolume*>(pParentSrc)) && (i++ < 12))
    {
        pParentSrc = pParentSrc->getParent();
    }

    if (!pParentSrc)
    {
        return pSurface;
    }

    if (pVolParentSrc != NULL)
        if (pVolParentSrc == pVolParentSurface)
        {
            pSurface = NULL;
        }

    return pSurface;
}
