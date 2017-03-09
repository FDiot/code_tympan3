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

#ifndef SHAPE_H
#define SHAPE_H

#include <string>
#include <vector>

#include "Tympan/models/common/mathlib.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Base.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Ray/Ray.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Acoustic/Material.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/BBox.h"


class Shape;

/*!
* \enum FORM
* \brief Primitives supported by the ray tracing method
*/
enum FORM
{
    TRIANGLE = 0,
    CYLINDRE,
    SPHERE
};

/**
 * \struct _Intersection
 * \brief Intersection struct
 */
typedef struct _Intersection
{
    vec3 normal;
    decimal t;
    Shape* p;
    FORM forme;
} Intersection;

/**
 * \brief base class for shapes (Cylindre, Mesh, Sphere, Triangle,...)
 */
class Shape : public Base
{

public:
	/// Default constructor
    Shape() : Base(), material(NULL) { name = "unknown shape"; }
    /// Constructor by giving a name to the Shape
    Shape(const std::string _name) : material(NULL) { name = _name; }
    /// Copy constructor
    Shape(Shape* other)
    {
        name = std::string(other->name);
    }
    /// Destructor
    virtual ~Shape() { }
    /// Copy constructor
    Shape(const Shape& other) : Base(other)
    {
    }
    /// Clone and return a pointer to a Shape
    virtual Shape* Clone()
    {
        Shape* newShape = new Shape(this);
        return newShape;
    }
    /// Return the pointed material
    Material* getMaterial() { return material; }
    /// Set the material
    void setMaterial(Material* m) { material = m; }
    /// Get the Intersection between a ray and this shape
    virtual bool getIntersection(Ray& ray, Intersection& inter) { cerr << "ERROR : Intersection with a non-type shape." << std::endl; return false; }
    /// Update the bounding box:
    virtual void updateBBox() { };
    /// Return the bounding box
    BBox getBBox() { return box; }

    /// Set/Get global vertices
    void setVertices(std::vector<vec3> *_vertices) { vertices = _vertices; }
    vector<vec3>* getVertices() { return vertices; }

    /// Get local vertices
    vector<unsigned int>* getLocalVertices() { return &localVertices; }

    /// Return visibility
    virtual bool isVisible() { return true;}

    /// Get normal
    virtual vec3 getNormal(const vec3 pos = vec3()) { return vec3();}

    /// Set/Get the primitive id
    void setPrimitiveId(int id) { primitiveId = id; }
    int getPrimitiveId() const { return primitiveId; }

    /// Set/Get the face id
    void setFaceId(int id) { faceId = id; }
    int getFaceId() const { return faceId; }
    /// Set/Get the floor id
    void setEtageId(int id) { etageId = id; }
    int getEtageId() { return etageId; }
    /// Set/Get the building id
    void setBuildingId(int id) { buildingId = id; }
    int getBuildingId() { return buildingId; }
    /// Uncommented cause not used:
    virtual bool sample(decimal density, std::vector<vec3>& samples) { return false; }

    /// Get/Set the flag _isSol (ground or not)
    bool isSol() const { return _isSol; }
    void setIsSol(const bool& isSol) { _isSol = isSol; }

    /// Return type of the shape
    virtual int form() { return -1; }

protected:
    BBox box;                                //!< Bounding box of the shape
    Material* material;                      //!< Pointer to material
    std::vector<vec3> *vertices;             //!< GlobalVertices of the scene
    std::vector<unsigned int> localVertices; //!< Index of the vertices used for this shape.
    int primitiveId;                         //!< Index of the primitive (given by the scene)
    int faceId;                              //!< Index of the face supporting the primitive
    int buildingId;                          //!< Index of the building supporting the primitive (-1 if none)
    int etageId;                             //!< Index of the floor (etage) supporting the primitive   (-1 if none)
    bool _isSol;                             //!< Triangle type (false = non natural, true = ground)
};

#endif
