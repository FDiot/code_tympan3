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
 * \file TYSourceWidget.cpp
 * \brief outil IHM pour une source
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/acoustic/TYSource.h"
#include <qmenu.h>
#include <qbuttongroup.h>


#define TR(id) OLocalizator::getString("TYSourceWidget", (id))
#define IMG(id) OLocalizator::getPicture("TYSourceWidget", (id))


TYSourceWidget::TYSourceWidget(TYSource* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{
    _elmW = new TYElementWidget(pElement, this);
    //  _colorW = new TYColorInterfaceWidget(pElement, this);

    //resize( 300, 220 );
    setWindowTitle(TR("id_caption"));
    _sourceLayout = new QGridLayout();

    _sourceLayout->addWidget(_elmW, 0, 0);
    //  _sourceLayout->addWidget( _colorW, 1, 0);

    _groupBox = new QGroupBox(this);
    _groupBox->setTitle(TR("id_tabspectre_box"));
    _groupBoxLayout = new QGridLayout();
    _groupBox->setLayout(_groupBoxLayout);

    _pushButtonShowSpectre = new QPushButton(_groupBox);
    _pushButtonShowSpectre->setText(TR("id_button_spectre"));

    _groupBoxLayout->addWidget(_pushButtonShowSpectre, 0, 0);

    _sourceLayout->addWidget(_groupBox, 2, 0);

    updateContent();

    connect(_pushButtonShowSpectre, SIGNAL(clicked()), this, SLOT(showSpectre()));
    _sourceLayout->setMargin(0);

    setLayout(_sourceLayout);
}

TYSourceWidget::~TYSourceWidget()
{
}

void TYSourceWidget::updateContent()
{
    _elmW->updateContent();
    //  _colorW->updateContent();
}

void TYSourceWidget::apply()
{
    _elmW->apply();
    //  _colorW->apply();

    emit modified();
}

void TYSourceWidget::showSpectre()
{
    TYSpectre* spectre = getElement()->getCurrentSpectre();
    spectre->edit(this);
}
