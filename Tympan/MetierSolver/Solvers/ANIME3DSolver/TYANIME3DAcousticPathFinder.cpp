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

#include "Tympan/MetierSolver/DataManagerCore/TYSolverInterface.h"

#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYPoint.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYGeometryNode.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticSurface.h"
#include "Tympan/MetierSolver/DataManagerMetier/Site/TYTopographie.h"
#include "Tympan/MetierSolver/DataManagerMetier/Site/TYInfrastructure.h"
#include "Tympan/MetierSolver/DataManagerMetier/Site/TYSiteNode.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYRay.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYTrajet.h"

#include "Tympan/MetierSolver/AcousticRaytracer/global.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/FaceSelector.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/LengthSelector.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/DiffractionSelector.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/ReflectionSelector.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/TargetManager.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/SelectorManager.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/Cylindre.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/Triangle.h"
#include "Tympan/MetierSolver/AnalyticRayTracer/Transfo.h"
#include "Tympan/MetierSolver/AnalyticRayTracer/meteoLin.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/UniformSphericSampler.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/UniformSphericSampler2.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/Latitude2DSampler.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/RandomSphericSampler.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Engine/Simulation.h"    //Classe de base pour utiliser le lancer de rayons


#include "TYANIME3DRayTracerSetup.h"
#include "TYANIME3DSolver.h"
#include "TYANIME3DAcousticPathFinder.h"

//#define _USE_METEO_


TYANIME3DAcousticPathFinder::TYANIME3DAcousticPathFinder(TYStructSurfIntersect* tabPolygon, const size_t& tabPolygonSize,
                                                         TYTabSourcePonctuelleGeoNode& tabSources, TYTabPointCalculGeoNode& tabRecepteurs,
                                                         TYTabRay& tabTYRays) : _tabPolygon(tabPolygon),
    _tabPolygonSize(tabPolygonSize),
    _tabSources(tabSources),
    _tabRecepteurs(tabRecepteurs),
    _tabTYRays(tabTYRays)
{
}

TYANIME3DAcousticPathFinder::~TYANIME3DAcousticPathFinder()
{
}

bool TYANIME3DAcousticPathFinder::exec()
{
    // Configuration du lancer de rayon geometriques (au debut pour initialiser les valeurs globales
    // Nettoyage de l'objet _rayTracing si il a ete utilise precedement
    _rayTracing.clean();

    // Ajout des parametres du _rayTracing liés à la methode acoustique
    TYANIME3DRayTracerSetup* solver = new TYANIME3DRayTracerSetup();
    solver->initGlobalValues();
    _rayTracing.setSolver(solver);

    vector<vec3> sources;
    vector<vec3> recepteurs;
    unsigned int sens = getTabsSAndR(sources, recepteurs);

#ifdef _USE_METEO_
    // Definition des objets de lancer de rayons courbes
    if (globalUseMeteo)
    {
        // Creation du lancer de rayons courbes
        _curveRayTracing.clear();

        // Recuperation des sources et des recepteurs
        //appendSourceToAnalyticRayTracer(_curveRayTracing.shot.sources);
        //appendRecepteurToAnalyticRayTracer(_curveRayTracing.shot.recepteurs);

        // Parametrage du lancer
        _curveRayTracing.setDMax(globalAnalyticDMax);
        _curveRayTracing.setTMax(globalAnalyticTMax);
        _curveRayTracing.setTimeStep(globalAnalyticH);
        _curveRayTracing.setNbRay(globalAnalyticNbRay);
        dynamic_cast<meteoLin*>(_curveRayTracing.shot._weather)->setGradC(globalAnalyticGradC);
        dynamic_cast<meteoLin*>(_curveRayTracing.shot._weather)->setGradV(globalAnalyticGradV);
        _curveRayTracing.shot._weather->setWindAngle(globalWindDirection);
        _curveRayTracing.shot._weather->setC0(globalAnalyticC0);
        _curveRayTracing.setMethode(globalAnalyticTypeTransfo);
        _curveRayTracing.shot.setLaunchType(1);                                 // Indique que l'on tire les rayons sur un plan horizontal

        dynamic_cast<Latitude2DSampler*>(_curveRayTracing.shot.getSampler())->setStartTheta(globalAnalyticAngleTheta);

        _curveRayTracing.shot.initialAngleTheta = globalAnalyticAngleTheta;     // Angle de tir vertical (theta) des rayons

        // Choix de la source
        _curveRayTracing.setSource(sources[0]);   // At this time source is the firs one

        // Transformation de la geometrie
        _curveRayTracing.buildInterpolationSurface();

        // Positionnement des sources et des recepteurs
        transformSEtR(sources, recepteurs);
    }
#endif //_USE_METEO_

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


    // This function creates TYRays from Rays .
    convertRaytoTYRay(sens);

    // This function corrects distances between events and angles at each event
    sampleAndCorrection();

    return true;
}

