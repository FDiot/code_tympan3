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
 * \file TYSolWidget.cpp
 * \brief outil IHM pour un sol
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/models/business/material/TYSol.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

#define TR(id) OLocalizator::getString("TYSolWidget", (id))


TYSolWidget::TYSolWidget(TYSol* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{
    _elmW = new TYElementWidget(pElement, this);

    resize(300, 300);
    setWindowTitle(TR("id_caption"));
    _solLayout = new QGridLayout();
    setLayout(_solLayout);

    _solLayout->addWidget(_elmW, 0, 0);

    _groupBox = new QGroupBox(this);
    _groupBox->setTitle(TR(""));
    _groupBoxLayout = new QGridLayout();
    _groupBox->setLayout(_groupBoxLayout);

    _labelVegetActive = new QLabel(_groupBox);
    _labelVegetActive->setText(TR("id_vegetactive_label"));
    _groupBoxLayout->addWidget(_labelVegetActive, 2, 0);

    _lineEditEpaisseur = new QLineEdit(_groupBox);
    _groupBoxLayout->addWidget(_lineEditEpaisseur, 1, 1);
    QLabel* pUnitEpais = new QLabel(_groupBox);
    pUnitEpais->setText(TR("id_unite_epaisseur"));
    _groupBoxLayout->addWidget(pUnitEpais, 1, 2);

    _lineEditResistivite = new QLineEdit(_groupBox);
    _groupBoxLayout->addWidget(_lineEditResistivite, 0, 1);
    QLabel* pUnitResis = new QLabel(_groupBox);
    pUnitResis->setText(TR("id_unite_resistivite"));
    _groupBoxLayout->addWidget(pUnitResis, 0, 2);

    _pushButtonResistivite = new QPushButton(_groupBox);
    _pushButtonResistivite->setText(TR("id_edit_resistivite"));
    _groupBoxLayout->addWidget(_pushButtonResistivite, 0, 3);

    _labelEpaisseur = new QLabel(_groupBox);
    _labelEpaisseur->setText(TR("id_epaisseur_label"));
    _groupBoxLayout->addWidget(_labelEpaisseur, 1, 0);

    _checkBoxVegetActive = new QCheckBox(_groupBox);
    _groupBoxLayout->addWidget(_checkBoxVegetActive, 2, 1);

    _labelResistivite = new QLabel(_groupBox);
    _labelResistivite->setText(TR("id_resistivite_label"));
    _groupBoxLayout->addWidget(_labelResistivite, 0, 0);

    _solLayout->addWidget(_groupBox, 1, 0);

    _groupBoxVegetation = new QGroupBox(this);
    _groupBoxVegetation->setTitle(TR("id_vegetation"));
    _groupBoxVegetationLayout = new QGridLayout();
    _groupBoxVegetation->setLayout(_groupBoxVegetationLayout);

    _lineEditNomVegetation = new QLineEdit(_groupBoxVegetation);
    _lineEditNomVegetation->setEnabled(FALSE);
    _groupBoxVegetationLayout->addWidget(_lineEditNomVegetation, 0, 0);

    _pushButtonVegetation = new QPushButton(_groupBoxVegetation);
    _pushButtonVegetation->setText(TR("id_proprietes_button"));
    _groupBoxVegetationLayout->addWidget(_pushButtonVegetation, 0, 1);

    _solLayout->addWidget(_groupBoxVegetation, 2, 0);

    connect(_pushButtonVegetation, SIGNAL(clicked()), this, SLOT(editVegetation()));
    connect(_checkBoxVegetActive, SIGNAL(clicked()), this, SLOT(useVegetation()));
    connect(_pushButtonResistivite, SIGNAL(clicked()), this, SLOT(editResistivite()));

    updateContent();
}

TYSolWidget::~TYSolWidget()
{
}

void TYSolWidget::updateContent()
{
    QString num;

    _elmW->updateContent();

    _lineEditResistivite->setText(num.setNum(getElement()->getResistivite(), 'f', 2));
    _lineEditEpaisseur->setText(num.setNum(getElement()->getEpaisseur(), 'f', 2));

    bool bVegActive = getElement()->getVegetActive();
    _checkBoxVegetActive->setChecked(bVegActive);
    _groupBoxVegetation->setEnabled(bVegActive);

    if (bVegActive)
    {
        TYVegetation* pVeg = getElement()->getVegetation();
        if (pVeg) { _lineEditNomVegetation->setText(pVeg->getName()); }
    }
}

void TYSolWidget::apply()
{
    _elmW->apply();

    // On ne peut pas rentrer une resistivite <= 0 (mini 1 kRayl)
    double resistivite = _lineEditResistivite->text().toDouble() ;
    resistivite = resistivite <= 1.0 ? 1.0 : resistivite;
    getElement()->setResistivite(resistivite);

    // On ne peut avoir une epaisseur <= 0 (mini 1 cm)
    double epaisseur = _lineEditEpaisseur->text().toDouble();
    epaisseur = epaisseur <= 0.01 ? 0.01 : epaisseur;
    getElement()->setEpaisseur(epaisseur);

    getElement()->useVegetation(_checkBoxVegetActive->isChecked());

    emit modified();
}

void TYSolWidget::editVegetation()
{
    int ret = getElement()->getVegetation()->edit(this);

    if (ret == QDialog::Accepted)
    {
        _lineEditNomVegetation->setText(getElement()->getVegetation()->getName());
    }
}

void TYSolWidget::useVegetation()
{
    getElement()->useVegetation(_checkBoxVegetActive->isChecked());
    _groupBoxVegetation->setEnabled(_checkBoxVegetActive->isChecked());
}

void TYSolWidget::editResistivite()
{
    TYSolResistanceDialog* pDialog = new TYSolResistanceDialog(this, getElement()->getResistivite());

    int ret = pDialog->exec();

    if (ret == QDialog::Accepted)
    {
        // Mise a jour du widget associe a la resistivite
        _lineEditResistivite->setText(QString().setNum(pDialog->getResistivite(), 'f', 2));
    }
}


