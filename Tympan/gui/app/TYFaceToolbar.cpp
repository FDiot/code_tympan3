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
 * \file TYFaceToolbar.cpp
 * \brief Barre d'outils Face
 */


#include <qtoolbutton.h>
#include <qbuttongroup.h>

#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/TYPreferenceManager.h"
#include "Tympan/gui/app/TYFaceModelerFrame.h"
#include "TYFaceToolbar.h"

#define TR(id) OLocalizator::getString("TYFaceToolbar", (id))
#define IMG(id) OLocalizator::getPicture("TYFaceToolbar", (id))


TYFaceToolbar::TYFaceToolbar(QButtonGroup* pBtnGroup, QMainWindow* parent /*= 0*/, QString title /*= ""*/):
    QToolBar(title, parent)
{
    _boucheActive = false;
    _chemineeActive = false;
    _fenetreActive = false;

    Q_CHECK_PTR(pBtnGroup);

    _toolButtonBouche = new QToolButton(this);
    _toolButtonBouche->setIcon(QPixmap(IMG("id_icon_bouche")));
    _toolButtonBouche->setText(TR("id_menuitem_bouche"));
    _toolButtonBouche->setToolTip(TR("id_menuitem_bouche"));
    _toolButtonBouche->setStatusTip(TR("id_menuitem_bouche"));
    _toolButtonBouche->setCheckable(true);
    addWidget(_toolButtonBouche);
    pBtnGroup->addButton(_toolButtonBouche, TYFaceModelerFrame::BoucheMode);

    _toolButtonCheminee = new QToolButton(this);
    _toolButtonCheminee->setIcon(QPixmap(IMG("id_icon_cheminee")));
    _toolButtonCheminee->setText(TR("id_menuitem_cheminee"));
    _toolButtonCheminee->setToolTip(TR("id_menuitem_cheminee"));
    _toolButtonCheminee->setStatusTip(TR("id_menuitem_cheminee"));
    _toolButtonCheminee->setCheckable(true);
    addWidget(_toolButtonCheminee);
    pBtnGroup->addButton(_toolButtonCheminee, TYFaceModelerFrame::ChemineeMode);

    _toolButtonFenetre = new QToolButton(this);
    _toolButtonFenetre->setIcon(QPixmap(IMG("id_icon_fenetre")));
    _toolButtonFenetre->setText(TR("id_menuitem_fenetre"));
    _toolButtonFenetre->setToolTip(TR("id_menuitem_fenetre"));
    _toolButtonFenetre->setStatusTip(TR("id_menuitem_fenetre"));
    _toolButtonFenetre->setCheckable(true);
    addWidget(_toolButtonFenetre);
    pBtnGroup->addButton(_toolButtonFenetre, TYFaceModelerFrame::FenetreMode);
}

TYFaceToolbar::~TYFaceToolbar()
{
}

void TYFaceToolbar::activeButtons(bool active)
{
    if (active)
    {
        _toolButtonBouche->setEnabled(_boucheActive);
        _toolButtonCheminee->setEnabled(_chemineeActive);
        _toolButtonFenetre->setEnabled(_fenetreActive);
    }
    else
    {
        _toolButtonBouche->setEnabled(active);
        _toolButtonCheminee->setEnabled(active);
        _toolButtonFenetre->setEnabled(active);
    }
}

void TYFaceToolbar::activeBouche(bool active)
{
    _boucheActive = active;
    _toolButtonBouche->setEnabled(active);
}

void TYFaceToolbar::activeCheminee(bool active)
{
    _chemineeActive = active;
    _toolButtonCheminee->setEnabled(active);
}

void TYFaceToolbar::activeFenetre(bool active)
{
    _fenetreActive = active;
    _toolButtonFenetre->setEnabled(active);
}
