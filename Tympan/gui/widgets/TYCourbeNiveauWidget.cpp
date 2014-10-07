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
 *
 * \file TYCourbeNiveauWidget.cpp
 * \brief Outil IHM pour une courbe de niveau
 *
 */

#include <qradiobutton.h>
#include <QGridLayout>
#include <QLabel>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QCheckBox>

#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/topography/TYCourbeNiveau.h"
#include "Tympan/gui/widgets/TYColorInterfaceWidget.h"
#include "Tympan/models/business/TYDefines.h"
#include "TabPointsWidget.h"
#include "TYCourbeNiveauWidget.h"

#define TR(id) OLocalizator::getString("TYCourbeNiveauWidget", (id))


TYCourbeNiveauWidget::TYCourbeNiveauWidget(TYCourbeNiveau* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{
    _statusDMax = false;

    _elmW = new TYElementWidget(pElement, this);
    _colorW = new TYColorInterfaceWidget(pElement, this);

    resize(300, 260);
    setWindowTitle(TR("id_caption"));
    _courbeNiveauLayout = new QGridLayout();
    setLayout(_courbeNiveauLayout);

    _courbeNiveauLayout->addWidget(_elmW, 0, 0);
    _courbeNiveauLayout->addWidget(_colorW, 1, 0);

    // Tableau des points de la courbe de niveau
    _groupBox = new QGroupBox(this);
    _groupBox->setTitle(TR("id_pts"));
    _groupBoxLayout = new QGridLayout();
    _groupBox->setLayout(_groupBoxLayout);

    _tabPoints = new TabPointsWidget(pElement->getListPoints(), _groupBox);
    _tabPoints->setEnabled(true);

    _groupBoxLayout->addWidget(_tabPoints, 0, 0);

    // Gestion de la cloture de la courbe de niveau
    _pClosedCheckBox = new QCheckBox();
    _groupBoxLayout->addWidget(new QLabel(TR("id_closed_label")), 1, 0);
    _groupBoxLayout->addWidget(_pClosedCheckBox, 1, 1);

    _courbeNiveauLayout->addWidget(_groupBox, 2, 0);



    // Choix de l'altitude de la courbe de niveau
    _groupBoxAlt = new QGroupBox(this);
    _groupBoxAlt->setTitle(TR(""));
    _groupBoxAltLayout = new QGridLayout();
    _groupBoxAlt->setLayout(_groupBoxAltLayout);

    _labelAltitude = new QLabel(_groupBoxAlt);
    _labelAltitude->setText(TR("id_altitude_label"));
    _groupBoxAltLayout->addWidget(_labelAltitude, 0, 0);

    _lineEditAltitude = new QLineEdit(_groupBoxAlt);
    _groupBoxAltLayout->addWidget(_lineEditAltitude, 0, 1);
    QLabel* pUnitAlt = new QLabel(_groupBoxAlt);
    pUnitAlt->setText(TR("id_unite_altitude"));
    _groupBoxAltLayout->addWidget(pUnitAlt, 0, 2);

    _courbeNiveauLayout->addWidget(_groupBoxAlt, 3, 0);

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

    _courbeNiveauLayout->addWidget(_groupBoxDistMax, 4, 0);

    connect(_pushButtonUseDefault, SIGNAL(clicked()), this, SLOT(setDefaultValue()));
    connect(_lineEditDistMax, SIGNAL(textChanged(const QString&)), this, SLOT(updateUseDefault()));

    updateContent();
}

TYCourbeNiveauWidget::~TYCourbeNiveauWidget()
{
}

void TYCourbeNiveauWidget::updateContent()
{
    _elmW->updateContent();
    _colorW->updateContent();

    _statusDMax = getElement()->getIsDMaxDefault();

    _lineEditAltitude->setText(QString().setNum(getElement()->getAltitude()));

    // On deconnecte la detection de modification du texte
    disconnect(_lineEditDistMax, SIGNAL(textChanged(const QString&)), this, SLOT(updateUseDefault()));

    _lineEditDistMax->setText(QString().setNum(getElement()->getDistMax()));
    _pElement->setIsGeometryModified(true);

    // On reconnecte apres modification
    connect(_lineEditDistMax, SIGNAL(textChanged(const QString&)), this, SLOT(updateUseDefault()));

    bool closed = dynamic_cast<TYCourbeNiveau *>(_pElement)->isClosed();
    // If closed, remove the last point (== 1st point)
    cleanTabPoints( dynamic_cast<TYCourbeNiveau *>(_pElement)->getListPoints(), closed );  
    _pClosedCheckBox->setChecked( closed );
    _tabPoints->update();
}

void TYCourbeNiveauWidget::apply()
{
    _elmW->apply();
    _colorW->apply();

    getElement()->setAltitude(_lineEditAltitude->text().toDouble());

    double valeur = _lineEditDistMax->text().toDouble();

    if (valeur > 0) // Pas de valeur negative ou nulle
    {
        getElement()->setDistMax(valeur);
        getElement()->setIsDMaxDefault(_statusDMax);
    }

    getElement()->setIsGeometryModified(true);

    _tabPoints->apply();

    if ( _pClosedCheckBox->isChecked() )
    { 
        dynamic_cast<TYCourbeNiveau *>(_pElement)->close(true); 
    }
    else
    {
        dynamic_cast<TYCourbeNiveau *>(_pElement)->close(false);
    }

    emit modified();
}

void TYCourbeNiveauWidget::setDefaultValue()
{
    // On deconnecte la detection de modification du texte
    disconnect(_lineEditDistMax, SIGNAL(textChanged(const QString&)), this, SLOT(updateUseDefault()));

    // Recuperation de la valeur par defaut
    _lineEditDistMax->setText(QString().setNum(TYCourbeNiveau::getDefaultDistMax()));    //getElement()->getDistMax()) );
    _statusDMax = false;

    _pElement->setIsGeometryModified(true);

    // On reconnecte apres modification
    connect(_lineEditDistMax, SIGNAL(textChanged(const QString&)), this, SLOT(updateUseDefault()));
}

void TYCourbeNiveauWidget::updateUseDefault()
{
    // Recuperation de la valeur par defaut
    _statusDMax = true;
}

void TYCourbeNiveauWidget::cleanTabPoints(TYTabPoint &tabPts, bool closed)
{
    if (closed)
    {
        tabPts.pop_back();
    }
}
