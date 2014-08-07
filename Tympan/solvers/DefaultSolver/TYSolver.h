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
#include <memory>
#include "Tympan/core/interfaces.h"
#include "Tympan/models/business/TYSolverInterface.h"
#include "Tympan/models/business/TYTrajet.h"
#include "Tympan/models/solver/acoustic_problem_model.hpp"
#include "Tympan/models/solver/acoustic_result_model.hpp"
#include "Tympan/solvers/DefaultSolver/TYSolverDefines.h"

class OThreadPool;
class TYAcousticModel;
class TYAcousticPathFinder;
class TYFaceSelector;
class Scene;

class TYSolver : public SolverInterface
{
public:
    TYSolver();
    virtual ~TYSolver();

    virtual bool solve(const TYSiteNode& site, TYCalcul& calcul,
            const tympan::AcousticProblemModel& aproblem,
            tympan::AcousticResultModel& aresult);

    const std::vector<TYStructSurfIntersect>& getTabPolygon() const { return _tabPolygon; }

    TYFaceSelector* getFaceSelector() { return _faceSelector._Myptr; }
    TYAcousticPathFinder* getAcousticPathFinder() { return _acousticPathFinder._Myptr; }
    TYAcousticModel* getAcousticModel() { return _acousticModel._Myptr; }

    const Scene* getScene() const { return _scene._Myptr; }

protected:
    std::unique_ptr<TYFaceSelector> make_face_selector();
    std::unique_ptr<TYAcousticPathFinder> make_path_finder();
    std::unique_ptr<TYAcousticModel> make_acoustic_model();

    std::unique_ptr<TYFaceSelector> _faceSelector;
    std::unique_ptr<TYAcousticPathFinder> _acousticPathFinder;
    std::unique_ptr<TYAcousticModel> _acousticModel;

private:
    bool buildCalcStruct(const TYSiteNode& site, TYCalcul& calcul, const tympan::AcousticProblemModel& aproblem);

    /*!
    * \fn bool appendTriangleToScene()
    * \brief Convertion des triangles Tympan en primitives utilisables par ray tracer.
    * \return Renvoie vrai si l'ensemble des primitives a bien pu etre importe dans la simulation.
    */
    bool appendTriangleToScene();

    /**
     * \fn size_t buildValidTrajects(tympan::AcousticProblemModel& aproblem)
     * \brief construit le tableau des trajets et la matrice resultat en supprimant les points trop proches d'une source
     */
   size_t buildTrajects(tympan::AcousticProblemModel& aproblem);

    // XXX This pointer is actually used like a C array :
    // TODO replace with a std::deque or similar container.
    std::vector<TYStructSurfIntersect> _tabPolygon;

	std::vector<TYTrajet> _tabTrajets; 

    OThreadPool* _pool;

private:
    std::unique_ptr<Scene> _scene;
};

#endif // __TY_SOLVER__
