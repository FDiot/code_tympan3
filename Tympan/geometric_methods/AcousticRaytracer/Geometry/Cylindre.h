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

#ifndef CYLINDRE_H
#define CYLINDRE_H

#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Shape.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Repere.h"

class Ray;
/**
 * \brief Cylinder class
 */
class Cylindre : public Shape
{
public:
    /// Constructors
    Cylindre(Shape* _p1 = NULL, Shape* _p2 = NULL, std::vector<vec3>* _vertices = NULL, unsigned int v1 = 0, unsigned int v2 = 1, decimal _epaisseur = 0.2);
    Cylindre(const Cylindre& other);
    /// Destructor
    virtual ~Cylindre() {};
    /// A cylinder is not visible
    virtual bool isVisible() { return false;}

    virtual bool getIntersection(Ray& ray, Intersection& inter);

    void setAngleOuverture(decimal angle) { angleOuverture = angle; }
    /// Get opening angle
    decimal getAngleOuverture() { return angleOuverture; }

    /// Get the first shape
    Shape* getFirstShape() { return p1; }
    /// Get the second shape
    Shape* getSecondShape() { return p2; }

    /// Uncommented cause not used:
    virtual bool sample(decimal density, std::vector<vec3>& samples);
    virtual int form() { return CYLINDRE; }


protected:

    void computeMesh();

    Shape* p1;	//!< First shape of the cylinder
    Shape* p2;	//!< Second shape of the cylinder

    decimal epaisseur;			//!< Width
    decimal hauteur;			//!< Height
    decimal angleOuverture;		//!< Opening angle

    Repere localRepere;			//!< Local frame

    vector<vec3> points;		//!< Array of points defining the mesh
    vector<Shape*> mesh;		//!< Cylinder mesh (array of triangles)
};

#endif
