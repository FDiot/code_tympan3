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
 
/*
 *
 *
 *
 *
 */

#ifndef __TY_COLOR_MANAGER__
#define __TY_COLOR_MANAGER__

#include "OColor.h"

/**
 Classe utilitaire permettant de gerer les palettes de couleurs pour les cartographies
*/

class TYColorManager
{
public:
    /// Generation de la table de couleurs basee sur une rampe S-Curve ( y = (1-cos(pi*x))/2 )
    static void getSCurveColorTable(const unsigned int& nbColors, const float* hueRange, const float* saturationRange, const float* valueRange, OLookupTable& outColors);

    /// Generation de la table de couleurs basee sur une rampe lineaire ( y = x+0.5/255 )
    static void getLinearColorTable(const unsigned int& nbColors, const float* hueRange, const float* saturationRange, const float* valueRange, OLookupTable& outColors);

    /// Generation de la table de couleurs basee sur une rampe sqrt ( y = sqrt(x)+0.5/255 )
    static void getSqrtColorTable(const unsigned int& nbColors, const float* hueRange, const float* saturationRange, const float* valueRange, OLookupTable& outColors);

    /// Generation de la table de couleurs a partir d'une repartition gaussienne en R, G, B
    static void getGaussColorTable(const unsigned int& nbColors, const float& alpha1, const float& alpha2, OLookupTable& outColors);
};

#endif //__TY_COLOR_MANAGER__
