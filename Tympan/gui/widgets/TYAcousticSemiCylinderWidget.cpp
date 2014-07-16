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
 * \file TYAcousticSemiCylinderWidget.cpp
 * \brief outil IHM pour un demi cylindre acoustique
 *
 *
 */




#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/geoacoustic/TYAcousticSemiCylinder.h"
//Added by qt3to4:
#include <QGridLayout>


#define TR(id) OLocalizator::getString("TYAcousticSemiCylinderWidget", (id))


TYAcousticSemiCylinderWidget::TYAcousticSemiCylinderWidget(TYAcousticSemiCylinder* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{

    _elmW = new TYAcousticVolumeWidget(pElement, this);

    resize(300, 650);
    setWindowTitle(TR("id_caption"));
    _acousticSemiCylinderLayout = new QGridLayout();
    setLayout(_acousticSemiCylinderLayout);

    _acousticSemiCylinderLayout->addWidget(_elmW, 0, 0);

    _groupBoxEnveloppe = new QGroupBox(this);
    _groupBoxEnveloppe->setTitle(TR("id_enveloppe"));
    _groupBoxEnveloppeLayout = new QGridLayout();
    _groupBoxEnveloppe->setLayout(_groupBoxEnveloppeLayout);

    _checkBoxEnveloppe = new QCheckBox(_groupBoxEnveloppe);
    _groupBoxEnveloppeLayout->addWidget(_checkBoxEnveloppe, 0, 0);

    _lineEditNomEnveloppe = new QLineEdit(_groupBoxEnveloppe);
    _lineEditNomEnveloppe->setText(TR("spectre d'attenuation"));
    _lineEditNomEnveloppe->setEnabled(FALSE);
    _groupBoxEnveloppeLayout->addWidget(_lineEditNomEnveloppe, 0, 1);

    _pushButtonEnveloppe = new QPushButton(_groupBoxEnveloppe);
    _pushButtonEnveloppe->setText(TR("id_proprietes_button"));
    _groupBoxEnveloppeLayout->addWidget(_pushButtonEnveloppe, 0, 2);

    _acousticSemiCylinderLayout->addWidget(_groupBoxEnveloppe, 1, 0);

    _groupBoxCircTop = new QGroupBox(this);
    _groupBoxCircTop->setTitle(TR("id_circtop"));
    _groupBoxCircTopLayout = new QGridLayout();
    _groupBoxCircTop->setLayout(_groupBoxCircTopLayout);

    _checkBoxSemiCircTop = new QCheckBox(_groupBoxCircTop);
    _groupBoxCircTopLayout->addWidget(_checkBoxSemiCircTop, 0, 0);

    _lineEditNomCircTop = new QLineEdit(_groupBoxCircTop);
    _lineEditNomCircTop->setText(getElement()->getSemiCircTop()->getName());
    _lineEditNomCircTop->setEnabled(FALSE);
    _groupBoxCircTopLayout->addWidget(_lineEditNomCircTop, 0, 1);

    _pushButtonSemicircTop = new QPushButton(_groupBoxCircTop);
    _pushButtonSemicircTop->setText(TR("id_proprietes_button"));
    _groupBoxCircTopLayout->addWidget(_pushButtonSemicircTop, 0, 2);

    _acousticSemiCylinderLayout->addWidget(_groupBoxCircTop, 2, 0);

    _groupBoxCircBottom = new QGroupBox(this);
    _groupBoxCircBottom->setTitle(TR("id_circbottom"));
    _groupBoxCircBottomLayout = new QGridLayout();
    _groupBoxCircBottom->setLayout(_groupBoxCircBottomLayout);

    _checkBoxSemiCircBottom = new QCheckBox(_groupBoxCircBottom);
    _groupBoxCircBottomLayout->addWidget(_checkBoxSemiCircBottom, 0, 0);

    _lineEditNomCircBottom = new QLineEdit(_groupBoxCircBottom);
    _lineEditNomCircBottom->setText(getElement()->getSemiCircBottom()->getName());
    _lineEditNomCircBottom->setEnabled(FALSE);
    _groupBoxCircBottomLayout->addWidget(_lineEditNomCircBottom, 0, 1);

    _pushButtonSemiCircBottom = new QPushButton(_groupBoxCircBottom);
    _pushButtonSemiCircBottom->setText(TR("id_proprietes_button"));
    _groupBoxCircBottomLayout->addWidget(_pushButtonSemiCircBottom, 0, 2);

    _acousticSemiCylinderLayout->addWidget(_groupBoxCircBottom, 3, 0);

    _groupBoxRect = new QGroupBox(this);
    _groupBoxRect->setTitle(TR("id_rect"));
    _groupBoxRectLayout = new QGridLayout();
    _groupBoxRect->setLayout(_groupBoxRectLayout);

    _checkBoxRect = new QCheckBox(_groupBoxRect);
    _groupBoxRectLayout->addWidget(_checkBoxRect, 0, 0);

    _lineEditNomRect = new QLineEdit(_groupBoxRect);
    _lineEditNomRect->setText(getElement()->getRect()->getName());
    _lineEditNomRect->setEnabled(FALSE);
    _groupBoxRectLayout->addWidget(_lineEditNomRect, 0, 1);

    _pushButtonRect = new QPushButton(_groupBoxRect);
    _pushButtonRect->setText(TR("id_proprietes_button"));
    _groupBoxRectLayout->addWidget(_pushButtonRect, 0, 2);

    _acousticSemiCylinderLayout->addWidget(_groupBoxRect, 4, 0);

    connect(_pushButtonEnveloppe, SIGNAL(clicked()), this, SLOT(editEnveloppe()));
    connect(_pushButtonSemicircTop, SIGNAL(clicked()), this, SLOT(editSemiCircTop()));
    connect(_pushButtonSemiCircBottom, SIGNAL(clicked()), this, SLOT(editSemiCircBottom()));
    connect(_pushButtonRect, SIGNAL(clicked()), this, SLOT(editRect()));


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

    _acousticSemiCylinderLayout->addWidget(_groupBoxDimensions, 5, 0);


    updateContent();
}

TYAcousticSemiCylinderWidget::~TYAcousticSemiCylinderWidget()
{
}

void TYAcousticSemiCylinderWidget::updateContent()
{
    _elmW->updateContent();

    _lineEditNomEnveloppe->setText(getElement()->getAcEnveloppe()->getName());
    _lineEditNomRect->setText(getElement()->getRect()->getName());
    _lineEditNomCircBottom->setText(getElement()->getSemiCircBottom()->getName());
    _lineEditNomCircTop->setText(getElement()->getSemiCircTop()->getName());

    _checkBoxEnveloppe->setChecked(getElement()->getAcEnveloppe()->getIsRayonnant());
    _checkBoxSemiCircTop->setChecked(getElement()->getSemiCircTop()->getIsRayonnant());
    _checkBoxSemiCircBottom->setChecked(getElement()->getSemiCircBottom()->getIsRayonnant());
    _checkBoxRect->setChecked(getElement()->getRect()->getIsRayonnant());

    _dimensionsDiamLineEdit->setText(QString().setNum(((TYAcousticSemiCylinder*) _pElement)->getDiameter(), 'f', 2));
    _dimensionsHauteurLineEdit->setText(QString().setNum(((TYAcousticSemiCylinder*) _pElement)->getHauteur(), 'f', 2));
}

void TYAcousticSemiCylinderWidget::apply()
{
    _elmW->apply();

    getElement()->getAcEnveloppe()->setIsRayonnant(_checkBoxEnveloppe->isChecked());
    getElement()->getSemiCircTop()->setIsRayonnant(_checkBoxSemiCircTop->isChecked());
    getElement()->getSemiCircBottom()->setIsRayonnant(_checkBoxSemiCircBottom->isChecked());
    getElement()->getRect()->setIsRayonnant(_checkBoxRect->isChecked());

    double diameter = _dimensionsDiamLineEdit->text().toDouble();
    double hauteur = _dimensionsHauteurLineEdit->text().toDouble();

    ((TYAcousticSemiCylinder*) _pElement)->setDiameter(diameter);
    ((TYAcousticSemiCylinder*) _pElement)->setHauteur(hauteur);

    emit modified();
}

void TYAcousticSemiCylinderWidget::editEnveloppe()
{
    int ret = getElement()->getAcEnveloppe()->edit(this);

    if (ret == QDialog::Accepted)
    {
        _lineEditNomEnveloppe->setText(TR("spectre d'attenuation"));
    }

    _checkBoxEnveloppe->setChecked(getElement()->getAcEnveloppe()->getIsRayonnant());
}

void TYAcousticSemiCylinderWidget::editSemiCircTop()
{
    getElement()->getSemiCircTop()->setIsRayonnant(_checkBoxSemiCircTop->isChecked());

    int ret = getElement()->getSemiCircTop()->edit(this);

    if (ret == QDialog::Accepted)
    {
        _lineEditNomCircTop->setText(getElement()->getSemiCircTop()->getName());
    }

    _checkBoxSemiCircTop->setChecked(getElement()->getSemiCircTop()->getIsRayonnant());
}

void TYAcousticSemiCylinderWidget::editSemiCircBottom()
{
    getElement()->getSemiCircBottom()->setIsRayonnant(_checkBoxSemiCircBottom->isChecked());

    int ret = getElement()->getSemiCircBottom()->edit(this);

    if (ret == QDialog::Accepted)
    {
        _lineEditNomCircBottom->setText(getElement()->getSemiCircBottom()->getName());
    }

    _checkBoxSemiCircBottom->setChecked(getElement()->getSemiCircBottom()->getIsRayonnant());
}

void TYAcousticSemiCylinderWidget::editRect()
{
    getElement()->getRect()->setIsRayonnant(_checkBoxRect->isChecked());

    int ret = getElement()->getRect()->edit(this);

    if (ret == QDialog::Accepted)
    {
        _lineEditNomRect->setText(getElement()->getRect()->getName());
    }

    _checkBoxRect->setChecked(getElement()->getRect()->getIsRayonnant());
}

