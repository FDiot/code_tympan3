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

#ifndef __TYACOUSTICPATHFINDER__
#define __TYACOUSTICPATHFINDER__

#include "TYSolverDefines.h"

class TYSolver;

class TYAcousticPathFinder
{
public:
    TYAcousticPathFinder(TYSolver& solver);
    virtual ~TYAcousticPathFinder();

    virtual void computePath(const std::deque<TYSIntersection>& tabIntersect, const TYTrajet& trajet, TabPoint3D& ptsTop, TabPoint3D& ptsLeft, TabPoint3D& ptsRight);
    virtual void init();

private :
    // Reference sur le solver
    TYSolver& _solver;

    bool computeParcoursLateraux(const std::deque<TYSIntersection>& tabIntersect, const OSegment3D& rayon, const bool& dessus, TabPoint3D& ptsLeft, TabPoint3D& ptsRight) const;

    // Faut il calculer les trajets horizontaux
    bool _bCalcTrajetHorizontaux;
};

#endif // __TYACOUSTICPATHFINDER__
