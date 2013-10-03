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

#ifndef __O_GLTEXTURE__
#define __O_GLTEXTURE__

#if _WIN32
#include <windows.h>
#endif //_WIN32

#include <GL/gl.h>

#include "Tympan/Tools/OSmartPtr.h"
/**
 * Interface pour la gestion des textures OpengGL.
 *
 * @author Projet_Tympan
 *
 */
class OGLTexture : public IRefCount
{
public:
    OGLTexture();
    virtual ~OGLTexture();

    /*
     * Charge la texture a partir d'un fichier.
     * Methode virtuelle pure a implementer.
     */
    virtual bool load(const char* filename) = 0;

    /*
     * Attache la texture.
     */
    virtual void bind();

    /*
     * Liberation de la memoire.
     */
    virtual void free();

protected:
    // Generation d'une id de texture
    void genTexture();

    // Id de texture OpenGL
    GLuint id;

};

#endif // __O_GLTEXTURE__
