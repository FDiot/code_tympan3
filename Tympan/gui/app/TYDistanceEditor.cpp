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
 * \file TYDistanceEditor.cpp
 * \brief outil de mesure des distances
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/gui/tools/OGLTextElement.h"
#include "Tympan/gui/tools/OGLLineElement.h"

#include <qcursor.h>
#include <qstatusbar.h>

#if defined(WIN32)
#include <crtdbg.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif


#define TR(id) OLocalizator::getString("TYDistanceEditor", (id))


TYDistanceEditor::TYDistanceEditor(TYModelerFrame* pModeler) :
    TYAbstractSceneEditor(pModeler)
{
    _pCamEditor = new TYCameraEditor(pModeler);

    _pCamEditor->setLeftButtonFunction2D(SLOT(doNothing()));
    _pCamEditor->setMiddleButtonFunction2D(SLOT(cameraTranslate()));
    _pCamEditor->setRightButtonFunction2D(SLOT(doNothing()));
    _pCamEditor->setShiftLeftButtonFunction2D(SLOT(cameraTranslate()));
    _pCamEditor->setShiftRightButtonFunction2D(SLOT(cameraZoom()));

    _shiftOn = false;
    _active = false;

    _pOGLLineElement = new OGLLineElement();
    _pOGLLineElement->setVisibility(false);
    OColor oLineColor;
    oLineColor.r = 1.0;
    oLineColor.g = 0.0;
    oLineColor.b = 0.0;
    _pOGLLineElement->setColor(oLineColor);

    _pInteractor->getRenderer()->addOGLElement(_pOGLLineElement);
}

TYDistanceEditor::~TYDistanceEditor()
{
    _pInteractor->getRenderer()->removeOGLElement(_pOGLLineElement);
    delete _pOGLLineElement;

    delete _pCamEditor;
}

void TYDistanceEditor::connect()
{
    TYAbstractSceneEditor::connect();

    _pCamEditor->connect();
}

void TYDistanceEditor::disconnect()
{
    TYAbstractSceneEditor::disconnect();

    _pCamEditor->disconnect();
}

void TYDistanceEditor::init()
{
}

void TYDistanceEditor::cancel()
{
    close();
}

void TYDistanceEditor::close()
{
    showText(false);

    _pOGLLineElement->setVisibility(false);

    _pInteractor->updateGL();
}

void TYDistanceEditor::slotKeyPressed(int key)
{
    switch (key)
    {
        case Qt::Key_Shift:
            _shiftOn = true;
            break;
        case Qt::Key_Escape:
            if (_active)
            {
                cancel();
            }
            break;
    }
}

void TYDistanceEditor::slotKeyReleased(int key)
{
    if (key == Qt::Key_Shift)
    {
        _shiftOn = false;
    }
}

void TYDistanceEditor::slotViewTypeChanged(int view)
{
    if ((view == TYModelerFrame::PerspView) || (view == TYModelerFrame::FreeView))
    {
        _active = false;
        cancel();
    }
    else
    {
        _active = true;
    }

    // On relaie le signal au CameraEditor
    _pCamEditor->slotViewTypeChanged(view);
}

void TYDistanceEditor::slotMousePressed(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
{
    if (_active && !_shiftOn)
    {
        if (button == Qt::LeftButton)
        {
            // 1er point
            _pOGLLineElement->setPoint1(OPoint3D(x, _pInteractor->height() - y , 0.0));

            // On peut dessiner la ligne
            _pOGLLineElement->setVisibility(true);
        }
    }
}

void TYDistanceEditor::slotMouseMoved(int x, int y, Qt::MouseButtons button, Qt::KeyboardModifiers state)
{
    if (button == Qt::LeftButton)
    {
        updateMovingLine();
    }
}

void TYDistanceEditor::slotMouseReleased(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
{
    if (_active && !_shiftOn)
    {
        if (button == Qt::LeftButton)
        {
            // Affichage Distance dans le output
            writeOutputMsg(_pOGLTextElement->getText());

            // Fin
            close();
        }
    }
}

bool TYDistanceEditor::updateMovingLine()
{
    if (!_active || _shiftOn)
    {
        return false;
    }

    // On recupere la position du curseur
    QPoint pos = _pInteractor->mapFromGlobal(QCursor::pos());

    // 2eme point
    _pOGLLineElement->setPoint2(OPoint3D(pos.x(), _pInteractor->height() - pos.y(), 0.0));

    // Calcul de la distance
    NxVec3 world0 = OGLCamera::displayToWorld(NxVec3(_pOGLLineElement->getPoint1()[0], _pOGLLineElement->getPoint1()[1], _pOGLLineElement->getPoint1()[2]));
    NxVec3 world1 = OGLCamera::displayToWorld(NxVec3(_pOGLLineElement->getPoint2()[0], _pOGLLineElement->getPoint2()[1], _pOGLLineElement->getPoint2()[2]));

    double dist = world0.distance(world1);

    // Msg Dist
    QString msg = QString(TR("id_output_dist")).arg(dist, 0, 'f', 2);

    updateText(msg, (int)(_pOGLLineElement->getPoint1()[0] + (_pOGLLineElement->getPoint2()[0] - _pOGLLineElement->getPoint1()[0]) / 2.0),
               (int)(_pOGLLineElement->getPoint2()[1] + (_pOGLLineElement->getPoint1()[1] - _pOGLLineElement->getPoint2()[1]) / 2.0));

    _pInteractor->updateGL();

    return true;
}
