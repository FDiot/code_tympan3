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
 * \file TYPointWidget.cpp
 * \brief outil IHM pour un point
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYPoint.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>


#define TR(id) OLocalizator::getString("TYPointWidget", (id))


TYPointWidget::TYPointWidget(TYPoint* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{

    //  _elmW = new TYElementWidget(pElement, this);
    //  _colorW = new TYColorInterfaceWidget(pElement, this);

    resize(300, 174);
    setWindowTitle(TR("id_caption"));
    _pointLayout = new QGridLayout();
    setLayout(_pointLayout);

    //  _pointLayout->addWidget( _elmW, 0, 0 );
    //  _pointLayout->addWidget( _colorW, 1, 0 );

    _groupBox = new QGroupBox(this);
    _groupBox->setTitle(TR("id_position"));
    _groupBoxLayout = new QGridLayout();
    _groupBox->setLayout(_groupBoxLayout);

    _labelX = new QLabel(_groupBox);
    _labelX->setText(TR("id_x_label"));
    _groupBoxLayout->addWidget(_labelX, 0, 0);

    _labelZ = new QLabel(_groupBox);
    _labelZ->setText(TR("id_z_label"));
    _groupBoxLayout->addWidget(_labelZ, 2, 0);

    _labelY = new QLabel(_groupBox);
    _labelY->setText(TR("id_y_label"));
    _groupBoxLayout->addWidget(_labelY, 1, 0);

    _lineEditX = new QLineEdit(_groupBox);
    _groupBoxLayout->addWidget(_lineEditX, 0, 1);

    _lineEditY = new QLineEdit(_groupBox);
    _groupBoxLayout->addWidget(_lineEditY, 1, 1);

    _lineEditZ = new QLineEdit(_groupBox);
    _groupBoxLayout->addWidget(_lineEditZ, 2, 1);

    _pointLayout->addWidget(_groupBox, 2, 0);

    updateContent();
}

TYPointWidget::~TYPointWidget()
{
}

void TYPointWidget::updateContent()
{
    //  _elmW->updateContent();
    //  _colorW->updateContent();

    _lineEditX->setText(QString().setNum(getElement()->_x, 'f', 3));
    _lineEditY->setText(QString().setNum(getElement()->_y, 'f', 3));
    _lineEditZ->setText(QString().setNum(getElement()->_z, 'f', 3));
}

void TYPointWidget::apply()
{
    //  _elmW->apply();
    //  _colorW->apply();

    getElement()->_x = _lineEditX->text().toDouble();
    getElement()->_y = _lineEditY->text().toDouble();
    getElement()->_z = _lineEditZ->text().toDouble();

    emit modified();
}

void TYPointWidget::disableZ()
{
    _lineEditZ->setEnabled(false);
}

void TYPointWidget::enableZ()
{
    _lineEditZ->setEnabled(false);
}

