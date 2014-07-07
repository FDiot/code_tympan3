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

#ifndef TY_COLOR
#define TY_COLOR

#include <vector>
#include <cmath>

#include "Tympan/core/smartptr.h"

#undef min
#undef max

/**
 * Classe definition un objet couleur.
 */
class OColor
{
public:
    float r, g, b, a;

public:
    /**
     * Constructeurs
     */
    OColor();
    OColor(const OColor& c);
    OColor(float* c);
    OColor(float red, float green, float blue, float alpha = 1.0f);

    /**
     * Operateur d'asignation
     */
    void operator=(const OColor& c);

    /**
     * Operateurs de cast
     */
    operator float* ();
    operator const float* () const;

    /**
     * Get de la couleur en RGBA
     */
    unsigned int getAsRGBA() const;

    /**
     * Set de la couleur en RGBA
     */
    void setAsRGBA(const unsigned int val);

    /**
     * Get de la couleur en HSB (Hue, Saturation and Brightness)
     */
    void getAsHSB(float& hue, float& saturation, float& brightness) const;

    /**
     * Set de la couleur en HSB (Hue, Saturation and Brightness)
     */
    void setAsHSB(const float hue, const float saturation, const float brightness);

    /**
     * Clamp les couleurs entre [0, 1]
     */
    void clamp();
    OColor clampCopy() const;

    bool operator==(const OColor& other) const;

    /**
     * Constantes
     */
    static const OColor BLACK;
    static const OColor BLUE;
    static const OColor GREEN;
    static const OColor CYAN;
    static const OColor RED;
    static const OColor MAGENTA;
    static const OColor YELLOW;
    static const OColor WHITE;

protected:
    static const float equality_threshold;

    static bool float_eq(float a, float b, float epsilon = equality_threshold)
    {
        return (std::fabs(a - b) <= epsilon * std::max(std::fabs(a), std::fabs(b)));
    }
};

///Collection de OColor
typedef std::vector<OColor> OLookupTable;

inline OColor::OColor() : r(1.0f), g(1.0f), b(1.0f), a(1.0f)
{

}

inline OColor::OColor(const OColor& c) : r(c.r), g(c.g), b(c.b), a(c.a)
{

}

inline OColor::OColor(float red, float green, float blue, float alpha) : r(red), g(green), b(blue), a(alpha)
{

}

inline OColor::OColor(float* c)
{
    r = c[0];
    g = c[1];
    b = c[2];
    a = c[3];
}

inline void OColor::operator=(const OColor& c)
{
    r = c.r;
    g = c.g;
    b = c.b;
    a = c.a;
}

inline OColor::operator float* ()
{
    return &r;
}

inline OColor::operator const float* () const
{
    return &r;
}


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


#endif // TY_COLOR
