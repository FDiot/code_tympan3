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
 * Copyright (c) 2011 EDF. All Rights Reserved.
 * All Rights Reserved. No part of this software and its documentation may
 * be used, copied, modified, distributed and transmitted, in any form or by
 * any means, without the prior written permission of EDF.
 * EDF-R&D Departement AMA - 1, avenue du general de Gaulle, 92141 CLAMART
 */

/**
 * \file TYANIME3DSolver.h
 * \brief Le solver associe a la methode ANIME3D
 * \author Projet_Tympan
 * \date 04 avril 2011
 */


#include "Tympan/Tools/TYProgressManager.h"

#include "Tympan/MetierSolver/AcousticRaytracer/global.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/UniformSphericSampler.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/Triangulate.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/Triangle.h"

#include "Tympan/MetierSolver/AnalyticRayTracer/R3.h"
#include "Tympan/MetierSolver/AnalyticRayTracer/RayCourb.h"
#include "Tympan/MetierSolver/AnalyticRayTracer/Transfo.h"
#include "Tympan/MetierSolver/AnalyticRayTracer/Lancer.h"

#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYChemin.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYTrajet.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "TYANIME3DSolver.h"
#include "TYANIME3DRayTracerParameters.h"
#include "TYANIME3DAcousticModel.h"
#include "TYANIME3DAcousticPathFinder.h"
#include "TYANIME3DFaceSelector.h"

typedef std::vector<TYTrajet> TabTrajet;

TYANIME3DSolver::TYANIME3DSolver(): _acousticPathFinder(0), _acousticModel(0)
{
    _tabPolygon = NULL;

    // Appel a l'outil de generation des chemins acoustiques : lancer de rayons
    createAcousticPathFinder();

    // Creation du modele acoustique
    createAcousticModel();
}

TYANIME3DSolver::~TYANIME3DSolver()
{
    if (_acousticModel)
    {
        delete _acousticModel;
    }

    if (_acousticPathFinder)
    {
        delete _acousticPathFinder;
    }
}

void TYANIME3DSolver::createAcousticModel()
{
    _acousticModel = new TYANIME3DAcousticModel();
}

void TYANIME3DSolver::createAcousticPathFinder()
{
    _acousticPathFinder = new TYANIME3DAcousticPathFinder();
}

void TYANIME3DSolver::createFaceSelector()
{
    _faceSelector = new TYANIME3DFaceSelector();
}

bool TYANIME3DSolver::solve(const TYSiteNode& site, TYCalcul& calcul)
{
    // Configuration du lancer de rayon geometriques (au debut pour initialiser les valeurs globales

    // Nettoyage de l'objet _rayTracing si il a ete utilise precedement
	_rayTracing.clean();

    // Ajout de la methode acoustique a la _rayTracing
    TYANIME3DRayTracerParameters* solver = new TYANIME3DRayTracerParameters();
    solver->initGlobalValues();

    _rayTracing.setSolver(solver);

    // Creation du lancer de rayons courbes
    lancer.Init();

    //////////////////////////////////////
    //Importation des elements de la scene
    //////////////////////////////////////

    //Importation de la geometrie et des materiaux associes

    //Construction d'une structure intermediaire stockant l'ensemble de la geometrie
    if (!buildCalcStruct(site, calcul))
    {
        //fic_out << "Erreur lors du chargement de la scene Tympan" << endl;
        return false;
    }

    ////////////////////////////////////////////
    //Construction de la scene pour le rayTracing
    ////////////////////////////////////////////

	// Sources et recepteurs du lancer de rayons
    vector<vec3> sources;
    vector<vec3> recepteurs;
    unsigned int sens = getTabsSAndR(site, calcul, sources, recepteurs);

    // Definition des objets de lancer de rayons courbes
    if (globalUseMeteo)
    {
        // Recuperation des sources et des recepteurs
        appendSourceToAnalyticRayTracer(site, calcul, lancer.shot.sources);
        appendRecepteurToAnalyticRayTracer(calcul, lancer.shot.recepteurs);

        // Parametrage du lancer
        lancer.setDMax(globalAnalyticDMax);
        lancer.setTMax(globalAnalyticTMax);
        lancer.setTimeStep(globalAnalyticH);
        lancer.setNbRay(globalAnalyticNbRay);
        lancer.Meteo.setGradC(globalAnalyticGradC);
        lancer.Meteo.setGradV(globalAnalyticGradV);
        lancer.Meteo.setC0(globalAnalyticC0);
        lancer.setMethode(globalAnalyticTypeTransfo);
		lancer.shot.launchType = 1;									// Indique que l'on tire les rayons sur un plan horizontal
        lancer.shot.initialAnglePhi = globalAnalyticAnglePhi;		// Angle de tir vertical (phi) des rayons

        // Choix de la source
        lancer.source = 0;
        lancer.setSrcForMapping(lancer.source);

        lancer.recepteurs = lancer.shot.recepteurs;

        // Transformation de la geometrie
        lancer.Transformer();

        // Affectation de la geometrie transformee au lancer de rayon geometriques
        //  appendTriangleToRayTracerScene();

        // Positionnement des sources et des recepteurs
        transformSEtR(sources, recepteurs);
    }

    //Construction de la scene pour le rayTracing avec le tableau de polygones.
    //Conversion des polygones en triangles
    triangulateConcavePolygon();

    //Ajout des triangles a l'objet Scene de _rayTracing
    appendTriangleToScene();

    //Importation des recepteurs ponctuels actifs
    appendRecepteurToSimulation(recepteurs);

	//Importation des sources ponctuelles actives
    appendSourceToSimulation(sources);

	//Une fois la scene convertie, on peut la post-traiter (ajouter de l'information : arretes de diffractions par ex)
    _rayTracing.getSolver()->postTreatmentScene(_rayTracing.getScene(), _rayTracing.getSources(), _rayTracing.getRecepteurs());

    _rayTracing.getScene()->finish();

    ////////////////////////////////////
    // Propagation des rayons
    ////////////////////////////////////

    _rayTracing.setEngine(); //PARALLELDEFAULT
    _rayTracing.launchSimulation();          //Traitement monothread
    //_rayTracing.computeThreadTreatment();  //Traitement multi-thread (necessite Qt 4.4 ou sup.)

    ////////////////////////////////////
    // Exportation des resultats
    ////////////////////////////////////

    saveAndOverSampleRay(site, calcul, sens, globalOverSampleD);

    if (globalUseMeteo && globalOverSampleD)
    {
        courbeRayon(calcul);
    }

    //  appendRayToTYCalcul(site, calcul, sens);  // pourkoi commente ??

//    if (globalRestitModifiedGeom) { restitModifiedAlti(site); }

    // Exportation des donnees des rayons dans un fichier texte pour comparaison
    _tabRay = calcul.getAllRays();

	////////////////////////////////////////////////////////////
    // Calculs acoustiques sur les rayons via la methode ANIME3D
    ////////////////////////////////////////////////////////////

	_acousticModel = new TYANIME3DAcousticModel(calcul, site, _rayTracing.getSolver()->getValidRays(), _tabPolygon);

	//_acousticModel->init(calcul, site, _rayTracing.getSolver()->getValidRays(), _tabPolygon);  // methode a supprimer

    // calcul de la matrice de pression totale pour chaque couple (S,R)
    OTab2DSpectreComplex tabSpectre = _acousticModel->ComputeAcousticModel(calcul, site);

    /*
    // Nombre total de calculs
    unsigned int count = static_cast<uint32>(tabTrajets.size());
    // On reset la thread pool si threade
    _pool->begin(count);

    // Lancement des calculs si threade
    for(unsigned int i=0; i<count; ++i)
        _pool->push(new TYTask(*this, tabTrajets[i], i+1));
    */

    TabTrajet& trajets = calcul.getTabTrajet();  // recuperation du tableau de trajets

	trajets.clear(); // nettoyage des trajets

    TYTabPointCalculGeoNode    tabRecepteurs;
    TYTabSourcePonctuelleGeoNode tabSources;

    TYMapElementTabSources& mapElementSources = calcul.getResultat()->getMapEmetteurSrcs();
    site.getInfrastructure()->getAllSrcs(&calcul, mapElementSources);

    calcul.getAllSources(mapElementSources, tabSources); // recuperation des sources
    calcul.getAllRecepteurs(tabRecepteurs); // recuperation des recepteurs

	OSpectre sLP; // puissance de la source et spectre de pression pour chaque couple (S,R)
	TYTrajet traj;

	for(int i=0; i<tabSources.size(); i++) // boucle sur les sources
	{
		for(int j=0; j<tabRecepteurs.size(); j++) // boucle sur les recepteurs
	   {
			tabSpectre[i][j].setEtat(SPECTRE_ETAT_LIN);
			sLP = tabSpectre[i][j];
			sLP.setType(SPECTRE_TYPE_LP);

			tabSpectre[i][j] = sLP.toDB();  // conversion du tableau resultat en dB

			tabRecepteurs[j]->addRef();  //pour pas que cet objet ne soit detruit

			traj.setSourcePonctuelle(tabSources[i]);
			traj.setPointCalcul(tabRecepteurs[j]);
			traj.setSpectre(tabSpectre[i][j]);
			trajets.push_back(traj);
		}
	}
    return true;
}

