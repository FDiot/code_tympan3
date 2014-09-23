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
 * \file TYRectangularMaillageGraphic.cpp
 * \brief Representation graphique d'un maillage rectangulaire
 */


#include "Tympan/core/color.h"
#include "Tympan/models/common/3d.h"
#include "Tympan/models/business/TYPreferenceManager.h"
#include "Tympan/models/business/TYRectangularMaillage.h"
#include "Tympan/models/business/TYProjet.h"
#include "Tympan/models/business/TYCalcul.h"
#include "Tympan/gui/OSizeManager.h"
#include "Tympan/gui/gl/TYPickingTable.h"
#include "Tympan/gui/gl/TYPaletteGraphic.h"
#include "Tympan/gui/gl/TYPanelGraphic.h"
#include "TYRectangularMaillageGraphic.h"

TYRectangularMaillageGraphic::TYRectangularMaillageGraphic(TYRectangularMaillage* pElement) :
    TYElementGraphic(pElement)
{
    _Rendering = RENDER_SURF;

    // XXX What are those constants ?
    _scalarX = 0.8;
    _scalarY = 0.25;

    _scalarW = 150.0;
    _scalarH = 15.0;

    _panelW = 125.0;
    _panelH = 40.0;

    TYElementGraphic* pTYElementGraphic = getElement()->getPalette()->getGraphicObject();
    ((TYPaletteGraphic*)pTYElementGraphic)->setWidth(_scalarW);
    ((TYPaletteGraphic*)pTYElementGraphic)->setHeight(_scalarH);

    pTYElementGraphic = getElement()->getPanel()->getGraphicObject();
    ((TYPanelGraphic*)pTYElementGraphic)->setWidth(_panelW);
    ((TYPanelGraphic*)pTYElementGraphic)->setHeight(_panelH);
    pTYElementGraphic->setVisible(false);
}

TYRectangularMaillageGraphic::~TYRectangularMaillageGraphic()
{}

void TYRectangularMaillageGraphic::update(bool force /*=false*/)
{
    TYRectangularMaillage* pRectangularMaillage = getElement();
    // TODO We do not want regular scale any longer.

    // And now... the mesh
    pRectangularMaillage->computeMesh(_mesh);

    // And ... the isocurve
    pRectangularMaillage->computeIsoCurve(_mesh, _isoCurve);

    pRectangularMaillage->getPalette()->getGraphicObject()->update(force);

    TYElementGraphic::update(force);
}

//===================================================================


void TYRectangularMaillageGraphic::computeBoundingBox()
{
    OBox reset;

    _boundingBox = reset;
    int index1 = 0, index2 = 0;
    unsigned long largeur, hauteur;

    TYRectangularMaillage* pMaillage = getElement();
    OVector3D vecX, vecY;
    pMaillage->getDimensionsAndSteps(largeur, hauteur, vecX, vecY);

    const unsigned int larg = largeur; // +1;
    const unsigned int haut = hauteur; // +1;

    TYTabLPPointCalcul* pPtsCalcul = &pMaillage->getPtsCalcul();

    for (int i = 0; i < haut - 1; i++)
    {
        for (int j = 0; j < larg; j++)
        {

            index1 = i * larg + j;
            TYPointCalcul* pPtCalcul = pPtsCalcul->at(index1);
            _boundingBox.Enlarge((float)(pPtCalcul->_x), (float)(pPtCalcul->_y), (float)(pPtCalcul->_z));
            index2 = (i + 1) * larg + j; //nextPoint(pPtsCalcul, index1, (i+1)*larg+j);
            pPtCalcul = pPtsCalcul->at(index2);
            _boundingBox.Enlarge((float)(pPtCalcul->_x), (float)(pPtCalcul->_y), (float)(pPtCalcul->_z));
        }
    }
}

void TYRectangularMaillageGraphic::display(GLenum mode) //GLenum mode = GL_RENDER
{
    // Pas affiche si inactif
    if (getElement()->getState() == TYMaillage::Inactif)
    {
        return;
    }

    if (_mesh.empty()) { update(); }

    // Les points de ce maillage
    TYRectangularMaillage* pMaillage = getElement();

    if (_visible)
    {

        if (mode == GL_SELECT)
        {
            TYPickingTable::addElement(getElement());
            glPushName((GLuint)(TYPickingTable::getIndex()));
        }

        switch (_Rendering)
        {
            case RENDER_SURF:
                displaySurface(mode);
                break;
            case RENDER_LINES:
                displayLines(mode);
                break;
            case RENDER_MIXED:
                displayMixed(mode);
                break;
            default:
                displaySurface(mode);
                break;
        }

        if (mode == GL_SELECT)
        {
            glPopName();
        }

        if (_highlight)
        {
            GLfloat pointSize;
            glGetFloatv(GL_POINT_SIZE, &pointSize);
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

            glPointSize(pointSize);

            // Affichage de la palette graphique
            TYElementGraphic* pTYElementGraphic = pMaillage->getPalette()->getGraphicObject();
            TYPaletteGraphic* pTYPaletteGraphic = (TYPaletteGraphic*)pTYElementGraphic;
            pTYPaletteGraphic->setPosX(OSizeManager::get()->getWidth()*_scalarX);
            pTYPaletteGraphic->setPosY(OSizeManager::get()->getHeight()*_scalarY);
            pTYPaletteGraphic->display();
        }

        // Affichage du panel
        TYElementGraphic* pTYElementGraphic = pMaillage->getPanel()->getGraphicObject();
        TYPanelGraphic* pTYPanelGraphic = (TYPanelGraphic*)pTYElementGraphic;
        pTYPanelGraphic->display();

        //Calcul du volume englobant pour le fit:
        _globalBoundingBox.Enlarge(_boundingBox);
    }
}


