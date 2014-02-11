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
 * \file TYDalleWidget.cpp
 * \brief Outil IHM pour une dalle
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYDalle.h"
#include <QCheckBox>
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>


#define TR(id) OLocalizator::getString("TYDalleWidget", (id))


TYDalleWidget::TYDalleWidget(TYDalle* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{

    _accPolygonW = new TYAcousticPolygonWidget(pElement, this);
    _accPolygonW->hideMat();
    //  _accPolygonW->setLayoutSpacing(0, 0);

    resize(300, 680);
    setWindowTitle(TR("id_caption"));
    _dalleLayout = new QGridLayout();
    setLayout(_dalleLayout);

    _dalleLayout->addWidget(_accPolygonW, 1, 0);

    _groupBoxParoi = new QGroupBox(this);
    _groupBoxParoi->setTitle(TR("id_paroi"));
    _groupBoxParoiLayout = new QGridLayout();
    _groupBoxParoi->setLayout(_groupBoxParoiLayout);

    _lineEditNomParoi = new QLineEdit(_groupBoxParoi);
    _lineEditNomParoi->setEnabled(FALSE);
    _groupBoxParoiLayout->addWidget(_lineEditNomParoi, 0, 0);

    _pushButtonParoi = new QPushButton(_groupBoxParoi);
    _pushButtonParoi->setText(TR("id_proprietes_button"));
    _groupBoxParoiLayout->addWidget(_pushButtonParoi, 0, 1);

    _labeUseEtage = new QLabel(_groupBoxParoi);
    _labeUseEtage->setText(TR("id_useEtage_label"));
    _checkBoxUseEtage = new QCheckBox(_groupBoxParoi);
    _checkBoxUseEtage->setText(TR(""));
    _groupBoxParoiLayout->addWidget(_labeUseEtage, 1, 0);
    _groupBoxParoiLayout->addWidget(_checkBoxUseEtage, 1, 1);

    _dalleLayout->addWidget(_groupBoxParoi, 3, 0);

    connect(_pushButtonParoi, SIGNAL(clicked()), this, SLOT(editParoi()));
    connect(_checkBoxUseEtage, SIGNAL(clicked()), this, SLOT(updateUseParoi()));

    updateContent();
}

TYDalleWidget::~TYDalleWidget()
{
}

void TYDalleWidget::updateContent()
{
    _accPolygonW->updateContent();

    _lineEditNomParoi->setText(getElement()->getParoi()->getName());
    _checkBoxUseEtage->setChecked(!(getElement()->isParoiLocked()));

    updateUseParoi();
}

void TYDalleWidget::apply()
{
    _accPolygonW->apply();
    getElement()->setParoiLocked(!(_checkBoxUseEtage->isChecked()));

    emit modified();
}

void TYDalleWidget::editParoi()
{
    int ret = getElement()->getParoi()->edit(this);

    if (ret == QDialog::Accepted)
    {
        _lineEditNomParoi->setText(getElement()->getParoi()->getName());
    }
}

void TYDalleWidget::updateUseParoi()
{
    _pushButtonParoi->setEnabled(!(_checkBoxUseEtage->isChecked()));
}
