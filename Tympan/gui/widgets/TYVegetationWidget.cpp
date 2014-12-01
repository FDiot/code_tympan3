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
#include <qcheckbox.h>
#include <qcombobox.h>

#include "TYVegetationWidget.h"

#define TR(id) OLocalizator::getString("TYVegetationWidget", (id))


TYVegetationWidget::TYVegetationWidget(TYVegetation* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{
    QString num;

    resize(300, 200);
    setWindowTitle(TR("id_caption"));
    _vegetationLayout = new QGridLayout();
    setLayout(_vegetationLayout);

//    _elmW = new TYElementWidget(pElement, this);


    _comboBoxSelectVegeName = new QComboBox(this);
    _comboBoxSelectVegeName->setEditable(true);

    _vegetationLayout->addWidget(_comboBoxSelectVegeName, 0, 0);

    _groupBox = new QGroupBox(this);
    _groupBox->setTitle(TR(""));
    _groupBoxLayout = new QGridLayout();
    _groupBox->setLayout(_groupBoxLayout);

    // Gestion du feuillage
    _labelFoliageActive = new QLabel(_groupBox);
    _labelFoliageActive->setText(TR("id_foliageactivelabel_label"));
    _groupBoxLayout->addWidget(_labelFoliageActive, 0, 0);
    _checkBoxFoliageActive = new QCheckBox(_groupBox);
    _groupBoxLayout->addWidget(_checkBoxFoliageActive, 0, 1);

    // Hauteur de la hauteur de la végétation
    _labelHauteur = new QLabel(_groupBox);
    _labelHauteur->setText(TR("id_hauteur_label"));
    _groupBoxLayout->addWidget(_labelHauteur, 1, 0);

    _lineEditHauteur = new QLineEdit(_groupBox);
    _groupBoxLayout->addWidget(_lineEditHauteur, 1, 1);
    QLabel* pUnitHauteur = new QLabel(_groupBox);
    pUnitHauteur->setText(TR("id_unite_hauteur"));
    _groupBoxLayout->addWidget(pUnitHauteur, 1, 2);

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
}

TYVegetationWidget::~TYVegetationWidget()
{
}

void TYVegetationWidget::updateContent()
{
    if ( getElement() )
    {
//        _elmW->updateContent();

        updateComboVegetation(); // Remplit le combobox

        _checkBoxFoliageActive->setChecked( getElement()->getFoliageStatus() );
        _lineEditHauteur->setText(QString().setNum(getElement()->getHauteur(), 'f', 2));
        _lineEditNomSpectreAtt->setText(getElement()->getSpectreAtt()->getName());
    }
}

void TYVegetationWidget::apply()
{
    if ( getElement() )
    {
//        _elmW->apply();

        getElement()->setName(_comboBoxSelectVegeName->itemText(_comboBoxSelectVegeName->currentIndex()));
        getElement()->setFoliageStatus( _checkBoxFoliageActive->isChecked() );
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

void TYVegetationWidget::updateComboVegetation()
{
    _comboBoxSelectVegeName->clear();
    for(unsigned int i=0; i<5; i++)
    {
        _comboBoxSelectVegeName->insertItem(i, TYVegetation::_vegeName[i]);
    }

    _comboBoxSelectVegeName->setCurrentIndex( TYVegetation::getIndexVegetation(getElement()->getName()) );
}

