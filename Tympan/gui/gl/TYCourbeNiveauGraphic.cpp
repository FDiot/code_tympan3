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
 * \file TYCourbeNiveauGraphic.cpp
 * \brief Representation graphique d'une courbe de niveau
 *
 *
 */




#include "Tympan/models/business/topography/TYCourbeNiveau.h"

#include "Tympan/models/business/TYPreferenceManager.h"



TYCourbeNiveauGraphic::TYCourbeNiveauGraphic(TYCourbeNiveau* pElement) :
    TYElementGraphic(pElement)
{
    _pPolyLineGraphic = new TYPolyLineGraphic(false);
    _pPolyLineGraphic->setElement(pElement);
}

void TYCourbeNiveauGraphic::update(bool force /*=false*/)
{
    TYElementGraphic::update(force);
}

void TYCourbeNiveauGraphic::getChilds(TYListPtrTYElementGraphic& childs, bool recursif /*=true*/)
{
    TYElementGraphic* pTYElementGraphic = _pPolyLineGraphic;
    childs.push_back(pTYElementGraphic);
    if (recursif)
    {
        pTYElementGraphic->getChilds(childs, recursif);
    }
}

void TYCourbeNiveauGraphic::computeBoundingBox()
{
    OBox reset;
    _boundingBox = reset;
    _pPolyLineGraphic->computeBoundingBox();
    _boundingBox.Enlarge(_pPolyLineGraphic->GetBox());
}

void TYCourbeNiveauGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    if (!_visible)
    {
        return;
    }

    TYElementGraphic::display(mode);

    _pPolyLineGraphic->setModified(true);
    _pPolyLineGraphic->setTabPoint(getElement()->getListPoints());

    if (mode == GL_SELECT)
    {
        TYPickingTable::addElement(getElement());
        glPushName((GLuint)(TYPickingTable::getIndex()));
    }
    GLenum Error;
    Error = glGetError();
    glColor4fv(getElement()->getColor());
    Error = glGetError();//GL_INVALID_ENUM
    float color[4];//az++ test
    glGetFloatv(GL_CURRENT_COLOR, color);//az++ test
    Error = glGetError();
    //delete [] color;//az++ test
    _pPolyLineGraphic->highlight(getHighlightState());//az++
    _pPolyLineGraphic->display(mode);

    if (mode == GL_SELECT)
    {
        glPopName();
    }

}
