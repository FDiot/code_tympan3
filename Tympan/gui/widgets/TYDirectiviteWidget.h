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
 * \file TYDirectiviteWidget.h
 * \brief outil IHM pourla directivite (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_DIRECTIVITY_WIDGET__
#define __TY_DIRECTIVITY_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYDirectivity;
class QLineEdit;
class QGridLayout;
class QLabel;
class TYElementWidget;
class QTableWidget;


/**
 * \class TYDirectiviteWidget
 * \brief classe de l'objet IHM pour la directivite
 */
class TYDirectiviteWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYDirectivity)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYDirectiviteWidget(TYDirectivity* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYDirectiviteWidget();


public slots:
    virtual void updateContent();
    virtual void apply();

    /**
     * Affiche un menu contextuel.
     */
    void showContextMenu(const QPoint& pos);


    // Membres
protected:

    QTableWidget* _table;

    QGridLayout* _directiviteLayout;

    TYElementWidget* _elmW;
};


#endif // __TY_DIRECTIVITY_WIDGET__
