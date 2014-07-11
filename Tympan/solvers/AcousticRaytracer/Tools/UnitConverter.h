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

#include "Tympan/models/common/mathlib.h"

namespace Tools
{
/*!
 * \fn fromRadianToCarthesien(decimal tetha, decimal phi, vec3& result);
 * \brief   Convert spheric coordinates to cartesian coordinates
 *          In this function :
 *                  + theta is the polar angle where 0 is on equatorius
 *                  + phi is the equatorial angle
 *          This function is used by samplers that genarates rays
 */
void fromRadianToCarthesien(decimal tetha, decimal phi, vec3& result);

/*!
 * \fn fromRadianToCarthesien2(decimal tetha, decimal phi, vec3& result);
 * \brief   Convert spheric coordinates to cartesian coordinates
 *          In this function :
 *                  + theta is the polar angle where 0 is on north pole
 *                  + phi is the equatorial angle
 *          This function is used by diffraction events to gernarate secondary rays
 */
void fromRadianToCarthesien2(decimal tetha, decimal phi, vec3& result);
};