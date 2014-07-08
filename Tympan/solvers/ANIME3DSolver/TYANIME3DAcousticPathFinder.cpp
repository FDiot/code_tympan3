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

#include <vector>
using std::vector;


#include "Tympan/models/business/TYSolverInterface.h"

#include "Tympan/models/common/triangle.h"
//#include "Tympan/models/business/TYRay.h"

#include "Tympan/solvers/AcousticRaytracer/global.h"
#include "Tympan/solvers/AcousticRaytracer/Tools/FaceSelector.h"
#include "Tympan/solvers/AcousticRaytracer/Tools/LengthSelector.h"
#include "Tympan/solvers/AcousticRaytracer/Tools/DiffractionSelector.h"
#include "Tympan/solvers/AcousticRaytracer/Tools/ReflectionSelector.h"
#include "Tympan/solvers/AcousticRaytracer/Tools/SelectorManager.h"
#include "Tympan/solvers/AcousticRaytracer/Geometry/Cylindre.h"
#include "Tympan/solvers/AcousticRaytracer/Geometry/Triangle.h"

#include "Tympan/solvers/AnalyticRayTracer/meteoLin.h"
#include "Tympan/solvers/AnalyticRayTracer/Lancer.h"

#include "Tympan/solvers/AcousticRaytracer/Geometry/Sampler.h"
#include "Tympan/solvers/AcousticRaytracer/Geometry/UniformSphericSampler.h"
#include "Tympan/solvers/AcousticRaytracer/Geometry/UniformSphericSampler2.h"
#include "Tympan/solvers/AcousticRaytracer/Geometry/Latitude2DSampler.h"
#include "Tympan/solvers/AcousticRaytracer/Geometry/RandomSphericSampler.h"
#include "Tympan/solvers/AcousticRaytracer/Engine/Simulation.h"    //Classe de base pour utiliser le lancer de rayons
#include "Tympan/solvers/AcousticRaytracer/Tools/Conversion_tools.h"

#include "TYANIME3DRayTracerSetup.h"
#include "TYANIME3DSolver.h"
#include "TYANIME3DAcousticPathFinder.h"

#define _USE_METEO_


TYANIME3DAcousticPathFinder::TYANIME3DAcousticPathFinder(TYStructSurfIntersect* tabPolygon, const size_t& tabPolygonSize,
                                                         TYTabSourcePonctuelleGeoNode& tabSources, TYTabPointCalculGeoNode& tabRecepteurs,
                                                         tab_acoustic_path& tabTYRays) : _tabPolygon(tabPolygon),
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

    // Create geometry transformer
    build_geometry_transformer( sources );

    // Positionnement des sources et des recepteurs
    transformSEtR(sources, recepteurs);

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
    convert_Rays_to_acoustic_path(sens);

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


OPoint3D TYANIME3DAcousticPathFinder::computePosGlobalPoint(const TYGeometryNode* pNode)
{
    OPoint3D * pPoint;
    TYSourcePonctuelle* pSource = dynamic_cast<TYSourcePonctuelle*>(pNode->getElement());
    if (pSource != nullptr)
    {
        pPoint = pSource->getPos();
    }
    else { pPoint = TYPoint::safeDownCast(pNode->getElement()); }

    return  pNode->getMatrix() * (*pPoint);
}

