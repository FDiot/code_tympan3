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
 * \file TYRepereWidget.h
 * \brief Outil IHM pour un repere (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_REPERE_WIDGET__
#define __TY_REPERE_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYRepere;
class QLineEdit;
class QGridLayout;
class QLabel;
class QGroupBox;
class QPushButton;
class TYElementWidget;

/**
 * \class TYRepereWidget
 * \brief classe de l'objet IHM pour un repere
 */
class TYRepereWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYRepere)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYRepereWidget(TYRepere* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYRepereWidget();


public slots:
    virtual void updateContent();
    virtual void apply();


    // Membres
protected:

    QGroupBox* GroupBoxOrigine;
    QLabel* labelX;
    QLabel* labelZ;
    QLabel* labelY;
    QLineEdit* lineEditX;
    QLineEdit* lineEditY;
    QLineEdit* lineEditZ;
    QGroupBox* GroupBoxVecI;
    QLabel* labelX_2;
    QLabel* labelZ_2;
    QLabel* labelY_2;
    QLineEdit* lineEditX_2;
    QLineEdit* lineEditY_2;
    QLineEdit* lineEditZ_2;
    QGroupBox* GroupBoxVecK;
    QLabel* labelX_4;
    QLabel* labelZ_4;
    QLabel* labelY_4;
    QLineEdit* lineEditX_4;
    QLineEdit* lineEditY_4;
    QLineEdit* lineEditZ_4;
    QGroupBox* GroupBoxVecJ;
    QLabel* labelX_3;
    QLabel* labelZ_3;
    QLabel* labelY_3;
    QLineEdit* lineEditX_3;
    QLineEdit* lineEditY_3;
    QLineEdit* lineEditZ_3;

    QGridLayout* RepereLayout;
    QGridLayout* GroupBoxOrigineLayout;
    QGridLayout* GroupBoxVecILayout;
    QGridLayout* GroupBoxVecKLayout;
    QGridLayout* GroupBoxVecJLayout;

    TYElementWidget* elmW;

};


#endif // __TY_REPERE_WIDGET__
