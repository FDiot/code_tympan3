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
 * \file TYPointGraphic.cpp
 * \brief Representation graphique d'un point
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHGraphic.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "TYPointGraphic.h"
#include "Tympan/models/business/geometry/TYPoint.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYPickingTable.h"


TYPointGraphic::TYPointGraphic(TYPoint* pElement) :
    TYElementGraphic(pElement)
{
}

TYPointGraphic::~TYPointGraphic()
{
}

void TYPointGraphic::update(bool force /*=false*/)
{
    TYElementGraphic::update(force);
}

void TYPointGraphic::computeBoundingBox()
{
    OBox reset;
    _boundingBox = reset;

    static const float crossSize = 0.2f;
    _boundingBox.Enlarge((float)(getElement()->_x - crossSize), (float)(getElement()->_y - crossSize), (float)(getElement()->_z - crossSize));
    _boundingBox.Enlarge((float)(getElement()->_x + crossSize), (float)(getElement()->_y + crossSize), (float)(getElement()->_z + crossSize));
}

void TYPointGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    static const float crossSize = 0.2f;

    if (_visible)
    {

        if (mode == GL_SELECT)
        {
            TYPickingTable::addElement(getElement());
            glPushName((GLuint)(TYPickingTable::getIndex()));
        }

        glBegin(GL_LINES);

        OColor color(1.0f, 0.0f, 0.0f);

        if (_highlight)
        {
            if (_bFirstDisp)
            {
                computeBoundingBox();
                _bFirstDisp = false;
            }
            drawLineBoundingBox();
            if (mode == GL_COMPILE) { drawName(); }
        }

        glColor4fv(color);

        glVertex3f(getElement()->_x - crossSize, getElement()->_y, getElement()->_z);
        glVertex3f(getElement()->_x + crossSize, getElement()->_y, getElement()->_z);

        glVertex3f(getElement()->_x, getElement()->_y - crossSize, getElement()->_z);
        glVertex3f(getElement()->_x, getElement()->_y + crossSize, getElement()->_z);

        glVertex3f(getElement()->_x, getElement()->_y, getElement()->_z + crossSize);
        glVertex3f(getElement()->_x, getElement()->_y, getElement()->_z - crossSize);

        glEnd();

        glLineWidth(1);

        if (mode == GL_SELECT)
        {
            glPopName();
        }
        //Calcul du volume englobant pour le fit:
        _globalBoundingBox.Enlarge(_boundingBox);
    }
}
