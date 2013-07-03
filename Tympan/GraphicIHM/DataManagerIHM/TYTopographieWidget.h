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
 * \file TYTopographieWidget.h
 * \brief outil IHM pour une topographie (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_TOPOGRAPHIE_WIDGET__
#define __TY_TOPOGRAPHIE_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYTopographie;
class QLineEdit;
class QGridLayout;
class QLabel;
class QTreeWidget;
class QGroupBox;
class QPushButton;
class QTreeWidgetItem;
class TYElementWidget;

/**
 * \class TYTopographieWidget
 * \brief classe de l'objet IHM pour une topographie
 */
class TYTopographieWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYTopographie)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYTopographieWidget(TYTopographie* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYTopographieWidget();


public slots:
    virtual void updateContent();
    virtual void apply();
    /**
     * Edite la widget du terrain par default.
     */
    void editDefTerrain();

    /**
     * Edite la widget du terrain selectionne.
     *
     * @param item Element de la liste correspondant au terrain selectionne.
     */
    void editTerrain(QTreeWidgetItem* item);
    /**
     * Edite la widget du cours d'eau selectionne.
     *
     * @param item Element de la liste correspondant au cours d'eau selectionne.
     */
    void editCrsEau(QTreeWidgetItem* item);
    /**
     * Edite la widget du plan d'eau selectionne.
     *
     * @param item Element de la liste correspondant au plan d'eau selectionne.
     */
    void editPlanEau(QTreeWidgetItem* item);
    /**
     * Edite la widget de la courbe de niveau selectionnee.
     *
     * @param item Element de la liste correspondant a la courbe de niveau selectionnee.
     */
    void editCrbNiv(QTreeWidgetItem* item);

    /**
     * Affiche un menu contextuel.
     */
    virtual void contextMenuEvent(QContextMenuEvent* e);


    // Membres
protected:
    QGroupBox* _groupBoxTerrainDef;
    QPushButton* _pushButtonTerrainDef;
    QLineEdit* _lineEditNomTerrainDef;
    QGroupBox* _groupBox;
    QTreeWidget* _listView;
    QGroupBox* _groupBox_2;
    QTreeWidget* _listView_5;
    QGroupBox* _groupBoxPlanEau;
    QTreeWidget* _listViewPlanEau;
    QGroupBox* _groupBox_3;
    QGroupBox* _groupBox_4;
    QTreeWidget* _listView_6;
    QTreeWidget* _listView_7;

    QGridLayout* _topographieLayout;
    QGridLayout* _groupBox_5Layout;
    QGridLayout* _groupBoxTerrainDefLayout;
    QGridLayout* _groupBoxLayout;
    QGridLayout* _groupBox_2Layout;
    QGridLayout* _groupBoxPlanEauLayout;
    QGridLayout* _groupBox_3Layout;
    QGridLayout* _groupBox_4Layout;

    TYElementWidget* _elmW;
};


#endif // __TY_TOPOGRAPHIE_WIDGET__
