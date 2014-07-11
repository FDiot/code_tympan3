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
 * \file TYElementGraphic.cpp
 * \brief Representation graphique d'un element de base
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHGraphic.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/models/business/TYElement.h"
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/GraphicIHM/ToolsGraphic/OGLTextElement.h"
#include "Tympan/GraphicIHM/ToolsGraphic/OGLCamera.h"
#include "Tympan/GraphicIHM/ToolsGraphic/OGLLineElement.h"
#include "Tympan/GraphicIHM/ToolsGraphic/NxVec3.h"
#include "Tympan/models/business/geometry/TYGeometryNode.h"

#define IMG(id) OLocalizator::getPicture("TYElementGraphic", (id))

OBox TYElementGraphic::_globalBoundingBox;

bool TYElementGraphic::_gDrawNormals = false;
bool TYElementGraphic::_gDrawPlafond = true;


TYElementGraphic::TYElementGraphic(TYElement* pElt)
{
    _pElement = pElt;

    _modified = true;
    _visible = true;
    _highlight = false;
    _texture = false;
    _bFirstDisp = true;

    // Highlight
    _viewOnlyHighlight = false;
}

TYElementGraphic::~TYElementGraphic()
{
}

void TYElementGraphic::setElement(TYElement* pElt)
{
    _pElement = pElt;
}

void TYElementGraphic::updateTree()
{
    TYElement* pParent = _pElement->getParent();

    if (getModified() && pParent && pParent->getGraphicObject())
    {
        // On force la mise a jour du parent
        pParent->setIsGeometryModified(true);
        // On remonte l'arbre
        pParent->getGraphicObject()->updateTree();
    }
    else
    {
        // On appelle la methode update surchargee par les objets graphiques
        update();
    }
}

void TYElementGraphic::update(bool force /*=false*/)
{
    computeBoundingBox();

    if (_pElement)
    {
        if (_pElement->isInCurrentCalcul())
        {
            _visible = !_viewOnlyHighlight;
        }
        else
        {
            _visible = false;
        }
    }

    // Update highlight seulement si actif
    highlight(getHighlightState());

    // On repositionne le flag modified
    setModified(false);

#if TY_USE_IHM
    // Emission du signal de notification de mise a jour
    activateSignal();
#endif // TY_USE_IHM
}

bool TYElementGraphic::getModified(bool andReset /*=false*/)
{
    bool ret = _modified;

    if (andReset)
    {
        setModified(false);
    }

    return ret;
}

void TYElementGraphic::highlight(bool state /*=true*/)
{
    // Update
    _highlight = state;
}

void TYElementGraphic::displayNormal(OVector3D normal, OPoint3D p1)
{
    OVector3D n = normal; //getElement()->_normale;
    OPoint3D p2;
    p2._x = p1._x + n._x;
    p2._y = p1._y + n._y;
    p2._z = p1._z + n._z;
    glColor3f(1.0f, 0.5f, 0.0f);
    glLineWidth(2);
    glPointSize(4);

    glBegin(GL_LINE_STRIP);
    glVertex3f(p1._x, p1._y, p1._z);
    glVertex3f(p2._x, p2._y, p2._z);
    glEnd();
}

