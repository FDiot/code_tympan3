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
 * \file TYSurfaceInterfaceWidget.cpp
 * \brief outil IHM pour la gestion des objets de type SurfaceInterface
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

#include "Tympan/Tools/OLocalizator.h"
#define TR(id) OLocalizator::getString("TYSurfaceInterfaceWidget", (id))

#include "Tympan/MetierSolver/CommonTools/OVector3D.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYPoint.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYSurfaceInterface.h"
#include "Tympan/GraphicIHM/DataManagerIHM/TYSurfaceInterfaceWidget.h"

TYSurfaceInterfaceWidget::TYSurfaceInterfaceWidget(TYSurfaceInterface* pElement, QWidget* _pParent /*=NULL*/):
    QWidget(_pParent), _pElement(pElement)
{

    resize(300, 540);
    setWindowTitle(TR("id_caption"));
    QGridLayout* _surfaceInterfaceLayout = new QGridLayout();

    _groupBox = new QGroupBox(this);
    _groupBox->setTitle(TR(""));
    _groupBoxLayout = new QGridLayout();
    _groupBox->setLayout(_groupBoxLayout);

    _lineEditAire = new QLineEdit(_groupBox);
    _lineEditAire->setEnabled(false);
    _groupBoxLayout->addWidget(_lineEditAire, 0, 1);

    _labelAire = new QLabel(_groupBox);
    _labelAire->setText(TR("id_aire_label"));
    _groupBoxLayout->addWidget(_labelAire, 0, 0);

    QLabel* pUnit = new QLabel(_groupBox);
    pUnit->setText(TR("id_unite_aire"));
    _groupBoxLayout->addWidget(pUnit, 0, 2);

    _surfaceInterfaceLayout->addWidget(_groupBox, 0, 0);

    updateContent();
}

TYSurfaceInterfaceWidget::~TYSurfaceInterfaceWidget()
{
}

void TYSurfaceInterfaceWidget::updateContent()
{
    _lineEditAire->setText(QString().setNum(getElement()->surface()));
}

void TYSurfaceInterfaceWidget::apply()
{
    TYElement::setIsSavedOk(true);
    //emit modified();
}
