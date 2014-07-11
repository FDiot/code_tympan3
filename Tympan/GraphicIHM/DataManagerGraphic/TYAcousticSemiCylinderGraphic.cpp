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
 * \file TYAcousticSemiCylinderGraphic.cpp
 * \brief Repri¿½sentation graphique d'un 1/2 cylindre acoustique
 *
 *
 */





#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHGraphic.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/GraphicIHM/DataManagerGraphic/TYAcousticSemiCylinderGraphic.h"
#include "Tympan/models/business/geoacoustic/TYAcousticSemiCylinder.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYPickingTable.h"


TYAcousticSemiCylinderGraphic::TYAcousticSemiCylinderGraphic(TYAcousticSemiCylinder* pElement) :
    TYElementGraphic(pElement)
{
}

TYAcousticSemiCylinderGraphic::~TYAcousticSemiCylinderGraphic()
{
}

void TYAcousticSemiCylinderGraphic::update(bool force /*=false*/)
{
    TYElementGraphic::update(force);
}

void TYAcousticSemiCylinderGraphic::getChilds(TYListPtrTYElementGraphic& childs, bool recursif /*=true*/)
{
    TYElementGraphic* pTYElementGraphic;
    pTYElementGraphic = getElement()->getSrcSurf()->getGraphicObject();
    childs.push_back(pTYElementGraphic);
    if (recursif)
    {
        pTYElementGraphic->getChilds(childs, recursif);
    }

    pTYElementGraphic = getElement()->getSemiCircTop()->getGraphicObject();
    childs.push_back(pTYElementGraphic);
    if (recursif)
    {
        pTYElementGraphic->getChilds(childs, recursif);
    }

    pTYElementGraphic = getElement()->getSemiCircBottom()->getGraphicObject();
    childs.push_back(pTYElementGraphic);
    if (recursif)
    {
        pTYElementGraphic->getChilds(childs, recursif);
    }

    // Face laterale
    pTYElementGraphic = getElement()->getRect()->getGraphicObject();
    childs.push_back(pTYElementGraphic);
    if (recursif)
    {
        pTYElementGraphic->getChilds(childs, recursif);
    }

    // Resolution
    int resolution = TYDEFAULTRESOLUTIONIONCIRCLE;
    TYTabRectangle tabRect = getElement()->getEnveloppe(resolution);
    for (int i = 0; i < resolution; i++)
    {
        pTYElementGraphic = tabRect[i].getGraphicObject();
        childs.push_back(pTYElementGraphic);
        if (recursif)
        {
            pTYElementGraphic->getChilds(childs, recursif);
        }
    }
}

void TYAcousticSemiCylinderGraphic::computeBoundingBox()
{
    OBox reset;
    _boundingBox = reset;

    // Src surf
    getElement()->getSrcSurf()->getGraphicObject()->computeBoundingBox();
    _boundingBox.Enlarge(getElement()->getSrcSurf()->getGraphicObject()->GetBox());

    // Cercle top
    getElement()->getSemiCircTop()->getGraphicObject()->computeBoundingBox();
    _boundingBox.Enlarge(getElement()->getSemiCircTop()->getGraphicObject()->GetBox());

    // Cercle bottom
    getElement()->getSemiCircBottom()->getGraphicObject()->computeBoundingBox();
    _boundingBox.Enlarge(getElement()->getSemiCircBottom()->getGraphicObject()->GetBox());
}

void TYAcousticSemiCylinderGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    //TYElementGraphic::display(mode);

    // CLM-NT35: Affiche que le nom de l'element localisi¿½ en overlay
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


        if (_visible)
        {
            // Src surf
            getElement()->getSrcSurf()->getGraphicObject()->display(mode);


            if (mode == GL_SELECT)
            {
                TYPickingTable::addElement(getElement());
                glPushName((GLuint)(TYPickingTable::getIndex()));
            }

            // Face laterale
            getElement()->getRect()->getGraphicObject()->display(mode);

            // Cercle top
            getElement()->getSemiCircTop()->getGraphicObject()->display(mode);

            // Cercle bottom
            getElement()->getSemiCircBottom()->getGraphicObject()->display(mode);

            // Couleur
            glColor3fv(getElement()->getColor());

            // Resolution
            int resolution = TYDEFAULTRESOLUTIONIONCIRCLE;

#if TY_USE_IHM
            //      static const char prefName[] = "ResolutionCircle";

            if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "ResolutionCircle"))
            {
                resolution = TYPreferenceManager::getInt(TYDIRPREFERENCEMANAGER, "ResolutionCircle");
            }
            else
            {
                TYPreferenceManager::setInt(TYDIRPREFERENCEMANAGER, "ResolutionCircle", resolution);
            }

#endif // TY_USE_IHM

            glTranslatef(getElement()->getCenter()._x, getElement()->getCenter()._y, getElement()->getCenter()._z);


            // Dessin de l'enveloppe
            TYTabRectangle tabRect = getElement()->getEnveloppe(resolution);

            for (int i = 0; i < resolution; i++)
            {
                tabRect[i].getGraphicObject()->display(mode);
            }

            if (mode == GL_SELECT)
            {
                glPopName();
                //Calcul du volume englobant pour le fit:
                _globalBoundingBox.Enlarge(_boundingBox);
            }
        }
    }
}
