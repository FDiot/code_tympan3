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
 * \file TYAcousticCylinderGraphic.cpp
 * \brief Repri¿½sentation graphique d'un cylindre accoustique
 *
 *
 */



#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHGraphic.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/gui/gl/TYAcousticCylinderGraphic.h"

#include "Tympan/models/business/geoacoustic/TYAcousticCylinder.h"
#include "Tympan/gui/gl/TYPickingTable.h"

TYAcousticCylinderGraphic::TYAcousticCylinderGraphic(TYAcousticCylinder* pElement) :
    TYElementGraphic(pElement)
{
}

TYAcousticCylinderGraphic::~TYAcousticCylinderGraphic()
{
}

void TYAcousticCylinderGraphic::update(bool force /*=false*/)
{
    TYElementGraphic::update(force);
}

void TYAcousticCylinderGraphic::getChilds(TYListPtrTYElementGraphic& childs, bool recursif /*=true*/)
{
    TYElementGraphic* pTYElementGraphic;
    // Src surf
    pTYElementGraphic = getElement()->getSrcSurf()->getGraphicObject();
    childs.push_back(pTYElementGraphic);
    if (recursif)
    {
        pTYElementGraphic->getChilds(childs, recursif);
    }
    // Cercle top
    pTYElementGraphic = getElement()->getCircTop()->getGraphicObject();
    childs.push_back(pTYElementGraphic);
    if (recursif)
    {
        pTYElementGraphic->getChilds(childs, recursif);
    }
    // Cercle bottom
    pTYElementGraphic = getElement()->getCircBottom()->getGraphicObject();
    childs.push_back(pTYElementGraphic);
    if (recursif)
    {
        pTYElementGraphic->getChilds(childs, recursif);
    }
}

void TYAcousticCylinderGraphic::computeBoundingBox()
{
    OBox reset;
    _boundingBox = reset;

    TYTabPoint sommets = getElement()->sommets();

    size_t nbPts = sommets.size();
    TYPoint pt;
    for (size_t i = 0; i < nbPts; i++)
    {
        pt = sommets[i];
        _boundingBox.Enlarge((float)(pt._x), (float)(pt._y), (float)(pt._z));
    }
}

void TYAcousticCylinderGraphic::display(GLenum mode /*= GL_RENDER*/)
{
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
            if (mode == GL_SELECT)
            {
                TYPickingTable::addElement(getElement());
                glPushName((GLuint)(TYPickingTable::getIndex()));
            }

            // Enveloppe
            getElement()->getSrcSurf()->getGraphicObject()->display(mode);
            // Cercle top
            getElement()->getCircTop()->getGraphicObject()->display(mode);
            // Cercle bottom
            getElement()->getCircBottom()->getGraphicObject()->display(mode);

            // Couleur
            glColor3fv(getElement()->getColor());

            // Centre
            float center[3];
            getElement()->getCenter().getToOGL(center);

            // Resolution
            int resolution = TYDEFAULTRESOLUTIONIONCIRCLE;

#if TY_USE_IHM
            if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "ResolutionCircle"))
            {
                resolution = TYPreferenceManager::getInt(TYDIRPREFERENCEMANAGER, "ResolutionCircle");
            }
            else
            {
                TYPreferenceManager::setInt(TYDIRPREFERENCEMANAGER, "ResolutionCircle", resolution);
            }

#endif // TY_USE_IHM

            OPoint3D oCenter = getElement()->getCenter();
            glTranslatef(oCenter._x, oCenter._y, oCenter._z);

            // Dessin de l'enveloppe
            TYTabRectangle tabRect = getElement()->getEnveloppe(resolution);

            for (int i = 0; i < resolution; i++)
            {
                tabRect[i].getGraphicObject()->display(mode);
            }


            if (mode == GL_SELECT)
            {
                glPopName();
                _globalBoundingBox.Enlarge(_boundingBox);
            }
        }
    }
}

