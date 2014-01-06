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
 * \file TYPanelGraphic.cpp
 * \brief Representation graphique d'un panel d'informations
 *
 *
 */

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHGraphic.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/Tools/OSizeManager.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYMaillage.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYPanel.h"

#define IMG(id) OLocalizator::getPicture("TYPanelGraphic", (id))

TYPanelGraphic::TYPanelGraphic(TYPanel* pElement) :
    TYElementGraphic(pElement)
{
    _font = new OGLFont();

    // Position
    _posX = 0.0;
    _posY = 0.0;

    // Taille du panel
    _width = 100.0;
    _height = 100.0;

    _backColor = OColor::YELLOW;
    _fontColor = OColor::BLACK;
}

TYPanelGraphic::~TYPanelGraphic()
{

}

void TYPanelGraphic::update(bool force /*=false*/)
{
    TYElementGraphic::update(force);
}

void TYPanelGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    LPTYPanel pPanel = getElement();

    if (pPanel == NULL)
    {
        return;
    }

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

        // Eloignement du curseur
        int offset = 5;

        int x = _posX + offset;
        int y = _posY - offset;
        int dx = _width;
        int dy = _height;

        // Dessin du panel
        glColor3fv(_backColor);
        glBegin(GL_QUADS);
        glVertex2i(x   , y);
        glVertex2i(x   , y - dy);
        glVertex2i(x + dx, y - dy);
        glVertex2i(x + dx, y);
        glEnd();

        // On charge la texture des polices pour la valeur min
        _font->load(IMG("id_font").toAscii().data());
        _font->bind();

        // On affiche la position
        // Delta en x :  3
        // Delta en y : -3
        dx =  3 + offset;
        dy = -3 - offset;
        x = _posX + dx;
        y = _posY + dy;
        _font->drawText(getElement()->getFirstPos() + "   " + \
                        getElement()->getSecondPos(), \
                        OColor::BLACK, x, y);

        // On affiche la valeur
        // Delta en x :   3
        // Delta en y : -23
        dx =   3 + offset;
        dy = -23 - offset;
        x = _posX + dx;
        y = _posY + dy;
        _font->drawText(getElement()->getValue(), OColor::BLACK, x, y);

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
