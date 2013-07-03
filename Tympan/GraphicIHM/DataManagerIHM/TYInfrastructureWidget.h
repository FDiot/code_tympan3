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
 * \file TYInfrastructureWidget.h
 * \brief Outil IHM pour une infrastructure (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_INFRASTRUCTURE_WIDGET__
#define __TY_INFRASTRUCTURE_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYInfrastructure;
class QLineEdit;
class QGridLayout;
class QLabel;
class QTreeWidget;
class QGroupBox;
class QPushButton;
class QGroupBox;
class QTreeWidgetItem;
class TYElementWidget;

/**
 * \class TYInfrastructureWidget
 * \brief Classe de l'objet IHM pour une infrastructure
 */
class TYInfrastructureWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYInfrastructure)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYInfrastructureWidget(TYInfrastructure* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYInfrastructureWidget();


public slots:
    virtual void updateContent();
    virtual void apply();

    /**
     * Edite la widget du batiment selectionne.
     *
     * @param item Element de la liste correspondant au batiment selectionne.
     */
    void editBatiment(QTreeWidgetItem* item);
    /**
     * Edite la widget de la machine selectionnee.
     *
     * @param item Element de la liste correspondant a la machine selectionnee.
     */
    void editMachine(QTreeWidgetItem* item);
    /**
     * Edite la widget du reseau de transport selectionne.
     *
     * @param item Element de la liste correspondant au reseau de transport selectionne.
     */
    void editResTransp(QTreeWidgetItem* item);
    /**
     * Edite la widget de la route selectionnee.
     *
     * @param item Element de la liste correspondant a la route selectionnee.
     */
    void editRoute(QTreeWidgetItem* item);
    /**
     * Edite la widget de la source selectionnee.
     *
     * @param item Element de la liste correspondant a la source selectionnee.
     */
    void editSource(QTreeWidgetItem* item);

    /**
     * Affiche un menu contextuel.
     */
    virtual void contextMenuEvent(QContextMenuEvent* e);


    // Membres
protected:

    QGroupBox* _groupBox;
    QTreeWidget* _listViewList;
    QGroupBox* _groupBox_3;
    QTreeWidget* _listViewList_6;
    QGroupBox* _groupBox_4;
    QTreeWidget* _listViewList_7;
    QGroupBox* _groupBox_2;
    QTreeWidget* _listViewList_5;
    QGroupBox* _groupBox_1;
    QTreeWidget* _listViewList_4;

    QGridLayout* _infrastructureLayout;
    QGridLayout* _groupBoxLayout;
    QGridLayout* _groupBox_3Layout;
    QGridLayout* _groupBox_4Layout;
    QGridLayout* _groupBox_2Layout;
    QGridLayout* _groupBox_1Layout;

    TYElementWidget* _elmW;
};


#endif // __TY_INFRASTRUCTURE_WIDGET__
