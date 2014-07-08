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
 * \file TYPanelGraphic.h
 * \brief Representation graphique d'un panel d'informations (fichier header)
 * \author Projet_Tympan
 *
 *
 */

#ifndef __TY_PANEL_GRAPHIC__
#define __TY_PANEL_GRAPHIC__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Tympan/gui/tools/OGLFont.h"
#include "TYElementGraphic.h"

class TYPanel;

/**
 * \class TYPanelGraphic
 * \brief classe graphique pour un panel d'informations
 */
class TYPanelGraphic : public TYElementGraphic
{
    TY_DECL_METIER_GRAPHIC(TYPanel)

    // Methodes
public:
    TYPanelGraphic(TYPanel* pElement);
    virtual ~TYPanelGraphic();

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

    /**
     * Get de la couleur du fond
     */
    OColor getBackColor() const { return _backColor; }

    /**
     * Set de la couleur du fond
     */
    void setBackColor(const OColor& backColor) { _backColor = backColor; }

    /**
     * Get de la couleur du texte
     */
    OColor getFontColor() const { return _fontColor; }

    /**
     * Set de la couleur du fond
     */
    void setFontColor(const OColor& fontColor) { _fontColor = fontColor; }

protected:
    // Font
    LPOGLFont _font;

    // Position
    double _posX;
    double _posY;

    // Taille du panel
    double _width;
    double _height;

    // couleur du panel
    OColor _backColor;

    // couleur du texte
    OColor _fontColor;
};

#endif // __TY_PANEL_GRAPHIC__
