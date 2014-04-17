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
 * \file TYBatimentToolbar.cpp
 * \brief Barre d'outils Batiment
 *
 *
 */



#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include <qtoolbutton.h>
#include <qbuttongroup.h>

#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerCore/TYPreferenceManager.h"


#define TR(id) OLocalizator::getString("TYBatimentToolbar", (id))
#define IMG(id) OLocalizator::getPicture("TYBatimentToolbar", (id))


TYBatimentToolbar::TYBatimentToolbar(QButtonGroup* pBtnGroup, QMainWindow* parent /*= 0*/, QString title /*= ""*/):
    QToolBar(title, parent)
{
    Q_CHECK_PTR(pBtnGroup);

    _toolButtonEtage = new QToolButton(this);
    _toolButtonEtage->setIcon(QPixmap(IMG("id_icon_etage")));
    _toolButtonEtage->setText(TR("id_menuitem_etage"));
    _toolButtonEtage->setToolTip(TR("id_menuitem_etage"));
    _toolButtonEtage->setStatusTip(TR("id_menuitem_etage"));
    _toolButtonEtage->setCheckable(true);
    addWidget(_toolButtonEtage);
    pBtnGroup->addButton(_toolButtonEtage, TYBatimentModelerFrame::EtageMode);

    _toolButtonSilos = new QToolButton(this);
    _toolButtonSilos->setIcon(QPixmap(IMG("id_icon_silos")));
    _toolButtonSilos->setText(TR("id_menuitem_silos"));
    _toolButtonSilos->setToolTip(TR("id_menuitem_silos"));
    _toolButtonSilos->setStatusTip(TR("id_menuitem_silos"));
    _toolButtonSilos->setCheckable(true);
    addWidget(_toolButtonSilos);
    pBtnGroup->addButton(_toolButtonSilos, TYBatimentModelerFrame::SilosMode);

    addSeparator();

    _toolButtonSource = new QToolButton(this);
    _toolButtonSource->setIcon(QPixmap(IMG("id_icon_srcponct")));
    _toolButtonSource->setText(TR("id_menuitem_srcponct"));
    _toolButtonSource->setToolTip(TR("id_menuitem_srcponct"));
    _toolButtonSource->setStatusTip(TR("id_menuitem_srcponct"));
    _toolButtonSource->setCheckable(true);
    addWidget(_toolButtonSource);
    pBtnGroup->addButton(_toolButtonSource, TYBatimentModelerFrame::SourceMode);

}

TYBatimentToolbar::~TYBatimentToolbar()
{
}

void TYBatimentToolbar::activeButtons(bool active)
{
    _toolButtonEtage->setEnabled(active);
    _toolButtonSilos->setEnabled(active);
    _toolButtonSource->setEnabled(active);
}
