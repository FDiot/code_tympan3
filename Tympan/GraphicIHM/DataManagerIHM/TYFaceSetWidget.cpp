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
 * \file TYFaceSetWidget.cpp
 * \brief Outil IHM pour un ensemble de faces
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/geometry/TYPolygon.h"
#include "Tympan/models/business/geometry/TYFaceSet.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>



#define TR(id) OLocalizator::getString("TYFaceSetWidget", (id))


TYFaceSetWidget::TYFaceSetWidget(TYFaceSet* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(pElement, _pParent)
{

    resize(300, 540);
    setWindowTitle(TR("id_caption"));
    QGridLayout* _faceSetLayout = new QGridLayout();
    setLayout(_faceSetLayout);

    _groupBoxVolume = new QGroupBox(this);
    _groupBoxVolume->setTitle(TR("id_volume_label"));
    _groupBoxVolumeLayout = new QGridLayout();
    _groupBoxVolume->setLayout(_groupBoxVolumeLayout);

    _groupBoxSurface = new QGroupBox(this);
    _groupBoxSurface->setTitle(TR("id_surface_label"));
    _groupBoxSurfaceLayout = new QGridLayout();
    _groupBoxSurface->setLayout(_groupBoxSurfaceLayout);

    // Presentation du volume
    _lineEditVolume = new QLineEdit(_groupBoxVolume);
    _lineEditVolume->setEnabled(false);
    _groupBoxVolumeLayout->addWidget(_lineEditVolume, 0, 0);

    QLabel* pUnitVolume = new QLabel(_groupBoxVolume);
    pUnitVolume->setText(TR("id_unite_volume"));
    _groupBoxVolumeLayout->addWidget(pUnitVolume, 0, 1);

    // Presentation de la surface
    _lineEditSurface = new QLineEdit(_groupBoxSurface);
    _lineEditSurface->setEnabled(false);
    _groupBoxSurfaceLayout->addWidget(_lineEditSurface, 0, 0);

    QLabel* pUnitSurface = new QLabel(_groupBoxSurface);
    pUnitSurface->setText(TR("id_unite_surface"));
    _groupBoxSurfaceLayout->addWidget(pUnitSurface, 0, 1);

    //Insertion dans la boite de dialogue
    _faceSetLayout->addWidget(_groupBoxVolume, 0, 0);
    _faceSetLayout->addWidget(_groupBoxSurface, 0, 1);

    updateContent();
}

TYFaceSetWidget::~TYFaceSetWidget()
{
}

void TYFaceSetWidget::updateContent()
{
    _lineEditVolume->setText(QString().setNum(getElement()->volume()));
    _lineEditSurface->setText(QString().setNum(getElement()->surface()));
}

void TYFaceSetWidget::apply()
{
    TYElement::setIsSavedOk(true);
    emit modified();
}
