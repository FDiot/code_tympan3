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
 * \file TYTerrainWidget.h
 * \brief outil IHM pour un terrain (fichier header)
 * \author Projet_Tympan
 *
 */

#ifndef __TY_TERRAIN_WIDGET__
#define __TY_TERRAIN_WIDGET__

#include <QGridLayout>
#include "TYWidget.h"
#include "Tympan/models/business/TYDefines.h"

class TYTerrain;
class QLineEdit;
class QLabel;
class QTreeWidget;
class QTabWidget;
class QGroupBox;
class QPushButton;
class QTreeWidgetItem;
class QCheckBox;
class TYElementWidget;
class TYColorInterfaceWidget;
class TYSolWidget;
class TYVegetationWidget;
class TabPointsWidget;

/**
 * \class TYTerrainWidget
 * \brief classe de l'objet IHM pour un terrain
 */
class TYTerrainWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYTerrain)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYTerrainWidget(TYTerrain* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYTerrainWidget();

    /**
     * Permet de modifier les parametres du layout.
     *
     * @param margin La valeur de la marge.
     * @param spacing La valeur de l'espacement.
     */
    void setLayoutSpacing(int margin, int spacing)
    {
        _terrainLayout->setMargin(margin);
        _terrainLayout->setSpacing(spacing);
    }

    /**
     * Permet de desactiver le widget Terrain (cas du plan d'eau).
     */
    void disableSolWidget();
    void disableVegetationWidget();


public slots:
    virtual void updateContent();
    virtual void apply();

    void useVegetation();

    // Membres
protected:

    QGroupBox* _groupBoxEdit;
    QGroupBox* _groupBox;
    TabPointsWidget *_tabPoints;

    QGridLayout* _terrainLayout;
    QGridLayout* _groupBoxEditLayout;
    QGridLayout* _groupBoxSolLayout;
    QGridLayout* _groupBoxLayout;

    TYElementWidget* _elmW;
    TYColorInterfaceWidget* _colorW;
    TYSolWidget* _solW;

    // Vegetation
    QCheckBox* _checkBoxVegetActive;
    QLabel* _labelVegetActive;
    TYVegetationWidget* _vegetationWidget;

    QGridLayout* _groupBoxVegetationLayout;

    QTabWidget* _tabWidget;

    LPTYVegetation _pVegetation;
};

#endif // __TY_TERRAIN_WIDGET__
