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

#include <deque>
#include <string>
#include <iostream>
#include <fstream>

#include "Tympan/models/solver/config.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Accelerator/BruteForceAccelerator.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Accelerator/KdtreeAccelerator.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Accelerator/BvhAccelerator.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Accelerator/GridAccelerator.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Tools/Logger.h"

#include "Triangle.h"
#include "Scene.h"

void Scene::clean()
{
    for (unsigned int i = 0; i < shapes.size(); i++)
    {
        delete shapes.at(i);
    }
    shapes.clear();
    registeredVertices.clear();
    vertices.clear();
    globalBox.isNull = true;
    if (accelerator) { delete accelerator; }
}

bool Scene::finish(int accelerator_id/* = 3*/, leafTreatment::treatment _intersectionChoice/* = leafTreatment::FIRST*/)
{

    ss << "La scene comporte " << vertices.size() << " vertex." << std::endl;
    for (unsigned int i = 0; i < vertices.size(); i++)
    {
        ss << "(" << vertices.at(i).x << "," << vertices.at(i).y << "," << vertices.at(i).z << ")" << std::endl;
    }
    ss << "La scene comporte " << shapes.size() << " shapes." << std::endl;
    switch (accelerator_id)
    {
        case 0 :
            accelerator = new BruteForceAccelerator(&shapes, globalBox);
            break;
        case 1 :
            accelerator = new GridAccelerator(&shapes, globalBox);
            break;
        case 2 :
            accelerator = new BvhAccelerator(&shapes, globalBox,
                                             tympan::SolverConfiguration::get()->MaxTreeDepth,
                                             "middle");
            break;
        case 3 :
            accelerator = new KdtreeAccelerator(&shapes, globalBox);
            break;
        default :
            accelerator = new GridAccelerator(&shapes, globalBox);
            break;
    }

    accelerator->setIntersectionChoice(_intersectionChoice);

    return accelerator->build();
}

bool Scene::getIndex(vec3& search, unsigned int& result)
{
    std::map<vec3, unsigned int, compVec>::iterator it = registeredVertices.find(search);
    if (it != registeredVertices.end())
    {
        result = it->second;
        return true;
    }

    return false;
}

bool Scene::getVertex(unsigned int& search, vec3& result)
{
    if (search < vertices.size())
    {
        result = vertices[search];
        return true;
    }

    return false;
}

bool Scene::addVertex(const vec3& newVertex, unsigned int& index)
{
    std::map<vec3, unsigned int, compVec>::iterator it = registeredVertices.find(newVertex);
    if (it != registeredVertices.end())
    {
        index = it->second;
        return false;
    }
    else
    {
        index = vertices.size();
        vertices.push_back(newVertex);
        registeredVertices.insert(std::pair<vec3, unsigned int>(newVertex, index));
        return true;
    }
}

Shape* Scene::addTriangle(unsigned int i1, unsigned int i2, unsigned int i3, Material* m, const bool& isSol)
{
    Triangle* triangle = new Triangle(i1, i2, i3, &vertices, m, isSol);
    addShape(triangle);

    return triangle;
}

void Scene::addBuilding(vec3 origine, vec3 dimension, Material* m)
{

    unsigned int p1, p2, p3, p4, p5, p6, p7, p8;

    addVertex(origine, p1);
    addVertex(vec3(origine.x + dimension.x, origine.y, origine.z), p2);
    addVertex(vec3(origine.x + dimension.x, origine.y + dimension.y, origine.z), p3);
    addVertex(vec3(origine.x, origine.y + dimension.y, origine.z), p4);
    addVertex(vec3(origine.x, origine.y, origine.z + dimension.z), p5);
    addVertex(vec3(origine.x + dimension.x, origine.y, origine.z + dimension.z), p6);
    addVertex(vec3(origine.x + dimension.x, origine.y + dimension.y, origine.z + dimension.z), p7);
    addVertex(vec3(origine.x, origine.y + dimension.y, origine.z + dimension.z), p8);

    addTriangle(p1, p2, p6, m);
    addTriangle(p6, p5, p1, m);

    addTriangle(p3, p4, p8, m);
    addTriangle(p8, p7, p3, m);

    addTriangle(p2, p3, p7, m);
    addTriangle(p7, p6, p2, m);

    addTriangle(p8, p4, p1, m);
    addTriangle(p1, p5, p8, m);

    addTriangle(p5, p6, p7, m);
    addTriangle(p7, p8, p5, m);
}

std::vector<Shape*> Scene::getShapes(int shape_type)
{
    std::vector<Shape*> vec;
    for (unsigned int i=0; i<shapes.size(); i++)
    {
        if (shapes.at(i)->form() == shape_type) { vec.push_back( shapes.at(i) ); }
    }
    return vec;
}

void Scene::export_to_ply(std::string fileName)
{
    // using long long cause of a limitation of MS visual studio 10 in std::to_string
    long long nb_vertex = vertices.size();
    long long nb_faces = shapes.size();

    // Create the file content as a vector of string
    std::deque<std::string> file_content;
    
    // Create header
    file_content.push_back( std::string("ply\n") );
    file_content.push_back( std::string("format ascii 1.0\n") );
    file_content.push_back(   std::string("element vertex ") 
                            + std::to_string(nb_vertex) 
                            + std::string("\n") );
    file_content.push_back( std::string("property float x\n") );
    file_content.push_back( std::string("property float y\n") );
    file_content.push_back( std::string("property float z\n") );
    file_content.push_back(   std::string("element face ") 
                            + std::to_string(nb_faces) 
                            + std::string("\n") );
    file_content.push_back( std::string("property list uchar int vertex_indices\n") );
    file_content.push_back( std::string("property uchar red\n") );
    file_content.push_back( std::string("property uchar green\n") );
    file_content.push_back( std::string("property uchar blue\n") );
    file_content.push_back( std::string("end_header\n") );
    // Creation of vertex list
    std::string line;
    for (unsigned int i=0; i<vertices.size(); i++)
    {
        line = std::to_string( static_cast<long double>(vertices.at(i).x) ) + " " +
               std::to_string( static_cast<long double>(vertices.at(i).y) ) + " " +
               std::to_string( static_cast<long double>(vertices.at(i).z) ) + "\n";
        file_content.push_back( line );
    }

    // Creation of faces list
    vector<unsigned int>* vertice_index = nullptr;
    for (unsigned int i=0; i<shapes.size(); i++)
    {
        if (shapes.at(i)->form() != FORM::TRIANGLE) 
        {
            continue; 
        }

        vertice_index = shapes.at(i)->getLocalVertices();
        file_content.push_back( std::to_string(static_cast<long long>(3)) + " " +
                                std::to_string(static_cast<long long>(vertice_index->at(0))) + " " +
                                std::to_string(static_cast<long long>(vertice_index->at(1))) + " " + 
                                std::to_string(static_cast<long long>(vertice_index->at(2))) + " " +
                                std::string("0 255 0\n") );
    }

    // Create the file
    ofstream out(fileName, ios::out);
    for (unsigned int i=0; i<file_content.size(); i++)
    {
        out << file_content.at(i);
    }

    out.close();
}
