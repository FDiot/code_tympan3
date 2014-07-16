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
 * \file TYPaletteGraphic.cpp
 * \brief Representation d'une palette graphique
 *
 *
 */




#include <limits>

#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/math/special_functions/fpclassify.hpp>

#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/gui/OSizeManager.h"
#include "Tympan/models/business/TYPalette.h"

#define IMG(id) OLocalizator::getPicture("TYPaletteGraphic", (id))

TYPaletteGraphic::TYPaletteGraphic(TYPalette* pElement)
    : TYElementGraphic(pElement)
{
    _font = new OGLFont();

    // Position
    _posX = 0.0;
    _posY = 0.0;

    // Taille de la barre
    _width = 0.1;
    _height = 0.1;
}

TYPaletteGraphic::~TYPaletteGraphic()
{

}

void TYPaletteGraphic::update(bool force /*= false*/)
{
    TYElementGraphic::update(force);
}

void TYPaletteGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    LPTYPalette pPalette = getElement();
    assert(pPalette && "How the hell pPalette could be NULL ?!?");

    if (pPalette->getNbColors() == 0) { return; } // XXX Shouldn't this be an assert ?!?

    if (_visible)
    {
        // On sauvegarde le mode d'affichage des polygones
        GLdouble polygonMode[2];
        glGetDoublev(GL_POLYGON_MODE, polygonMode);

        // Mode GL_FILL
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // Nouvelle projection
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0, OSizeManager::get()->getWidth(), 0.0, OSizeManager::get()->getHeight(), 0.0, 1.0);

        // Nouvelle vue
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        // Taille relative des bornes inf et sup
        const double relBoundSize = 0.2;

        // Initialisation des variables de position
        double x = _posX;
        double y = _posY;
        const double width_but_bounds = (_width * (1.0 - 2 * relBoundSize));
        const double dy = _height;

        // On dessine un quad
        glBegin(GL_QUADS);

        TYPalette::values_type value;
        OColor color;
        TYPalette::values_type prev_value = - std::numeric_limits<TYPalette::values_type>::infinity();
        // On affiche chaque echellon
        BOOST_FOREACH(boost::tie(value, color), pPalette->getColorMap())
        {
            // Handle specially first iteration
            const double dx =  boost::math::isinf(value - prev_value) ? relBoundSize * _width :
                               (pPalette->normalize(value) - pPalette->normalize(prev_value)) * width_but_bounds;
            prev_value = value;
            glColor3fv(color);
            glVertex2d(x   , y);
            glVertex2d(x   , y - dy);
            glVertex2d(x + dx, y - dy);
            glVertex2d(x + dx, y);
            x += dx;
        }

        // Fin du quad
        glEnd();

        // On convertie en string
        std::ostringstream minValue;
        minValue << pPalette->getValueMin();

        std::ostringstream midValue;
        midValue << " -- ";

        std::ostringstream maxValue;
        maxValue << pPalette->getValueMax();

        // On charge la texture des polices pour la valeur min
        _font->load(IMG("id_font").toAscii().data());
        _font->bind();

        // On affiche la borne inf
        // Delta en x : 0.0
        // Delta en y : 3.0
        x = _posX;
        y = _posY + _height + 3.0;
        _font->drawText("<", OColor::BLACK, x, y);

        // On affiche la valeur min
        // Delta en x : -3.0
        // Delta en y :  3.0
        x = _posX + _width * 0.2 - 3.0; //XXX What is that ?
        y = _posY + _height + 3.0;
        _font->drawText(minValue.str(), OColor::BLACK, x, y);

        // On affiche la valeur mid
        // Delta en x : -6.0
        // Delta en y :  3.0
        x = _posX + _width * 0.5 - 6.0;
        y = _posY + _height + 3.0;
        _font->drawText(midValue.str(), OColor::BLACK, x, y);

        // On affiche la valeur max
        // Delta en x : -9.0
        // Delta en y :  3.0
        x = _posX + _width * 0.8 - 9.0; //XXX What is that ?
        y = _posY + _height + 3.0;
        _font->drawText(maxValue.str(), OColor::BLACK, x, y);

        // On affiche la borne sup
        // Delta en x : -8.0
        // Delta en y :  3.0
        x = _posX + _width - 8.0; //XXX What is that ?
        y = _posY + _height + 3.0;
        _font->drawText(">", OColor::BLACK, x, y);

        // Ancienne projection
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        // Ancienne vue
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        // On restaure le mode d'affichage des polygones
        glPolygonMode(GL_FRONT_AND_BACK, polygonMode[0]);
    }
}
