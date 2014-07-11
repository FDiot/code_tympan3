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
 * \file TYPointControlWidget.h
 * \brief outil IHM pour un point de controle (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 *
 */

#ifndef __TY_POINT_CONTROL_WIDGET__
#define __TY_POINT_CONTROL_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QLabel>

class TYPointControl;
class QLineEdit;
class TYPointCalculWidget;
class QLabel;
class QComboBox;
class QButtonGroup;
class QRadioButton;
class TYColorInterfaceWidget;


/**
 * \class TYPointControlWidget
 * \brief class de l'objet IHM pour un point de controle
 */
class TYPointControlWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYPointControl)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYPointControlWidget(TYPointControl* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYPointControlWidget();

    /**
     * Mise a jour du comboBox des forme d'objet
     */
    void updateComboObject();

public slots:
    virtual void updateContent();
    virtual void apply();
    virtual void reject();

    /**
     * Changement la forme de l'objet a afficher
     */
    void changeObject(int object);

    /**
     * Affichage des coordonnees selon le mode choisi (TYMPAN ou SIG)
     */
    void updateCoord(int selected);

    // Membres
protected:
    TYPointCalculWidget* _ptCalculW;
    QLineEdit* _lineEditHauteur;

    /// Choix de la forme de l'objet
    QLabel* _labelSelectObject;
    QComboBox* _comboBoxSelectObject;

    /// Choix du systeme de coordonnee
    QButtonGroup* _buttonGroupSysCoord;
    QLabel* _labelRepere;
    QLineEdit* _lineEditRepere;

    /// Copie du point de controle
    TYPointControl* _pPointBack;

    /// Interface pour la gestion de la couleur
    TYColorInterfaceWidget* _colorW;


};


#endif // __TY_POINT_CONTROL_WIDGET__
