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

#include "Cylindre.h"
#include "Triangle.h"


Cylindre::Cylindre(Shape* _p1, Shape* _p2 , std::vector<vec3>* _vertices, unsigned int v1, unsigned int v2, decimal _epaisseur) :
    Shape(std::string("unknown cylindre"))
{
    vertices = _vertices;
    p1 = _p1;
    p2 = _p2;
    epaisseur = _epaisseur;
    localVertices.push_back(v1);
    localVertices.push_back(v2);

	//Skip instructions dependant on v1 and v2 if at least one of them does not exist
	if(vertices && vertices->size()>max(v1,v2)){

		vec3 X = vertices->at(localVertices.at(1)) - vertices->at(localVertices.at(0));
		X.normalize();
		vec3 Y = p1->getNormal() + p2->getNormal();
		Y.normalize();
		vec3 Z; Z.cross(X, Y);

		vec3 segment = vertices->at(localVertices.at(1)) - vertices->at(localVertices.at(0));
		hauteur = segment.length();

		localRepere = Repere(X, Y, Z, vertices->at(localVertices.at(0)));
		angleOuverture = static_cast<decimal>(2 * M_PI);

		material = p1->getMaterial();

		computeMesh();
	}
}


Cylindre::Cylindre(const Cylindre& other)
{
    vertices = other.vertices;
    p1 = other.p1;
    p2 = other.p2;
    name = other.name;
    epaisseur = other.epaisseur;
    hauteur = other.hauteur;
    for (unsigned int i = 0; i < other.localVertices.size(); i++)
    {
        localVertices.push_back(other.localVertices.at(i));
    }

    localRepere = Repere(other.localRepere);
    angleOuverture = other.angleOuverture;
    material = other.material;

    computeMesh();
    box = other.box;
}

/*bool Cylindre::getIntersection(Ray &ray, Intersection &inter){

    vec3 localPos = localRepere.positionFromGlobalToLocal(ray.position);
    vec3 localDir = localRepere.vectorFromGlobalToLocal(ray.direction);

    decimal A = localDir.y * localDir.y + localDir.z * localDir.z;
    decimal B = 2 * (localDir.y * localPos.y + localDir.z * localPos.z);
    decimal C = localPos.y * localPos.y + localPos.z * localPos.z - epaisseur * epaisseur;

    decimal delta = B *B - 4. * A * C;

    if(delta <= 0.)
        return false;

    return true;

    decimal t1 = (-B - sqrt(delta)) / (2. * A);
    decimal t2 = (-B + sqrt(delta)) / (2. * A);

    decimal hauteur1 = localPos.x + t1 * localDir.x;
    decimal hauteur2 = localPos.x + t2 * localDir.x;

    if(hauteur1 > 0. && hauteur1 < hauteur){
        inter.p = this;
        inter.t = t1;
        inter.forme = CYLINDRE;
        return true;
    }

    return false;
}*/

bool Cylindre::getIntersection(Ray& ray, Intersection& inter)
{

    bool findIntersection = false;
    Intersection currentInter;
    decimal tmin = -1.;
    for (unsigned int i = 0; i < mesh.size(); i++)
    {
        if (mesh.at(i)->getIntersection(ray, currentInter))
        {
            if (tmin < 0 || currentInter.t < tmin)
            {
                findIntersection = true;
                inter.forme = CYLINDRE;
                inter.p = this;
                inter.t = currentInter.t;
                tmin = inter.t;
            }
        }
    }
    return findIntersection;

}

void Cylindre::computeMesh()
{

    vec3 O = localRepere.getO();
    vec3 X = localRepere.getU();
    vec3 Y = localRepere.getV();
    vec3 Z = localRepere.getW();

    X.normalize();
    Y.normalize();
    Z.normalize();

    X *= hauteur;
    Y *= epaisseur;
    Z *= epaisseur;

    points.reserve(8);
    points.push_back(O - Y - Z);
    points.push_back(O + Y - Z);
    points.push_back(O + Y + Z);
    points.push_back(O - Y + Z);

    points.push_back(O + X - Y - Z);
    points.push_back(O + X + Y - Z);
    points.push_back(O + X + Y + Z);
    points.push_back(O + X - Y + Z);

    mesh.push_back(new Triangle(0, 1, 5, &points, material));
    mesh.push_back(new Triangle(5, 4, 0, &points, material));
    mesh.push_back(new Triangle(2, 3, 7, &points, material));
    mesh.push_back(new Triangle(7, 6, 2, &points, material));
    mesh.push_back(new Triangle(1, 2, 6, &points, material));
    mesh.push_back(new Triangle(6, 5, 1, &points, material));
    mesh.push_back(new Triangle(7, 3, 0, &points, material));
    mesh.push_back(new Triangle(0, 4, 7, &points, material));
    mesh.push_back(new Triangle(4, 5, 6, &points, material));
    mesh.push_back(new Triangle(6, 7, 4, &points, material));

    box = mesh.at(0)->getBBox();
    for (unsigned int i = 1; i < mesh.size(); i++)
    {
        box = box.Union(mesh.at(i)->getBBox());
    }

    box.pMin = vec3(box.pMin.x - (decimal)0.01, box.pMin.y - (decimal)0.01, box.pMin.z - (decimal)0.01);
    box.pMax = vec3(box.pMax.x + (decimal)0.01, box.pMax.y + (decimal)0.01, box.pMax.z + (decimal)0.01);
}

bool Cylindre::sample(decimal density, std::vector<vec3>& samples)
{
    vec3 v1 = vertices->at(localVertices.at(0));
    vec3 v2 = vertices->at(localVertices.at(1));
    vec3 v1v2 = v2 - v1;

    vec3 normale = p1->getNormal() + p2->getNormal();
    normale.normalize();

    decimal lenght = v1.distance(v2);

    unsigned int nbSamples = (unsigned int)(lenght * density) + 1;

    for (unsigned int i = 0; i < nbSamples; i++)
    {
        decimal choice = (decimal)rand() / (decimal)RAND_MAX;
        vec3 newSample = v1 + v1v2 * choice + normale * decimal(0.001); //On releve legerement le point pour etre sur que le point soit visible de l'exterieur du batiment
        samples.push_back(newSample);
    }

    return true;

}