void TYElementGraphic::drawLineBoundingBox()
{
    float org[3];
    org[0] = (_boundingBox._min._x + _boundingBox._max._x) / 2;
    org[1] = (_boundingBox._min._y + _boundingBox._max._y) / 2;
    org[2] = (_boundingBox._min._z + _boundingBox._max._z) / 2;

    //Ajout de 0.1 contre bug faces coplanaires
    // CLM-NT33 : agrandit la bb avec un % de la taille
    //double x = 5 + (_boundingBox._max._x - _boundingBox._min._x)/2;
    //double y = 5 + (_boundingBox._max._y - _boundingBox._min._y)/2;
    //double z = 5 + (_boundingBox._max._z - _boundingBox._min._z)/2;
    //OPoint3D d = _boundingBox._max - _boundingBox._min;
    double x = (_boundingBox._max._x - _boundingBox._min._x) * 1.1f / 2;
    double y = (_boundingBox._max._y - _boundingBox._min._y) * 1.1f / 2;
    double z = (_boundingBox._max._z - _boundingBox._min._z) * 1.1f / 2;

    glColor3f(1.0f, 1.0f, 0.5f);
    glLineWidth(2);
    glPointSize(4);

    glBegin(GL_LINE_LOOP);
    glVertex3f(org[0] + x, org[1] - y, org[2] - z);
    glVertex3f(org[0] - x, org[1] - y, org[2] - z);
    glVertex3f(org[0] - x, org[1] + y, org[2] - z);
    glVertex3f(org[0] + x, org[1] + y, org[2] - z);
    glEnd();
    glBegin(GL_LINE_LOOP);
    glVertex3f(org[0] + x, org[1] - y, org[2] + z);
    glVertex3f(org[0] - x, org[1] - y, org[2] + z);
    glVertex3f(org[0] - x, org[1] + y, org[2] + z);
    glVertex3f(org[0] + x, org[1] + y, org[2] + z);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex3f(org[0] + x, org[1] - y, org[2] - z);
    glVertex3f(org[0] + x, org[1] - y, org[2] + z);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex3f(org[0] - x, org[1] - y, org[2] - z);
    glVertex3f(org[0] - x, org[1] - y, org[2] + z);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex3f(org[0] - x, org[1] + y, org[2] - z);
    glVertex3f(org[0] - x, org[1] + y, org[2] + z);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex3f(org[0] + x, org[1] + y, org[2] - z);
    glVertex3f(org[0] + x, org[1] + y, org[2] + z);
    glEnd();
    glBegin(GL_POINTS);
    glVertex3f(org[0] + x, org[1] - y, org[2] - z);
    glVertex3f(org[0] - x, org[1] - y, org[2] - z);
    glVertex3f(org[0] - x, org[1] + y, org[2] - z);
    glVertex3f(org[0] + x, org[1] + y, org[2] - z);
    glVertex3f(org[0] + x, org[1] - y, org[2] + z);
    glVertex3f(org[0] - x, org[1] - y, org[2] + z);
    glVertex3f(org[0] - x, org[1] + y, org[2] + z);
    glVertex3f(org[0] + x, org[1] + y, org[2] + z);
    glEnd();
}

void TYElementGraphic::drawName()
{
    TYGeometryNode* pThisGeoNode = TYGeometryNode::GetGeoNode(_pElement);

    if (pThisGeoNode)
    {
        //pour determiner la position du texte, il faut le centre de la boundingbox dans le repere global
        float org[3];
        org[0] = (_boundingBox._min._x + _boundingBox._max._x) / 2;
        org[1] = (_boundingBox._min._y + _boundingBox._max._y) / 2;
        org[2] = (_boundingBox._min._z + _boundingBox._max._z) / 2;

        OMatrix mat = pThisGeoNode->localToGlobal();
        NxVec3 origin(mat._m[0][3] + org[0], mat._m[2][3] + org[2], -mat._m[1][3] - org[1]);
        NxVec3 ret = OGLCamera::worldToDisplay(origin);

        //ajouter une etiquette avec le nom
        OGLTextElement oOGLTextElement;
        oOGLTextElement.setFont(IMG("id_font"));
        oOGLTextElement.setVisibility(true);
        if (_pElement)
        {
            oOGLTextElement.setTextToDisplay(_pElement->getName());
        }
        oOGLTextElement.setColor(OColor::BLACK);
        oOGLTextElement.setDisplayPosition(ret.x + 22, ret.y + 40);
        oOGLTextElement.render();

        OGLLineElement oglLineElement;
        oglLineElement.setColor(OColor::BLACK);
        oglLineElement.setLineWidth(2.0f);
        oglLineElement.setPoint1(OPoint3D(ret.x, ret.y, 0));
        oglLineElement.setPoint2(OPoint3D(ret.x + 20, ret.y + 40, 0));
        oglLineElement.setVisibility(true);
        oglLineElement.render();
    }
}

