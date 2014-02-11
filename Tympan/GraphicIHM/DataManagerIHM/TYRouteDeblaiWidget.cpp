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

/*
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYRouteDeblai.h"
#include <qcombobox.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>


#define TR(id) OLocalizator::getString("TYRouteDeblaiWidget", (id))


TYRouteDeblaiWidget::TYRouteDeblaiWidget(TYRouteDeblai* pElement, QWidget* _pParent):
    TYWidget(pElement, _pParent)
{
    QString num;


    _elmW = new TYAcousticLineWidget(pElement, this);

    resize(300, 720);
    setWindowTitle(TR("id_caption"));
    _routeLayout = new QGridLayout();
    setLayout(_routeLayout);

    _routeLayout->addWidget(_elmW, 0, 0);

    _groupBox = new QGroupBox(this);
    _groupBox->setTitle(TR(""));
    _groupBoxLayout = new QGridLayout();
    _groupBox->setLayout(_groupBoxLayout);

    _labelVitMoy = new QLabel(_groupBox);
    _labelVitMoy->setText(TR("id_vitmoyvl_label"));
    _groupBoxLayout->addWidget(_labelVitMoy, 0, 0);

    _lineEditVitMoy = new QLineEdit(_groupBox);
    _groupBoxLayout->addWidget(_lineEditVitMoy, 0, 1);
    QLabel* pUnitVitMoy = new QLabel(_groupBox);
    pUnitVitMoy->setText(TR("id_unite_vitessemoyenne"));
    _groupBoxLayout->addWidget(pUnitVitMoy, 0, 2);

    _comboBoxTrafic = new QComboBox(_groupBox);
    _groupBoxLayout->addWidget(_comboBoxTrafic, 1, 1, 1, 2);
    _comboBoxTrafic->insertItem(0, TR("id_fluide"));
    _comboBoxTrafic->insertItem(1, TR("id_pulse"));

    _routeLayout->addWidget(_groupBox, 1, 0);

    _groupBoxTraficJour = new QGroupBox(this);
    _groupBoxTraficJour->setTitle(TR("id_traficjour"));
    _groupBoxTraficJourLayout = new QGridLayout();
    _groupBoxTraficJour->setLayout(_groupBoxTraficJourLayout);

    _pLineEditDebitPLJour = new QLineEdit(_groupBoxTraficJour);
    _groupBoxTraficJourLayout->addWidget(_pLineEditDebitPLJour, 1, 1);
    QLabel* pUnitDebitPLJour = new QLabel(TR("id_unite_debit"), _groupBoxTraficJour);
    _groupBoxTraficJourLayout->addWidget(pUnitDebitPLJour, 1, 2);

    _pLineEditDebitVLJour = new QLineEdit(_groupBoxTraficJour);
    _groupBoxTraficJourLayout->addWidget(_pLineEditDebitVLJour, 0, 1);
    QLabel* pUnitDebitVLJour = new QLabel(TR("id_unite_debit"), _groupBoxTraficJour);
    _groupBoxTraficJourLayout->addWidget(pUnitDebitVLJour, 0, 2);

    QLabel* pLabelDebitPLJour = new QLabel(TR("id_debitpl_label"), _groupBoxTraficJour);
    _groupBoxTraficJourLayout->addWidget(pLabelDebitPLJour, 1, 0);
    QLabel* pLabelDebitVLJour = new QLabel(TR("id_debitvl_label"), _groupBoxTraficJour);
    _groupBoxTraficJourLayout->addWidget(pLabelDebitVLJour, 0, 0);

    _routeLayout->addWidget(_groupBoxTraficJour, 2, 0);

    _groupBoxTraficNuit = new QGroupBox(this);
    _groupBoxTraficNuit->setTitle(TR("id_traficnuit"));
    _groupBoxTraficNuitLayout = new QGridLayout();
    _groupBoxTraficNuit->setLayout(_groupBoxTraficNuitLayout);

    _pLineEditDebitPLNuit = new QLineEdit(_groupBoxTraficNuit);
    _groupBoxTraficNuitLayout->addWidget(_pLineEditDebitPLNuit, 1, 1);
    QLabel* pUnitDebitPLNuit = new QLabel(TR("id_unite_debit"),  _groupBoxTraficNuit);
    _groupBoxTraficNuitLayout->addWidget(pUnitDebitPLNuit, 1, 2);

    _pLineEditDebitVLNuit = new QLineEdit(_groupBoxTraficNuit);
    _groupBoxTraficNuitLayout->addWidget(_pLineEditDebitVLNuit, 0, 1);
    QLabel* pUnitDebitVLNuit = new QLabel(TR("id_unite_debit"), _groupBoxTraficNuit);
    _groupBoxTraficNuitLayout->addWidget(pUnitDebitVLNuit, 0, 2);

    QLabel* pLabelDebitPLNuit = new QLabel(TR("id_debitpl_label"), _groupBoxTraficNuit);
    _groupBoxTraficNuitLayout->addWidget(pLabelDebitPLNuit, 1, 0);
    QLabel* pLabelDebitVLNuit = new QLabel(TR("id_debitvl_label"), _groupBoxTraficNuit);
    _groupBoxTraficNuitLayout->addWidget(pLabelDebitVLNuit, 0, 0);

    _routeLayout->addWidget(_groupBoxTraficNuit, 3, 0);

    updateContent();
}

TYRouteDeblaiWidget::~TYRouteDeblaiWidget()
{
}

void TYRouteDeblaiWidget::updateContent()
{
    _elmW->updateContent();

    _lineEditVitMoy->setText(QString().setNum(getElement()->getVitMoy(), 'f', 2));
    _pLineEditDebitPLJour->setText(QString().setNum(getElement()->getTraficJour().getDebitPL()));
    _pLineEditDebitVLJour->setText(QString().setNum(getElement()->getTraficJour().getDebitVL()));
    _pLineEditDebitPLNuit->setText(QString().setNum(getElement()->getTraficNuit().getDebitPL()));
    _pLineEditDebitVLNuit->setText(QString().setNum(getElement()->getTraficNuit().getDebitVL()));
    // _comboBoxTrafic->setCurrentIndex(getElement()->getModeCalc()); // WIP replace with RoadFlowType
}

void TYRouteDeblaiWidget::apply()
{
    _elmW->apply();

    getElement()->setVitMoy(_lineEditVitMoy->text().toDouble());
    // getElement()->setModeCalc(_comboBoxTrafic->currentIndex()); // WIP replace with RoadFlowType
    getElement()->getTraficJour().setDebitPL(_pLineEditDebitPLJour->text().toDouble());
    getElement()->getTraficJour().setDebitVL(_pLineEditDebitVLJour->text().toDouble());
    getElement()->getTraficNuit().setDebitPL(_pLineEditDebitPLNuit->text().toDouble());
    getElement()->getTraficNuit().setDebitVL(_pLineEditDebitVLNuit->text().toDouble());

    emit modified();
}
