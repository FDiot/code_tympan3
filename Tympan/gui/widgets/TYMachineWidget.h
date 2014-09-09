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
 * \file TYMachineWidget.h
 * \brief outil IHM pour une machine (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_MACHINE_WIDGET__
#define __TY_MACHINE_WIDGET__


#include "TYWidget.h"
#include <QGridLayout>
#include <QLabel>

class TYMachine;
class QLineEdit;
class QGridLayout;
class QLabel;
class QGroupBox;
class QTextEdit;
class TYAcousticVolumeNodeWidget;

/**
 * \class TYMachineWidget
 * \brief classe de l'objet IHM pour une machine
 */
class TYMachineWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYMachine)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYMachineWidget(TYMachine* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYMachineWidget();


public slots:
    /**
     * Remplis les champs avec les valeurs de l'element associe.
     */
    virtual void updateContent();

    /**
     * Effectue la tâche inverse de updateContent().
     * Mets a jour les parametres de l'element associe avec les
     * valeurs des champs.
     */
    virtual void apply();


    // Membres
protected:
    QGroupBox* _groupBox;
    QLineEdit* _lineEditCategorie;
    QLabel* _labelCategorie;
    QLabel* _labelConstructeur;
    QLineEdit* _lineEditConstructeur;
    QLabel* _labelModele;
    QLineEdit* _lineEditModele;
    QTextEdit* _lineEditComment;
    QLabel* _labelComment;


protected:
    QGridLayout* _machineLayout;
    QGridLayout* _groupBoxLayout;

    TYAcousticVolumeNodeWidget* _elmW;
};


#endif // __TY_MACHINE_WIDGET__
