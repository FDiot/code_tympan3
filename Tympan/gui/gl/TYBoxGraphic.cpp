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
 * \file TYBoxGraphic.cpp
 * \brief Representation graphique d'une boite
 *
 *
 */



#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHGraphic.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/models/business/geometry/TYBox.h"


TYBoxGraphic::TYBoxGraphic(TYBox* pElement) :
    TYElementGraphic(pElement)
{
}

TYBoxGraphic::~TYBoxGraphic()
{
}

void TYBoxGraphic::update(bool force)//=false
{
    TYElementGraphic::update(force);
}

void TYBoxGraphic::computeBoundingBox()
{
    OBox reset;
    _boundingBox = reset;

    double x = (getElement()->_sizeX) / 2;
    double y = (getElement()->_sizeY) / 2;
    double z = (getElement()->_sizeZ) / 2;

    float org[3];
    getElement()->getPosition().getToOGL(org);

    _boundingBox.Enlarge((float)(org[0] + x), (float)(org[1] + y), (float)(org[2] + z));
    _boundingBox.Enlarge((float)(org[0] - x), (float)(org[1] - y), (float)(org[2] - z));
}

void TYBoxGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    double x = (getElement()->_sizeX) / 2;
    double y = (getElement()->_sizeY) / 2;
    double z = (getElement()->_sizeZ) / 2;

    float org[3];
    getElement()->getPosition().getToOGL(org);

    OColor color(getElement()->getColor());

    if (_highlight)
    {
        OColor tmpColor;
        glGetFloatv(GL_CURRENT_COLOR, tmpColor);
        if (_bFirstDisp)
        {
            computeBoundingBox();
            _bFirstDisp = false;
        }
        drawLineBoundingBox();
        if (mode == GL_COMPILE) { drawName(); }
        glColor3fv(tmpColor);
    }

    if (_visible)
    {

        if (mode == GL_SELECT)
        {
            TYPickingTable::addElement(getElement());
            glPushName((GLuint)(TYPickingTable::getIndex()));
        }

        glBegin(GL_QUADS);

        glColor4fv(color);

        glVertex3d(org[0] + x, org[1] + y, org[2] - z);
        glVertex3d(org[0] - x, org[1] + y, org[2] - z);
        glVertex3d(org[0] - x, org[1] + y, org[2] + z);
        glVertex3d(org[0] + x, org[1] + y, org[2] + z);

        glVertex3f(org[0] + x, org[1] - y, org[2] + z);
        glVertex3f(org[0] - x, org[1] - y, org[2] + z);
        glVertex3f(org[0] - x, org[1] - y, org[2] - z);
        glVertex3f(org[0] + x, org[1] - y, org[2] - z);

        glVertex3f(org[0] + x, org[1] + y, org[2] + z);
        glVertex3f(org[0] - x, org[1] + y, org[2] + z);
        glVertex3f(org[0] - x, org[1] - y, org[2] + z);
        glVertex3f(org[0] + x, org[1] - y, org[2] + z);

        glVertex3f(org[0] + x, org[1] - y, org[2] - z);
        glVertex3f(org[0] - x, org[1] - y, org[2] - z);
        glVertex3f(org[0] - x, org[1] + y, org[2] - z);
        glVertex3f(org[0] + x, org[1] + y, org[2] - z);

        glVertex3f(org[0] - x, org[1] + y, org[2] + z);
        glVertex3f(org[0] - x, org[1] + y, org[2] - z);
        glVertex3f(org[0] - x, org[1] - y, org[2] - z);
        glVertex3f(org[0] - x, org[1] - y, org[2] + z);

        glVertex3f(org[0] + x, org[1] + y, org[2] - z);
        glVertex3f(org[0] + x, org[1] + y, org[2] + z);
        glVertex3f(org[0] + x, org[1] - y, org[2] + z);
        glVertex3f(org[0] + x, org[1] - y, org[2] - z);

        glEnd();

        //Calcul du volume englobant pour le fit:
        _globalBoundingBox.Enlarge(_boundingBox);

        if (mode == GL_SELECT)
        {
            glPopName();
        }
    }
}
