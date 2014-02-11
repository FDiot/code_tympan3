/*
 * Copyright (C) <2013> <EDF-R&D> <FRANCE>
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

/*
 *
 *
 *
 *
 */

#ifndef __TY_TOOLS_GEOMETRICAL_FUNCTION__
#define __TY_TOOLS_GEOMETRICAL_FUNCTION__


//! Compute the squared distance between a point and a segment.
/*!
 \param point_x
 \param point_y
 \param a_x
 \param a_y
 \param b_x
 \param b_y
  \return Squared distance.
 */
double compute_segment_point_square_distance(double point_x, double point_y,
                                             double a_x, double a_y,
                                             double b_x, double b_y)
{
    double l2 = (a_x - b_x) * (a_x - b_x) + (a_y - b_y) * (a_y - b_y) ;
    if (l2 != 0)
    {
        double t = ((point_x - a_x) * (b_x - a_x) + (point_y - a_y) * (b_y - a_y)) / l2;
        if (t < 0)
        {
            return (point_x - a_x) * (point_x - a_x) + (point_y - a_y) * (point_y - a_y);
        }
        else if (t > 1)
        {
            return (point_x - b_x) * (point_x - b_x) + (point_y - b_y) * (point_y - b_y);
        }
        else
        {
            return (point_x - (a_x + t * (b_x - a_x)))
                   * (point_x - (a_x + t * (b_x - a_x)))
                   + (point_y - (a_y + t * (b_y - a_y)))
                   * (point_y - (a_y + t * (b_y - a_y)));
        }
    }
    return (point_x - a_x) * (point_x - a_x) + (point_y - a_y) * (point_y - a_y);
}


#endif //  __TY_TOOLS_GEOMETRICAL_FUNCTION__
