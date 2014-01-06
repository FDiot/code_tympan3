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



#include <stdio.h>
#include "Tympan/Tools/OColor.h"
#include "OGLFont.h"
#include "OImageGlf.h"
#include "TYImageManager.h"

OGLFont::OGLFont() : _image(0)
{

}

OGLFont::~OGLFont()
{

}

bool OGLFont::load(const char* filename)
{
    if (id > 0)
    {
        free();
    }

    genTexture();

    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // On recupere l'image
    _image = (OImageGlf*)TYImageManager::get()->getImage(filename);

    if (!_image)
    {
        return false;
    }

    // On recupere les informations
    GLsizei w = _image->getSizeX();
    GLsizei h = _image->getSizeY();

    unsigned int bpp = _image->getDepth() / 8;
    if (bpp != 2)
    {
        return false;
    }

    GLenum format = GL_LUMINANCE_ALPHA;

    // Creation de la texture
    glTexImage2D(GL_TEXTURE_2D, 0, bpp, w, h, 0, format, GL_UNSIGNED_BYTE, _image->getData());

    return true;
}

void OGLFont::drawText(const std::string& msg, const OColor& color, double x, double y) const
{
    if (!_image)
    {
        return;
    }

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    // Couleur du texte
    glColor3fv(color);

    // On dessine un quad
    glBegin(GL_QUADS);

    // On traite chaque caractere
    for (unsigned int i = 0; i < msg.length(); ++i)
    {
        if (!_image->isCharValid(msg[i]))
        {
            continue;
        }

        OImageGlf::OGLFontChar c = _image->getChar(msg[i]);

        float width = c._dx * _image->getSizeX();
        float height = c._dy * _image->getSizeY();

        // Vertices et coordonnees de textures
        glTexCoord2f(c._tx0, c._ty0);
        glVertex2d(x, y);
        glTexCoord2f(c._tx0, c._ty1);
        glVertex2d(x, y - height);
        glTexCoord2f(c._tx1, c._ty1);
        glVertex2d(x + width, y - height);
        glTexCoord2f(c._tx1, c._ty0);
        glVertex2d(x + width, y);

        // Prochain caractere.
        x += width;
    }

    // Fin du quad
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
}
