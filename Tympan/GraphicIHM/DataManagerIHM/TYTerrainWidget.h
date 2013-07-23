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
 *
 *
 *
 *
 */

#ifndef __TY_TERRAIN_WIDGET__
#define __TY_TERRAIN_WIDGET__


#include "TYWidget.h"
#include <qlayout.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYTerrain;
class QLineEdit;
class QGridLayout;
class QLabel;
class QTreeWidget;
class QTabWidget;
class QGroupBox;
class QPushButton;
class QTreeWidgetItem;
class TYElementWidget;
class TYColorInterfaceWidget;
class TYSolWidget;

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
     *
     */
    void disableSolWidget();


public slots:
    virtual void updateContent();
    virtual void apply();

    /**
     * Edite la widget du sol.
     */
    //  void editSol();


    // Membres
protected:

    QGroupBox* _groupBoxEdit;
    //QLabel* _labelType;
    //QLineEdit* _lineEditType;
    //   QGroupBox* _groupBoxSol;
    //   QPushButton* _pushButtonSol;
    //   QLineEdit* _lineEditNomSol;
    QGroupBox* _groupBox;
    QTreeWidget* _listViewTabPt;

    QGridLayout* _terrainLayout;
    QGridLayout* _groupBoxEditLayout;
    QGridLayout* _groupBoxSolLayout;
    QGridLayout* _groupBoxLayout;

    TYElementWidget* _elmW;
    TYColorInterfaceWidget* _colorW;
    TYSolWidget* _solW;

    QTabWidget* _tabWidget;
};


#endif // __TY_TERRAIN_WIDGET__
