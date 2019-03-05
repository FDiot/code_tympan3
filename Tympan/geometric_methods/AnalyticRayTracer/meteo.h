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
#include "Tympan/models/common/mathlib.h"

/*! \class meteo
* \brief Weather data class for the simulation.
*/
class meteo
{
public:
    /// Default constructor
    meteo() : c0(340.), wind_angle(0.) {}
    /// Constructor
    meteo(const double& windAngle, const double& sound_speed) : c0(sound_speed),
        wind_angle(RADIANS(windAngle)) {}

    ~meteo() {}

    /*!
    * \fn bool setC0(const double& c)
    * \brief Set sound speed
    * \param c New sound speed
    */
    virtual void setC0(const double& c) { c0 = c; }
    /// Get sound speed
    virtual double getC0() const { return c0; }

    /*!
     * \fn void setWindAngle(const double& windAngle)
     * \brief Define wind: angle given in windrose orientation (clockwise angle with 0 for a wind from north to south) 
     */
    virtual void setWindAngle(const double& windAngle) { wind_angle = RADIANS(windAngle); init(); }
    /// Get the windrose oriented wind angle in radian (clockwise angle with 0 for a wind from north to south) 
    virtual double getWindAngle() const { return DEGRES(wind_angle); }

    /*!
     * \fn void init()
     * \brief Init parameters as needed
     */
    virtual void init() {}

protected:
    double c0;              //!< Ground sound speed (z = 0)
    double wind_angle;      //!< The windrose oriented wind angle in radian (clockwise angle with 0 for a wind from north to south) 
};

#endif //__METEO_H
