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
 * \file TYOrientationEditor.cpp
 * \brief edition de l'orientation
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/GraphicIHM/ToolsGraphic/OGLLineElement.h"

#include "Tympan/models/business/OLocalizator.h"

#if defined(WIN32)
#include <crtdbg.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif


#define TR(id) OLocalizator::getString("TYOrientationEditor", (id))


TYOrientationEditor::TYOrientationEditor(TYSiteModelerFrame* pModeler) :
    TYAbstractSceneEditor(pModeler)
{
    _active = false;

    _pOGLLineElement = new OGLLineElement();
    _pOGLLineElement->setVisibility(false);
    OColor oLineColor;
    oLineColor.r = 1.0;
    oLineColor.g = 0.0;
    oLineColor.b = 0.0;
    _pOGLLineElement->setColor(oLineColor);

    _pInteractor->getRenderer()->addOGLElement(_pOGLLineElement);

    slotViewTypeChanged(_pModeler->getCurrentView());
}

TYOrientationEditor::~TYOrientationEditor()
{
    _pInteractor->getRenderer()->removeOGLElement(_pOGLLineElement);
    delete _pOGLLineElement;
}

void TYOrientationEditor::init()
{
    // Init seg a 0
    _pOGLLineElement->setPoint1(OPoint3D(0.0, 0.0, 0.0));
    _pOGLLineElement->setPoint2(OPoint3D(0.0, 0.0, 0.0));
}

void TYOrientationEditor::close()
{
    _pOGLLineElement->setVisibility(false);
    //_pOrientationActor->VisibilityOff();
    _pInteractor->updateGL();
}

void TYOrientationEditor::slotViewTypeChanged(int view)
{
    if (view == TYModelerFrame::TopView)
    {
        _active = true;
    }
    else
    {
        _active = false;
    }
}

void TYOrientationEditor::slotMousePressed(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
{
    if ((button == Qt::LeftButton) && _active)
    {
        _pOGLLineElement->setPoint1(OPoint3D(x, _pInteractor->height() - y, 0.0));
        _pOGLLineElement->setPoint2(OPoint3D(x, _pInteractor->height() - y, 0.0));
    }
}

void TYOrientationEditor::slotMouseMoved(int x, int y, Qt::MouseButtons button, Qt::KeyboardModifiers state)
{
    if ((button == Qt::LeftButton) && _active)
    {
        _pOGLLineElement->setPoint2(OPoint3D(x, _pInteractor->height() - y, 0.0));
        _pOGLLineElement->setVisibility(true);

        _pInteractor->updateGL();
    }
}

void TYOrientationEditor::slotMouseReleased(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
{
    if ((button == Qt::LeftButton) && _active)
    {
        double point[4];
        TYSegment segOrientation;
        LPTYSiteNode pSite = ((TYSiteModelerFrame*)_pModeler)->getSite();

        // Pt 1
        displayToWorld(_pOGLLineElement->getPoint1()[0],
                       _pOGLLineElement->getPoint1()[1],
                       _pOGLLineElement->getPoint1()[2],
                       point);
        segOrientation._ptA.setFromOGL(point);

        // Pt 2
        displayToWorld(_pOGLLineElement->getPoint2()[0],
                       _pOGLLineElement->getPoint2()[1],
                       _pOGLLineElement->getPoint2()[2],
                       point);
        segOrientation._ptB.setFromOGL(point);

        TYAction* pAction = new TYSetOrientationSiteAction(pSite, pSite->getOrientation(), segOrientation, _pModeler, TR("id_action_setorientation"));
        _pModeler->getActionManager()->addAction(pAction);

        pSite->setOrientation(segOrientation);
        pSite->updateGraphic();

        _pOGLLineElement->setVisibility(false);

        _pModeler->getView()->getRenderer()->updateDisplayList();

        _pModeler->updateView();

        // La scene a ete modifiee
        TYElement::setIsSavedOk(true);
    }
}
