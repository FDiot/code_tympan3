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
 * \file TYBoundaryNoiseMapWidget.h
 * \brief BoundaryNoiseMap widget (header file)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_BOUNDARY_NOISE_MAP_WIDGET__
#define __TY_BOUNDARY_NOISE_MAP_WIDGET__

#include <QDoubleSpinBox>

#include "TYWidget.h"

class TYBoundaryNoiseMap;
class QCheckBox;
class TYMaillageWidget;
class QDoubleSpinBox;

/**
 * \class TYBoundaryNoiseMapWidget
 * \brief BoundaryNoiseMap widget
 */
class TYBoundaryNoiseMapWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYBoundaryNoiseMap)

    // Methods
public:
    /**
     * Constructor.
     */
    TYBoundaryNoiseMapWidget(TYBoundaryNoiseMap* pElement, QWidget* _pParent = NULL);
    /**
     * Destructor.
     */
    virtual ~TYBoundaryNoiseMapWidget();


public slots:
    /**
     * \brief Update the content of the widgets by reading the stuffs in pElement.
     */
    virtual void updateContent();

    /**
     * \brief When the user validate the dialog window (press ok).
     */
    virtual void apply();

    /**
     * \brief Update the minimum value of the density thanks to the new thickness value.
     * \param thickness The value of thickness to be tested
     */
    void updateMinimumDensity(double thickness);

    // Members
protected:
    /// Density spin box.
    QDoubleSpinBox* _pDensitySpinBox;

    /// Density spin box.
    QDoubleSpinBox* _pThicknessSpinBox;

    /// Closed check box (if the polyline is closed or pen).
    QCheckBox* _pClosedCheckBox;

    /// The maillage widget.
    TYMaillageWidget* _maillageW;
};


#endif // __TY_BOUNDARY_NOISE_MAP_WIDGET__
