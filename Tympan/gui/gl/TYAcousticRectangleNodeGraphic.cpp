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
 * \file TYAcousticRectangleNodeGraphic.cpp
 * \brief Representation graphique d'un ensemble de rectangles acoustiques
 *
 *
 */




#include "Tympan/models/business/geoacoustic/TYAcousticRectangleNode.h"


TYAcousticRectangleNodeGraphic::TYAcousticRectangleNodeGraphic(TYAcousticRectangleNode* pElement) :
    TYAcousticSurfaceNodeGraphic(pElement)
{
}

void TYAcousticRectangleNodeGraphic::update(bool force /*=false*/)
{
    if (getModified() || force)
    {
        getElement()->updateGrid();
    }

    TYAcousticSurfaceNodeGraphic::update(force);
}

void TYAcousticRectangleNodeGraphic::computeBoundingBox()
{
    OBox reset;
    _boundingBox = reset;

    TYAcousticSurfaceNodeGraphic::computeBoundingBox();
}

void TYAcousticRectangleNodeGraphic::display(GLenum mode /*=GL_RENDER*/)
{
    TYAcousticSurfaceNodeGraphic::display(mode);
}
