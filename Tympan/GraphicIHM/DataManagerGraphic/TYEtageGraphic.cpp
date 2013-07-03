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
 * \file TYEtageGraphic.cpp
 * \brief Representation graphique d'un etage
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHGraphic.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYEtage.h"


TYEtageGraphic::TYEtageGraphic(TYEtage* pElement) :
    TYElementGraphic(pElement)
{
    TYMurElement dummy;
    _pPolyLineGraphic = new TYPolyLineGraphic();//az++
    _pPolyLineGraphic->setElement(pElement);
}

TYEtageGraphic::~TYEtageGraphic()
{
    delete _pPolyLineGraphic;
}

void TYEtageGraphic::update(bool force /*=false*/)
{
    TYElementGraphic::update(force);
}

void TYEtageGraphic::getChilds(TYListPtrTYElementGraphic& childs, bool recursif /*=true*/)
{
    TYListPtrTYElementGraphic Localchilds;
    int i;
    for (i = 0; i < getElement()->getTabMachine().size(); i++)
    {
        Localchilds.push_back(getElement()->getTabMachine()[i]->getGraphicObject());
    }
    //Sources
    for (i = 0; i < getElement()->getTabSources().size(); i++)
    {
        Localchilds.push_back(getElement()->getTabSources()[i]->getGraphicObject());
    }

    // Murs
    for (i = 0; i < getElement()->getTabMur().size(); i++)
    {
        Localchilds.push_back(getElement()->getTabMur()[i]->getGraphicObject());
    }
    if (!getElement()->getClosed())
    {
        Localchilds.push_back(_pPolyLineGraphic);
    }
    else
    {
        Localchilds.push_back(getElement()->getSol()->getGraphicObject());
        Localchilds.push_back(getElement()->getPlafond()->getGraphicObject());
    }

    TYListPtrTYElementGraphic::iterator ite = Localchilds.begin();
    for (i = 0; i < Localchilds.size(); i++)
    {
        TYElementGraphic* pTYElementGraphic = (TYElementGraphic*)(*ite);
        childs.push_back(pTYElementGraphic);
        if (recursif)
        {
            pTYElementGraphic->getChilds(childs, recursif);
        }
        ite++;
    }
}

void TYEtageGraphic::computeBoundingBox()
{
    OBox reset;
    _boundingBox = reset;

    int i;
    for (i = 0; i < getElement()->getTabMachine().size(); i++)
    {
        getElement()->getTabMachine()[i]->getGraphicObject()->computeBoundingBox();
        _boundingBox.Enlarge(getElement()->getTabMachine()[i]->getGraphicObject()->GetBox());
    }

    //Sources
    for (i = 0; i < getElement()->getTabSources().size(); i++)
    {
        getElement()->getTabSources()[i]->getGraphicObject()->computeBoundingBox();
        _boundingBox.Enlarge(getElement()->getTabSources()[i]->getGraphicObject()->GetBox());
    }

    // Murs
    for (i = 0; i < getElement()->getTabMur().size(); i++)
    {
        getElement()->getTabMur()[i]->getGraphicObject()->computeBoundingBox();
        _boundingBox.Enlarge(getElement()->getTabMur()[i]->getGraphicObject()->GetBox());
    }

    // Sol et plafond
    TYPolyLineGraphic* pPolyLineGraphic = (TYPolyLineGraphic*)_pPolyLineGraphic;
    if (!getElement()->getClosed())
    {
        pPolyLineGraphic->computeBoundingBox();
        _boundingBox.Enlarge(pPolyLineGraphic->GetBox());
    }
    else
    {
        getElement()->getSol()->getGraphicObject()->computeBoundingBox();
        getElement()->getPlafond()->getGraphicObject()->computeBoundingBox();
        _boundingBox.Enlarge(getElement()->getSol()->getGraphicObject()->GetBox());
        _boundingBox.Enlarge(getElement()->getPlafond()->getGraphicObject()->GetBox());
    }
}

void TYEtageGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    int i;

    for (i = 0; i < getElement()->getTabMachine().size(); i++)
    {
        // Met l'attribut inCurrentCalcul a trou pour rendre la machine visible
        getElement()->getTabMachine()[i]->getElement()->setInCurrentCalcul(true);
        getElement()->getTabMachine()[i]->getGraphicObject()->display(mode);
    }

    //Sources
    for (i = 0; i < getElement()->getTabSources().size(); i++)
    {
        // Met l'attribut inCurrentCalcul a trou pour rendre la source visible
        getElement()->getTabSources()[i]->getElement()->setInCurrentCalcul(true);
        getElement()->getTabSources()[i]->getGraphicObject()->display(mode);
    }

    // Murs
    for (i = 0; i < getElement()->getTabMur().size(); i++)
    {
        getElement()->getTabMur()[i]->getGraphicObject()->display(mode);
    }

    // Sol et plafond
    TYPolyLineGraphic* pPolyLineGraphic = (TYPolyLineGraphic*)_pPolyLineGraphic;
    if (!getElement()->getClosed())
    {
        pPolyLineGraphic->setTabPoint(getElement()->sommets());
        pPolyLineGraphic->display(mode);
    }
    else
    {
        getElement()->getSol()->getGraphicObject()->display(mode);
        if (TYElementGraphic::_gDrawPlafond)
        {
            getElement()->getPlafond()->getGraphicObject()->display(mode);
        }
    }
}
