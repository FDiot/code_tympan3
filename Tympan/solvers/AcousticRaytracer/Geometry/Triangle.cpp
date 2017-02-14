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

#include "Triangle.h"

Triangle::Triangle(vec3& p1, vec3& p2, vec3& p3, Material* m, const bool& isSol)
{
    name = "unknown triangle";
    Shape::setIsSol(isSol);
    material = m;
    p = p1;
    u = p2 - p1;
    v = p3 - p1;
    vertices = NULL;

    normal.cross(u, v);
    normal.normalize();

    updateBBox();

}

Triangle::Triangle(unsigned int p1, unsigned int p2, unsigned int p3, std::vector<vec3>* _vertices, Material* _m, const bool& isSol)
{
    Shape::setIsSol(isSol);
    vertices = _vertices;
    material = _m;

    localVertices.push_back(p1);
    localVertices.push_back(p2);
    localVertices.push_back(p3);

    vec3 v1 = vertices->at(p1);
    vec3 v2 = vertices->at(p2);
    vec3 v3 = vertices->at(p3);

    p = v1;
    u = v2 - v1;
    v = v3 - v1;

    normal.cross(u, v);
    normal.normalize();



    updateBBox();

}

void Triangle::updateBBox()
{
    box = BBox(p, p);
    box = box.Union(p + u);
    box = box.Union(p + v);
    box.pMin = vec3(box.pMin.x - 0.01, box.pMin.y - 0.01, box.pMin.z - 0.01);
    box.pMax = vec3(box.pMax.x + 0.01, box.pMax.y + 0.01, box.pMax.z + 0.01);
};

bool Triangle::getIntersection(Ray& ray, Intersection& inter)
{
    vec3 directeur = ray.direction;
    directeur.normalize();
    if (directeur.dot(this->normal) > 0.)
    {
        return false;
    }
    vec3 v = Vector_r(vertices->at(localVertices.at(1)), vertices->at(localVertices.at(0)));
    vec3 u = Vector_r(vertices->at(localVertices.at(2)), vertices->at(localVertices.at(0)));
    vec3 n;
    n.cross(u, v);
    vec3 otr = Vector_r(ray.position,
                        vertices->at(localVertices.at(0)));
    double ir, iu, iv;
    vec3 temp;

    ir = -(n.dot(otr)) / (n.dot(directeur));
    temp.cross(otr, v);
    iu = (temp.dot(directeur)) / (n.dot(directeur));
    temp.cross(u, otr);
    iv = (temp.dot(directeur)) / (n.dot(directeur));

    if (iu >= -0.00001 && iu <= 1.00001 && iv >= -0.00001 && iv <= 1.00001 && ir >= -0.00001 && iu + iv <= 1.00001) //Detection du point d'intersection
    {
        inter.t =  ir;
        inter.p = this;
        inter.forme = TRIANGLE;
        return true;
    }
    return false;

}

bool Triangle::sample(decimal density, std::vector<vec3>& samples)
{

    vec3 proj = vertices->at(localVertices.at(0)).closestPointOnLine(vertices->at(localVertices.at(1)), vertices->at(localVertices.at(2)));

    decimal areaTriangle = vertices->at(localVertices.at(1)).distance(vertices->at(localVertices.at(2))) * proj.distance(vertices->at(localVertices.at(0))) / 2.;

    unsigned int nbSamples = (unsigned int)(areaTriangle * density) + 1;

    for (unsigned int i = 0; i < nbSamples; i++)
    {
        decimal randU = 1.;
        decimal randV = 1.;
        while (randU + randV > 1.)
        {
            randU = (decimal)rand() / (decimal)RAND_MAX;
            randV = (decimal)rand() / (decimal)RAND_MAX;
        }
        samples.push_back(p + u * randU + v * randV);
    }

    return true;
}
