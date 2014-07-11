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

#ifndef __TY_SOLVER__
#define __TY_SOLVER__

#include <vector>

#include "TYSolverDefines.h"
#include "Tympan/MetierSolver/DataManagerCore/TYSolverInterface.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYTrajet.h"

#include "Tympan/MetierSolver/SolverDataModel/acoustic_problem_model.hpp"
#include "Tympan/MetierSolver/SolverDataModel/acoustic_result_model.hpp"

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

    virtual bool solve(const TYSiteNode& site, TYCalcul& calcul,
            const tympan::AcousticProblemModel& aproblem,
            tympan::AcousticResultModel& aresult);

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
    bool buildCalcStruct(const TYSiteNode& site, TYCalcul& calcul, const tympan::AcousticProblemModel& aproblem);

    /**
     * \fn size_t buildValidTrajects(tympan::AcousticProblemModel& aproblem)
     * \brief construit le tableau des trajets et la matrice resultat en supprimant les points trop proches d'une source
     */
   size_t buildTrajects(tympan::AcousticProblemModel& aproblem);

    // XXX This pointer is actually used like a C array :
    // TODO replace with a std::deque or similar container.
    TYStructSurfIntersect* _tabPolygon;
    size_t _tabPolygonSize;

	std::vector<TYTrajet> _tabTrajets; 

    OThreadPool* _pool;
};

#endif // __TY_SOLVER__
