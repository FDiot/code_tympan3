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
 */

#include <cmath>
#include "TYColorManager.h"

#ifndef M_PI
///Pi.
#define M_PI        3.1415926535897932384626433832795
#endif


void TYColorManager::getSCurveColorTable(const unsigned int& nbColors, const float* hueRange, const float* saturationRange, const float* valueRange, OLookupTable& outColors)
{
    outColors.resize(nbColors);

    float hueStep = (hueRange[1] - hueRange[0]) / (nbColors - 1);
    float saturationStep = (saturationRange[1] - saturationRange[0]) / (nbColors - 1);
    float valueStep = (valueRange[1] - valueRange[0]) / (nbColors - 1);

    float h, s, v;
    OColor rgb;
    for (unsigned int i = 0; i < nbColors; ++i)
    {
        h = hueRange[0] + i * hueStep;
        s = saturationRange[0] + i * saturationStep;
        v = valueRange[0] + i * valueStep;
        rgb.setAsHSB(h, s, v);

        outColors[i].r = (1.0f - cos(rgb.r * static_cast<float>(M_PI))) * 0.5f;
        outColors[i].g = (1.0f - cos(rgb.g * static_cast<float>(M_PI))) * 0.5f;
        outColors[i].b = (1.0f - cos(rgb.b * static_cast<float>(M_PI))) * 0.5f;
    }
}

void TYColorManager::getLinearColorTable(const unsigned int& nbColors, const float* hueRange, const float* saturationRange, const float* valueRange, OLookupTable& outColors)
{
    outColors.resize(nbColors);

    float hueStep = (hueRange[1] - hueRange[0]) / (nbColors - 1);
    float saturationStep = (saturationRange[1] - saturationRange[0]) / (nbColors - 1);
    float valueStep = (valueRange[1] - valueRange[0]) / (nbColors - 1);

    float h, s, v;
    OColor rgb;
    for (unsigned int i = 0; i < nbColors; ++i)
    {
        h = hueRange[0] + i * hueStep;
        s = saturationRange[0] + i * saturationStep;
        v = valueRange[0] + i * valueStep;
        rgb.setAsHSB(h, s, v);

        //XXX What is the meaning of this offset?
        outColors[i].r = rgb.r + 0.5f / 255.0f;
        outColors[i].g = rgb.g + 0.5f / 255.0f;
        outColors[i].b = rgb.b + 0.5f / 255.0f;
    }
}

void TYColorManager::getSqrtColorTable(const unsigned int& nbColors, const float* hueRange, const float* saturationRange, const float* valueRange, OLookupTable& outColors)
{
    outColors.resize(nbColors);

    float hueStep = (hueRange[1] - hueRange[0]) / (nbColors - 1);
    float saturationStep = (saturationRange[1] - saturationRange[0]) / (nbColors - 1);
    float valueStep = (valueRange[1] - valueRange[0]) / (nbColors - 1);

    float h, s, v;
    OColor rgb;
    for (unsigned int i = 0; i < nbColors; ++i)
    {
        h = hueRange[0] + i * hueStep;
        s = saturationRange[0] + i * saturationStep;
        v = valueRange[0] + i * valueStep;
        rgb.setAsHSB(h, s, v);

        outColors[i].r = std::sqrt(rgb.r) + 0.5f / 255.0f;
        outColors[i].g = std::sqrt(rgb.g) + 0.5f / 255.0f;
        outColors[i].b = std::sqrt(rgb.b) + 0.5f / 255.0f;
    }
}

void TYColorManager::getGaussColorTable(const unsigned int& nbColors, const float& alpha1, const float& alpha2, OLookupTable& outColors)
{
    outColors.resize(nbColors);

    float colors = static_cast<float>(nbColors);
    for (unsigned int i = 0; i < nbColors; ++i)
    {
        OColor& color = outColors[i];
        color.r = exp(-(colors - i) * (colors - i) * alpha2 * alpha2 / (colors * colors));
        color.g = exp(-(i - colors / 2) * (i - colors / 2) * alpha1 * alpha1 / (colors * colors));
        color.b = exp(-(i * i * alpha2 * alpha2 / (colors * colors)));
    }
}
