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
 * \file TYCoursEauWidget.cpp
 * \brief Outil IHM pour un cours d'eau
 *
 *
 */




#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/topography/TYCoursEau.h"
//Added by qt3to4:
#include <QGridLayout>


#define TR(id) OLocalizator::getString("TYCoursEauWidget", (id))


TYCoursEauWidget::TYCoursEauWidget(TYCoursEau* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{

    resize(300, 480);
    setWindowTitle(TR("id_caption"));
    _coursEauLayout = new QGridLayout();
    setLayout(_coursEauLayout);

    _elmW = new TYAcousticLineWidget(pElement, this);
    _coursEauLayout->addWidget(_elmW, 0, 0);
}

TYCoursEauWidget::~TYCoursEauWidget()
{
}

void TYCoursEauWidget::updateContent()
{
    _elmW->updateContent();
}

void TYCoursEauWidget::apply()
{
    _elmW->apply();

    emit modified();
}
