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

#ifndef __O_IMAGEGLF__
#define __O_IMAGEGLF__

#include "OImage.h"
#include <vector>

class OImageGlf : public OImage
{
public:
    struct OGLFontChar
    {
        float _dx;
        float _dy;

        float _tx0, _ty0;
        float _tx1, _ty1;
    };

public:
    OImageGlf();
    virtual ~OImageGlf();

    /**
     * Surcharge de OImage::load
     */
    virtual bool load(const std::string& filename);

    /**
     * Renvoie une reference constante sur un caractere.
     */
    const OGLFontChar& getChar(char c) const;

    /**
     * Renvoie true si le caractere est valide, false sinon.
     */
    bool isCharValid(char c) const;

private:
    // Position des caracteres
    unsigned int _start;
    unsigned int _end;

    // Tableau des caracteres
    std::vector<OGLFontChar> _char;
};

#endif // __O_IMAGEGLF__
