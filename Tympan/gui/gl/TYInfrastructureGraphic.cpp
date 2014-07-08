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
 * \file TYInfrastructureGraphic.cpp
 * \brief Representation graphique d'une infrastructure
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHGraphic.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/models/business/infrastructure/TYInfrastructure.h"


TYInfrastructureGraphic::TYInfrastructureGraphic(TYInfrastructure* pElement) :
    TYElementGraphic(pElement)
{
}

void TYInfrastructureGraphic::update(bool force /*=false*/)
{
    TYElementGraphic::update(force);
}

void TYInfrastructureGraphic::getChilds(TYListPtrTYElementGraphic& childs, bool recursif /*=true*/)
{
    TYListPtrTYElementGraphic Localchilds;
    unsigned int i;
    TYTabRouteGeoNode* pTabRoute = &getElement()->getListRoute();
    TYRouteGeoNode* pRouteGeoNode = NULL;
    for (i = 0; i < pTabRoute->size(); i++)
    {
        pRouteGeoNode = pTabRoute->at(i);
        Localchilds.push_back(pRouteGeoNode->getGraphicObject());
    }

    // Reseaux de transport
    TYTabReseauTransportGeoNode* pTabResTransp = &getElement()->getListResTrans();
    TYReseauTransportGeoNode* pResTranspGeoNode = NULL;
    for (i = 0; i < pTabResTransp->size(); i++)
    {
        pResTranspGeoNode = pTabResTransp->at(i);
        Localchilds.push_back(pResTranspGeoNode->getGraphicObject());
    }

    // Bâtiments
    TYTabBatimentGeoNode* pTabBat = &getElement()->getListBatiment();
    TYBatimentGeoNode* pBatGeoNode = NULL;
    for (i = 0; i < pTabBat->size(); i++)
    {
        pBatGeoNode = pTabBat->at(i);
        Localchilds.push_back(pBatGeoNode->getGraphicObject());
    }

    // Machines
    TYTabMachineGeoNode* pTabMachine = &getElement()->getListMachine();
    TYMachineGeoNode* pMachineGeoNode = NULL;
    for (i = 0; i < pTabMachine->size(); i++)
    {
        pMachineGeoNode = pTabMachine->at(i);
        Localchilds.push_back(pMachineGeoNode->getGraphicObject());
    }

    // Source ponctuelles utilisateur
    TYTabSourcePonctuelleGeoNode* pTabSrc = &getElement()->getSrcs();
    LPTYSourcePonctuelleGeoNode pSrcPonctGeoNode = NULL;
    for (i = 0; i < pTabSrc->size(); i++)
    {
        pSrcPonctGeoNode = pTabSrc->at(i);
        Localchilds.push_back(pSrcPonctGeoNode->getGraphicObject());
    }

    TYListPtrTYElementGraphic::iterator ite = Localchilds.begin();
    TYElementGraphic* pTYElementGraphic = NULL;
    for (i = 0; i < Localchilds.size(); i++)
    {
        pTYElementGraphic = (TYElementGraphic*)(*ite);
        childs.push_back(pTYElementGraphic);
        if (recursif)
        {
            pTYElementGraphic->getChilds(childs, recursif);
        }
        ite++;
    }
}

void TYInfrastructureGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    unsigned int i;

    // Routes
    TYTabRouteGeoNode* pTabRoute = &getElement()->getListRoute();
    TYRouteGeoNode* pRouteGeoNode = NULL;
    for (i = 0; i < pTabRoute->size(); i++)
    {
        pRouteGeoNode = pTabRoute->at(i);
        pRouteGeoNode->getGraphicObject()->display(mode);
    }

    // Reseaux de transport
    TYTabReseauTransportGeoNode* pTabResTransp = &getElement()->getListResTrans();
    TYReseauTransportGeoNode* pResTranspGeoNode = NULL;
    for (i = 0; i < pTabResTransp->size(); i++)
    {
        pResTranspGeoNode = pTabResTransp->at(i);
        pResTranspGeoNode->getGraphicObject()->display(mode);
    }

    // Bâtiments
    TYTabBatimentGeoNode* pTabBat = &getElement()->getListBatiment();
    TYBatimentGeoNode* pBatGeoNode = NULL;
    for (i = 0; i < pTabBat->size(); i++)
    {
        pBatGeoNode = pTabBat->at(i);
        pBatGeoNode->getGraphicObject()->display(mode);
    }

    // Machines
    TYTabMachineGeoNode* pTabMachine = &getElement()->getListMachine();
    TYMachineGeoNode* pMachineGeoNode = NULL;
    for (i = 0; i < pTabMachine->size(); i++)
    {
        pMachineGeoNode = pTabMachine->at(i);
        pMachineGeoNode->getGraphicObject()->display(mode);
    }

    // Source ponctuelles utilisateur
    TYTabSourcePonctuelleGeoNode* pTabSrc = &getElement()->getSrcs();
    LPTYSourcePonctuelleGeoNode pSrcPonctGeoNode = NULL;
    for (i = 0; i < pTabSrc->size(); i++)
    {
        pSrcPonctGeoNode = pTabSrc->at(i);
        pSrcPonctGeoNode->getGraphicObject()->display(mode);
    }
}
