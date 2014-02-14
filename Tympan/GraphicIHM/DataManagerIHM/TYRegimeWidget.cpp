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
 * \file TYRegimeWidget.cpp
 * \brief outil IHM pour un regime
 *
 *
 */


#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYRegime.h"

#define TR(id) OLocalizator::getString("TYRegimeWidget", (id))


TYRegimeWidget::TYRegimeWidget(TYRegime* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{
}


TYRegimeWidget::~TYRegimeWidget()
{
}

void TYRegimeWidget::updateContent()
{
}

void TYRegimeWidget::apply()
{
}

void TYRegimeWidget::showContextMenu(int row, int col, const QPoint& pos)
{
}

