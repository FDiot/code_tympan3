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
 * \file TYAcousticBoxGraphic.cpp
 * \brief Representation graphique d'une boite acoustique
 *
 *
 */




#include "Tympan/models/business/geoacoustic/TYAcousticBox.h"


#include "TYAcousticBoxGraphic.h"

TYAcousticBoxGraphic::TYAcousticBoxGraphic(TYAcousticBox* pElement) :
    TYElementGraphic(pElement)
{
}

void TYAcousticBoxGraphic::update(bool force /*=false*/)
{
    TYElementGraphic::update(force);
}

void TYAcousticBoxGraphic::getChilds(TYListPtrTYElementGraphic& childs, bool recursif /*=true*/)
{
    for (int i = 0; i < 6; i++)
    {
        // On recupere la face
        LPTYAcousticRectangleNodeGeoNode pAccRectNodeGeoNode = getElement()->getFace(i);
        TYElementGraphic* pTYElementGraphic = (TYElementGraphic*)pAccRectNodeGeoNode->getGraphicObject();
        childs.push_back(pTYElementGraphic);
        if (recursif)
        {
            pTYElementGraphic->getChilds(childs, recursif);
        }
    }
}

void TYAcousticBoxGraphic::computeBoundingBox()
{
    OBox reset;
    _boundingBox = reset;

    // CLM-NT35 : Correction calcul BB des machines
    // Attention les bb des faces sont orientes dans leur repere local
    // --> Utilisation de 2 Faces pour calculer la bb d'une box
    LPTYAcousticRectangleNodeGeoNode pAccRectNodeGeoNode1 = getElement()->getFace(0);
    pAccRectNodeGeoNode1->getGraphicObject()->computeBoundingBox();
    OBox rect_bb1 = pAccRectNodeGeoNode1->getGraphicObject()->GetBox();
    LPTYAcousticRectangleNodeGeoNode pAccRectNodeGeoNode2 = getElement()->getFace(1);
    pAccRectNodeGeoNode2->getGraphicObject()->computeBoundingBox();
    OBox rect_bb2 = pAccRectNodeGeoNode2->getGraphicObject()->GetBox();

    _boundingBox._min._x = rect_bb2._min._x;
    _boundingBox._max._x = rect_bb2._max._x;
    _boundingBox._min._y = rect_bb1._min._x;
    _boundingBox._max._y = rect_bb1._max._x;
    _boundingBox._min._z = rect_bb1._min._y;
    _boundingBox._max._z = rect_bb1._max._y;
    // CLM-NT35 : Correction calcul BB des machines

}

void TYAcousticBoxGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    TYElementGraphic::display(mode);

    int i;

    // CLM-NT35: Affiche que le nom de l'element localise en overlay
    if (mode == GL_COMPILE)
    {
        drawName();
    }
    else
    {
        if (_highlight)
        {
            if (_bFirstDisp)
            {
                computeBoundingBox();
                _bFirstDisp = false;
            }
            drawLineBoundingBox();
            //Calcul du volume englobant pour le fit:
            _globalBoundingBox.Enlarge(_boundingBox);
        }

        for (i = 0; i < 6; i++)
        {
            // On recupere la face
            LPTYAcousticRectangleNodeGeoNode pAccRectNodeGeoNode = getElement()->getFace(i);
            pAccRectNodeGeoNode->getGraphicObject()->display(mode);
        }
    }
}
