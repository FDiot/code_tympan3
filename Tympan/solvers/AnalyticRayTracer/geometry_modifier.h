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

#ifndef __GEOMETRY_MODIFIER_H
#define __GEOMETRY_MODIFIER_H

#include <qlist.h>

#include "Tympan/models/common/3d.h"
#include "Tympan/models/common/triangle.h"
#include "Tympan/models/common/mathlib.h"
#include "Tympan/solvers/AcousticRaytracer/Geometry/Scene.h"

class Lancer;

using namespace std;

class IGeometryModifier
{
public:
    virtual ~IGeometryModifier() {};

    /*!
    * \fn void Init()
    * \brief Efface tous les tableaux.
    */
    virtual void clear() = 0;

    /*!
     * \fn void trianguleNappe()
     * \brief creation de la nappe de rayons triangulee pour l'interpolation
     */
    virtual void buildNappe(const Lancer& shot) = 0;

    /*!
    * \fn vec3 fonction_h (vec3 P)
    * \brief fonction de transformation.
    * \param P point que l'on desire transformer
    * \return rend les coordonnees du point transforme.
    */
    virtual vec3 fonction_h(const vec3& P) = 0;

    /*!
    * \fn vec3 fonction_h_inverse (vec3 P, QList<OTriangle> Liste_triangles)
    * \brief fonction inverse de la fonction de transformation.
    * \param P point que l'on desire transformer
    * \param Liste_triangles liste des triangles de la geometrie
    * \return rend les coordonnees du point transforme (point de l'espace original).
    */
    virtual vec3 fonction_h_inverse(const vec3& P) = 0;
};

class geometry_modifier_z_correction :
    public IGeometryModifier
{
public:

    // Constructeurs :
    geometry_modifier_z_correction() { _scene = std::unique_ptr<Scene>( new Scene() ); }

    // Destructeur :
    ~geometry_modifier_z_correction() {}

    virtual void clear();

    virtual void buildNappe(const Lancer& shot);

    virtual vec3 fonction_h(const vec3& P);

    virtual vec3 fonction_h_inverse(const vec3& P);

private :
    /*!
     * \fn double interpo(const vec3* triangle, vec3 P);
     * \brief return z position of point (P) inside a triangle
     */
    double interpo(const vec3* triangle, vec3 P);

    /*!
     * \fn const scene* get_scene();
     * \brief return the scene
     */
    const Scene* get_scene() { return _scene.get(); }

    void append_triangles_to_scene();
    double compute_h(const vec3& P);

    vec3 pos_center;                    /*!< Position de la source */

    QList<OTriangle> Liste_triangles;   /*!< Liste des triangles de la nappe interpolee */
    QList<OPoint3D> Liste_vertex;       /*!< Liste des vertex de la triangulation */
    std::unique_ptr<Scene> _scene;      /*!< Support de la structure acceleratrice pour la nappe */
};

class geometry_modifier_spherical_correction :
    public IGeometryModifier
{
public:
    geometry_modifier_spherical_correction() {}
    ~geometry_modifier_spherical_correction() {}

    virtual void clear();

    virtual void buildNappe(const Lancer& shot);

    virtual vec3 fonction_h(const vec3& P);

    virtual vec3 fonction_h_inverse(const vec3& P);

private:
};

bool IsInTriangle(const vec3& P, const vec3* triangle);

#endif //__GEOMETRY_MODIFIER_H
