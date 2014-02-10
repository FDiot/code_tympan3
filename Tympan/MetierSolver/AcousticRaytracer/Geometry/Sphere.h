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

#ifndef SPHERE_H
#define SPHERE_H

//#ifdef USE_QT
//  #include "SphereGraphic.h"
//#endif

#include "Shape.h"

class Sphere : public Shape
{

    //#ifdef USE_QT
    //  //WIDGET_DECL(Sphere)
    //  GRAPHIC_DECL(Sphere)
    //#endif

public:
    Sphere() : Shape() { position = vec3(0.0, 0.0, 0.0); radius = 0.;}
    Sphere(vec3 pos, decimal r) : Shape()
    {
        position = vec3(pos);
        radius = r;

        box = BBox(vec3(pos.x - r, pos.y - r, pos.z - r), vec3(pos.x + r, pos.y + r, pos.z + r));
    }

    Sphere(Sphere* other) : Shape(other)
    {
        position = vec3(other->position);
        radius = other->radius;
        box = other->box;
    }

    virtual Shape* Clone()
    {
        Sphere* newShape = new Sphere(this);
        return newShape;
    }

    Sphere(const Sphere& other) : Shape(other)
    {
        position = vec3(other.position);
        radius = other.radius;
        box = other.box;
    }

    virtual bool getIntersection(Ray& ray, Intersection& inter);

    void setRadius(decimal _radius) { radius = _radius; }
    decimal getRadius() { return radius; }

    void setPosition(vec3& _position) { position = _position; }
    vec3 getPosition() { return position; }

protected:
    vec3 position;
    decimal radius;

};

#endif
