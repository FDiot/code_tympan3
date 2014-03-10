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
 * \file TYBoundaryNoiseMap.cpp
 * \brief Graphical representation of the BoundaryNoiseMap entity
 *
 *
 */

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHGraphic.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/Tools/OSizeManager.h"

#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYBoundaryNoiseMap.h"
#include "Tympan/MetierSolver/DataManagerCore/TYPreferenceManager.h"

#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYCalcul.h"

TYBoundaryNoiseMapGraphic::TYBoundaryNoiseMapGraphic(TYBoundaryNoiseMap* pElement) :
    TYElementGraphic(pElement)
{
    _Rendering = RENDER_SURF;
    _pPolyLineGraphic = new TYPolyLineGraphic(pElement->isClosed());
    _pPolyLineGraphic->setTabPoint(getElement()->getTabPoint());
    _pPolyLineGraphic->setElement(pElement);
    // We don't want to render the polyline
    _pPolyLineGraphic->setVisible(false);

    _paletteX = 0.8;
    _paletteY = 0.25;

    _paletteW = 150.0;
    _paletteH = 15.0;

    TYElementGraphic* pTYElementGraphic = getElement()->getPalette()->getGraphicObject();
    ((TYPaletteGraphic*)pTYElementGraphic)->setWidth(_paletteW);
    ((TYPaletteGraphic*)pTYElementGraphic)->setHeight(_paletteH);

    _panelW = 125.0;
    _panelH = 40.0;

    pTYElementGraphic = getElement()->getPanel()->getGraphicObject();
    ((TYPanelGraphic*)pTYElementGraphic)->setWidth(_panelW);
    ((TYPanelGraphic*)pTYElementGraphic)->setHeight(_panelH);
    pTYElementGraphic->setVisible(false);
}

void TYBoundaryNoiseMapGraphic::update(bool force /*=false*/)
{
    // Update the polyline
    _pPolyLineGraphic = new TYPolyLineGraphic(getElement()->isClosed());
    _pPolyLineGraphic->setElement(getElement());
    _pPolyLineGraphic->setTabPoint(getElement()->getTabPoint());
    // We hide the polyline
    _pPolyLineGraphic->setVisible(false);

    TYBoundaryNoiseMap* boundaryNoiseMap = getElement();

    // And now... the mesh
    boundaryNoiseMap->computeMesh(_mesh);

    // And ... the isocurve
    boundaryNoiseMap->computeIsoCurve(_mesh, _isoCurve);

    boundaryNoiseMap->getPalette()->getGraphicObject()->update(force);
    TYElementGraphic::update(force);
}

void TYBoundaryNoiseMapGraphic::getChilds(TYListPtrTYElementGraphic& childs, bool recursive /*=true*/)
{
    TYElementGraphic* pTYElementGraphic = _pPolyLineGraphic;
    childs.push_back(pTYElementGraphic);
    if (recursive)
    {
        pTYElementGraphic->getChilds(childs, recursive);
    }
}

void TYBoundaryNoiseMapGraphic::computeBoundingBox()
{
    _pPolyLineGraphic->computeBoundingBox();
    _boundingBox = OBox(_pPolyLineGraphic->GetBox());
    double offset = getElement()->getThickness() * 0.5;
    OPoint3D& bb_min = _boundingBox._min;
    OPoint3D& bb_max = _boundingBox._max;
    bb_min._x -= offset;
    bb_min._y -= offset;
    bb_max._x += offset;
    bb_max._y += offset;
}

void TYBoundaryNoiseMapGraphic::displayMixed(float opacity)
{
    displaySurface(opacity);
    displayLines(opacity, true);
}

void TYBoundaryNoiseMapGraphic::display(GLenum mode /*= GL_RENDER*/)
{
    if (_mesh.empty())
    {
        update();
    }

    if (!_visible || getElement()->getState() == TYMaillage::Inactif)
    {
        return;
    }

    if (mode == GL_SELECT)
    {
        TYPickingTable::addElement(getElement());
        glPushName((GLuint)(TYPickingTable::getIndex()));
    }

    // Default opacity
    float opacity = 1.f;
#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "MaillageOpacity"))
    {
        opacity = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, "MaillageOpacity");
    }
    else
    {
        TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, "MaillageOpacity", opacity);
    }
#endif // TY_USE_IHM

    switch (_Rendering)
    {
        case RENDER_SURF:
            displaySurface(opacity);
            break;
        case RENDER_LINES:
            displayLines(opacity);
            break;
        case RENDER_MIXED:
            displayMixed(opacity);
            break;
        default:
            displaySurface(opacity);
            break;
    }

    glColor3f(1.f, 0.f, 0.f);
    if (mode == GL_SELECT) { glPopName(); }
    if (_highlight)
    {
        if (_bFirstDisp)
        {
            computeBoundingBox();
            _bFirstDisp = false;
        }
        drawLineBoundingBox();
        displayOnHighlight(mode);
    }
    // Panel display
    TYElementGraphic* pTYElementGraphic = getElement()->getPanel()->getGraphicObject();
    TYPanelGraphic* pTYPanelGraphic = (TYPanelGraphic*)pTYElementGraphic;
    pTYPanelGraphic->display();

    // Bounding box computation
    _globalBoundingBox.Enlarge(_boundingBox);
}

void TYBoundaryNoiseMapGraphic::displaySurface(float opacity) //GLenum mode = GL_RENDER
{
    LPTYPalette pPalette = getElement()->getPalette();
    MTriangle vtx;
    size_t nbTriangles = _mesh.size();
    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < nbTriangles; i++)
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
}

void TYBoundaryNoiseMapGraphic::displayLines(float opacity, bool invertColors)
{
    float lw;
    glGetFloatv(GL_LINE_WIDTH, &lw);
    glLineWidth(3.0f);

    LPTYPalette pPalette = getElement()->getPalette();
    size_t nbIsoCurve = _isoCurve.size();

    glBegin(GL_LINES);
    for (size_t i = 0; i < nbIsoCurve; ++i)
    {
        const MPoint& mp = _isoCurve[i];
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

void TYBoundaryNoiseMapGraphic::displayOnHighlight(int mode)
{
    // Display the graphical palette
    TYElementGraphic* pTYElementGraphic = getElement()->getPalette()->getGraphicObject();
    TYPaletteGraphic* pTYPaletteGraphic = (TYPaletteGraphic*)pTYElementGraphic;
    pTYPaletteGraphic->setPosX(OSizeManager::get()->getWidth() * _paletteX);
    pTYPaletteGraphic->setPosY(OSizeManager::get()->getHeight() * _paletteY);
    pTYPaletteGraphic->display();
}
