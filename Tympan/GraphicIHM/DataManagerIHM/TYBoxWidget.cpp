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
 * \file TYBoxWidget.cpp
 * \brief outil IHM pour une boite
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYBox.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>



#define TR(id) OLocalizator::getString("TYBoxWidget", (id))


TYBoxWidget::TYBoxWidget(TYBox* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{
    QString num;


    _elmW = new TYElementWidget(pElement, this);
    _colorW = new TYColorInterfaceWidget(pElement, this);

    resize(300, 240);
    setWindowTitle(TR("id_caption"));
    _boxLayout = new QGridLayout();
    setLayout(_boxLayout);

    _boxLayout->addWidget(_elmW, 0, 0);
    _boxLayout->addWidget(_colorW, 1, 0);

    _groupBox = new QGroupBox(this);
    _groupBox->setTitle(TR(""));
    _groupBoxLayout = new QGridLayout();
    _groupBox->setLayout(_groupBoxLayout);

    _labelSizeX = new QLabel(_groupBox);
    _labelSizeX->setText(TR("id_sizeX_label"));
    _groupBoxLayout->addWidget(_labelSizeX, 0, 0);

    _labelSizeY = new QLabel(_groupBox);
    _labelSizeY->setText(TR("id_sizeY_label"));
    _groupBoxLayout->addWidget(_labelSizeY, 1, 0);

    _labelSizeZ = new QLabel(_groupBox);
    _labelSizeZ->setText(TR("id_sizeZ_label"));
    _groupBoxLayout->addWidget(_labelSizeZ, 2, 0);

    _lineEditSizeX = new QLineEdit(_groupBox);
    _lineEditSizeX->setText(num.setNum(getElement()->_sizeX, 'f', 2));
    _groupBoxLayout->addWidget(_lineEditSizeX, 0, 1);
    QLabel* pUnitSizeX = new QLabel(_groupBox);
    pUnitSizeX->setText(TR("id_unite_size"));
    _groupBoxLayout->addWidget(pUnitSizeX, 0, 2);

    _lineEditSizeY = new QLineEdit(_groupBox);
    _lineEditSizeY->setText(num.setNum(getElement()->_sizeY, 'f', 2));
    _groupBoxLayout->addWidget(_lineEditSizeY, 1, 1);
    QLabel* pUnitSizeY = new QLabel(_groupBox);
    pUnitSizeY->setText(TR("id_unite_size"));
    _groupBoxLayout->addWidget(pUnitSizeY, 1, 2);

    _lineEditSizeZ = new QLineEdit(_groupBox);
    _lineEditSizeZ->setText(num.setNum(getElement()->_sizeZ, 'f', 2));
    _groupBoxLayout->addWidget(_lineEditSizeZ, 2, 1);
    QLabel* pUnitSizeZ = new QLabel(_groupBox);
    pUnitSizeZ->setText(TR("id_unite_size"));
    _groupBoxLayout->addWidget(pUnitSizeZ, 2, 2);

    _boxLayout->addWidget(_groupBox, 2, 0);

    _groupBoxPosition = new QGroupBox(this);
    _groupBoxPosition->setTitle(TR("id_position"));
    _groupBoxPositionLayout = new QGridLayout();
    _groupBoxPosition->setLayout(_groupBoxPositionLayout);

    _lineEditNomPosition = new QLineEdit(_groupBoxPosition);
    _lineEditNomPosition->setEnabled(FALSE);
    _lineEditNomPosition->setText(getElement()->getPosition().getName());
    _groupBoxPositionLayout->addWidget(_lineEditNomPosition, 0, 0);

    _pushButtonPosition = new QPushButton(_groupBoxPosition);
    _pushButtonPosition->setText(TR("id_proprietes_button"));
    _groupBoxPositionLayout->addWidget(_pushButtonPosition, 0, 1);

    _boxLayout->addWidget(_groupBoxPosition, 3, 0);

    connect(_pushButtonPosition, SIGNAL(clicked()), this, SLOT(editPosition()));
}


TYBoxWidget::~TYBoxWidget()
{
}

void TYBoxWidget::updateContent()
{
    QString num;

    _elmW->updateContent();
    _colorW->updateContent();

    _lineEditSizeX->setText(num.setNum(getElement()->_sizeZ, 'f', 2));
    _lineEditSizeY->setText(num.setNum(getElement()->_sizeZ, 'f', 2));
    _lineEditSizeZ->setText(num.setNum(getElement()->_sizeZ, 'f', 2));
    _lineEditNomPosition->setText(getElement()->getPosition().getName());
}

void TYBoxWidget::apply()
{
    _elmW->apply();
    _colorW->apply();

    getElement()->_sizeX = _lineEditSizeX->text().toDouble() ;
    getElement()->_sizeY = _lineEditSizeY->text().toDouble() ;
    getElement()->_sizeZ = _lineEditSizeZ->text().toDouble() ;

    emit modified();
}

void TYBoxWidget::editPosition()
{
    int ret = getElement()->getPosition().edit(this);

    if (ret == QDialog::Accepted)
    {
        _lineEditNomPosition->setText(getElement()->getPosition().getName());
    }
}
