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
 * \file TYPointControlGraphic.cpp
 * \brief Representation graphique d'un point de controle
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHGraphic.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYPointControl.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYProjet.h"


///Taille du point de controle.
static const float PointControlGraphicDefaultSize = 2.0;


TYPointControlGraphic::TYPointControlGraphic(TYPointControl* pElement) :
    TYElementGraphic(pElement)
{
    setSize();
}

TYPointControlGraphic::~TYPointControlGraphic()
{
}

void TYPointControlGraphic::setSize()
{
    _size = PointControlGraphicDefaultSize;

#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "PointControlGraphicSize"))
    {
        _size = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, "PointControlGraphicSize");
    }
    else
    {
        TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, "PointControlGraphicSize", _size);
    }
#endif
}


void TYPointControlGraphic::update(bool force /*=false*/)
{
    TYElementGraphic::update(force);
}

void TYPointControlGraphic::computeBoundingBox()
{
    OBox reset;
    _boundingBox = reset;
    LPTYPointControl pPt = getElement();
    if (pPt == NULL) { return; }

    _boundingBox.Enlarge((float)(pPt->_x - _size / 2), (float)(pPt->_y - _size / 2), (float)(pPt->_z - _size / 2));
    _boundingBox.Enlarge((float)(pPt->_x + _size / 2), (float)(pPt->_y + _size / 2), (float)(pPt->_z + _size / 2));
}

void TYPointControlGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    TYCalcul* pCalcul = static_cast<TYProjet*>(getElement()->getParent())->getCurrentCalcul();
    // Pas affiche si inactif
    if (getElement()->getEtat(pCalcul) == false)
    {
        return;
    }

    setSize();

    LPTYPointControl pPt = getElement();

    if (pPt == NULL) { return; }

    OColor color = getElement()->getColor();

    int object = getElement()->getObject();

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
        if (mode == GL_COMPILE) { drawName(); }
        glColor3f(tmpColor[0], tmpColor[1], tmpColor[2]);
    }

    if (_visible)
    {
        if (mode == GL_SELECT)
        {
            TYPickingTable::addElement(getElement());
            glPushName((GLuint)(TYPickingTable::getIndex()));
        }

        glColor4fv(color);

        switch (object)
        {
            case TYPointControl::objectCube:
                drawCube(pPt);
                break;
            case TYPointControl::objectPyramid:
                drawPyramid(pPt);
                break;
            case TYPointControl::objectSphere:
                drawSphere(pPt);
                break;
            case TYPointControl::objectStar:
                drawStar(pPt);
                break;
            default:
                break;
        }

        if (mode == GL_SELECT)
        {
            glPopName();
        }
        //Calcul du volume englobant pour le fit:
        _globalBoundingBox.Enlarge(_boundingBox);
    }
}


void TYPointControlGraphic::drawCube(TYPointControl* pPt) const
{
    glBegin(GL_QUADS);

    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(pPt->_x + _size / 2, pPt->_y + _size / 2, pPt->_z - _size / 2);
    glVertex3f(pPt->_x - _size / 2, pPt->_y + _size / 2, pPt->_z - _size / 2);
    glVertex3f(pPt->_x - _size / 2, pPt->_y + _size / 2, pPt->_z + _size / 2);
    glVertex3f(pPt->_x + _size / 2, pPt->_y + _size / 2, pPt->_z + _size / 2);

    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(pPt->_x + _size / 2, pPt->_y - _size / 2, pPt->_z + _size / 2);
    glVertex3f(pPt->_x - _size / 2, pPt->_y - _size / 2, pPt->_z + _size / 2);
    glVertex3f(pPt->_x - _size / 2, pPt->_y - _size / 2, pPt->_z - _size / 2);
    glVertex3f(pPt->_x + _size / 2, pPt->_y - _size / 2, pPt->_z - _size / 2);

    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(pPt->_x + _size / 2, pPt->_y + _size / 2, pPt->_z + _size / 2);
    glVertex3f(pPt->_x - _size / 2, pPt->_y + _size / 2, pPt->_z + _size / 2);
    glVertex3f(pPt->_x - _size / 2, pPt->_y - _size / 2, pPt->_z + _size / 2);
    glVertex3f(pPt->_x + _size / 2, pPt->_y - _size / 2, pPt->_z + _size / 2);

    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(pPt->_x + _size / 2, pPt->_y - _size / 2, pPt->_z - _size / 2);
    glVertex3f(pPt->_x - _size / 2, pPt->_y - _size / 2, pPt->_z - _size / 2);
    glVertex3f(pPt->_x - _size / 2, pPt->_y + _size / 2, pPt->_z - _size / 2);
    glVertex3f(pPt->_x + _size / 2, pPt->_y + _size / 2, pPt->_z - _size / 2);

    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(pPt->_x - _size / 2, pPt->_y + _size / 2, pPt->_z + _size / 2);
    glVertex3f(pPt->_x - _size / 2, pPt->_y + _size / 2, pPt->_z - _size / 2);
    glVertex3f(pPt->_x - _size / 2, pPt->_y - _size / 2, pPt->_z - _size / 2);
    glVertex3f(pPt->_x - _size / 2, pPt->_y - _size / 2, pPt->_z + _size / 2);

    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(pPt->_x + _size / 2, pPt->_y + _size / 2, pPt->_z - _size / 2);
    glVertex3f(pPt->_x + _size / 2, pPt->_y + _size / 2, pPt->_z + _size / 2);
    glVertex3f(pPt->_x + _size / 2, pPt->_y - _size / 2, pPt->_z + _size / 2);
    glVertex3f(pPt->_x + _size / 2, pPt->_y - _size / 2, pPt->_z - _size / 2);

    glEnd();
}