bool TYANIME3DSolver ::buildCalcStruct(const TYSiteNode& site, const TYCalcul& calcul)
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
    std::vector<std::pair<int, int> > indexBatiment;
    std::vector<int> etages;

    site.getListFacesWithoutFloor(true, tabFaces, nbFacesInfra, estUnIndexDeFaceEcran, indexBatiment, etages);

    // Reservation de l'espace pour les tableaux
    _tabPolygonSize = static_cast<uint32>(tabFaces.size());

    _tabPolygon = new TYStructSurfIntersect[_tabPolygonSize];

#if TY_USE_IHM
    TYProgressManager::setMessage("Initialisation du solveur et chargement de la scene.");
    TYProgressManager::set(_tabPolygonSize);
#endif // TY_USE_IHM

    unsigned int i, j;

    OMatrix mat;

    //====================================================
    // 1. Construction du tableau pour les infrastructures
    //====================================================

    // Types de faces d'infrastructure
    TYAcousticPolygon* pPoly = NULL;
    TYAcousticRectangle* pRect = NULL;
    TYAcousticCircle* pCircle = NULL;
    TYAcousticSemiCircle* pSemiCircle = NULL;

    //logs.open("error_infrac.txt",ios::out | ios::trunc);

    for (i = 0 ; i < nbFacesInfra ; i++)
    {
        // Creation d'une structure de type TYStructSurfIntersect
        _tabPolygon[i].pSurfGeoNode = tabFaces[i];

        // Incrementation manuel du compteur de reference
        // Necessaire pour la non destruction du pointeur
        _tabPolygon[i].pSurfGeoNode->addRef();

        // Ajout de la matrice inverse
        _tabPolygon[i].matInv = tabFaces[i]->getMatrix().getInvert();

        // Ajout d'une face d'ecran
        _tabPolygon[i].isEcran = estUnIndexDeFaceEcran[i];

        // Ajout du booleen d'intrastructure et d'ecran
        _tabPolygon[i].isInfra = true;

        // Ajout du booleen de l'indice du batiment
        _tabPolygon[i].idBuilding = indexBatiment.at(i).second;

        // Ajout du booleen de l'indice de face du batiment
        _tabPolygon[i].idFace = indexBatiment.at(i).first;

        // Ajout du booleen de l'indice d'etage du batiment
        _tabPolygon[i].idEtage = etages.at(i);

        // Ajout du spectre d'attenuation
        _tabPolygon[i].spectreAbso = new double[18];

        // Recuperation des informations de la suface acoustique
        // si c'est un polygone
        if (pPoly = TYAcousticPolygon::safeDownCast(tabFaces[i]->getElement()))
        {
            _tabPolygon[i].tabPoint = pPoly->getContour();
            _tabPolygon[i].normal = pPoly->normal();
            pPoly->getMateriau()->getSpectreAbso().getRangeValueReal(_tabPolygon[i].spectreAbso, 18, 8);
        }
        // si c'est un rectangle
        else if (pRect = TYAcousticRectangle::safeDownCast(tabFaces[i]->getElement()))
        {
            _tabPolygon[i].tabPoint = pRect->getContour();
            _tabPolygon[i].normal = pRect->normal();
            pRect->getMateriau()->getSpectreAbso().getRangeValueReal(_tabPolygon[i].spectreAbso, 18, 8);
        }
        // si c'est un cercle
        else if (pCircle = TYAcousticCircle::safeDownCast(tabFaces[i]->getElement()))
        {
            _tabPolygon[i].tabPoint = pCircle->getContour();
            _tabPolygon[i].normal = pCircle->normal();
            pCircle->getMateriau()->getSpectreAbso().getRangeValueReal(_tabPolygon[i].spectreAbso, 18, 8);
        }
        // si c'est un 1/2 cercle
        else if (pSemiCircle = TYAcousticSemiCircle::safeDownCast(tabFaces[i]->getElement()))
        {
            _tabPolygon[i].tabPoint = pSemiCircle->getContour();
            _tabPolygon[i].normal = pSemiCircle->normal();
            pSemiCircle->getMateriau()->getSpectreAbso().getRangeValueReal(_tabPolygon[i].spectreAbso, 18, 8);
        }
        else
        {
            // Verification avec un type de surface non identifie
            continue; // type de face non identifiee
        }
        // Conversion des points dans le repere global
        for (j = 0; j < _tabPolygon[i].tabPoint.size(); j++)
        {
            _tabPolygon[i].tabPoint[j] =  tabFaces[i]->getMatrix() * _tabPolygon[i].tabPoint[j];
        }

        // Ajout de la normale
        _tabPolygon[i].normal = tabFaces[i]->getMatrix().multNormal(_tabPolygon[i].normal);

        // Ajout de G =0
        _tabPolygon[i].G = 0.;

        // A la fin du tableau de pts des contours on rajoute le premier point car ce sont des objets fermes
        if (!_tabPolygon[i].tabPoint.empty())
        {
            _tabPolygon[i].tabPoint.push_back(_tabPolygon[i].tabPoint[0]);
        }
        else
            //logs << "Erreur, pas de contours pour ce polygone." << endl;

#if TY_USE_IHM
            TYProgressManager::step(cancel);
        if (cancel)
        {
            TYProgressManager::stepToEnd();
            return false;
        }
#endif // TY_USE_IHM
    }

    //===================================================================
    // 2. Construction du tableau pour toutes les faces de la topographie
    //===================================================================

    for (i = nbFacesInfra ; i < tabFaces.size() ; i++)
    {
        // Creation d'une structure de type TYStructSurfIntersect
        _tabPolygon[i].pSurfGeoNode = tabFaces[i];

        // Incrementation manuel du compteur de reference
        // Necessaire pour la non destruction du pointeur
        _tabPolygon[i].pSurfGeoNode->addRef();

        _tabPolygon[i].matInv = OMatrix();

        // pas d'ecran
        _tabPolygon[i].isEcran = false;

        // pas d'infrastructures
        _tabPolygon[i].isInfra = false;

        // Recuperation du tableau des points formant le contour
        TYAcousticPolygon* pPoly = TYAcousticPolygon::safeDownCast(tabFaces[i]->getElement());
        OPoint3D pt(0., 0., 0.);
        if (pPoly)
        {
            _tabPolygon[i].tabPoint = pPoly->getContour();
            for (unsigned int j = 0; j < _tabPolygon[i].tabPoint.size(); j++)
            {
                double coords[3];
                _tabPolygon[i].tabPoint[j].getCoords(coords);
                pt.set(pt._x +  coords[0], pt._y +  coords[1], pt._z +  coords[2]);
            }
            _tabPolygon[i].normal = pPoly->normal();
        }
        pt.set(pt._x / (double)_tabPolygon[i].tabPoint.size(), pt._y / (double)_tabPolygon[i].tabPoint.size(), pt._z / (double)_tabPolygon[i].tabPoint.size());

		TYSol* sol = site.getTopographie()->terrainAt(pt)->getSol();
		TYAtmosphere& atmos = *calcul.getAtmosphere();

        // indices de batiment, de face et d'etage =-1
        _tabPolygon[i].idBuilding = -1;
        _tabPolygon[i].idFace = indexBatiment.at(i).first;
        _tabPolygon[i].idEtage = -1;

        // calcul et ajout de G j'ai pas vu
        _tabPolygon[i].G = min(pow(300.0 / sol->getResistivite(), 0.57), 1.0);  //Resistivite du sol capable de calculer le G

        //Spectre d'attenuation pour le sol ( calcule dans la methode acoustique car depend du rayon incident
		_tabPolygon[i].spectreAbso = NULL;

        if (!_tabPolygon[i].tabPoint.empty())
        {
            _tabPolygon[i].tabPoint.push_back(_tabPolygon[i].tabPoint[0]);    // On rajoute le premier point car ce sont des objets fermes
        }
        else
            //  logs << "Erreur de recuperation d'un polygone de topographie avec " << _tabPolygon[i].tabPoint.size() << " points." << endl;

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

bool TYANIME3DSolver::triangulateConcavePolygon()
{
    if (_tabPolygon == NULL || _tabPolygonSize <= 0)
    {
        return false;
    }

    for (unsigned int i = 0; i < _tabPolygonSize; i++)
    {
        //On triangule toutes les faces qui ne sont pas des triangles
        //(On ignore le dernier point qui est le premier)
        if (((int)_tabPolygon[i].tabPoint.size()) - 1 > 3)
        {
            double normalCoord[3];
            _tabPolygon[i].normal.normalize();
            _tabPolygon[i].normal.getCoords(normalCoord);

            OMatrix P;
            OMatrix invP;

            //Matrice de passage pour que l'ensemble des points soient 2D (z = 0)
            double matrix[4][4];
            double origineCoord[3];
            _tabPolygon[i].tabPoint[0].getCoords(origineCoord);

            if (normalCoord[2] > 0.99 || normalCoord[2] < -0.99) //Face horizontale, on a pas besoin de faire de rotation, juste la translation.
            {
                matrix[0][0] = 1; matrix[0][1] = 0; matrix[0][2] = 0;   matrix[0][3] = origineCoord[0];
                matrix[1][0] = 0; matrix[1][1] = 1; matrix[1][2] = 0;   matrix[1][3] = origineCoord[1];
                matrix[2][0] = 0; matrix[2][1] = 0; matrix[2][2] = 1;   matrix[2][3] = origineCoord[2];
                matrix[3][0] = 0; matrix[3][1] = 0; matrix[3][2] = 0;   matrix[3][3] = 1;

            }
            else //Matrice de passage pour une face non horizontale
            {
                vec3 X = vec3(normalCoord[1], -normalCoord[0], 0);
                vec3 Y = vec3(-normalCoord[0] * normalCoord[2], -normalCoord[2] * normalCoord[1], normalCoord[0] * normalCoord[0] + normalCoord[1] * normalCoord[1]);
                vec3 Z = vec3(normalCoord[0], normalCoord[1], normalCoord[2]);
                X.normalize();
                Y.normalize();
                Z.normalize();

                matrix[0][0] = X.x; matrix[0][1] = Y.x; matrix[0][2] = Z.x; matrix[0][3] = origineCoord[0];
                matrix[1][0] = X.y; matrix[1][1] = Y.y; matrix[1][2] = Z.y; matrix[1][3] = origineCoord[1];
                matrix[2][0] = X.z; matrix[2][1] = Y.z; matrix[2][2] = Z.z; matrix[2][3] = origineCoord[2];
                matrix[3][0] = 0;   matrix[3][1] = 0;   matrix[3][2] = 0;   matrix[3][3] = 1;
            }

            //Definition des matrices de passage
            P = OMatrix(matrix);
            invP = P.getInvert();

            //Mise en place des coordonnees 2D pour la triangulation
            Vector2dVector input, output;
            for (unsigned int j = 0; j < _tabPolygon[i].tabPoint.size() - 1; j++)
            {
                double coord[3];
                OCoord3D newCoord = invP * _tabPolygon[i].tabPoint[j];
                newCoord.getCoords(coord);

                input.push_back(vec2(newCoord._x, newCoord._y));
            }
            // Lancement de la procedure de triangulation
            Triangulate::Process(input, output);


            //Fabrication des OTriangles
            for (unsigned int j = 0; j < output.size(); j++)
            {
                OTriangle t = OTriangle(j, j + 1, j + 2);
                j += 2;
                _tabPolygon[i].triangles.append(t);
            }

            //Fabrication des coordonnees vers le repere reel.
            for (unsigned int j = 0; j < output.size(); j++)
            {
                //Conversion vers le repere reel
                OCoord3D coordLocal = OCoord3D(output.at(j).x, output.at(j).y, 0.f);
                OCoord3D coordGlobal = P * coordLocal;
                double coords[3];
                coordGlobal.getCoords(coords);
                _tabPolygon[i].realVertex.append(OPoint3D(coords[0], coords[1], coords[2]));
            }
        }
    }
    return true;
}

bool TYANIME3DSolver::appendTriangleToScene()
{
    if (_tabPolygon == NULL || _tabPolygonSize <= 0)
    {
        return false;
    }

    Scene* scene = _rayTracing.getScene();
    Material* m1 = new Material();
    Material* m2 = new Material();
    m2->setName(std::string("MyMaterial"));
    //  m1->r = 255;m1->g = 125; m1->b = 0;
    m1->isNatural = false;
    //  m2->r = 0;m2->g = 255; m2->b = 0;
    m2->isNatural = false;

    vec3 pos;

    for (unsigned int i = 0; i < _tabPolygonSize; i++)
    {
        //Recuperation et convertion de la normale de la surface
        double coordNormal[3];
        _tabPolygon[i].normal.getCoords(coordNormal);
        vec3 normalFace = vec3(coordNormal[0], coordNormal[1], coordNormal[2]);
        if (_tabPolygon[i].tabPoint.size() == 4) //Creation d'un triangle a partir des points de la surface
        {
            unsigned int a, b, c;


            double coord[3];
            _tabPolygon[i].tabPoint[0].getCoords(coord);
            pos = modifGeom(vec3(coord[0], coord[1], coord[2]));
            scene->addVertex(pos, a);

            _tabPolygon[i].tabPoint[1].getCoords(coord);
            pos = modifGeom(vec3(coord[0], coord[1], coord[2]));
            scene->addVertex(pos, b);

            _tabPolygon[i].tabPoint[2].getCoords(coord);
            pos = modifGeom(vec3(coord[0], coord[1], coord[2]));
            scene->addVertex(pos, c);

            Triangle* face;
            if (_tabPolygon[i].isEcran || _tabPolygon[i].isInfra)
            {
                //logs << "Ajout d'un triangle non naturel." << endl;
                //face->setInfos(101,0,false);  //Correspond a un element non naturel
                //face->setInfos(101,0,false,18,_tabPolygon[i].spectreAbsoMat,_tabPolygon[i].G);
                //face->setExterieur(false);
                face = (Triangle*)scene->addTriangle(a, b, c, m1);
                ss << "Ajout d'un triangle non naturel." << std::endl;
            }
            else
            {
                //logs << "Ajout d'un triangle naturel." << endl;
                //face->setInfos(100,0,false);  //Correspond a un element naturel
                //face->setExterieur(true);
                face = (Triangle*)scene->addTriangle(a, b, c, m2, true);
            }
            face->setBuildingId(_tabPolygon[i].idBuilding);
            face->setFaceId(_tabPolygon[i].idFace);
            face->setEtageId(_tabPolygon[i].idEtage);

            if (face->getNormal().dot(normalFace) < -0.001) //Les normales sont de sens contraire
            {
                face->setNormal(face->getNormal() * (-1.));
            }
        }
        else if (_tabPolygon[i].tabPoint.size() > 4)
        {
            OPoint3D coord;
            for (unsigned int j = 0; j < _tabPolygon[i].triangles.size(); j++)
            {
                unsigned int a, b, c;

                //scene->addVertex( vec3(coord._x,coord._y,coord._z),a);
                //coord = _tabPolygon[i].realVertex.at(_tabPolygon[i].triangles.at(j)._p2);
                //scene->addVertex( vec3(coord._x,coord._y,coord._z),b);
                //coord = _tabPolygon[i].realVertex.at(_tabPolygon[i].triangles.at(j)._p3);
                //scene->addVertex( vec3(coord._x,coord._y,coord._z),c);

                coord = _tabPolygon[i].realVertex.at(_tabPolygon[i].triangles.at(j)._p1);
                pos = modifGeom(vec3(coord._x, coord._y, coord._z));
                scene->addVertex(pos, a);

                coord = _tabPolygon[i].realVertex.at(_tabPolygon[i].triangles.at(j)._p2);
                pos = modifGeom(vec3(coord._x, coord._y, coord._z));
                scene->addVertex(pos, b);

                coord = _tabPolygon[i].realVertex.at(_tabPolygon[i].triangles.at(j)._p3);
                pos = modifGeom(vec3(coord._x, coord._y, coord._z));
                scene->addVertex(pos, c);

                Triangle* face;
                if (_tabPolygon[i].isEcran || _tabPolygon[i].isInfra)
                {
                    //logs << "Ajout d'un triangle non naturel." << endl;
                    //face->setInfos(101,0,false);  //Correspond a un element non naturel
                    //face->setInfos(101,0,false,18,_tabPolygon[i].spectreAbsoMat,_tabPolygon[i].G);
                    //face->setExterieur(false);
                    face = (Triangle*)scene->addTriangle(a, b, c, m1);
                    ss << "Ajout d'un triangle non naturel." << std::endl;
                }
                else
                {
                    //logs << "Ajout d'un triangle naturel." << endl;
                    //face->setInfos(100,0,false);  //Correspond a un element naturel
                    //face->setExterieur(true);
                    face = (Triangle*)scene->addTriangle(a, b, c, m2, true);
                }
                face->setBuildingId(_tabPolygon[i].idBuilding);
                face->setFaceId(_tabPolygon[i].idFace);
                face->setEtageId(_tabPolygon[i].idEtage);

                if (face->getNormal().dot(normalFace) < -0.001) //Les normales sont de sens contraire
                {
                    face->setNormal(face->getNormal() * (-1.));
                }
            }
        }
    }
    return true;
}

vec3 TYANIME3DSolver::modifGeom(const vec3& point)
{
    if (!globalUseMeteo) { return point; } // Aucune action si pas meteo

    R3 mPoint(point.x, point.y, point.z);
    mPoint = lancer.fonction_h(mPoint);

    return vec3(mPoint.x, mPoint.y, mPoint.z);
}

void TYANIME3DSolver::restitModifiedAlti(const TYSiteNode& site)
{
    // Recuperation de l'altimetrie du site
    const TYTopographie* pTopo = site.getTopographie().getRealPointer();
    TYAltimetrie* pAlti = pTopo->getAltimetrie();

    // Recuperation de la scene de calcul
    vector<vec3>* tabvertex = _rayTracing.getScene()->getVertices();

    // Remplissage du tableau de points
    TYTabPoint tabPts;
    for (unsigned int i = 0; i < tabvertex->size(); i++)
    {
        // A SUPPRIMER (MAIS PAS TOUT DE SUITE - 20120727 -)
        //R3 point = TYPointToR3(TYPoint( tabvertex->at(i).x, tabvertex->at(i).y, tabvertex->at(i).z) );
        //point = lancer.fonction_h_inverse(point);
        //TYPoint p = R3ToTYPoint(point);
        //tabPts.push_back(p);
        // ===========
        tabPts.push_back(TYPoint(tabvertex->at(i).x, tabvertex->at(i).y, tabvertex->at(i).z));

    }

    // Recalcul de la triangulation de l'altimetrie
    pAlti->compute(tabPts, 1.E-5);
}

unsigned int TYANIME3DSolver::getTabsSAndR(const TYSiteNode& site, TYCalcul& calcul, vector<vec3>& sources, vector<vec3>& recepteurs)
{
    unsigned int sens = globalRayTracingOrder;

    // Recuperation de la position des sources du projet
    vector<vec3> srcs;
    //Recuperation de la liste des sources poncutelles
    TYMapElementTabSources& mapElementSources = calcul.getResultat()->getMapEmetteurSrcs();
    site.getInfrastructure()->getAllSrcs(&calcul, mapElementSources);

    TYTabSourcePonctuelleGeoNode MSources; // Creation d'une collection des sources metiers
    calcul.getAllSources(mapElementSources, MSources);

    //Conversion des sources Tympan en source lancer de rayons
    TYSourcePonctuelle* sourceP = NULL;
    for (unsigned int i = 0; i < MSources.size(); i++)
    {
        sourceP = TYSourcePonctuelle::safeDownCast(MSources.at(i)->getElement());
        if (sourceP)
        {
            double coord[3];
            OMatrix matrice = MSources.at(i)->getMatrix();
            OPoint3D globalPos = matrice * (*sourceP->getPos());

            globalPos.getCoords(coord);
            vec3 pos = vec3(coord[0], coord[1], coord[2]);
            srcs.push_back(pos);
        }
    }

    // Recuperation de la position des recepteurs du projet
    vector<vec3> rcpts;

    //Recuperation des points de controle
    TYTabPointCalculGeoNode tabRecepteurs;
    calcul.getAllRecepteurs(tabRecepteurs);

    //Conversion du recepteur Tympan en recepteur lancer de rayons
    for (unsigned int i = 0; i < tabRecepteurs.size(); i++)
    {
        TYPointCalcul* pc = TYPointCalcul::safeDownCast(tabRecepteurs.at(i)->getElement());
        //if(tabRecepteurs.at(i)->getState() == TYPointCalcul::Actif)

        OPoint3D globalPos = tabRecepteurs.at(i)->getMatrix() * (*pc);
        double coord[3];

        globalPos.getCoords(coord);
        vec3 pos = vec3(coord[0], coord[1], coord[2]);

        rcpts.push_back(pos);
    }

    // Choix du traitement S->R ou R->S
    switch (globalRayTracingOrder)
    {
        case 0 :
            sources = srcs;
            recepteurs = rcpts;
            break;
        case 1 :
            sources = rcpts;
            recepteurs = srcs;
            break;
        case 2 : // Choix automatique
            if (rcpts.size() > srcs.size())
            {
                sources = rcpts;
                recepteurs = srcs;
                sens = 1;
            }
            else
            {
                sources = srcs;
                recepteurs = rcpts;
                sens = 0;
            }
        default :
            sources = srcs;
            recepteurs = rcpts;
            break;
    }

    return sens;
}

void TYANIME3DSolver::transformSEtR(vector<vec3>& sources, vector<vec3>& recepteurs)
{
    // Pour toutes les sources
    for (unsigned int i = 0; i < sources.size(); i++)
    {
        sources[i] = modifGeom(sources[i]);
    }

    // Pour tous les recepteurs
    for (unsigned int i = 0; i < recepteurs.size(); i++)
    {
        recepteurs[i] = modifGeom(recepteurs[i]);
    }
}

void TYANIME3DSolver::appendRecepteurToSimulation(vector<vec3>& recepteurs)
{
    //Ajout des recepteurs
    unsigned int idRecepteur = 0;
    //Conversion du recepteur Tympan en recepteur lancer de rayons
    for (unsigned int i = 0; i < recepteurs.size(); i++)
    {
        Recepteur recep;
        recep.setShape(new Sphere(recepteurs[i], globalSizeReceiver));
        recep.setId(idRecepteur);

        _rayTracing.addRecepteur(recep);
        ss << "Ajout d'un recepteur en (" << recepteurs[i].x << "," << recepteurs[i].y << "," << recepteurs[i].z << ")" << endl;
        idRecepteur++;
    }
}

void TYANIME3DSolver::appendRecepteurToSimulation(TYCalcul& calcul)
{
    //Ajout des recepteurs
    unsigned int idRecepteur = 0;

    //Recuperation des points de controle
    //TYTabLPPointControl tabRecepteurs = calcul.getProjet()->getPointsControl();
    TYTabPointCalculGeoNode tabRecepteurs;
    calcul.getAllRecepteurs(tabRecepteurs);

    //Conversion du recepteur Tympan en recepteur lancer de rayons
    for (unsigned int i = 0; i < tabRecepteurs.size(); i++)
    {
        TYPointCalcul* pc = TYPointCalcul::safeDownCast(tabRecepteurs.at(i)->getElement());
        //if(tabRecepteurs.at(i)->getState() == TYPointCalcul::Actif)

        OPoint3D pos = tabRecepteurs.at(i)->getMatrix() * (*pc);
        double coordRecepteur[3];

        pos.getCoords(coordRecepteur);
        //      pc->getCoords(coordRecepteur);
        Recepteur recep;
        recep.setShape(new Sphere(vec3(coordRecepteur[0], coordRecepteur[1], coordRecepteur[2]), globalSizeReceiver));
        recep.setId(idRecepteur);

        _rayTracing.addRecepteur(recep);
        ss << "Ajout d'un recepteur en (" << coordRecepteur[0] << "," << coordRecepteur[1] << "," << coordRecepteur[2] << ")" << endl;
        idRecepteur++;
    }

}

void TYANIME3DSolver::appendSourceToSimulation(vector<vec3>& sources)
{
    //Conversion des sources Tympan en sources lancer de rayons
    int idSource = 0;
    for (unsigned int i = 0; i < sources.size(); i++)
    {
        Source source;
        source.setSampler(new UniformSphericSampler());
        source.setPosition(sources[i]);
        source.setInitialRayCount(globalNbRaysPerSource);
        source.setId(idSource);

        ss << "Ajout d'une source en (" << sources[i].x << "," << sources[i].y << "," << sources[i].y << ")" << endl;

        _rayTracing.addSource(source);
        idSource++;
    }
}

void TYANIME3DSolver::appendSourceToSimulation(const TYSiteNode& site, TYCalcul& calcul, unsigned int nbRayons)
{
    //Recuperation de la liste des sources poncutelles
    TYMapElementTabSources& mapElementSources = calcul.getResultat()->getMapEmetteurSrcs();
    site.getInfrastructure()->getAllSrcs(&calcul, mapElementSources);

    TYTabSourcePonctuelleGeoNode sources; // Creation d'une collection des sources
    calcul.getAllSources(mapElementSources, sources);

    //Conversion des sources Tympan en sources lancer de rayons
    int idSource = 0;
    TYSourcePonctuelle* sourceP = NULL;
    for (unsigned int i = 0; i < sources.size(); i++)
    {
        sourceP = TYSourcePonctuelle::safeDownCast(sources.at(i)->getElement());
        if (sourceP)
        {
            double coord[3];
            OMatrix matrice = sources.at(i)->getMatrix();
            TYPoint globalPos = matrice * (*sourceP->getPos());

            globalPos.getCoords(coord);
            vec3 pos = vec3(coord[0], coord[1], coord[2]);
            Source source;
            source.setSampler(new UniformSphericSampler());
            source.setPosition(pos);
            source.setInitialRayCount(nbRayons);

            ss << "Ajout d'une source en (" << coord[0] << "," << coord[1] << "," << coord[2] << ")" << endl;

            _rayTracing.addSource(source);
        }
        idSource++;
    }
}

struct pairComparator
{
    bool operator()(pair<int, int> p1, pair<int, int> p2)
    {
        if (p1.first < p2.first)
        {
            return true;
        }
        if (p1.second < p2.second)
        {
            return true;
        }
        return false;
    };
};

bool TYANIME3DSolver::appendRayToTYCalcul(const TYSiteNode& site, TYCalcul& calcul, const unsigned int& sens)
{
    //On nettoie le tableau de trajet que TYCalcul a pu generer au prealable.
    calcul.getTabTrajet().clear();

    //Generation de la structure qui va faciliter la manipulation des TYTrajet
    //Cle = idSource,idRecepteur
    std::map<pair<int, int>, TYTrajet, pairComparator> tableTrajet;

    //Recuperation de la liste des rayons valides pour la _rayTracing.
    std::vector<Ray*>* rays = _rayTracing.getSolver()->getValidRays();;
    bool errorType = false;

    //Recuperation de la liste des recepteurs
    //  TYTabLPPointControl tabRecepteurs =  calcul.getProjet()->getPointsControl();
    TYTabPointCalculGeoNode tabRecepteurs;
    calcul.getAllRecepteurs(tabRecepteurs);

    //Recuperation de la liste des sources poncutelles
    TYMapElementTabSources& mapElementSources = calcul.getResultat()->getMapEmetteurSrcs();
    site.getInfrastructure()->getAllSrcs(&calcul, mapElementSources);
    TYTabSourcePonctuelleGeoNode sources; // Creation d'une collection des sources
    calcul.getAllSources(mapElementSources, sources);

    //Conversion des rayons du lancer en rayons metier Tympan
    unsigned int idSource = 0;
    unsigned int idRecep = 0;
    for (unsigned int i = 0; i < rays->size(); i++)
    {
        LPTYRay ray;
        std::vector<TYRayEvent> events;
        Ray* r = rays->at(i);
        if (sens == 1) // On inverse les identifiants de source et de recepteur
        {
            idSource = static_cast<Recepteur*>(r->getRecepteur())->getId();
            idRecep = r->getSource()->getId();
        }
        else
        {
            idRecep = static_cast<Recepteur*>(r->getRecepteur())->getId();
            idSource = r->getSource()->getId();
        }

        //Les identifiants des recepteurs et sources sont construit pour correspondre a l'index des sources et recepteurs dans Tympan.
        if (static_cast<unsigned int>(idRecep) >= tabRecepteurs.size() || static_cast<unsigned int>(idSource) >= sources.size()) { continue; }

        TYPoint posSourceGlobal = computePosGlobalPoint(sources.at(idSource));
        TYSourcePonctuelle* sourceP = TYSourcePonctuelle::safeDownCast(sources.at(idSource)->getElement());
        //      OPoint3D posSourceGlobal = sources.at(idSource)->getMatrix() * (*(sourceP->getPos()));

        TYPoint pointR = computePosGlobalPoint(tabRecepteurs.at(idRecep));
        TYPointCalcul* recepP = TYPointCalcul::safeDownCast(tabRecepteurs.at(idRecep)->getElement());
        //      OPoint3D pointR = tabRecepteurs.at(idRecep)->getMatrix() * (*recepP);

        //Definition des Evenements.
        TYRayEvent e;

        //Ajout de la source
        e.type = TYSOURCE;
        e.pos = TYPoint(posSourceGlobal);
        events.push_back(e);

        if (sens == 1)
        {
            std::vector<QSharedPointer<Event> > tempEvents;

            //Creation des evenements de diffractions et reflexions
            std::vector<QSharedPointer<Event> >::reverse_iterator rit;

            for (rit = rays->at(i)->getEvents()->rbegin(); rit != rays->at(i)->getEvents()->rend(); rit++)
            {
                switch ((*rit)->getType())
                {
                    case SPECULARREFLEXION :
                        e.type = TYREFLEXION;
                        break;
                    case DIFFRACTION:
                        e.type = TYDIFFRACTION;
                        break;
                    default:
                        e.type = TY_NO_TYPE;
                        errorType = true;
                        break;
                }

                e.pos = TYPoint((*rit)->getPosition().x, (*rit)->getPosition().y, (*rit)->getPosition().z);
                events.push_back(e);
            }
        }
        else
        {
            //Creation des evenements de diffractions et reflexions
            for (unsigned int j = 0; j < r->getEvents()->size(); j++)
            {
                Event* ev = dynamic_cast<Event*>(r->getEvents()->at(j).data());
                switch (ev->getType())
                {
                    case SPECULARREFLEXION :
                        e.type = TYREFLEXION;
                        break;
                    case DIFFRACTION:
                        e.type = TYDIFFRACTION;
                        break;
                    default:
                        e.type = TY_NO_TYPE;
                        errorType = true;
                        break;
                }

                e.pos = TYPoint(ev->getPosition().x, ev->getPosition().y, ev->getPosition().z);
                events.push_back(e);
            }
        }

        //Ajout du recepteur
        e.type = TYRECEPTEUR;
        e.pos = TYPoint(pointR);
        events.push_back(e);

        //Construction du rayon et envoie au calcul
        ray = new TYRay(sourceP, recepP, events);
        calcul.addRay(ray);

        std::map<pair<int, int>, TYTrajet, pairComparator>::iterator it;

        it = tableTrajet.find(pair<int, int>(idSource, idRecep));
        if (it != tableTrajet.end())
        {
            //Cas ou un trajet existait deja, on genere un nouveau chemin avec un spectre calcule
            OSpectreComplex spectre;
            spectre.setValue(16., -200.);
            spectre.setValue(20., -200.);
            spectre.setValue(25., -200.);
            spectre.setValue(31.5, -200.);
            spectre.setValue(40., -200.);
            spectre.setValue(50., -200.);
            spectre.setValue(63., -200.);
            spectre.setValue(80., -200.);
            spectre.setValue(100., 10.);
            spectre.setValue(125., 10.);
            spectre.setValue(160., 10.);
            spectre.setValue(200., 10.);
            spectre.setValue(250., 10.);
            spectre.setValue(315., 10.);
            spectre.setValue(400., 10.);
            spectre.setValue(500., 10.);
            spectre.setValue(630., 10.);
            spectre.setValue(800., 10.);
            spectre.setValue(1000., 10.);
            spectre.setValue(1250., 10.);
            spectre.setValue(1600., 10.);
            spectre.setValue(2000., 10.);
            spectre.setValue(2500., 10.);
            spectre.setValue(3150., 10.);
            spectre.setValue(4000., 10.);
            spectre.setValue(5000., 10.);
            spectre.setValue(6300., -200.);
            spectre.setValue(8000., -200.);
            spectre.setValue(10000., -200.);
            spectre.setValue(12500., -200.);
            spectre.setValue(16000., -200.);

            TYChemin chemin;
            chemin.setAttenuation(spectre);
            it->second.addChemin(chemin);
        }
        else
        {
            //Cas ou il n'existe pas encore de trajet entre la source et le recepteur.
            //TYTrajet trajet;
            //trajet.setSourcePonctuelle(sources.at(idSource));
            //trajet.setPointCalcul(tabRecepteurs.at(idRecep));

        }
    }
    if (errorType) return false;

    return true;
}

void TYANIME3DSolver::buildListEvent(const int& sens, Ray* ray, std::vector<TYRayEvent>& tabEvents)
{
    //Definition des Evenements.
    TYRayEvent e;

    if (sens == 1)
    {
        //Creation des evenements de diffractions et reflexions
        std::vector<QSharedPointer<Event> >::reverse_iterator rit;

        for (rit = ray->getEvents()->rbegin(); rit != ray->getEvents()->rend(); rit++)
        {
            switch ((*rit)->getType())
            {
                case SPECULARREFLEXION :
                    e.type = TYREFLEXION;
                    break;
                case DIFFRACTION:
                    e.type = TYDIFFRACTION;
                    break;
                default:
                    e.type = TY_NO_TYPE;
                    break;
            }

            e.pos = TYPoint((*rit)->getPosition().x, (*rit)->getPosition().y, (*rit)->getPosition().z);
            e.angle = (*rit)->getAngle();
            tabEvents.push_back(e);
        }
    }
    else
    {
        //Creation des evenements de diffractions et reflexions
        for (unsigned int j = 0; j < ray->getEvents()->size(); j++)
        {
            Event* ev = dynamic_cast<Event*>(ray->getEvents()->at(j).data());
            switch (ev->getType())
            {
                case SPECULARREFLEXION :
                    e.type = TYREFLEXION;
                    break;
                case DIFFRACTION:
                    e.type = TYDIFFRACTION;
                    break;
                default:
                    e.type = TY_NO_TYPE;
                    break;
            }

            e.pos = TYPoint(ev->getPosition().x, ev->getPosition().y, ev->getPosition().z);
            e.angle = ev->getAngle();
            tabEvents.push_back(e);
        }
    }
}

bool TYANIME3DSolver::setSEtRToTYRay(TYTabSourcePonctuelleGeoNode& sources, TYTabPointCalculGeoNode& tabRecepteurs, const unsigned int sens, Ray* r, TYRay* tyRay)
{
    unsigned int idSource = 0;
    unsigned int idRecep = 0;
    if (sens == 1) // On inverse les identifiants de source et de recepteur
    {
        idSource = static_cast<Recepteur*>(r->getRecepteur())->getId();
        idRecep = r->getSource()->getId();
    }
    else
    {
        idRecep = static_cast<Recepteur*>(r->getRecepteur())->getId();
        idSource = r->getSource()->getId();
    }

    //Les identifiants des recepteurs et sources sont construit pour correspondre a l'index des sources et recepteurs dans Tympan.
    if (static_cast<unsigned int>(idRecep) >= tabRecepteurs.size() || static_cast<unsigned int>(idSource) >= sources.size()) { return false; }

    TYPoint posSourceGlobal = computePosGlobalPoint(sources.at(idSource));
    TYSourcePonctuelle* sourceP = TYSourcePonctuelle::safeDownCast(sources.at(idSource)->getElement());

    TYPoint posReceptGlobal = computePosGlobalPoint(tabRecepteurs.at(idRecep));
    TYPointCalcul* recepP = TYPointCalcul::safeDownCast(tabRecepteurs.at(idRecep)->getElement());

    tyRay->setSource(sourceP, posSourceGlobal);
    tyRay->setRecepteur(recepP, posReceptGlobal);

    //Ajout de la source
    TYRayEvent e;
    e.type = TYSOURCE;
    e.pos = TYPoint(r->source->getPosition().x, r->source->getPosition().y, r->source->getPosition().z);
    e.angle = 0.0;
    tyRay->getEvents().push_back(e);

    e.type = TYRECEPTEUR;
    Recepteur* recep = (static_cast<Recepteur*>(r->getRecepteur()));
    e.pos = TYPoint(recep->getPosition().x, recep->getPosition().y, recep->getPosition().z);
    e.angle = 0.0;
    tyRay->getEvents().push_back(e);

    return true;
}

void TYANIME3DSolver::saveAndOverSampleRay(const TYSiteNode& site, TYCalcul& calcul, const unsigned int& sens, const double& dMin)
{
    //Recuperation de la liste des rayons valides pour la _rayTracing.
    std::vector<Ray*>* rays = _rayTracing.getSolver()->getValidRays();

    //Recuperation de la liste des sources poncutelles
    TYMapElementTabSources& mapElementSources = calcul.getResultat()->getMapEmetteurSrcs();
    site.getInfrastructure()->getAllSrcs(&calcul, mapElementSources);
    TYTabSourcePonctuelleGeoNode sources; // Creation d'une collection des sources
    calcul.getAllSources(mapElementSources, sources);

    //Recuperation de la liste des recepteurs
    TYTabPointCalculGeoNode recepteurs;
    calcul.getAllRecepteurs(recepteurs);

    // Boucle sur les rayons
    //Conversion des rayons du lancer en rayons metier Tympan
    for (unsigned int i = 0; i < rays->size(); i++)
    {
        LPTYRay tyRay = new TYRay(); // Creation du rayon
        Ray* r = rays->at(i);

        // Recuperation de la source et du recepteur
        if (!setSEtRToTYRay(sources, recepteurs, sens, r, tyRay.getRealPointer())) { continue; }

        // Recuperation de la liste des evenements
        std::vector<TYRayEvent> events;
        buildListEvent(sens, r, events);
        // Insertion dans la liste des evenements du rayon (qui contient deja la source et le recepteur)
        vector<TYRayEvent>::iterator iter = tyRay->getEvents().begin();
        iter++; // On passe le premier point
        for (unsigned j = 0; j < events.size(); j++)
        {
            iter = tyRay->getEvents().insert(iter, events[j]);
            iter++;
        }

        // Calcul de l'angle de tir des rayons

        calculeAngleTirRayon(tyRay);

        // Eventuel surechantillonnage du tableau des evenements
        if (dMin > 0)
        {
            overSampleTYRay(tyRay, dMin);
        }

        // Ajoute le rayon au calcul
        calcul.addRay(tyRay);
    }
}

void TYANIME3DSolver::calculeAngleTirRayon(TYRay* tyRay)
{
    TYTabRayEvent& tabEvent = tyRay->getEvents();
    R3 P0 = TYPointToR3(tabEvent[0].pos);
    R3 P1 = TYPointToR3(tabEvent[1].pos);
    R3 v0(P0, P1);
    R3 v1(v0);
    v1.z = 0;
    v0.normalize();
    v1.normalize();

    double result = v0.mult(v1);
    int sign = result > 0 ?  1 : -1;
    R angle = ::acos(v0.mult(v1)) * sign;

    tabEvent[0].angle = angle;
}

void TYANIME3DSolver::overSampleTYRay(TYRay* tyRay, const double& dMin)
{
    TYTabPoint tabPoints;
    TYTabRayEvent& tabEvents = tyRay->getEvents();
    TYRayEvent rayEvent;

    // Gestion des iterateurs
    TYTabRayEvent::iterator iter = tabEvents.begin();
    TYTabRayEvent::iterator endIter = tabEvents.end() - 1;
    // On va traiter les evenements deux a deux
    while (iter != endIter)
    {
        // Recuperation des deux points initiaux
        tabPoints.push_back((*iter).pos);
        tabPoints.push_back((*(iter + 1)).pos);

        // Surechantillonnage du tableau de points
        tabPoints = TYPoint::checkPointsMaxDistance(tabPoints, dMin);

        // Insertion des points sous forme d'evenement au tableau des evenements
        TYTabRayEvent::iterator iter2 = iter + 1;
        for (unsigned int i = 1; i < tabPoints.size() - 1; i++)
        {
            rayEvent.type = TYREFRACTION;
            rayEvent.angle = 0.0;
            rayEvent.pos = tabPoints[i];
            iter2 = tabEvents.insert(iter2, rayEvent);
            iter2++;
        }

        tabPoints.clear();
        endIter = tabEvents.end() - 1;  // Actualisation car le tableau a ete modifie
        iter = iter2;               // Atualisation car le tableau a ete modifie
    };
}

void TYANIME3DSolver::courbeRayon(TYCalcul& calcul)
{
    TYTabRay& tabRay = calcul.getAllRays();
    for (unsigned int i = 0; i < tabRay.size(); i++)
    {
        TYTabRayEvent& tabEvent =  tabRay[i]->getEvents();
        for (unsigned int j = 0; j < tabEvent.size(); j++)
        {
            R3 point = TYPointToR3(tabEvent[j].pos);
            point = lancer.fonction_h_inverse(point);
            tabEvent[j].pos = R3ToTYPoint(point);
        }
    }
}

void TYANIME3DSolver::appendRecepteurToAnalyticRayTracer(TYCalcul& calcul, vector<R3>& tabRecepteurs)
{
    // Nettoyage du tableau des recepteurs
    tabRecepteurs.clear();

    //Ajout des recepteurs
    unsigned int idRecepteur = 0;

    //Recuperation des points de controle
    //TYTabLPPointControl tabRecepteurs = calcul.getProjet()->getPointsControl();
    TYTabPointCalculGeoNode tabRec;
    calcul.getAllRecepteurs(tabRec);

    //Conversion du recepteur Tympan en recepteur lancer de rayons
    for (unsigned int i = 0; i < tabRec.size(); i++)
    {
        TYPointCalcul* pc = TYPointCalcul::safeDownCast(tabRec.at(i)->getElement());
        //if(tabRecepteurs.at(i)->getState() == TYPointCalcul::Actif)

        OPoint3D pos = tabRec.at(i)->getMatrix() * (*pc);
        double coordRecepteur[3];

        pos.getCoords(coordRecepteur);

        R3 unPoint(coordRecepteur[0], coordRecepteur[1], coordRecepteur[2]);
        tabRecepteurs.push_back(unPoint);

        //      idRecepteur++;
    }

}

void TYANIME3DSolver::appendSourceToAnalyticRayTracer(const TYSiteNode& site, TYCalcul& calcul, vector<R3>& tabSources)
{
    //Nettoyage du tableau des sources
    tabSources.clear();

    //Recuperation de la liste des sources poncutelles
    TYMapElementTabSources& mapElementSources = calcul.getResultat()->getMapEmetteurSrcs();
    site.getInfrastructure()->getAllSrcs(&calcul, mapElementSources);

    TYTabSourcePonctuelleGeoNode sources; // Creation d'une collection des sources
    calcul.getAllSources(mapElementSources, sources);

    //Conversion des sources Tympan en source lancer de rayons
    int idSource = 0;
    TYSourcePonctuelle* sourceP = NULL;
    for (unsigned int i = 0; i < sources.size(); i++)
    {
        sourceP = TYSourcePonctuelle::safeDownCast(sources.at(i)->getElement());
        if (sourceP)
        {
            double coord[3];
            OMatrix matrice = sources.at(i)->getMatrix();
            TYPoint globalPos = matrice * (*sourceP->getPos());

            globalPos.getCoords(coord);
            R3 pos = R3(coord[0], coord[1], coord[2]);

            tabSources.push_back(pos);
        }
        idSource++;
    }
}

void TYANIME3DSolver::print()
{
	logger.write("LogsANIME3DSolver.txt");
    ostringstream fic_out;

	fic_out << "La simulation comporte" << _rayTracing.getRecepteurs().size() << "recepteurs et" << _rayTracing.getSources().size() << "sources." << ends;
    fic_out << "La scene est compose de" << _rayTracing.getScene()->getShapes()->size() << "primitives." << ends;

    fic_out << _rayTracing.getSolver()->getValidRays()->size() << "rayons ont ete trouves" << ends;
    fic_out << "Simulation terminee." << ends;

    fic_out << "Rayons_C" << lancer.Meteo.gradC << "_V" << lancer.Meteo.gradV << "_D" << lancer.dmax << ".txt" << ends;
    ofstream fic(fic_out.str().c_str());

    fic << "On a " << _tabRay.size() << " rayons" << endl; // nombre de rayons

    for (unsigned int i = 0; i < _tabRay.size(); i++)
    {
        TYTabRayEvent& tabEvent =  _tabRay[i]->getEvents();

        fic << "angle de depart du rayon " << (i + 1) << " : " << tabEvent[0].angle << endl; // angle de depart
        fic << "le rayon possede " << tabEvent.size() << " points" << endl; // nombre de points dans le rayon

        for (unsigned int j = 0; j < tabEvent.size(); j++)
        {
            R3 point = TYPointToR3(tabEvent[j].pos);

            fic << point.x << "\t" << point.y << "\t" << point.z << "\t" << tabEvent[j].angle << endl; // on prend les coordonnees des points
        }

        fic << endl; // on est a la fin du rayon
    }
}

 TYPoint TYANIME3DSolver::computePosGlobalPoint(const TYGeometryNode* pNode)
{
	TYPoint* pPoint;
	if (pNode->getElement()->inherits("TYSourcePonctuelle"))
		pPoint = TYSourcePonctuelle::safeDownCast(pNode->getElement())->getPos();
	else
		pPoint = TYPoint::safeDownCast(pNode->getElement());

	return  pNode->getMatrix() * (*pPoint);
}

 TYPoint TYANIME3DSolver::R3ToTYPoint(const R3& p)
{
	TYPoint point;
	point._x = p.x;
	point._y = p.y;
	point._z = p.z;
	return point;
}

R3 TYANIME3DSolver::TYPointToR3(const TYPoint& p)
{
	R3 point;
	point.x = p._x;
	point.y = p._y;
	point.z = p._z;
	return point;
}
