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
 * \file TYRouteWidget.cpp
 * \brief outil IHM pour une route
 */

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "TYRouteWidget.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYRoute.h"

#include "RoadEmissionNMPB08.h"

#include "Tympan/Tools/OLocalizator.h"
#define TR(id) OLocalizator::getString("TYRouteWidget", (id))


TYRouteWidget::TYRouteWidget(TYRoute* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{
    // Initialise the QT Designer generated UI base
    setupUi(this);

    // Display the widget for pElement as an AcousticSource in an additional tab
    _elmW = new TYAcousticLineWidget(pElement, this);
    QTabWidget* qTabW =findChild<QTabWidget *>("tabWidget");
    assert(qTabW && "Check name consistency with the UI file");
    qTabW->addTab(_elmW, "Source");

    // Find the widget by name so that it is easy to access them
    q_RoadSurfaceType_Combo = findChild<QComboBox*>("route_classe_revetement");
    assert(q_RoadSurfaceType_Combo);
    q_RoadSurfaceDraining_Check = findChild<QCheckBox*>("route_drainant");
    assert(q_RoadSurfaceDraining_Check);
    q_RoadSurfaceAge_Spin = findChild<QSpinBox*>("route_age");
    assert(q_RoadSurfaceAge_Spin);

    // Update the GUI from the data in the TYRoute instance.
    updateContent();
}

TYRouteWidget::~TYRouteWidget()
{
}

void TYRouteWidget::updateContent()
{
    update_road_surface();
    _elmW->updateContent();
/*
    _lineEditVitMoy->setText(QString().setNum(getElement()->getVitMoy(), 'f', 2));
    _pLineEditDebitPLJour->setText(QString().setNum(getElement()->getTraficJour().getDebitPL()));
    _pLineEditDebitVLJour->setText(QString().setNum(getElement()->getTraficJour().getDebitVL()));
    _pLineEditDebitPLNuit->setText(QString().setNum(getElement()->getTraficNuit().getDebitPL()));
    _pLineEditDebitVLNuit->setText(QString().setNum(getElement()->getTraficNuit().getDebitVL()));
    // _comboBoxTrafic->setCurrentIndex(getElement()->getModeCalc()); // WIP replace with RoadFlowType
*/
}

void TYRouteWidget::apply()
{
    _elmW->apply();
    apply_road_surface();

    /*
    getElement()->setVitMoy(_lineEditVitMoy->text().toDouble());
    // getElement()->setModeCalc(_comboBoxTrafic->currentIndex());// WIP replace with RoadFlowType
    getElement()->getTraficJour().setDebitPL(_pLineEditDebitPLJour->text().toDouble());
    getElement()->getTraficJour().setDebitVL(_pLineEditDebitVLJour->text().toDouble());
    getElement()->getTraficNuit().setDebitPL(_pLineEditDebitPLNuit->text().toDouble());
    getElement()->getTraficNuit().setDebitVL(_pLineEditDebitVLNuit->text().toDouble());

    emit modified();
*/
}

void TYRouteWidget::apply_road_surface()
{
    TYRoute& road = *getElement();

    int index = q_RoadSurfaceType_Combo->currentIndex();
    if (q_RoadSurfaceDraining_Check->isChecked())
        index += RoadSurface_DR1 - 1;
    assert(index >= 0 && index < RoadSurface_UserDefined);
    RoadSurfaceType surf_type = static_cast<RoadSurfaceType>(index);
    road.setSurfaceType(surf_type);

    int age = q_RoadSurfaceAge_Spin->value();
    road.setSurfaceAge(age);
}

void TYRouteWidget::update_road_surface()
{
    TYRoute& road = *getElement();

    int index = road.surfaceType();
    assert(index >= 0 && index < RoadSurface_UserDefined);
    if (index >= RoadSurface_DR1)
    {
        q_RoadSurfaceDraining_Check->setChecked(true);
        index -= RoadSurface_DR1 + 1;
    }
    q_RoadSurfaceType_Combo->setCurrentIndex(index);

    q_RoadSurfaceAge_Spin->setValue( road.surfaceAge() );
}
