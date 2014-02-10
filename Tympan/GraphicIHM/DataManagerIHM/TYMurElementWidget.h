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
 * \file TYMurElementWidget.h
 * \brief outil IHM pour un element de mur (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_MURELEMENT_WIDGET__
#define __TY_MURELEMENT_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYMurElement;
class QLineEdit;
class QGridLayout;
class QLabel;
class QGroupBox;
class QPushButton;
class TYAcousticRectangleWidget;

/**
 * \class TYMurElementWidget
 * \brief classe de l'outil IHM pour un element de mur
 */
class TYMurElementWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYMurElement)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYMurElementWidget(TYMurElement* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYMurElementWidget();


public slots:
    virtual void updateContent();
    virtual void apply();

    /**
     * Edite la widget de la paroi.
     */
    void editParoi();


    // Membres
protected:

    QGroupBox* _groupBoxParoi;
    QPushButton* _pushButtonParoi;
    QLineEdit* _lineEditNomParoi;

    QGridLayout* _murElemntLayout;
    QGridLayout* _groupBoxParoiLayout;

    QGroupBox* _groupBoxDimensions;
    QGridLayout* _groupBoxDimensionsLayout;
    QLabel*     _dimensionsXLabel;
    QLineEdit*  _dimensionsXLineEdit;
    QLabel*     _dimensionsYLabel;
    QLineEdit*  _dimensionsYLineEdit;

    TYAcousticRectangleWidget* _elmW;
};


#endif // __TY_MURELEMENT_WIDGET__
