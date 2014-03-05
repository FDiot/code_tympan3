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
* \file meteoLin.h
* \author Projet_Tympan
*/

#include <map>
#include <array>
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/mathlib.h"
#include "meteo.h"

#ifndef __METEO_LIN_H
#define __METEO_LIN_H

/*! \class meteoLin
*   \brief class to define linear gradiant for wind and temperature.
*/

using namespace std;

class meteoLin : public meteo
{
public:
    /*!
     *  \brief Constructeur
     *
     *  Constructeurs de la classe meteo par defaut et par passage d'arguments
     *
     */
    meteoLin() : meteo() {}
    meteoLin(const double& windAngle, const double& sound_speed, const double& gradC = 0., const double& gradV = 0.) : meteo(windAngle, sound_speed),
        grad_C(gradC),
        grad_V(gradV)
    {
        init();
    }

    /*!
     *  \brief Destructeur
     *
     *  Destructeur de la classe Lancer
     */
    ~meteoLin() {};


    /*!
    * \fn bool setGradC(const double& g)
    * \brief Modifie la valeur du gradient de celerite
    * \param g nouvelle valeur que l'on souhaite attribuer a notre gradient
    */
    void setGradC(const double& g) { grad_C = g; }
    double getGradC() const { return grad_C; }

    /*!
    * \fn bool setGradV(const double& g)
    * \brief Modifie la valeur de la vitesse du vent
    * \param g nouvelle valeur que l'on souhaite attribuer a notre vent
    */
    void setGradV(const double& g) { grad_V = g; init(); }
    double getGradV() const { return grad_V; }

    /*!
    * \fn decimal cTemp(const vec3& P, const meteo& Meteo, vec3& grad)
    * \brief Prend en compte la temperature pour le point P
    * \param P Position du rayon
    * \param grad gradient de température selon z (valeur modifiee)
    */
    double cTemp(const vec3& P, vec3& grad) const;

    /*!
    * \fn vec3 vent(const vec3& P, map<pair<int, int>, decimal> &jacob)
    * \brief Prend en compte le vent pour le point P
    * \param P Position du rayon
    * \param jacob : jacobien (veleur modifiee)
    */
    vec3 cWind(const vec3& P) const;

    /*!
     * \fn const double** getJacobMatrix()
     * \brief Get the jacobian matrix
     */
    virtual const array< array<double, 3>, 3 >&  getJacobMatrix() { return jacob_matrix; }

    virtual void init();

private:
    double grad_C;      /*!< gradient de la celerite */
    double grad_V;      /*!< gradient du vent */

    array< array<double, 3>, 3 > jacob_matrix;
};

#endif //__METEO_LIN_H