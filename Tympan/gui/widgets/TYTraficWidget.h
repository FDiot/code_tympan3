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
 * \file TYTraficWidget.h
 * \brief outil IHM pour un traffic (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_TRAFIC_WIDGET__
#define __TY_TRAFIC_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYTrafic;
class QLineEdit;
class QGridLayout;
class QLabel;
class QGroupBox;
class TYElementWidget;


/**
 * \class TYTraficWidget
 * \brief classe de l'objet IHM pour un traffic
 */
class TYTraficWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYTrafic)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYTraficWidget(TYTrafic* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYTraficWidget();


public slots:
    virtual void updateContent();
    virtual void apply();


    // Membres
protected:

    QGroupBox* _groupBox;
    QLineEdit* _lineEditDebitPL;
    QLineEdit* _lineEditDebitVL;
    QLabel* _labelDebitPL;
    QLabel* _labelDebitVL;


protected:
    QGridLayout* _traficLayout;
    QGridLayout* _groupBoxLayout;

    TYElementWidget* _elmW;
};


#endif // __TY_TRAFIC_WIDGET__