void TYElementGraphic::drawBoundingBox()
{
    float org[3];
    org[0] = (_boundingBox._min._x + _boundingBox._max._x) / 2;
    org[1] = (_boundingBox._min._y + _boundingBox._max._y) / 2;
    org[2] = (_boundingBox._min._z + _boundingBox._max._z) / 2;

    //Ajout de 0.1 contre bug faces coplanaires
    double x = 0.1 + (_boundingBox._max._x - _boundingBox._min._x) / 2;
    double y = 0.1 + (_boundingBox._max._y - _boundingBox._min._y) / 2;
    double z = 0.1 + (_boundingBox._max._z - _boundingBox._min._z) / 2;

    glColor4f(1.0f, 1.0f, 0.0f, 0.1f);

    glBegin(GL_QUADS);

    glVertex3d(org[0] + x, org[1] + y, org[2] - z);
    glVertex3d(org[0] - x, org[1] + y, org[2] - z);
    glVertex3d(org[0] - x, org[1] + y, org[2] + z);
    glVertex3d(org[0] + x, org[1] + y, org[2] + z);

    glVertex3f(org[0] + x, org[1] - y, org[2] + z);
    glVertex3f(org[0] - x, org[1] - y, org[2] + z);
    glVertex3f(org[0] - x, org[1] - y, org[2] - z);
    glVertex3f(org[0] + x, org[1] - y, org[2] - z);

    glVertex3f(org[0] + x, org[1] + y, org[2] + z);
    glVertex3f(org[0] - x, org[1] + y, org[2] + z);
    glVertex3f(org[0] - x, org[1] - y, org[2] + z);
    glVertex3f(org[0] + x, org[1] - y, org[2] + z);

    glVertex3f(org[0] + x, org[1] - y, org[2] - z);
    glVertex3f(org[0] - x, org[1] - y, org[2] - z);
    glVertex3f(org[0] - x, org[1] + y, org[2] - z);
    glVertex3f(org[0] + x, org[1] + y, org[2] - z);

    glVertex3f(org[0] - x, org[1] + y, org[2] + z);
    glVertex3f(org[0] - x, org[1] + y, org[2] - z);
    glVertex3f(org[0] - x, org[1] - y, org[2] - z);
    glVertex3f(org[0] - x, org[1] - y, org[2] + z);

    glVertex3f(org[0] + x, org[1] + y, org[2] - z);
    glVertex3f(org[0] + x, org[1] + y, org[2] + z);
    glVertex3f(org[0] + x, org[1] - y, org[2] + z);
    glVertex3f(org[0] + x, org[1] - y, org[2] - z);

    glEnd();
}
void TYElementGraphic::glVertex3fAndEnlargeBB(GLfloat x, GLfloat y, GLfloat z)
{
    glVertex3f(x, y, z);
    _boundingBox.Enlarge((float)x, (float)y, (float)z);
}

void TYElementGraphic::glVertex3dAndEnlargeBB(GLdouble x, GLdouble y, GLdouble z)
{
    glVertex3d(x, y, z);
    _boundingBox.Enlarge((float)x, (float)y, (float)z);
}

void TYElementGraphic::gluDiskAndEnlargeBB(
    GLUquadric*          qobj,
    GLdouble            innerRadius,
    GLdouble            outerRadius,
    GLint               slices,
    GLint               loops)
{
    gluDisk(qobj, innerRadius, outerRadius, slices, loops);
}

void TYElementGraphic::gluCylinderAndEnlargeBB(
    GLUquadric*          qobj,
    GLdouble            baseRadius,
    GLdouble            topRadius,
    GLdouble            height,
    GLint               slices,
    GLint               stacks)
{
    gluCylinder(qobj, baseRadius, topRadius, height, slices, stacks);
}

void TYElementGraphic::gluPartialDiskAndEnlargeBB(
    GLUquadric*          qobj,
    GLdouble            innerRadius,
    GLdouble            outerRadius,
    GLint               slices,
    GLint               loops,
    GLdouble            startAngle,
    GLdouble            sweepAngle)
{
    gluPartialDisk(qobj, innerRadius, outerRadius, slices, loops, startAngle, sweepAngle);
}


bool TYElementGraphic::getHighlightState()
{
    return _highlight;
}

void TYElementGraphic::viewOnlyHighlight(bool state /*=true*/)
{
    _viewOnlyHighlight = state;

    // Active le highlight
    highlight(_viewOnlyHighlight);
}

void TYElementGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    OBox reset;
    _boundingBox = reset;
#if TY_USE_IHM
    // Emission du signal de notification de mise a jour
    activateSignal();

#endif // TY_USE_IHM
}

void TYElementGraphic::setRecursiveVisible(bool visible /*=true*/)
{
    LPTYElementArray childs;

    _pElement->getChilds(childs);

    for (int i = 0; i < childs.size(); i++)
    {
        childs[i]->getGraphicObject()->setVisible(visible);
    }
}


void TYElementGraphic::computeBoundingBox()
{
    OBox reset;
    _boundingBox = reset;

    TYListPtrTYElementGraphic childs;
    getChilds(childs, false);
    TYListPtrTYElementGraphic::iterator ite;
    OBox boundingBox;
    TYElementGraphic* pChild = NULL;
    for (ite = childs.begin(); ite != childs.end(); ite++)
    {
        pChild = (*ite);
        if (pChild != NULL)
        {
            pChild->computeBoundingBox();
            boundingBox = pChild->GetBox();
        }
        _boundingBox.Enlarge(boundingBox);
    }
}
