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
 * \file TYDimensionDialog.cpp
 * \brief Widget pour l'edition des dimensions des volumes
 *
 */


//Added by qt3to4:
#include <QGridLayout>
#include <QBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

#include "Tympan/models/common/3d.h"
#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/geoacoustic/TYAcousticVolume.h"
#include "Tympan/models/business/geoacoustic/TYAcousticBox.h"
#include "Tympan/models/business/geoacoustic/TYAcousticCylinder.h"
#include "Tympan/models/business/geoacoustic/TYAcousticSemiCylinder.h"
#include "Tympan/gui/widgets/TYIncludesQT.h"
#include "Tympan/gui/widgets/TYElementWidget.h"

#include "TYDimensionDialog.h"

#define TR(id) OLocalizator::getString("TYDimensionDialog", (id))


TYDimensionDialog::TYDimensionDialog(TYAcousticVolume* pElement, QWidget* _pParent /*=NULL*/):
    QDialog(_pParent)
{
    Q_ASSERT(pElement);
    _pElement = pElement;

    resize(300, 174);
    setWindowTitle(TR("id_caption"));

    QGridLayout* pLayout = new QGridLayout();
    setLayout(pLayout);

    QBoxLayout* pEditLayout = new QHBoxLayout();
    pEditLayout->setMargin(10);
    pLayout->addLayout(pEditLayout, 0, 1);

    _pXLineEdit = nullptr;
    _pYLineEdit = nullptr;
    _pZLineEdit = nullptr;
    _pDiamLineEdit = nullptr;
    _pHauteurLineEdit = nullptr;

    if (_pElement->isA("TYAcousticBox"))
    {
        // Size X
        QLabel* pXLabelName = new QLabel(this);
        pXLabelName->setText(TR("id_x_label"));
        pEditLayout->addWidget(pXLabelName);
        _pXLineEdit = new QLineEdit(this);
        _pXLineEdit->setFixedWidth(60);
        pEditLayout->addWidget(_pXLineEdit);

        // Size Y
        pEditLayout->addSpacing(10);
        QLabel* pYLabelName = new QLabel(this);
        pYLabelName->setText(TR("id_y_label"));
        pEditLayout->addWidget(pYLabelName);
        _pYLineEdit = new QLineEdit(this);
        _pYLineEdit->setFixedWidth(60);
        pEditLayout->addWidget(_pYLineEdit);

        // Size Z
        pEditLayout->addSpacing(10);
        QLabel* pZLabelName = new QLabel(this);
        pZLabelName->setText(TR("id_z_label"));
        pEditLayout->addWidget(pZLabelName);
        _pZLineEdit = new QLineEdit(this);
        _pZLineEdit->setFixedWidth(60);
        pEditLayout->addWidget(_pZLineEdit);
    }
    else if (_pElement->isA("TYAcousticCylinder") || _pElement->isA("TYAcousticSemiCylinder"))
    {
        // Diametre
        QLabel* pDiamLabelName = new QLabel(this);
        pDiamLabelName->setText(TR("id_diameter_label"));
        pEditLayout->addWidget(pDiamLabelName);
        _pDiamLineEdit = new QLineEdit(this);
        _pDiamLineEdit->setFixedWidth(60);
        pEditLayout->addWidget(_pDiamLineEdit);

        // Hauteur
        pEditLayout->addSpacing(10);
        QLabel* pHauteurLabelName = new QLabel(this);
        pHauteurLabelName->setText(TR("id_hauteur_label"));
        pEditLayout->addWidget(pHauteurLabelName);
        _pHauteurLineEdit = new QLineEdit(this);
        _pHauteurLineEdit->setFixedWidth(60);
        pEditLayout->addWidget(_pHauteurLineEdit);
    }

    QBoxLayout* pBtnLayout = new QHBoxLayout();
    pLayout->addLayout(pBtnLayout, 1, 1);

    pBtnLayout->addStretch(1);

    QPushButton* pButtonOK = new QPushButton(TR("id_ok_btn"), this);
    pButtonOK->setDefault(true);
    QObject::connect(pButtonOK, SIGNAL(clicked()), this, SLOT(apply()));
    pBtnLayout->addWidget(pButtonOK);

    QPushButton* pButtonCancel = new QPushButton(TR("id_cancel_btn"), this);
    pButtonCancel->setShortcut(Qt::Key_Escape);
    QObject::connect(pButtonCancel, SIGNAL(clicked()), this, SLOT(reject()));
    pBtnLayout->addWidget(pButtonCancel);

    updateContent();
}

TYDimensionDialog::~TYDimensionDialog()
{
}

void TYDimensionDialog::updateContent()
{
    if (_pElement->isA("TYAcousticBox"))
    {
        TYAcousticBox* pAccBox = dynamic_cast<TYAcousticBox*>(_pElement);
        float sizeX, sizeY, sizeZ;
        pAccBox->getDimension(sizeX, sizeY, sizeZ);

		_pXLineEdit->setText(QString().setNum(sizeX, 'f', 2));
        _pYLineEdit->setText(QString().setNum(sizeY, 'f', 2));
        _pZLineEdit->setText(QString().setNum(sizeZ, 'f', 2));
	}
	else if (_pElement->isA("TYAcousticCylinder"))
	{
		float diameter = dynamic_cast<TYAcousticCylinder*>(_pElement)->getDiameter();
        float hauteur = dynamic_cast<TYAcousticCylinder*>(_pElement)->getHauteur();
        _pDiamLineEdit->setText(QString().setNum(diameter, 'f', 2));
		_pHauteurLineEdit->setText(QString().setNum(hauteur, 'f', 2));
	}
	else if (_pElement->isA("TYAcousticSemiCylinder"))
	{
		float diameter = dynamic_cast<TYAcousticSemiCylinder*>(_pElement)->getDiameter();
        float hauteur = dynamic_cast<TYAcousticSemiCylinder*>(_pElement)->getHauteur();
        _pDiamLineEdit->setText(QString().setNum(diameter, 'f', 2));
		_pHauteurLineEdit->setText(QString().setNum(hauteur, 'f', 2));
	}
}

void TYDimensionDialog::apply()
{
	if (_pElement->isA("TYAcousticBox"))
	{
		float sizeX = _pXLineEdit->text().toDouble();
		float sizeY = _pYLineEdit->text().toDouble();
		float sizeZ = _pZLineEdit->text().toDouble();

		dynamic_cast<TYAcousticBox*>(_pElement)->setDimension(sizeX, sizeY, sizeZ);
	}
	else if (_pElement->isA("TYAcousticCylinder"))
	{
		double diameter = _pDiamLineEdit->text().toDouble();
		double hauteur = _pHauteurLineEdit->text().toDouble();

		dynamic_cast<TYAcousticCylinder*>(_pElement)->setDiameter(diameter);
		dynamic_cast<TYAcousticCylinder*>(_pElement)->setHauteur(hauteur);
	}
	else if (_pElement->isA("TYAcousticSemiCylinder"))
	{
		double diameter = _pDiamLineEdit->text().toDouble();
		double hauteur = _pHauteurLineEdit->text().toDouble();

		dynamic_cast<TYAcousticSemiCylinder*>(_pElement)->setDiameter(diameter);
		dynamic_cast<TYAcousticSemiCylinder*>(_pElement)->setHauteur(hauteur);
	}
	
    QDialog::accept();
}
