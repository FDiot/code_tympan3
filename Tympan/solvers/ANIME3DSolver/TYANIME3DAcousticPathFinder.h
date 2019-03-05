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

#include "Tympan/solvers/ANIME3DSolver/TYANIME3DSolver.h"
#include "Tympan/geometric_methods/AnalyticRayTracer/geometry_modifier.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Engine/Simulation.h"

class AtmosphericConditions;

/**
 * \class TYANIME3DAcousticPathFinder
 * \brief Research of acoustic paths for the ANIME3D method
 */
class TYANIME3DAcousticPathFinder
{
public:
    //TYANIME3DAcousticPathFinder(    TYStructSurfIntersect* tabPolygon, 
    //                                const size_t& tabPolygonSize, 
    //                                TYTabSourcePonctuelleGeoNode& tabSources, 
    //                                TYTabPointCalculGeoNode& tabRecepteurs,
    //                                const tympan::AcousticProblemModel& aproblem_,
    //                                tab_acoustic_path& tabTYRays);
    /**
     * \brief Constructor
     * \param tabPolygon Array containing all the informations relative to a site geometry and associated material to each face
     * \param tabPolygonSize Size of the previous array
     * \param aproblem_ Acoustic problem
     * \param tabTYRays Array containing the acoustic paths for the rays
     * \param atmos Atmospheric conditions object
     */
    TYANIME3DAcousticPathFinder(    TYStructSurfIntersect* tabPolygon, 
                                    const size_t& tabPolygonSize, 
                                    const tympan::AcousticProblemModel& aproblem_,
                                    tab_acoustic_path& tabTYRays,
                                    AtmosphericConditions& atmos);

    /// Destructor
    virtual ~TYANIME3DAcousticPathFinder();

    /// Launch the research of acoustic paths
    bool exec();

    /// Get ray tracing object
    Simulation& getRayTracer() { return _rayTracing; }

    /// Get the geometry modifier
    IGeometryModifier* get_geometry_modifier() { return transformer.get(); }

private :
    /*!
     * \fn unsigned int getTabsSAndR(const TYSite& site, TYCalcul& calcul, vector<OCoord3D>& sources, vector<OCoord3D>& recepteurs)
     * \brief Contruit la liste des points sources et des points recepteurs en fonction du parametre de sens de propa (S -> R) ou (R -> S)
     * \return Renvoie 0 pour le sens S->R et 1 pour le sens R->S
     */
    unsigned int getTabsSAndR(vector<vec3>& sources, vector<vec3>& recepteurs,vector<tympan::VolumeFaceDirectivity*>& directivities);

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
    void appendSourceToSimulation(vector<vec3>& sources,vector<tympan::VolumeFaceDirectivity*>& directivities);

    /*!
    * \fn  void convert_Rays_to_acoustic_path(const unsigned int& sens)
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
     * \fn void set_source_idx_and_receptor_idx_to_acoustic_path();
     */
    void set_source_idx_and_receptor_idx_to_acoustic_path(int sens, Ray *ray, acoustic_path *tyRay);

private:
    /// Method to build the transformer of the scene geometry according to different parameters 
    void build_geometry_transformer( const vector<vec3>& sources );

    /// Configure parameters of the AcousticRayTracer
    void configure_raytracer();
    /// Object _rayTracing for the straigth-line ray tracer
    Simulation _rayTracing;

    /// Object _curveRayTracing for the curved ray tracer
    std::unique_ptr<IGeometryModifier> transformer;

    /// Array containing all the informations relative to a site geometry and associated material to each face
    TYStructSurfIntersect* _tabPolygon;

    /// Polygons number in _tabPolygon
    const size_t& _tabPolygonSize;

    /// Array containing all the Code_Tympan rays
    tab_acoustic_path& _tabTYRays;

    /// Atmospheric conditions
    AtmosphericConditions& _atmos;

    const tympan::AcousticProblemModel& _aproblem;
};

#endif // __TYANIME3DACOUSTICPATHFINDER__
