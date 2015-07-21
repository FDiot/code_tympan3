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
#include <string>
#include <memory>

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

    /*!
     * \fn void export()
     * \brief export to a file
     */
    virtual void save_to_file(std::string fileName) = 0;

protected:
    vec3 pos_center;                    /*!< Position de la source */
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class geometry_modifier_no_correction :
    public IGeometryModifier
{
public:
    geometry_modifier_no_correction() : IGeometryModifier() {}
    ~geometry_modifier_no_correction() {}

    virtual void clear() {}

    virtual void buildNappe(const Lancer& shot) {}

    virtual vec3 fonction_h(const vec3& P) { return P; }

    virtual vec3 fonction_h_inverse(const vec3& P) { return P; }

    virtual void save_to_file(std::string fileName) {}
};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class geometry_modifier_z_correction :
    public IGeometryModifier
{
public:

    // Constructeurs :
    geometry_modifier_z_correction() : _scene( std::unique_ptr<Scene>( new Scene() ) ) {}

    // Destructeur :
    ~geometry_modifier_z_correction() {}

    virtual void buildNappe(const Lancer& shot);

    virtual vec3 fonction_h(const vec3& P);

    virtual vec3 fonction_h_inverse(const vec3& P);

    virtual void clear() {}

    virtual void save_to_file(std::string fileName) { _scene->export_to_ply(fileName); }

    /*!
     * \fn const scene* get_scene();
     * \brief return the scene
     */
    const Scene* get_scene() { return _scene.get(); }

private :
    void append_triangles_to_scene(QList<OPoint3D>& Liste_vertex, QList<OTriangle>& Liste_triangles);
    double compute_h(const vec3& P);

    std::unique_ptr<Scene> _scene;      /*!< Support de la structure acceleratrice pour la nappe */
};

#endif //__GEOMETRY_MODIFIER_H
