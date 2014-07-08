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
#include "Lancer.h"
#include "RayCourb.h"

using namespace std;

void geometry_modifier::clear()
{
    Liste_triangles.clear();
}

vec3 geometry_modifier::fonction_h(const vec3& P)
{
    // c'est notre fonction h(x,y) qui transforme notre geometrie

    vec3 R(P);

    // On cherche a quel triangle de Surf_Interp le point P appartient.
    // Un point P appartient au triangle ABC si l'aire signee des triangles ABP, BCP et CAP est positive.

    vec3* TabTriangle = new vec3[3];

    for (int i = 0; i < Liste_triangles.size(); ++i)
    {
        //On boucle sur les triangles du plan de transformation (Liste_triangles),
        //Si le point appartient a un triangle, on le transforme.
        //Sinon, le point n'est pas modifie (il est en dehors de la zone de transformation).

        TabTriangle[0] = vec3(OPoint3Dtovec3(Liste_vertex[Liste_triangles[i]._p1]));
        TabTriangle[1] = vec3(OPoint3Dtovec3(Liste_vertex[Liste_triangles[i]._p2]));
        TabTriangle[2] = vec3(OPoint3Dtovec3(Liste_vertex[Liste_triangles[i]._p3]));

        if (IsInTriangle(P, TabTriangle))
        {
            R.z = R.z - interpo(TabTriangle, R) + pos_center.z ;
            break;
        }
    }

    delete [] TabTriangle; // Gestion memoire

    return R;
}


vec3 geometry_modifier::fonction_h_inverse(const vec3& P)
{
    // Fonction inverse de la fonction h.

    vec3 R(P);

    // On cherche a quel triangle de Surf_Interp le point P appartient.
    // Un point P appartient au triangle ABC si l'aire signee des triangles ABP, BCP et CAP est positive.

    vec3* TabTriangle = new vec3[3];

    for (int i = 0; i < Liste_triangles.size(); ++i)
    {
        //On boucle sur les triangles du plan de transformation (Liste_triangles),
        //On cherche dans quel triangle est le point et on lui applique la transformation inverse.

        OPoint3D v1 = Liste_vertex[Liste_triangles[i]._p1];
        OPoint3D v2 = Liste_vertex[Liste_triangles[i]._p2];
        OPoint3D v3 = Liste_vertex[Liste_triangles[i]._p3];

        TabTriangle[0] = vec3(v1._x, v1._y, v1._z);
        TabTriangle[1] = vec3(v2._x, v2._y, v2._z);
        TabTriangle[2] = vec3(v3._x, v3._y, v3._z);

        if (IsInTriangle(P, TabTriangle))
        {
            R.z = R.z + interpo(TabTriangle, R) - pos_center.z;
            break;
        }

    }

    delete [] TabTriangle; // Gestion memoire

    return R;
}


void geometry_modifier::trianguleNappe(const Lancer& shot)
{
    unsigned int nbRay = shot.nbRay;
    pos_center = shot.sources[0];

    // 1- On creer nos triangles de Delaunay
    ODelaunayMaker oDelaunayMaker(1e-5);

    for (unsigned int i = 0; i < nbRay; ++i)                                                      // boucle sur les rayons lances
    {
        for (unsigned int j = 0; j < shot.MatRes[0][i].etapes.size(); ++j)                         // boucle sur les points du rayon
        {
            oDelaunayMaker.addVertex(vec3toOPoint3D(shot.MatRes[0][i].etapes[j].pos));
        }
    }

    oDelaunayMaker.compute();

    // 2- On a notre liste de triangles
    Liste_triangles = oDelaunayMaker.getFaces();
    Liste_vertex = oDelaunayMaker.getVertex();
}

double geometry_modifier::interpo(const vec3* triangle, vec3 P)
{
    // rend la coordonnee P.z

    const vec3& A = triangle[0];
    const vec3& B = triangle[1];
    const vec3& C = triangle[2];

    double del = (B.x - A.x) * (C.y - A.y) - (C.x - A.x) * (B.y - A.y);

    double a = ((B.x - P.x) * (C.y - P.y) - (C.x - P.x) * (B.y - P.y)) / del;
    double b = ((C.x - P.x) * (A.y - P.y) - (A.x - P.x) * (C.y - P.y)) / del;
    double c = ((A.x - P.x) * (B.y - P.y) - (B.x - P.x) * (A.y - P.y)) / del;

    return a * A.z + b * B.z + c * C.z;
}

bool IsInTriangle(const vec3& P, const vec3* triangle)  // R2:: ?
{
    vec2 A(triangle[0].x, triangle[0].y);
    vec2 B(triangle[1].x, triangle[1].y);
    vec2 C(triangle[2].x, triangle[2].y);
    vec2 Q(P.x, P.y);

    decimal resultat1 = area(A, B, Q);
    if (resultat1 < 0) { return false; }

    decimal resultat2 = area(B, C, Q);
    if (resultat2 < 0) { return false; }

    decimal resultat3 = area(C, A, Q);
    if (resultat3 < 0) { return false; }

    return true;
}
