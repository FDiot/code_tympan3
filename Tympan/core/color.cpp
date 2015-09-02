/*
 * Copyright (C) <2012-2014> <EDF-R&D> <FRANCE>
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


#include <limits>
#include <cmath>

#include "color.h"

#ifndef M_PI
  ///Pi.
  #define M_PI        3.1415926535897932384626433832795
#endif

const OColor OColor::BLACK      = OColor(0.0f, 0.0f, 0.0f);
const OColor OColor::BLUE       = OColor(0.0f, 0.0f, 1.0f);
const OColor OColor::GREEN      = OColor(0.0f, 1.0f, 0.0f);
const OColor OColor::CYAN       = OColor(0.0f, 1.0f, 1.0f);
const OColor OColor::RED        = OColor(1.0f, 0.0f, 0.0f);
const OColor OColor::MAGENTA    = OColor(1.0f, 0.0f, 1.0f);
const OColor OColor::YELLOW     = OColor(1.0f, 1.0f, 0.0f);
const OColor OColor::WHITE      = OColor(1.0f, 1.0f, 1.0f);

const float OColor::equality_threshold = 1e3 * std::numeric_limits<float>::epsilon();

bool OColor::operator==(const OColor& c) const
{
    return (float_eq(r, c.r) &&
            float_eq(g, c.g) &&
            float_eq(b, c.b) &&
            float_eq(a, c.a));
}


unsigned int OColor::getAsRGBA() const
{
    // Conversion au format 32 bits
    // (RGBA = 8888)

    unsigned char val8;
    unsigned int val32 = 0;

    // Red
    val8 = static_cast<unsigned char>(r * 255);
    val32 = val8 << 24;

    // Green
    val8 = static_cast<unsigned char>(g * 255);
    val32 += val8 << 16;

    // Blue
    val8 = static_cast<unsigned char>(b * 255);
    val32 += val8 << 8;

    // Alpha
    val8 = static_cast<unsigned char>(a * 255);
    val32 += val8;

    return val32;
}

void OColor::setAsRGBA(const unsigned int val)
{
    // Conversion au format 32 bits
    // (RGBA = 8888)

    unsigned int val32 = val;

    // Red
    r = ((val32 >> 24) & 0xFF) / 255.0f;

    // Green
    g = ((val32 >> 16) & 0xFF) / 255.0f;

    // Blue
    b = ((val32 >> 8) & 0xFF) / 255.0f;

    // Alpha
    a = (val32 & 0xFF) / 255.0f;
}

void OColor::getAsHSB(float& hue, float& saturation, float& brightness) const
{
    // Clamp RGB color
    OColor c = clampCopy();

    // Get min and max rgb value
    float min = std::min<float>(std::min<float>(c.r, c.g), c.b);
    float max = std::max<float>(std::max<float>(c.r, c.g), c.b);

    // Get delta between min and max value
    float delta = max - min;

    // Brightness is the max value
    brightness = max;

    if (delta == 0.0f)
    {
        // This is a gray, no chroma
        hue = 0.0f;
        saturation = 0.0f;
        return;
    }

    // Chromatic
    saturation = delta / max;

    if (c.r == max)
    {
        hue = ((c.g - c.b) / delta) / 6.0f;
    }
    else if (c.g == max)
    {
        hue = (2.0f + (c.b - c.r) / delta) / 6.0f;
    }
    else if (c.b == max)
    {
        hue = (4.0f + (c.r - c.g) / delta) / 6.0f;
    }

    // Wrap hue
    if (hue > 1.0f)
    {
        hue -= (int)hue;
    }
    else if (hue < 0.0f)
    {
        hue += (int)hue + 1;
    }
}

void OColor::setAsHSB(const float hue, const float saturation, const float brightness)
{
    float h = hue;
    float s = saturation;
    float br = brightness;

    // Wrap hue
    if (h > 1.0f)
    {
        h -= (int)h;
    }
    else if (h < 0.0f)
    {
        h += (int)h + 1;
    }

    // Set alpha = 100%;
    a = 1.0;

    // Clamp saturation / brightness
    s = std::min<float>(s, 1.0f);
    s = std::max<float>(s, 0.0f);
    br = std::min<float>(br, 1.0f);
    br = std::max<float>(br, 0.0f);

    if (br == 0.0f)
    {
        // Early exit, this has to be black
        r = g = b = 0.0f;
        return;
    }

    if (s == 0.0f)
    {
        // Early exit, this has to be grey

        r = g = b = br;
        return;
    }

    float hueDomain  = h * 6.0f;
    if (hueDomain >= 6.0f)
    {
        // Wrap around, and allow mathematical errors
        hueDomain = 0.0f;
    }
    unsigned short domain = (unsigned short)hueDomain;
    float f1 = br * (1 - s);
    float f2 = br * (1 - s * (hueDomain - domain));
    float f3 = br * (1 - s * (1 - (hueDomain - domain)));

    switch (domain)
    {
        case 0:
            // Red domain; green ascends
            r = br;
            g = f3;
            b = f1;
            break;
        case 1:
            // Yellow domain; red descends
            r = f2;
            g = br;
            b = f1;
            break;
        case 2:
            // Green domain; blue ascends
            r = f1;
            g = br;
            b = f3;
            break;
        case 3:
            // Cyan domain; green descends
            r = f1;
            g = f2;
            b = br;
            break;
        case 4:
            // Blue domain; red ascends
            r = f3;
            g = f1;
            b = br;
            break;
        case 5:
            // Magenta domain; blue descends
            r = br;
            g = f1;
            b = f2;
            break;
    }
}

void OColor::clamp()
{
    // Red
    if (r < 0.0f)
    {
        r = 0.0f;
    }
    else if (r > 1.0f)
    {
        r = 1.0f;
    }

    // Green
    if (g < 0.0f)
    {
        g = 0.0f;
    }
    else if (g > 1.0f)
    {
        g = 1.0f;
    }

    // Blue
    if (b < 0.0f)
    {
        b = 0.0f;
    }
    else if (b > 1.0f)
    {
        b = 1.0f;
    }

    // Alpha
    if (a < 0.0f)
    {
        a = 0.0f;
    }
    else if (a > 1.0f)
    {
        a = 1.0f;
    }
}

OColor OColor::clampCopy() const
{
    OColor color = *this;
    color.clamp();
    return color;
}


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
