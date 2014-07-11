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
 * \file TYProjetGraphic.cpp
 * \brief Representation graphique d'un projet
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHGraphic.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/models/business/TYProjet.h"
//Added by qt3to4:
//#include <QSignal>


TYProjetGraphic::TYProjetGraphic(TYProjet* pElement) :
    TYElementGraphic(pElement)
{
#if TY_USE_IHM
    //  _pUpdateSignal = new Q3Signal();
#endif // TY_USE_IHM
}

TYProjetGraphic::~TYProjetGraphic()
{
#if TY_USE_IHM
    //  delete _pUpdateSignal;
#endif // TY_USE_IHM
}

void TYProjetGraphic::update(bool force /*=false*/)
{
    TYElementGraphic::update(force);
}

void TYProjetGraphic::getChilds(TYListPtrTYElementGraphic& childs, bool recursif /*=true*/)
{
    TYElementGraphic* pTYElementGraphic = getElement()->getSite()->getGraphicObject();
    childs.push_back(pTYElementGraphic);
    if (recursif)
    {
        pTYElementGraphic->getChilds(childs, recursif);
    }
    if (getElement()->getCurrentCalcul())
    {
        pTYElementGraphic = getElement()->getCurrentCalcul()->getGraphicObject();
        childs.push_back(pTYElementGraphic);
        if (recursif)
        {
            pTYElementGraphic->getChilds(childs, recursif);
        }
    }

    // Points de controle
    unsigned int i;
    for (i = 0; i < getElement()->getPointsControl().size(); i++)
    {
        pTYElementGraphic = getElement()->getPointControl(i)->getGraphicObject();
        childs.push_back(pTYElementGraphic);
        if (recursif)
        {
            pTYElementGraphic->getChilds(childs, recursif);
        }
    }
}

void TYProjetGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    TYElementGraphic::display(mode);

    // Site
    getElement()->getSite()->getGraphicObject()->display(mode);

    // Points de controle
    unsigned int i;
    for (i = 0; i < getElement()->getPointsControl().size(); i++)
    {
        getElement()->getPointControl(i)->getGraphicObject()->display(mode);
    }

    // Calcul courant
    if (getElement()->getCurrentCalcul())
    {
        getElement()->getCurrentCalcul()->getGraphicObject()->display(mode);
    }

    activateSignal();
}

#if TY_USE_IHM
void TYProjetGraphic::connectUpdateSignal(QObject* pReceiver, const char* member)
{
    //  _pUpdateSignal->connect(pReceiver, member);
}
#endif // TY_USE_IHM

#if TY_USE_IHM
void TYProjetGraphic::disconnectUpdateSignal(QObject* pReceiver, const char* member)
{
    //  _pUpdateSignal->disconnect(pReceiver, member);
}
#endif // TY_USE_IHM