unsigned int TYANIME3DAcousticPathFinder::getTabsSAndR(vector<vec3>& sources, vector<vec3>& recepteurs)
{
    unsigned int sens = globalRayTracingOrder;

    // Recuperation de la position des sources du projet
    vector<vec3> srcs;

    //Conversion des sources Tympan en source lancer de rayons
    TYSourcePonctuelle* sourceP = NULL;
    for (unsigned int i = 0; i < _tabSources.size(); i++)
    {
        OPoint3D globalPos = computePosGlobalPoint(_tabSources.at(i));
        vec3 pos = vec3(globalPos._x, globalPos._y, globalPos._z);
        srcs.push_back(pos);
    }
    // Recuperation de la position des recepteurs du projet
    vector<vec3> rcpts;

    //Conversion du recepteur Tympan en recepteur lancer de rayons
    for (unsigned int i = 0; i < _tabRecepteurs.size(); i++)
    {
        OPoint3D globalPos = computePosGlobalPoint(_tabRecepteurs.at(i));
        vec3 pos = vec3(globalPos._x, globalPos._y, globalPos._z);

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


TYPoint TYANIME3DAcousticPathFinder::computePosGlobalPoint(const TYGeometryNode* pNode)
{
    TYPoint* pPoint;
    if (pNode->getElement()->inherits("TYSourcePonctuelle"))
    {
        pPoint = TYSourcePonctuelle::safeDownCast(pNode->getElement())->getPos();
    }
    else { pPoint = TYPoint::safeDownCast(pNode->getElement()); }

    return  pNode->getMatrix() * (*pPoint);
}


//void TYANIME3DAcousticPathFinder::appendSourceToAnalyticRayTracer(vector<vec3>& tabR3Sources)
//{
//    //Nettoyage du tableau des sources
//    tabR3Sources.clear();
//
//    //Conversion des sources Tympan en source lancer de rayons
//    int idSource = 0;
//    TYSourcePonctuelle* sourceP = NULL;
//    for (unsigned int i = 0; i < _tabSources.size(); i++)
//    {
//      OPoint3D globalPos = computePosGlobalPoint(_tabSources.at(i));
//
//        vec3 pos = vec3(globalPos._x, globalPos._y, globalPos._z);
//
//        tabR3Sources.push_back(pos);
//
//        idSource++;
//    }
//}
//
//void TYANIME3DAcousticPathFinder::appendRecepteurToAnalyticRayTracer(vector<vec3>& tabR3Recept)
//{
//    // Nettoyage du tableau des recepteurs
//    tabR3Recept.clear();
//
//    //Ajout des recepteurs
//    unsigned int idRecepteur = 0;
//
//    //Conversion du recepteur Tympan en recepteur lancer de rayons
//    for (unsigned int i = 0; i < _tabRecepteurs.size(); i++)
//    {
//      OPoint3D pos = computePosGlobalPoint(_tabRecepteurs.at(i));
//
//        vec3 unPoint(pos._x, pos._y, pos._z);
//        tabR3Recept.push_back(unPoint);
//    }
//}


void TYANIME3DAcousticPathFinder::transformSEtR(vector<vec3>& sources, vector<vec3>& recepteurs)
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


vec3 TYANIME3DAcousticPathFinder::modifGeom(const vec3& point)
{
    if (!globalUseMeteo) { return point; } // Aucune action si pas meteo

    vec3 mPoint(point.x, point.y, point.z);
    mPoint = _curveRayTracing.fonction_h(mPoint);

    return vec3(mPoint.x, mPoint.y, mPoint.z);
}


bool TYANIME3DAcousticPathFinder::appendTriangleToScene()
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

#ifdef _USE_METEO_
            _tabPolygon[i].tabPoint[0].getCoords(coord);
            pos = modifGeom(vec3(coord[0], coord[1], coord[2]));
            scene->addVertex(pos, a);

            _tabPolygon[i].tabPoint[1].getCoords(coord);
            pos = modifGeom(vec3(coord[0], coord[1], coord[2]));
            scene->addVertex(pos, b);

            _tabPolygon[i].tabPoint[2].getCoords(coord);
            pos = modifGeom(vec3(coord[0], coord[1], coord[2]));
            scene->addVertex(pos, c);
#else
            _tabPolygon[i].tabPoint[0].getCoords(coord);
            pos = vec3(coord[0], coord[1], coord[2]);
            scene->addVertex(pos, a);

            _tabPolygon[i].tabPoint[1].getCoords(coord);
            pos = vec3(coord[0], coord[1], coord[2]);
            scene->addVertex(pos, b);

            _tabPolygon[i].tabPoint[2].getCoords(coord);
            pos = vec3(coord[0], coord[1], coord[2]);
            scene->addVertex(pos, c);

#endif
            Triangle* face;
            if (_tabPolygon[i].isEcran || _tabPolygon[i].isInfra)
            {
                face = (Triangle*)scene->addTriangle(a, b, c, m1);
                ss << "Ajout d'un triangle non naturel." << std::endl;
            }
            else
            {
                face = (Triangle*)scene->addTriangle(a, b, c, m2, true);
            }
            face->setBuildingId(_tabPolygon[i].idBuilding);
            face->setFaceId(_tabPolygon[i].idFace);
            face->setEtageId(_tabPolygon[i].idEtage);
            //face->setNormal(normalFace);

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

#ifdef _USE_METEO_
                coord = _tabPolygon[i].realVertex.at(_tabPolygon[i].triangles.at(j)._p1);
                pos = modifGeom(vec3(coord._x, coord._y, coord._z));
                scene->addVertex(pos, a);

                coord = _tabPolygon[i].realVertex.at(_tabPolygon[i].triangles.at(j)._p2);
                pos = modifGeom(vec3(coord._x, coord._y, coord._z));
                scene->addVertex(pos, b);

                coord = _tabPolygon[i].realVertex.at(_tabPolygon[i].triangles.at(j)._p3);
                pos = modifGeom(vec3(coord._x, coord._y, coord._z));
                scene->addVertex(pos, c);
#else
                coord = _tabPolygon[i].realVertex.at(_tabPolygon[i].triangles.at(j)._p1);
                pos = vec3(coord._x, coord._y, coord._z);
                scene->addVertex(pos, a);

                coord = _tabPolygon[i].realVertex.at(_tabPolygon[i].triangles.at(j)._p2);
                pos = vec3(coord._x, coord._y, coord._z);
                scene->addVertex(pos, b);

                coord = _tabPolygon[i].realVertex.at(_tabPolygon[i].triangles.at(j)._p3);
                pos = vec3(coord._x, coord._y, coord._z);
                scene->addVertex(pos, c);

#endif //_USE_METEO_

                Triangle* face;
                if (_tabPolygon[i].isEcran || _tabPolygon[i].isInfra)
                {
                    face = (Triangle*)scene->addTriangle(a, b, c, m1);
                    ss << "Ajout d'un triangle non naturel." << std::endl;
                }
                else
                {
                    face = (Triangle*)scene->addTriangle(a, b, c, m2, true);
                }
                face->setBuildingId(_tabPolygon[i].idBuilding);
                face->setFaceId(_tabPolygon[i].idFace);
                face->setEtageId(_tabPolygon[i].idEtage);
                //face->setNormal(normalFace);

                if (face->getNormal().dot(normalFace) < -0.001) //Les normales sont de sens contraire
                {
                    face->setNormal(face->getNormal() * (-1.));
                }
            }
        }
    }
    return true;
}

