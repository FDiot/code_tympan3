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
 * \file TYRectangleGraphic.cpp
 * \brief Representation graphique d'un rectangle
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHGraphic.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYRectangle.h"


TYRectangleGraphic::TYRectangleGraphic(TYRectangle* pElement) :
    TYElementGraphic(pElement)
{
}

TYRectangleGraphic::~TYRectangleGraphic()
{
}

void TYRectangleGraphic::update(bool force /*=false*/)
{
    TYElementGraphic::update(force);
}

void TYRectangleGraphic::computeBoundingBox()
{
    OBox reset;
    _boundingBox = reset;

    for (int i = 0; i < 4; i++)
    {
        _boundingBox.Enlarge((float)(getElement()->_pts[i]._x), (float)(getElement()->_pts[i]._y), (float)(getElement()->_pts[i]._z));
    }
}

void TYRectangleGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    if (_highlight)
    {
        float color[4];
        glGetFloatv(GL_CURRENT_COLOR, color);
        if (_bFirstDisp)
        {
            computeBoundingBox();
            _bFirstDisp = false;
        }
        drawLineBoundingBox();
        if (mode == GL_COMPILE) { drawName(); }
        glColor3f(color[0], color[1], color[2]);
    }

    if (_visible)
    {

        glBegin(GL_QUADS);

        // Calcul de la normale a la face
        OVector3D normal = getElement()->normal();
        normal.normalize();
        glNormal3f(normal._x, normal._y, normal._z);

        for (int i = 0; i < 4; i++)
        {
            glVertex3f(getElement()->_pts[i]._x, getElement()->_pts[i]._y, getElement()->_pts[i]._z);
        }

        glEnd();
        //Calcul du volume englobant pour le fit:
        _globalBoundingBox.Enlarge(_boundingBox);

        if (TYElementGraphic::_gDrawNormals)
        {
            OPoint3D p1;
            OVector3D n = normal * 5;
            p1._x = (_boundingBox._max._x + _boundingBox._min._x) / 2;
            p1._y = (_boundingBox._max._y + _boundingBox._min._y) / 2;
            p1._z = (_boundingBox._max._z + _boundingBox._min._z) / 2;
            displayNormal(n, p1);
        }
    }
}
