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
*/


//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/material/TYSol.h"
#include "TYSolWidget.h"
#include "Tympan/gui/widgets/TYSolResistanceDialog.h"

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

	_lineEditEcartType = new QLineEdit(_groupBox);
	_groupBoxLayout->addWidget(_lineEditEcartType,2,1);
	QLabel* pUnitEcart = new QLabel(_groupBox);
	pUnitEcart->setText(TR("id_unite_ecarttype"));
	_groupBoxLayout->addWidget(pUnitEcart,2,2);

	_lineEditLongueur = new QLineEdit(_groupBox);
	_groupBoxLayout->addWidget(_lineEditLongueur,3,1);
	QLabel* pUnitLong = new QLabel(_groupBox);
	pUnitLong->setText(TR("id_unite_longueur"));
	_groupBoxLayout->addWidget(pUnitLong,3,2);

	_pushButtonResistivite = new QPushButton(_groupBox);
	_pushButtonResistivite->setText(TR("id_edit_resistivite"));
	_groupBoxLayout->addWidget(_pushButtonResistivite, 0, 3);

	_labelEpaisseur = new QLabel(_groupBox);
	_labelEpaisseur->setText(TR("id_epaisseur_label"));
	_groupBoxLayout->addWidget(_labelEpaisseur, 1, 0);

	_labelResistivite = new QLabel(_groupBox);
	_labelResistivite->setText(TR("id_resistivite_label"));
	_groupBoxLayout->addWidget(_labelResistivite, 0, 0);

	_labelEcartType = new QLabel(_groupBox);
	_labelEcartType->setText(TR("id_ecarttype_label"));
	_groupBoxLayout->addWidget(_labelEcartType,2,0);

	_labelLongueur = new QLabel(_groupBox);
	_labelLongueur->setText(TR("id_longueur_label"));
	_groupBoxLayout->addWidget(_labelLongueur,3,0);

	_solLayout->addWidget(_groupBox, 1, 0);

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
	_lineEditEcartType->setText(num.setNum(getElement()->getEcartType(),'f',2));
	_lineEditLongueur->setText(num.setNum(getElement()->getLongueur(),'f',2));
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

	double ecarttype = _lineEditEcartType->text().toDouble();
	ecarttype = ecarttype < 0 ? 0 : ecarttype;
	getElement()->setEcartType(ecarttype);

	double longueur = _lineEditLongueur->text().toDouble();
	longueur = longueur <=0 ? 0.0001: longueur;
	getElement()->setLongueur(longueur);

	emit modified();
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