void TYANIME3DAcousticPathFinder::appendRecepteurToSimulation(vector<vec3>& recepteurs)
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


void TYANIME3DAcousticPathFinder::appendSourceToSimulation(vector<vec3>& sources)
{
    //Conversion des sources Tympan en sources lancer de rayons
    int idSource = 0;
    for (unsigned int i = 0; i < sources.size(); i++)
    {
        Source source;
        switch (globalDiscretization)
        {
            case 0 :
                source.setSampler(new RandomSphericSampler(globalNbRaysPerSource));
                break;
            case 1 :
                source.setSampler(new UniformSphericSampler(globalNbRaysPerSource));
                globalNbRaysPerSource = dynamic_cast<UniformSphericSampler*>(source.getSampler())->getRealNbRays();
                break;
            case 2 :
                source.setSampler(new UniformSphericSampler2(globalNbRaysPerSource));
                globalNbRaysPerSource = dynamic_cast<UniformSphericSampler2*>(source.getSampler())->getRealNbRays();
                break;
            case 3 :
                source.setSampler(new Latitude2DSampler(globalNbRaysPerSource));
                dynamic_cast<Latitude2DSampler*>(source.getSampler())->setStartPhi(0.);
                dynamic_cast<Latitude2DSampler*>(source.getSampler())->setEndPhi(360.);
                dynamic_cast<Latitude2DSampler*>(source.getSampler())->setStartTheta(0.);
                dynamic_cast<Latitude2DSampler*>(source.getSampler())->setEndTheta(0.);
        }

        source.setPosition(sources[i]);
        source.setInitialRayCount(globalNbRaysPerSource);
        source.setId(idSource);

        ss << "Ajout d'une source en (" << sources[i].x << "," << sources[i].y << "," << sources[i].y << ")" << endl;

        _rayTracing.addSource(source);
        idSource++;
    }
}

