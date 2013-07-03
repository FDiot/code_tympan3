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
 * \file TYEcranWidget.h
 * \brief Outil IHM pour un ecran (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_ECRAN_WIDGET__
#define __TY_ECRAN_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYEcran;
class QLineEdit;
class QGridLayout;
class QLabel;
class QGroupBox;
class QPushButton;
class QButtonGroup;
class QTabWidget;
class TYAcousticFaceSetWidget;

/**
 * \class TYEcranWidget
 * \brief classe de l'objet IHM pour un ecran
 */
class TYEcranWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYEcran)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYEcranWidget(TYEcran* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYEcranWidget();


public slots:
    virtual void updateContent();
    virtual void apply();


    // Membres
protected:

    QGridLayout* _ecranLayout;

    TYAcousticFaceSetWidget* _elmW;

    QWidget* _widgetProp;
    QGroupBox* _groupBoxHauteur;
    QGroupBox* _groupBoxEpaisseur;
    QGridLayout* _groupBoxPropLayout;
    QGridLayout* _groupBoxHauteurLayout;
    QGridLayout* _groupBoxEpaisseurLayout;

    QLineEdit* _lineEditHauteur;
    QLineEdit* _lineEditEpaisseur;
};


#endif // __TY_ETAGE_WIDGET__
