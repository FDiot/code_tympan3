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
 * \file TYRotationDialog.cpp
 * \brief Boite de dialogue pour la saisie de la l'angle des objets
 *
 *
 */



#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/GraphicIHM/DataManagerIHM/TYIncludesQT.h"
#include "Tympan/GraphicIHM/DataManagerIHM/TYElementWidget.h"
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/ToolsMetier/OPoint3D.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

#define TR(id) OLocalizator::getString("TYRotationDialog", (id))


TYRotationDialog::TYRotationDialog(OPoint3D* pElement, QWidget* _pParent /*=NULL*/):
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

    _labelZ = new QLabel(_groupBox);
    _labelZ->setText(TR("id_z_label"));
    _groupBoxLayout->addWidget(_labelZ, 2, 0);

    _labelY = new QLabel(_groupBox);
    _labelY->setText(TR("id_y_label"));
    _groupBoxLayout->addWidget(_labelY, 1, 0);

    _lineEditX = new QLineEdit(_groupBox);
    _groupBoxLayout->addWidget(_lineEditX, 0, 1);

    _lineEditY = new QLineEdit(_groupBox);
    _groupBoxLayout->addWidget(_lineEditY, 1, 1);

    _lineEditZ = new QLineEdit(_groupBox);
    _groupBoxLayout->addWidget(_lineEditZ, 2, 1);

    _pointLayout->addWidget(_groupBox, 0, 0);

    // Ajout de la checkBox de concatenation des rotations
    _pConcatenateCheckBox = new QCheckBox(TR("id_concatenate_checkbox"), this);
    _pConcatenateCheckBox->setChecked(false);
    _pointLayout->addWidget(_pConcatenateCheckBox, 1, 0);

    // Bonton ok et cancel
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

    _pointLayout->addLayout(pBtnLayout, 2, 0);

    updateContent();
}

TYRotationDialog::~TYRotationDialog()
{
}

void TYRotationDialog::switchConcatenate()
{
    if (_pConcatenateCheckBox->isChecked() == true)
    {
        _pConcatenateCheckBox->setChecked(false);
    }
    else
    {
        _pConcatenateCheckBox->setChecked(true);
    }
}

void TYRotationDialog::updateContent()
{
    _lineEditX->setText(QString().setNum(_pElement->_x, 'f', 2));
    _lineEditY->setText(QString().setNum(_pElement->_y, 'f', 2));
    _lineEditZ->setText(QString().setNum(_pElement->_z, 'f', 2));
}

void TYRotationDialog::apply()
{
    _pElement->_x = _lineEditX->text().toDouble();
    _pElement->_y = _lineEditY->text().toDouble();
    _pElement->_z = _lineEditZ->text().toDouble();

    accept();

}

void TYRotationDialog::lockXY()
{
    _lineEditX->setText("0.0");
    _lineEditX->setEnabled(false);
    _lineEditY->setText("0.0");
    _lineEditY->setEnabled(false);
}
