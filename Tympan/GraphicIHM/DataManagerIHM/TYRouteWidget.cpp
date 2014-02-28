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

#include <QMessageBox>

#include "TYRouteWidget.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYRoute.h"

#include "RoadEmissionNMPB08.h"

#include "Tympan/Tools/OLocalizator.h"
#define TR(id) OLocalizator::getString("TYRouteWidget", (id))

static struct {const char* name ; int id;} RoadSurfaceFormulationMap[] =
{
    {"BBUM 0/6",           BBUM_0_6},
    {"BBDr 0/10",          BBDR_0_10},
    {"BBTM 0/6 - type 2",  BBTM_0_6_type2},
    {"BBTM 0/6 - type 1",  BBTM_0_6_type1},
    {"BBTM 0/10 - type 2", BBTM_0_10_type2},
    {"BBSG 0/10",          BBSG_0_10},
    {"BBTM 0/10 - type 1", BBTM_0_10_type1},
    {"BBUM 0/10",          BBUM_0_10},
    {"ECF",                ECF},
    {"BBSG 0/14",          BBSG_0_14},
    {"BBTM 0/14",          BBTM_0_14},
    {"ES 6/10",            ES_6_10},
    {"BC",                 BC},
    {"ES 10/14",           ES_10_14}
};

TYRouteWidget::TYRouteWidget(TYRoute* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{
    // Initialise the QT Designer generated UI base
    setupUi(this);
    bool connect_ok;

    // Display the widget for pElement as an AcousticSource in an additional tab
    _elmW = new TYAcousticLineWidget(pElement, this);
    QTabWidget* qTabW = findChild<QTabWidget*>("tabWidget");
    assert(qTabW && "Check name consistency with the UI file");
    qTabW->insertTab(0, _elmW, "Source");
    qTabW->setCurrentIndex(0);

    // Find the widget by name so that it is easy to access them
    q_RoadSurfaceType_Combo = findChild<QComboBox*>("route_classe_revetement");
    assert(q_RoadSurfaceType_Combo);
    q_RoadSurfaceFormulation_Combo = findChild<QComboBox*>("route_formule_revetement");
    assert(q_RoadSurfaceFormulation_Combo);
    q_RoadSurfaceDraining_Check = findChild<QCheckBox*>("route_drainant");
    assert(q_RoadSurfaceDraining_Check);
    q_RoadSurfaceAge_Spin = findChild<QSpinBox*>("route_age");
    assert(q_RoadSurfaceAge_Spin);
    q_RoadFlowType_Combo = findChild<QComboBox*>("route_ecoulement");
    assert(q_RoadFlowType_Combo);

    // Get he widget for the traffic speed
    q_RoadSpeed_Spin[TYRoute::Day][TYTrafic::LV] = findChild<QSpinBox*>("vitesse_vl_jour");
    assert(q_RoadSpeed_Spin[TYRoute::Day][TYTrafic::LV]);
    q_RoadSpeed_Spin[TYRoute::Evening][TYTrafic::LV] = findChild<QSpinBox*>("vitesse_vl_soir");
    assert(q_RoadSpeed_Spin[TYRoute::Evening][TYTrafic::LV]);
    q_RoadSpeed_Spin[TYRoute::Night][TYTrafic::LV] = findChild<QSpinBox*>("vitesse_vl_nuit");
    assert(q_RoadSpeed_Spin[TYRoute::Night][TYTrafic::LV]);
    q_RoadSpeed_Spin[TYRoute::Day][TYTrafic::HGV] = findChild<QSpinBox*>("vitesse_pl_jour");
    assert(q_RoadSpeed_Spin[TYRoute::Day][TYTrafic::HGV]);
    q_RoadSpeed_Spin[TYRoute::Evening][TYTrafic::HGV] = findChild<QSpinBox*>("vitesse_pl_soir");
    assert(q_RoadSpeed_Spin[TYRoute::Evening][TYTrafic::HGV]);
    q_RoadSpeed_Spin[TYRoute::Night][TYTrafic::HGV] = findChild<QSpinBox*>("vitesse_pl_nuit");
    assert(q_RoadSpeed_Spin[TYRoute::Night][TYTrafic::HGV]);
    // Get he widget for the traffic flow
    q_RoadFlow_Spin[TYRoute::Day][TYTrafic::LV] = findChild<QSpinBox*>("periode_debit_vl_jour");
    assert(q_RoadFlow_Spin[TYRoute::Day][TYTrafic::LV]);
    q_RoadFlow_Spin[TYRoute::Evening][TYTrafic::LV] = findChild<QSpinBox*>("periode_debit_vl_soir");
    assert(q_RoadFlow_Spin[TYRoute::Evening][TYTrafic::LV]);
    q_RoadFlow_Spin[TYRoute::Night][TYTrafic::LV] = findChild<QSpinBox*>("periode_debit_vl_nuit");
    assert(q_RoadFlow_Spin[TYRoute::Night][TYTrafic::LV]);
    q_RoadFlow_Spin[TYRoute::Day][TYTrafic::HGV] = findChild<QSpinBox*>("periode_debit_pl_jour");
    assert(q_RoadFlow_Spin[TYRoute::Day][TYTrafic::HGV]);
    q_RoadFlow_Spin[TYRoute::Evening][TYTrafic::HGV] = findChild<QSpinBox*>("periode_debit_pl_soir");
    assert(q_RoadFlow_Spin[TYRoute::Evening][TYTrafic::HGV]);
    q_RoadFlow_Spin[TYRoute::Night][TYTrafic::HGV] = findChild<QSpinBox*>("periode_debit_pl_nuit");
    assert(q_RoadFlow_Spin[TYRoute::Night][TYTrafic::HGV]);

    connect_ok = QObject::connect(q_RoadSurfaceType_Combo,      SIGNAL(currentIndexChanged(int)),
                                  this,                         SLOT(onRoadSurfaceChange(int)));
    assert(connect_ok && "Qt signal connection failed");
    connect_ok = QObject::connect(q_RoadSurfaceFormulation_Combo,      SIGNAL(activated(int)),
                                  this,                                SLOT(onRoadSurfaceFormulationChange(int)));
    assert(connect_ok && "Qt signal connection failed");

    q_AADT_Push = findChild<QPushButton*>("bouton_tmja");
    assert(q_AADT_Push);
    connect_ok = QObject::connect(q_AADT_Push,      SIGNAL(clicked()),
                                  this,             SLOT(display_AADT_dialog()));
    assert(connect_ok && "Qt signal connection failed");
    p_ModeCalcul_ButtonGroup = findChild<QButtonGroup*>("groupe_boutons_mode_calcul");
    assert(p_ModeCalcul_ButtonGroup);
    QRadioButton* q_check_box;
    q_check_box = findChild<QRadioButton*>("radio_spectres");
    assert(q_check_box);
    p_ModeCalcul_ButtonGroup->setId(q_check_box, 0);
    q_check_box = findChild<QRadioButton*>("radio_debits");
    assert(q_check_box);
    p_ModeCalcul_ButtonGroup->setId(q_check_box, 1);
    q_check_box = findChild<QRadioButton*>("radio_tmja");
    assert(q_check_box);
    p_ModeCalcul_ButtonGroup->setId(q_check_box, 2);

    q_EditSpectre_Button[TYRoute::Day] = findChild<QPushButton*>("bouton_spectre_jour");
    assert(q_EditSpectre_Button[TYRoute::Day]);
    connect_ok = QObject::connect(q_EditSpectre_Button[TYRoute::Day], SIGNAL(clicked()),
                                  this, SLOT(displaySpectrumDay()));
    assert(connect_ok && "Qt signal connection failed");
    q_EditSpectre_Button[TYRoute::Evening] = findChild<QPushButton*>("bouton_spectre_soir");
    assert(q_EditSpectre_Button[TYRoute::Evening]);
    connect_ok = QObject::connect(q_EditSpectre_Button[TYRoute::Evening], SIGNAL(clicked()),
                                  this, SLOT(displaySpectrumEvening()));
    assert(connect_ok && "Qt signal connection failed");
    q_EditSpectre_Button[TYRoute::Night] = findChild<QPushButton*>("bouton_spectre_nuit");
    assert(q_EditSpectre_Button[TYRoute::Night]);
    connect_ok = QObject::connect(q_EditSpectre_Button[TYRoute::Night], SIGNAL(clicked()),
                                  this, SLOT(displaySpectrumNight()));
    assert(connect_ok && "Qt signal connection failed");

    checkComputationMode(1);
    connect_ok = QObject::connect(p_ModeCalcul_ButtonGroup, SIGNAL(buttonClicked(int)),
                                  this, SLOT(checkComputationMode(int)));
    assert(connect_ok && "Qt signal connection failed");

    // Update the GUI from the data in the TYRoute instance.
    updateContent();
}

TYRouteWidget::~TYRouteWidget()
{
}

void TYRouteWidget::updateContent()
{
    _elmW->updateContent();
    update_road_surface();
    update_road_traffic();
    QWidget::update();
}

void TYRouteWidget::apply()
{
    _elmW->apply();
    apply_road_surface();
    apply_road_traffic();

    emit modified();
}

void TYRouteWidget::apply_road_surface()
{
    TYRoute& road = *getElement();
    int index;

    index = q_RoadSurfaceType_Combo->currentIndex();
    if (index != 0 && q_RoadSurfaceDraining_Check->isChecked())
    {
        index += RoadSurface_DR1 - 1;
    }
    assert(index >= 0 && index < RoadSurface_UserDefined);
    RoadSurfaceType surf_type = static_cast<RoadSurfaceType>(index);
    road.setSurfaceType(surf_type);

    int age = q_RoadSurfaceAge_Spin->value();
    road.setSurfaceAge(age);
}

void TYRouteWidget::update_road_surface()
{
    TYRoute& road = *getElement();
    int index;

    index = road.surfaceType();
    assert(index >= 0 && index < RoadSurface_UserDefined);
    if (index >= RoadSurface_DR1)
    {
        q_RoadSurfaceDraining_Check->setChecked(true);
        index -= RoadSurface_DR1 - 1;
    }
    else
    {
        q_RoadSurfaceDraining_Check->setChecked(false);
    }
    q_RoadSurfaceType_Combo->setCurrentIndex(index);

    q_RoadSurfaceAge_Spin->setValue(road.surfaceAge());

    RoadFlowType flow_type =
        road.getNMPB08RoadTrafficComponent(TYRoute::Day, TYTrafic::LV).flowType;

    for (unsigned j = 0; j < TYRoute::NB_TRAFFIC_REGIMES; ++j)
    {
        enum TYRoute::TrafficRegimes regime = static_cast<TYRoute::TrafficRegimes>(j);
        for (unsigned i = 0; i < TYTrafic::NB_VEHICLE_TYPES; ++i)
        {
            enum TYTrafic::VehicleTypes vehicle_type = static_cast<TYTrafic::VehicleTypes>(i);
            assert(road.getNMPB08RoadTrafficComponent(regime, vehicle_type).flowType == flow_type
                   && "The traffic flow type for all componenets are expected to be the same for now.");
        }
    }
    q_RoadFlowType_Combo->setCurrentIndex(flow_type);
}

void TYRouteWidget::apply_road_traffic()
{
    TYRoute& road = *getElement();

    for (unsigned j = 0; j < TYRoute::NB_TRAFFIC_REGIMES; ++j)
    {
        enum TYRoute::TrafficRegimes regime = static_cast<TYRoute::TrafficRegimes>(j);
        for (unsigned i = 0; i < TYTrafic::NB_VEHICLE_TYPES; ++i)
        {
            enum TYTrafic::VehicleTypes vehicle_type = static_cast<TYTrafic::VehicleTypes>(i);

            int speed = q_RoadSpeed_Spin[regime][vehicle_type]->value();
            int flow = q_RoadFlow_Spin[regime][vehicle_type]->value();
            // NB : For now there is only one flow type for all traffic componenet in the GUI
            int index = q_RoadFlowType_Combo->currentIndex();
            RoadFlowType flow_type = static_cast<RoadFlowType>(index);
            road.setRoadTrafficComponent(regime, vehicle_type, flow, speed, flow_type);
        }
    }
}

void TYRouteWidget::update_road_traffic()
{
    TYRoute& road = *getElement();

    for (unsigned j = 0; j < TYRoute::NB_TRAFFIC_REGIMES; ++j)
    {
        enum TYRoute::TrafficRegimes regime = static_cast<TYRoute::TrafficRegimes>(j);
        for (unsigned i = 0; i < TYTrafic::NB_VEHICLE_TYPES; ++i)
        {
            enum TYTrafic::VehicleTypes vehicle_type = static_cast<TYTrafic::VehicleTypes>(i);

            const RoadTrafficComponent& rtc = road.getNMPB08RoadTrafficComponent(regime, vehicle_type);
            q_RoadSpeed_Spin[regime][vehicle_type]->setValue(rtc.trafficSpeed);
            q_RoadFlow_Spin[regime][vehicle_type]->setValue(rtc.trafficFlow);
            /* TODO Handle rtc.flowType */
        }
    }
}

TYRouteWidget_AADT_Dialog::TYRouteWidget_AADT_Dialog(QWidget* _pParent)
    : QDialog(_pParent)
{
    setupUi(this);
}

void TYRouteWidget::display_AADT_dialog()
{
    TYRouteWidget_AADT_Dialog dlg(this);
    bool ok = false;
    while (!ok)
    {
        int status = dlg.exec();
        if (status == QDialog::Accepted)
        {
            QSpinBox* q_lv_Spin = findChild<QSpinBox*>("tmja_debit_vl");
            assert(q_lv_Spin);
            QSpinBox* q_hgv_Spin = findChild<QSpinBox*>("tmja_debit_pl");
            assert(q_hgv_Spin);
            QComboBox* q_RoadType_Combo = findChild<QComboBox*>("tmja_type_route");
            assert(q_RoadType_Combo);
            QComboBox* q_RoadFunction_Combo = findChild<QComboBox*>("tmja_fonction_route");
            assert(q_RoadFunction_Combo);

            double lv_aadt = q_lv_Spin->value();
            double hgv_aadt = q_hgv_Spin->value();
            TYRoute::RoadType road_type =
                static_cast<TYRoute::RoadType>(q_RoadType_Combo->currentIndex());
            TYRoute::RoadFunction road_function =
                static_cast<TYRoute::RoadFunction>(q_RoadFunction_Combo->currentIndex());

            TYRoute& road = *getElement();
            QString msg;
            ok = road.setFromAADT(hgv_aadt, lv_aadt, road_type, road_function, &msg);

            if (ok)
            {
                // Do not forget to update the traffic to reflect the computed parameters
                updateContent();
            }
            else
            {
                // TODO i18n when serious i18n based on Qt will be in place
                msg.replace("\n", "<br/>");
                QString text = QString::fromUtf8(
                                   "<p>Les valeurs du TMJA spécifiées sont hors du domaine "
                                   "de validité de la <i>Note 77</i>. Merci de les rectifier :</p>");
                QMessageBox::information(this, "TMJA invalide !", text + msg,
                                         QMessageBox::Ok | QMessageBox::Default,
                                         QMessageBox::NoButton, QMessageBox::NoButton);
            }
        }
        else
        {
            break;    // Dialog canceled
        }
    } // while(!ok)
}

void TYRouteWidget::setSpeedBoxEnabled(bool enabled)
{
    for (unsigned j = 0; j < TYRoute::NB_TRAFFIC_REGIMES; ++j)
        for (unsigned i = 0; i < TYTrafic::NB_VEHICLE_TYPES; ++i)
        {
            q_RoadSpeed_Spin[j][i]->setEnabled(enabled);
        }
}

void TYRouteWidget::setFlowBoxEnabled(bool enabled)
{
    for (unsigned j = 0; j < TYRoute::NB_TRAFFIC_REGIMES; ++j)
        for (unsigned i = 0; i < TYTrafic::NB_VEHICLE_TYPES; ++i)
        {
            q_RoadFlow_Spin[j][i]->setEnabled(enabled);
        }
}

void TYRouteWidget::setSpectresEditable(bool enabled)
{
    TYRoute& road = *getElement();
    spectrum_read_only = !enabled;
    for (unsigned j = 0; j < TYRoute::NB_TRAFFIC_REGIMES; ++j)
    {
        q_EditSpectre_Button[j]->setEnabled(enabled);
        road.setCurRegime(j);
        if (enabled)
        {
            road.setTypeDistribution(TYAcousticLine::TY_PUISSANCE_IMPOSEE);
        }
        else
        {
            road.setTypeDistribution(TYAcousticLine::TY_PUISSANCE_CALCULEE);
        }
        road.updateCurrentRegime();
    }
}

void TYRouteWidget::checkComputationMode(int mode)
{
    TYRoute& road = *getElement();
    switch (mode)
    {
        case 0: // Directly input spectrums
            setSpeedBoxEnabled(false);
            setFlowBoxEnabled(false);
            setSpectresEditable(true);
            q_AADT_Push->setEnabled(false);
            break;
        case 1: // Input flows and speeds
            setSpeedBoxEnabled(true);
            setFlowBoxEnabled(true);
            setSpectresEditable(false);
            q_AADT_Push->setEnabled(false);
            break;
        case 2: // Use note 77 to estimate traffic from the TMJA
            setSpeedBoxEnabled(true);
            setFlowBoxEnabled(false);
            setSpectresEditable(false);
            q_AADT_Push->setEnabled(true);
            break;
        default:
            assert(false && "mode should be in 0..2");
    }

    _elmW->updateContent();
}

void TYRouteWidget::displaySpectrum(TYRoute::TrafficRegimes regime)
{
    TYRoute& road = *getElement();

    assert(road.getNbRegimes() == 3);
    road.setCurRegime(regime);
    TYSpectre* spectre = road.getCurrentSpectre();
    int ret = spectre->edit(this);
}

void TYRouteWidget::onRoadSurfaceChange(int)
{
    int index = q_RoadSurfaceType_Combo->currentIndex();
    assert(index >= 0 && index < RoadSurface_UserDefined);
    q_RoadSurfaceDraining_Check->setEnabled(index != RoadSurface_Default);
    q_RoadSurfaceFormulation_Combo->setCurrentIndex(0);
}

void TYRouteWidget::onRoadSurfaceFormulationChange(int index)
{
    if (index > 0)
    {
        TYRoute& road = *getElement();
        int surf_type_no = RoadSurfaceFormulationMap[index - 1].id;
        RoadSurfaceType surf_type = static_cast<RoadSurfaceType>(surf_type_no);
        road.setSurfaceType(surf_type);
        update_road_surface();
        q_RoadSurfaceFormulation_Combo->setCurrentIndex(index);
        QWidget::update();
    }
}
