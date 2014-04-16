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
 * \file TYVolumeInterfaceWidget.cpp
 * \brief outil IHM pour la gestion des objets de type VolumeInterface
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "TYVolumeInterfaceWidget.h"

//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYVolumeInterface.h"

#include "Tympan/Tools/OLocalizator.h"
#define TR(id) OLocalizator::getString("TYVolumeInterfaceWidget", (id))


TYVolumeInterfaceWidget::TYVolumeInterfaceWidget(TYVolumeInterface* pElement, QWidget* _pParent /*=NULL*/):
    QWidget(_pParent), _pElement(pElement)
{

    resize(300, 540);
    setWindowTitle(TR("id_caption"));
    QGridLayout* _volumeInterfaceLayout = new QGridLayout();
    setLayout(_volumeInterfaceLayout);

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
    _volumeInterfaceLayout->addWidget(_groupBoxVolume, 0, 0);
    _volumeInterfaceLayout->addWidget(_groupBoxSurface, 0, 1);


    updateContent();
}

TYVolumeInterfaceWidget::~TYVolumeInterfaceWidget()
{
}

void TYVolumeInterfaceWidget::updateContent()
{
    _lineEditVolume->setText(QString().setNum(getElement()->volume()));
    _lineEditSurface->setText(QString().setNum(getElement()->surface()));
}

void TYVolumeInterfaceWidget::apply()
{
    TYElement::setIsSavedOk(true);

    //emit modified();
}
