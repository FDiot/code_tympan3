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
 * \file TYSourceSurfacicGraphic.cpp
 * \brief Representation graphique d'une surface de source
 *
 *
 */




#include "Tympan/models/business/acoustic/TYSourceSurfacic.h"


bool TYSourceSurfacicGraphic::_gVisible = false;


TYSourceSurfacicGraphic::TYSourceSurfacicGraphic(TYSourceSurfacic* pElement) :
    TYElementGraphic(pElement)
{
    _srcVisible = false;
}

void TYSourceSurfacicGraphic::update(bool force)//=false
{
    TYElementGraphic::update(force);
}

void TYSourceSurfacicGraphic::getChilds(TYListPtrTYElementGraphic& childs, bool recursif /*=true*/)
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

void TYSourceSurfacicGraphic::computeBoundingBox()
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

void TYSourceSurfacicGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    unsigned int i;
    LPTYSourcePonctuelle pSrcPonct = NULL;

    if ((_visible) && (_srcVisible || _gVisible))
    {
        for (i = 0; i < getElement()->getNbSrcs(); i++)
        {
            // On recupere la source
            pSrcPonct = getElement()->getSrc(i);
            pSrcPonct->getGraphicObject()->display(mode);
        }
    }
}
