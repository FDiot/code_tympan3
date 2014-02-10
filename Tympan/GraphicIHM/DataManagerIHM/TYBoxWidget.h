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
 * \file TYBoxWidget.h
 * \brief outil IHM pour une boite (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_BOX_WIDGET__
#define __TY_BOX_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYBox;
class QLineEdit;
class QGridLayout;
class QLabel;
class QGroupBox;
class QPushButton;
class TYElementWidget;
class TYColorInterfaceWidget;


/**
 * \class TYBoxWidget
 * \brief classe d'un objet IHM pour une boite
 */
class TYBoxWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYBox)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYBoxWidget(TYBox* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYBoxWidget();


public slots:
    virtual void updateContent();
    virtual void apply();

    /**
     * Edite la widget de la position.
     */
    void editPosition();


    // Membres
protected:

    QGroupBox* _groupBox;
    QLabel* _labelSizeX;
    QLabel* _labelSizeY;
    QLabel* _labelSizeZ;
    QLineEdit* _lineEditSizeX;
    QLineEdit* _lineEditSizeY;
    QLineEdit* _lineEditSizeZ;
    QGroupBox* _groupBoxPosition;
    QLineEdit* _lineEditNomPosition;
    QPushButton* _pushButtonPosition;


protected:
    QGridLayout* _boxLayout;
    QGridLayout* _groupBoxLayout;
    QGridLayout* _groupBoxPositionLayout;

    TYElementWidget* _elmW;
    TYColorInterfaceWidget* _colorW;

};


#endif // __TY_BOX_WIDGET__
