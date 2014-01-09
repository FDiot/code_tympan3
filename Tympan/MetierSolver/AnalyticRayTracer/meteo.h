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
* \file meteo.h
* \author Projet_Tympan
*/

#ifndef __METEO_H
#define __METEO_H

#include <map>
#include "../AcousticRaytracer/Geometry"

/*! \class meteo
* \brief classe representant les donnees meteo relatives au calcul.
*/
class meteo
{
public:
    double gradC;      /*!< gradient de la celerite */
    double gradV;      /*!< gradient du vent */
    double c0;         /*!< celerite en z = 0 */
	double windDirection;  /*!< angle du vent */


    /*!
     *  \brief Constructeur
     *
     *  Constructeurs de la classe meteo par defaut et par passage d'arguments
     *
     */
    meteo();
    meteo(const double& gradC, const double& gradV, const double& windDir, const double& c0);

    /*!
    * \fn bool setGradC(const double& g)
    * \brief Modifie la valeur du gradient de celerite
    * \param g nouvelle valeur que l'on souhaite attribuer a notre gradient
    */
    void setGradC(const double& g) { gradC = g; }

    /*!
    * \fn bool setGradV(const double& g)
    * \brief Modifie la valeur de la vitesse du vent
    * \param g nouvelle valeur que l'on souhaite attribuer a notre vent
    */
    void setGradV(const double& g) { gradV = g; }

	    /*!
    * \fn bool setGradV(const double& g)
    * \brief Modifie la valeur de la vitesse du vent
    * \param g nouvelle valeur que l'on souhaite attribuer a notre vent
    */
    void setWindDirection(const double& dir) { windDirection = dir; }

    /*!
    * \fn bool setC0(const double& c)
    * \brief Modifie la valeur de c0
    * \param c nouvelle valeur que l'on souhaite attribuer a c0
    */
    void setC0(const double& c) { c0 = c; }

    /*!
    * \fn decimal cLin(const vec3& P, const meteo& Meteo, vec3& grad)
    * \brief Prend en compte la temperature pour le point P
    * \param P Position du rayon
	* \param grad gradient selon z (veleur modifiee) 
    */
 	double cLin(const vec3& P, vec3& grad) const;

    /*!
    * \fn vec3 vent(const vec3& P, map<pair<int, int>, decimal> &jacob)
    * \brief Prend en compte le vent pour le point P
    * \param P Position du rayon
	* \param jacob : jacobien (veleur modifiee) 
    */	
	vec3 vent(const vec3& P, std::map<std::pair<int, int>, decimal> &jacob) const;

    /*!
     *  \brief Destructeur
     *
     *  Destructeur de la classe Lancer
     */
    ~meteo();
};

#endif //__METEO_H