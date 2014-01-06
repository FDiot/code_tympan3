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
 * \file TYColorInterfaceWidget.cpp
 * \brief outil IHM pour l'objet ColorInterface
 *
 *
 */



#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>

#include "TYColorInterfaceWidget.h"

#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerCore/TYColorInterface.h"
#include "TYToolButton.h"

#define TR(id) OLocalizator::getString("TYColorInterfaceWidget", (id))

TYColorInterfaceWidget::TYColorInterfaceWidget(TYColorInterface* pElement, QWidget* _pParent /*=0*/):
    TYWidget(_pParent)
{
    Q_ASSERT(pElement);
    _pElement = pElement;

    resize(300, 40);
    setWindowTitle(TR("id_caption"));

    _labelColor = new QLabel("labelColor");
    _labelColor->setText(TR("id_color_label"));

    _pushButtonColor = new TYToolButton(NULL, "pushButtonColor");
    _pushButtonColor->setFixedSize(30, 30);

    QObject::connect(_pushButtonColor, SIGNAL(colorAccepted(QColor)), this, SLOT(colorAccepted()));

    _groupBoxLayout = new QGridLayout();
    _groupBoxLayout->setAlignment(Qt::AlignLeft);
    _groupBoxLayout->addWidget(_labelColor, 0, 0);
    _groupBoxLayout->addWidget(_pushButtonColor, 0, 1);

    _groupBox = new QGroupBox(this);
    _groupBox->setTitle(TR(""));
    _groupBox->setLayout(_groupBoxLayout);
    //_groupBox->layout()->setSpacing( 6 );
    //_groupBox->layout()->setMargin( 11 );

    QGridLayout* colorInterfaceLayout = new QGridLayout();
    colorInterfaceLayout->addWidget(_groupBox, 0, 0);
    colorInterfaceLayout->setMargin(0);
    setLayout(colorInterfaceLayout);
    updateContent();
}

TYColorInterfaceWidget::~TYColorInterfaceWidget()
{
}

void TYColorInterfaceWidget::updateContent()
{
    OColor color = getElement()->getColor();
    _pushButtonColor->setColor(QColor(color.r * 255, color.g * 255, color.b * 255));
}

void TYColorInterfaceWidget::apply()
{
    int color[3];
    _pushButtonColor->getColor().getRgb(&color[0], &color[1], &color[2]);
    getElement()->setColor(OColor(color[0] / 255.0f, color[1] / 255.0f, color[2] / 255.0f));

    emit modified();
}

void TYColorInterfaceWidget::reject()
{
}

void TYColorInterfaceWidget::colorAccepted()
{
    emit colorModified();
}
