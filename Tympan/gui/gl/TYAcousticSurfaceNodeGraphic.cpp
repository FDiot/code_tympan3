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
 * \file TYAcousticSurfaceNodeGraphic.cpp
 * \brief Representation graphique d'un ensemble de surfaces acoustiques
 */


#include "Tympan/models/business/geoacoustic/TYAcousticSurfaceNode.h"
#include "Tympan/gui/gl/TYPickingTable.h"
#include "TYAcousticSurfaceNodeGraphic.h"

TYAcousticSurfaceNodeGraphic::TYAcousticSurfaceNodeGraphic(TYAcousticSurfaceNode* pElement) :
    TYElementGraphic(pElement)
{
}

void TYAcousticSurfaceNodeGraphic::update(bool force /*=false*/)
{
}

void TYAcousticSurfaceNodeGraphic::getChilds(TYListPtrTYElementGraphic& childs, bool recursif /*=true*/)
{
    TYTabAcousticSurfaceGeoNode* pTab = &getElement()->getTabAcousticSurf();
    TYElementGraphic* pTYElementGraphic;
    for (int i = 0; i < pTab->size(); i++)
    {
        TYAcousticSurfaceGeoNode* pAccSurfGeoNode = pTab->operator[](i);
        pTYElementGraphic = pAccSurfGeoNode->getGraphicObject();
        childs.push_back(pTYElementGraphic);
        if (recursif)
        {
            pTYElementGraphic->getChilds(childs, recursif);
        }
    }
}

void TYAcousticSurfaceNodeGraphic::computeBoundingBox()
{
    OBox reset;
    _boundingBox = reset;

    TYTabAcousticSurfaceGeoNode* pTab = &getElement()->getTabAcousticSurf();
    for (int i = 0; i < pTab->size(); i++)
    {
        TYAcousticSurfaceGeoNode* pAccSurfGeoNode = pTab->operator[](i);
        pAccSurfGeoNode->getGraphicObject()->computeBoundingBox();

        // CLM-NT33 : Correction calcul BB des machines
        //_boundingBox.Enlarge(pAccSurfGeoNode->getGraphicObject()->GetBox());
        OBox bb = pAccSurfGeoNode->getGraphicObject()->GetBox();
        bb.Translate(pAccSurfGeoNode->getORepere3D()._origin);
        _boundingBox.Enlarge(bb);
    }
}

void TYAcousticSurfaceNodeGraphic::display(GLenum mode /*= GL_RENDER*/)
{
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
        }

        TYTabAcousticSurfaceGeoNode* pTab = &getElement()->getTabAcousticSurf();
        for (int i = 0; i < pTab->size(); i++)
        {
            // Get face child
            TYAcousticSurfaceGeoNode* pAccSurfGeoNode = pTab->operator[](i);

            if (mode == GL_SELECT)
            {
                TYPickingTable::addElement(getElement());
                glPushName((GLuint)(TYPickingTable::getIndex()));
            }

            //pAccSurfGeoNode->getElement()->getGraphicObject()->highlight(_highlight); //sm++ repercution du highlight sur les elements composes.
            pAccSurfGeoNode->getGraphicObject()->display(mode);

            if (mode == GL_SELECT)
            {
                glPopName();
            }
        }
    }
}