void TYANIME3DAcousticPathFinder::transformSEtR(vector<vec3>& sources, vector<vec3>& recepteurs)
{
    // Pour toutes les sources
    for (unsigned int i = 0; i < sources.size(); i++)
    {
        sources[i] = transformer.fonction_h(sources[i]);
    }

    // Pour tous les recepteurs
    for (unsigned int i = 0; i < recepteurs.size(); i++)
    {
        recepteurs[i] = transformer.fonction_h(recepteurs[i]);
    }
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

            _tabPolygon[i].tabPoint[0].getCoords(coord);
            pos = transformer.fonction_h( OPoint3Dtovec3(coord) );
            scene->addVertex(pos, a);

            _tabPolygon[i].tabPoint[1].getCoords(coord);
            pos = transformer.fonction_h( OPoint3Dtovec3(coord) );
            scene->addVertex(pos, b);

            _tabPolygon[i].tabPoint[2].getCoords(coord);
            pos = transformer.fonction_h( OPoint3Dtovec3(coord) );
            scene->addVertex(pos, c);

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

                coord = _tabPolygon[i].realVertex.at(_tabPolygon[i].triangles.at(j)._p1);
                pos = transformer.fonction_h( OPoint3Dtovec3(coord) );
                scene->addVertex(pos, a);

                coord = _tabPolygon[i].realVertex.at(_tabPolygon[i].triangles.at(j)._p2);
                pos = transformer.fonction_h( OPoint3Dtovec3(coord) );
                scene->addVertex(pos, b);

                coord = _tabPolygon[i].realVertex.at(_tabPolygon[i].triangles.at(j)._p3);
                pos = transformer.fonction_h( OPoint3Dtovec3(coord) );
                scene->addVertex(pos, c);

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
        Recepteur recep(recepteurs[i], globalSizeReceiver);
        //recep.setShape(new Sphere(recepteurs[i], globalSizeReceiver));
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

void TYANIME3DAcousticPathFinder::convert_Rays_to_acoustic_path(const unsigned int& sens)
{
    //Recuperation de la liste des rayons valides pour la _rayTracing.
    std::deque<Ray*>* rays = _rayTracing.getSolver()->getValidRays();

    // Dimensionnement du tableau des rayons
    _tabTYRays.reserve(rays->size());

    //Conversion des rayons du lancer en rayons metier Tympan
    for (unsigned int i = 0; i < rays->size(); i++)
    {
        Ray* ray = rays->at(i);
        acoustic_path *tyRay = new acoustic_path( Tools::build_from_Ray(sens, ray) ); // Creation du rayon

        // Connect TYSource & TYReceptor (will be obsolete in future solver data model)
        set_source_idx_and_receptor_idx_to_acoustic_path(sens, ray, tyRay);

        // Ajoute le rayon au calcul
        _tabTYRays.push_back(tyRay);
    }
}

void TYANIME3DAcousticPathFinder::sampleAndCorrection()
{
    for (int i = 0; i < _tabTYRays.size(); i++)
    {
        // Récupération des longueurs simples (éléments suivants)
        _tabTYRays.at(i)->nextLenghtCompute(transformer);

        // Récupération des distances aux évènements pertinents
        _tabTYRays.at(i)->endLenghtCompute(transformer);

        // Distance entre évènement précédent et suivant
        _tabTYRays.at(i)->prevNextLengthCompute(transformer);

        // Récupération des angles
        _tabTYRays.at(i)->angleCompute(transformer);

        // Correction de la position des évènements
        _tabTYRays.at(i)->eventPosCompute(transformer);
    }
}

void TYANIME3DAcousticPathFinder::set_source_idx_and_receptor_idx_to_acoustic_path(int sens, Ray *ray, acoustic_path *tyRay)
{
    unsigned int idSource = 0;
    unsigned int idRecep = 0;
    if (sens == 1) // On inverse les identifiants de source et de recepteur
    {
        idSource = static_cast<Recepteur*>(ray->getRecepteur())->getId();
        idRecep = ray->getSource()->getId();
    }
    else
    {
        idRecep = static_cast<Recepteur*>(ray->getRecepteur())->getId();
        idSource = ray->getSource()->getId();
    }

    //Les identifiants des recepteurs et sources sont construit pour correspondre a l'index des sources et recepteurs dans Tympan.
    assert (static_cast<unsigned int>(idRecep) < _tabRecepteurs.size() && static_cast<unsigned int>(idSource) < _tabSources.size());

    vec3 pS = _rayTracing.getSources().at(idSource).getPosition();
    OPoint3D posSourceGlobal(pS.x, pS.y, pS.z);
    TYSourcePonctuelle* sourceP = TYSourcePonctuelle::safeDownCast(_tabSources.at(idSource)->getElement());

    vec3 pR = _rayTracing.getRecepteurs().at(idRecep).getPosition();
    OPoint3D posReceptGlobal(pR.x, pR.y, pR.z);
    TYPointCalcul* recepP = TYPointCalcul::safeDownCast(_tabRecepteurs.at(idRecep)->getElement());

    tyRay->setSource(idSource); //(sourceP, posSourceGlobal);
    tyRay->setRecepteur(idRecep); //(recepP, posReceptGlobal);
}

void TYANIME3DAcousticPathFinder::build_geometry_transformer( const vector<vec3>& sources )
{
    if (globalUseMeteo)
    {
        // Creation du lancer de rayons courbes
        Lancer CurveRayShot;

        // Parametrage du lancer de rayons courbe
        CurveRayShot.clear();
        CurveRayShot.setDMax(globalAnalyticDMax);

        CurveRayShot.setTMax(globalAnalyticTMax);
        CurveRayShot.setTimeStep(globalAnalyticH);
        CurveRayShot.setNbRay(globalAnalyticNbRay);
        dynamic_cast<meteoLin*>(CurveRayShot._weather)->setGradC(globalAnalyticGradC);
        dynamic_cast<meteoLin*>(CurveRayShot._weather)->setGradV(globalAnalyticGradV);
        CurveRayShot._weather->setWindAngle(globalWindDirection);
        CurveRayShot._weather->setC0(globalAnalyticC0);
        CurveRayShot.setLaunchType(1);                                 // Indique que l'on tire les rayons sur un plan horizontal

        dynamic_cast<Latitude2DSampler*>(CurveRayShot.getSampler())->setStartTheta(globalInitialAnglePhi);

        CurveRayShot.initialAngleTheta = globalInitialAngleTheta;     // Angle de tir vertical (theta) des rayons

        // Choix de la source
        CurveRayShot.addSource(sources[0]);   // At this time source is the firs one

        // Transformation de la geometrie
        transformer.clear();
        transformer.setMethode(globalAnalyticTypeTransfo);
        transformer.trianguleNappe(CurveRayShot);
    }
}
