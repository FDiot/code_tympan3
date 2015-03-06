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
 * \file TYTerrainWidget.cpp
 * \brief outil IHM pour un terrain
 */

#include <qlayout.h>
#include <QLabel>
#include <qcheckbox.h>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/topography/TYTerrain.h"
#include "Tympan/models/business/material/TYVegetation.h"
#include "Tympan/gui/widgets/TYColorInterfaceWidget.h"
#include "Tympan/gui/widgets/TYSolWidget.h"
#include "TabPointsWidget.h"
#include "TYVegetationWidget.h"
#include "TYTerrainWidget.h"

#define TR(id) OLocalizator::getString("TYTerrainWidget", (id))

TYTerrainWidget::TYTerrainWidget(TYTerrain* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent), _pVegetation(nullptr)
{
    unsigned short lnW = 0;

    _elmW = new TYElementWidget(pElement, this);
    _colorW = new TYColorInterfaceWidget(pElement, this);

    resize(300, 330);
    setWindowTitle(TR("id_caption"));
    _terrainLayout = new QGridLayout();
    setLayout(_terrainLayout);

    _terrainLayout->addWidget(_elmW, lnW++, 0);
    _terrainLayout->addWidget(_colorW, lnW++, 0);

    _groupBox = new QGroupBox(this);
    _groupBox->setTitle(TR(""));
    _groupBoxLayout = new QGridLayout();
    _groupBox->setLayout(_groupBoxLayout);
    _labelVegetActive = new QLabel(_groupBox);
    _labelVegetActive->setText(TR("id_vegetactive_label"));
    _groupBoxLayout->addWidget(_labelVegetActive, 0, 0);
    _checkBoxVegetActive = new QCheckBox(_groupBox);
    _groupBoxLayout->addWidget(_checkBoxVegetActive, 0, 1);

    _terrainLayout->addWidget(_groupBox, lnW++, 0);

    _tabWidget = new QTabWidget(this);

    _solW = new TYSolWidget(getElement()->getSol(), _tabWidget);
    _tabWidget->insertTab(1, _solW, TR("id_sol"));

    _pVegetation = getElement()->getVegetation();
    _vegetationWidget = new TYVegetationWidget(_pVegetation, _tabWidget);

    _tabWidget->insertTab(2, _vegetationWidget, TR("id_vegetation"));

    _groupBox = new QGroupBox(_tabWidget);
    _groupBox->setTitle(TR("id_pts"));
    _groupBoxLayout = new QGridLayout();
    _groupBox->setLayout(_groupBoxLayout);

    _tabPoints = new TabPointsWidget(pElement->getListPoints(), _groupBox);
    _tabPoints->setEnabled(true);

    _groupBoxLayout->addWidget(_tabPoints, 0, 0);

    _tabWidget->insertTab(3, _groupBox, TR("id_geometrie"));

    _terrainLayout->addWidget(_tabWidget, lnW++, 0);

    connect(_checkBoxVegetActive, SIGNAL(clicked()), this, SLOT(useVegetation()));

    updateContent();
}

TYTerrainWidget::~TYTerrainWidget()
{
}

void TYTerrainWidget::updateContent()
{
    QString num;

    _elmW->updateContent();
    _colorW->updateContent();
    _solW->updateContent();

    bool bVegActive = getElement()->isUsingVegetation();
    _checkBoxVegetActive->setChecked(bVegActive);
    _vegetationWidget->setEnabled(bVegActive);
    _vegetationWidget->updateContent();

    _tabPoints->update();
}

void TYTerrainWidget::apply()
{
    _elmW->apply();
    _colorW->apply();
    _solW->apply();
    _vegetationWidget->apply();

    getElement()->setVegetation(_pVegetation);

    _tabPoints->apply();

    emit modified();
}

void TYTerrainWidget::disableSolWidget()
{
    _tabWidget->removeTab(0);
}

void TYTerrainWidget::disableVegetationWidget()
{
    // Efface le widget d'activation de la vegatation
    _checkBoxVegetActive->setEnabled(false);
    _tabWidget->removeTab(0);
}

void TYTerrainWidget::useVegetation()
{
    if (_checkBoxVegetActive->isChecked())
    {
        _pVegetation = getElement()->getVegetation();

        // If no vegetation create it
        if (_pVegetation == nullptr)
        {
            _pVegetation = new TYVegetation();
        }
    }
    else
    {
        _pVegetation = nullptr;
    }

    _vegetationWidget->setElement(_pVegetation);
    _vegetationWidget->updateContent();
    _vegetationWidget->setEnabled(_checkBoxVegetActive->isChecked());
}

