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
 * \file TYAcousticLineGraphic.cpp
 * \brief Representation graphique d'une ligne acoustique
 */


#include <boost/foreach.hpp>

#include "Tympan/models/business/geoacoustic/TYAcousticLine.h"
#include "Tympan/gui/gl/TYPickingTable.h"
#include "TYAcousticLineGraphic.h"


TYAcousticLineGraphic::TYAcousticLineGraphic(TYAcousticLine* pElement) :
    TYElementGraphic(pElement)
{
    _pPolyLineGraphic = new TYPolyLineGraphic(false);
    _pPolyLineGraphic->setElement(pElement);
}

void TYAcousticLineGraphic::update(bool force /*=false*/)
{
    TYElementGraphic::update(force);
}

void TYAcousticLineGraphic::getChilds(TYListPtrTYElementGraphic& childs, bool recursif /*=true*/)
{
    TYElementGraphic* pTYElementGraphic = _pPolyLineGraphic;
    childs.push_back(pTYElementGraphic);
    if (recursif)
    {
        pTYElementGraphic->getChilds(childs, recursif);
    }
}

void TYAcousticLineGraphic::computeBoundingBox()
{
    OBox reset;
    _boundingBox = reset;

    _pPolyLineGraphic->computeBoundingBox();
    _boundingBox.Enlarge(_pPolyLineGraphic->GetBox());
}

void TYAcousticLineGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    if (!getElement()->isInCurrentCalcul())
    {
        return;
    }

    _pPolyLineGraphic->highlight(_highlight);

    TYTabPoint& tabpts = _pPolyLineGraphic->getTabPoint();
    tabpts.clear();
    tabpts.reserve(getElement()->getSrcLineic()->getNbSrcs());

    BOOST_FOREACH(LPTYSourcePonctuelle pSrc, getElement()->getSrcLineic()->getSrcs())
    {
        tabpts.push_back(*pSrc->getPos());
    }

    _pPolyLineGraphic->setTabPoint(tabpts);

    if (mode == GL_SELECT)
    {
        TYPickingTable::addElement(getElement());
        glPushName((GLuint)(TYPickingTable::getIndex()));
    }

    glColor4fv(getElement()->getColor());
    _pPolyLineGraphic->display(mode);

    if (mode == GL_SELECT)
    {
        glPopName();
    }
}