void TYPointControlGraphic::drawPyramid(TYPointControl* pPt) const
{
    glBegin(GL_TRIANGLES);

    glNormal3f(0.0f, -1.0f, 0.5f);
    glVertex3f(pPt->_x          , pPt->_y          , pPt->_z + _size / 2);
    glVertex3f(pPt->_x - _size / 2, pPt->_y - _size / 2, pPt->_z - _size / 2);
    glVertex3f(pPt->_x + _size / 2, pPt->_y - _size / 2, pPt->_z - _size / 2);

    glNormal3f(1.0f, 0.0f, 0.5f);
    glVertex3f(pPt->_x          , pPt->_y          , pPt->_z + _size / 2);
    glVertex3f(pPt->_x + _size / 2, pPt->_y - _size / 2, pPt->_z - _size / 2);
    glVertex3f(pPt->_x + _size / 2, pPt->_y + _size / 2, pPt->_z - _size / 2);

    glNormal3f(0.0f, 1.0f, 0.5f);
    glVertex3f(pPt->_x          , pPt->_y          , pPt->_z + _size / 2);
    glVertex3f(pPt->_x + _size / 2, pPt->_y + _size / 2, pPt->_z - _size / 2);
    glVertex3f(pPt->_x - _size / 2, pPt->_y + _size / 2, pPt->_z - _size / 2);

    glNormal3f(-1.0f, 0.0f, 0.5f);
    glVertex3f(pPt->_x          , pPt->_y          , pPt->_z + _size / 2);
    glVertex3f(pPt->_x - _size / 2, pPt->_y + _size / 2, pPt->_z - _size / 2);
    glVertex3f(pPt->_x - _size / 2, pPt->_y - _size / 2, pPt->_z - _size / 2);

    glEnd();

    glBegin(GL_QUADS);

    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(pPt->_x + _size / 2, pPt->_y - _size / 2, pPt->_z - _size / 2);
    glVertex3f(pPt->_x - _size / 2, pPt->_y - _size / 2, pPt->_z - _size / 2);
    glVertex3f(pPt->_x - _size / 2, pPt->_y + _size / 2, pPt->_z - _size / 2);
    glVertex3f(pPt->_x + _size / 2, pPt->_y + _size / 2, pPt->_z - _size / 2);

    glEnd();
}

void TYPointControlGraphic::drawSphere(TYPointControl* pPt) const
{
    GLUquadricObj* qobj = gluNewQuadric();
    glTranslatef(pPt->_x, pPt->_y, pPt->_z);
    gluSphere(qobj, _size / 2.0f, 20, 20);
    glTranslatef(-pPt->_x, -pPt->_y, -pPt->_z);
    gluDeleteQuadric(qobj);
}

