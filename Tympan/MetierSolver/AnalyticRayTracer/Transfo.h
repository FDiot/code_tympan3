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

#ifndef __TRANSFO_H
#define __TRANSFO_H

#include <cmath>
#include <cassert>
#include <cstdlib>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>

#include "qlist.h"
#include "Lancer.h"
#include "Tympan/MetierSolver/CommonTools/OTriangle.h"

using namespace std;

class RayCourb;
class Lancer;

class Transfo
{
public:

    // Constructeurs :
    Transfo() : methode(1) {}
    Transfo(Lancer& L) : shot(L), methode(1) {}
    Transfo(Transfo& r) : shot(r.shot), methode(r.methode) {}

    // Destructeur :
    ~Transfo() {}


    void setRecepteurs(const vector<vec3>& R) { shot.recepteurs = R; }
    void setMeteo(const meteo* Meteo) { shot.setMeteo(Meteo); }

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
    * \fn void Transformer()
    * \brief Transformation de la geometrie, lancer de rayons droits et transformation de ces rayons droits en rayons courbes.
    */
    void buildInterpolationSurface();

    /*!
     * \fn void trianguleNappe()
     * \brief creation de la nappe de rayons triangulee pour l'interpolation
     */
    void trianguleNappe();

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

    /*!
    * \fn vec3 fonction_h_inverse (vec3 P, QList<OTriangle> Liste_triangles)
    * \brief fonction inverse de la fonction de transformation.
    * \param P point que l'on desire transformer
    * \param Liste_triangles liste des triangles de la geometrie
    * \return rend les coordonnees du point transforme (point de l'espace original).
    */
    vec3 fonction_h_inverse(const vec3& P);

    /*!
     * \fn double interpo(const vec3* triangle, vec3 P);
     * \brief return z position of point (P) inside a triangle
     */
    double interpo(const vec3* triangle, vec3 P);

    /*!
     * \brief functions below are only interface with "shot" member
     */
    void setSource(const vec3& source) { shot.sources.push_back(source); }
    void setNbRay(const unsigned int& nb) { shot.setNbRay(nb); }
    void setDMax(const decimal& dm) { shot.setDMax(dm); }
    void setTMax(const decimal& tm) { shot.setTMax(tm); }
    void setTimeStep(const decimal& tt) { shot.setTimeStep(tt); }

public :
    Lancer shot;                                               /*!< notre lancer sans reflexion */

private :
    int methode;                                               /*!< entier definissant la methode de transformation utilisee */

    QList<OTriangle> Liste_triangles;                           /*!< Liste des triangles de la nappe interpolee */
    QList<OPoint3D> Liste_vertex;                                   /*!< Liste des vertex de la triangulation */

private :
};

bool IsInTriangle(const vec3& P, const vec3* triangle);

#endif //__TRANSFO_H
