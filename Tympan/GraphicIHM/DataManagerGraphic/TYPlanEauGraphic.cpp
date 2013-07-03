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
 * \file TYPlanEauGraphic.cpp
 * \brief Representation graphique d'un plan d'eau
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHGraphic.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "TYPlanEauGraphic.h"

#include "Tympan/MetierSolver/DataManagerMetier/EltTopographique/TYPlanEau.h"

#include "Tympan/MetierSolver/DataManagerCore/TYPreferenceManager.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYPickingTable.h"


TYPlanEauGraphic::TYPlanEauGraphic(TYPlanEau* pElement) :
    TYElementGraphic(pElement)
{
    _pPolygon = new TYPolygon();
}

void TYPlanEauGraphic::update(bool force /*=false*/)
{
    TYElementGraphic::update(force);
}

void TYPlanEauGraphic::getChilds(TYListPtrTYElementGraphic& childs, bool recursif /*=true*/)
{
    TYElementGraphic* pTYElementGraphic = _pPolygon->getGraphicObject();
    childs.push_back(pTYElementGraphic);
    if (recursif)
    {
        pTYElementGraphic->getChilds(childs, recursif);
    }
}


void TYPlanEauGraphic::computeBoundingBox()
{
    OBox reset;
    _boundingBox = reset;
    _pPolygon->getGraphicObject()->computeBoundingBox();
    _boundingBox = _pPolygon->getGraphicObject()->GetBox();
}

void TYPlanEauGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    if (!_visible) { return; }

    TYElementGraphic::display(mode);

    TYTabPoint tabpoints = getElement()->getListPoints();

    // Ajout d'un offset pour limiter le flickering avec l'altimetrie
    for (register unsigned int i = 0; i < tabpoints.size(); i++)
    {
        tabpoints[i]._z += 0.5;  // Anciennement 0.05
    }

    _pPolygon->setPoints(tabpoints);

    if (mode == GL_SELECT)
    {
        TYPickingTable::addElement(getElement());
        glPushName((GLuint)(TYPickingTable::getIndex()));
    }

    glColor4fv(getElement()->getColor());

    _pPolygon->getGraphicObject()->highlight(_highlight);
    _pPolygon->getGraphicObject()->display(mode);
    if ((_highlight) && (mode == GL_COMPILE))
    {
        computeBoundingBox();
        drawName();
    }

    if (mode == GL_SELECT)
    {
        glPopName();
    }
}
