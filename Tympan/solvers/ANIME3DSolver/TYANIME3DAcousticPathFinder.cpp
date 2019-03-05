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

#include "Tympan/models/common/triangle.h"
#include "Tympan/models/common/atmospheric_conditions.h"
#include "Tympan/models/solver/config.h"
#include "Tympan/models/solver/acoustic_problem_model.hpp"
#include "Tympan/geometric_methods/AnalyticRayTracer/meteoLin.h"
#include "Tympan/geometric_methods/AnalyticRayTracer/Lancer.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Tools/FaceSelector.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Tools/LengthSelector.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Tools/DiffractionSelector.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Tools/ReflectionSelector.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Tools/SelectorManager.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Cylindre.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Triangle.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Sampler.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/UniformSphericSampler.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/UniformSphericSampler2.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Latitude2DSampler.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/RandomSphericSampler.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/UniformBeamSampler.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Engine/Simulation.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Engine/AcousticRaytracerConfiguration.h"
#include "Tympan/solvers/ANIME3DSolver/TYANIME3DRayTracerSolverAdapter.h"
#include "Tympan/solvers/ANIME3DSolver/TYANIME3DAcousticPathFinder.h"
#include "TYANIME3DSolver.h"
#include "TYANIME3DConversionTools.h"

TYANIME3DAcousticPathFinder::TYANIME3DAcousticPathFinder(   TYStructSurfIntersect* tabPolygon, 
                                                            const size_t& tabPolygonSize,
                                                            const tympan::AcousticProblemModel& aproblem_, 
                                                            tab_acoustic_path& tabTYRays,
                                                            AtmosphericConditions& atmos) :
    _tabPolygon(tabPolygon),
    _tabPolygonSize(tabPolygonSize),  
    _tabTYRays(tabTYRays),
    _atmos(atmos),
    _aproblem(aproblem_)
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
    TYANIME3DRayTracerSolverAdapter* solver = new TYANIME3DRayTracerSolverAdapter();
    _rayTracing.setSolver(solver);

    vector<vec3> sources;
    vector<vec3> recepteurs;
	vector<tympan::VolumeFaceDirectivity*> directivities;
    unsigned int sens = getTabsSAndR(sources, recepteurs,directivities);

    // Create geometry transformer
    build_geometry_transformer( sources );

    //Ajout des triangles a l'objet Scene de _rayTracing
    appendTriangleToScene();

    //Importation des recepteurs ponctuels actifs
    appendRecepteurToSimulation(recepteurs);

    //Importation des sources ponctuelles actives
    appendSourceToSimulation(sources,directivities);

    //Une fois la scene convertie, on peut la post-traiter (ajouter de l'information : arretes de diffractions par ex)
    _rayTracing.getSolver()->postTreatmentScene(_rayTracing.getScene(), _rayTracing.getSources(), _rayTracing.getRecepteurs());

    _rayTracing.getScene()->finish(tympan::SolverConfiguration::get()->Accelerator);

#ifdef TEST_ACCELERATION_RECEPTORS
    _rayTracing.get_receptors_landscape()->finish(1, leafTreatment::ALL); // Using grid accelerator
#endif

    ////////////////////////////////////
    // Propagation des rayons
    ////////////////////////////////////

    _rayTracing.setEngine(); //DEFAULT
    _rayTracing.launchSimulation();          //Traitement monothread

    // This function creates TYRays from Rays .
    convert_Rays_to_acoustic_path(sens);

    // This function corrects distances between events and angles at each event
    sampleAndCorrection();

    return true;
}

