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
 * \file TYAcousticCircleGraphic.cpp
 * \brief Repri¿½sentation graphique d'un cercle accoustique
 *
 *
 */



#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHGraphic.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "TYAcousticCircleGraphic.h"

#include "Tympan/models/business/geoacoustic/TYAcousticCircle.h"

#include "Tympan/gui/gl/TYPickingTable.h"

TYAcousticCircleGraphic::TYAcousticCircleGraphic(TYAcousticCircle* pElement) :
    TYElementGraphic(pElement)
{
}

TYAcousticCircleGraphic::~TYAcousticCircleGraphic()
{
}

void TYAcousticCircleGraphic::update(bool force /*=false*/)
{
    TYElementGraphic::update(force);
}

void TYAcousticCircleGraphic::computeBoundingBox()
{
    OBox reset;
    _boundingBox = reset;

    double diametre = getElement()->getDiameter();
    _boundingBox.Enlarge((float)(-diametre / 2), (float)(-diametre / 2), (float)(getElement()->getCenter()._z));
    _boundingBox.Enlarge((float)(diametre / 2), (float)(diametre / 2), (float)(getElement()->getCenter()._z));
}

void TYAcousticCircleGraphic::display(GLenum mode /*= GL_RENDER*/)
{

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
            OColor tmpColor;
            glGetFloatv(GL_CURRENT_COLOR, tmpColor);
            glColor4fv(getElement()->getColor());

            double rayon = getElement()->getDiameter() / 2.0;

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

            GLUquadricObj* qobj = gluNewQuadric();
            glPushMatrix();
            glTranslatef(0.0, 0.0, getElement()->getCenter()._z);

            if (mode == GL_SELECT)
            {
                TYPickingTable::addElement(getElement());
                glPushName((GLuint)(TYPickingTable::getIndex()));
            }

            //Dessine un disque de centre [0 0 getElement()->getCenter()._z]:
            gluDiskAndEnlargeBB(qobj, 0, rayon, resolution, 1);

            if (mode == GL_SELECT)
            {
                glPopName();
            }
            gluDeleteQuadric(qobj);
            glPopMatrix();

            //Calcul du volume englobant pour le fit:
            _globalBoundingBox.Enlarge(_boundingBox);

            // Affichage de la normale
            if (TYElementGraphic::_gDrawNormals)
            {
                OPoint3D p1 = getElement()->getCenter();
                OVector3D n = getElement()->normal();
                n.normalize();
                n = n * 5;
                displayNormal(n, p1);
            }

            glColor3fv(tmpColor);

            // Affichage de la source surfacique
            getElement()->getSrcSurf()->getGraphicObject()->display(mode);
        }
    }
}


