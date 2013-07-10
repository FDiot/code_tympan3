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

#ifndef __TY_SOLVER__
#define __TY_SOLVER__

#include "TYSolverDefines.h"
#include "Tympan/MetierSolver/DataManagerCore/TYSolverInterface.h"

class OThreadPool;

class TYAcousticModel;
class TYAcousticPathFinder;
class TYFaceSelector;

class TYSolver : public TYSolverInterface
{
public:
    TYSolver();
    virtual ~TYSolver();

    virtual void purge();

    virtual bool solve(const TYSiteNode& site, TYCalcul& calcul);

    const TYStructSurfIntersect* getTabPolygon() const { return _tabPolygon; }
    const size_t getTabPolygonSize() const { return _tabPolygonSize; }

    TYFaceSelector* getFaceSelector() { return _faceSelector; }
    TYAcousticPathFinder* getAcousticPathFinder() { return _acousticPathFinder; }
    TYAcousticModel* getAcousticModel() { return _acousticModel; }

protected:
    virtual void createFaceSelector();
    virtual void createAcousticPathFinder();
    virtual void createAcousticModel();

    TYFaceSelector* _faceSelector;
    TYAcousticPathFinder* _acousticPathFinder;
    TYAcousticModel* _acousticModel;

private:
    bool buildCalcStruct(const TYSiteNode& site, TYCalcul& calcul);

    TYStructSurfIntersect* _tabPolygon;
    size_t _tabPolygonSize;

    OThreadPool* _pool;
};

#endif // __TY_SOLVER__
