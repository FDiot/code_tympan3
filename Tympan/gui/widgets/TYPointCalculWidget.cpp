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
 * \file TYPointCalculWidget.cpp
 * \brief outil IHM pour un point de calcul
 */


#include <qradiobutton.h>
#include <qbuttongroup.h>
//Added by qt3to4:
#include <QGridLayout>

#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/TYPointCalcul.h"
#include "Tympan/models/business/TYCalcul.h"
#include "Tympan/gui/widgets/TYPointWidget.h"
#include "TYPointCalculWidget.h"

#define TR(id) OLocalizator::getString("TYPointCalculWidget", (id))


TYPointCalculWidget::TYPointCalculWidget(TYPointCalcul* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{
    unsigned short wLn = 0;

    resize(300, 200);

    setWindowTitle(TR("id_caption"));
    _pPointCalculLayout = new QGridLayout();
    setLayout(_pPointCalculLayout);

    _elmW = new TYElementWidget(pElement, this);
    _pPointCalculLayout->addWidget(_elmW, wLn++, 0);

    // Affichage de l'etat du spectre (actif/inactif)
    _buttonGroupState = new QButtonGroup();
    _buttonGroupState->setExclusive(true);
    _pRadioButtonInactif = new QRadioButton(TR("id_inactif"));
    _buttonGroupState->addButton(_pRadioButtonInactif, 0);
    _pRadioButtonActif = new QRadioButton(TR("id_actif"));
    _buttonGroupState->addButton(_pRadioButtonActif, 1);

    QGridLayout* groupBoxStateLayout = new QGridLayout();
    groupBoxStateLayout->addWidget(_pRadioButtonInactif, 0, 0);
    groupBoxStateLayout->addWidget(_pRadioButtonActif, 0, 1);

    _groupBoxState = new QGroupBox();
    _groupBoxState->setTitle(TR("id_etat"));
    _groupBoxState->setLayout(groupBoxStateLayout);

    _pPointCalculLayout->addWidget(_groupBoxState, wLn++, 0);

    // Groupbox du spectre
    QGroupBox* pGroupBoxSpectre = new QGroupBox(this);
    pGroupBoxSpectre->setTitle(TR("id_spectre"));
    QGridLayout* pGroupBoxSpectreLayout = new QGridLayout();
    pGroupBoxSpectre->setLayout(pGroupBoxSpectreLayout);

    // Choix du calcul associe au spectre
    _labelSpectreCalcul = new QLabel(pGroupBoxSpectre);
    _labelSpectreCalcul->setText(TR("id_spectre_calcul"));
    pGroupBoxSpectreLayout->addWidget(_labelSpectreCalcul, 0, 0);

    // Spectre
    _lineEditNomSpectre = new QLineEdit(pGroupBoxSpectre);
    _lineEditNomSpectre->setEnabled(false);
    pGroupBoxSpectreLayout->addWidget(_lineEditNomSpectre, 1, 0);

    QPushButton* pPushButtonSpectre = new QPushButton(TR("id_proprietes_button"), pGroupBoxSpectre);
    QObject::connect(pPushButtonSpectre, SIGNAL(clicked()), this, SLOT(editSpectre()));
    pGroupBoxSpectreLayout->addWidget(pPushButtonSpectre, 1, 1);

    _pPointCalculLayout->addWidget(pGroupBoxSpectre, wLn++, 0);

    // Affichage de la position
    _ptW = new TYPointWidget(pElement, this);
    _ptW->setLayoutSpacing(0, 0);

    _pPointCalculLayout->addWidget(_ptW, wLn++, 0);

    updateContent();
}

TYPointCalculWidget::~TYPointCalculWidget()
{
}

void TYPointCalculWidget::updateContent()
{

    _elmW->updateContent();
    _ptW->updateContent();
    _ptW->disableZ();

    _lineEditNomSpectre->setText(getElement()->getSpectre()->getName());

    int bouton = getElement()->etat() ? 1 : 0;
    _buttonGroupState->button(bouton)->setChecked(true);

}

void TYPointCalculWidget::apply()
{
    _elmW->apply();
    _ptW->apply();
    getElement()->setEtat(_buttonGroupState->checkedId());
    emit modified();
}

void TYPointCalculWidget::changeCalcul(int calcul)
{
    _lineEditNomSpectre->setText(getElement()->getSpectre()->getName());

    int bouton = getElement()->etat() ? 1 : 0;
    _buttonGroupState->button(bouton)->setChecked(true);
}

void TYPointCalculWidget::editSpectre()
{
    LPTYSpectre sp = getElement()->getSpectre();
    bool bSave = sp->getIsReadOnly();

    sp->setIsReadOnly(true);

    //int ret = sp->edit(this);
    sp->edit(this);

    sp->setIsReadOnly(bSave);
}

//void TYPointCalculWidget::showResultatTreeDialog()
//{
//  TYResultatTreeDialog * pDlg = new TYResultatTreeDialog(this);
//  TYResultat * pResultat = NULL;
//  TYCalcul * pCalcul = NULL;
//
//  if (TYMaillage::safeDownCast(getElement()->getParent())) {
//      pCalcul = TYCalcul::safeDownCast(getElement()->getParent()->getParent());
//  } else {
//      pCalcul = TYCalcul::safeDownCast(getElement()->getParent());
//  }
//
//  if (pCalcul) {
//      pResultat = pCalcul->getResultat();
//  }
//
//  if (pResultat) {
//      pDlg->set(pResultat, getElement());
//      pDlg->exec();
//  }
//}
