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
#include "../AcousticRaytracer/Geometry/mathlib.h"
#include "../AcousticRaytracer/Geometry/Sampler.h"
#include "meteo.h"


using namespace std;

class RayCourb;

/*! \class Lancer
* \brief classe representant un lancer de rayons courbes
*/

class Lancer
{
public:
    Lancer();
    Lancer(Lancer& L);
    //Lancer(vector<vec3> sources, vector<vec3> recepteurs, vector<vec3*> plan, meteo *Meteo, decimal h, decimal TmpMax, vector<decimal> temps, decimal dmax, unsigned int nbRay);

    ~Lancer();

	//--------------------------------------------------------------------------------------------------------//
    // Fonctions membres :
    //--------------------------------------------------------------------------------------------------------//

	void setMeteo(const meteo* Meteo) { _weather = const_cast<meteo*>(Meteo); }

	/*!
    * \fn void purgeMatRes()
    * \brief Libere la memoire occupee par MatRes
    */
    void purgeMatRes();

    /*!
    * \fn void clear()
    * \brief Efface tous les tableaux
    */
    void clear() { sources.clear(); recepteurs.clear(); _plan.clear(); temps.clear(); purgeMatRes(); }

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
    * \fn void setTimeStep(const decimal& tt)
    * \brief Modification du pas de temps
    * \param tt nouveau pas de discretisation du temps
    */
    void setTimeStep(const decimal& tt) { h = tt; }

    /*!
    * \fn void setTriangle(vec3* triangle)
    * \brief Ajoute des triangles (objets) a la geometrie
    * \param triangle tableau de vec3, represente l'objet que l'on desire ajouter a la geometrie
    */
    void setTriangle(vec3* triangle) { _plan.push_back(triangle); }

    /*!
    * \fn decimal distance_max()
    * \brief Calcul de la distance maximale entre les sources et les recepteurs
    * \return rend la distance maximale
    */
    decimal distance_max();

    /*!
    * \fn void createTemps()
    * \brief Creation du vecteur temps
    */
    void createTemps();

    /*!
    * \fn vector<vec3> EqRay(vector<vec3> y0, meteo Meteo)
    * \brief Fonction definissant l'equation eikonale
    * \param y0 vecteur initial
    * \return rend un point du rayon sous la forme (position, normale)
    */
    vector<vec3> EqRay(const vector<vec3>& y0);

    /*!
    * \fn vec3 intersection(vec3 S, vec3 R, vec3*A, int &reflexion, vec3 nExt_plan, vec3 SR)
    * \brief Calcule le point d'intersection entre un plan et une droite
    * \param S source
    * \param R recepteur
    * \param A Tableau contenant les 3 sommets de la face (tous les sommets doivent etre coplanaires)
    * \param reflexion variable test pour savoir si on a ou non trouve une intersection entre la droite SR et le plan A
    * \param nExt_plan normale exterieure au plan considere
    * \param SR vecteur source-recepteur
    * \return rend le point d'intersection entre le plan A et la droite SR.
    */
    vec3 valideIntersection(const vec3& S, const vec3& R, const vec3* A, int& reflexion, const vec3& nExt_plan, const vec3& SR);

	void intersection(const unsigned int& timer, RayCourb& current, vector<vec3>& Y_t0, vector<vec3>& Y_t1);

    /*!
    * \fn RayCourb RK4(vector<vec3> y0, vector<vec3*> plan, vec3 source)
    * \brief Algorithme de Runge-Kutta d'ordre 4
    * \param y0 vecteur initial
    * \param plan geometrie
    * \param S source
    * \return rend un rayon (liste de points de l'espace)
    */
    RayCourb RK4(const vector<vec3>& y0);

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
     * \fn loadRayFile(vector<vec3>& tableau_norm)
     * \brief Chargement d'un fichier definissant les angles de depart des rayons
     */
    void loadRayFile(vector<vec3>& tableau_norm);

	/*!
	 * \fn void setSampler(Sampler* generator);
	 * \brief set ray generator
	 */
	void setSampler( Sampler *generator ) { _sampler = generator; }

	/*!
	 * \fn Sampler* getSampler(Sampler* generator);
	 * \brief get ray generator for modification 
	 */
	Sampler* getSampler() { return _sampler; } 

	/*!
	 * \fn void setLauchType(const unsigned int &launchType)
	 * \brief define how rays will be created;
	 */
	void setLaunchType(const unsigned int &launchType) { _launchType = launchType; init(); }

private :
	void init();

	void save(); /*!< save rays to a file */

public :
    // Donnees membres :

    vector<vec3> sources;                 /*!< vector contenant les sources */
    vector<vec3> recepteurs;              /*!<  vector contenant les recepteurs */
    vector<vec3*> _plan;                   /*!<  "liste" des objets definis par 3 points */
    meteo *_weather;                        /*!<  meteo */
	Sampler *_sampler;						/*!< ray generator */

    decimal h;                                /*!<  pas de discretisation */
    decimal TMax;                             /*!<  temps de propagation maximal */
    vector<decimal> temps;                    /*!<  [0:h:temps_max] vecteur des temps ou l'on resouds */
    decimal dmax;                             /*!<  distance maximale parcourue par les rayons */
    decimal initialAngleTheta;                /*!<  angle de tir initial selon theta */
    decimal finalAngleTheta;                  /*!<  angle de tir final selon theta */
    decimal initialAnglePhi;                  /*!<  angle de tir initial selon phi */
    decimal finalAnglePhi;                    /*!<  angle de tir final selon phi */

    unsigned int nbRay;                 /*!<  nombre de rayons que l'on lance */
    unsigned int _launchType;            /*!<  mode de lancer des rayons 1:horizontal / 2:vertical / 3:spheric / 4:file */
    bool wantOutFile;                   /*!<  true if outputfile wanted */
    string ray_fileName;                /*!<  filename of file containing angles of rays */
    string out_fileName;                /*!<  filename of the output file */


    vector<RayCourb*> MatRes;           /*!<  tableau contenant les rayons obtenus pour chaque source */
};

/*!
* \fn decimal angle_depart(decimal a, decimal c, decimal d, decimal h)
* \brief Calcul l'angle avec lequel le rayon doit etre lance afin de parcourir au minimum la distance d
* \param a gradient de celerite
* \param c celerite au sol
* \param d distance minimale a parcourir
* \param h hauteur de la source
* \return rend l'angle du lancer de rayon.
*/
decimal angle_depart(const decimal& a, const decimal& c, const decimal& d, const decimal& h);

#endif // __LANCER_H