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
 
#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Shape.h"

//#ifdef USE_QT
//  #include "TriangleGraphic.h"
//#endif

class Triangle : public Shape
{

    //#ifdef USE_QT
    //  //WIDGET_DECL(Sphere)
    //  GRAPHIC_DECL(Triangle)
    //#endif

public:
    Triangle() { name = "unknown triangle"; };
    Triangle(unsigned int p1, unsigned int p2, unsigned int p3, std::vector<vec3>* _vertices, Material* _m, const bool& isSol = false);
    Triangle(vec3& p1, vec3& p2, vec3& p3, Material* m, const bool& isSol = false);
    Triangle(const Triangle& other) : Shape(other)
    {
        box = other.box;
    };

    virtual ~Triangle() { }

    virtual void updateBBox();

    virtual bool getIntersection(Ray& ray, Intersection& inter);

    void setNormal(const vec3& _normal) { normal = _normal; }
    virtual vec3 getNormal(const vec3 pos = vec3()) { return normal;}

    virtual bool sample(decimal density, std::vector<vec3>& samples);

protected:
    vec3 p;
    vec3 u;
    vec3 v;

    vec3 normal;
};

#endif
