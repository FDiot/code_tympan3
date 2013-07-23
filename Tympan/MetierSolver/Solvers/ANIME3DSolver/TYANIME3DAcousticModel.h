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
* EDF-R&D Dpartement AMA - 1, avenue du gnral de Gaulle, 92141 CLAMART
*/

/**
 * \file TYANIME3DAcousticModel.h
 * \brief Le modle acoustique de la methode ANIME3D (header)
 * \author Projet_Tympan
 * \date 04 avril 2011
 */

#ifndef __TYANIME3DACOUSTICMODEL__
#define __TYANIME3DACOUSTICMODEL__

#include "Tympan/MetierSolver/DataManagerCore/TYAcousticModelInterface.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Engine/Simulation.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYCalcul.h"

// Pour l'utilisation de la librairie standard STL.
#include <map>
#include <list>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>

#include "TYANIME3DAcousticPathFinder.h"
#include "Tympan/MetierSolver/ToolsMetier/OBox2.h"

///Type pour les valeurs complexes
typedef std::complex<double> OComplex;

///Type pour les tableaux de doubles
typedef std::vector<double> OTabDouble;

///Type pour les tableaux de complexes
typedef std::vector<OComplex> OTabComplex;

///Type pour les tableaux de spectres complexes
typedef std::vector<OSpectreComplex> OTabSpectreComplex;

///Type pour les tableaux de points
typedef std::vector<OPoint3D> OTabPoints;

///Type pour les tableaux de rays
typedef std::vector<Ray*> TabRays;

///Type pour les tableaux de rayons TYMPAN
typedef std::vector<TYRay> TYTabRays;

///Type pour les tableaux d'evenements
typedef std::vector<TYRayEvent> TYTabRayEvent;

//Structure permettant de stocker les informations de Tympan dans un format aisement convertible
//Les infos idFace, idBuilding, idEtage, spectreAbsoMat, G ne sont pas obligatoires.
//De meme, il est possible de rajouter des informations lors de la collecte pour repondre a un besoin d'une methode acoustique
//Exemple : le dev peut choisir de conserver la resistivite plutot que le G.
struct TYStructSurfIntersect
{
    TYAcousticSurfaceGeoNode* pSurfGeoNode; //Geonode de la surface
    OMatrix matInv;                         // Matrice inverse pour les faces d'infrastructure
    TYTabPoint tabPoint;                    // Tableau de point utilise pour la preselection
    bool isEcran;                           // Est un ecran
    bool isInfra;                           // Face d'infrastructure
    OVector3D normal;                       //Normale de la surface
    QList<OTriangle> triangles;             //Ensemble des triangles contenus dans la surface apres triangulation
    QList<OPoint3D> realVertex;              //Ensemble des vertex de la surface en coordonn�es globales.
    int idFace;                             //Indice de la face
    int idBuilding;                         //Indice du building dans TYSite
    int idEtage;                            //Indice de l'etage
    double* spectreAbso;			        //Spectre d'absorption
    double G;                               //Coefficient d'impedance
};

class OBox2;

/**
 * \class TYANIME3DAcousticModel
 * \brief Le modle acoustique de la mthode ANIME3D
 */
class TYANIME3DAcousticModel : public TYAcousticModelInterface
{
public:
    /// constructeurs
	TYANIME3DAcousticModel();

	TYANIME3DAcousticModel(TYCalcul& calcul, const TYSiteNode& site, TabRays* tabRayons, TYStructSurfIntersect* tabStruct);

    /// destructeur
    virtual ~TYANIME3DAcousticModel();

    /**
     * \fn computeAcousticModel(TYCalcul & calcul, TYSiteNode & site)
     * \brief calcul complet lie au modele acoustique ANIME3D
     */
    OTab2DSpectreComplex ComputeAcousticModel(TYCalcul& calcul, const TYSiteNode& site);

    /**
     * \fn void ComputePressionAcoustEff(TYCalcul & calcul, const TYSiteNode & site)
     * \brief calcul de la pression acoustique efficace par rayon et par frequence
     */
    void ComputePressionAcoustEff(TYCalcul& calcul, const TYSiteNode& site);

