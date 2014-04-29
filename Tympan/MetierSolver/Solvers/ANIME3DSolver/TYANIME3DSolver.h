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
 * \brief Le solver associe a la methode ANIME3D (header)
 * \author Projet_Tympan
 * \date 04 avril 2011
 */

#ifndef __TY_ANIME3DSOLVER__
#define __TY_ANIME3DSOLVER__

#include "Tympan/MetierSolver/DataManagerCore/TYSolverInterface.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticSurface.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYCalcul.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYRay.h"

#include "Tympan/MetierSolver/ToolsMetier/OSegment3D.h"
#include "Tympan/MetierSolver/ToolsMetier/ODelaunayMaker.h"

#include "Tympan/MetierSolver/AnalyticRayTracer/Transfo.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Engine/Simulation.h"    //Classe de base pour utiliser le lancer de rayons


class TYANIME3DAcousticModel;
class TYANIME3DAcousticPathFinder;
class TYANIME3DFaceSelector;
struct TYStructSurfIntersect;

class Lancer;
class R3;

/*
 * \class TYANIME3DSolver
 * \brief Le solver associe a la methode ANIME3D
 */
class TYANIME3DSolver : public TYSolverInterface
{
public:
    /**
    * \fn TYANIME3DSolver()
    * \brief Constructeur de l'objet ANIME3DSolver
    */
    TYANIME3DSolver();

    /*!
    * \fn ~TYANIME3DSolver()
    * \brief Destructeur de l'objet ANIME3DSolver
    */
    ~TYANIME3DSolver();

    /*!
    * \fn bool solve(const TYSiteNode&, TYCalcul&)
    * \brief Methode principale de resolution avec la methode ANIME3D
    */
    virtual bool solve(const TYSiteNode& site, TYCalcul& calcul);

    /*!
    * \fn TYAcousticPathFinder* getAcousticPathFinder()
    * \return attribut de la classe _acousticPathFinder
    */
    //TYANIME3DAcousticPathFinder* getAcousticPathFinder() { return _acousticPathFinder; }

    /*!
    * \fn TYAcousticModel* getAcousticModel()
    * \return attribut de la classe _acousticModel
    */
    //TYANIME3DAcousticModel* getAcousticModel() { return _acousticModel; }

protected:
    virtual void createAcousticModel();
    virtual void createAcousticPathFinder();
    virtual void createFaceSelector();

    /*!
    * \fn bool buildCalcStruct(const TYSiteNode& site)
    * \brief Construit un tableau contenant la structure regroupant l'ensemble des informations relatives a une face/triangle du site.
    * Correspond a la fonction du meme nom dans le SolveurHarmonoise avec l'ajout des normales et des faces triangulees dans la structure.
    * \param site Reference vers le site de la scene
    */
    bool buildCalcStruct(const TYSiteNode&, const TYCalcul&);

    /*!
    * \fn bool triangulateConcavePolygon()
    * \brief Effectue une triangulation de l'ensemble des faces de la topographie et des infrastructures.
    * \return Renvoie vrai si les triangulations se sont bien passees.
    */
    bool triangulateConcavePolygon();

    /*!
    * \fn bool appendTriangleToScene()
    * \brief Convertion des triangles Tympan en primitives utilisables par le lancer de rayons.
    * \return Renvoie vrai si l'ensemble des primitives a bien pu etre importe dans la simulation.
    */
    bool appendTriangleToScene();

    /**
     * \fn bool appendTriangleToRayTracerScene()
     * \brief Convertion des triangles Tympan en primitives utilisables par le lancer de rayons.
     * \return Renvoie vrai si l'ensemble des primitives a bien pu etre importe dans la simulation.
     */
    bool appendTriangleToRayTracerScene();

    /*!
    * \fn void appendRecepteurToSimulation(TYCalcul &calcul)
    * \brief Ajoute les recepteurs ponctuels actifs a la simulation
    */
    void appendRecepteurToSimulation(TYCalcul& calcul);

    /*!
    * \fn void appendRecepteurToSimulation(vector<vec3>& recepteurs)
    * \brief Ajoute les recepteurs a la simulation
    */
    void appendRecepteurToSimulation(vector<vec3>& recepteurs);

    /*!
    * \fn void appendSourceToSimulation(const TYSiteNode& site, TYCalcul &calcul, unsigned int nbRayons)
    * \brief Ajoute les sources ponctuelles actives a la simulation
    */
    void appendSourceToSimulation(const TYSiteNode& site, TYCalcul& calcul, unsigned int nbRayons);

    /*!
    * \fn void appendSourceToSimulation(TYCalcul &calcul, vector<vec3>& sources)
    * \brief Ajoute les sources ponctuelles actives a la simulation
    */
    void appendSourceToSimulation(vector<vec3>& sources);

    /*!
     * \fn void transformSEtR(vector<vec3>& sources, vector<vec3>& recepteurs)
     * \brief Adapte la position des sources et des recepteurs dans la geometrie transformee
     */
    void transformSEtR(vector<vec3>& sources, vector<vec3>& recepteurs);

    /*!
    * \fn bool appendRayToTYCalcul(const TYSiteNode& site,TYCalcul& calcul, const unsigned int& sens=0)
    * \brief Convertit les rayons du lancer de rayons en rayons Tympan et les ajoute dans le tableau contenu dans le calcul.
    * \return Renvoie vrai si l'ensemble des rayons a bien ete ajoute au calcul.
    */
    bool appendRayToTYCalcul(const TYSiteNode& site, TYCalcul& calcul, const unsigned int& sens = 0);