void TYANIME3DAcousticPathFinder::convertRaytoTYRay(const unsigned int& sens)
{
    //Recuperation de la liste des rayons valides pour la _rayTracing.
    std::vector<Ray*>* rays = _rayTracing.getSolver()->getValidRays();

    // Dimensionnement du tableau des rayons
    _tabTYRays.reserve(rays->size());

    //Conversion des rayons du lancer en rayons metier Tympan
    for (unsigned int i = 0; i < rays->size(); i++)
    {
        LPTYRay tyRay = new TYRay(); // Creation du rayon
        Ray* r = rays->at(i);

        // Recuperation de la source et du recepteur
        if (!setSEtRToTYRay(sens, r, tyRay.getRealPointer())) { continue; }

        // Recuperation de la liste des evenements
        std::vector<TYRayEvent*> events;
        buildListEvent(sens, r, events);

        // Insertion dans la liste des evenements du rayon (qui contient deja la source et le recepteur)
        std::vector<TYRayEvent*>::iterator iter = tyRay->getEvents().begin();
        iter++; // On passe le premier point
        for (unsigned int j = 0; j < events.size(); j++, iter++)
        {
            iter = tyRay->getEvents().insert(iter, events[j]);
        }

        // Ajoute les informations de liaison entre les évènements du TYRay
        std::vector<int> tabIndex = tyRay->getIndexOfEvents(TYREFLEXION | TYREFLEXIONSOL | TYRECEPTEUR);
        unsigned int k = 0;
        TYTabRayEvent tabEvents = tyRay->getEvents();
        for (unsigned int j = 0; j < tabEvents.size() - 1; j++)
        {
            tabEvents.at(j)->next = tabEvents.at(j + 1);

            if (j > 0) { tabEvents.at(j)->previous = tabEvents.at(j - 1); }
            if (j == tabEvents.size() - 2) { tabEvents.at(j + 1)->previous = tabEvents.at(j); }

            if (j == tabIndex[k]) { k++; }

            tabEvents.at(j)->endEvent = tabEvents.at(tabIndex[k]);
        }


        // Calcul de l'angle de tir des rayons
        calculeAngleTirRayon(tyRay);

        // Ajoute le rayon au calcul
        _tabTYRays.push_back(tyRay);
    }
}


