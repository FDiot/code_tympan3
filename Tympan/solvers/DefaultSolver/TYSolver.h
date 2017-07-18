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
#include "Tympan/solvers/DefaultSolver/TYTrajet.h"
#include "Tympan/models/solver/acoustic_problem_model.hpp"
#include "Tympan/models/solver/acoustic_result_model.hpp"
#include "Tympan/core/interfaces.h"
#include "Tympan/solvers/DefaultSolver/TYSolverDefines.h"
#include "Tympan/models/solver/config.h"

class OThreadPool;
class TYAcousticModel;
class TYAcousticPathFinder;
class TYFaceSelector;
class Scene;

/**
 * \brief Default solver
 */
class TYSolver : public SolverInterface
{
public:
	TYSolver(); //!< Constructor
    virtual ~TYSolver(); //!< Destructor

    /**
     * \brief Launch the resolution and get the results
     * \param aproblem Acoustic problem
     * \param aresult Results
     * \param configuration Solver configuration
     * \return Always true
     */
    virtual bool solve(const tympan::AcousticProblemModel& aproblem,
                       tympan::AcousticResultModel& aresult,
                       tympan::LPSolverConfiguration configuration);

    const std::vector<TYStructSurfIntersect>& getTabPolygon() const { return _tabPolygon; }	//!< Get the array of polygons

    TYFaceSelector* getFaceSelector() { return _faceSelector.get();}	//!< Get the face selector
    TYAcousticPathFinder* getAcousticPathFinder() { return _acousticPathFinder.get(); } //!< Get the acoustic path finder

    TYAcousticModel* getAcousticModel() { return _acousticModel.get(); } //!< Get acoustic model

    const Scene* getScene() const { return _scene.get(); }     //!< Get the Scene

protected:
    std::unique_ptr<TYFaceSelector> make_face_selector();		//!< TYFaceSelector builder
    std::unique_ptr<TYAcousticPathFinder> make_path_finder();	//!< TYAcousticPathFinder builder
    std::unique_ptr<TYAcousticModel> make_acoustic_model();		//!< TYAcousticModel builder

    std::unique_ptr<TYFaceSelector> _faceSelector;				//!< Pointer to the TYFaceSelector
    std::unique_ptr<TYAcousticPathFinder> _acousticPathFinder;	//!< Pointer to the TYAcousticPathFinder
    std::unique_ptr<TYAcousticModel> _acousticModel;			//!< Pointer to the TYAcousticModel

private:
    bool buildCalcStruct(const tympan::AcousticProblemModel& aproblem);

    /*!
    * \fn bool appendTriangleToScene()
    * \brief Convertion des triangles Tympan en primitives utilisables par ray tracer.
    * \return Renvoie vrai si l'ensemble des primitives a bien pu etre importe dans la simulation.
    */
    bool appendTriangleToScene();

    /**
     * \fn size_t buildTrajects(tympan::AcousticProblemModel& aproblem)
     * \brief construit le tableau des trajets et la matrice resultat en supprimant les points trop proches d'une source
     */
   size_t buildTrajects(tympan::AcousticProblemModel& aproblem);

    // XXX This pointer is actually used like a C array :
    // TODO replace with a std::deque or similar container.
    std::vector<TYStructSurfIntersect> _tabPolygon; //!< Vector of TYStructSurfIntersect

	std::vector<TYTrajet> _tabTrajets; //!< Vector of TYTrajet

    OThreadPool* _pool;

private:
    std::unique_ptr<Scene> _scene; //!< Pointer to the Scene
};

#endif // __TY_SOLVER__
