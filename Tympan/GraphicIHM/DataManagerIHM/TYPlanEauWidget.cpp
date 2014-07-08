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
 * \file TYPlanEauWidget.cpp
 * \brief outil IHM pour un plan d'eau
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/topography/TYPlanEau.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>


#define TR(id) OLocalizator::getString("TYPlanEauWidget", (id))


TYPlanEauWidget::TYPlanEauWidget(TYPlanEau* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{

    _terrainW = new TYTerrainWidget(pElement, this);
    _terrainW->setLayoutSpacing(0, 0);

    resize(300, 330);
    setWindowTitle(TR("id_caption"));

    _planEauLayout = new QGridLayout();
    setLayout(_planEauLayout);
    _planEauLayout->addWidget(_terrainW, 0, 0);

    _groupBoxAlt = new QGroupBox(this);
    _groupBoxAlt->setTitle(TR(""));
    _groupBoxAltLayout = new QGridLayout();
    _groupBoxAlt->setLayout(_groupBoxAltLayout);

    _labelAltitude = new QLabel(_groupBoxAlt);
    _labelAltitude->setText(TR("id_altitude_label"));
    _groupBoxAltLayout->addWidget(_labelAltitude, 0, 0);

    _lineEditAltitude = new QLineEdit(_groupBoxAlt);
    _lineEditAltitude->setText(QString().setNum(getElement()->getAltitude(), 'f', 2));
    _groupBoxAltLayout->addWidget(_lineEditAltitude, 0, 1);
    QLabel* pUnitAlt = new QLabel(_groupBoxAlt);
    pUnitAlt->setText(TR("id_unite_altitude"));
    _groupBoxAltLayout->addWidget(pUnitAlt, 0, 2);

    _planEauLayout->addWidget(_groupBoxAlt, 1, 0);

    // Gestion de la distance max entre deux points de la courbe de niveau
    _groupBoxDistMax = new QGroupBox(this);
    _groupBoxDistMax->setTitle(TR(""));
    _groupBoxDistLayout = new QGridLayout();
    _groupBoxDistMax->setLayout(_groupBoxDistLayout);

    _labelDistMax = new QLabel(_groupBoxDistMax);
    _labelDistMax->setText(TR("id_distmax_label"));
    _groupBoxDistLayout->addWidget(_labelDistMax, 0, 0);

    _lineEditDistMax = new QLineEdit(_groupBoxDistMax);
    _groupBoxDistLayout->addWidget(_lineEditDistMax, 0, 1);
    QLabel* pUnitDist = new QLabel(_groupBoxDistMax);
    pUnitDist->setText(TR("id_unite_altitude"));
    _groupBoxDistLayout->addWidget(pUnitDist, 0, 2);

    _labelUseDefault = new QLabel(_groupBoxDistMax);
    _labelUseDefault->setText(TR("id_use_default_label"));
    QPushButton* _pushButtonUseDefault = new QPushButton(_groupBoxDistMax);
    _pushButtonUseDefault->setText(TR("id_default_button"));

    _groupBoxDistLayout->addWidget(_labelUseDefault, 1, 0);
    _groupBoxDistLayout->addWidget(_pushButtonUseDefault, 1, 1);

    _planEauLayout->addWidget(_groupBoxDistMax, 2, 0);

    connect(_pushButtonUseDefault, SIGNAL(clicked()), this, SLOT(setDefaultValue()));
    connect(_lineEditDistMax, SIGNAL(textChanged(const QString&)), this, SLOT(updateUseDefault()));

    updateContent();
}

TYPlanEauWidget::~TYPlanEauWidget()
{
}

void TYPlanEauWidget::updateContent()
{
    _terrainW->updateContent();
    _terrainW->disableSolWidget();

    _lineEditAltitude->setText(QString().setNum(getElement()->getAltitude(), 'f', 2));

    // On deconnecte la detection de modification du texte
    disconnect(_lineEditDistMax, SIGNAL(textChanged(const QString&)), this, SLOT(updateUseDefault()));

    _lineEditDistMax->setText(QString().setNum(getElement()->getDistMax()));

    // On reconnecte apres modification
    connect(_lineEditDistMax, SIGNAL(textChanged(const QString&)), this, SLOT(updateUseDefault()));
}

void TYPlanEauWidget::apply()
{
    _terrainW->apply();

    getElement()->setAltitude(_lineEditAltitude->text().toDouble());

    double valeur = _lineEditDistMax->text().toDouble();

    if (valeur > 0) // Pas de valeur negative ou nulle
    {
        getElement()->setDistMax(valeur);
    }

    getElement()->setIsGeometryModified(true);

    emit modified();
}

void TYPlanEauWidget::setDefaultValue()
{
    // On deconnecte la detection de modification du texte
    disconnect(_lineEditDistMax, SIGNAL(textChanged(const QString&)), this, SLOT(updateUseDefault()));
    // Recuperation de la valeur par defaut
    _lineEditDistMax->setText(QString().setNum(TYCourbeNiveau::getDefaultDistMax()));    //getElement()->getDistMax()) );
    getElement()->setIsDMaxDefault(false);
    _pElement->setIsGeometryModified(true);
    // On reconnecte apres modification
    connect(_lineEditDistMax, SIGNAL(textChanged(const QString&)), this, SLOT(updateUseDefault()));
}

void TYPlanEauWidget::updateUseDefault()
{
    // Recuperation de la valeur par defaut
    getElement()->setIsDMaxDefault(true);
    _pElement->setIsGeometryModified(true);
}
