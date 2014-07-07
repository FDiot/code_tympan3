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
 * \file TYPolygonGraphic.cpp
 * \brief Representation graphique d'un polygone
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHGraphic.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYPolygon.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYRectangle.h"

// portability

#ifndef STDCALL
# ifdef _WIN32
#  define STDCALL __stdcall
# else
#  define STDCALL
//__attribute__((stdcall))
# endif
#endif


TYPolygonGraphic::TYPolygonGraphic(TYPolygon* pElement) :
    TYElementGraphic(pElement)
{
    _altimetrie = false;
    _gLUtesselator = NULL;
    _textureBg = false;
}

TYPolygonGraphic::~TYPolygonGraphic()
{
    if (_gLUtesselator) { gluDeleteTess(_gLUtesselator); }
}

void TYPolygonGraphic::update(bool force /*=false*/)
{
    TYElementGraphic::update(force);
}

void STDCALL beginCallback(GLenum which)
{
    glBegin(which);
}
void STDCALL endCallback()
{
    glEnd();
}
void STDCALL vertexCallback(GLdouble* vertex)
{
    glTexCoord2dv(vertex + 3);
    glVertex3dv(vertex);
}
void errorCallback(GLenum errorCode)
{
    const unsigned char* sError = gluErrorString(errorCode);
    printf("Erreur OpenGL %d : %s\n", errorCode, sError);
}

void TYPolygonGraphic::computeBoundingBox()
{
    OBox reset;
    _boundingBox = reset;

    size_t nbPts = getElement()->getNbPts();
    for (size_t i = 0; i < nbPts; i++)
    {
        OPoint3D pt = getElement()->getPoint(i);
        _boundingBox.Enlarge((float)(pt._x), (float)(pt._y), (float)(pt._z));
    }
}

void TYPolygonGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    size_t nbPts = getElement()->getNbPts();

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
        // Calcul de la normale a la face
        OVector3D normal = getElement()->normal();
        normal.normalize();

        OPoint3D topLeftTexture;
        double xSize, ySize;
        if (_textureBg)
        {
            topLeftTexture._x = _bgImagePosition._x - _semiXBg;
            topLeftTexture._y = _bgImagePosition._y - _semiYBg;
            xSize = _semiXBg * 2;
            ySize = _semiYBg * 2;
        }

        if ((_altimetrie) && (nbPts == 3) && (!_texture))
        {
            glBegin(GL_TRIANGLES);//az++

            OPoint3D pt1 = getElement()->getPoint(0);
            OPoint3D pt2 = getElement()->getPoint(1);
            OPoint3D pt3 = getElement()->getPoint(2);

            OPoint3D pt = pt1;
            glNormal3f(normal._x, normal._y, normal._z);
            if (_textureBg)
            {
                double x = pt._x - topLeftTexture._x;
                double y = pt._y - topLeftTexture._y;
                if ((x >= 0) && (y >= 0))
                {
                    glTexCoord2f(x / xSize, y / ySize);
                }
                else
                {
                    glColor3f(_color0[0], _color0[1], _color0[2]);
                }
            }
            else
            {
                glColor3f(_color0[0], _color0[1], _color0[2]);
            }
            glVertex3f(pt._x, pt._y, pt._z);

            pt = pt2;
            if (_textureBg)
            {
                double x = pt._x - topLeftTexture._x;
                double y = pt._y - topLeftTexture._y;
                if ((x >= 0) && (y >= 0))
                {
                    glTexCoord2f(x / xSize, y / ySize);
                }
                else
                {
                    glColor3f(_color1[0], _color1[1], _color1[2]);
                }
            }
            else
            {
                glColor3f(_color1[0], _color1[1], _color1[2]);
            }
            glVertex3f(pt._x, pt._y, pt._z);

            pt = pt3;
            if (_textureBg)
            {
                double x = pt._x - topLeftTexture._x;
                double y = pt._y - topLeftTexture._y;
                if ((x >= 0) && (y >= 0))
                {
                    glTexCoord2f(x / xSize, y / ySize);
                }
                else
                {
                    glColor3f(_color2[0], _color2[1], _color2[2]);
                }
            }
            else
            {
                glColor3f(_color2[0], _color2[1], _color2[2]);
            }
            glVertex3f(pt._x, pt._y, pt._z);

            glEnd();

            if (TYElementGraphic::_gDrawNormals)
            {
                OPoint3D p1 = getElement()->getCenter();
                OVector3D n = getElement()->normal();
                n = n * 5;
                displayNormal(n, p1);
            }
        }
        else
        {
            double(*vertex)[5] = new double[nbPts][5];

            if (_gLUtesselator) { gluDeleteTess(_gLUtesselator); }
            _gLUtesselator = gluNewTess();
            gluTessCallback(_gLUtesselator, GLU_TESS_VERTEX, (void (STDCALL*)(void))vertexCallback);
            gluTessCallback(_gLUtesselator, GLU_TESS_BEGIN, (void (STDCALL*)(void))beginCallback);
            gluTessCallback(_gLUtesselator, GLU_TESS_END, (void (STDCALL*)(void))endCallback);
            gluTessCallback(_gLUtesselator, GLU_TESS_ERROR, (void (STDCALL*)(void))errorCallback);

            gluTessBeginPolygon(_gLUtesselator, NULL);
            gluTessNormal(_gLUtesselator, normal._x, normal._y, normal._z);

            gluTessBeginContour(_gLUtesselator);
            const TYTabPoint* pts = &(getElement()->getPoints());

            TYNameManager::get()->enable(false);

            // On recupere les coordonnees du rectangle englobant;
            TYRectangle rect = getElement()->getBoundingRect();

            TYNameManager::get()->enable(true);

            double minX, minY;
            minX = rect.getMinX();
            minY = rect.getMinY();

            // Echelle des textures (1 = une texture par m)
            int scale = 2;

            for (size_t i = 0; i < nbPts; i++)
            {
                // Coordonnees
                vertex[i][0] = (*pts)[i]._x;
                vertex[i][1] = (*pts)[i]._y;
                vertex[i][2] = (*pts)[i]._z;

                // Coordonnees de texture.
                vertex[i][3] = (vertex[i][0] - minX) / scale;
                vertex[i][4] = (vertex[i][1] - minY) / scale;

                gluTessVertex(_gLUtesselator, (GLdouble*)(*pts)[i]._value, (GLdouble*)vertex[i]);
            }
            gluTessEndContour(_gLUtesselator);
            gluTessEndPolygon(_gLUtesselator);

            if (TYElementGraphic::_gDrawNormals)
            {
                OPoint3D p1 = getElement()->getCenter();
                OVector3D n = getElement()->normal();
                n = n * 5;
                displayNormal(n, p1);
            }

            delete [] vertex;
        }

        //Calcul du volume englobant pour le fit:
        _globalBoundingBox.Enlarge(_boundingBox);
    }
}

void TYPolygonGraphic::setAltimetrieColor(double color0[3], double color1[3], double color2[3])
{
    _altimetrie = true;

    for (int i = 0; i < 3; i++)
    {
        _color0[i] = color0[i];
        _color1[i] = color1[i];
        _color2[i] = color2[i];
    }
}
