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
 */

#ifndef _GLOBAL_MACROS_
#define _GLOBAL_MACROS_

#include "Defines.h"
#include "OGeometrie.h"

///Pour la gestion des assertions.
#include <assert.h>

#include <complex>

///Type pour les valeurs complexes.
typedef std::complex<double> TYComplex;

#define CPLX_UN TYComplex(1.0, 0.0)
#define CPLX_MUN TYComplex(-1.0, 0.0)
#define CPLX_J TYComplex(0.0, 1.0)

/**
 * Calcul de la cotangente hyperbolique d'un complexe.
 */
inline TYComplex cotanh(const TYComplex& valeur)
{
    double reel = 0.0 ;
    double imag = 0.0 ;

    if (valeur.real() >= 100.0)
    {
        reel = 100.0 ;
    }
    else if (valeur.real() <= -100.0)
    {
        reel = -100.0 ;
    }
    else // -100 <= valeur.real() <= 100
    {
        reel = valeur.real();
    }

    imag = valeur.imag();

    TYComplex valeurTravail = TYComplex(reel, imag);

    return (std::cosh(valeurTravail) / std::sinh(valeurTravail));
}


/**
 * Retourne autant de tabulations que d'indentations demandees.
 *
 * \param Le nombre d'indentation demandees.
 *
 * \return Une chaine de caractere composee de tabulations.
 */
inline std::string indentNbToStr(int indentNb)
{
    std::string str = "";

    while (indentNb--)
    {
        str += "\t";
    }

    return str;
}

#if TY_ARCH_TYPE == TY_ARCHITECTURE_64
/**
 * Convertit un <code>unsigned int</code> en une chaine de caractere.
 *
 * \param val La valeur a convertir.
 *
 * \return La valeur convertie.
 */
inline std::string uintToStr(unsigned int val)
{
    std::ostringstream oss;
    oss << val;
    return oss.str();
}

/**
 * Convertit un <code>size_t</code> en une chaine de caractere.
 *
 * \param val La valeur a convertir.
 *
 * \return La valeur convertie.
 */
inline std::string uintToStr(size_t val)
{
    std::ostringstream oss;
    oss << val;
    return oss.str();
}
#if TY_COMPILER == TY_COMPILER_MSVC
/**
 * Convertit un <code>unsigned long</code> en une chaine de caractere.
 *
 * \param val La valeur a convertir.
 *
 * \return La valeur convertie.
 */
inline std::string uintToStr(unsigned long val)
{
    std::ostringstream oss;
    oss << val;
    return oss.str();
}
#endif
#else
/**
 * Convertit un <code>size_t</code> en une chaine de caractere.
 *
 * \param val La valeur a convertir.
 *
 * \return La valeur convertie.
 */
inline std::string uintToStr(size_t val)
{
    std::ostringstream oss;
    oss << val;
    return oss.str();
}

/**
 * Convertit un <code>unsigned long</code> en une chaine de caractere.
 *
 * \param val La valeur a convertir.
 *
 * \return La valeur convertie.
 */
inline std::string uintToStr(unsigned long val)
{
    std::ostringstream oss;
    oss << val;
    return oss.str();
}
#endif

/**
 * Convertit un <code>int</code> en une chaine de caractere.
 *
 * \param val La valeur a convertir.
 *
 * \return La valeur convertie.
 */
inline std::string intToStr(int val)
{
    std::ostringstream oss;
    oss << val;
    return oss.str();
}

/**
 * Convertit un <code>float</code> en une chaine de caractere.
 * Avec une precision apres la virgule par defaut de 6 digits.
 *
 * \param val La valeur a convertir.
 *
 * \return La valeur convertie.
 */
inline std::string floatToStr(float val)
{
    std::ostringstream oss;
    oss << val;
    return oss.str();
}

/**
 * Convertit un <code>double</code> en une chaine de caractere.
 * Avec une precision apres la virgule par defaut de 6 digits.
 *
 * \param val La valeur a convertir.
 *
 * \return La valeur convertie.
 */
inline std::string doubleToStr(double val)
{
    std::ostringstream oss;
    oss << val;
    return oss.str();
}

/**
 * Convertit un <code>double</code> en une chaine de caractere
 * avec une precision donnee.
 * La valeur est arrondie en fonction de sa precision.
 *
 * \param val La valeur a convertir.
 * \param precision La precision pour la conversion.
 *
 * \return La valeur convertie.
 */
inline std::string doubleToStrPre(double val, int precision = 2)
{
    std::ostringstream oss;
    oss.precision(precision);
    oss << std::fixed << val;
    return oss.str();
}

template <class T>
inline T fromString(const std::string& str, std::ios_base & (*f)(std::ios_base&) = std::dec)
{
    T value;
    std::istringstream iss(str);
    iss >> f >> value;
    return value;
}

#ifndef SAFE_DELETE
/**
 * Delete propre sur un objet.
 */
#define SAFE_DELETE(_p)         { if ((_p) != NULL) { delete(_p); (_p) = NULL; } }
#endif

#ifndef SAFE_DELETE_LIST
/**
 * Delete propre sur un tableau.
 */
#define SAFE_DELETE_LIST(_p)    { if ((_p) != NULL) { delete[](_p); (_p) = NULL; } }
#endif


#endif // _GLOBAL_MACROS_
