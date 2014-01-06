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
 * \file TYBoundaryNoiseMapWidget.cpp
 * \brief GUI tool for a TYBoundaryNoiseMap
 *
 *
 */

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include <limits>

#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYBoundaryNoiseMap.h"

//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

#if defined(WIN32)
#include <crtdbg.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif


#define TR(id) OLocalizator::getString("TYBoundaryNoiseMapWidget", (id))

#undef min
#undef max

TYBoundaryNoiseMapWidget::TYBoundaryNoiseMapWidget(TYBoundaryNoiseMap* pElement, QWidget* _pParent /*=NULL*/):
    TYWidget(_pParent)
{
    Q_ASSERT(pElement);
    _pElement = pElement;

    _maillageW = new TYMaillageWidget(pElement, this);

    resize(300, 200);
    setWindowTitle(TR("id_caption"));
    QGridLayout* pBoundaryNoiseMapLayout = new QGridLayout();
    setLayout(pBoundaryNoiseMapLayout );

    pBoundaryNoiseMapLayout ->addWidget(_maillageW, 0, 0);

    QGroupBox* pGroupBox = new QGroupBox(this);
    QGridLayout* pGroupBoxLayout = new QGridLayout();
    pGroupBox->setLayout(pGroupBoxLayout);

    // Thickness
    _pThicknessSpinBox = new QDoubleSpinBox(pGroupBox);
    _pThicknessSpinBox->setRange(0.1, std::numeric_limits<double>::max());
    _pThicknessSpinBox->setCorrectionMode(QAbstractSpinBox::CorrectToNearestValue);	
    pGroupBoxLayout->addWidget(new QLabel(TR("id_thickness"), pGroupBox), 0, 0);
    pGroupBoxLayout->addWidget(_pThicknessSpinBox, 0, 1);

    // Closed or open polyline
    _pClosedCheckBox = new QCheckBox(pGroupBox);
    if(!pElement->getCanBeClosed())
        _pClosedCheckBox->setDisabled(true);
    pGroupBoxLayout->addWidget(new QLabel(TR("id_closed"), pGroupBox), 1, 0);
    pGroupBoxLayout->addWidget(_pClosedCheckBox, 1, 1);

    // Density
    _pDensitySpinBox = new QDoubleSpinBox(pGroupBox);
    _pDensitySpinBox->setSingleStep(0.1);
    _pDensitySpinBox->setCorrectionMode(QAbstractSpinBox::CorrectToNearestValue);	
    pGroupBoxLayout->addWidget(new QLabel(TR("id_density"), pGroupBox), 2, 0);
    pGroupBoxLayout->addWidget(_pDensitySpinBox, 2, 1);
    pGroupBoxLayout->addWidget(new QLabel(TR("id_density_unit"), pGroupBox), 2, 2);

    // The minimum value of thickness and density will be set automatically, because
    // the signal valueChanged() will be triggered by the setValue() functions in updateContent()
    // (that's why we don't use valueEdited() that only works with user input)
    QObject::connect(_pThicknessSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateMinimumDensity(double)));

    pBoundaryNoiseMapLayout ->addWidget(pGroupBox, 1, 0);

    updateContent();
}

TYBoundaryNoiseMapWidget::~TYBoundaryNoiseMapWidget()
{
}

void TYBoundaryNoiseMapWidget::updateContent()
{
    _maillageW->updateContent();

    _pThicknessSpinBox->setValue(getElement()->getThickness());
    _pClosedCheckBox->setChecked(getElement()->isClosed());
    _pDensitySpinBox->setValue(getElement()->getDensity());
}

void TYBoundaryNoiseMapWidget::apply()
{
    _maillageW->apply();

    double thickness = _pThicknessSpinBox->value();
    bool closed = _pClosedCheckBox->isChecked();
    double density = _pDensitySpinBox->value();

    // If something changed
    if( getElement()->getThickness() != thickness || getElement()->isClosed() != closed || getElement()->getDensity() != density )
        getElement()->make(getElement()->getTabPoint(), thickness, closed, density);

    emit modified();
}

void TYBoundaryNoiseMapWidget::updateMinimumDensity(double thickness)
{
    _pDensitySpinBox->setRange(TYBoundaryNoiseMap::computeMinimumDensity(thickness), std::numeric_limits<double>::max());
}