void TYPointControlGraphic::drawStar(TYPointControl* pPt) const
{
    glBegin(GL_TRIANGLES);

    // Up pyramid
    glNormal3f(0.0f, -1.0f, 0.5f);
    glVertex3f(pPt->_x          , pPt->_y          , pPt->_z + _size / 2);
    glVertex3f(pPt->_x - _size / 6, pPt->_y - _size / 6, pPt->_z + _size / 6);
    glVertex3f(pPt->_x + _size / 6, pPt->_y - _size / 6, pPt->_z + _size / 6);

    glNormal3f(1.0f, 0.0f, 0.5f);
    glVertex3f(pPt->_x          , pPt->_y          , pPt->_z + _size / 2);
    glVertex3f(pPt->_x + _size / 6, pPt->_y - _size / 6, pPt->_z + _size / 6);
    glVertex3f(pPt->_x + _size / 6, pPt->_y + _size / 6, pPt->_z + _size / 6);

    glNormal3f(0.0f, 1.0f, 0.5f);
    glVertex3f(pPt->_x          , pPt->_y          , pPt->_z + _size / 2);
    glVertex3f(pPt->_x + _size / 6, pPt->_y + _size / 6, pPt->_z + _size / 6);
    glVertex3f(pPt->_x - _size / 6, pPt->_y + _size / 6, pPt->_z + _size / 6);

    glNormal3f(-1.0f, 0.0f, 0.5f);
    glVertex3f(pPt->_x          , pPt->_y          , pPt->_z + _size / 2);
    glVertex3f(pPt->_x - _size / 6, pPt->_y + _size / 6, pPt->_z + _size / 6);
    glVertex3f(pPt->_x - _size / 6, pPt->_y - _size / 6, pPt->_z + _size / 6);

    // Right pyramid
    glNormal3f(0.5f, -1.0f, 0.0f);
    glVertex3f(pPt->_x + _size / 2, pPt->_y          , pPt->_z);
    glVertex3f(pPt->_x + _size / 6, pPt->_y - _size / 6, pPt->_z + _size / 6);
    glVertex3f(pPt->_x + _size / 6, pPt->_y - _size / 6, pPt->_z - _size / 6);

    glNormal3f(0.5f, 0.0f, -1.0f);
    glVertex3f(pPt->_x + _size / 2, pPt->_y          , pPt->_z);
    glVertex3f(pPt->_x + _size / 6, pPt->_y - _size / 6, pPt->_z - _size / 6);
    glVertex3f(pPt->_x + _size / 6, pPt->_y + _size / 6, pPt->_z - _size / 6);

    glNormal3f(0.5f, 1.0f, 0.0f);
    glVertex3f(pPt->_x + _size / 2, pPt->_y          , pPt->_z);
    glVertex3f(pPt->_x + _size / 6, pPt->_y + _size / 6, pPt->_z - _size / 6);
    glVertex3f(pPt->_x + _size / 6, pPt->_y + _size / 6, pPt->_z + _size / 6);

    glNormal3f(0.5f, 0.0f, 1.0f);
    glVertex3f(pPt->_x + _size / 2, pPt->_y          , pPt->_z);
    glVertex3f(pPt->_x + _size / 6, pPt->_y + _size / 6, pPt->_z + _size / 6);
    glVertex3f(pPt->_x + _size / 6, pPt->_y - _size / 6, pPt->_z + _size / 6);

    // Down pyramid
    glNormal3f(0.0f, 1.0f, 0.5f);
    glVertex3f(pPt->_x          , pPt->_y          , pPt->_z - _size / 2);
    glVertex3f(pPt->_x - _size / 6, pPt->_y - _size / 6, pPt->_z - _size / 6);
    glVertex3f(pPt->_x + _size / 6, pPt->_y - _size / 6, pPt->_z - _size / 6);

    glNormal3f(-1.0f, 0.0f, 0.5f);
    glVertex3f(pPt->_x          , pPt->_y          , pPt->_z - _size / 2);
    glVertex3f(pPt->_x + _size / 6, pPt->_y - _size / 6, pPt->_z - _size / 6);
    glVertex3f(pPt->_x + _size / 6, pPt->_y + _size / 6, pPt->_z - _size / 6);

    glNormal3f(0.0f, -1.0f, 0.5f);
    glVertex3f(pPt->_x          , pPt->_y          , pPt->_z - _size / 2);
    glVertex3f(pPt->_x + _size / 6, pPt->_y + _size / 6, pPt->_z - _size / 6);
    glVertex3f(pPt->_x - _size / 6, pPt->_y + _size / 6, pPt->_z - _size / 6);

    glNormal3f(1.0f, 0.0f, 0.5f);
    glVertex3f(pPt->_x          , pPt->_y          , pPt->_z - _size / 2);
    glVertex3f(pPt->_x - _size / 6, pPt->_y + _size / 6, pPt->_z - _size / 6);
    glVertex3f(pPt->_x - _size / 6, pPt->_y - _size / 6, pPt->_z - _size / 6);

    // Left pyramid
    glNormal3f(0.5f, 1.0f, 0.0f);
    glVertex3f(pPt->_x - _size / 2, pPt->_y          , pPt->_z);
    glVertex3f(pPt->_x - _size / 6, pPt->_y - _size / 6, pPt->_z + _size / 6);
    glVertex3f(pPt->_x - _size / 6, pPt->_y - _size / 6, pPt->_z - _size / 6);

    glNormal3f(0.5f, 0.0f, 1.0f);
    glVertex3f(pPt->_x - _size / 2, pPt->_y          , pPt->_z);
    glVertex3f(pPt->_x - _size / 6, pPt->_y - _size / 6, pPt->_z - _size / 6);
    glVertex3f(pPt->_x - _size / 6, pPt->_y + _size / 6, pPt->_z - _size / 6);

    glNormal3f(0.5f, -1.0f, 0.0f);
    glVertex3f(pPt->_x - _size / 2, pPt->_y          , pPt->_z);
    glVertex3f(pPt->_x - _size / 6, pPt->_y + _size / 6, pPt->_z - _size / 6);
    glVertex3f(pPt->_x - _size / 6, pPt->_y + _size / 6, pPt->_z + _size / 6);

    glNormal3f(0.5f, 0.0f, -1.0f);
    glVertex3f(pPt->_x - _size / 2, pPt->_y          , pPt->_z);
    glVertex3f(pPt->_x - _size / 6, pPt->_y + _size / 6, pPt->_z + _size / 6);
    glVertex3f(pPt->_x - _size / 6, pPt->_y - _size / 6, pPt->_z + _size / 6);


    // Back pyramid
    glNormal3f(0.0f, 0.5f, 1.0f);
    glVertex3f(pPt->_x          , pPt->_y + _size / 2, pPt->_z);
    glVertex3f(pPt->_x - _size / 6, pPt->_y + _size / 6, pPt->_z + _size / 6);
    glVertex3f(pPt->_x + _size / 6, pPt->_y + _size / 6, pPt->_z + _size / 6);

    glNormal3f(1.0f, 0.5f, 0.0f);
    glVertex3f(pPt->_x          , pPt->_y + _size / 2, pPt->_z);
    glVertex3f(pPt->_x + _size / 6, pPt->_y + _size / 6, pPt->_z + _size / 6);
    glVertex3f(pPt->_x + _size / 6, pPt->_y + _size / 6, pPt->_z - _size / 6);

    glNormal3f(0.0f, 0.5f, -1.0f);
    glVertex3f(pPt->_x          , pPt->_y + _size / 2, pPt->_z);
    glVertex3f(pPt->_x + _size / 6, pPt->_y + _size / 6, pPt->_z - _size / 6);
    glVertex3f(pPt->_x - _size / 6, pPt->_y + _size / 6, pPt->_z - _size / 6);

    glNormal3f(-1.0f, 0.5f, 0.0f);
    glVertex3f(pPt->_x          , pPt->_y + _size / 2, pPt->_z);
    glVertex3f(pPt->_x - _size / 6, pPt->_y + _size / 6, pPt->_z - _size / 6);
    glVertex3f(pPt->_x - _size / 6, pPt->_y + _size / 6, pPt->_z + _size / 6);

    // Front pyramid
    glNormal3f(0.0f, 0.5f, -1.0f);
    glVertex3f(pPt->_x          , pPt->_y - _size / 2, pPt->_z);
    glVertex3f(pPt->_x - _size / 6, pPt->_y - _size / 6, pPt->_z + _size / 6);
    glVertex3f(pPt->_x + _size / 6, pPt->_y - _size / 6, pPt->_z + _size / 6);

    glNormal3f(-1.0f, 0.5f, 0.0f);
    glVertex3f(pPt->_x          , pPt->_y - _size / 2, pPt->_z);
    glVertex3f(pPt->_x + _size / 6, pPt->_y - _size / 6, pPt->_z + _size / 6);
    glVertex3f(pPt->_x + _size / 6, pPt->_y - _size / 6, pPt->_z - _size / 6);

    glNormal3f(0.0f, 0.5f, 1.0f);
    glVertex3f(pPt->_x          , pPt->_y - _size / 2, pPt->_z);
    glVertex3f(pPt->_x + _size / 6, pPt->_y - _size / 6, pPt->_z - _size / 6);
    glVertex3f(pPt->_x - _size / 6, pPt->_y - _size / 6, pPt->_z - _size / 6);

    glNormal3f(1.0f, 0.5f, 0.0f);
    glVertex3f(pPt->_x          , pPt->_y - _size / 2, pPt->_z);
    glVertex3f(pPt->_x - _size / 6, pPt->_y - _size / 6, pPt->_z - _size / 6);
    glVertex3f(pPt->_x - _size / 6, pPt->_y - _size / 6, pPt->_z + _size / 6);

    glEnd();
}
