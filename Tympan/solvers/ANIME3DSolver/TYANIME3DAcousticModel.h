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

#ifndef __TYANIME3DACOUSTICMODEL__
#define __TYANIME3DACOUSTICMODEL__

#include "Tympan/core/interfaces.h"
#include "Tympan/solvers/ANIME3DSolver/TYANIME3DSolver.h"

// Pour l'utilisation de la librairie standard STL.

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

class OBox2;

/**
 * \class TYANIME3DAcousticModel
 * \brief Le modle acoustique de la mthode ANIME3D
 */
class TYANIME3DAcousticModel : public AcousticModelInterface
{
public:
    /// constructeurs
    TYANIME3DAcousticModel(TYCalcul& calcul, const TYSiteNode& site, tab_acoustic_path& tabRayons, TYStructSurfIntersect* tabStruct,
                           TYTabSourcePonctuelleGeoNode& tabSources, TYTabPointCalculGeoNode& tabRecepteurs);

    /// destructeur
    virtual ~TYANIME3DAcousticModel();

    /**
     * \fn computeAcousticModel(TYCalcul & calcul, TYSiteNode & site)
     * \brief calcul complet lie au modele acoustique ANIME3D
     */
    OTab2DSpectreComplex ComputeAcousticModel();

    /**
     * \fn void ComputePressionAcoustEff(TYCalcul & calcul, const TYSiteNode & site)
     * \brief calcul de la pression acoustique efficace par rayon et par frequence
     */
    void ComputePressionAcoustEff();

    /**
     *\fn  void ComputePressionAcoustTotalLevel(TYCalcul & calcul, const TYSiteNode & site)
     *\brief calcul de la pression quadratique totale pour un couple source/recepteur - calcul avec coherence partielle
     */
    OTab2DSpectreComplex ComputePressionAcoustTotalLevel();


protected:

    /**
     * \fn void ComputeAbsAtm();
     * \brief calcul de l'absorption atmospherique
     */
    void ComputeAbsAtm();

    /**
     * \fn void ComputeAbsRefl(TYCalcul & calcul, TYSiteNode & site)
     * \brief calcul de l'absorption par reflexion
     */
    void ComputeAbsRefl();

    /**
     * \fn OBox ComputeFresnelArea(TYCalcul & calcul,TYSiteNode & site, TYRay & ray)
     * \brief calcul des triangles de la zone de Fresnel
     */
    OBox2 ComputeFresnelArea(double angle, OPoint3D Pprec, OPoint3D Prefl, OPoint3D Psuiv, int rayNbr, int reflIndice);

    /**
     * \fn  OTabDouble ComputeFresnelWeighting(double angle, OPoint3D Pprec, OPoint3D Prefl, OPoint3D Psuiv, int rayNbr, int reflIndice, TYTabPoint3D& triangleCentre );
     * \brief calcul des ponderations de Fresnel associees a la zone de Fresnel
     */
    OTabDouble ComputeFresnelWeighting(double angle, OPoint3D Pprec, OPoint3D Prefl, OPoint3D Psuiv, int rayNbr, int reflIndice, TYTabPoint3D& triangleCentre);

    /**
     * \fn  std::vector<OTriangle> ComputeTriangulation(const TYTabPoint& points, const double& delaunay);
     * \brief Computes triangulation inside the bounding box
     * \brief Needs 4 points which are the box corners' projection on the ground
     */
    std::vector<OTriangle> ComputeTriangulation(const TYTabPoint& points, const double& delaunay);

    /**
    * \fn void ComputeAbsDiff(TYCalcul & calcul, const TYSiteNode & site)
    * \brief calcul de l'absorption par diffraction
    */
    void ComputeAbsDiff();



private :
    /*
     * \fn OSpectre computeFc(const double& dd, const double& dr);
     * \brief compute transition frequency spectrum fc
     */
    OSpectre computeFc(const double& dd, const double& dr);

protected :

    // Test : vector de triangles
    std::vector<OTriangle> _oTriangles;
    //std::vector<OPoint3D>& _oVertex;
    QList<OTriangle> triangles;

    TYAltimetrie* _alti;

    TYCalcul& _calcul;
    const TYSiteNode& _site;

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

    /// tableau de l'ensemble des rayons TYMPAN
    tab_acoustic_path& _tabTYRays;

    TYStructSurfIntersect* _tabSurfIntersect; /*!< Tableau contenant l'ensemble des infos relatives a la geometrie d'un site et les materiaux associes a chaque face */

    /// l'atmosphere
    TYAtmosphere _atmos;

    /// la topographie du site
    TYTopographie* _topo;

    /// la liste des terrains du site
    TYTabTerrainGeoNode _listeTerrains;

    /// la liste des triangles de la topo
    TYTabLPPolygon _listeTriangles;

    /// la liste des triangles de la topo
    TYTabLPPolygon _listeTrianglesBox;

    /*!< List of sources used by the solver */
    TYTabSourcePonctuelleGeoNode& _tabSources;

    /*!< List of receptors used by the solver */
    TYTabPointCalculGeoNode& _tabRecepteurs;

    /// vitesse du son
    double _c;

    /// nombre d'onde
    OSpectre _K;

    /// longueur d'onde
    OSpectre _lambda;

    /// take into account the fresnel area
    bool _useFresnelArea;
};

#endif // __TYANIME3DACOUSTICMODEL__
