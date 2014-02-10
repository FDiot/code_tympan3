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
 * \file TYAcousticCylinderWidget.cpp
 * \brief outil IHM pour un cylindre acoustique
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticCylinder.h"
//Added by qt3to4:
#include <QGridLayout>

#define TR(id) OLocalizator::getString("TYAcousticCylinderWidget", (id))


TYAcousticCylinderWidget::TYAcousticCylinderWidget(TYAcousticCylinder* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(_pParent)
{
    Q_ASSERT(pElement);
    _pElement = pElement;

    resize(300, 700);
    setWindowTitle(TR("id_caption"));
    _acousticCylinderLayout = new QGridLayout();
    setLayout(_acousticCylinderLayout);

    _elmW = new TYAcousticVolumeWidget(pElement, this);

    _acousticCylinderLayout->addWidget(_elmW, 0, 0);

    _groupBoxEnveloppe = new QGroupBox(this);
    _groupBoxEnveloppe->setTitle(TR("id_enveloppe"));
    _groupBoxEnveloppeLayout = new QGridLayout();
    _groupBoxEnveloppe->setLayout(_groupBoxEnveloppeLayout);

    _checkBoxEnveloppe = new QCheckBox(_groupBoxEnveloppe);
    _groupBoxEnveloppeLayout->addWidget(_checkBoxEnveloppe, 0, 0);

    _lineEditNomEnveloppe = new QLineEdit(_groupBoxEnveloppe);
    _lineEditNomEnveloppe->setText(getElement()->getAcEnveloppe()->getName());
    _lineEditNomEnveloppe->setEnabled(FALSE);
    _groupBoxEnveloppeLayout->addWidget(_lineEditNomEnveloppe, 0, 1);

    _pushButtonEnveloppe = new QPushButton(_groupBoxEnveloppe);
    _pushButtonEnveloppe->setText(TR("id_proprietes_button"));
    _groupBoxEnveloppeLayout->addWidget(_pushButtonEnveloppe, 0, 2);

    _acousticCylinderLayout->addWidget(_groupBoxEnveloppe, 1, 0);

    _groupBoxCircTop = new QGroupBox(this);
    _groupBoxCircTop->setTitle(TR("id_circtop"));
    _groupBoxCircTopLayout = new QGridLayout();
    _groupBoxCircTop->setLayout(_groupBoxCircTopLayout);

    _checkBoxCircTop = new QCheckBox(_groupBoxCircTop);
    _groupBoxCircTopLayout->addWidget(_checkBoxCircTop, 0, 0);

    _lineEditNomCircTop = new QLineEdit(_groupBoxCircTop);
    _lineEditNomCircTop->setText(getElement()->getCircTop()->getName());
    _lineEditNomCircTop->setEnabled(FALSE);
    _groupBoxCircTopLayout->addWidget(_lineEditNomCircTop, 0, 1);

    _pushButtonCircTop = new QPushButton(_groupBoxCircTop);
    _pushButtonCircTop->setText(TR("id_proprietes_button"));
    _groupBoxCircTopLayout->addWidget(_pushButtonCircTop, 0, 2);

    _acousticCylinderLayout->addWidget(_groupBoxCircTop, 2, 0);

    _groupBoxCircBottom = new QGroupBox(this);
    _groupBoxCircBottom->setTitle(TR("id_circbottom"));
    _groupBoxCircBottomLayout = new QGridLayout();
    _groupBoxCircBottom->setLayout(_groupBoxCircBottomLayout);

    _checkBoxCircBottom = new QCheckBox(_groupBoxCircBottom);
    _groupBoxCircBottomLayout->addWidget(_checkBoxCircBottom, 0, 0);

    _lineEditNomCircBottom = new QLineEdit(_groupBoxCircBottom);
    _lineEditNomCircBottom->setText(getElement()->getCircBottom()->getName());
    _lineEditNomCircBottom->setEnabled(FALSE);
    _groupBoxCircBottomLayout->addWidget(_lineEditNomCircBottom, 0, 1);

    _pushButtonCircBottom = new QPushButton(_groupBoxCircBottom);
    _pushButtonCircBottom->setText(TR("id_proprietes_button"));
    _groupBoxCircBottomLayout->addWidget(_pushButtonCircBottom, 0, 2);

    _acousticCylinderLayout->addWidget(_groupBoxCircBottom, 3, 0);

    _groupBoxDimensions = new QGroupBox(this);
    _groupBoxDimensions->setTitle(TR("id_title_dimensions"));
    _groupBoxDimensionsLayout = new QGridLayout();
    _groupBoxDimensions->setLayout(_groupBoxDimensionsLayout);

    _dimensionsDiamLabel = new QLabel("labelName");
    _dimensionsDiamLabel->setText(TR("id_diameter_label"));
    _dimensionsDiamLineEdit = new QLineEdit("");

    _dimensionsHauteurLabel = new QLabel("labelName");
    _dimensionsHauteurLabel->setText(TR("id_hauteur_label"));
    _dimensionsHauteurLineEdit = new QLineEdit("");

    _groupBoxDimensionsLayout->addWidget(_dimensionsDiamLabel, 0, 0);
    _groupBoxDimensionsLayout->addWidget(_dimensionsDiamLineEdit, 0, 1);
    _groupBoxDimensionsLayout->addWidget(_dimensionsHauteurLabel, 0, 2);
    _groupBoxDimensionsLayout->addWidget(_dimensionsHauteurLineEdit, 0, 3);

    _acousticCylinderLayout->addWidget(_groupBoxDimensions, 4, 0);

    connect(_pushButtonEnveloppe, SIGNAL(clicked()), this, SLOT(editEnveloppe()));
    connect(_pushButtonCircTop, SIGNAL(clicked()), this, SLOT(editCircTop()));
    connect(_pushButtonCircBottom, SIGNAL(clicked()), this, SLOT(editCircBottom()));

    updateContent();
}

TYAcousticCylinderWidget::~TYAcousticCylinderWidget()
{
}

void TYAcousticCylinderWidget::updateContent()
{
    _elmW->updateContent();

    _lineEditNomEnveloppe->setText(getElement()->getAcEnveloppe()->getName());
    _lineEditNomCircTop->setText(getElement()->getCircTop()->getName());
    _lineEditNomCircBottom->setText(getElement()->getCircBottom()->getName());

    _checkBoxEnveloppe->setChecked(getElement()->getAcEnveloppe()->getIsRayonnant());
    _checkBoxCircTop->setChecked(getElement()->getCircTop()->getIsRayonnant());
    _checkBoxCircBottom->setChecked(getElement()->getCircBottom()->getIsRayonnant());

    _dimensionsDiamLineEdit->setText(QString().setNum(getElement()->getDiameter(), 'f', 2));
    _dimensionsHauteurLineEdit->setText(QString().setNum(getElement()->getHauteur(), 'f', 2));
}

void TYAcousticCylinderWidget::apply()
{
    _elmW->apply();

    getElement()->getAcEnveloppe()->setIsRayonnant(_checkBoxEnveloppe->isChecked());
    getElement()->getCircTop()->setIsRayonnant(_checkBoxCircTop->isChecked());
    getElement()->getCircBottom()->setIsRayonnant(_checkBoxCircBottom->isChecked());

    double diameter = _dimensionsDiamLineEdit->text().toDouble();
    double hauteur = _dimensionsHauteurLineEdit->text().toDouble();

    getElement()->setDiameter(diameter);
    getElement()->setHauteur(hauteur);

    emit modified();
}

void TYAcousticCylinderWidget::editEnveloppe()
{
    getElement()->getAcEnveloppe()->setIsRayonnant(_checkBoxEnveloppe->isChecked());

    int ret = getElement()->getAcEnveloppe()->edit(this);

    if (ret == QDialog::Accepted)
    {
        _lineEditNomEnveloppe->setText(getElement()->getAcEnveloppe()->getName());
    }

    _checkBoxEnveloppe->setChecked(getElement()->getAcEnveloppe()->getIsRayonnant());
}

void TYAcousticCylinderWidget::editCircTop()
{
    getElement()->getCircTop()->setIsRayonnant(_checkBoxCircTop->isChecked());

    int ret = getElement()->getCircTop()->edit(this);

    if (ret == QDialog::Accepted)
    {
        _lineEditNomCircTop->setText(getElement()->getCircTop()->getName());
    }

    _checkBoxCircTop->setChecked(getElement()->getCircTop()->getIsRayonnant());
}

void TYAcousticCylinderWidget::editCircBottom()
{
    getElement()->getCircBottom()->setIsRayonnant(_checkBoxCircBottom->isChecked());

    int ret = getElement()->getCircBottom()->edit(this);

    if (ret == QDialog::Accepted)
    {
        _lineEditNomCircBottom->setText(getElement()->getCircBottom()->getName());
    }

    _checkBoxCircBottom->setChecked(getElement()->getCircBottom()->getIsRayonnant());
}
