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
 * \file TYPolyLineGraphic.cpp
 * \brief Representation graphique d'une polyligne
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHGraphic.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "TYPolyLineGraphic.h"

#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYPoint.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYRectangle.h"


TYPolyLineGraphic::TYPolyLineGraphic(bool closed /*=false*/)
    : TYElementGraphic(NULL)
    , width(default_width_px)
{
    _closed = closed;
}

TYPolyLineGraphic::~TYPolyLineGraphic()
{
}

void TYPolyLineGraphic::update(bool force /*=false*/)
{
    TYElementGraphic::update(force);
}

void TYPolyLineGraphic::computeBoundingBox()
{
    OBox reset;
    _boundingBox = reset;

    size_t nbPts = _tabPts.size();
    for (size_t i = 0; i < nbPts; i++)
    {
        TYPoint pt(_tabPts[i % (_tabPts.size())]);
        _boundingBox.Enlarge((float)(pt._x), (float)(pt._y), (float)(pt._z));
    }
}

void TYPolyLineGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    size_t nbPts = _tabPts.size();
    float colorTYPolyLineGraphic[4];
    glGetFloatv(GL_CURRENT_COLOR, colorTYPolyLineGraphic);

    if (_highlight)
    {
        if (_bFirstDisp)
        {
            computeBoundingBox();
            _bFirstDisp = false;
        }
        drawLineBoundingBox();
        if (mode == GL_COMPILE) { drawName(); }
        glColor3f(colorTYPolyLineGraphic[0], colorTYPolyLineGraphic[1], colorTYPolyLineGraphic[2]);
    }

    if (_visible)
    {
        GLenum line_mode;

        if (_closed)
        {
            line_mode = GL_LINE_LOOP;
        }
        else
        {
            line_mode = GL_LINE_STRIP;
        }

        glLineWidth(width);

        glBegin(line_mode);

        for (size_t i = 0; i < nbPts; i++)
        {
            TYPoint pt(_tabPts[i % (_tabPts.size())]);
            glVertex3f(pt._x, pt._y, pt._z);
        }

        glEnd();
        glColor3f(colorTYPolyLineGraphic[0], colorTYPolyLineGraphic[1], colorTYPolyLineGraphic[2]);
        //Calcul du volume englobant pour le fit:
        _globalBoundingBox.Enlarge(_boundingBox);
    }
}
