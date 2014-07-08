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
 */

#include "Tympan/MetierSolver/Solvers/OThreadPool.h"
#include "Tympan/models/business/TYTrajet.h"
#include "TYSolver.h"
#include "TYAcousticModel.h"
#include "TYAcousticPathFinder.h"
#include "Tympan/models/business/TYCalcul.h"
#include "TYFaceSelector.h"
#include "TYTask.h"
#include "Tympan/models/business/TYPreferenceManager.h"

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
    if (!buildCalcStruct(site, calcul))
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
        _pool->push(new TYTask(*this, tabTrajets[i], i + 1));
    }


    if (!_pool->end())
    {
        return false;
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

bool TYSolver::buildCalcStruct(const TYSiteNode& site, TYCalcul& calcul)
{
    bool cancel = false;

    // On nettoie le tableau des faces
    // XXX This should finally boil down to _tabPolygon.clear().
    if (_tabPolygon)
    {
        for (size_t i = 0; i < _tabPolygonSize; ++i)
            if (_tabPolygon[i].pSurfGeoNode)
            {
                _tabPolygon[i].pSurfGeoNode->decRef();
            }
        delete [] _tabPolygon;
        _tabPolygonSize = 0;
    }

    // On recupere les faces
    TYTabAcousticSurfaceGeoNode tabFaces;
    unsigned int nbFacesInfra = 0;
    std::vector<bool> estUnIndexDeFaceEcran; // XXX Should probably be a deque
    /* This call does implement quite a lot of functionalities :
     *
     * - it basically fills the `tabFaces` with all faces from the site
     * - but it does some filtering out
     *    - optionnaly of the faces belonging to screens depending on 1st argument
     * - and it actually fill an array of GeoNodes to Faces, such that the transfrom
     *   is from global to face's local, regardless of the intermediary transforms
     *   in the Site.
     * - it also fill the estUnIndexDeFaceEcran to indicate whether the corresponding
     *   face belongs to a screen in the site
     */
    assert(calcul.getUseEcran() && "The useEcran option is obsolete and should always be true before being removed.");
    site.getListFaces(tabFaces, nbFacesInfra, estUnIndexDeFaceEcran);

    // Reservation de l'espace pour les tableaux
    _tabPolygonSize = static_cast<uint32>(tabFaces.size());

    _tabPolygon = new TYStructSurfIntersect[_tabPolygonSize];

    unsigned int i, j;

    OMatrix mat;

    // 1. Construction du tableau pour les infrastructures

    // Faces d'infrastructure
    TYAcousticPolygon* pPoly = NULL;
    TYAcousticRectangle* pRect = NULL;
    TYAcousticCircle* pCircle = NULL;
    TYAcousticSemiCircle* pSemiCircle = NULL;

    //TYStructSurfIntersect SI;
    for (i = 0 ; i < nbFacesInfra ; i++)
    {
        // Creation d'une structure de type TYStructSurfIntersect
        _tabPolygon[i].pSurfGeoNode = tabFaces[i]._pObj;

        // Incrementation manuel du compteur de reference
        // Necessaire pour la non destruction du pointeur
        _tabPolygon[i].pSurfGeoNode->incRef();

        _tabPolygon[i].matInv = tabFaces[i]->getMatrix().getInvert();
        _tabPolygon[i].isEcran = estUnIndexDeFaceEcran[i];
        _tabPolygon[i].isInfra = true;

        // XXX This should be a virtual method call not this unmaintable and
        // inefficient testing !
        // Recuperation des information du polygon
        if (pPoly = dynamic_cast<TYAcousticPolygon*>(tabFaces[i]->getElement()))
        {
            _tabPolygon[i].tabPoint = pPoly->getOContour();
        }
        else if (pRect = dynamic_cast<TYAcousticRectangle*>(tabFaces[i]->getElement()))
        {
            _tabPolygon[i].tabPoint = pRect->getOContour();
        }
        else if (pCircle = dynamic_cast<TYAcousticCircle*>(tabFaces[i]->getElement()))
        {
            _tabPolygon[i].tabPoint = pCircle->getOContour();
        }
        else if (pSemiCircle = dynamic_cast<TYAcousticSemiCircle*>(tabFaces[i]->getElement()))
        {
            _tabPolygon[i].tabPoint = pSemiCircle->getOContour();
        }
        else
        {
            // XXX This is a dangerous silent error/warning sink.
            continue; // Unidentified face type
        }

        // Trying to transform coordinates to the global reference frame
        // and closing the polygons at the same time (CHECKME: why ?)
        if (_tabPolygon[i].tabPoint.size() != 0)
        {
            for (j = 0; j < _tabPolygon[i].tabPoint.size(); j++)
            {
                _tabPolygon[i].tabPoint[j] =  tabFaces[i]->getMatrix() * _tabPolygon[i].tabPoint[j];
            }

            _tabPolygon[i].tabPoint.push_back(_tabPolygon[i].tabPoint[0]); // On rajoute le premier point car ce sont des objets fermes
        }
    }
    // Meme chose pour toutes les faces de la topographie
    for (i = nbFacesInfra ; i < tabFaces.size() ; i++)
    {
        // Creation d'une structure de type TYStructSurfIntersect
        _tabPolygon[i].pSurfGeoNode = tabFaces[i]._pObj;

        // Incrementation manuel du compteur de reference
        // Necessaire pour la non destruction du pointeur
        _tabPolygon[i].pSurfGeoNode->incRef();

        _tabPolygon[i].matInv = OMatrix();
        _tabPolygon[i].isEcran = false;
        _tabPolygon[i].isInfra = false;

        // Recuperation du tableau des points formant le contour
        TYAcousticPolygon* pPoly = dynamic_cast<TYAcousticPolygon*>(tabFaces[i]->getElement());

        if (pPoly)
        {
            _tabPolygon[i].tabPoint = pPoly->getOContour();
        }

        _tabPolygon[i].tabPoint.push_back(_tabPolygon[i].tabPoint[0]); // On rajoute le premier point car ce sont des objets fermes
    }
    return true;
}
