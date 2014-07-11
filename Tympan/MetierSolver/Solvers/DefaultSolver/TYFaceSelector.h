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

#include "Tympan/models/business/TYFaceSelectorInterface.h"
#include "TYSolverDefines.h"

class TYSolver;

class TYFaceSelector : public TYFaceSelectorInterface
{
public:
    TYFaceSelector(TYSolver& solver);
    virtual ~TYFaceSelector();

    virtual void selectFaces(TYSIntersection* tabIntersect, const TYSourcePonctuelleGeoNode* pSrcGeoNode, const OSegment3D& rayon);

protected :
    // Reference sur le solver
    TYSolver& _solver;

private  :
    bool testIntersect2D(const TYStructSurfIntersect& SI, TYSIntersection& I, TYSPlan* plan, const OSegment3D& segTest, const double& cosA, const double& sinA, const short& indice, const short& XYZ);
    bool buildPlans(TYSPlan* plan, const OSegment3D& rayon);
    bool CalculSegmentCoupe(const TYStructSurfIntersect& FaceCourante, TYSIntersection& Intersect, OPoint3D& pt1, OPoint3D& pt2, OPoint3D& pt3, const int& indice) const;
    TYAcousticSurface* getActiveSurface(const TYAcousticSurfaceGeoNode* pSurfaceGeoNode, const TYSourcePonctuelle* pSrc) const;
};

#endif // __TYFACESELECTOR__
