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
 * \file TYAcousticPolygonGraphic.cpp
 * \brief Representation graphique d'un polygone accoustique
 */


#include "Tympan/models/business/geoacoustic/TYAcousticPolygon.h"
#include "Tympan/gui/gl/TYPickingTable.h"
#include "TYAcousticPolygonGraphic.h"


TYAcousticPolygonGraphic::TYAcousticPolygonGraphic(TYAcousticPolygon* pElement) :
    TYElementGraphic(pElement)
{
}

void TYAcousticPolygonGraphic::update(bool force /*=false*/)
{
    TYElementGraphic::update(force);
}

void TYAcousticPolygonGraphic::getChilds(TYListPtrTYElementGraphic& childs, bool recursif /*=true*/)
{
    TYElementGraphic* pTYElementGraphic;
    pTYElementGraphic = getElement()->getSrcSurf()->getGraphicObject();
    childs.push_back(pTYElementGraphic);
    if (recursif)
    {
        pTYElementGraphic->getChilds(childs, recursif);
    }

    pTYElementGraphic = getElement()->getPolygon()->getGraphicObject();
    childs.push_back(pTYElementGraphic);
    if (recursif)
    {
        pTYElementGraphic->getChilds(childs, recursif);
    }
}

void TYAcousticPolygonGraphic::computeBoundingBox()
{
    OBox reset;
    _boundingBox = reset;

    getElement()->getSrcSurf()->getGraphicObject()->computeBoundingBox();
    getElement()->getPolygon()->getGraphicObject()->computeBoundingBox();

    _boundingBox.Enlarge(getElement()->getSrcSurf()->getGraphicObject()->GetBox());
    _boundingBox.Enlarge(getElement()->getPolygon()->getGraphicObject()->GetBox());
}

void TYAcousticPolygonGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    getElement()->getSrcSurf()->getGraphicObject()->display(mode);
    getElement()->getPolygon()->getGraphicObject()->highlight(_highlight);
    glColor3fv(getElement()->getColor());

    if (mode == GL_SELECT)
    {
        TYPickingTable::addElement(getElement());
        glPushName((GLuint)(TYPickingTable::getIndex()));
    }

    getElement()->getPolygon()->getGraphicObject()->display(mode);

    if (mode == GL_SELECT)
    {
        glPopName();
    }
}
