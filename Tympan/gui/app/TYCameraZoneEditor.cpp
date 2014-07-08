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
 * \file TYCameraZoneEditor.cpp
 * \brief gestion de zoom par zone selectionnee (fichier obsolete)
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/gui/tools/OGLRectangleElement.h"

#include "Tympan/gui/gl/TYPointGraphic.h"

#include "Tympan/models/business/geometry/TYRepere.h"
#include "TYApplication.h"
#include "Tympan/models/business/OLocalizator.h"
#include <cmath>

#include <qdialog.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qcursor.h>


#if defined(WIN32)
#include <crtdbg.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif


#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#define TR(id) OLocalizator::getString("TYCameraZoneEditor", (id))


TYCameraZoneEditor::TYCameraZoneEditor(TYModelerFrame* pModeler) :
    TYAbstractSceneEditor(pModeler)
{
    _pOGLRectangleElement = new OGLRectangleElement();
    _pOGLRectangleElement->setVisibility(false);
    OColor oLineColor;
    oLineColor.r = 1.0;
    oLineColor.g = 0.0;
    oLineColor.b = 0.0;
    _pOGLRectangleElement->setColor(oLineColor);
    _pOGLRectangleElement->setPoint0(OPoint3D(0.0, 0.0, 0.0));
    _pOGLRectangleElement->setPoint1(OPoint3D(0.0, 0.0, 0.0));
    _pOGLRectangleElement->setPoint2(OPoint3D(0.0, 0.0, 0.0));
    _pOGLRectangleElement->setPoint3(OPoint3D(0.0, 0.0, 0.0));
    _pInteractor->getRenderer()->addOGLElement(_pOGLRectangleElement);

    _moving = false;
    _active = false;
    slotViewTypeChanged(_pModeler->getCurrentView());
}

TYCameraZoneEditor::~TYCameraZoneEditor()
{
    _pInteractor->getRenderer()->removeOGLElement(_pOGLRectangleElement);
    delete _pOGLRectangleElement;
}

void TYCameraZoneEditor::init()
{
}

void TYCameraZoneEditor::cancel()
{
    _moving = false;
    _active = false;

    showText(false);

    _pOGLRectangleElement->setVisibility(false);
    _pInteractor->updateGL();
}

void TYCameraZoneEditor::close()
{
    cancel();
}

void TYCameraZoneEditor::slotViewTypeChanged(int view)
{
    if (view != TYModelerFrame::PerspView)
    {
        _active = true;
    }
    else
    {
        _active = false;
    }
}