bool TYANIME3DAcousticPathFinder::setSEtRToTYRay(const unsigned int sens, Ray* r, TYRay* tyRay)
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
    if (static_cast<unsigned int>(idRecep) >= _tabRecepteurs.size() || static_cast<unsigned int>(idSource) >= _tabSources.size()) { return false; }

    vec3 pS = _rayTracing.getSources().at(idSource).getPosition();
    TYPoint posSourceGlobal(pS.x, pS.y, pS.z);
    TYSourcePonctuelle* sourceP = TYSourcePonctuelle::safeDownCast(_tabSources.at(idSource)->getElement());

    vec3 pR = _rayTracing.getRecepteurs().at(idRecep).getPosition();
    TYPoint posReceptGlobal(pR.x, pR.y, pR.z);
    TYPointCalcul* recepP = TYPointCalcul::safeDownCast(_tabRecepteurs.at(idRecep)->getElement());

    tyRay->setSource(sourceP, posSourceGlobal);
    tyRay->setRecepteur(recepP, posReceptGlobal);

    //Ajout de la source
    TYRayEvent* e = new TYRayEvent();
    e->type = TYSOURCE;
    e->pos = TYPoint(r->source->getPosition().x, r->source->getPosition().y, r->source->getPosition().z);
    e->angle = 0.0;
    tyRay->getEvents().push_back(e);

    e = new TYRayEvent();
    e->type = TYRECEPTEUR;
    Recepteur* recep = (static_cast<Recepteur*>(r->getRecepteur()));
    e->pos = TYPoint(recep->getPosition().x, recep->getPosition().y, recep->getPosition().z);
    e->angle = 0.0;
    tyRay->getEvents().push_back(e);

    return true;
}


void TYANIME3DAcousticPathFinder::buildListEvent(const int& sens, Ray* ray, std::vector<TYRayEvent*>& tabEvents)
{
    //Definition des Evenements.
    TYRayEvent* e = NULL;

    if (sens == 1)
    {
        //Creation des evenements de diffractions et reflexions
        std::vector<QSharedPointer<Event> >::reverse_iterator rit;

        for (rit = ray->getEvents()->rbegin(); rit != ray->getEvents()->rend(); rit++)
        {
            e = new TYRayEvent();
            convertRayEventToTYRayEvent((*rit), e);
            tabEvents.push_back(e);
        }
    }
    else
    {
        //Creation des evenements de diffractions et reflexions
        std::vector<QSharedPointer<Event> >::iterator rit;

        for (rit = ray->getEvents()->begin(); rit != ray->getEvents()->end(); rit++)
        {
            e = new TYRayEvent();
            convertRayEventToTYRayEvent((*rit), e);
            tabEvents.push_back(e);
        }
    }
}


