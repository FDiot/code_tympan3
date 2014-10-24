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
 * \file TYVegetationWidget.cpp
 * \brief outil IHM pour une vegetation
 *
 *
 */

#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/material/TYVegetation.h"
#include <QGridLayout>
#include <QLabel>

#include "TYVegetationWidget.h"

#define TR(id) OLocalizator::getString("TYVegetationWidget", (id))


TYVegetationWidget::TYVegetationWidget(TYVegetation* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{
    QString num;

    _elmW = new TYElementWidget(pElement, this);

    resize(300, 200);
    setWindowTitle(TR("id_caption"));
    _vegetationLayout = new QGridLayout();
    setLayout(_vegetationLayout);

    _vegetationLayout->addWidget(_elmW, 0, 0);

    _groupBox = new QGroupBox(this);
    _groupBox->setTitle(TR(""));
    _groupBoxLayout = new QGridLayout();
    _groupBox->setLayout(_groupBoxLayout);

    _labelHauteur = new QLabel(_groupBox);
    _labelHauteur->setText(TR("id_hauteur_label"));
    _groupBoxLayout->addWidget(_labelHauteur, 0, 0);

    _lineEditHauteur = new QLineEdit(_groupBox);
    _groupBoxLayout->addWidget(_lineEditHauteur, 0, 1);
    QLabel* pUnitHauteur = new QLabel(_groupBox);
    pUnitHauteur->setText(TR("id_unite_hauteur"));
    _groupBoxLayout->addWidget(pUnitHauteur, 0, 2);

    _vegetationLayout->addWidget(_groupBox, 1, 0);

    _groupBoxSpectreAtt = new QGroupBox(this);
    _groupBoxSpectreAtt->setTitle(TR("id_spectreatt"));
    _groupBoxSpectreAttLayout = new QGridLayout();
    _groupBoxSpectreAtt->setLayout(_groupBoxSpectreAttLayout);

    _lineEditNomSpectreAtt = new QLineEdit(_groupBoxSpectreAtt);
    _groupBoxSpectreAttLayout->addWidget(_lineEditNomSpectreAtt, 0, 0);

    _pushButtonSpectreAtt = new QPushButton(_groupBoxSpectreAtt);
    _pushButtonSpectreAtt->setText(TR("id_proprietes_button"));
    _groupBoxSpectreAttLayout->addWidget(_pushButtonSpectreAtt, 0, 1);

    _vegetationLayout->addWidget(_groupBoxSpectreAtt, 2, 0);

    connect(_pushButtonSpectreAtt, SIGNAL(clicked()), this, SLOT(editSpectre()));

    updateContent();
}

TYVegetationWidget::~TYVegetationWidget()
{
}

void TYVegetationWidget::updateContent()
{
    if ( getElement() )
    {
        _elmW->updateContent();

        _lineEditHauteur->setText(QString().setNum(getElement()->getHauteur(), 'f', 2));
        _lineEditNomSpectreAtt->setText(getElement()->getSpectreAtt()->getName());
    }
}

void TYVegetationWidget::apply()
{
    if ( getElement() )
    {
        _elmW->apply();

        getElement()->setHauteur(_lineEditHauteur->text().toDouble());

        emit modified();
    }
}

void TYVegetationWidget::editSpectre()
{
    int ret = getElement()->getSpectreAtt()->edit(this);

    if (ret == QDialog::Accepted)
    {
        _lineEditNomSpectreAtt->setText(getElement()->getSpectreAtt()->getName());
    }
}

