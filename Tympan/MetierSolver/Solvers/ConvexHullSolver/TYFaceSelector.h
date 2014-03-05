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
 *
 *
 *
 */

#ifndef __TYFACESELECTOR__
#define __TYFACESELECTOR__

#include "Tympan/MetierSolver/DataManagerCore/TYFaceSelectorInterface.h"
#include "TYSolverDefines.h"

class TYSolver;

class TYFaceSelector : public TYFaceSelectorInterface
{
public:
    TYFaceSelector(TYSolver& solver);
    virtual ~TYFaceSelector();

	// build list of intersection segments
	virtual void selectFaces(std::vector<TYSIntersection*>& tabIntersect, const TYSourcePonctuelleGeoNode* pSrcGeoNode, const OSegment3D& rayon);

private  :
	// Build an horizontal and a vertical plan from rayon
	bool buildPlans(TYSPlan* plan, const OSegment3D& rayon);

	// Compute cut segment of a face by a plane
	bool CalculSegmentCoupe(const TYStructSurfIntersect& FaceCourante, TYSIntersection* intersect, OPoint3D& pt1, OPoint3D& pt2, OPoint3D& pt3, const int& indice) const;

	// Test if acousticSuface (pSurfaceGeoNode) own the ponctual source (pSrc)
	bool isActiveSurface(const TYAcousticVolume* pParentSurface, const TYSourcePonctuelle* pSrc) const;

	// Fin parent volume of an object (face or source)
	TYElement* findParentVolume(TYElement* pElement) const; 

	// Renvoie la surface et son volume parent (si présent)
	void getSurfaceAndVolume(const TYAcousticSurfaceGeoNode* pSurfaceGeoNode, TYSIntersection* intersect);

protected :
    // Reference sur le solver
    TYSolver& _solver;


};

#endif // __TYFACESELECTOR__
