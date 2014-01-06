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
 
#include "Scene.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Accelerator/BruteForceAccelerator.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Accelerator/KdtreeAccelerator.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Accelerator/BvhAccelerator.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Accelerator/GridAccelerator.h"
#include "Triangle.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/Logger.h"
#include "Tympan/MetierSolver/AcousticRaytracer/global.h"

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

bool Scene::finish()
{

    ss << "La scene comporte " << vertices.size() << " vertex." << std::endl;
    for (unsigned int i = 0; i < vertices.size(); i++)
    {
        ss << "(" << vertices.at(i).x << "," << vertices.at(i).y << "," << vertices.at(i).z << ")" << std::endl;
    }
    ss << "La scene comporte " << shapes.size() << " shapes." << std::endl;
    switch (globalAccelerator)
    {
        case 0 :
            accelerator = new BruteForceAccelerator(&shapes, globalBox);
            break;
        case 1 :
            accelerator = new GridAccelerator(&shapes, globalBox);
            break;
        case 2 :
            accelerator = new BvhAccelerator(&shapes, globalBox, globalMaxTreeDepth, "middle");
            break;
        case 3 :
            accelerator = new KdtreeAccelerator(&shapes, globalBox);
            break;
        default :
            accelerator = new GridAccelerator(&shapes, globalBox);
            break;
    }

    accelerator->setIntersectionChoice(leafTreatment::FIRST);

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
        //std::cout<<"Le vertex ("<<newVertex.x<<","<<newVertex.y<<","<<newVertex.z<<") etait deja enregistre."<<std::endl;
        return false;
    }
    else
    {
        index = vertices.size();
        vertices.push_back(newVertex);
        registeredVertices.insert(std::pair<vec3, unsigned int>(newVertex, index));
        //std::cout<<"Enregistrement du vertex ("<<newVertex.x<<","<<newVertex.y<<","<<newVertex.z<<") ("<<registeredVertices.size()<<")"<<std::endl;
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
