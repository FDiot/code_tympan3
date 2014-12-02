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
#include "Tympan/solvers/AcousticRaytracer/Base.h"
#include "Tympan/solvers/AcousticRaytracer/Ray/Ray.h"
#include "Tympan/solvers/AcousticRaytracer/Acoustic/Material.h"
#include "Tympan/solvers/AcousticRaytracer/Geometry/BBox.h"


class Shape;

/*!
* \enum FORM
* \brief Ensemble des primitives supportees par le lancer de rayons
*/
enum FORM
{
    TRIANGLE = 0,
    CYLINDRE,
    SPHERE
};

typedef struct _Intersection
{
    vec3 normal;
    decimal t;
    Shape* p;
    FORM forme;
} Intersection;

class Shape : public Base
{

public:
    Shape() : Base(), material(NULL) { name = "unknown shape"; }
    Shape(const std::string _name) : material(NULL) { name = _name; }
    Shape(Shape* other)
    {
        name = std::string(other->name);
    }

    virtual ~Shape() { }

    Shape(const Shape& other) : Base(other)
    {
    }

    virtual Shape* Clone()
    {
        Shape* newShape = new Shape(this);
        return newShape;
    }

    Material* getMaterial() { return material; }
    void setMaterial(Material* m) { material = m; }

    virtual bool getIntersection(Ray& ray, Intersection& inter) { cerr << "ERROR : Intersection with a non-type shape." << std::endl; return false; }

    virtual void updateBBox() { };
    BBox getBBox() { return box; }

    void setVertices(std::vector<vec3> *_vertices) { vertices = _vertices; }
    vector<vec3>* getVertices() { return vertices; }

    vector<unsigned int>* getLocalVertices() { return &localVertices; }

    virtual bool isVisible() { return true;}

    virtual vec3 getNormal(const vec3 pos = vec3()) { return vec3();}

    void setPrimitiveId(int id) { primitiveId = id; }
    int getPrimitiveId() { return primitiveId; }

    void setFaceId(int id) { faceId = id; }
    int getFaceId() const { return faceId; }

    void setEtageId(int id) { etageId = id; }
    int getEtageId() { return etageId; }

    void setBuildingId(int id) { buildingId = id; }
    int getBuildingId() { return buildingId; }

    virtual bool sample(decimal density, std::vector<vec3>& samples) { return false; }

    bool isSol() const { return _isSol; }
    void setIsSol(const bool& isSol) { _isSol = isSol; }



protected:
    BBox box;                                //Bounding box of the shape
    Material* material;                      //Material
    std::vector<vec3> *vertices;             //GlobalVertices of the scene
    std::vector<unsigned int> localVertices; //Index of the vertices used for this shape.
    int primitiveId;                         //Index of the primitive (given by the scene)
    int faceId;                              //Index of the face supporting the primitive
    int buildingId;                          //Index of the building supporting the primitive (-1 if none)
    int etageId;                             //Index of the etage supporting the primitive   (-1 if none)
    bool _isSol;                             // Type de triangle (false = non naturel, true = sol)
};

#endif
