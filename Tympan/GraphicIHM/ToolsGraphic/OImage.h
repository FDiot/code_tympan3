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

#ifndef __O_IMAGE__
#define __O_IMAGE__

#include <string>

class TYImageManager;

/**
 * Class definition une image.
 *
 * @author Projet_Tympan
 *
 */
class OImage
{
    // TYImageManager a besoin d'acceder au donnee privee.
    friend class TYImageManager;

public:
    OImage();
    virtual ~OImage();

    /**
     * Renvoi l'image sous la forme d'un pointer sur un octet non signe.
     */
    unsigned char* getData() const { return _data; }

    /**
     * Renvoi la largeur de l'image.
     */
    unsigned int getSizeX() const { return _sizeX; }

    /**
     * Renvoi la hauteur de l'image.
     */
    unsigned int getSizeY() const { return _sizeY; }

    /**
     * Renvoi le nombre de bits par pixel.
     */
    unsigned int getDepth() const { return _depth; }

    /**
     * Renvoi le nombres d'octet d'un pixel
     */
    unsigned int getPixelSize() const;

    /**
     * Charge une image a partir d'un fichier
     */
    virtual bool load(const std::string& filename) = 0;

protected:
    // Donnee de l'image.
    unsigned char* _data;

    // Bits par pixel.
    unsigned int _depth;

    // Taille de l'image.
    unsigned int _sizeX;
    unsigned int _sizeY;
};

#endif //__O_IMAGE__
