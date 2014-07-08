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

#ifndef SCENE_H
#define SCENE_H

#include "Shape.h"
#include "Tympan/solvers/AcousticRaytracer/Accelerator/Accelerator.h"
#include <vector>

//#ifdef USE_QT
//  #include "SceneGraphic.h"
//#endif


struct compVec
{
    bool operator()(const vec3& _v1, const vec3& _v2) const
    {
        if (fabs(_v1.x - _v2.x) > 0.001)
        {
            return _v1.x < _v2.x;
        }
        if (fabs(_v1.y - _v2.y) > 0.001)
        {
            return _v1.y < _v2.y;
        }
        if (fabs(_v1.z - _v2.z) > 0.001)
        {
            return _v1.z < _v2.z;
        }
        return false;
    }
};

class Scene : public Base
{

    //#ifdef USE_QT
    //  //WIDGET_DECL(Sphere)
    //  GRAPHIC_DECL(Scene)
    //#endif

public:
    Scene() : accelerator(NULL), Base() { name = "unknown scene"; compteurPrimitive = 0; compteurFace = 0;}
    Scene(const Scene& other) : Base(other) { }

    virtual ~Scene() { }

    void clean();

    void addShape(Shape* shape)
    {
        shapes.push_back(shape);
        globalBox = globalBox.Union(shape->getBBox());

        shape->setPrimitiveId(compteurPrimitive);
        compteurPrimitive++;
    }

    BBox getGlobalBox() { return globalBox; }

    Accelerator* getAccelerator() { return accelerator; }

    std::vector<Shape*>* getShapes() { return &shapes; }
    std::vector<vec3>* getVertices() { return &vertices; }

    bool finish();

    bool getIndex(vec3& search, unsigned int& result);
    bool getVertex(unsigned int& search, vec3& result);

    bool addVertex(const vec3& newVertex, unsigned int& index);

    Shape* addTriangle(unsigned int i1, unsigned int i2, unsigned int i3, Material* m, const bool& isSol = false);

    void addBuilding(vec3 origine, vec3 dimension, Material* m);

protected:
    std::vector<Shape*> shapes;
    BBox globalBox;
    Accelerator* accelerator;

    std::vector<vec3> vertices;                             //All the vertices used by the different shapes
    std::map<vec3, unsigned int, compVec> registeredVertices; //Association between a vertex and his index in vertices

    unsigned int compteurPrimitive;
    unsigned int compteurFace;
};

#endif
