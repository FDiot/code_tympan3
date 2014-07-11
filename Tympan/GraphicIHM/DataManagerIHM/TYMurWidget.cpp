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
 * \file TYMurWidget.cpp
 * \brief outil IHM pour un mur
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/models/business/infrastructure/TYMur.h"
#include <QPushButton>
#include <QCheckBox>
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>


#define TR(id) OLocalizator::getString("TYMurWidget", (id))


TYMurWidget::TYMurWidget(TYMur* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{

    resize(300, 700);
    setWindowTitle(TR("id_caption"));
    _murLayout = new QGridLayout();
    setLayout(_murLayout);

    _elmW = new TYAcousticRectangleNodeWidget(pElement, this);
    _elmW->setLayoutSpacing(0, 0);
    _murLayout->addWidget(_elmW, 0, 0);

    QGroupBox* pGroupBoxParoi = new QGroupBox(this);
    pGroupBoxParoi->setTitle(TR("id_paroi"));
    QGridLayout* pGroupBoxParoiLayout = new QGridLayout();
    pGroupBoxParoi->setLayout(pGroupBoxParoiLayout);

    _lineEditNomParoi = new QLineEdit(pGroupBoxParoi);
    _lineEditNomParoi->setEnabled(false);
    pGroupBoxParoiLayout->addWidget(_lineEditNomParoi, 0, 0);

    _pPushButtonParoi = new QPushButton(pGroupBoxParoi);
    _pPushButtonParoi->setText(TR("id_proprietes_button"));
    pGroupBoxParoiLayout->addWidget(_pPushButtonParoi, 0, 1);

    _labeUseEtage = new QLabel(pGroupBoxParoi);
    _labeUseEtage->setText(TR("id_useEtage_label"));
    _checkBoxUseEtage = new QCheckBox(pGroupBoxParoi);
    _checkBoxUseEtage->setText(TR(""));
    pGroupBoxParoiLayout->addWidget(_labeUseEtage, 1, 0);
    pGroupBoxParoiLayout->addWidget(_checkBoxUseEtage, 1, 1);

    _murLayout->addWidget(pGroupBoxParoi, 1, 0);

    updateContent();

    connect(_pPushButtonParoi, SIGNAL(clicked()), this, SLOT(editParoi()));
    connect(_checkBoxUseEtage, SIGNAL(clicked()), this, SLOT(updateUseParoi()));
}

TYMurWidget::~TYMurWidget()
{
}

void TYMurWidget::updateContent()
{
    _elmW->updateContent();

    _lineEditNomParoi->setText(getElement()->getParoi()->getName());
    _checkBoxUseEtage->setChecked(!(getElement()->isParoiLocked()));
    updateUseParoi();
}

void TYMurWidget::apply()
{
    _elmW->apply();
    getElement()->setParoiLocked(!(_checkBoxUseEtage->isChecked()));

    emit modified();
}

void TYMurWidget::editParoi()
{
    int ret = getElement()->getParoi()->edit(this);

    if (ret == QDialog::Accepted)
    {
        _lineEditNomParoi->setText(getElement()->getParoi()->getName());
    }
}

void TYMurWidget::updateUseParoi()
{
    _pPushButtonParoi->setEnabled(!(_checkBoxUseEtage->isChecked()));
}
