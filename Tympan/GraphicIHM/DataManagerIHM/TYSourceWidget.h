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
 * \file TYSourceWidget.h
 * \brief outil IHM pour une source (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_SOURCE_WIDGET__
#define __TY_SOURCE_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYSource;
class QLineEdit;
class QGridLayout;
class QLabel;
class QGroupBox;
class TYElementWidget;
//class TYColorInterfaceWidget;
class QPushButton;
/**
 * \class TYSourceWidget
 * \brief classe de l'objet IHM pour une source
 */
class TYSourceWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYSource)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYSourceWidget(TYSource* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYSourceWidget();


public slots:
    virtual void updateContent();
    virtual void apply();

    /**
     * Visualise le spectre de l'etat courant de l'element
     */
    void showSpectre();


    // Membres
protected:

    QGroupBox* _groupBox;

    QGridLayout* _sourceLayout;
    QGridLayout* _groupBoxLayout;

    // Affichage du spectre
    QGroupBox* _groupBoxSpectre;
    QGridLayout* _groupBoxSpectreLayout;
    QPushButton* _pushButtonShowSpectre;

    TYElementWidget* _elmW;
    //  TYColorInterfaceWidget * _colorW;
};


#endif // __TY_SOURCE_WIDGET__
