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
 * \file TYSourceSurfacicWidget.h
 * \brief outil IHM pour une source surfacique (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_SOURCESURFACIC_WIDGET__
#define __TY_SOURCESURFACIC_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYSourceSurfacic;
class QLineEdit;
class QGridLayout;
class QLabel;
class QCheckBox;
class QGroupBox;
class QTreeWidgetItem;
class QTreeWidget;
class TYSourceWidget;


/**
 * \class TYSourceSurfacicWidget
 * \brief classe de l'objet IHM pour une source surfacique
 */
class TYSourceSurfacicWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYSourceSurfacic)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYSourceSurfacicWidget(TYSourceSurfacic* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYSourceSurfacicWidget();


public slots:
    virtual void updateContent();
    virtual void apply();

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
    QLabel* _labeSrcsCor;
    QCheckBox* _checkBoxSrcsCor;
    QLabel* _labelDensiteSrcsH;
    QLineEdit* _lineEditDensiteSrcsH;
    QLineEdit* _lineEditDensiteV;
    QLabel* _labelDensiteSrcsV;
    QGroupBox* _groupBoxListSrc;
    QTreeWidget* _listViewListSrc;

    QGridLayout* _sourceSurfacicLayout;
    QGridLayout* _groupBoxListSrcLayout;
    QGridLayout* _groupBoxLayout;

    TYSourceWidget* _elmW;
};


#endif // __TY_SOURCESURFACIC_WIDGET__