void TYANIME3DAcousticPathFinder::convertRayEventToTYRayEvent(const QSharedPointer<Event> rev, TYRayEvent* tyrev)
{
    Cylindre* cyl = NULL;
    switch (rev->getType())
    {
        case SPECULARREFLEXION :
            if (rev->getShape()->isSol())
            {
                tyrev->type = TYREFLEXIONSOL;
            }
            else
            {
                tyrev->type = TYREFLEXION;
            }

            tyrev->idFace1 = rev->getShape()->getFaceId();

            break;

        case DIFFRACTION:
            tyrev->type = TYDIFFRACTION;
            cyl = static_cast<Cylindre*>(rev->getShape());
            if (cyl)
            {
                tyrev->idFace1 = cyl->getFirstShape()->getFaceId();
                tyrev->idFace2 = cyl->getSecondShape()->getFaceId();
            }
            break;

        default:
            tyrev->type = TY_NO_TYPE;
            break;
    }

    tyrev->pos = TYPoint(rev->getPosition().x, rev->getPosition().y, rev->getPosition().z);
    tyrev->angle = rev->getAngle();
}


void TYANIME3DAcousticPathFinder::calculeAngleTirRayon(TYRay* tyRay)
{
    TYTabRayEvent& tabEvent = tyRay->getEvents();
    vec3 P0 = OPoint3Dtovec3(tabEvent[0]->pos);
    vec3 P1 = OPoint3Dtovec3(tabEvent[1]->pos);
    vec3 v0(P0, P1);
    vec3 v1(v0);
    vec3 v2(v0);
    v1.z = 0;
    v2.y = 0;
    v0.normalize();
    v1.normalize();
    v2.normalize();

    // Angle phi
    double result = v0 * v1;
    int sign = v0.z > 0 ?  1 : -1;
    double angle = ::acos(result) * sign;

    tabEvent[0]->angle = angle;

    // Angle theta
    result = v0 * v2;
    sign = v0.y > 0 ?  1 : -1;
    angle = ::acos(result) * sign;
    angle = v0.x < 0 ? M_PI - angle : angle;

    tabEvent[0]->angletheta = angle;
}

void TYANIME3DAcousticPathFinder::sampleAndCorrection()
{
    TYRay* tmpTYRay = NULL;
    // Pour tous les rayons
    for (int i = 0; i < _tabTYRays.size(); i++)
    {
        // Création d'une copie du TYRay
        tmpTYRay = _tabTYRays.at(i);

        // Récupération des longueurs simples (éléments suivants)
        nextLenghtCompute(tmpTYRay);

        // Récupération des distances aux évènements pertinents
        endLenghtCompute(tmpTYRay);

        // Distance entre évènement précédent et suivant
        prevNextLengthCompute(tmpTYRay);

        // Récupération des angles
        angleCompute(tmpTYRay);

        // Correction de la position des évènements
        eventPosCompute(tmpTYRay);
    }
}

void TYANIME3DAcousticPathFinder::endLenghtCompute(TYRay* tyRay)
{
    TYTabRayEvent& tabEvent = tyRay->getEvents();

    // Boucle sur tous les évènements du rayon
    for (unsigned j = 0; j < tabEvent.size() - 1; j++)
    {
        // Path length correction for distance from one event to next event needed for calculus
        tabEvent.at(j)->distEndEvent = lengthCorrection(tabEvent.at(j), tabEvent.at(j)->endEvent);
    }
}

void TYANIME3DAcousticPathFinder::nextLenghtCompute(TYRay* tyRay)
{
    TYTabRayEvent& tabEvent = tyRay->getEvents();

    // Boucle sur tous les évènements du rayon
    for (unsigned j = 0; j < tabEvent.size() - 1; j++)
    {
        // Path length correction for distance from one event to next event needed for calculus
        tabEvent.at(j)->distNextEvent = lengthCorrection(tabEvent.at(j), tabEvent.at(j)->next);
    }
}

void TYANIME3DAcousticPathFinder::prevNextLengthCompute(TYRay* tyRay)
{
    TYTabRayEvent& tabEvent = tyRay->getEvents();

    // For all events in the ray
    for (unsigned j = 1; j < tabEvent.size() - 1; j++)
    {
        // Path length correction gives back the corrected distance between event-1 and event+1
        tabEvent.at(j)->distPrevNext = lengthCorrection(tabEvent.at(j)->previous, tabEvent.at(j)->next);
    }
}

