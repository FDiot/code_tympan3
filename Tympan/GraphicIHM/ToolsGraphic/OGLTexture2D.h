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

#ifndef __O_GLTEXTURE2D__
#define __O_GLTEXTURE2D__

#include "OGLTexture.h"

/**
 * Classe pour la gestion des textures 2D OpengGL.
 */
class OGLTexture2D : public OGLTexture
{
public:
    OGLTexture2D();
    virtual ~OGLTexture2D();

    /*
     * Charge la texture a partir d'un fichier.
     */
    bool load(const char* filename);
};

// Smart pointer sur OGLTexture2D.
typedef SmartPtr<OGLTexture2D> LPOGLTexture2D;

#endif // __O_GLTEXTURE2D__
