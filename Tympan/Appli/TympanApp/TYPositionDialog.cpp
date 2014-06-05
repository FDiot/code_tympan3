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
 * \file TYPositionDialog.cpp
 * \brief Widget pour la classe TYPoint
 *
 *
 */


#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/GraphicIHM/DataManagerIHM/TYIncludesQT.h"
#include "Tympan/GraphicIHM/DataManagerIHM/TYElementWidget.h"
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/CommonTools/OPoint3D.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYGeometryNode.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QBoxLayout>
#include <QHBoxLayout>
#include <QLabel>


#define TR(id) OLocalizator::getString("TYPositionDialog", (id))


TYPositionDialog::TYPositionDialog(TYGeometryNode* pElement, QWidget* _pParent /*=NULL*/):
    QDialog(_pParent)
{
    Q_ASSERT(pElement);
    _pElement = pElement;

    resize(300, 174);
    setWindowTitle(TR("id_caption"));
    _pointLayout = new QGridLayout();
    setLayout(_pointLayout);

    _groupBox = new QGroupBox(this);
    _groupBox->setTitle(TR(""));
    _groupBoxLayout = new QGridLayout();
    _groupBox->setLayout(_groupBoxLayout);

    _labelX = new QLabel(_groupBox);
    _labelX->setText(TR("id_x_label"));
    _groupBoxLayout->addWidget(_labelX, 0, 0);

    _lineEditX = new QLineEdit(_groupBox);
    _groupBoxLayout->addWidget(_lineEditX, 0, 1);

    _labelY = new QLabel(_groupBox);
    _labelY->setText(TR("id_y_label"));
    _groupBoxLayout->addWidget(_labelY, 1, 0);

    _lineEditY = new QLineEdit(_groupBox);
    _groupBoxLayout->addWidget(_lineEditY, 1, 1);

    _labelZ = new QLabel(_groupBox);
    _labelZ->setText(TR("id_z_label"));
    _groupBoxLayout->addWidget(_labelZ, 2, 0);

    _lineEditZ = new QLineEdit(_groupBox);
    _groupBoxLayout->addWidget(_lineEditZ, 2, 1);

    _labelHauteur = new QLabel(_groupBox);
    _labelHauteur->setText(TR("id_h_label"));
    _groupBoxLayout->addWidget(_labelHauteur, 4, 0);

    _lineEditH = new QLineEdit(_groupBox);
    _groupBoxLayout->addWidget(_lineEditH, 4, 1);

    _pointLayout->addWidget(_groupBox, 0, 0);

    QBoxLayout* pBtnLayout = new QHBoxLayout();

    pBtnLayout->addStretch(1);

    QPushButton* pButtonOK = new QPushButton(TR("id_ok_btn"), this);
    pButtonOK->setDefault(true);
    QObject::connect(pButtonOK, SIGNAL(clicked()), this, SLOT(apply()));
    pBtnLayout->addWidget(pButtonOK);

    QPushButton* pButtonCancel = new QPushButton(TR("id_cancel_btn"), this);
    pButtonCancel->setShortcut(Qt::Key_Escape);
    QObject::connect(pButtonCancel, SIGNAL(clicked()), this, SLOT(reject()));
    pBtnLayout->addWidget(pButtonCancel);

    _pointLayout->addLayout(pBtnLayout, 1, 0);

    _bHauteurEnable = true;

    updateContent();
}

TYPositionDialog::~TYPositionDialog()
{
}

void TYPositionDialog::updateContent()
{
    _lineEditX->setText(QString().setNum(_pElement->getORepere3D()._origin._x, 'f', 2));
    _lineEditY->setText(QString().setNum(_pElement->getORepere3D()._origin._y, 'f', 2));
    _lineEditZ->setText(QString().setNum(_pElement->getORepere3D()._origin._z, 'f', 2));
    _lineEditH->setText(QString().setNum(_pElement->getHauteur(), 'f', 2));

    // Quand on doit saisir la hauteur, le Z est desactive et incersement
    _lineEditH->setEnabled(_bHauteurEnable);
    _lineEditZ->setEnabled(!_bHauteurEnable);

    // Cas particulier d'un siteNode

    if (dynamic_cast<TYSiteNode*>(_pElement->getElement()) != nullptr)
    {
        _lineEditZ->setEnabled(false);
    }

    // Cas particulier d'un maillage
    if ((dynamic_cast<TYMaillage*>(_pElement->getElement()) != nullptr)
        || (dynamic_cast<TYAcousticSurface*>(_pElement->getElement()) != nullptr))
    {
        _bHauteurEnable = false;
        _lineEditZ->setEnabled(false);
        _lineEditH->setEnabled(false);
    }
}

void TYPositionDialog::apply()
{
    _pElement->getORepere3D()._origin._x = _lineEditX->text().toDouble();
    _pElement->getORepere3D()._origin._y = _lineEditY->text().toDouble();
    _pElement->getORepere3D()._origin._z = _lineEditZ->text().toDouble();

    _pElement->setHauteur(_lineEditH->text().toDouble());

    TYElement::setIsSavedOk(true);
    accept();
}
