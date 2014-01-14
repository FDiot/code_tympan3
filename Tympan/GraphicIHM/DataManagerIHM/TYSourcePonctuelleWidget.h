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
 * \file TYSourcePonctuelleWidget.h
 * \brief outil IHM pour une source ponctuelle  (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_SOURCEPONCTUELLE_WIDGET__
#define __TY_SOURCEPONCTUELLE_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYSourcePonctuelle;
class QLineEdit;
class QGridLayout;
class QLabel;
class QTreeWidget;
class QGroupBox;
class QPushButton;
class QTreeWidgetItem;
class QCheckBox;
class QComboBox;
class QButtonGroup;
class QRadioButton;
class TYSourceWidget;
class TYPointWidget;
class TYColorInterfaceWidget;



/**
 * \class TYSourcePonctuelleWidget
 * \brief classe de l'objet IHM pour une source ponctuelle
 */
class TYSourcePonctuelleWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYSourcePonctuelle)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYSourcePonctuelleWidget(TYSourcePonctuelle* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYSourcePonctuelleWidget();

    /**
     * Mise a jour du comboBox des forme d'objet
     */
    void updateComboObject();

signals:
    /**
     * Type de rayonnement modifie
     */
//    void typeRayntChanged(int typeRaynt);

public slots:
    virtual void updateContent();
    virtual void apply();

    /**
     * Edite la widget de la position.
     */
    void editPosition();

    /**
     * Mise a jour de la directivite.
     */
    void updateDir();

    /**
     * Changement du type de rayonnement de la source
     */
//    void changeTypeRaynt(int id);

    /**
     * Changement la forme de l'objet a afficher
     */
    void changeObject(int object);

    // Membres
protected:

    QGroupBox* _groupBoxType;
//    QLabel* _labelTypeRaynt;
    QGroupBox* _groupBoxPosition;

    QPushButton* _pushButtonPosition;
    QLineEdit* _lineEditNomPosition;
    QLineEdit* _pLineEditHauteur;

    /// Choix de la forme de l'objet
    QLabel* _labelSelectObject;
    QComboBox* _comboBoxSelectObject;

    /// Bouton radio pour choisir le type de rayonnement
    QRadioButton* _pRadioButtonSpheric;
    QRadioButton* _pRadioButtonHemi;
//    QRadioButton* _pRadioButtonForce;
    QButtonGroup* _buttonGroupTypeRaynt;

    QGridLayout* _sourcePonctuelleLayout;
    QGridLayout* _groupBoxTypeLayout;
    QGridLayout* _groupBoxPositionLayout;
    QGridLayout* _groupBoxDirLayout;

    TYSourceWidget* _elmW;
    TYPointWidget* _pPointW;
    TYColorInterfaceWidget* _colorW;
};


#endif // __TY_SOURCEPONCTUELLE_WIDGET__
