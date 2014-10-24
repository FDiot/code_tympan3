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
 * \file TYMateriauConstructionWidget.h
 * \brief Outil IHM pour des materiaux de construction (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_MATERIAUCONSTRUCTION_WIDGET__
#define __TY_MATERIAUCONSTRUCTION_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYMateriauConstruction;
class QLineEdit;
class QGridLayout;
class QLabel;
class QGroupBox;
class QPushButton;
class TYElementWidget;

/**
 * \class TYMateriauConstructionWidget
 * \brief classe de l'objet IHM pour des materiaux de construction
 */
class TYMateriauConstructionWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYMateriauConstruction)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYMateriauConstructionWidget(TYMateriauConstruction* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYMateriauConstructionWidget();

    /**
     * Retourne l'element associe a ce Widget.
     */
    TYMateriauConstruction* getMateriauConstruction();


public slots:
    virtual void updateContent();
    virtual void apply();

    /**
     * Edite la widget du spectre de transmission.
     */
    void editSpectreT();
    /**
     * Edite la widget du spectre d'absorption.
     */
    void editSpectreA();

    /**
     * Mise a jour par selection dans la XML.
     */
    void getFromXML();


    // Membres
protected:

    QGroupBox* _groupBox;
    QLineEdit* _lineEditMasseVol;
    QLabel* _labelMasseVol;
    QGroupBox* _groupBoxSpectreAbs;
    QPushButton* _pushButtonSpectreAbs;
    QLineEdit* _lineEditNomSpectreAbs;
    QGroupBox* _groupBoxSpectreTransm;
    QPushButton* _pushButtonSpectreTransm;
    QLineEdit* _lineEditNomSpectreTransm;
    QPushButton* _pushButtonGetFromXML;

    QGridLayout* _matConstrLayout;
    QGridLayout* _groupBoxLayout;
    QGridLayout* _groupBoxSpectreAbsLayout;
    QGridLayout* _groupBoxSpectreTransmLayout;

    TYElementWidget* _elmW;
};


#endif // __TY_MATERIAUCONSTRUCTION_WIDGET__
