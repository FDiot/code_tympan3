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
	purge();
}

void TYSolver::purge()
{
    if (_tabPolygon)
    {
        for (size_t i = 0; i < _tabPolygonSize; i++)
        {
            _tabPolygon[i].pSurfGeoNode->release();
        }

        delete [] _tabPolygon;
		_tabPolygon = NULL;
    }

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

bool TYSolver::solve(const TYSiteNode& site, TYCalcul& calcul)
{
	// Ajout de la récupération des TYRay par le calcul
	bool bSaveRay = false;

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
    _acousticPathFinder->init(site, calcul);

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

	// Sauvegarde des rayons
	if (calcul.getSaveRay())
	{
		for (unsigned int i=0; i<tabTrajets.size(); i++)
		{
			for (unsigned j=0; j<tabTrajets[i].getTabRays().size(); j++)
			{
				calcul.addRay(new TYRay(*(tabTrajets[i].getTabRays()[j])));
			}
		}
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
    if (_tabPolygon)
    {
        for (size_t i = 0; i < _tabPolygonSize; ++i)
            if (_tabPolygon[i].pSurfGeoNode)
            {
                _tabPolygon[i].pSurfGeoNode->release();
            }
        delete [] _tabPolygon;
        _tabPolygonSize = 0;
    }

    // On recupere les faces
    TYTabAcousticSurfaceGeoNode tabFaces;
    unsigned int nbFacesInfra = 0;
    std::vector<bool> estUnIndexDeFaceEcran;
    site.getListFaces(calcul.getUseEcran(), tabFaces, nbFacesInfra, estUnIndexDeFaceEcran);

    // Reservation de l'espace pour les tableaux
    _tabPolygonSize = static_cast<uint32>(tabFaces.size());

    _tabPolygon = new TYStructSurfIntersect[_tabPolygonSize];

#if TY_USE_IHM
    TYProgressManager::setMessage("Initialisation du solveur");
    TYProgressManager::set(_tabPolygonSize);
#endif // TY_USE_IHM

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
        _tabPolygon[i].pSurfGeoNode = tabFaces[i];

        // Incrementation manuel du compteur de reference
        // Necessaire pour la non destruction du pointeur
        _tabPolygon[i].pSurfGeoNode->addRef();

        _tabPolygon[i].matInv = tabFaces[i]->getMatrix().getInvert();
        _tabPolygon[i].isEcran = estUnIndexDeFaceEcran[i];
        _tabPolygon[i].isInfra = true;

        // Recuperation des information du polygon
        if (pPoly = TYAcousticPolygon::safeDownCast(tabFaces[i]->getElement()))
        {
            _tabPolygon[i].tabPoint = pPoly->getContour();
        }
        else if (pRect = TYAcousticRectangle::safeDownCast(tabFaces[i]->getElement()))
        {
            _tabPolygon[i].tabPoint = pRect->getContour();
        }
        else if (pCircle = TYAcousticCircle::safeDownCast(tabFaces[i]->getElement()))
        {
            _tabPolygon[i].tabPoint = pCircle->getContour();
        }
        else if (pSemiCircle = TYAcousticSemiCircle::safeDownCast(tabFaces[i]->getElement()))
        {
            _tabPolygon[i].tabPoint = pSemiCircle->getContour();
        }
        else
        {
            continue; // type de face non identifiee
        }

        // Tentative de conversion des points dans le repere global
        if (_tabPolygon[i].tabPoint.size() != 0)
        {
            for (j = 0; j < _tabPolygon[i].tabPoint.size(); j++)
            {
                _tabPolygon[i].tabPoint[j] =  tabFaces[i]->getMatrix() * _tabPolygon[i].tabPoint[j];
            }

            _tabPolygon[i].tabPoint.push_back(_tabPolygon[i].tabPoint[0]); // On rajoute le premier point car ce sont des objets fermes
        }

#if TY_USE_IHM
        TYProgressManager::step(cancel);
        if (cancel)
        {
            TYProgressManager::stepToEnd();
            return false;
        }
#endif // TY_USE_IHM
    }


    // Meme chose pour toutes les faces de la topographie
    for (i = nbFacesInfra ; i < tabFaces.size() ; i++)
    {
        // Creation d'une structure de type TYStructSurfIntersect
        _tabPolygon[i].pSurfGeoNode = tabFaces[i];

        // Incrementation manuel du compteur de reference
        // Necessaire pour la non destruction du pointeur
        _tabPolygon[i].pSurfGeoNode->addRef();

        _tabPolygon[i].matInv = OMatrix();
        _tabPolygon[i].isEcran = false;
        _tabPolygon[i].isInfra = false;

        // Recuperation du tableau des points formant le contour
        TYAcousticPolygon* pPoly = TYAcousticPolygon::safeDownCast(tabFaces[i]->getElement());

        if (pPoly)
        {
            _tabPolygon[i].tabPoint = pPoly->getContour();
        }

        _tabPolygon[i].tabPoint.push_back(_tabPolygon[i].tabPoint[0]); // On rajoute le premier point car ce sont des objets fermes
#if TY_USE_IHM
        TYProgressManager::step(cancel);
        if (cancel)
        {
            TYProgressManager::stepToEnd();
            return false;
        }
#endif // TY_USE_IHM
    }

#if TY_USE_IHM
    TYProgressManager::stepToEnd();
#endif // TY_USE_IHM
    return true;
}
