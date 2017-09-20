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

#ifndef MESH_H
#define MESH_H

#include "Shape.h"

/**
 * \brief Mesh class
 */
class Mesh : public Shape
{

public:
	/// Constructors
    Mesh() { };
    Mesh(const Mesh& other) { };
    /// Destructor
    virtual ~Mesh() { clear(); };
    /// Clear arrays
    void clear();

    /// Get/Set the vertices of the mesh
    std::vector<vec3>& getVertices() { return vertices; }
    void setVertices(const std::vector<vec3> &_vertices)
    {
        for (unsigned int i = 0; i < _vertices.size(); i++)
        {
            vertices.push_back(vec3(_vertices.at(i)));
        }
    }

    /// Get/Set triangles of the mesh
    std::vector<ivec3>& getTriangles() { return triangles; }
    void setTriangles(const std::vector<ivec3> &_triangles)
    {
        for (unsigned int i = 0; i < _triangles.size(); i++)
        {
            triangles.push_back(ivec3(_triangles.at(i)));
        }
    }
    /// Add a triangle to the mesh with the material m
    bool addTriangle(const ivec3 newTriangle, Material* m);

    virtual bool getIntersection(Ray& ray, Intersection& inter) { return false; }


protected:
    std::vector<vec3> vertices;		//!< Vertices of the mesh
    std::vector<ivec3> triangles;	//!< Triangles of the mesh

};
#endif
