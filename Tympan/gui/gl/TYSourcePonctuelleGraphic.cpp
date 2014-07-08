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
 * \file TYSourcePonctuelleGraphic.cpp
 * \brief Representation graphique d'une source ponctuelle
 *
 *
 */



#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHGraphic.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "TYSourcePonctuelleGraphic.h"

#include "Tympan/models/business/acoustique/TYSourcePonctuelle.h"
#include "Tympan/gui/gl/TYPickingTable.h"


///Taille de la source par defaut.
static const float SourceGraphicDefaultSize = 0.1f;
///Taille de la source de type utilisateur.
static const float SourceGraphicTypeUserSize = 2.0f;


TYSourcePonctuelleGraphic::TYSourcePonctuelleGraphic(TYSourcePonctuelle* pElement) :
    TYElementGraphic(pElement)
{
    setSize();
}

TYSourcePonctuelleGraphic::~TYSourcePonctuelleGraphic()
{
}

void TYSourcePonctuelleGraphic::update(bool force /*=false*/)
{
    TYElementGraphic::update(force);
}

void TYSourcePonctuelleGraphic::setSize()
{
    float defaultSize = SourceGraphicDefaultSize;

#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "SrcPonctDefaultGraphicSize"))
    {
        defaultSize = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, "SrcPonctDefaultGraphicSize");
    }
    else
    {
        TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, "SrcPonctDefaultGraphicSize", defaultSize);
    }
#endif

    float UserSrcSize = SourceGraphicTypeUserSize;

#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "UserSrcPonctGraphicSize"))
    {
        UserSrcSize = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, "UserSrcPonctGraphicSize");
    }
    else
    {
        TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, "UserSrcPonctGraphicSize", UserSrcSize);
    }
#endif

    if (((TYSourcePonctuelle*)_pElement)->getType() == TYSourcePonctuelle::TypeUser)
    {
        _size = UserSrcSize;
    }
    else
    {
        _size = defaultSize;
    }
}

void TYSourcePonctuelleGraphic::computeBoundingBox()
{
    OBox reset;
    _boundingBox = reset;

    LPTYPoint pPt = getElement()->getPos();
    _boundingBox.Enlarge((float)(pPt->_x - _size / 2), (float)(pPt->_y - _size / 2), (float)(pPt->_z - _size / 2));
    _boundingBox.Enlarge((float)(pPt->_x + _size / 2), (float)(pPt->_y + _size / 2), (float)(pPt->_z + _size / 2));
    if (getElement()->getType() == TYSourcePonctuelle::TypeUser)
    {
        // Vecteur Orientation
        getElement()->getOrientation().normalize();
        OVector3D vec = OVector3D(*pPt) + (getElement()->getOrientation());
        _boundingBox.Enlarge((float)(pPt->_x), (float)(pPt->_y), (float)(pPt->_z));//inutile...
        _boundingBox.Enlarge((float)(vec._x), (float)(vec._y), (float)(vec._z));
    }
}

void TYSourcePonctuelleGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    if (!getElement()->isInCurrentCalcul())
    {
        return;
    }

    setSize();

    LPTYPoint pPt = getElement()->getPos();

    // Couleur de l'objet.
    OColor color = getElement()->getColor();

    int object = getElement()->getObject();

    if (_highlight)
    {
        OColor tmpColor;
        glGetFloatv(GL_CURRENT_COLOR, tmpColor);
        if (_bFirstDisp)
        {
            computeBoundingBox();
            _bFirstDisp = false;
        }
        drawLineBoundingBox();
        if (mode == GL_COMPILE) { drawName(); }
        // Modification de la couleur en GL_CURRENT_COLOR si _highlight.
        glColor3fv(tmpColor);
    }

    if (_visible)
    {
        if (mode == GL_SELECT)
        {
            TYPickingTable::addElement(getElement());
            glPushName((GLuint)(TYPickingTable::getIndex()));
        }

        // Fixe la couleur a l'objet.
        glColor3fv(color);

        switch (object)
        {
            case TYSourcePonctuelle::objectCube:
                drawCube(pPt);
                break;
            case TYSourcePonctuelle::objectPyramid:
                drawPyramid(pPt);
                break;
            case TYSourcePonctuelle::objectSphere:
                drawSphere(pPt);
                break;
            case TYSourcePonctuelle::objectStar:
                drawStar(pPt);
                break;
            default:
                break;
        }

        if (getElement()->getType() == TYSourcePonctuelle::TypeUser)
        {
            // Vecteur Orientation
            getElement()->getOrientation().normalize();
            OVector3D vec = OVector3D(*pPt) + (getElement()->getOrientation());

            glBegin(GL_LINES);
            glColor3fv(color);
            glVertex3f(pPt->_x, pPt->_y, pPt->_z);
            glVertex3f((vec._x + _size / 2), (vec._y + _size / 2), (vec._z + _size / 2));
            glEnd();
        }

        if (mode == GL_SELECT)
        {
            glPopName();
        }
        //Calcul du volume englobant pour le fit:
        _globalBoundingBox.Enlarge(_boundingBox);
    }
}

void TYSourcePonctuelleGraphic::drawCube(TYPoint* pPt) const
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

void TYSourcePonctuelleGraphic::drawPyramid(TYPoint* pPt) const
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

void TYSourcePonctuelleGraphic::drawSphere(TYPoint* pPt) const
{
    GLUquadricObj* qobj = gluNewQuadric();
    glTranslatef(pPt->_x, pPt->_y, pPt->_z);
    gluSphere(qobj, _size / 2.0f, 20, 20);
    glTranslatef(-pPt->_x, -pPt->_y, -pPt->_z);
    gluDeleteQuadric(qobj);
}

void TYSourcePonctuelleGraphic::drawStar(TYPoint* pPt) const
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
