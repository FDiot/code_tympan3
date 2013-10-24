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
 * \file TYAcousticVolumeNodeGraphic.cpp
 * \brief Representation graphique d'un ensemble de volumes acoustiques
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHGraphic.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticVolumeNode.h"


TYAcousticVolumeNodeGraphic::TYAcousticVolumeNodeGraphic(TYAcousticVolumeNode* pElement) :
    TYElementGraphic(pElement)
{
}

void TYAcousticVolumeNodeGraphic::update(bool force /*=false*/)
{
    TYElementGraphic::update(force);
}

void TYAcousticVolumeNodeGraphic::getChilds(TYListPtrTYElementGraphic& childs, bool recursif /*=true*/)
{
    TYTabAcousticVolumeGeoNode& pTab = getElement()->getTabAcousticVol();
    TYElementGraphic* pTYElementGraphic = NULL;
    TYAcousticVolumeGeoNode* pAccVolGeoNode = NULL;

    for (unsigned int i = 0; i < pTab.size(); i++)
    {
        pAccVolGeoNode = pTab[i];
        pTYElementGraphic = pAccVolGeoNode->getGraphicObject();
        childs.push_back(pTYElementGraphic);
        if (recursif)
        {
            pTYElementGraphic->getChilds(childs, recursif);
        }
    }
}

void TYAcousticVolumeNodeGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    if (!getElement()->isInCurrentCalcul())
    {
        return;
    }

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

        TYTabAcousticVolumeGeoNode* pTab = &getElement()->getTabAcousticVol();
        TYAcousticVolumeGeoNode* pAccVolGeoNode = NULL;

        for (unsigned int i = 0; i < pTab->size(); i++)
        {
            // Get face child
            pAccVolGeoNode = pTab->operator[](i);
            pAccVolGeoNode->getGraphicObject()->display(mode);
        }
    }
}

void TYAcousticVolumeNodeGraphic::computeBoundingBox()
{
    OBox reset;
    _boundingBox = reset;

    TYTabAcousticVolumeGeoNode* pTab = &getElement()->getTabAcousticVol();
    TYAcousticVolumeGeoNode* pAccVolGeoNode = NULL;

    for (unsigned int i = 0; i < pTab->size(); i++)
    {
        // Get face child
        pAccVolGeoNode = pTab->operator[](i);
        pAccVolGeoNode->getGraphicObject()->computeBoundingBox();

        // CLM-NT33 : Correction calcul BB des machines
        //_boundingBox.Enlarge(pAccVolGeoNode->getGraphicObject()->GetBox());
        OBox bb = pAccVolGeoNode->getGraphicObject()->GetBox();
        OPoint3D origin = pAccVolGeoNode->getRepere()._origin;
        bb.Translate(origin);
        _boundingBox.Enlarge(bb);
        // CLM-NT33
    }

    /*
    TYTabPoint sommets = getElement()->sommets();

    size_t nbPts = sommets.size();
    TYPoint pt;
    for (size_t i = 0; i < nbPts; i++)
    {
        pt = sommets[i];
        _boundingBox.Enlarge((float)(pt._x), (float)(pt._y), (float)(pt._z));
    }*/
}
