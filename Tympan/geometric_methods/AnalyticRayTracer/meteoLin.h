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

#include <array>
#include "Tympan/models/common/mathlib.h"
#include "meteo.h"

#ifndef __METEO_LIN_H
#define __METEO_LIN_H

/*! \class meteoLin
*   \brief Class to define linear gradient for wind and sound speed.
*/
using namespace std;

class meteoLin : public meteo
{
public:
	/// Default constructor
    meteoLin() : meteo() {}
    /// Constructor
    meteoLin(const double& windAngle, const double& sound_speed, const double& gradC = 0., const double& gradV = 0.) : meteo(windAngle, sound_speed),
        grad_C(gradC),
        grad_V(gradV)
    {
        init();
    }

    /// Destructor
    ~meteoLin() {};

    /// Set speed gradient
    void setGradC(const double& g) { grad_C = g; }
    /// Get speed gradient
    double getGradC() const { return grad_C; }

    /// Set wind gradient
    void setGradV(const double& g) { grad_V = g; init(); }
    /// Get wind gradient
    double getGradV() const { return grad_V; }

    /*!
    * \fn double cTemp(const vec3& P, vec3& grad) const;
    * \brief Compute speed gradient and sound speed on a position
    * \param [in] P Position
    * \param [out] grad Speed gradient according to z
    * \return Sound speed at position z
    */
    double cTemp(const vec3& P, vec3& grad) const;

    /*!
    * \fn vec3 cWind(const vec3& P) const;
    * \brief Compute wind on a given position
    * \param P Position
    * \return Wind
    */
    vec3 cWind(const vec3& P) const;

    /*!
     * \fn const double** getJacobMatrix()
     * \brief Get the jacobian matrix
     */
    virtual const array< array<double, 3>, 3 >&  getJacobMatrix() { return jacob_matrix; }

    /// Initialize the Jacobian matrix
    virtual void init();

private:
    double grad_C;      //!< Sound speed gradient
    double grad_V;      //!< Wind gradient

    array< array<double, 3>, 3 > jacob_matrix; //!< Jacobian matrix
};

#endif //__METEO_LIN_H
