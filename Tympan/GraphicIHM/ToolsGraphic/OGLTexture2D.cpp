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



#include "OImage.h"
#include "OGLTexture2D.h"
#include "TYImageManager.h"
#include <cassert>

OGLTexture2D::OGLTexture2D()
{

}

OGLTexture2D::~OGLTexture2D()
{

}

bool OGLTexture2D::load(const char* filename)
{
    if (id > 0)
    {
        free();
    }

    genTexture();

    // Bind texture and activate bilinear filtering
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // Get image
    OImage* im = TYImageManager::get()->getImage(filename);

    if (!im)
    {
        return false;
    }

    // Get image informations
    GLsizei w = im->getSizeX();
    GLsizei h = im->getSizeY();
    GLenum format;

    unsigned int bpp = im->getDepth() / 8;
    switch (bpp)
    {
        case 1:
            format = GL_LUMINANCE;
            break;
        case 2:
            format = GL_LUMINANCE_ALPHA;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            return false;
    }

    // Upload texture
    glTexImage2D(GL_TEXTURE_2D, 0, bpp, w, h, 0, format, GL_UNSIGNED_BYTE, im->getData());

    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}
