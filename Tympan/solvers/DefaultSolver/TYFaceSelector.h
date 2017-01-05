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


#ifndef __TYFACESELECTOR__
#define __TYFACESELECTOR__

#include "Tympan/models/solver/TYFaceSelectorInterface.h"
#include "TYSolverDefines.h"

class TYSolver;
class TYTrajet;

/**
 * \brief Building class of the faces list
 */
class TYFaceSelector : public TYFaceSelectorInterface
{
public:
    TYFaceSelector(TYSolver& solver);
    virtual ~TYFaceSelector();

    /**
     * \brief Build the array of intersections
     * \param tabIntersect Array of intersections
     * \param rayon Ray path
     */
    virtual void selectFaces(std::deque<TYSIntersection>& tabIntersect, const TYTrajet& rayon);

protected :
    TYSolver& _solver; /// Reference to the solver

private  :
    bool buildPlans(TYSPlan* plan, const OSegment3D& rayon);
    bool CalculSegmentCoupe(const TYStructSurfIntersect& FaceCourante, TYSIntersection& Intersect, OPoint3D& pt1, OPoint3D& pt2, OPoint3D& pt3, const int& indice) const;
    void reorder_intersect(std::deque<TYSIntersection>& tabIntersect); // put infrastructure faces on top
};

#endif // __TYFACESELECTOR__
