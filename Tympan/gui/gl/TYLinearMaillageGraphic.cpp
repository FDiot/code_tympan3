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
 * \file TYLinearMaillageGraphic.cpp
 * \brief Representation graphique d'un maillage lineaire
 */


#include "Tympan/models/business/TYLinearMaillage.h"
#include "Tympan/gui/gl/TYPickingTable.h"
#include "Tympan/core/defines.h"
#include "Tympan/gui/OSizeManager.h"
#include "Tympan/gui/gl/TYPaletteGraphic.h"
#include "Tympan/gui/gl/TYPanelGraphic.h"
#include "TYLinearMaillageGraphic.h"




TYLinearMaillageGraphic::TYLinearMaillageGraphic(TYLinearMaillage* pElement) :
    TYElementGraphic(pElement)
{
    _scalarX = 0.8;
    _scalarY = 0.25;

    _scalarW = 150.0;
    _scalarH = 15.0;

    _panelW = 125.0;
    _panelH = 40.0;

    TYElementGraphic* pTYElementGraphic = getElement()->getPalette()->getGraphicObject();
    ((TYPaletteGraphic*)pTYElementGraphic)->setWidth(_scalarW);
    ((TYPaletteGraphic*)pTYElementGraphic)->setHeight(_scalarH);

    pTYElementGraphic = getElement()->getPanel()->getGraphicObject();
    ((TYPanelGraphic*)pTYElementGraphic)->setWidth(_panelW);
    ((TYPanelGraphic*)pTYElementGraphic)->setHeight(_panelH);
    pTYElementGraphic->setVisible(false);
}

TYLinearMaillageGraphic::~TYLinearMaillageGraphic()
{

}

void TYLinearMaillageGraphic::update(bool force /*=false*/)
{
    TYLinearMaillage* pMaillage = getElement();
    pMaillage->getPalette()->getGraphicObject()->update(force);

    TYElementGraphic::update(force);
}

void TYLinearMaillageGraphic::computeBoundingBox()
{
    OBox reset;
    _boundingBox = reset;
    TYTabLPPointCalcul* pPtsCalcul = &getElement()->getPtsCalcul();
    TYPointCalcul* pPtCalcul = NULL;
    for (unsigned int i = 0; i < pPtsCalcul->size(); i++)
    {
        pPtCalcul = pPtsCalcul->at(i);
        _boundingBox.Enlarge((float)(pPtCalcul->_x), (float)(pPtCalcul->_y), (float)(pPtCalcul->_z));
    }
}

void TYLinearMaillageGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    // Pas affiche si inactif
    if (getElement()->getState() == TYMaillage::Inactif)
    {
        return;
    }

    // Les points de ce maillage
    TYTabLPPointCalcul* pPtsCalcul = &getElement()->getPtsCalcul();

    if (_visible)
    {

        if (mode == GL_SELECT)
        {
            TYPickingTable::addElement(getElement());
            glPushName((GLuint)(TYPickingTable::getIndex()));
        }

        float opacity = 1.0;

#if TY_USE_IHM
        //      static const char prefName[] = "MaillageOpacity";

        if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "MaillageOpacity"))
        {
            opacity = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, "MaillageOpacity");
        }
        else
        {
            TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, "MaillageOpacity", opacity);
        }
#endif // TY_USE_IHM

        glBegin(GL_LINE_STRIP);
        TYPointCalcul* pPtCalcul = NULL;
        double value = 0.0;
        for (unsigned int i = 0; i < pPtsCalcul->size(); i++)
        {
            pPtCalcul = pPtsCalcul->at(i);

            // Valeur du scalaire pour la color map
            switch (getElement()->getDataType())
            {
                case TYMaillage::ValGlobalDBA:
                default:
                    value = pPtCalcul->getSpectre()->valGlobDBA();
                    break;
                case TYMaillage::ValGlobalDBLin:
                    value = pPtCalcul->getSpectre()->valGlobDBLin();
                    break;
                case TYMaillage::DataFreq:
                    value = pPtCalcul->getSpectre()->getValueReal(getElement()->getDataFreq());
                    break;
            }
            const OColor& color = getElement()->getPalette()->getColorFromValue(value);
            glColor4f(color.r, color.g, color.b, opacity);
            glVertex3f(pPtCalcul->_x, pPtCalcul->_y, pPtCalcul->_z);
        }
        glEnd();

        if (mode == GL_SELECT)
        {
            glPopName();
        }

        // Affichage du panel
        TYElementGraphic* pTYElementGraphic = getElement()->getPanel()->getGraphicObject();
        TYPanelGraphic* pTYPanelGraphic = (TYPanelGraphic*)pTYElementGraphic;
        pTYPanelGraphic->display();

        //Calcul du volume englobant pour le fit:
        _globalBoundingBox.Enlarge(_boundingBox);

    }

    if (_highlight)
    {
        GLfloat pointSize;
        glGetFloatv(GL_POINT_SIZE, &pointSize);

        if (_bFirstDisp)
        {
            computeBoundingBox();
            _bFirstDisp = false;
        }
        drawLineBoundingBox();
        if (mode == GL_COMPILE) { drawName(); }

        glPointSize(pointSize);

        TYElementGraphic* pTYElementGraphic = getElement()->getPalette()->getGraphicObject();
        TYPaletteGraphic* pTYPaletteGraphic = (TYPaletteGraphic*)pTYElementGraphic;
        pTYPaletteGraphic->setPosX(OSizeManager::get()->getWidth()*_scalarX);
        pTYPaletteGraphic->setPosY(OSizeManager::get()->getHeight()*_scalarY);
        pTYPaletteGraphic->display();
    }

}
