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
 * \file TYModelerToolbar.cpp
 * \brief Barre d'outils generale pour les modelers
 *
 *
 */



#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
//Added by qt3to4:
#include <QPixmap>

#include <qtoolbutton.h>
#include <qbuttongroup.h>

#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/models/business/TYPreferenceManager.h"


#define TR(id) OLocalizator::getString("TYModelerToolbar", (id))
#define IMG(id) OLocalizator::getPicture("TYModelerToolbar", (id))


TYModelerToolbar::TYModelerToolbar(QButtonGroup* pBtnGroup /*= 0*/, QMainWindow* parent /*= 0*/, QString title /*= ""*/):
    QToolBar(title, parent)
{
    _toolButtonCamera = new QToolButton(this);
    _toolButtonCamera->setIcon(QPixmap(IMG("id_icon_camera")));
    _toolButtonCamera->setText(TR("id_menuitem_camera"));
    _toolButtonCamera->setToolTip(TR("id_menuitem_camera"));
    _toolButtonCamera->setStatusTip(TR("id_menuitem_camera"));
    _toolButtonCamera->setCheckable(true);
    addWidget(_toolButtonCamera);

    _toolButtonCameraZone = new QToolButton(this);
    _toolButtonCameraZone->setIcon(QPixmap(IMG("id_icon_camerazone")));
    _toolButtonCameraZone->setText(TR("id_menuitem_camerazone"));
    _toolButtonCameraZone->setToolTip(TR("id_menuitem_camerazone"));
    _toolButtonCameraZone->setStatusTip(TR("id_menuitem_camerazone"));
    _toolButtonCameraZone->setCheckable(true);
    addWidget(_toolButtonCameraZone);

    _toolButtonCameraStep = new QToolButton(this);
    _toolButtonCameraStep->setIcon(QPixmap(IMG("id_icon_camerastep")));
    _toolButtonCameraStep->setText(TR("id_menuitem_camerastep"));
    _toolButtonCameraStep->setToolTip(TR("id_menuitem_camerastep"));
    _toolButtonCameraStep->setStatusTip(TR("id_menuitem_camerastep"));
    _toolButtonCameraStep->setCheckable(true);
    addWidget(_toolButtonCameraStep);

    _toolButtonDistance = new QToolButton(this);
    _toolButtonDistance->setIcon(QPixmap(IMG("id_icon_distance")));
    _toolButtonDistance->setText(TR("id_menuitem_distance"));
    _toolButtonDistance->setToolTip(TR("id_menuitem_distance"));
    _toolButtonDistance->setStatusTip(TR("id_menuitem_distance"));
    _toolButtonDistance->setCheckable(true);
    addWidget(_toolButtonDistance);

    _toolButtonMoving = new QToolButton(this);
    _toolButtonMoving->setIcon(QPixmap(IMG("id_icon_moving")));
    _toolButtonMoving->setText(TR("id_menuitem_moving"));
    _toolButtonMoving->setToolTip(TR("id_menuitem_moving"));
    _toolButtonMoving->setStatusTip(TR("id_menuitem_moving"));
    _toolButtonMoving->setCheckable(true);
    addWidget(_toolButtonMoving);

    _toolButtonRotation = new QToolButton(this);
    _toolButtonRotation->setIcon(QPixmap(IMG("id_icon_rotation")));
    _toolButtonRotation->setText(TR("id_menuitem_rotation"));
    _toolButtonRotation->setToolTip(TR("id_menuitem_rotation"));
    _toolButtonRotation->setStatusTip(TR("id_menuitem_rotation"));
    _toolButtonRotation->setCheckable(true);
    addWidget(_toolButtonRotation);

    _toolButtonEdition = new QToolButton(this);
    _toolButtonEdition->setIcon(QPixmap(IMG("id_icon_edition")));
    _toolButtonEdition->setText(TR("id_menuitem_edition"));
    _toolButtonEdition->setToolTip(TR("id_menuitem_edition"));
    _toolButtonEdition->setStatusTip(TR("id_menuitem_edition"));
    _toolButtonEdition->setCheckable(true);
    addWidget(_toolButtonEdition);

    if (pBtnGroup)
    {
        addToButtonGroup(pBtnGroup);
    }
}

