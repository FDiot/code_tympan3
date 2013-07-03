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
 * \file TYAttenuateurWidget.cpp
 * \brief outil IHM pour un attenuateur
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYAttenuateur.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>


#define TR(id) OLocalizator::getString("TYAttenuateurWidget", (id))


TYAttenuateurWidget::TYAttenuateurWidget(TYAttenuateur* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(_pParent)
{
    QString num;

    Q_ASSERT(pElement);
    _pElement = pElement;

    _elmW = new TYElementWidget(pElement, this);
    resize(300, 200);
    setWindowTitle(TR("id_caption"));
    _attenuateurLayout = new QGridLayout();

    _attenuateurLayout->addWidget(_elmW, 0, 0);

    TYSpectre& spectre = (static_cast<TYAttenuateur*>(_pElement))->getSpectreAtt();
    _spectreW = new TYSpectreWidget(&spectre, this);
    _attenuateurLayout->addWidget(_spectreW, 1, 0);
    setLayout(_attenuateurLayout);

    updateContent();
}

TYAttenuateurWidget::~TYAttenuateurWidget()
{
}

void TYAttenuateurWidget::updateContent()
{
    _elmW->updateContent();
    _spectreW->updateContent();
}

void TYAttenuateurWidget::apply()
{
    _elmW->apply();
    _spectreW->apply();

    emit modified();
}