    /**
     *\fn  void ComputePressionAcoustTotalLevel(TYCalcul & calcul, const TYSiteNode & site)
     *\brief calcul de la pression quadratique totale pour un couple source/recepteur - calcul avec coherence partielle
     */
    OTab2DSpectreComplex ComputePressionAcoustTotalLevel(TYCalcul& calcul, const TYSiteNode& site);


    /*!
    * \fn LPTYRay convertRaytoTYRay()
    * \brief convertion du rayon en rayon TYMPAN
    */
    //LPTYRay convertRaytoTYRay(Ray *r, TYCalcul &calcul, const TYSiteNode &site);

	/**
     * \fn void init(TYCalcul & calcul);
     * \brief initialisation du calcul
     */
    void init(TYCalcul&, const TYSiteNode&, TabRays*, TYStructSurfIntersect*);

protected:
  

    /**
     * \fn void ComputeAbsAtm();
     * \brief calcul de l'absorption atmosphrique
     */
    void ComputeAbsAtm();

    /**
     * \fn void ComputeAbsRefl(TYCalcul & calcul, TYSiteNode & site)
     * \brief calcul de l'absorption par reflexion
     */
    void ComputeAbsRefl();

    /**
     * \fn OBox ComputeFrenelArea(TYCalcul & calcul,TYSiteNode & site, TYRay & ray)
     * \brief calcul des triangles de la zone de Fresnel
     */
    OBox2 ComputeFrenelArea(TYRay* ray, double angle, double distPrefPsuiv, OPoint3D Pref);

    /**
     * \fn OTabDouble ComputeFrenelWeighting(TYCalcul & calcul, TYSiteNode & site);
     * \brief calcul des ponderations de Fresnel associees a la zone de Fresnel
     */
    OTabDouble ComputeFrenelWeighting(TYRay* ray, double angle, double distPrefPsuiv, OPoint3D Pref);


	//TYMateriauConstruction TYANIME3DAcousticModel::getMateriauFace(Shape* pSurf, const vec3& seg) const;

	 /**
     * \fn void ComputeAbsDiff(TYCalcul & calcul, const TYSiteNode & site)
     * \brief calcul de l'absorption par diffraction
     */
    void ComputeAbsDiff(TYCalcul& calcul, const TYSiteNode& site);

    /// tableau des pressions acoustiques efficaces par rayon
    OTabSpectreComplex _pressAcoustEff;

    ///tableau des absorptions atmospherique par rayon
    OTabSpectreComplex _absAtm;

    ///tableau des absorptions par reflexion par rayon
    OTabSpectreComplex _absRefl;

    ///tableau des absorptions par diffraction par rayon
    OTabSpectreComplex _absDiff;

    /// nbr de rayons
    int _nbRays;

    /// tableau de l'ensemble des rayons
    TabRays* _tabRays;

    /// tableau de l'ensemble des rayons TYMPAN
    TYTabRay _tabTYRays;

    /// la methode de recherche de chemins acoustiques
    //TYANIME3DAcousticPathFinder _acoustPathFinder;

	TYStructSurfIntersect* _tabSurfIntersect; /*!< Tableau contenant l'ensemble des infos relatives a la geometrie d'un site et les materiaux associes a chaque face */

	/// l'atmosphere
	TYAtmosphere _atmos; 

	/// la topographie du site
	TYTopographie _topo;

	/// la liste des terrains du site
	TYTabTerrainGeoNode _listeTerrains; 

	/// la liste des triangles de la topo
    TYTabLPPolygon _listeTriangles;  

	/// la liste des triangles de la topo
    TYTabLPPolygon _listeTrianglesBox;  

	/// le tableau de recepteurs de la scene
	TYTabPointCalculGeoNode _tabRecepteur;

	/// le tableau de sources de la scene 
	TYTabSourcePonctuelleGeoNode _tabSources;

	double _c;	// vitesse du son
	OSpectre _K;	// nombre d'onde
	OSpectre _lambda;	// longueur d'onde
};

#endif // __TYANIME3DACOUSTICMODEL__
