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
 * \file TYAcousticSemiCircleGraphic.cpp
 * \brief Repri¿½sentation graphique d'1/2 cercle acoustique
 */


#include "Tympan/models/business/TYPreferenceManager.h"
#include "Tympan/models/business/geometry/TYPolygon.h"
#include "Tympan/models/business/geoacoustic/TYAcousticSemiCircle.h"
#include "Tympan/gui/gl/TYPickingTable.h"
#include "TYAcousticSemiCircleGraphic.h"

TYAcousticSemiCircleGraphic::TYAcousticSemiCircleGraphic(TYAcousticSemiCircle* pElement) :
    TYElementGraphic(pElement)
{
}

TYAcousticSemiCircleGraphic::~TYAcousticSemiCircleGraphic()
{
}

void TYAcousticSemiCircleGraphic::update(bool force /*=false*/)
{
    TYElementGraphic::update(force);
}

void TYAcousticSemiCircleGraphic::computeBoundingBox()
{
    OBox reset;
    _boundingBox = reset;

    double rayon = getElement()->getDiameter() / 2.0;
    _boundingBox.Enlarge((float)(getElement()->getCenter()._x + rayon), (float)(getElement()->getCenter()._y + rayon), (float)(getElement()->getCenter()._z));
    _boundingBox.Enlarge((float)(getElement()->getCenter()._x - rayon), (float)(getElement()->getCenter()._y - rayon), (float)(getElement()->getCenter()._z));
}

void TYAcousticSemiCircleGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    // CLM-NT35: Affiche que le nom de l'element localisi¿½ en overlay
    if (mode == GL_COMPILE)
    {
        drawName();
    }
    else
    {
        //TYElementGraphic::display(mode);
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
            OColor tmpColor;
            glGetFloatv(GL_CURRENT_COLOR, tmpColor);
            //glColor4fv(getElement()->getColor());
            glColor3fv(getElement()->getColor());

            double rayon = getElement()->getDiameter() / 2.0;

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

            GLUquadricObj* qobj = gluNewQuadric();
            glPushMatrix();
            glTranslatef(getElement()->getCenter()._x, getElement()->getCenter()._y, getElement()->getCenter()._z);

            if (mode == GL_SELECT)
            {
                TYPickingTable::addElement(getElement());
                glPushName((GLuint)(TYPickingTable::getIndex()));
            }

            gluPartialDiskAndEnlargeBB(qobj, 0, rayon, resolution, 1, 0, 180);

            if (mode == GL_SELECT)
            {
                glPopName();
            }

            gluDeleteQuadric(qobj);
            glPopMatrix();
            //Calcul du volume englobant pour le fit:
            _globalBoundingBox.Enlarge(_boundingBox);

            // Affichage de la source surfacique
            getElement()->getSrcSurf()->getGraphicObject()->display(mode);
        }
    }
}