    /*!
     * \fn void saveAndOverSampleRay(const double& dMin = 0.0)
     * \brief Transforme les ray en TYRay et ajoute des points intermediaires sur les rayons
     */
    void saveAndOverSampleRay(const TYSiteNode& site, TYCalcul& calcul, const unsigned int& sens = 0, const double& dMin = 0.0);

    /*!
     * \fn void buildListEvent(const int& sens, const Ray ray, std::list<TYEvent>& listEvent)
     * \brief construit une liste d'evenement TYEvent a partir d'un rayon
     */
    void buildListEvent(const int& sens, Ray* ray, std::vector<TYRayEvent>& tabEvents);

    /*!
     * \fn void overSampleTYRay(TYRay* tyRay);
     * \brief Rajoute des evenements aux rayons de type TYRay
     */
    void overSampleTYRay(TYRay* tyRay, const double& dMin);

    /*!
     * \fn  void courbeRayon(TYCalcul& calcul)
     * \brief Courbe les rayon droits lances dans la geometrie courbe pour les restituer dans la geometrie droit
     */
    void courbeRayon(TYCalcul& calcul);

    /*!
     * \fn void calculAnglesRayons(TYCalcul& calcul)
     * \brief Calcule les angles associes aux evenements des rayons
     */
    void calculeAngleTirRayon(TYRay* tyRay);

    /*!
     * \fn TYPoint R3ToTYPoint(const R3& p)
     * \brief convertit un R3 en TYPoint
     */
    TYPoint R3ToTYPoint(const R3& p);

    /*!
     * \fn R3 TYPointToR3(const TYPoint& p)
     * \brief convertit un TYPointR3 en R3
     */
    R3 TYPointToR3(const TYPoint& p);

    /*!
     * \fn bool setSEtR(TYTabSourcePonctuelleGeoNode& sources, TYTabPointCalculGeoNode& tabRecepteurs, const unsigned int sens, TYRay& tyRay)
     * \brief affecte la source et le recepteur d'un rayon de type TYRay
     * \return Renvoie false si la source ou le recepteur ne sont pas trouves
     */
    bool setSEtRToTYRay(TYTabSourcePonctuelleGeoNode& sources, TYTabPointCalculGeoNode& tabRecepteurs, const unsigned int sens, Ray* r, TYRay* tyRay);

    /*!
     * \fn unsigned int getTabsSAndR(const TYSite& site, TYCalcul& calcul, vector<OCoord3D>& sources, vector<OCoord3D>& recepteurs)
     * \brief Contruit la liste des points sources et des points recepteurs en fonction du parametre de sens de propa (S -> R) ou (R -> S)
     * \return Renvoie 0 pour le sens S->R et 1 pour le sens R->S
     */
    unsigned int getTabsSAndR(const TYSiteNode& site, TYCalcul& calcul, vector<vec3>& sources, vector<vec3>& recepteurs);

    /*!
    * \fn void appendRecepteurToAnalyticRayTracer(TYCalcul& calcul, vector<R3>& tabRecepteurs)
    * \brief Ajoute les recepteurs poncutels actifs a la simulation
    */
    void appendRecepteurToAnalyticRayTracer(TYCalcul& calcul, vector<R3>& tabRecepteurs);

    /*!
    * \fn void appendSourceToAnalyticRayTracer(TYCalcul &calcul, unsigned int nbRayons)
    * \brief Ajoute les sources ponctuelles actives a la simulation
    */
    void appendSourceToAnalyticRayTracer(const TYSiteNode& site, TYCalcul& calcul, vector<R3>& tabSources);

    // Desactivated while refactoring the altimetry computation
    //    /*!
    //     *\fn void restitModifiedAlti(TYSiteNode& site)
    //     *\brief Restitue au site la topographie (et infrastructure) transformee
    //     */
    //    void restitModifiedAlti(const TYSiteNode& site); // XXX Problem for refactoring altimetry computation

    /*!
     * \fn vec3 modifGeom(const vec3&)
     * \brief modifie l'altimetrie d'un point en prenant en compte l'effet meteo
     */
    vec3 modifGeom(const vec3& point);

    /*!
     * \fn TYPoint computePosGlobalPoint (const TYGeometryNode* pNode);
     * \brief Calcule la position d'un point (source ou recepteur) dans le repere global
     */
    TYPoint computePosGlobalPoint(const TYGeometryNode* pNode);

    /*
    * \fn void TYANIME3DSolver::print()
    * \brief Sortie des rayons dans un fichier
    */
    void print();

protected:

    TYANIME3DAcousticModel* _acousticModel;
    TYANIME3DAcousticPathFinder* _acousticPathFinder;
    TYANIME3DFaceSelector* _faceSelector;
    TYTabRay _tabRay;

    /// Tableau contenant l'ensemble des infos relatives a la geometrie d'un site et les materiaux associes a chaque face
    TYStructSurfIntersect* _tabPolygon;

    /// Nombre de polygones presents dans _tabPolygon
    size_t _tabPolygonSize;

    /// Objet Simulation pour le lancer de rayons droits
    Simulation _rayTracing;

    /// Objet Lancer pour le lancer de rayons courbes
    Transfo lancer;

    //OThreadPool* _pool;  // utile si threade

    Logger logger;
    FILE logs;
};

#endif // __TY_ANIME3DSOLVER__
