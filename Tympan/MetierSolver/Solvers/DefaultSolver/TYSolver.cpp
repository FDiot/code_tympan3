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

#include "Tympan/MetierSolver/ToolsMetier/OThreadPool.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYTrajet.h"
#include "TYSolver.h"
#include "TYAcousticModel.h"
#include "TYAcousticPathFinder.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYCalcul.h"
#include "TYFaceSelector.h"
#include "Tympan/Tools/TYProgressManager.h"
#include "TYTask.h"
#include "Tympan/MetierSolver/DataManagerCore/TYPreferenceManager.h"

TYSolver::TYSolver()
    : _faceSelector(0), _acousticPathFinder(0), _acousticModel(0), _tabPolygon(0), _tabPolygonSize(0), _pool(0)
{
    // Creation du face selector
    createFaceSelector();

    // Creation du path finder
    createAcousticPathFinder();

    // Creation du acoustic model
    createAcousticModel();

    // Creation de la collection de thread
    _pool = NULL;
}

TYSolver::~TYSolver()
{
    // Release des pointeurs sur les TYAcousticSurfaceGeoNode
    if (_tabPolygon)
        for (size_t i = 0; i < _tabPolygonSize; ++i)
        {
            _tabPolygon[i].pSurfGeoNode->decRef();
        }

    if (_faceSelector)
    {
        delete _faceSelector;
    }
    if (_acousticPathFinder)
    {
        delete _acousticPathFinder;
    }
    if (_acousticModel)
    {
        delete _acousticModel;
    }
    if (_tabPolygon)
    {
        delete [] _tabPolygon;
    }
    if (_pool)
    {
        delete _pool;
    }
}

void TYSolver::purge()
{
    if (_tabPolygon)
    {
        for (size_t i = 0; i < _tabPolygonSize; i++)
        {
            _tabPolygon[i].pSurfGeoNode->decRef();
        }
    }

    if (_tabPolygon)
    {
        delete [] _tabPolygon;
    }

    _tabPolygon = NULL;

    if (_faceSelector)
    {
        delete _faceSelector;
    }
    _faceSelector = NULL;


    if (_acousticPathFinder)
    {
        delete _acousticPathFinder;
    }
    _acousticPathFinder = NULL;


    if (_acousticModel)
    {
        delete _acousticModel;
    }
    _acousticModel = NULL;

    if (_pool)
    {
        delete _pool;
    }
    _pool = NULL;
}

bool TYSolver::solve(const TYSiteNode& site, TYCalcul& calcul,
        const tympan::AcousticProblemModel& aproblem,
        tympan::AcousticResultModel& aresult)
{
    // Creation de la collection de thread
    const unsigned int nbThread = calcul.getNbTHread();
    _pool = new OThreadPool(nbThread);

    // On recupere les trajets
    std::vector<TYTrajet>& tabTrajets = calcul.getTabTrajet();

    // Creation du face selector
    if (!_faceSelector) { createFaceSelector(); }

    // Creation du path finder
    if (!_acousticPathFinder) { createAcousticPathFinder(); }

    // Creation du acoustic model
    if (!_acousticModel) { createAcousticModel(); }

    // On calcule la structure
    if (!buildCalcStruct(site, calcul, aproblem))
    {
        return false;
    }

    // Initialisation du path finder
    _acousticPathFinder->init(calcul);

    // Initialisation du acoustic model
    _acousticModel->init(site, calcul);

    // Nombre total de calcul
    unsigned int count = static_cast<uint32>(tabTrajets.size());

    // On reset la thread pool
    _pool->begin(count);

    // Lancement des calculs
    for (unsigned int i = 0; i < count; ++i)
    {
        _pool->push(new TYTask(*this, aproblem.nodes(), aproblem.triangles(), aproblem.materials(), tabTrajets[i], i + 1));
    }


    if (!_pool->end())
    {
        return false;
    }

    tympan::SpectrumMatrix& matrix = aresult.get_data();
    matrix.resize(aproblem.nreceptors(), aproblem.nsources());
    for (unsigned int i=0; i<tabTrajets.size(); i++)
    {
        tympan::source_idx sidx = tabTrajets[i].asrc_idx;
        tympan::receptor_idx ridx = tabTrajets[i].arcpt_idx;
        matrix(ridx, sidx) = tabTrajets[i].getSpectre();
    }

    return true;
}

void TYSolver::createFaceSelector()
{
    _faceSelector = new TYFaceSelector(*this);
}

void TYSolver::createAcousticPathFinder()
{
    _acousticPathFinder = new TYAcousticPathFinder(*this);
}

void TYSolver::createAcousticModel()
{
    _acousticModel = new TYAcousticModel(*this);
}

bool TYSolver::buildCalcStruct(const TYSiteNode& site, TYCalcul& calcul, const tympan::AcousticProblemModel& aproblem)
{
    bool cancel = false;

    const tympan::nodes_pool_t& nodes = aproblem.nodes(); 
    const tympan::triangle_pool_t& triangles = aproblem.triangles();

    _tabPolygon = new TYStructSurfIntersect[triangles.size()];

    for (unsigned int i=0; i<triangles.size(); i++)
    {
        OPoint3D p1 = nodes[triangles[i].n[0]];
        OPoint3D p2 = nodes[triangles[i].n[1]];
        OPoint3D p3 = nodes[triangles[i].n[2]];

        _tabPolygon[i].tabPoint.push_back(p1);
        _tabPolygon[i].tabPoint.push_back(p2);
        _tabPolygon[i].tabPoint.push_back(p3);
        _tabPolygon[i].material = const_cast<tympan::AcousticMaterialBase*>(&aproblem.material(i));
    }

    return true;
}
