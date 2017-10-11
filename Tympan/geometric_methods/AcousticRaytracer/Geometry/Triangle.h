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

/**
 * \brief Triangle class
 */
class Triangle : public Shape
{

public:
	/// Default constructor
    Triangle() { name = "unknown triangle"; };
    /// Constructor with 3 index points of the array _vertices
    Triangle(unsigned int p1, unsigned int p2, unsigned int p3, std::vector<vec3>* _vertices, Material* _m, const bool& isSol = false);
    /// Constructor with 3 points
    Triangle(const vec3& p1,const vec3& p2,const vec3& p3, Material* m, const bool& isSol = false);
    /// Copy constructor
    Triangle(const Triangle& other) : Shape(other)
    {
        box = other.box;
    };
    /// Destructor
    virtual ~Triangle() { }

    virtual void updateBBox();

    virtual bool getIntersection(Ray& ray, Intersection& inter);

    /// Set/Get the normal
    void setNormal(const vec3& _normal) { normal = _normal; }
    virtual vec3 getNormal(const vec3 pos = vec3()) { return normal;}

    /// Uncommented method cause not used
    virtual bool sample(decimal density, std::vector<vec3>& samples);

    virtual int form() { return TRIANGLE; }

protected:
    vec3 p;			//!< First vertex (vertex1=p)
    vec3 u;			//!< Vector to reach the second vertex (vertex2=p+u)
    vec3 v;			//!< Vector to reach the third vertex (vertex3=p+v)

    vec3 normal;	//!< Normal to triangle
};

#endif
