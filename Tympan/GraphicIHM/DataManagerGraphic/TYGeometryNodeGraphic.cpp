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
 * \file TYGeometryNodeGraphic.cpp
 * \brief Representation graphique d'un GeometryNode
 *
 *
 */



#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHGraphic.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include <GL/gl.h>
#include <GL/glu.h>

#if defined(WIN32)
#include <crtdbg.h>
#endif

#if defined(WIN32)
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif

TYGeometryNodeGraphic::TYGeometryNodeGraphic(TYGeometryNode* pElement) :
    TYElementGraphic(pElement)
{
}


TYGeometryNodeGraphic::~TYGeometryNodeGraphic()
{
}

void TYGeometryNodeGraphic::update(bool force /*=false*/)
{
    // On affecte le meme etat au GeoNode que l'element pour savoir s'il est actif dans le calcul courant
    getElement()->setInCurrentCalcul(getElement()->getElement()->isInCurrentCalcul());

    TYElementGraphic::update(force);
}

void TYGeometryNodeGraphic::pushMatrix()
{
    glPushMatrix();

    GLdouble m[4][4];
    OMatrix matrix = getElement()->getMatrix();
    for (int j = 0; j < 4; j++)
    {
        for (int k = 0; k < 4; k++)
        {
            m[j][k] = matrix._m[k][j];
        }
    }
    glMultMatrixd((GLdouble*)m);
}
void TYGeometryNodeGraphic::displayPushingParentMatrix(GLenum mode, TYGeometryNode* pDansCeRepere)
{
    TYListPtrGeoNode GeoNodeParents;
    TYGeometryNode* pThisGeoNode = ((TYGeometryNode*)getElement());
    pThisGeoNode->GetGeoNodeParentList(GeoNodeParents);
    TYListPtrGeoNode::iterator ite = GeoNodeParents.end();
    if (!GeoNodeParents.empty())
    {
        ite--;    //end n'est jamais valide
    }
    int i;
    int nbPushMatrix = 0;
    bool bPushMatrix = false;
    for (i = 0; i < GeoNodeParents.size() && (*ite) != pThisGeoNode; i++)
    {
        if (pDansCeRepere == NULL) //NULL = site pere (repere du TYSite le plus haut)
        {
            bPushMatrix = true;
        }
        if (bPushMatrix)
        {
            //PushMatrix
            TYGeometryNode* pCurrentTYGeometryNode = (TYGeometryNode*)(*ite);
            TYElementGraphic* pCurrentTYElementGraphic = (TYElementGraphic*)pCurrentTYGeometryNode->getGraphicObject();
            TYGeometryNodeGraphic* pCurrentTYGeometryNodeGraphic = (TYGeometryNodeGraphic*)pCurrentTYElementGraphic;
            pCurrentTYGeometryNodeGraphic->pushMatrix();
            nbPushMatrix++;
        }
        if ((*ite) == pDansCeRepere) //NULL = root
        {
            bPushMatrix = true;
        }

        if (ite != GeoNodeParents.begin())
        {
            ite--;
        }
    }
    display(mode);
    //PopMatrix
    for (i = 0; i < nbPushMatrix; i++)
    {
        glPopMatrix();
    }
}

void TYGeometryNodeGraphic::getChilds(TYListPtrTYElementGraphic& childs, bool recursif /*=true*/)
{
    TYElementGraphic* pTYElementGraphic = (TYElementGraphic*)getElement()->getElement()->getGraphicObject();
    childs.push_back(pTYElementGraphic);
    if (recursif)
    {
        pTYElementGraphic->getChilds(childs, recursif);
    }
}

void TYGeometryNodeGraphic::computeBoundingBox()
{
    OBox reset;
    _boundingBox = reset;
    ((TYElementGraphic*)getElement()->getElement()->getGraphicObject())->computeBoundingBox();
    _boundingBox.Enlarge(((TYElementGraphic*)getElement()->getElement()->getGraphicObject())->GetBox());
}


void TYGeometryNodeGraphic::display(GLenum mode)//= GL_RENDER
{
    pushMatrix();

    LPTYElementGraphic pGraphicObject = getElement()->getElement()->getGraphicObject();

    pGraphicObject->display(mode);

    glPopMatrix();
}
