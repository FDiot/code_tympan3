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
 
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticVolume.h"
#include "TYSolver.h"
#include "TYSolverDefines.h"
#include "TYFaceSelector.h"

TYFaceSelector::TYFaceSelector(TYSolver& solver)
    : _solver(solver)
{
}

TYFaceSelector::~TYFaceSelector()
{
}

void TYFaceSelector::selectFaces(std::vector<TYSIntersection*>& tabIntersect, const TYSourcePonctuelleGeoNode* pSrcGeoNode, const OSegment3D& rayon)
{
    // Construction des plans de coupe
    TYSPlan plan[2];
    buildPlans(plan, rayon);

    // Recuperation de la source ponctuelle
    TYSourcePonctuelle* pSrc = static_cast<TYSourcePonctuelle*>(pSrcGeoNode->getElement());

	// Récupération du tableau des polygones
	const TYStructSurfIntersect* pTabSurf = _solver.getTabPolygon();

    // Recuperation de la taille  du tableau de faces
    size_t nbFaces = _solver.getTabPolygonSize();

	TYSIntersection* intersect = NULL;

	bool bIntersectV, bIntersectH;

	for (unsigned int i = 0; i < nbFaces; i++)
	{
		bIntersectV = false;
		bIntersectH = false;
		intersect = new TYSIntersection();

		// Initialisation des intersections
		getSurfaceAndVolume(pTabSurf[i].pSurfGeoNode, intersect);
		intersect->isInfra = pTabSurf[i].isInfra;
		intersect->isEcran = pTabSurf[i].isEcran;
		intersect->noIntersect = false;
		intersect->bIntersect[0] = false;
		intersect->bIntersect[1] = false;

		// Neutralisation des faces qui ne peuvent etre prises en compte (face portant la source)
		if (pTabSurf[i].isInfra && !(pSrc->isA("TYUserSourcePonctuelle")) && isActiveSurface(intersect->pVolume, pSrc)) 
		{
			intersect->noIntersect = true;
		}

		bIntersectV = CalculSegmentCoupe(pTabSurf[i], intersect, plan[0].pt1, plan[0].pt2, plan[0].pt3, 0); // Plan vertical

		bIntersectH = CalculSegmentCoupe(pTabSurf[i], intersect, plan[1].pt1, plan[1].pt2, plan[1].pt3, 1); // Plan horizontal
		
		if (bIntersectV || bIntersectH) 
		{
			tabIntersect.push_back(intersect);
		}
		else // Dans ce cas, on a créé une "intersection" pour rien
		{
			delete intersect;
			intersect = NULL;
		}
	}
}

bool TYFaceSelector::CalculSegmentCoupe(const TYStructSurfIntersect& FaceCourante, TYSIntersection* intersect, OPoint3D& pt1, OPoint3D& pt2, OPoint3D& pt3, const int& indice) const
{
    bool bRes = false;
    const OMatrix& matrixinv = FaceCourante.matInv;
    TYAcousticSurfaceGeoNode* pSurfaceGeoNode = FaceCourante.pSurfGeoNode;
    TYAcousticSurface* pSurface = static_cast<TYAcousticSurface*>(pSurfaceGeoNode->getElement());
    OSegment3D segInter;

    // Calcul de l'intersection de la face avec le plan si necessaire
    if (FaceCourante.isInfra)
    {
        OPlan planRayon(matrixinv * pt1, matrixinv * pt2, matrixinv * pt3);
        if (pSurface->intersectsPlan(planRayon, segInter))
        {
            intersect->bIntersect[indice] = true;
            intersect->segInter[indice] = segInter * pSurfaceGeoNode->getMatrix();
            bRes = true;
        }
    }
    else
    {
        OPlan planRayon(pt1, pt2, pt3);
        if (pSurface->intersectsPlan(planRayon, segInter))
        {
            intersect->bIntersect[indice] = true;
            intersect->segInter[indice] = segInter;
            bRes = true;
        }
    }

    return  bRes;
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



bool TYFaceSelector::isActiveSurface(const TYAcousticVolume* pParentSurface, const TYSourcePonctuelle* pSrc) const
{
	TYElement* pParentSrc = findParentVolume( const_cast<TYSourcePonctuelle*>(pSrc) );
	if (!pParentSrc) { return false; }

    if (pParentSrc == pParentSurface) { return true; }

    return false;
}

TYElement* TYFaceSelector::findParentVolume(TYElement* pElement) const
{
    assert(pElement);
	TYElement* pParent = NULL;

	do 
    {
        pParent = pElement->getParent();
    }
    while ( pParent && !(static_cast<TYAcousticVolume*>(pParent)) );

	return pParent;
}

void TYFaceSelector::getSurfaceAndVolume(const TYAcousticSurfaceGeoNode* pSurfaceGeoNode, TYSIntersection* intersect)
{
	intersect->pSurface = NULL;
	intersect->pVolume = NULL;
	intersect->pSurface = static_cast<TYAcousticSurface*>(pSurfaceGeoNode->getElement());
	intersect->pVolume = static_cast<TYAcousticVolume*>(findParentVolume(intersect->pSurface));
}

