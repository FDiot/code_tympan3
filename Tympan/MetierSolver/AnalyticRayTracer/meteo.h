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
#include "../AcousticRaytracer/Geometry/mathlib.h"

/*! \class meteo
* \brief classe representant les donnees meteo relatives au calcul.
*/
class meteo
{
public:
    /*!
     *  \brief Constructeur
     *
     *  Constructeurs de la classe meteo par defaut et par passage d'arguments
     *
     */
	meteo() : c0(340.), wind_angle(0.) {}

	meteo(const double& windAngle, const double& sound_speed) : c0(sound_speed), 
															    wind_angle( RADIANS(windAngle) )
	{}

	~meteo() {}

    /*!
    * \fn bool setC0(const double& c)
    * \brief Modifie la valeur de c0
    * \param c nouvelle valeur que l'on souhaite attribuer a c0
    */
    virtual void setC0(const double& c) { c0 = c; }
	virtual double getC0() const { return c0; }

	/*!
	 * \fn void setWindAngle(const double& windAngle)
	 * \brief define wind angle 0 means wind from north to south
	 */
	virtual void setWindAngle(const double& windAngle) { wind_angle = RADIANS(windAngle); }
	virtual double getWindAngle() const { return DEGRES(wind_angle); }


protected:
    double c0;				/*!< sound speed for z = 0 */
	double wind_angle;		/*!< wind angle in radian 0 for a wind from north to south */
};

#endif //__METEO_H