void TYANIME3DAcousticPathFinder::angleCompute(TYRay* tyRay)
{
    TYTabRayEvent& tabEvent = tyRay->getEvents();

    // Boucle sur tous les évènements du rayon
    for (unsigned j = 1; j < tabEvent.size() - 1; j++)
    {
        // Path length correction for distance from one event to next event needed for calculus
        tabEvent.at(j)->angle = angleCorrection(tabEvent.at(j)->previous, tabEvent.at(j), tabEvent.at(j)->next);
    }
}

void TYANIME3DAcousticPathFinder::eventPosCompute(TYRay* tyRay)
{
    TYTabRayEvent& tabEvent = tyRay->getEvents();

    // Boucle sur tous les évènements du rayon
    for (unsigned i = 0; i < tabEvent.size(); i++)
    {
        vec3 point = OPoint3Dtovec3(tabEvent[i]->pos);
        point = _curveRayTracing.fonction_h_inverse(point);
        tabEvent[i]->pos = vec3toOPoint3D(point);
    }
}

double TYANIME3DAcousticPathFinder::lengthCorrection(TYRayEvent* ev1, const TYRayEvent* ev2)
{
    TabPoint3D tabPoint = OPoint3D::checkPointsMaxDistance(ev1->pos, ev2->pos, globalOverSampleD);

    // Calculation with h for each event
    // Useful for lengths & angles
    for (int i = 0; i < tabPoint.size(); i++)
    {
        vec3 point = OPoint3Dtovec3(tabPoint[i]);
        point = _curveRayTracing.fonction_h_inverse(point);
        tabPoint[i] = vec3toOPoint3D(point);
    }

    double length = 0.;
    for (int i = 0; i < tabPoint.size() - 1; i++)
    {
        length += tabPoint[i].distFrom(tabPoint[i + 1]);
    }


    return length;
}


double TYANIME3DAcousticPathFinder::angleCorrection(const TYRayEvent* ev1, TYRayEvent* ev2, const TYRayEvent* ev3)
{
    TabPoint3D tabPoint1 = OPoint3D::checkPointsMaxDistance(ev1->pos, ev2->pos, globalOverSampleD);
    TabPoint3D tabPoint2 = OPoint3D::checkPointsMaxDistance(ev2->pos, ev3->pos, globalOverSampleD);

    // Corrects position for the only three events of interest
    OPoint3D points[3] = { tabPoint1.at(tabPoint1.size() - 2), tabPoint1.at(tabPoint1.size() - 1), tabPoint2.at(1) };
    for (int i = 0; i < 3; i++)
    {
        vec3 point = OPoint3Dtovec3(points[i]);
        point = _curveRayTracing.fonction_h_inverse(point);
        points[i] = vec3toOPoint3D(point);
    }

    OVector3D vec1(points[1], points[0]);
    OVector3D vec2(points[1], points[2]);

#ifdef _DEBUG
    double angle = vec1.angle(vec2);
    double angleComp = (M_PI - vec1.angle(vec2)) / 2.;
#endif

    return (M_PI - vec1.angle(vec2)) / 2.;
}

void TYANIME3DAcousticPathFinder::tyRayCorrection(TYRay* tyRay)
{
    TYTabRayEvent& tabEvents = tyRay->getEvents();

    // Repositionnement des elements du rayon
    for (unsigned i = 0; i < tabEvents.size(); i++)
    {
        vec3 point = OPoint3Dtovec3(tabEvents[i]->pos);
        point = _curveRayTracing.fonction_h(point);
        tabEvents[i]->pos = vec3toOPoint3D(point);
    }

    tyRay->overSample(globalOverSampleD);

    TYTabRayEvent& tabEvents2 = tyRay->getEvents();

    for (unsigned int i = 0 ; i < tabEvents2.size() ; i++)
    {
        vec3 point = OPoint3Dtovec3(tabEvents2[i]->pos);
        point = _curveRayTracing.fonction_h_inverse(point);
        tabEvents2[i]->pos = vec3toOPoint3D(point);
    }
}
