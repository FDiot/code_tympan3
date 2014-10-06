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
 * \file TYBoundaryNoiseMapEditor.h
 * \brief Creation of a \c TYBoundaryNoiseMap (header file)
 *
 *
 *
 *
 *
 */


#ifndef __TY_BOUNDARY_NOISE_MAP_EDITOR__
#define __TY_BOUNDARY_NOISE_MAP_EDITOR__

#include <qdialog.h>
#include <qlineedit.h>
#include <qradiobutton.h>
//Added by qt3to4:
#include <QBoxLayout>
#include <QLabel>

#include "TYPolyLineEditor.h"
#include "Tympan/models/business/TYDefines.h"

class QDoubleSpinBox;
class TabPointsWidget;
/**
 * \class TYBoundaryNoiseMapEditor
 * \brief Creation of a \c TYBoundaryNoiseMap
 */
class TYBoundaryNoiseMapEditor : public TYPolyLineEditor
{
    Q_OBJECT

public:
    TYBoundaryNoiseMapEditor(TYSiteModelerFrame* pModeler);
    ~TYBoundaryNoiseMapEditor();

public slots:
    /**
     * \brief Build a BoundaryNoiseMap from an array of points.
     */
    void endBoundaryNoiseMap();

    /**
     * \brief Update the minimum value of the density thanks to the new thickness value.
     * \param thickness The value of thickness to be tested
     */
    void updateMinimumDensity(double thickness);

    /**
     * \brief Return true if the polyline is valid and can be built. It also tells if the polyline should be opened.
     * \param forceOpened If the user will have the choice between a closed/opened polyline.
     * \return True if the polyline can be created.
     */
    bool checkValidity(bool& forceOpened);
    /**
    * \brief Create the properties dialog that will pop up once the user finishes the polyline creation.
    */
    void createPropertiesDlg(bool forceOpened);

    /**
     * \brief When the user confirms the boundary noise map creation after the properties dialog pop up.
     */
    void dialogConfirmed(double height, double thickness, bool closed, double density, bool forceOpened);

    virtual void slotMousePressed(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state);

protected:
    /// Density spin box.
    QDoubleSpinBox* _pDensitySpinBox;

    /// Thickness spin box.
    QDoubleSpinBox* _pThicknessSpinBox;

    /// Geometry description
    TabPointsWidget *_tabPtsW;

};


#endif // __TY_BOUNDARY_NOISE_MAP_EDITOR__