void TYCameraZoneEditor::slotMousePressed(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
{
    if ((button == Qt::LeftButton) && _active)
    {
        if (getTYApp()->getCurProjet() && getTYApp()->getCurProjet()->getCurrentCalcul())
        {
            // Init points
            _pOGLRectangleElement->setPoint0(OPoint3D(x, _pInteractor->height() - y, 0.0));
            _pOGLRectangleElement->setPoint1(OPoint3D(x, _pInteractor->height() - y, 0.0));
            _pOGLRectangleElement->setPoint2(OPoint3D(x, _pInteractor->height() - y, 0.0));
            _pOGLRectangleElement->setPoint3(OPoint3D(x, _pInteractor->height() - y, 0.0));

            _moving = true;
        }
        else
        {
            _moving = false;

            // Msg "Pas de calcul courant..."
            QString msg(TR("id_warning_no_curcalcul"));
            writeOutputMsg(msg);
            QMessageBox::warning(_pModeler, "Tympan", msg, QMessageBox::Ok, QMessageBox::NoButton);
        }
    }
}

void TYCameraZoneEditor::slotMouseReleased(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
{
    if ((button == Qt::LeftButton) && _active && _moving)
    {
        _moving = false;

        NxVec3 world0 = OGLCamera::displayToWorld(NxVec3(_pOGLRectangleElement->getPoint0()[0], -_pOGLRectangleElement->getPoint0()[2], _pOGLRectangleElement->getPoint0()[1]));
        NxVec3 world1 = OGLCamera::displayToWorld(NxVec3(_pOGLRectangleElement->getPoint1()[0], -_pOGLRectangleElement->getPoint1()[2], _pOGLRectangleElement->getPoint1()[1]));
        NxVec3 world2 = OGLCamera::displayToWorld(NxVec3(_pOGLRectangleElement->getPoint2()[0], -_pOGLRectangleElement->getPoint2()[2], _pOGLRectangleElement->getPoint2()[1]));
        TYPoint pt0(world0.x, world0.y, world0.z);
        TYPoint pt1(world1.x, world1.y, world1.z);
        TYPoint pt2(world2.x, world2.y, world2.z);


        double sizeX = OVector3D(pt0, pt1).norme();
        double sizeY = OVector3D(pt1, pt2).norme();
        double sizeZ = 0;

        // Si la grille magnetique est activee
        if (_pModeler->getSnapGridActive())
        {
            snapToGrid(sizeX, sizeY, sizeZ);
        }

        // Si la taille est valide
        if (sizeX && sizeY)
        {
            if (_pModeler->askForResetResultat())
            {
                TYCameraEditor* pCameraEditor = (TYCameraEditor*) _pModeler->getCameraEditor();

                float focalPoint[3];
                focalPoint[0] = _pOGLRectangleElement->getPoint0()[0] + (_pOGLRectangleElement->getPoint2()[0] - _pOGLRectangleElement->getPoint0()[0]) / 2;
                focalPoint[1] = _pOGLRectangleElement->getPoint0()[1] + (_pOGLRectangleElement->getPoint2()[1] - _pOGLRectangleElement->getPoint0()[1]) / 2;
                focalPoint[2] = _pOGLRectangleElement->getPoint0()[2] + (_pOGLRectangleElement->getPoint2()[2] - _pOGLRectangleElement->getPoint0()[2]) / 2;

                double factor = 1;
                factor = min(_pInteractor->width() / (_pOGLRectangleElement->getPoint1()[0] - _pOGLRectangleElement->getPoint0()[0]), _pInteractor->height() / (_pOGLRectangleElement->getPoint1()[1] - _pOGLRectangleElement->getPoint2()[1]));
                pCameraEditor->cameraZoneZoom(focalPoint, fabs(factor));
            }
        }

        showText(false);

        _pOGLRectangleElement->setVisibility(false);
        _pModeler->updateView();

        TYApplication::restoreOverrideCursor();
    }
}

void TYCameraZoneEditor::slotMouseMoved(int x, int y, Qt::MouseButtons button, Qt::KeyboardModifiers state)
{
    if (_active && _moving)
    {
        _pOGLRectangleElement->setPoint1(OPoint3D(x, _pOGLRectangleElement->getPoint0()[1], 0.0));
        _pOGLRectangleElement->setPoint2(OPoint3D(x, _pInteractor->height() - y, 0.0));
        _pOGLRectangleElement->setPoint3(OPoint3D(_pOGLRectangleElement->getPoint0()[0], _pInteractor->height() - y, 0.0));

        NxVec3 world0 = OGLCamera::displayToWorld(NxVec3(_pOGLRectangleElement->getPoint0()[0], _pOGLRectangleElement->getPoint0()[2], 0));
        NxVec3 world1 = OGLCamera::displayToWorld(NxVec3(_pOGLRectangleElement->getPoint1()[0], _pOGLRectangleElement->getPoint1()[2], 0));
        NxVec3 world2 = OGLCamera::displayToWorld(NxVec3(_pOGLRectangleElement->getPoint2()[0], _pOGLRectangleElement->getPoint2()[2], 0));
        TYPoint pt0(world0.x, world0.y, world0.z);
        TYPoint pt1(world1.x, world1.y, world1.z);
        TYPoint pt2(world2.x, world2.y, world2.z);

        double sizeX = ABS(pt1[0] - pt0[0]);
        double sizeY = ABS(pt2[1] - pt1[1]);
        double sizeZ = (sizeX + sizeY) / 2.0;

        // Si la grille magnetique est activee
        if (_pModeler->getSnapGridActive())
        {
            snapToGrid(sizeX, sizeY, sizeZ);
        }
        updateText(QString(TR("id_size_info")).arg(sizeX, 0, 'f', 2).arg(sizeY, 0, 'f', 2),
                   (int)(_pOGLRectangleElement->getPoint0()[0] + (_pOGLRectangleElement->getPoint1()[0] - _pOGLRectangleElement->getPoint0()[0]) / 2.0),
                   (int)(_pOGLRectangleElement->getPoint1()[1] + (_pOGLRectangleElement->getPoint2()[1] - _pOGLRectangleElement->getPoint1()[1]) / 2.0));

        _pOGLRectangleElement->setVisibility(true);
        _pInteractor->updateGL();
    }
}
