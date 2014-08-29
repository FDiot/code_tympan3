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
 * \file TYSourceLineicWidget.h
 * \brief outil IHM pour une source lineique (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 *
 */

#ifndef __TY_SOURCELINEIC_WIDGET__
#define __TY_SOURCELINEIC_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYSourceLineic;
class QLineEdit;
class QGridLayout;
class QLabel;
class QTreeWidget;
class QCheckBox;
class QGroupBox;
class QTreeWidgetItem;
class TYSourceWidget;


/**
 * \class TYSourceLineicWidget
 * \brief classe de l'objet IHM pour une source lineique
 */
class TYSourceLineicWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYSourceLineic)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYSourceLineicWidget(TYSourceLineic* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYSourceLineicWidget();


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

    QGroupBox* _groupBoxListSrc;
    QLineEdit* _lineEditDensiteSrcs;
    QLabel* _labelDensiteSrcs;
    QLabel* _labeSrcsCor;
    QCheckBox* _checkBoxSrcsCor;
    QGroupBox* _groupBoxEdit;
    QTreeWidget* _listViewListSrc;

    QGridLayout* _sourceLineicLayout;
    QGridLayout* _groupBoxListSrcLayout;
    QGridLayout* _groupBoxEditLayout;

    TYSourceWidget* _elmW;
};


#endif // __TY_SOURCELINEIC_WIDGET__
