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
 * \file TYReseauTransportWidget.cpp
 * \brief Outil IHM pour un reseau de transport
 */

//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/infrastructure/TYReseauTransport.h"
#include "Tympan/gui/widgets/TYAcousticLineWidget.h"
#include "TYReseauTransportWidget.h"

#define TR(id) OLocalizator::getString("TYReseauTransportWidget", (id))


TYReseauTransportWidget::TYReseauTransportWidget(TYReseauTransport* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{
    QString num;


    _elmW = new TYAcousticLineWidget(pElement, this);

    resize(300, 610);
    setWindowTitle(TR("id_caption"));
    _reseauTransportLayout = new QGridLayout();
    setLayout(_reseauTransportLayout);

    _reseauTransportLayout->addWidget(_elmW, 0, 0);

    _groupBox = new QGroupBox(this);
    _groupBox->setTitle(TR(""));
    _groupBoxLayout = new QGridLayout();
    _groupBox->setLayout(_groupBoxLayout);

    _lineEditNbBrins = new QLineEdit(_groupBox);
    _lineEditNbBrins->setText(num.setNum(getElement()->getNbBrins()));
    _groupBoxLayout->addWidget(_lineEditNbBrins, 0, 1);

    _labelNbBrins = new QLabel(_groupBox);
    _labelNbBrins->setText(TR("id_nbbrins_label"));
    _groupBoxLayout->addWidget(_labelNbBrins, 0, 0);

    _labelTension = new QLabel(_groupBox);
    _labelTension->setText(TR("id_tension_label"));
    _groupBoxLayout->addWidget(_labelTension, 1, 0);

    _lineEditTension = new QLineEdit(_groupBox);
    _lineEditTension->setText(num.setNum(getElement()->getTension(), 'f', 2));
    _groupBoxLayout->addWidget(_lineEditTension, 1, 1);
    QLabel* pUnitTension = new QLabel(_groupBox);
    pUnitTension->setText(TR("id_unite_tension"));
    _groupBoxLayout->addWidget(pUnitTension, 1, 2);

    _labelPuissance = new QLabel(_groupBox);
    _labelPuissance->setText(TR("id_puissance_label"));
    _groupBoxLayout->addWidget(_labelPuissance, 2, 0);

    _lineEditPuissance = new QLineEdit(_groupBox);
    _lineEditPuissance->setText(num.setNum(getElement()->getPuissance(), 'f', 2));
    _groupBoxLayout->addWidget(_lineEditPuissance, 2, 1);
    QLabel* pUnitPuissance = new QLabel(_groupBox);
    pUnitPuissance->setText(TR("id_unite_puissance"));
    _groupBoxLayout->addWidget(pUnitPuissance, 2, 2);

    _lineEditHautMoy = new QLineEdit(_groupBox);
    _lineEditHautMoy->setText(num.setNum(getElement()->getHauteurMoyenne(), 'f', 2));
    _groupBoxLayout->addWidget(_lineEditHautMoy, 3, 1);
    QLabel* pUnitHautMoy = new QLabel(_groupBox);
    pUnitHautMoy->setText(TR("id_unite_hauteurmoyenne"));
    _groupBoxLayout->addWidget(pUnitHautMoy, 3, 2);

    _labelHautMoy = new QLabel(_groupBox);
    _labelHautMoy->setText(TR("id_hautmoy_label"));
    _groupBoxLayout->addWidget(_labelHautMoy, 3, 0);

    _reseauTransportLayout->addWidget(_groupBox, 1, 0);
}

TYReseauTransportWidget::~TYReseauTransportWidget()
{
}

void TYReseauTransportWidget::updateContent()
{
    QString num;

    _elmW->updateContent();

    _lineEditNbBrins->setText(num.setNum(getElement()->getNbBrins()));
    _lineEditTension->setText(num.setNum(getElement()->getTension(), 'f', 2));
    _lineEditPuissance->setText(num.setNum(getElement()->getPuissance(), 'f', 2));
    _lineEditHautMoy->setText(num.setNum(getElement()->getHauteurMoyenne(), 'f', 2));
}

void TYReseauTransportWidget::apply()
{
    _elmW->apply();

    getElement()->setNbBrins(_lineEditNbBrins->text().toInt());
    getElement()->setTension(_lineEditTension->text().toDouble());
    getElement()->setPuissance(_lineEditPuissance->text().toDouble());
    getElement()->setHauteurMoyenne(_lineEditHautMoy->text().toDouble());

    emit modified();
}
