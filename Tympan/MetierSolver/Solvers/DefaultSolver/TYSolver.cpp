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
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/Scene.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/Triangle.h"

TYSolver::TYSolver()
{
    // Creation du face selector
    _faceSelector = make_face_selector();

    // Creation du path finder
    _acousticPathFinder = make_path_finder();

    // Creation du acoustic model
    _acousticModel = make_acoustic_model();

    _tabPolygon.clear();

    _scene = std::unique_ptr<Scene>( new Scene() ); 

    // Creation de la collection de thread
    _pool = NULL;
}

TYSolver::~TYSolver()
{
    if (_pool)
    {
        delete _pool;
    }
    _pool = NULL;}


bool TYSolver::solve(const TYSiteNode& site, TYCalcul& calcul,
        const tympan::AcousticProblemModel& aproblem,
        tympan::AcousticResultModel& aresult)
{
    // Creation de la collection de thread
    const unsigned int nbThread = calcul.getNbTHread();
    _pool = new OThreadPool(nbThread);

    // build trajects and return count of
    size_t count = buildTrajects( const_cast<tympan::AcousticProblemModel&>(aproblem) );

    // Creation du face selector
    if (!_faceSelector) { _faceSelector = make_face_selector(); }

    // Creation du path finder
    if (!_acousticPathFinder) { _acousticPathFinder = make_path_finder(); }

    // Creation du acoustic model
    if (!_acousticModel) { _acousticModel = make_acoustic_model(); }

    // On calcule la structure
    if (buildCalcStruct(site, calcul, aproblem))
    {
        appendTriangleToScene();
    }
    else
    {
        return false;
    }

    // Initialisation du path finder
    _acousticPathFinder->init(calcul);

    // Initialisation du acoustic model
    _acousticModel->init(site, calcul);

    // On reset la thread pool
    _pool->begin(count);

    // Lancement des calculs
    for (unsigned int i = 0; i < count; ++i)
    {
        _pool->push(new TYTask(*this, aproblem.nodes(), aproblem.triangles(), aproblem.materials(), _tabTrajets[i], i + 1));
    }

    if (!_pool->end())
    {
        return false;
    }

    tympan::SpectrumMatrix& matrix = aresult.get_data();
    matrix.resize(aproblem.nreceptors(), aproblem.nsources());
    for (unsigned int i=0; i<_tabTrajets.size(); i++)
    {
        tympan::source_idx sidx = _tabTrajets[i].asrc_idx;
        tympan::receptor_idx ridx = _tabTrajets[i].arcpt_idx;
        matrix(ridx, sidx) = _tabTrajets[i].getSpectre();
    }


//  --- BEGIN ---
//	Code below is only for compatibility reasons and must be suppressed when result matrix will be implemented correctly in TYResultat
 
	calcul.getTabTrajet().clear();
	calcul.getTabTrajet() = _tabTrajets;

//  ---  END  ---
    return true;
}

std::unique_ptr<TYFaceSelector> TYSolver::make_face_selector()
{
    return std::unique_ptr<TYFaceSelector>( new TYFaceSelector(*this) );
}

std::unique_ptr<TYAcousticPathFinder> TYSolver::make_path_finder()
{
    return std::unique_ptr<TYAcousticPathFinder>( new TYAcousticPathFinder(*this) );
}

std::unique_ptr<TYAcousticModel> TYSolver::make_acoustic_model()
{
    return std::unique_ptr<TYAcousticModel>( new TYAcousticModel(*this) );
}

bool TYSolver::buildCalcStruct(const TYSiteNode& site, TYCalcul& calcul, const tympan::AcousticProblemModel& aproblem)
{
    bool cancel = false;

    const tympan::nodes_pool_t& nodes = aproblem.nodes(); 
    const tympan::triangle_pool_t& triangles = aproblem.triangles();

    _tabPolygon.reserve( triangles.size() );

    OPoint3D pts[3];
    for (unsigned int i=0; i<triangles.size(); i++)
    {
        pts[0] = nodes[triangles[i].n[0]];
        pts[1] = nodes[triangles[i].n[1]];
        pts[2] = nodes[triangles[i].n[2]];

        TYStructSurfIntersect SI;
        OGeometrie::computeNormal(pts, 3, SI.normal);

        SI.tabPoint.push_back(pts[0]);
        SI.tabPoint.push_back(pts[1]);
        SI.tabPoint.push_back(pts[2]);
        SI.material = const_cast<tympan::AcousticMaterialBase*>(&aproblem.material(i));

        _tabPolygon.push_back(SI);
    }

    return true;
}

bool TYSolver::appendTriangleToScene()
{
    if ( _tabPolygon.empty() )
    {
        return false;
    }

    Material *m = new Material(); // Only for compatibility, may be suppressed;

    vec3 pos;

    for (unsigned int i = 0; i < _tabPolygon.size(); i++)
    {
        //Recuperation et convertion de la normale de la surface
        double coordNormal[3];
        _tabPolygon[i].normal.getCoords(coordNormal);
        vec3 normalFace = vec3(coordNormal[0], coordNormal[1], coordNormal[2]);

        unsigned int a, b, c;
        double coord[3];

        _tabPolygon[i].tabPoint[0].getCoords(coord);
        pos = OPoint3Dtovec3(coord);
        _scene->addVertex(pos, a);

        _tabPolygon[i].tabPoint[1].getCoords(coord);
        pos = OPoint3Dtovec3(coord);
        _scene->addVertex(pos, b);

        _tabPolygon[i].tabPoint[2].getCoords(coord);
        pos = OPoint3Dtovec3(coord);
        _scene->addVertex(pos, c);

        Triangle* face;
        if ( dynamic_cast<tympan::AcousticGroundMaterial*>(_tabPolygon[i].material) )
        {
            // Set last parameter true means triangle is part of the ground
            face = (Triangle*)_scene->addTriangle(a, b, c, m, true);
        }
        else
        {
            face = (Triangle*)_scene->addTriangle(a, b, c, m);
        }
    }

    _scene->finish(); // Build accelerating structure

    return true;
}

size_t TYSolver::buildTrajects(tympan::AcousticProblemModel& aproblem)
{
    _tabTrajets.reserve( aproblem.nsources() * aproblem.nreceptors() );

    for(unsigned int i=0; i< aproblem.nsources(); i++)
    {
        for (unsigned int j = 0; j<aproblem.nreceptors(); j++)
        {
            double distance = aproblem.source(i).position.distFrom(aproblem.receptor(j).position);
            TYTrajet trajet(aproblem.source(i), aproblem.receptor(j));
            trajet.setDistance(distance);
            trajet.asrc_idx = i;
            trajet.arcpt_idx = j;
            _tabTrajets.push_back(trajet);
        }
    }

	return _tabTrajets.size();
}
