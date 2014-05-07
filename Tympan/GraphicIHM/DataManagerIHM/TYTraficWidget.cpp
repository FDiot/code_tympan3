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
 * \file TYTraficWidget.cpp
 * \brief outil IHM pour un trafic
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYTrafic.h"

//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

#define TR(id) OLocalizator::getString("TYTraficWidget", (id))


TYTraficWidget::TYTraficWidget(TYTrafic* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{
    QString num;


    _elmW = new TYElementWidget(pElement, this);
    _lineEditDebitPL = new QLineEdit();
    _lineEditDebitPL->setText(num.setNum(getElement()->getDebitPL(), 'f', 2)) ;
    QLabel* pUnitDebitPL = new QLabel();
    pUnitDebitPL->setText(TR("id_unite_debit"));

    _lineEditDebitVL = new QLineEdit();
    _lineEditDebitVL->setText(num.setNum(getElement()->getDebitVL(), 'f', 2));
    QLabel* pUnitDebitVL = new QLabel();
    pUnitDebitVL->setText(TR("id_unite_debit"));

    _labelDebitPL = new QLabel();
    _labelDebitPL->setText(TR("id_debitpl_label"));

    _labelDebitVL = new QLabel();
    _labelDebitVL->setText(TR("id_debitvl_label"));

    _groupBoxLayout = new QGridLayout();
    _groupBoxLayout->addWidget(_lineEditDebitPL, 1, 1);
    _groupBoxLayout->addWidget(pUnitDebitPL, 1, 2);
    _groupBoxLayout->addWidget(_lineEditDebitVL, 0, 1);
    _groupBoxLayout->addWidget(pUnitDebitVL, 0, 2);
    _groupBoxLayout->addWidget(_labelDebitPL, 1, 0);
    _groupBoxLayout->addWidget(_labelDebitVL, 0, 0);

    _groupBox = new QGroupBox(this);
    _groupBox->setTitle(TR(""));
    _groupBox->setLayout(_groupBoxLayout);

    resize(300, 150);
    setWindowTitle(TR("id_caption"));
    _traficLayout = new QGridLayout();
    _traficLayout->addWidget(_elmW, 0, 0);
    _traficLayout->addWidget(_groupBox, 1, 0);

    setLayout(_traficLayout);
}


TYTraficWidget::~TYTraficWidget()
{
}

void TYTraficWidget::updateContent()
{
    _elmW->updateContent();

    QString num;

    _lineEditDebitPL->setText(num.setNum(getElement()->getDebitPL(), 'f', 2));
    _lineEditDebitVL->setText(num.setNum(getElement()->getDebitVL(), 'f', 2));
}

void TYTraficWidget::apply()
{
    _elmW->apply();

    if (_lineEditDebitPL->text().toDouble() >= 0)
    {
        getElement()->setDebitPL(_lineEditDebitPL->text().toDouble());
    }
    if (_lineEditDebitVL->text().toDouble() >= 0)
    {
        getElement()->setDebitVL(_lineEditDebitVL->text().toDouble());
    }

    emit modified();
}
