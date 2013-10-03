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

#ifndef __O_GLFONT__
#define __O_GLFONT__

#include "OGLTexture.h"

#include <string>

class OColor;
class OImageGlf;

class OGLFont : public OGLTexture
{
public:
    OGLFont();
    virtual ~OGLFont();

    /*
     * Charge la police a partir d'un fichier.
     */
    bool load(const char* filename);

    /**
     * Affiche un texte
     */
    void drawText(const std::string& msg, const OColor& color, double x, double y) const;

private:
    OImageGlf* _image;
};

// Smart pointer sur OGLFont.
typedef SmartPtr<OGLFont> LPOGLFont;

#endif // _O_GLFONT_H_
