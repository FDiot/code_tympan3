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
 * \file TYCalculGraphic.cpp
 * \brief Representation graphique d'un calcul
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHGraphic.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYProjet.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYCalcul.h"


TYCalculGraphic::TYCalculGraphic(TYCalcul* pElement) :
    TYElementGraphic(pElement)
{
}

void TYCalculGraphic::update(bool force /*=false*/)
{
    TYElementGraphic::update(force);
}

void TYCalculGraphic::getChilds(TYListPtrTYElementGraphic& childs, bool recursif /*=true*/)
{
    unsigned int i;
    TYElementGraphic* pTYElementGraphic = NULL;

    // Maillage
    TYTabMaillageGeoNode* pTabMaillage = &getElement()->getMaillages();
    TYMaillageGeoNode* pMaillageGeoNode = NULL;
    for (i = 0; i < pTabMaillage->size(); i++)
    {
        pMaillageGeoNode = pTabMaillage->operator[](i);
        pTYElementGraphic = pMaillageGeoNode->getGraphicObject();
        childs.push_back(pTYElementGraphic);
        if (recursif)
        {
            pTYElementGraphic->getChilds(childs, recursif);
        }
    }
}

void TYCalculGraphic::computeBoundingBox()
{
    unsigned int i;
    OBox reset;
    _boundingBox = reset;

    // Maillage
    TYTabMaillageGeoNode* pTabMaillage = &getElement()->getMaillages();
    TYMaillageGeoNode* pMaillageGeoNode = NULL;
    for (i = 0; i < pTabMaillage->size(); i++)
    {
        pMaillageGeoNode = pTabMaillage->operator[](i);
        pMaillageGeoNode->getGraphicObject()->computeBoundingBox();
        _boundingBox.Enlarge(pMaillageGeoNode->getGraphicObject()->GetBox());
    }
}

void TYCalculGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    unsigned int i;
    // Maillage
    TYTabMaillageGeoNode* pTabMaillage = &getElement()->getMaillages();
    TYMaillageGeoNode* pMaillageGeoNode = NULL;
    for (i = 0; i < pTabMaillage->size(); i++)
    {
        pMaillageGeoNode = pTabMaillage->operator[](i);
        pMaillageGeoNode->getGraphicObject()->display(mode);
    }


    // Rayons
    if (TYRayGraphic::_gVisible)
    {
        TYTabRay* pTabRays = &getElement()->getAllRays();
        for (i = 0; i < pTabRays->size(); i++)
        {
            TYRay* r = pTabRays->at(i);
            if (r != NULL)
            {
                r->getGraphicObject()->display(mode);
            }
        }
    }
}

