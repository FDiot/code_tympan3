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

/**
 * \brief Class to modify the scene geometry
 */
class IGeometryModifier
{
public:
	/// Destructor
    virtual ~IGeometryModifier() {};

    /// Clear all the arrays
    virtual void clear() = 0;

    /*!
     * \brief Pure virtual function to create the triangles mesh built with rays
     */
    virtual void buildNappe(const Lancer& shot) = 0;

    /*!
    * \brief Pure virtual function for a point transformation
    * \param P Point
    * \return The transformed point coordinates
    */
    virtual vec3 fonction_h(const vec3& P) = 0;

    /*!
    * \brief Pure virtual function for an inverse point transformation
    * \param P Point
    * \return The inverse transformed point coordinates (original point coordinates)
    */
    virtual vec3 fonction_h_inverse(const vec3& P) = 0;

    /// Export to a file
    virtual void save_to_file(std::string fileName) = 0;

protected:
    vec3 pos_center;                    //!< Source position
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

    /// Constructor
    geometry_modifier_z_correction() : _scene( std::unique_ptr<Scene>( new Scene() ) ) {}

    /// Destructor
    ~geometry_modifier_z_correction() {}

    virtual void buildNappe(const Lancer& shot);

    virtual vec3 fonction_h(const vec3& P);

    virtual vec3 fonction_h_inverse(const vec3& P);

    virtual void clear() {}

    virtual void save_to_file(std::string fileName) { _scene->export_to_ply(fileName); }

    /// Get the scene
    const Scene* get_scene() { return _scene.get(); }

private :
    void append_triangles_to_scene(QList<OPoint3D>& Liste_vertex, QList<OTriangle>& Liste_triangles);
    double compute_h(const vec3& P);

    std::unique_ptr<Scene> _scene;      //!< Support de la structure acceleratrice pour la nappe
};

#endif //__GEOMETRY_MODIFIER_H
