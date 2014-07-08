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
 * \file TYMachineWidget.cpp
 * \brief outil IHM pour une machine
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/infrastructure/TYMachine.h"

//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>
#include <QTextEdit>

#define TR(id) OLocalizator::getString("TYMachineWidget", (id))


TYMachineWidget::TYMachineWidget(TYMachine* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{
    QString num;


    resize(300, 680);
    setWindowTitle(TR("id_caption"));

    _elmW = new TYAcousticVolumeNodeWidget(pElement, this);

    _lineEditCategorie = new QLineEdit();
    _lineEditCategorie->setText(num.setNum(getElement()->getCategorie()));

    _labelCategorie = new QLabel();
    _labelCategorie->setText(TR("id_categorie_label"));

    _labelConstructeur = new QLabel();
    _labelConstructeur->setText(TR("id_constructeur_label"));

    _lineEditConstructeur = new QLineEdit();
    _lineEditConstructeur->setText(getElement()->getConstructeur());

    _labelModele = new QLabel();
    _labelModele->setText(TR("id_modele_label"));

    _lineEditModele = new QLineEdit();
    _lineEditModele->setText(getElement()->getModele());

    _lineEditComment = new QTextEdit(getElement()->getCommentaire());

    _labelComment = new QLabel();
    _labelComment->setText(TR("id_commentaire_label"));

    _groupBoxLayout = new QGridLayout();
    _groupBoxLayout->addWidget(_lineEditCategorie, 0, 1);
    _groupBoxLayout->addWidget(_labelCategorie, 0, 0);
    _groupBoxLayout->addWidget(_labelConstructeur, 1, 0);
    _groupBoxLayout->addWidget(_lineEditConstructeur, 1, 1);
    _groupBoxLayout->addWidget(_labelModele, 2, 0);
    _groupBoxLayout->addWidget(_lineEditModele, 2, 1);
    _groupBoxLayout->addWidget(_lineEditComment, 4, 0, 1, 2);
    _groupBoxLayout->addWidget(_labelComment, 3, 0);

    _groupBox = new QGroupBox();
    _groupBox->setTitle(TR(""));
    _groupBox->setLayout(_groupBoxLayout);

    _machineLayout = new QGridLayout();
    _machineLayout->addWidget(_elmW, 0, 0);
    _machineLayout->addWidget(_groupBox, 1, 0);

    setLayout(_machineLayout);
}

TYMachineWidget::~TYMachineWidget()
{
}

void TYMachineWidget::updateContent()
{
    QString num;

    _elmW->updateContent();

    _lineEditComment->setPlainText(getElement()->getCommentaire());
    _lineEditModele->setText(getElement()->getModele());
    _lineEditConstructeur->setText(getElement()->getConstructeur());
    _lineEditCategorie->setText(num.setNum(getElement()->getCategorie()));

}

void TYMachineWidget::apply()
{
    _elmW->apply();

    getElement()->setCommentaire(_lineEditComment->toPlainText());
    getElement()->setModele(_lineEditModele->text());
    getElement()->setConstructeur(_lineEditConstructeur->text());
    getElement()->setCategorie(_lineEditCategorie->text().toInt());

    emit modified();
}
