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

#ifndef __TYANIME3DACOUSTICPATHFINDER__
#define __TYANIME3DACOUSTICPATHFINDER__

#include "Tympan/MetierSolver/AnalyticRayTracer/geometry_modifier.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Engine/Simulation.h"

/**
 * \class TYANIME3DAcousticPathFinder
 * \brief La recherche de chemins acoustiques associee a la methode ANIME3D
 */
class TYANIME3DAcousticPathFinder
{
public:
    TYANIME3DAcousticPathFinder(TYStructSurfIntersect* tabPolygon, const size_t& tabPolygonSize, TYTabSourcePonctuelleGeoNode& tabSources, TYTabPointCalculGeoNode& tabRecepteurs, tab_acoustic_path& tabTYRays);
    virtual ~TYANIME3DAcousticPathFinder();

    bool exec();

    Simulation& getRayTracer() { return _rayTracing; }

    geometry_modifier& get_geometry_modifier() { return transformer; }

private :
    /*!
     * \fn unsigned int getTabsSAndR(const TYSite& site, TYCalcul& calcul, vector<OCoord3D>& sources, vector<OCoord3D>& recepteurs)
     * \brief Contruit la liste des points sources et des points recepteurs en fonction du parametre de sens de propa (S -> R) ou (R -> S)
     * \return Renvoie 0 pour le sens S->R et 1 pour le sens R->S
     */
    unsigned int getTabsSAndR(vector<vec3>& sources, vector<vec3>& recepteurs);

    /*!
     * \fn TYPoint computePosGlobalPoint (const TYGeometryNode* pNode);
     * \brief Calcule la position d'un point (source ou recepteur) dans le repere global
     */
    OPoint3D computePosGlobalPoint(const TYGeometryNode* pNode);

    /*!
     * \fn void transformSEtR(vector<vec3>& sources, vector<vec3>& recepteurs)
     * \brief Adapte la position des sources et des recepteurs dans la geometrie transformee
     */
    void transformSEtR(vector<vec3>& sources, vector<vec3>& recepteurs);

    /*!
    * \fn bool appendTriangleToScene()
    * \brief Convertion des triangles Tympan en primitives utilisables par le lancer de rayons.
    * \return Renvoie vrai si l'ensemble des primitives a bien pu etre importe dans la simulation.
    */
    bool appendTriangleToScene();

    /*!
    * \fn void appendRecepteurToSimulation(vector<vec3>& recepteurs)
    * \brief Ajoute les recepteurs a la simulation
    */
    void appendRecepteurToSimulation(vector<vec3>& recepteurs);

    /*!
    * \fn void appendSourceToSimulation(TYCalcul &calcul, vector<vec3>& sources)
    * \brief Ajoute les sources ponctuelles actives a la simulation
    */
    void appendSourceToSimulation(vector<vec3>& sources);

    /*!
    * \fn  void convertRaytoTYRay(const TYSiteNode& site, TYCalcul& calcul, const unsigned int& sens)
    * \brief convertion des rayons en rayons TYMPAN
    */
    void convert_Rays_to_acoustic_path(const unsigned int& sens);

    /*!
    * \fn void sampleAndCorrection()
    * \brief Computes angle and length correction
    * \ by calling the three previous functions
    * \ Creates two matrix which have corrected lengths and angles
    */
    void sampleAndCorrection();

    /*!
     * \fn void set_metier_source_and_receptor();
     */
    void set_source_idx_and_receptor_idx_to_acoustic_path(int sens, Ray *ray, acoustic_path *tyRay);

private:
    void build_geometry_transformer( const vector<vec3>& sources );

    /// Objet _rayTracing pour le lancer de rayons droits
    Simulation _rayTracing;

    /// Objet _curveRayTracing pour le lancer de rayons courbes
    geometry_modifier transformer;

    /// Tableau contenant l'ensemble des infos relatives a la geometrie d'un site et les materiaux associes a chaque face
    TYStructSurfIntersect* _tabPolygon;

    /// Nombre de polygones presents dans _tabPolygon
    const size_t& _tabPolygonSize;

    /*!< List of sources used by the solver */
    TYTabSourcePonctuelleGeoNode& _tabSources;

    /*!< List of receptors used by the solver */
    TYTabPointCalculGeoNode& _tabRecepteurs;

    /// tableau de l'ensemble des rayons métier Code_TYMPAN
    tab_acoustic_path& _tabTYRays;
};

#endif // __TYANIME3DACOUSTICPATHFINDER__
