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
 * \file TYSourceLineicGraphic.cpp
 * \brief Representation graphique d'une source lineique
 *
 *
 */




#include "Tympan/models/business/acoustic/TYSourceLineic.h"


bool TYSourceLineicGraphic::_gVisible = false;


TYSourceLineicGraphic::TYSourceLineicGraphic(TYSourceLineic* pElement) :
    TYElementGraphic(pElement)
{
    _srcVisible = false;
}

void TYSourceLineicGraphic::update(bool force /*=false*/)
{
    TYElementGraphic::update(force);
}

void TYSourceLineicGraphic::getChilds(TYListPtrTYElementGraphic& childs, bool recursif /*=true*/)
{
    unsigned int i;
    LPTYSourcePonctuelle pSrcPonct = NULL;

    for (i = 0; i < getElement()->getNbSrcs(); i++)
    {
        // On recupere la source
        pSrcPonct = getElement()->getSrc(i);
        TYElementGraphic* pTYElementGraphic = pSrcPonct->getGraphicObject();
        childs.push_back(pTYElementGraphic);
        if (recursif)
        {
            pTYElementGraphic->getChilds(childs, recursif);
        }
    }
}

void TYSourceLineicGraphic::computeBoundingBox()
{
    OBox reset;
    _boundingBox = reset;

    unsigned int i;
    LPTYSourcePonctuelle pSrcPonct = NULL;

    for (i = 0; i < getElement()->getNbSrcs(); i++)
    {
        // On recupere la source
        pSrcPonct = getElement()->getSrc(i);
        pSrcPonct->getGraphicObject()->computeBoundingBox();
        _boundingBox.Enlarge(pSrcPonct->getGraphicObject()->GetBox());
    }
}

void TYSourceLineicGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    unsigned int i;
    LPTYSourcePonctuelle pSrcPonct = NULL;
    if (_visible)
    {
        for (i = 0; i < getElement()->getNbSrcs(); i++)
        {
            // On recupere la source
            pSrcPonct = getElement()->getSrc(i);
            pSrcPonct->getGraphicObject()->display(mode);
        }
    }
}
