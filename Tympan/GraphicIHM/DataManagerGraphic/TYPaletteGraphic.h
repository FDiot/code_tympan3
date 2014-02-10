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

/**
 * \file TYPaletteGraphic.h
 * \brief Representation d'une palette graphique (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_PALETTE_GRAPHIC__
#define __TY_PALETTE_GRAPHIC__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Tympan/GraphicIHM/ToolsGraphic/OGLFont.h"
#include "TYElementGraphic.h"

class TYPalette;

/**
 * \class TYPaletteGraphic
 * \brief Classe graphique pour representer une palette
 */
class TYPaletteGraphic : public TYElementGraphic
{
    TY_DECL_METIER_GRAPHIC(TYPalette)

    // Methodes
public:
    TYPaletteGraphic(TYPalette* pElement);
    virtual ~TYPaletteGraphic();

    virtual void update(bool force = false);
    virtual void display(GLenum mode = GL_RENDER);

    /**
     * Get de la position en x
     */
    double getPosX() const { return _posX; }

    /**
     * Set de la position en x
     */
    void setPosX(double x) { _posX = x; }

    /**
     * Get de la position en y
     */
    double getPosY() const { return _posY; }

    /**
     * Set de la position en y
     */
    void setPosY(double y) { _posY = y; }

    /**
     * Get de la largeur
     */
    double getWidth() const { return _width; }

    /**
     * Set de la largeur
     */
    void setWidth(double width) { _width = width; }

    /**
     * Get de la hauteur
     */
    double getHeight() const { return _height; }

    /**
     * Set de la hauteur
     */
    void setHeight(double height) { _height = height; }

protected:
    // Font
    LPOGLFont _font;

    // Position
    double _posX;
    double _posY;

    // Taille de la barre
    double _width;
    double _height;

};

#endif // __TY_PALETTE_GRAPHIC__