unsigned int TYANIME3DAcousticPathFinder::getTabsSAndR(vector<vec3>& sources, vector<vec3>& recepteurs, vector<tympan::VolumeFaceDirectivity*>& directivities)
{
    tympan::LPSolverConfiguration config = tympan::SolverConfiguration::get();

    unsigned int sens = config->RayTracingOrder;

    // Recuperation de la position des sources du projet
    vector<vec3> srcs;
	directivities = vector<tympan::VolumeFaceDirectivity*>(_aproblem.nsources(),nullptr);
	vector<tympan::VolumeFaceDirectivity*> dirs = vector<tympan::VolumeFaceDirectivity*>(_aproblem.nsources(),nullptr);
    //Conversion des sources Tympan en source lancer de rayons
    for (unsigned int i = 0; i < _aproblem.nsources(); i++)
    {
        OPoint3D globalPos = _aproblem.source(i).position;
        vec3 pos = vec3(globalPos._x, globalPos._y, globalPos._z);
        srcs.push_back(pos);

		dirs.at(i) = dynamic_cast<tympan::VolumeFaceDirectivity*>(_aproblem.source(i).directivity);
		
    }
    // Recuperation de la position des recepteurs du projet
    vector<vec3> rcpts;

    //Conversion du recepteur Tympan en recepteur lancer de rayons
    for (unsigned int i = 0; i < _aproblem.nreceptors(); i++)
    {
        OPoint3D globalPos = _aproblem.receptor(i).position;
        vec3 pos = vec3(globalPos._x, globalPos._y, globalPos._z);

        rcpts.push_back(pos);
    }

    // Choix du traitement S->R ou R->S
    switch (config->RayTracingOrder)
    {
        case 0 :
            sources = srcs;
            recepteurs = rcpts;
			directivities = dirs;
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
				directivities = dirs;
                sens = 0;
            }
        default :
            sources = srcs;
            recepteurs = rcpts;
			directivities = dirs;
            break;
    }
    return sens;
}

