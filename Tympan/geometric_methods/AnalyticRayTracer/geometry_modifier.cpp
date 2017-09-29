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



#include "geometry_modifier.h"
#include "Tympan/models/common/delaunay_maker.h"
#include "Tympan/models/common/triangle.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Triangle.h"
#include "Lancer.h"
#include "RayCourb.h"

using namespace std;

void geometry_modifier_z_correction::buildNappe(const Lancer& shot)
{
    unsigned int nbRay = shot.nbRay;
    pos_center = shot.sources[0];

    // 1- On creer nos triangles de Delaunay
    ODelaunayMaker oDelaunayMaker(1e-5);

    for (unsigned int i = 0; i < nbRay; ++i)                                                      // boucle sur les rayons lances
    {
        for (unsigned int j = 0; j < shot.MatRes[0][i].etapes.size(); ++j)                         // boucle sur les points du rayon
        {
            // the mesh is corrected to reflect final geometry transformation 
            vec3 pos(shot.MatRes[0][i].etapes[j].pos);
            pos.z = -(pos.z-pos_center.z);
            oDelaunayMaker.addVertex(vec3toOPoint3D(pos));
        }
    }

    oDelaunayMaker.compute();

    // 2- On a notre liste de triangles
    QList<OTriangle> Liste_triangles = oDelaunayMaker.getFaces(); /*!< Liste des triangles de la nappe interpolee */
    QList<OPoint3D> Liste_vertex = oDelaunayMaker.getVertex(); /*!< Liste des vertex de la triangulation */

    append_triangles_to_scene(Liste_vertex, Liste_triangles);
}

vec3 geometry_modifier_z_correction::fonction_h(const vec3& P)
{
    double h = compute_h(P);

    return vec3(P.x, P.y, P.z + h);
}

vec3 geometry_modifier_z_correction::fonction_h_inverse(const vec3& P)
{
    double h = compute_h(P);

    return vec3(P.x, P.y, P.z - h);
}

double geometry_modifier_z_correction::compute_h(const vec3& P)
{
    double offset = 2000.;
    vec3 origine(P.x, P.y, (P.z + offset) );
    Ray ray1( origine, vec3(0., 0., -1.) );

    std::list<Intersection> LI;
    return (P.z + offset) - static_cast<double>( _scene->getAccelerator()->traverse( &ray1, LI ) );
}

void geometry_modifier_z_correction::append_triangles_to_scene(QList<OPoint3D>& Liste_vertex, QList<OTriangle>& Liste_triangles)
{
    vec3 pos;
    for (int i = 0; i<Liste_triangles.size(); i++)
    {
        unsigned int a, b, c;

        pos = OPoint3Dtovec3( Liste_vertex.at( Liste_triangles.at(i)._p1 ) );
        _scene->addVertex(pos, a);

        pos = OPoint3Dtovec3( Liste_vertex.at( Liste_triangles.at(i)._p2 ) );
        _scene->addVertex(pos, b);

        pos = OPoint3Dtovec3( Liste_vertex.at( Liste_triangles.at(i)._p3 ) );
        _scene->addVertex(pos, c);

        (Triangle*)_scene->addTriangle(a, b, c, nullptr);
    }

    _scene->finish(1); // Use grid accelerator
}
