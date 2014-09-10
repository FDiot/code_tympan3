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
 * \file TYAtmosphereWidget.cpp
 * \brief outil IHM pour l'objet atmosphere
 *
 *
 */



#include <qmessagebox.h>
#include "Tympan/models/business/acoustic/TYSpectre.h"
#include "Tympan/models/business/TYCalcul.h"
#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/material/TYAtmosphere.h"
#include <qcombobox.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>


#include "TYAtmosphereWidget.h"

#define TR(id) OLocalizator::getString("TYAtmosphereWidget", (id))


TYAtmosphereWidget::TYAtmosphereWidget(TYAtmosphere* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{
    QString num;


    _elmW = new TYElementWidget(pElement, this);

    resize(300, 300);
    setWindowTitle(TR("id_caption"));
    _atmosphereLayout = new QGridLayout();
    setLayout(_atmosphereLayout);
    _atmosphereLayout->addWidget(_elmW, 0, 0);

    _groupBox = new QGroupBox(this);
    _groupBox->setTitle(TR(""));
    _groupBoxLayout = new QGridLayout();
    _groupBox->setLayout(_groupBoxLayout);

    // Description des conditions atmospheriques (pression, temperature, hygrometrie)
    _labelPression = new QLabel(_groupBox);
    _labelPression->setText(TR("id_pression_label"));
    _groupBoxLayout->addWidget(_labelPression, 0, 0);
    _lineEditPression = new QLineEdit(_groupBox);
    _groupBoxLayout->addWidget(_lineEditPression, 0, 1);
    QLabel* pUnitPression = new QLabel(_groupBox);
    pUnitPression->setText(TR("id_unite_pression"));
    _groupBoxLayout->addWidget(pUnitPression, 0, 2);

    _labelTemperature = new QLabel(_groupBox);
    _labelTemperature->setText(TR("id_temperature_label"));
    _groupBoxLayout->addWidget(_labelTemperature, 1, 0);

    _lineEditTemperature = new QLineEdit(_groupBox);
    _groupBoxLayout->addWidget(_lineEditTemperature, 1, 1);
    QLabel* pUnitTemp = new QLabel(_groupBox);
    pUnitTemp->setText(TR("id_unite_temp"));
    _groupBoxLayout->addWidget(pUnitTemp, 1, 2);

    _labelHygrometrie = new QLabel(_groupBox);
    _labelHygrometrie->setText(TR("id_hygrometrie_label"));
    _groupBoxLayout->addWidget(_labelHygrometrie, 2, 0);

    _lineEditHygrometrie = new QLineEdit(_groupBox);
    _lineEditHygrometrie->setText(num.setNum(getElement()->getHygrometrie(), 'f', 2));
    _groupBoxLayout->addWidget(_lineEditHygrometrie, 2, 1);
    QLabel* pUnitHygro = new QLabel(_groupBox);
    pUnitHygro->setText(TR("id_unite_hygro"));
    _groupBoxLayout->addWidget(pUnitHygro, 2, 2);

    // Affichage de la vitesse du son
    _labelVitSon = new QLabel(_groupBox);
    _labelVitSon->setText(TR("id_celerite_label"));
    _groupBoxLayout->addWidget(_labelVitSon, 3, 0);

    _lineEditVitSon = new QLineEdit(_groupBox);
    _groupBoxLayout->addWidget(_lineEditVitSon, 3, 1);
    QLabel* pUnitCelerite = new QLabel(_groupBox);
    pUnitCelerite->setText(TR("id_unite_celerite"));
    _groupBoxLayout->addWidget(pUnitCelerite, 3, 2);

    _atmosphereLayout->addWidget(_groupBox, 1, 0);

    // Ajout du bouton pour visualiser le spectre d'attenuation
    _groupBoxSpectreAtt = new QGroupBox(this);
    _groupBoxSpectreAtt->setTitle(TR("id_spectreatt"));
    _groupBoxSpectreAttLayout = new QGridLayout();
    _groupBoxSpectreAtt->setLayout(_groupBoxSpectreAttLayout);

    _lineEditNomSpectre = new QLineEdit(_groupBoxSpectreAtt);
    _lineEditNomSpectre->setText(getElement()->getSpectreAtt().getName());
    _lineEditNomSpectre->setEnabled(FALSE);
    _groupBoxSpectreAttLayout->addWidget(_lineEditNomSpectre, 0, 0);

    _pushButtonSpectre = new QPushButton(_groupBoxSpectreAtt);
    _pushButtonSpectre->setText(TR("id_proprietes_button"));
    _groupBoxSpectreAttLayout->addWidget(_pushButtonSpectre, 0, 1);

    _atmosphereLayout->addWidget(_groupBoxSpectreAtt, 2, 0);

    connect(_lineEditTemperature, SIGNAL(lostFocus()), this, SLOT(changeCelerite()));
    connect(_pushButtonSpectre, SIGNAL(clicked()), this, SLOT(editSpectre()));

    updateContent();
}

TYAtmosphereWidget::~TYAtmosphereWidget()
{
}

void TYAtmosphereWidget::updateContent()
{
    QString num;

    _elmW->updateContent();

    _lineEditPression->setText(num.setNum(getElement()->getPression(), 'f', 2));
    _lineEditVitSon->setText(num.setNum(getElement()->getVitSon(), 'f', 2));
    _lineEditVitSon->setEnabled(false);

    _lineEditTemperature->setText(num.setNum(getElement()->getTemperature(), 'f', 2));
    _lineEditHygrometrie->setText(num.setNum(getElement()->getHygrometrie(), 'f', 2));
    _lineEditNomSpectre->setText(getElement()->getSpectreAtt().getName());
}

void TYAtmosphereWidget::apply()
{
    _elmW->apply();

    getElement()->setPression(_lineEditPression->text().toDouble());
    getElement()->setTemperature(_lineEditTemperature->text().toDouble());
    getElement()->setHygrometrie(_lineEditHygrometrie->text().toDouble());

    emit modified();
}

void TYAtmosphereWidget::changeCelerite()
{
    getElement()->setPression(_lineEditPression->text().toDouble());
    getElement()->setTemperature(_lineEditTemperature->text().toDouble());
    getElement()->setHygrometrie(_lineEditHygrometrie->text().toDouble());

    _lineEditVitSon->setText(QString().setNum(getElement()->getVitSon(), 'f', 2));
}

void TYAtmosphereWidget::editSpectre()
{
    // On augment la precision d'affichage car les valeurs sont faibles
    int oldPrecision = TYSpectre::getXMLPrecision();
    TYSpectre::setXMLPrecision(7);

    int ret = getElement()->getSpectreAtt().edit(this);

    // Affiche le nom du spectre si modifie
    if (ret == QDialog::Accepted)
    {
        _lineEditNomSpectre->setText(getElement()->getSpectreAtt().getName());
    }

    // On remet la precision d'affichage a la valeur anterieure
    TYSpectre::setXMLPrecision(oldPrecision);
}
