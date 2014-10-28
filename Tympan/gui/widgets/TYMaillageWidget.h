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
 * \file TYMaillageWidget.h
 * \brief Outil IHM pour un maillage (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_MAILLAGE_WIDGET__
#define __TY_MAILLAGE_WIDGET__


#include "TYWidget.h"

#include <vector>
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYMaillage;
class QLineEdit;
class QGridLayout;
class QLabel;
class QGroupBox;
class QPushButton;
class QButtonGroup;
class QComboBox;
class QRadioButton;
class TYElementWidget;
class TYLabeledLookupTableWidget;


/**
 * \class TYMaillageWidget
 * \brief classe de l'objet IHM pour un maillage
 */
class TYMaillageWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYMaillage)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYMaillageWidget(TYMaillage* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYMaillageWidget();

public slots:
    virtual void updateContent();
    virtual void apply();

    /**
     * Mets a jour la palette de couleur.
     */
    void updatePalette();

    /**
     * Mets a jour le nombre de points que contiendra le maillage
     * en fonction des parametres entres.
     */
    void updateNbPoints(unsigned int nbPts);

    /**
     * Mets a jour les valeurs min et max selon le type de
     * representation choisi.
     */
    void updateValMinMax();

    /**
     * Gere les changements de type de donnees.
     */
    void dataTypeChanged(int selected);

    /**
     * Choix de la frequence pour visualiser la carto
     */
    void setFrequency(int freq);

    /**
     * Visualise l'editeur de palette
     */
    void editPalette();

protected :
    void updateFreqList();

    // Membres
protected:
    QLineEdit* _lineEditHauteur;
    QGroupBox* _groupBox;
    QLineEdit* _lineEditNbPoints;
    QLineEdit* _lineEditDataValMin;
    QLineEdit* _lineEditDataValMax;
    QGridLayout* _maillageLayout;
    QGridLayout* _groupBoxLayout;
    QButtonGroup* _buttonGroupDataType;
    QRadioButton* _pRadioButtondBA;
    QRadioButton* _pRadioButtondBLin;
    QRadioButton* _pRadioButtonDataFreq;

    QComboBox* _comboBoxFreq;

    // Palette
    QLineEdit* _lineEditPaletteName;
    QPushButton* _pushButtonEditPalette;
    TYLabeledLookupTableWidget* _lookupTableWidget;

    TYElementWidget* _elmW;

    float _dataFreq;
};


#endif // __TY_MAILLAGE_WIDGET__
