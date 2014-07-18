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
 * \file TYWidget.cpp
 * \brief outil IHM pour un objet metier de type TYElement
 *
 *
 */





#include <qstring.h>

#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/TYPreferenceManager.h"
#include "Tympan/models/business/TYElement.h"

#include "TYWidget.h"

#define TR(id) OLocalizator::getString("TYWidget", (id))


TYWidget::TYWidget(TYElement* pElement, QWidget* parent, const char* name, Qt::WFlags f):
    QWidget(parent, f), _pElement(pElement)
{
    setObjectName(QString(name));
    _locked = false;
}

TYWidget::~TYWidget()
{
}

/*static*/ int TYWidget::edit(TYElement* pElement, QWidget* pParent /*=NULL*/)
{
    assert(pElement);
    QDialog* pDlg = new QDialog(pParent);
    pDlg->setModal(true);
    pDlg->setWindowTitle(getDisplayName(pElement));

    QWidget* pMainWidget = pElement->getEditWidget();
    pMainWidget->setParent(pDlg);

    QBoxLayout* pLayout = new QVBoxLayout();
    pLayout->addWidget(pMainWidget);
    pDlg->setLayout(pLayout);

    // On recupere les settings
    TYPreferenceManager::loadGeometryFromPreferences(pMainWidget->metaObject()->className(), pDlg);

    QPushButton* pButtonOK = new QPushButton(TR("id_ok_btn"), pDlg);
    pButtonOK->setDefault(true);
    connect(pButtonOK, SIGNAL(clicked()), pDlg, SLOT(accept()));

    QPushButton* pButtonCancel = new QPushButton(TR("id_cancel_btn"), pDlg);
    pButtonCancel->setShortcut(Qt::Key_Escape);
    connect(pButtonCancel, SIGNAL(clicked()), pDlg, SLOT(reject()));


    pLayout->addSpacing(5);
    QBoxLayout* pBtnLayout = new QHBoxLayout();
    pBtnLayout->setMargin(10);
    pLayout->addLayout(pBtnLayout);

    pBtnLayout->addStretch();
    pBtnLayout->addWidget(pButtonOK);
    pBtnLayout->addSpacing(5);
    pBtnLayout->addWidget(pButtonCancel);

    // Affiche la boite de dialogue
    int ret = pDlg->exec();

    // Applique les modificatins si necessaire
    if (ret == QDialog::Accepted)
    {
        ((TYWidget*)pMainWidget)->apply();
    }
    else // Reject
    {
        ((TYWidget*)pMainWidget)->reject();
    }

    // On sauve les settings
    TYPreferenceManager::saveGeometryToPreferences(pMainWidget->metaObject()->className(), pDlg);

    // Liberation de la memoire
    if (pParent)
    {
        pDlg->setParent(0);
    }

    disconnect(pButtonOK, SIGNAL(clicked()), pDlg, SLOT(accept()));
    disconnect(pButtonCancel, SIGNAL(clicked()), pDlg, SLOT(reject()));

    delete pDlg;

    return ret;
}


/*static*/ QString TYWidget::getDisplayName(TYElement* pElt)
{
    Q_ASSERT(pElt);

    return OLocalizator::getString("DisplayName", pElt->getClassName());
}
