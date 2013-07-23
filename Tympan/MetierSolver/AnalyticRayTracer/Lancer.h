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
 
/*!
* \file Lancer.h
* \author Projet_Tympan
*/

#ifndef __LANCER_H
#define __LANCER_H

#include <cmath>
#include <cassert>
#include <cstdlib>
#include <vector>
#include <map>
#include <iostream>
#include "R3.h"
#include "meteo.h"


using namespace std;

class RayCourb;

typedef double R;

/*! \class Lancer
* \brief classe representant un lancer de rayons courbes
*/

class Lancer
{
public:

    // Donnees membres :

    vector<R3> sources;                 /*!< vector contenant les sources */
    vector<R3> recepteurs;              /*!<  vector contenant les recepteurs */
    vector<R3*> plan;                   /*!<  "liste" des objets definis par 3 points */
    meteo Meteo;                        /*!<  meteo */

    R h;                                /*!<  pas de discretisation */
    R TMax;                             /*!<  temps de propagation maximal */
    vector<R> temps;                    /*!<  [0:h:temps_max] vecteur des temps ou l'on resouds */
    R dmax;                             /*!<  distance maximale parcourue par les rayons */
    R initialAngleTheta;                /*!<  angle de tir initial selon theta */
    R finalAngleTheta;                  /*!<  angle de tir final selon theta */
    R initialAnglePhi;                  /*!<  angle de tir initial selon phi */
    R finalAnglePhi;                    /*!<  angle de tir final selon phi */

    unsigned int nbRay;                 /*!<  nombre de rayons que l'on lance */
    unsigned int launchType;            /*!<  mode de lancer des rayons 1:horizontal / 2:vertical / 3:spheric / 4:file */
    bool wantOutFile;                   /*!<  true if outputfile wanted */
    string ray_fileName;                /*!<  filename of file containing angles of rays */
    string out_fileName;                /*!<  filename of the output file */


    vector<RayCourb*> MatRes;           /*!<  tableau contenant les rayons obtenus pour chaque source */


    /*!
     *  \brief Constructeur
     *
     *  Constructeurs de la classe Lancer par defaut, par copie et par passage d'arguments
     *
     */
    Lancer();
    Lancer(Lancer& L);
    Lancer(vector<R3> sources, vector<R3> recepteurs, vector<R3*> plan, meteo Meteo, R h, R TmpMax, vector<R> temps, R dmax, unsigned int nbRay);

    /*!
     *  \brief Destructeur
     *
     *  Destructeur de la classe Lancer
     */
    ~Lancer();

    //--------------------------------------------------------------------------------------------------------//
    // Fonctions membres :
    //--------------------------------------------------------------------------------------------------------//

    /*!
    * \fn void purgeMatRes()
    * \brief Libere la memoire occupee par MatRes
    */
    void purgeMatRes();

    /*!
    * \fn void Init()
    * \brief Efface tous les tableaux
    */
    void Init() { sources.clear(); recepteurs.clear(); plan.clear(); temps.clear(); purgeMatRes(); }

    /*!
    * \fn void setNbRay(const unsigned int& nb)
    * \brief Modifie le nombre de rayons
    * \param nb nombre de rayons que l'on souhaite lancer
    */
    void setNbRay(const unsigned int& nb) {nbRay = nb;}

    /*!
    * \fn void setTMax(const double& TmpMax)
    * \brief Modifie le temps de propagation
    * \param TmpMax nouvelle valeur que l'on souhaite attribuer a TMax
    */
    void setTMax(const double& TmpMax) { TMax = TmpMax; }

    /*!
    * \fn void setDMax(const double& DistMax)
    * \brief Modifie de la distance de propagation
    * \param DistMax nouvelle valeur de la distance maximale
    */
    void setDMax(const double& DistMax) { dmax = DistMax; }

    /*!
    * \fn void setTimeStep(const R& tt)
    * \brief Modification du pas de temps
    * \param tt nouveau pas de discretisation du temps
    */
    void setTimeStep(const R& tt) { h = tt; }

    /*!
    * \fn void setTriangle(R3* triangle)
    * \brief Ajoute des triangles (objets) a la geometrie
    * \param triangle tableau de R3, represente l'objet que l'on desire ajouter a la geometrie
    */
    void setTriangle(R3* triangle) { plan.push_back(triangle); }

    /*!
    * \fn R distance_max()
    * \brief Calcul de la distance maximale entre les sources et les recepteurs
    * \return rend la distance maximale
    */
    R distance_max();

    /*!
    * \fn void createTemps()
    * \brief Creation du vecteur temps
    */
    void createTemps();

    /*!
    * \fn vector<R3> EqRay(vector<R3> y0, meteo Meteo)
    * \brief Fonction definissant l'equation eikonale
    * \param y0 vecteur initial
    * \param Meteo meteo du probleme
    * \return rend un point du rayon sous la forme (position, normale)
    */
    vector<R3> EqRay(const vector<R3>& y0, const meteo& Meteo);

    /*!
    * \fn R3 intersection(R3 S, R3 R, R3*A, int &reflexion, R3 nExt_plan, R3 SR)
    * \brief Calcule le point d'intersection entre un plan et une droite
    * \param S source
    * \param R recepteur
    * \param A Tableau contenant les 3 sommets de la face (tous les sommets doivent etre coplanaires)
    * \param reflexion variable test pour savoir si on a ou non trouve une intersection entre la droite SR et le plan A
    * \param nExt_plan normale exterieure au plan considere
    * \param SR vecteur source-recepteur
    * \return rend le point d'intersection entre le plan A et la droite SR.
    */
    R3 intersection(const R3& S, const R3& R, const R3* A, int& reflexion, const R3& nExt_plan, const R3& SR);

    /*!
    * \fn RayCourb RK4(vector<R3> y0, vector<R3*> plan, R3 source)
    * \brief Algorithme de Runge-Kutta d'ordre 4
    * \param y0 vecteur initial
    * \param plan geometrie
    * \param S source
    * \return rend un rayon (liste de points de l'espace)
    */
    RayCourb RK4(const vector<R3>& y0, const vector<R3*>& plan, const R3& source);

    /*!
    * \fn void RemplirMat()
    * \brief Remplit la matrice MatRes contenant les rayons courbes
    */
    void RemplirMat();

    /*!
    * \fn void run()
    * \brief Effectue le calcul.
    */
    void run() { createTemps(); RemplirMat(); }

    /*!
     * \fn loadRayFile(vector<R3>& tableau_norm)
     * \brief Chargement d'un fichier definissant les angles de depart des rayons
     */
    void loadRayFile(vector<R3>& tableau_norm);

	/*!
	 * \fn traveledDistance(vector<R3>& vec)
	 * \brief Calcule la distance parcourue sur le plan horizontale depuis la source jusqu'au point courant
	 * \param vector<R3&> vec
	 */
	double traveledDistance(const vector<R3>& vec, const R3& source)  { return (vec[0].x - source.x) * (vec[0].x - source.x) + (vec[0].y - source.y) * (vec[0].y - source.y); }
};

/*!
* \fn R angle_depart(R a, R c, R d, R h)
* \brief Calcul l'angle avec lequel le rayon doit etre lance afin de parcourir au minimum la distance d
* \param a gradient de celerite
* \param c celerite au sol
* \param d distance minimale a parcourir
* \param h hauteur de la source
* \return rend l'angle du lancer de rayon.
*/
R angle_depart(const R& a, const R& c, const R& d, const R& h);

#endif // __LANCER_H