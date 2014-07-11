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
 * \file TYCalculToolbar.cpp
 * \brief Barre d'outil calcul
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include <qtoolbutton.h>
#include <qbuttongroup.h>

#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/models/business/TYPreferenceManager.h"


#define TR(id) OLocalizator::getString("TYCalculToolbar", (id))
#define IMG(id) OLocalizator::getPicture("TYCalculToolbar", (id))


TYCalculToolbar::TYCalculToolbar(QButtonGroup* pBtnGroup, QMainWindow* parent /*= 0*/, QString title /*= ""*/):
    QToolBar(title, parent)
{
    Q_CHECK_PTR(pBtnGroup);

    _toolButtonRecept = new QToolButton(this);
    _toolButtonRecept->setIcon(QPixmap(IMG("id_icon_recept")));
    _toolButtonRecept->setText(TR("id_menuitem_recept"));
    _toolButtonRecept->setToolTip(TR("id_menuitem_recept"));
    _toolButtonRecept->setStatusTip(TR("id_menuitem_recept"));
    _toolButtonRecept->setCheckable(true);
    addWidget(_toolButtonRecept);
    pBtnGroup->addButton(_toolButtonRecept, TYSiteModelerFrame::RecepteurMode);

    addSeparator();

    _toolButtonMailRect = new QToolButton(this);
    _toolButtonMailRect->setIcon(QPixmap(IMG("id_icon_mailrect")));
    _toolButtonMailRect->setText(TR("id_menuitem_mailrect"));
    _toolButtonMailRect->setToolTip(TR("id_menuitem_mailrect"));
    _toolButtonMailRect->setStatusTip(TR("id_menuitem_mailrect"));
    _toolButtonMailRect->setCheckable(true);
    addWidget(_toolButtonMailRect);
    pBtnGroup->addButton(_toolButtonMailRect, TYSiteModelerFrame::MaillageRectMode);

    _toolButtonMailLin = new QToolButton(this);
    _toolButtonMailLin->setIcon(QPixmap(IMG("id_icon_maillin")));
    _toolButtonMailLin->setText(TR("id_menuitem_maillin"));
    _toolButtonMailLin->setToolTip(TR("id_menuitem_maillin"));
    _toolButtonMailLin->setStatusTip(TR("id_menuitem_maillin"));
    _toolButtonMailLin->setCheckable(true);
    addWidget(_toolButtonMailLin);
    pBtnGroup->addButton(_toolButtonMailLin, TYSiteModelerFrame::MaillageLinearMode);

    _toolButtonMailVert = new QToolButton(this);
    _toolButtonMailVert->setIcon(QPixmap(IMG("id_icon_mailvert")));
    _toolButtonMailVert->setText(TR("id_menuitem_mailvert"));
    _toolButtonMailVert->setToolTip(TR("id_menuitem_mailvert"));
    _toolButtonMailVert->setStatusTip(TR("id_menuitem_mailvert"));
    _toolButtonMailVert->setCheckable(true);
    addWidget(_toolButtonMailVert);
    pBtnGroup->addButton(_toolButtonMailVert, TYSiteModelerFrame::MaillageVerticalMode);

    _toolButtonBoundaryNoiseMap = new QToolButton(this);
    _toolButtonBoundaryNoiseMap->setIcon(QPixmap(IMG("id_icon_boundarynoisemap")));
    _toolButtonBoundaryNoiseMap->setText(TR("id_menuitem_boundarynoisemap"));
    _toolButtonBoundaryNoiseMap->setToolTip(TR("id_menuitem_boundarynoisemap"));
    _toolButtonBoundaryNoiseMap->setStatusTip(TR("id_menuitem_boundarynoisemap"));
    _toolButtonBoundaryNoiseMap->setCheckable(true);
    addWidget(_toolButtonBoundaryNoiseMap);
    pBtnGroup->addButton(_toolButtonBoundaryNoiseMap, TYSiteModelerFrame::BoundaryNoiseMapMode);
}

TYCalculToolbar::~TYCalculToolbar()
{
}

void TYCalculToolbar::activeButtons(bool active)
{
    _toolButtonRecept->setEnabled(active);
    _toolButtonMailRect->setEnabled(active);
    _toolButtonMailLin->setEnabled(active);
    _toolButtonMailVert->setEnabled(active);
    _toolButtonBoundaryNoiseMap->setEnabled(active);
}
