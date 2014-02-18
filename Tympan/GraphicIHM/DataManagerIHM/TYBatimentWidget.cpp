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
 * \file TYBatimentWidget.cpp
 * \brief outil IHM pour un batiment
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYBatiment.h"
//Added by qt3to4:
#include <QGridLayout>


#define TR(id) OLocalizator::getString("TYBatimentWidget", (id))


TYBatimentWidget::TYBatimentWidget(TYBatiment* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{

    resize(300, 540);
    setWindowTitle(TR("id_caption"));
    QGridLayout* _batimentLayout = new QGridLayout();
    setLayout(_batimentLayout);

    _elmW = new TYAcousticVolumeNodeWidget(pElement, this);
    _batimentLayout->addWidget(_elmW, 0, 0);
}

TYBatimentWidget::~TYBatimentWidget()
{
}

void TYBatimentWidget::updateContent()
{
    _elmW->updateContent();
}

void TYBatimentWidget::apply()
{
    _elmW->apply();

    emit modified();
}
