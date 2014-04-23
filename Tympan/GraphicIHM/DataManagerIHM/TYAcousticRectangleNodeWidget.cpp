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
 * \file TYAcousticRectangleNodeWidget.cpp
 * \brief outil IHM pour un ensemble de rectangles acoustiques
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticRectangleNode.h"
//Added by qt3to4:
#include <QGridLayout>


#define TR(id) OLocalizator::getString("TYAcousticRectangleNodeWidget", (id))


TYAcousticRectangleNodeWidget::TYAcousticRectangleNodeWidget(TYAcousticRectangleNode* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{
    _elmW = new TYAcousticSurfaceNodeWidget(pElement, this);

    resize(300, 680);
    setWindowTitle(TR("id_caption"));
    _acousticRectangleNodeLayout = new QGridLayout();
    setLayout(_acousticRectangleNodeLayout);

    _acousticRectangleNodeLayout->addWidget(_elmW, 0, 0);
}

TYAcousticRectangleNodeWidget::~TYAcousticRectangleNodeWidget()
{
}

void TYAcousticRectangleNodeWidget::updateContent()
{
    _elmW->updateContent();
}

void TYAcousticRectangleNodeWidget::apply()
{
    _elmW->apply();

    emit modified();
}

