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
 * \file TYAcousticFaceSetWidget.cpp
 * \brief outil IHM pour un ensemble de faces acoustiques
 *
 *
 */



#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/GraphicIHM/DataManagerIHM/TYColorInterfaceWidget.h"
#include "Tympan/models/business/geoacoustic/TYAcousticFaceSet.h"
#include "Tympan/models/business/infrastructure/TYEcran.h"

//Added by qt3to4:
#include <QGridLayout>

#include "Tympan/models/business/OLocalizator.h"
#define TR(id) OLocalizator::getString("TYAcousticFaceSetWidget", (id))


TYAcousticFaceSetWidget::TYAcousticFaceSetWidget(TYAcousticFaceSet* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{

    resize(300, 425);
    setWindowTitle(TR("id_caption"));
    _acousticVolumeLayout = new QGridLayout();
    setLayout(_acousticVolumeLayout);

    _elmW = new TYElementWidget(pElement, this);
    _colorW = new TYColorInterfaceWidget(dynamic_cast<TYColorInterface*>(pElement), this);
    _volumeW = new TYFaceSetWidget(pElement->getFaceSet(), this);

    _acousticVolumeLayout->addWidget(_elmW, 0, 0);
    _acousticVolumeLayout->addWidget(_colorW, 1, 0);
    _acousticVolumeLayout->addWidget(_volumeW, 2, 0);

    _groupBoxMat = new QGroupBox(this);
    _groupBoxMat->setTitle(TR("id_mat"));
    _groupBoxMatLayout = new QGridLayout();
    _groupBoxMat->setLayout(_groupBoxMatLayout);

    _lineEditNomMat = new QLineEdit(_groupBoxMat);
    _lineEditNomMat->setEnabled(FALSE);
    _groupBoxMatLayout->addWidget(_lineEditNomMat, 0, 0);

    _pushButtonMat = new QPushButton(_groupBoxMat);
    _pushButtonMat->setText(TR("id_proprietes_button"));
    _groupBoxMatLayout->addWidget(_pushButtonMat, 0, 1);

    if (getElement()->getMateriau() != NULL)
    {
        _lineEditNomMat->setText(getElement()->getMateriau()->getName());
    }
    else
    {
        _groupBoxMat->setEnabled(FALSE);
    }

    _acousticVolumeLayout->addWidget(_groupBoxMat, 3, 0);

    connect(_pushButtonMat, SIGNAL(clicked()), this, SLOT(editMat()));
}

TYAcousticFaceSetWidget::~TYAcousticFaceSetWidget()
{
}

void TYAcousticFaceSetWidget::updateContent()
{
    _elmW->updateContent();
    _colorW->updateContent();
    _volumeW->updateContent();
}

void TYAcousticFaceSetWidget::apply()
{
    _elmW->apply();
    _colorW->apply();
    _volumeW->apply();

    emit modified();
}

void TYAcousticFaceSetWidget::reject()
{
    // Si l'utilisateur annule, on restitue l'etat du regime courant
    getElement()->setCurRegime(getElement()->getCurRegime());
}

void TYAcousticFaceSetWidget::editMat()
{
    int ret = getElement()->getMateriau()->edit(this);

    if ((ret == QDialog::Accepted) && (getElement()->getMateriau() != NULL))
    {
        _lineEditNomMat->setText(getElement()->getMateriau()->getName());
    }
}
