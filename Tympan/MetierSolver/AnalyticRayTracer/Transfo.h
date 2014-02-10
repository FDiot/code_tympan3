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
#include "Tympan/MetierSolver/ToolsMetier/OTriangle.h"

using namespace std;

class RayCourb;
class Lancer;

typedef double R;

class Transfo
{
public:
    // Donnees membres :

    unsigned int source;                                       /*!< Indice de la source dans le tableau des source du lancer */
    vector<R3> recepteurs;                                     /*!< la liste de recepteurs */
    meteo Meteo;                                               /*!< meteo */
    unsigned int nbRay;                                        /*!< nombre de rayons que l'on lance */
    R dmax;                                                    /*!< distance maximale parcourue par les rayons */
    R h;                                                       /*!< pas de discretisation */
    R TMax;                                                    /*!< temps de propagation maximal */
    vector<R> temps;                                           /*!< [0:h:temps_max] vecteur des temps ou l'on resouds */

    Lancer shot;                                               /*!< notre lancer sans reflexion */

    vector<R3*> planDepart;                                    /*!< l'espace de depart */
    vector<R3*> planTransfo;                                   /*!< l'espace transforme */
    vector<RayCourb*> MatRes;                                  /*!< matrice contenant les rayons droits */
    vector<RayCourb*> MatTransfo;                              /*!< tableau contenant les rayons droits transformes */

    int methode;                                               /*!< entier definissant la methode de transformation utilisee */

    QList<OTriangle> Liste_triangles;                           /*!< Liste des triangles de la nappe interpolee */
    QList<OPoint3D> list_vertex;                                    /*!< Liste des vertex de la triangulation */


    // Constructeurs :
    Transfo();
    Transfo(Lancer& L);
    Transfo(Transfo& r);

    // Destructeur :
    ~Transfo();

    /*!
     * \fn void purge()
     * \brief Nettoie les tableaux en desallouant proprement la memoire
     */
    void purge();

    // Fonctions membres :

    /*!
    * \fn void Init()
    * \brief Efface tous les tableaux.
    */
    void Init();

    /*!
    * \fn void setNbRay(const unsigned int& nb)
    * \brief Modifie le nombre de rayons.
    * \param nb nombre de rayons que l'on souhaite lancer
    */
    void setNbRay(const unsigned int& nb) {nbRay = nb;}

    /*!
    * \fn void setTMax(const double& TmpMax)
    * \brief Modifie le temps de propagation.
    * \param TmpMax nouvelle valeur que l'on souhaite attribuer a TMax
    */
    void setTMax(const double& TmpMax) { TMax = TmpMax; }

    /*!
    * \fn void setDMax(const double& DistMax)
    * \brief Modifie de la distance de propagation.
    * \param DistMax nouvelle valeur de la distance maximale
    */
    void setDMax(const double& DistMax) { dmax = DistMax; }

    /*!
    * \fn void setTimeStep(const R& tt)
    * \brief Modification du pas de temps.
    * \param tt nouveau pas de discretisation du temps
    */
    void setTimeStep(const R& tt) { h = tt; }

    /*!
    * \fn void setMethode(const unsigned int& meth)
    * \brief Modification de la methode de transformation.
    * \param meth methode que l'on souhaite utiliser pour transformer notre geometrie
    */
    void setMethode(const unsigned int& meth) {methode = meth;}

    /*!
    * \fn R distance_max()
    * \brief Calcul de la distance maximale entre les sources et les recepteurs.
    * \return rend la distance maximale
    */
    R distance_max();

    /*!
    * \fn void createTemps()
    * \brief Creation du vecteur temps.
    */
    void createTemps();

    /*!
    * \fn void createShot()
    * \brief Creation du lancer dans une geometrie vide.
    */
    void createShot();

    /*!
    * \fn RemplirSurf()
    * \brief Remplissage de Surf_Interp qui contient tous les points de notre lancer
       par ordre croissant des x puis des y.
    */
    void RemplirSurf(vector<R3>& vectSurf_Interp);

    /*!
    * \fn Transfo_Geom1()
    * \brief Transformations de la geometrie.
    */
    void Transfo_Geom1();
    void Transfo_Geom2();

    /*!
    * \fn void TransfoR1()
    * \brief Transformations des rayons droits en rayons courbes.
    */
    void TransfoR1();
    void TransfoR2();

    /*!
    * \fn void Transformer()
    * \brief Transformation de la geometrie, lancer de rayons droits et transformation de ces rayons droits en rayons courbes.
    */
    void Transformer();

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
    //  void find_voisin(R3 P, map< pair<double, double>, double > plan, R3* Tableau);

    /*!
    * \fn R3 fonction_h (R3 P)
    * \brief fonction de transformation.
    * \param P point que l'on desire transformer
    * \return rend les coordonnees du point transforme.
    */
    R3 fonction_h(const R3& P);

    /*!
    * \fn R3 fonction_h_inverse (R3 P, QList<OTriangle> Liste_triangles)
    * \brief fonction inverse de la fonction de transformation.
    * \param P point que l'on desire transformer
    * \param Liste_triangles liste des triangles de la geometrie
    * \return rend les coordonnees du point transforme (point de l'espace original).
    */
    R3 fonction_h_inverse(const R3& P);

    /**
     * Force le choix de la source pour le lancer de rayons
    */
    void setSrcForMapping(unsigned int& sourceIdx);
};

bool IsInTriangle(const R3& P, const R3* triangle);

#endif //__TRANSFO_H
