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

#ifndef __GEOMETRY_MODIFIER_H
#define __GEOMETRY_MODIFIER_H

#include <qlist.h>

#include "Tympan/solvers/AcousticRaytracer/Geometry/mathlib.h"
#include "Tympan/models/business/TYRay.h"
#include "Tympan/models/common/3d.h"
#include "Tympan/models/common/triangle.h"
#include "Tympan/solvers/AcousticRaytracer/Geometry/mathlib.h"

class Lancer;

using namespace std;

class geometry_modifier :
    public IGeometryModifier
{
public:

    // Constructeurs :
    geometry_modifier() : methode(1) {}
    geometry_modifier(Lancer& L) : methode(1) {}
    geometry_modifier(geometry_modifier& r) : methode(r.methode) {}

    // Destructeur :
    ~geometry_modifier() {}

    /*!
    * \fn void Init()
    * \brief Efface tous les tableaux.
    */
    void clear();

    /*!
    * \fn void setMethode(const unsigned int& meth)
    * \brief Modification de la methode de transformation.
    * \param meth methode que l'on souhaite utiliser pour transformer notre geometrie
    */
    void setMethode(const unsigned int& meth) {methode = meth;}

    /*!
     * \fn void trianguleNappe()
     * \brief creation de la nappe de rayons triangulee pour l'interpolation
     */
    void trianguleNappe(const Lancer& shot);

    /*!
     * \fn QList<OTriangle>& getNappe()
     * \brief Get de la nappe de rayons triangulee
     */
    QList<OTriangle>& getNappe() { return Liste_triangles; }

    /* Fonction qui met dans le tableau Tableau les 4 points voisins du point P se trouvant entre x(i) et x(i+1) */
    //  void find_voisin(vec3 P, map< pair<double, double>, double > plan, vec3* Tableau);

    /*!
    * \fn vec3 fonction_h (vec3 P)
    * \brief fonction de transformation.
    * \param P point que l'on desire transformer
    * \return rend les coordonnees du point transforme.
    */
    vec3 fonction_h(const vec3& P);
    virtual OPoint3D fonction_h(const OPoint3D& P)
    { return vec3toOPoint3D(fonction_h(OPoint3Dtovec3(P))); }

    /*!
    * \fn vec3 fonction_h_inverse (vec3 P, QList<OTriangle> Liste_triangles)
    * \brief fonction inverse de la fonction de transformation.
    * \param P point que l'on desire transformer
    * \param Liste_triangles liste des triangles de la geometrie
    * \return rend les coordonnees du point transforme (point de l'espace original).
    */
    vec3 fonction_h_inverse(const vec3& P);
    virtual OPoint3D fonction_h_inverse(const OPoint3D& P)
    { return vec3toOPoint3D(fonction_h_inverse(OPoint3Dtovec3(P))); }

    /*!
     * \fn double interpo(const vec3* triangle, vec3 P);
     * \brief return z position of point (P) inside a triangle
     */
    double interpo(const vec3* triangle, vec3 P);

private :
    int methode;                                               /*!< entier definissant la methode de transformation utilisee */
    vec3 pos_center;

    QList<OTriangle> Liste_triangles;                           /*!< Liste des triangles de la nappe interpolee */
    QList<OPoint3D> Liste_vertex;                                   /*!< Liste des vertex de la triangulation */

private :
};

bool IsInTriangle(const vec3& P, const vec3* triangle);

#endif //__GEOMETRY_MODIFIER_H
