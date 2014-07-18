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
 * \file TYAcousticCircleWidget.cpp
 * \brief outil IHM pour un cercle acoustique
 */


//Added by qt3to4:
#include <QGridLayout>

#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/geoacoustic/TYAcousticCircle.h"
#include "Tympan/gui/widgets/TYAcousticSurfaceWidget.h"
#include "TYAcousticCircleWidget.h"

#define TR(id) OLocalizator::getString("TYAcousticCircleWidget", (id))


TYAcousticCircleWidget::TYAcousticCircleWidget(TYAcousticCircle* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{

    resize(300, 640);
    setWindowTitle(TR("id_caption"));
    acousticCircleLayout = new QGridLayout();
    setLayout(acousticCircleLayout);

    _elmW = new TYAcousticSurfaceWidget(pElement, this);
    acousticCircleLayout->addWidget(_elmW, 0, 0);
}

TYAcousticCircleWidget::~TYAcousticCircleWidget()
{
}

void TYAcousticCircleWidget::updateContent()
{
    _elmW->updateContent();
}

void TYAcousticCircleWidget::apply()
{
    _elmW->apply();

    emit modified();
}
