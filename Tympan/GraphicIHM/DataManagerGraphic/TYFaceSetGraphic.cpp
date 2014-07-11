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
 * \file TYFaceSetGraphic.cpp
 * \brief Representation d'un ensemble de faces
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHGraphic.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/models/business/geometry/TYFaceSet.h"
#include "Tympan/models/business/geometry/TYPolygon.h"

TYFaceSetGraphic::TYFaceSetGraphic(TYFaceSet* pElement) :
    TYElementGraphic(pElement)
{
}

TYFaceSetGraphic::~TYFaceSetGraphic()
{
}

void TYFaceSetGraphic::update(bool force /*=false*/)
{
    TYElementGraphic::update(force);
}

void TYFaceSetGraphic::computeBoundingBox()
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

void TYFaceSetGraphic::display(GLenum mode /*= GL_RENDER*/)
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
        TYTabLPPolygon faces = getElement()->faces();
        TYTabVector normals = getElement()->normals();
        size_t nbFaces = faces.size();

        // Polygon representant la face
        TYPolygon* pFace = NULL;
        OVector3D normal;
        OPoint3D sommet;

        //      glBegin(GL_QUADS);

        for (int i = 0; i < nbFaces; i++)
        {
            pFace = faces[i];

            if ((pFace != NULL) && (pFace->getNbPts() == 4))
            {
                glBegin(GL_QUADS);
                for (int j = 0; j < 4; j++)
                {
                    sommet = pFace->getPoint(j);
                    glVertex3f(sommet._x, sommet._y, sommet._z);
                }
                glEnd();

                if (TYElementGraphic::_gDrawNormals)
                {
                    normal = normals[i];
                    glNormal3f(normal._x, normal._y, normal._z);
                    OPoint3D p1 = pFace->getCenter();
                    displayNormal(normal * 5, p1);
                }
            }
        }

        //      glEnd();

        //Calcul du volume englobant pour le fit:
        _globalBoundingBox.Enlarge(_boundingBox);
    }
}