bool TYANIME3DAcousticPathFinder::appendTriangleToScene()
{
    if (_tabPolygon == NULL || _tabPolygonSize <= 0)
    {
        return false;
    }

    Scene* scene = _rayTracing.getScene();

    Material *m = new Material(); // Only for compatibility, may be suppressed;

    vec3 pos;

    for (unsigned int i = 0; i < _tabPolygonSize; i++)
    {
        //Recuperation et convertion de la normale de la surface
        
        unsigned int a, b, c;
        double coord[3];

        _tabPolygon[i].tabPoint[0].getCoords(coord);
        pos = transformer->fonction_h( OPoint3Dtovec3(coord) );
        scene->addVertex(pos, a);

        _tabPolygon[i].tabPoint[1].getCoords(coord);
        pos = transformer->fonction_h( OPoint3Dtovec3(coord) );
        scene->addVertex(pos, b);

        _tabPolygon[i].tabPoint[2].getCoords(coord);
        pos = transformer->fonction_h( OPoint3Dtovec3(coord) );
        scene->addVertex(pos, c);

        //Triangle* face;
        if ( dynamic_cast<tympan::AcousticGroundMaterial*>(_tabPolygon[i].material) )
        {
            // Set last parameter true means triangle is part of the ground
            //face = (Triangle*)scene->addTriangle(a, b, c, m, true);
            (Triangle*)scene->addTriangle(a, b, c, m, true);
        }
        else
        {
            //face = (Triangle*)scene->addTriangle(a, b, c, m);
            (Triangle*)scene->addTriangle(a, b, c, m);
            ss << "Ajout d'un triangle non naturel." << std::endl;
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
        Recepteur recep( transformer->fonction_h(recepteurs[i]), tympan::SolverConfiguration::get()->SizeReceiver );
        recep.setId(idRecepteur);

        _rayTracing.addRecepteur(recep);
        idRecepteur++;
    }
}

void TYANIME3DAcousticPathFinder::appendSourceToSimulation(vector<vec3>& sources,vector<tympan::VolumeFaceDirectivity*>& directivities)
{
    tympan::LPSolverConfiguration config = tympan::SolverConfiguration::get();
    //Conversion des sources Tympan en sources lancer de rayons
    int idSource = 0;
    int nbRaysPerSource = config->NbRaysPerSource;
    int realNbRaysPerSource = 0;
    for (unsigned int i = 0; i < sources.size(); i++)
    {
        Source source;

		if(directivities.at(i)){ // special case when the source's directivity is a VolumeFaceDirectivity -> attach a BeamSampler with a 180° opening angle
			vec3 dir = vec3(directivities.at(i)->get_normal()._x,directivities.at(i)->get_normal()._y,directivities.at(i)->get_normal()._z);

			source.setSampler(new UniformBeamSampler(nbRaysPerSource,(decimal)M_PI,dir));
			realNbRaysPerSource = dynamic_cast<UniformBeamSampler*>(source.getSampler())->getRealNbRays();;
			
		}else{					// general case -> attach the sampler setup in the configuration

			switch (config->Discretization)
			{
				case 0 :
					source.setSampler(new RandomSphericSampler(nbRaysPerSource));
					realNbRaysPerSource = nbRaysPerSource;
					break;
				case 1 :
					source.setSampler(new UniformSphericSampler(nbRaysPerSource));
					realNbRaysPerSource = dynamic_cast<UniformSphericSampler*>(source.getSampler())->getRealNbRays();
					break;
				case 2 :
					source.setSampler(new UniformSphericSampler2(nbRaysPerSource));
					realNbRaysPerSource = dynamic_cast<UniformSphericSampler2*>(source.getSampler())->getRealNbRays();
					break;
				case 3 :
					source.setSampler(new Latitude2DSampler(config->NbRaysPerSource));
					realNbRaysPerSource = nbRaysPerSource;
					dynamic_cast<Latitude2DSampler*>(source.getSampler())->setStartPhi(0.);
					dynamic_cast<Latitude2DSampler*>(source.getSampler())->setEndPhi(360.);
					dynamic_cast<Latitude2DSampler*>(source.getSampler())->setStartTheta(0.);
					dynamic_cast<Latitude2DSampler*>(source.getSampler())->setEndTheta(0.);
			}
		}

        source.setPosition( transformer->fonction_h(sources[i]) );
        source.setInitialRayCount(realNbRaysPerSource);
        source.setId(idSource);

        ss << "Ajout d'une source (id=" << idSource << ") en (" << sources[i].x << "," << sources[i].y << "," << sources[i].y << ")" << endl;

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

        tyRay->build_links_between_events();
        tyRay->compute_shot_angle();

        // Ajoute le rayon au calcul
        _tabTYRays.push_back(tyRay);
    }
}

void TYANIME3DAcousticPathFinder::sampleAndCorrection()
{
    tympan::LPSolverConfiguration config = tympan::SolverConfiguration::get();
    double t_step(config->AnalyticH);
    double c0(_atmos.compute_c());
    acoustic_path::set_sampler_step(c0 * t_step);

    for (size_t i = 0; i < _tabTYRays.size(); i++)
    {
        // Compute distance to the next event
        _tabTYRays.at(i)->nextLenghtCompute(transformer.get());

        // Compute distance to the next "pertinent" event
        _tabTYRays.at(i)->endLenghtCompute(transformer.get());

        // Compute distance between previous and next event for each event
        _tabTYRays.at(i)->prevNextLengthCompute(transformer.get());

        // Compute incident angles for each event
        _tabTYRays.at(i)->angleCompute(transformer.get());

        // Compute events position in real world
        _tabTYRays.at(i)->eventPosCompute(transformer.get());
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
    assert (static_cast<unsigned int>(idRecep) < _aproblem.nreceptors() && static_cast<unsigned int>(idSource) < _aproblem.nsources());

    tyRay->setSource(idSource); 
    tyRay->setRecepteur(idRecep); 
}

void TYANIME3DAcousticPathFinder::build_geometry_transformer( const vector<vec3>& sources )
{
    tympan::LPSolverConfiguration config = tympan::SolverConfiguration::get();

    // Geometry transformer choice
    if (config->UseMeteo == false)
    {
        transformer = std::unique_ptr<IGeometryModifier>( new geometry_modifier_no_correction() ) ;    
    }
    else
    {
        switch(config->AnalyticTypeTransfo)
        {
        case 1 :
        default:
            transformer = std::unique_ptr<IGeometryModifier>( new geometry_modifier_z_correction() ) ;
            break;
        }
    }

    transformer->clear();

    if (config->UseMeteo)
    {
        // Creation du lancer de rayons courbes
        Lancer CurveRayShot;

        // Parametrage du lancer de rayons courbe
        CurveRayShot.clear();
        CurveRayShot.setDMax(config->AnalyticDMax);

        CurveRayShot.setTMax(config->AnalyticTMax);
        CurveRayShot.setTimeStep(config->AnalyticH); // Propagation time step
        CurveRayShot.setNbRay(config->AnalyticNbRay);
        dynamic_cast<meteoLin*>(CurveRayShot._weather)->setGradC(config->AnalyticGradC);
        dynamic_cast<meteoLin*>(CurveRayShot._weather)->setGradV(config->AnalyticGradV);
        CurveRayShot._weather->setWindAngle(config->WindDirection);
        CurveRayShot._weather->setC0(_atmos.compute_c());
        CurveRayShot.setLaunchType(1); // Indique que l'on tire les rayons sur un plan horizontal

        dynamic_cast<Latitude2DSampler*>(CurveRayShot.getSampler())->setStartPhi(config->InitialAnglePhi);

        dynamic_cast<Latitude2DSampler*>(CurveRayShot.getSampler())->setStartTheta(config->InitialAngleTheta);

//        CurveRayShot.initialAngleTheta = config->InitialAngleTheta;     // Angle de tir vertical (theta) des rayons

        // Choix de la source
        vec3 p = OPoint3Dtovec3( tympan::ComputeAcousticCentroid( _aproblem.sources() ) );
        CurveRayShot.addSource(p);   // source is the centroid of all sources taking account of acoustic power 

        // Lancer des rayons
        CurveRayShot.run();

        transformer->buildNappe(CurveRayShot);
    }
}

void TYANIME3DAcousticPathFinder::configure_raytracer()
{
    tympan::LPSolverConfiguration solver_config = tympan::SolverConfiguration::get();
    AcousticRaytracerConfiguration *raytracer_config = AcousticRaytracerConfiguration::get();

    raytracer_config->NbRaysPerSource = solver_config->NbRaysPerSource;
    raytracer_config->Discretization = solver_config->Discretization;
    raytracer_config->Accelerator = solver_config->Accelerator;
    raytracer_config->MaxTreeDepth = solver_config->MaxTreeDepth;
    raytracer_config->MaxProfondeur = solver_config->MaxProfondeur;
    raytracer_config->MaxReflexion = solver_config->MaxReflexion;
    raytracer_config->UseSol = solver_config->UseSol;
    raytracer_config->MaxDiffraction = solver_config->MaxDiffraction;
    raytracer_config->NbRayWithDiffraction = solver_config->NbRayWithDiffraction;
    raytracer_config->MaxLength = solver_config->MaxLength;
    raytracer_config->SizeReceiver = solver_config->SizeReceiver;
    raytracer_config->AngleDiffMin = solver_config->AngleDiffMin;
    raytracer_config->CylindreThick = solver_config->CylindreThick;
    raytracer_config->MaxPathDifference = solver_config->MaxPathDifference;
    raytracer_config->InitialAnglePhi = solver_config->InitialAnglePhi;
    raytracer_config->FinalAnglePhi = solver_config->FinalAnglePhi;
    raytracer_config->InitialAngleTheta = solver_config->InitialAngleTheta;
    raytracer_config->FinalAngleTheta = solver_config->FinalAngleTheta;
    raytracer_config->UsePathDifValidation = solver_config->UsePathDifValidation;
    raytracer_config->DiffractionFilterRayAtCreation = solver_config->DiffractionFilterRayAtCreation;
    raytracer_config->DiffractionUseDistanceAsFilter = solver_config->DiffractionUseDistanceAsFilter;
    raytracer_config->DiffractionUseRandomSampler = solver_config->DiffractionUseRandomSampler;
    raytracer_config->DiffractionDropDownNbRays = solver_config->DiffractionDropDownNbRays;
    raytracer_config->UsePostFilter = solver_config->UsePostFilters;
    raytracer_config->DebugUseCloseEventPostFilter = solver_config->DebugUseCloseEventSelector;
    raytracer_config->DebugUseDiffractionAngleSelector = solver_config->DebugUseDiffractionAngleSelector;
    raytracer_config->DebugUseDiffractionPathSelector = solver_config->DebugUseDiffractionPathSelector;
    raytracer_config->DebugUseFermatSelector = solver_config->DebugUseFermatSelector;
    raytracer_config->DebugUseFaceSelector = solver_config->DebugUseFaceSelector;
    raytracer_config->KeepDebugRay = solver_config->KeepDebugRay;
}
