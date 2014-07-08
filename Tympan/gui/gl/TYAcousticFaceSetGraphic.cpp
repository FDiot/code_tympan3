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
 * \file TYAcousticFaceSetGraphic.cpp
 * \brief Representation graphique d'un ensemble de faces acoustiques
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHGraphic.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/models/business/geometry/TYFaceSet.h"
#include "Tympan/models/business/geoacoustic/TYAcousticFaceSet.h"


TYAcousticFaceSetGraphic::TYAcousticFaceSetGraphic(TYAcousticFaceSet* pElement) :
    TYElementGraphic(pElement)
{
}

void TYAcousticFaceSetGraphic::update(bool force /*=false*/)
{
    TYFaceSet* pFaces = getElement()->getFaceSet();
    //  LPTYFaceSetGraphic* pFacesGraphic= pFaces->getGraphicObject();
    //  pFacesGraphic->update();
    pFaces->getGraphicObject()->update();

    TYElementGraphic::update(force);
}

void TYAcousticFaceSetGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    TYElementGraphic::display(mode);

    glColor4fv(getElement()->getColor());

    if (mode == GL_SELECT)
    {
        TYPickingTable::addElement(getElement());
        glPushName((GLuint)(TYPickingTable::getIndex()));
    }

    TYFaceSet* pFaces = getElement()->getFaceSet();
    pFaces->getGraphicObject()->display(mode);

    if (mode == GL_SELECT)
    {
        glPopName();
    }
}

void TYAcousticFaceSetGraphic::computeBoundingBox()
{
    OBox reset;
    _boundingBox = reset;

    TYTabPoint sommets = getElement()->sommets();

    size_t nbPts = sommets.size();
    TYPoint pt;
    for (size_t i = 0; i < nbPts; i++)
    {
        pt = sommets[i];
        _boundingBox.Enlarge((float)(pt._x), (float)(pt._y), (float)(pt._z));
    }
}
