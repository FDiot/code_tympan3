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
 * \file TYAcousticRectangleGraphic.cpp
 * \brief Representation graphique d'un rectangle accoustique
 *
 *
 */




#include "Tympan/models/business/geoacoustic/TYAcousticRectangle.h"

#include "Tympan/models/business/TYPreferenceManager.h"


TYAcousticRectangleGraphic::TYAcousticRectangleGraphic(TYAcousticRectangle* pElement) :
    TYElementGraphic(pElement)
{
}

void TYAcousticRectangleGraphic::update(bool force /*=false*/)
{
    TYElementGraphic::update();
}

void TYAcousticRectangleGraphic::getChilds(TYListPtrTYElementGraphic& childs, bool recursif /*=true*/)
{
    TYElementGraphic* pTYElementGraphic;
    pTYElementGraphic = getElement()->getSrcSurf()->getGraphicObject();
    childs.push_back(pTYElementGraphic);
    if (recursif)
    {
        pTYElementGraphic->getChilds(childs, recursif);
    }

    pTYElementGraphic = getElement()->getShape()->getGraphicObject();
    childs.push_back(pTYElementGraphic);
    if (recursif)
    {
        pTYElementGraphic->getChilds(childs, recursif);
    }
}

void TYAcousticRectangleGraphic::computeBoundingBox()
{
    OBox reset;
    _boundingBox = reset;

    getElement()->getSrcSurf()->getGraphicObject()->computeBoundingBox();
    getElement()->getShape()->getGraphicObject()->computeBoundingBox();
    _boundingBox.Enlarge(getElement()->getSrcSurf()->getGraphicObject()->GetBox());
    _boundingBox.Enlarge(getElement()->getShape()->getGraphicObject()->GetBox());
}

void TYAcousticRectangleGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    // CLM-NT35: Affiche que le nom de l'element localise en overlay
    if (mode == GL_COMPILE)
    {
        drawName();
    }
    else
    {
        OColor color = getElement()->getColor();
        color.a = 1.0;

#if TY_USE_IHM
        if (getElement()->getIsSub() && strcmp(getElement()->getClassName(), "TYMurElement") == 0)
        {
            if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "WinOpacity"))
            {
                color.a = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "WinOpacity");
            }
            else
            {
                TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "WinOpacity", color.a);
            }
        }
#endif // TY_USE_IHM

        if (_highlight)
        {
            float tmpColor[4];
            glGetFloatv(GL_CURRENT_COLOR, tmpColor);
            if (_bFirstDisp)
            {
                computeBoundingBox();
                _bFirstDisp = false;
            }
            drawLineBoundingBox();
            glColor3f(tmpColor[0], tmpColor[1], tmpColor[2]);
        }

        if (_visible)
        {
            glColor4fv(color);

            getElement()->getSrcSurf()->getGraphicObject()->display(mode);
            getElement()->getShape()->setInCurrentCalcul(getElement()->isInCurrentCalcul());
            // Update shape

            if ((getElement()->getIsSub()) && (mode == GL_SELECT))
            {
                TYPickingTable::addElement(getElement());
                glPushName((GLuint)(TYPickingTable::getIndex()));
            }

            getElement()->getShape()->getGraphicObject()->display(mode);

            if ((getElement()->getIsSub()) && (mode == GL_SELECT))
            {
                glPopName();
            }

            //Calcul du volume englobant pour le fit:
            _globalBoundingBox.Enlarge(_boundingBox);
        }
    }
}
