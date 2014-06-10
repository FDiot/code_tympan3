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
 * \file TYPointCalculWidget.h
 * \brief outil IHM pour un point de calcul (fichier header)
 * \author Projet_Tympan
 *
 * TYPointCalculWidget.cpp
 *
 *
 *
 *
 */

#ifndef __TY_POINT_CALCUL_WIDGET__
#define __TY_POINT_CALCUL_WIDGET__


#include "TYWidget.h"
#include <qlayout.h>
//Added by qt3to4:
#include <QGridLayout>

class QLineEdit;
class TYPointWidget;
class QButtonGroup;
class QRadioButton;
class QGroupBox;
class QComboBox;
class QLabel;

class TYPointCalcul;
class TYCalcul;
class TYElementWidget;

/**
 * \class TYPointCalculWidget
 * \brief classe de l'objet IHM pour un point de calcul
 */
class TYPointCalculWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYPointCalcul)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYPointCalculWidget(TYPointCalcul* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYPointCalculWidget();

    /**
     * Permet de modifier les parametres du layout.
     *
     * @param margin La valeur de la marge.
     * @param spacing La valeur de l'espacement.
     */
    void setLayoutSpacing(int margin, int spacing)
    {
        _pPointCalculLayout->setMargin(margin);
        _pPointCalculLayout->setSpacing(spacing);
    }

private:
    /**
     * Affiche la liste des calculs existant
     */
    void updateComboCalcul();

    /**
     * Retourne le spectre (calcul) en cours de selection
     */
    TYCalcul* getSelectedCalcul();

public slots:
    virtual void updateContent();
    virtual void apply();

    /**
     * Change le calcul et le spectre concerne
     */
    void changeCalcul(int calcul);

    /**
     * Edite la widget du spectre.
     */
    void editSpectre();

    /**
     * Affiche la dialog ResultatTree.
     */
    //  void showResultatTreeDialog();


    // Membres
protected:

    QLineEdit* _lineEditNomSpectre;
    QButtonGroup* _buttonGroupState;
    QGroupBox* _groupBoxState;
    QRadioButton* _pRadioButtonInactif;
    QRadioButton* _pRadioButtonActif;

    /// Choix du calcul associe au spectre
    QLabel* _labelSpectreCalcul;
    QComboBox* _comboBoxSpectreCalul;

    TYElementWidget* _elmW;

    TYPointWidget* _ptW;
    QGridLayout* _pPointCalculLayout;
};


#endif // __TY_POINT_CALCUL_WIDGET__
