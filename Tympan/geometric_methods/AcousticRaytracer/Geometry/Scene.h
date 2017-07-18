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

#include <vector>
#include "Accelerator/Accelerator.h"
#include "Shape.h"

/**
 * \struct compVec
 * \brief Use to compare vectors
 */
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

/**
 * \brief This class mainly define a mesh (list of Shape) used by the Simulation object
 */
class Scene : public Base
{

public:
	/// Constructor
    Scene() : accelerator(NULL), Base() { name = "unknown scene"; compteurPrimitive = 0; compteurFace = 0;}
    /// Copy constructor
    Scene(const Scene& other) : Base(other) { }
    /// Destructor
    virtual ~Scene() { }
    /// Clear all arrays
    void clean();
    /// Add a shape to the list
    void addShape(Shape* shape)
    {
        shapes.push_back(shape);
        globalBox = globalBox.Union(shape->getBBox());

        shape->setPrimitiveId(compteurPrimitive);
        compteurPrimitive++;
    }
    /// Return global bounding box
    BBox getGlobalBox() { return globalBox; }
    /// Get the accelerator
    Accelerator* getAccelerator() const { return accelerator; }

    /// Return all the shapes
    std::vector<Shape*>* getShapes() { return &shapes; }
    const std::vector<Shape*>* getShapes() const { return &shapes; }
    /// Return all the vertices
    std::vector<vec3>* getVertices() { return &vertices; }
    const std::vector<vec3>* getVertices() const { return &vertices; }
    /// Return all the shapes of a specific type
    std::vector<Shape*> getShapes(int shape_type);

    /**
     * @brief Build the selected accelerator on the scene
     * @param accelerator_id Accelerator id
     * @param _intersectionChoice Intersection choice
     * @return True if succeeds, false if not
     */
    bool finish(int accelerator_id = 3, leafTreatment::treatment _intersectionChoice = leafTreatment::FIRST);

    /**
     * @brief Get the index of a vertex in the vertices array
     * @param search [in] Vertex
     * @param result [out] Index found
     * @return True if succeeds, false if not
     */
    bool getIndex(vec3& search, unsigned int& result);
    /**
     * @brief Get a vertex from the vertices array with its index
     * @param search [in] Index
     * @param result [out] Vertex found
     * @return True if vertex is found else false
     */
    bool getVertex(unsigned int& search, vec3& result);
    /**
     * @brief Add a vertex to the vertices array
     * @param newVertex [in] New vertex
     * @param index [out] Index in the array vertices
     * @return True if succeeds, else false
     */
    bool addVertex(const vec3& newVertex, unsigned int& index);

    /**
     * @brief Add a triangle to the scene built with the vertices array
     * @param i1 Index of first vertex in vertices
     * @param i2 Index of second vertex in vertices
     * @param i3 Index of third vertex in vertices
     * @param m Material
     * @param isSol True is ground triangle
     * @return The triangle
     */
    Shape* addTriangle(unsigned int i1, unsigned int i2, unsigned int i3, Material* m, const bool& isSol = false);

    /**
     * @brief Add a building to the scene
     * @param origine Position
     * @param dimension Dimensions
     * @param m Material
     */
    void addBuilding(vec3 origine, vec3 dimension, Material* m);
    /// Export the Scene to a PLY format file named fileName
    void export_to_ply(std::string fileName);
    /// Import a Scene from a PLY format file named fileName
    void import_from_ply(std::string fileName);

protected:
    std::vector<Shape*> shapes;									//!< Array of pointers to the shapes
    BBox globalBox;												//!< Bounding box of the Scene
    Accelerator* accelerator;									//!< Pointer to the accelerator

    std::vector<vec3> vertices;                             	//!< All the vertices used by the different shapes
    std::map<vec3, unsigned int, compVec> registeredVertices; 	//!< Association between a vertex and his index in vertices

    unsigned int compteurPrimitive;								//!< Primitives counter
    unsigned int compteurFace;									//!< Faces counter
};

#endif
