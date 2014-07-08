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
 * \file TYElementWidget.cpp
 * \brief outil IHM pour un element
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/models/business/TYElement.h"
#include "Tympan/models/business/OLocalizator.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>


#define TR(id) OLocalizator::getString("TYElementWidget", (id))


TYElementWidget::TYElementWidget(TYElement* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{

    // resize( 300, 40 );
    setWindowTitle(TR("id_caption"));

    _lineEditName = new QLineEdit("lineEditName");
    _lineEditName->setText(getElement()->getName());

    _labelName = new QLabel("labelName");
    _labelName->setText(TR("id_name_label"));

    _groupBoxLayout = new QGridLayout();
    _groupBoxLayout->addWidget(_labelName, 0, 0);
    _groupBoxLayout->addWidget(_lineEditName, 0, 1);

    _groupBox = new QGroupBox(this);
    _groupBox->setTitle(TR(""));
    _groupBox->setLayout(_groupBoxLayout);

    QGridLayout* ElementLayout = new QGridLayout();
    ElementLayout->setMargin(0);
    ElementLayout->addWidget(_groupBox, 0, 0);

    setContentsMargins(0, 0, 0, 0);

    setLayout(ElementLayout);
}

TYElementWidget::~TYElementWidget()
{
}

void TYElementWidget::updateContent()
{
    _lineEditName->setText(getElement()->getName());
}

void TYElementWidget::apply()
{
    getElement()->setName(_lineEditName->text());

    TYElement::setIsSavedOk(true); // Il faut sauvegarder le fichier

    emit modified();
}

void TYElementWidget::reject()
{
}

void TYElementWidget::updateEditName(const QString& name)
{
    _lineEditName->setText(name);
}
