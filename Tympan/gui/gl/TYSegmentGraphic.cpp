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
 * \file TYSegmentGraphic.cpp
 * \brief Representation graphique d'un segment
 *
 *
 */




#include "Tympan/models/business/geometry/TYSegment.h"
#include "Tympan/gui/gl/TYPickingTable.h"

#include "TYSegmentGraphic.h"

TYSegmentGraphic::TYSegmentGraphic(TYSegment* pElement) :
    TYElementGraphic(pElement)
{
}

TYSegmentGraphic::~TYSegmentGraphic()
{
}

void TYSegmentGraphic::update(bool force /*=false*/)
{
    TYElementGraphic::update(force);
}


void TYSegmentGraphic::computeBoundingBox()
{
    OBox reset;
    _boundingBox = reset;

    _boundingBox.Enlarge(getElement()->_ptA._x, getElement()->_ptA._z, -getElement()->_ptA._y);
    _boundingBox.Enlarge(getElement()->_ptB._x, getElement()->_ptB._z, -getElement()->_ptB._y);
}

void TYSegmentGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    OColor color(getElement()->getColor());

    if (_highlight)
    {
        float tmpColor[4];
        glGetFloatv(GL_CURRENT_COLOR, tmpColor);
        if (_bFirstDisp)
        {
            computeBoundingBox();
            _bFirstDisp = false;
        }
        drawLineBoundingBox();
        if (mode == GL_COMPILE) { drawName(); }
        glColor3f(tmpColor[0], tmpColor[1], tmpColor[2]);
    }

    if (_visible)
    {

        if (mode == GL_SELECT)
        {
            TYPickingTable::addElement(getElement());
            glPushName((GLuint)(TYPickingTable::getIndex()));
        }

        glBegin(GL_LINES);

        glColor4fv(color);

        glVertex3dAndEnlargeBB(getElement()->_ptA._x, getElement()->_ptA._z, -getElement()->_ptA._y);
        glVertex3dAndEnlargeBB(getElement()->_ptB._x, getElement()->_ptB._z, -getElement()->_ptB._y);

        glEnd();

        if (mode == GL_SELECT)
        {
            glPopName();
        }

        //Calcul du volume englobant pour le fit:
        _globalBoundingBox.Enlarge(_boundingBox);
    }
}