///////////////////////////////////////////////////////////////////////////////////////
// Mode de Rendu Surface
///////////////////////////////////////////////////////////////////////////////////////

void TYRectangularMaillageGraphic::displaySurface(GLenum mode) //GLenum mode = GL_RENDER
{
    // Opacite par defaut
    float opacity = 1.0;

#if TY_USE_IHM
    //  static const char prefName[] = "MaillageOpacity";

    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "MaillageOpacity"))
    {
        opacity = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, "MaillageOpacity");
    }
    else
    {
        TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, "MaillageOpacity", opacity);
    }

#endif // TY_USE_IHM

    // XXX WTF this part is funny... instead of a single loop it does
    // several loops (by batch of 1000 triangles)... for no reason at all?
    // Maybe an (old) issue with the number of primitives between the glBegin()
    // and glEnd()... but i've never heard of limit in this case...
    unsigned int bmin = 0;
    unsigned int bmax = 0;
    unsigned int inc = 0;

    int nbTriangles = _mesh.size();

    if (nbTriangles > 1000)
    {
        inc = 1000;
    }
    else
    {
        inc = nbTriangles;
    }

    bmin = 0;
    bmax += inc;
    bmax = (bmax <= nbTriangles ? bmax : nbTriangles);

    // Palette
    TYRectangularMaillage* pMaillage = getElement();
    LPTYPalette pPalette = pMaillage->getPalette();

    do
    {
        glBegin(GL_TRIANGLES);
        for (unsigned int i = bmin ; i < bmax; i++)
        {
            const MTriangle& vtx = _mesh[i];
            const MPoint& v1 = vtx.pts[0];
            const MPoint& v2 = vtx.pts[1];
            const MPoint& v3 = vtx.pts[2];

            const OColor& color0 = pPalette->getColorFromValue(v1.scalar);
            glColor4f(color0.r, color0.g, color0.b, opacity);
            glVertex3f(v1.pt._x, v1.pt._y, v1.pt._z);

            const OColor& color1 = pPalette->getColorFromValue(v2.scalar);
            glColor4f(color1.r, color1.g, color1.b, opacity);
            glVertex3f(v2.pt._x, v2.pt._y, v2.pt._z);

            const OColor& color2 = pPalette->getColorFromValue(v3.scalar);
            glColor4f(color2.r, color2.g, color2.b, opacity);
            glVertex3f(v3.pt._x, v3.pt._y, v3.pt._z);
        }
        glEnd();

        bmin = bmax;
        bmax += inc;
        bmax = (bmax <= nbTriangles ? bmax : nbTriangles);
    }
    while (bmin < nbTriangles);
}

///////////////////////////////////////////////////////////////////////////////////////
// Mode de Rendu Courbes
///////////////////////////////////////////////////////////////////////////////////////

void TYRectangularMaillageGraphic::displayLines(GLenum mode, bool invertColors)
{
    // Opacite par defaut
    float opacity = 1.0;

#if TY_USE_IHM
    //  static const char prefName[] = "MaillageOpacity";

    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "MaillageOpacity"))
    {
        opacity = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, "MaillageOpacity");
    }
    else
    {
        TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, "MaillageOpacity", opacity);
    }
#endif // TY_USE_IHM

    float lw;
    glGetFloatv(GL_LINE_WIDTH, &lw);
    glLineWidth(3.0f);

    std::vector<MPoint>::iterator it;

    // Palette
    TYRectangularMaillage* pMaillage = getElement();
    LPTYPalette pPalette = pMaillage->getPalette();

    glBegin(GL_LINES);
    for (it = _isoCurve.begin(); it != _isoCurve.end(); it++)
    {
        const MPoint& mp = *it;
        const OColor& color = pPalette->getColorFromValue(mp.scalar);
        if (invertColors)
        {
            glColor4f(1.0f - color.r, 1.0f - color.g, 1.0f - color.b, opacity);
        }
        else
        {
            glColor4f(color.r, color.g, color.b, opacity);
        }
        glVertex3f(mp.pt._x, mp.pt._y, mp.pt._z + 0.01); // 0.01 offset so that we can display the lines from top view
    }
    glEnd();

    glLineWidth(lw);
}

void TYRectangularMaillageGraphic::displayMixed(GLenum mode)
{
    displaySurface(mode);
    displayLines(mode, true);
}

int TYRectangularMaillageGraphic::nextPoint(const TYTabLPPointCalcul* pPtsCalcul, const int& index1, const int& index2)
{
    TYCalcul* pCalcul = static_cast<TYCalcul*>(getElement()->getParent());

    int indice = index2;

    while ((pPtsCalcul->at(indice)->getEtat(pCalcul) == false) && (indice > 0))
    {
        indice--;
    }

    return indice;
}
