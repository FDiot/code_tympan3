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
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include <qradiobutton.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "Tympan/models/business/OLocalizator.h"
#define TR(id) OLocalizator::getString("TYCourbeNiveauWidget", (id))

#include "Tympan/models/business/topography/TYCourbeNiveau.h"
#include "Tympan/GraphicIHM/DataManagerIHM/TYColorInterfaceWidget.h"


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

    _listViewTabPt = new QTreeWidget(_groupBox);
    QStringList stringList;
    stringList.append(TR("id_x"));
    stringList.append(TR("id_y"));
    stringList.append(TR("id_z"));
    _listViewTabPt->setColumnCount(3);
    _listViewTabPt->setHeaderLabels(stringList);
    _listViewTabPt->setSelectionMode(QTreeWidget::NoSelection);

    _groupBoxLayout->addWidget(_listViewTabPt, 0, 0);

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

    _listViewTabPt->clear();

    for (unsigned int i = 0; i < getElement()->getListPoints().size(); i++)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(_listViewTabPt, 0);
        item->setText(0, QString().setNum(getElement()->getListPoints()[i]._x, 'f', 2));
        item->setText(1, QString().setNum(getElement()->getListPoints()[i]._y, 'f', 2));
        item->setText(2, QString().setNum(getElement()->getListPoints()[i]._z, 'f', 2));
    }
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
    //  getElement()->setIsDMaxDefault(false);

    // On reconnecte apres modification
    connect(_lineEditDistMax, SIGNAL(textChanged(const QString&)), this, SLOT(updateUseDefault()));
}

void TYCourbeNiveauWidget::updateUseDefault()
{
    // Recuperation de la valeur par defaut
    //  getElement()->setIsDMaxDefault(true);
    _statusDMax = true;
}