TYModelerToolbar::~TYModelerToolbar()
{
}

void TYModelerToolbar::activeButtons(bool active)
{
    _toolButtonCamera->setEnabled(active);
    _toolButtonCameraZone->setEnabled(active);
    _toolButtonCameraStep->setEnabled(active);
    _toolButtonDistance->setEnabled(active);
    _toolButtonMoving->setEnabled(active);
    _toolButtonRotation->setEnabled(active);
    _toolButtonEdition->setEnabled(active);
}

void TYModelerToolbar::activeModif(bool active)
{
    _toolButtonMoving->setEnabled(active);
    _toolButtonRotation->setEnabled(active);
    _toolButtonEdition->setEnabled(active);
}

void TYModelerToolbar::addToButtonGroup(QButtonGroup* pBtnGroup)
{
    Q_CHECK_PTR(pBtnGroup);

    pBtnGroup->addButton(_toolButtonCamera, TYModelerFrame::CameraMode);
    _toolButtonCamera->setChecked(false);
    pBtnGroup->addButton(_toolButtonCameraZone, TYModelerFrame::CameraZoneMode);
    _toolButtonCameraZone->setChecked(false);
    pBtnGroup->addButton(_toolButtonCameraStep, TYModelerFrame::CameraStepMode);
    _toolButtonCameraStep->setChecked(false);
    pBtnGroup->addButton(_toolButtonDistance, TYModelerFrame::DistanceMode);
    _toolButtonDistance->setChecked(false);
    pBtnGroup->addButton(_toolButtonMoving, TYModelerFrame::MovingMode);
    _toolButtonMoving->setChecked(false);
    pBtnGroup->addButton(_toolButtonRotation, TYModelerFrame::RotationMode);
    _toolButtonRotation->setChecked(false);
    pBtnGroup->addButton(_toolButtonEdition, TYModelerFrame::EditionMode);
    _toolButtonEdition->setChecked(false);
}

void TYModelerToolbar::removeFromButtonGroup(QButtonGroup* pBtnGroup)
{
    Q_CHECK_PTR(pBtnGroup);

    pBtnGroup->removeButton(_toolButtonCamera);
    pBtnGroup->removeButton(_toolButtonCameraZone);
    pBtnGroup->removeButton(_toolButtonCameraStep);
    pBtnGroup->removeButton(_toolButtonDistance);
    pBtnGroup->removeButton(_toolButtonMoving);
    pBtnGroup->removeButton(_toolButtonRotation);
    pBtnGroup->removeButton(_toolButtonEdition);
}

void TYModelerToolbar::viewTypeChanged(int view)
{
    activeCameraButtons(view);
    getTYMainWnd()->setDefaultCameraMode();
}

void TYModelerToolbar::activeCameraButtons(int view)
{
    if (view == TYModelerFrame::PerspView)
    {
        _toolButtonCameraStep->setEnabled(true);
        _toolButtonCameraZone->setEnabled(false);

        // CLM-NT35 : Empecher de deplacer/tourner en vue perspective/mobile
        getTYMainWnd()->getModelerToolbar()->activeModif(false);
    }
    else if (view == TYModelerFrame::FreeView)
    {
        _toolButtonCameraStep->setEnabled(false);
        _toolButtonCameraZone->setEnabled(false);

        // CLM-NT35 : Empecher de deplacer/tourner en vue perspective/mobile
        getTYMainWnd()->getModelerToolbar()->activeModif(false);
    }
    else
    {
        _toolButtonCameraStep->setEnabled(false);
        _toolButtonCameraZone->setEnabled(true);

        // CLM-NT35 : Empecher de deplacer/tourner en vue perspective/mobile
        getTYMainWnd()->getModelerToolbar()->activeModif(true);
    }
}
