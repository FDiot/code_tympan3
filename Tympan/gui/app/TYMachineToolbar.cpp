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
 * \file TYMachineToolbar.cpp
 * \brief Barre d'outils Machine
 */


#include <qtoolbutton.h>
#include <qbuttongroup.h>
#include <QToolBar>

#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/TYPreferenceManager.h"
#include "Tympan/gui/app/TYMachineModelerFrame.h"
#include "TYMachineToolbar.h"

#define TR(id) OLocalizator::getString("TYMachineToolbar", (id))
#define IMG(id) OLocalizator::getPicture("TYMachineToolbar", (id))


TYMachineToolbar::TYMachineToolbar(QButtonGroup* pBtnGroup, QMainWindow* parent /*= 0*/, QString title /*= ""*/):
    QToolBar(title, parent)
{
    Q_CHECK_PTR(pBtnGroup);

    _toolButtonBox = new QToolButton(this);
    _toolButtonBox->setIcon(QPixmap(IMG("id_icon_box")));
    _toolButtonBox->setText(TR("id_menuitem_box"));
    _toolButtonBox->setToolTip(TR("id_menuitem_box"));
    _toolButtonBox->setStatusTip(TR("id_menuitem_box"));
    _toolButtonBox->setCheckable(true);
    addWidget(_toolButtonBox);
    pBtnGroup->addButton(_toolButtonBox, TYMachineModelerFrame::AddBoxMode);

    _toolButtonCyl = new QToolButton(this);
    _toolButtonCyl->setIcon(QPixmap(IMG("id_icon_cyl")));
    _toolButtonCyl->setText(TR("id_menuitem_cyl"));
    _toolButtonCyl->setToolTip(TR("id_menuitem_cyl"));
    _toolButtonCyl->setStatusTip(TR("id_menuitem_cyl"));
    _toolButtonCyl->setCheckable(true);
    addWidget(_toolButtonCyl);
    pBtnGroup->addButton(_toolButtonCyl, TYMachineModelerFrame::AddCylMode);

    _toolButtonSemiCyl = new QToolButton(this);
    _toolButtonSemiCyl->setIcon(QPixmap(IMG("id_icon_semicyl")));
    _toolButtonSemiCyl->setText(TR("id_menuitem_semicyl"));
    _toolButtonSemiCyl->setToolTip(TR("id_menuitem_semicyl"));
    _toolButtonSemiCyl->setStatusTip(TR("id_menuitem_semicyl"));
    _toolButtonSemiCyl->setCheckable(true);
    addWidget(_toolButtonSemiCyl);
    pBtnGroup->addButton(_toolButtonSemiCyl, TYMachineModelerFrame::AddSemiCylMode);

}

TYMachineToolbar::~TYMachineToolbar()
{
}

void TYMachineToolbar::activeButtons(bool active)
{
    _toolButtonBox->setEnabled(active);
    _toolButtonCyl->setEnabled(active);
    _toolButtonSemiCyl->setEnabled(active);
}
