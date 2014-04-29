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
 * \file TYTopoToolbar.cpp
 * \brief Barre d'outils topographie
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include <qtoolbutton.h>
#include <qbuttongroup.h>

#include "TYTopoToolbar.h"

#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerCore/TYPreferenceManager.h"


#define TR(id) OLocalizator::getString("TYTopoToolbar", (id))
#define IMG(id) OLocalizator::getPicture("TYTopoToolbar", (id))


TYTopoToolbar::TYTopoToolbar(QButtonGroup* pBtnGroup, QMainWindow* parent /*= 0*/, QString title /*= ""*/):
    QToolBar(title, parent)
{
    Q_CHECK_PTR(pBtnGroup);

    _toolButtonEchelle = new QToolButton(this);
    _toolButtonEchelle->setIcon(QPixmap(IMG("id_icon_echelle")));
    _toolButtonEchelle->setText(TR("id_menuitem_echelle"));
    _toolButtonEchelle->setToolTip(TR("id_menuitem_echelle"));
    _toolButtonEchelle->setStatusTip(TR("id_menuitem_echelle"));
    _toolButtonEchelle->setCheckable(true);
    addWidget(_toolButtonEchelle);
    pBtnGroup->addButton(_toolButtonEchelle, TYSiteModelerFrame::EchelleMode);

    _toolButtonPtRef = new QToolButton(this);
    _toolButtonPtRef->setIcon(QPixmap(IMG("id_icon_ptref")));
    _toolButtonPtRef->setText(TR("id_menuitem_ptref"));
    _toolButtonPtRef->setToolTip(TR("id_menuitem_ptref"));
    _toolButtonPtRef->setStatusTip(TR("id_menuitem_ptref"));
    _toolButtonPtRef->setCheckable(true);
    addWidget(_toolButtonPtRef);
    pBtnGroup->addButton(_toolButtonPtRef, TYSiteModelerFrame::PointRefMode);

    _toolButtonOrientation = new QToolButton(this);
    _toolButtonOrientation->setIcon(QPixmap(IMG("id_icon_orientation")));
    _toolButtonOrientation->setText(TR("id_menuitem_orientation"));
    _toolButtonOrientation->setToolTip(TR("id_menuitem_orientation"));
    _toolButtonOrientation->setStatusTip(TR("id_menuitem_orientation"));
    _toolButtonOrientation->setCheckable(true);
    addWidget(_toolButtonOrientation);
    pBtnGroup->addButton(_toolButtonOrientation, TYSiteModelerFrame::OrientationMode);

    _toolButtonEmprise = new QToolButton(this);
    _toolButtonEmprise->setIcon(QPixmap(IMG("id_icon_emprise")));
    _toolButtonEmprise->setText(TR("id_menuitem_emprise"));
    _toolButtonEmprise->setToolTip(TR("id_menuitem_emprise"));
    _toolButtonEmprise->setStatusTip(TR("id_menuitem_emprise"));
    _toolButtonEmprise->setCheckable(true);
    addWidget(_toolButtonEmprise);
    pBtnGroup->addButton(_toolButtonEmprise, TYSiteModelerFrame::EmpriseMode);

    addSeparator();

    _toolButtonCrbNiv = new QToolButton(this);
    _toolButtonCrbNiv->setIcon(QPixmap(IMG("id_icon_crbniv")));
    _toolButtonCrbNiv->setText(TR("id_menuitem_crbniv"));
    _toolButtonCrbNiv->setToolTip(TR("id_menuitem_crbniv"));
    _toolButtonCrbNiv->setStatusTip(TR("id_menuitem_crbniv"));
    _toolButtonCrbNiv->setCheckable(true);
    addWidget(_toolButtonCrbNiv);
    pBtnGroup->addButton(_toolButtonCrbNiv, TYSiteModelerFrame::CourbeNiveauMode);

    _toolButtonTerrain = new QToolButton(this);
    _toolButtonTerrain->setIcon(QPixmap(IMG("id_icon_ter")));
    _toolButtonTerrain->setText(TR("id_menuitem_ter"));
    _toolButtonTerrain->setToolTip(TR("id_menuitem_ter"));
    _toolButtonTerrain->setStatusTip(TR("id_menuitem_ter"));
    _toolButtonTerrain->setCheckable(true);
    addWidget(_toolButtonTerrain);
    pBtnGroup->addButton(_toolButtonTerrain, TYSiteModelerFrame::TerrainMode);

    _toolButtonVeg = new QToolButton(this);
    _toolButtonVeg->setIcon(QPixmap(IMG("id_icon_veg")));
    _toolButtonVeg->setText(TR("id_menuitem_veg"));
    _toolButtonVeg->setToolTip(TR("id_menuitem_veg"));
    _toolButtonVeg->setStatusTip(TR("id_menuitem_veg"));
    _toolButtonVeg->setCheckable(true);
    addWidget(_toolButtonVeg);
    pBtnGroup->addButton(_toolButtonVeg, TYSiteModelerFrame::VegetationMode);

    _toolButtonPlanEau = new QToolButton(this);
    _toolButtonPlanEau->setIcon(QPixmap(IMG("id_icon_planeau")));
    _toolButtonPlanEau->setText(TR("id_menuitem_planeau"));
    _toolButtonPlanEau->setToolTip(TR("id_menuitem_planeau"));
    _toolButtonPlanEau->setStatusTip(TR("id_menuitem_planeau"));
    _toolButtonPlanEau->setCheckable(true);
    addWidget(_toolButtonPlanEau);
    pBtnGroup->addButton(_toolButtonPlanEau, TYSiteModelerFrame::PlanEauMode);

    _toolButtonCrsEau = new QToolButton(this);
    _toolButtonCrsEau->setIcon(QPixmap(IMG("id_icon_crseau")));
    _toolButtonCrsEau->setText(TR("id_menuitem_crseau"));
    _toolButtonCrsEau->setToolTip(TR("id_menuitem_crseau"));
    _toolButtonCrsEau->setStatusTip(TR("id_menuitem_crseau"));
    _toolButtonCrsEau->setCheckable(true);
    addWidget(_toolButtonCrsEau);
    pBtnGroup->addButton(_toolButtonCrsEau, TYSiteModelerFrame::CoursEauMode);
}

TYTopoToolbar::~TYTopoToolbar()
{
}

void TYTopoToolbar::activeButtons(bool active)
{
    _toolButtonEchelle->setEnabled(active);
    _toolButtonPtRef->setEnabled(active);
    _toolButtonOrientation->setEnabled(active);
    _toolButtonEmprise->setEnabled(active);
    _toolButtonCrbNiv->setEnabled(active);
    _toolButtonTerrain->setEnabled(active);
    _toolButtonVeg->setEnabled(active);
    _toolButtonPlanEau->setEnabled(active);
    _toolButtonCrsEau->setEnabled(active);
}
