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
 * \file TYParoiWidget.h
 * \brief outil IHM pour une paroi (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_PAROI_WIDGET__
#define __TY_PAROI_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYParoi;
class QLineEdit;
class QGridLayout;
class QLabel;
class QGroupBox;
class QPushButton;
class TYElementWidget;

/**
 * \class TYParoiWidget
 * \brief classe de l'objet IHM pour une paroi
 */
class TYParoiWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYParoi)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYParoiWidget(TYParoi* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYParoiWidget();


public slots:
    virtual void updateContent();
    virtual void apply();

    /**
     * Edite la widget du materiau.
     */
    void editMatStruct();

    /**
     * Edite la widget du materiau de la face 1.
     */
    void editMatFace1();

    /**
     * Edite la widget du materiau de la face 2.
     */
    void editMatFace2();

    /**
     * Mise a jour par selection dans la XML.
     */
    void getFromXML();



    // Membres
protected:

    QGroupBox* _groupBox4;
    QLabel* _labelEpaisseur;
    QLineEdit* _lineEditEpaisseur;
    QGroupBox* _groupBoxMatStruct;
    QPushButton* _pushButtonMatStruct;
    QLineEdit* _lineEditNomMatstruct;
    QGroupBox* _groupBoxMatFace1;
    QPushButton* _pushButtonMatFace1;
    QLineEdit* _lineEditNomMatFace1;
    QGroupBox* _groupBoxMatFace2;
    QLineEdit* _lineEditNomMatFace2;
    QPushButton* _pushButtonMatFace2;
    QPushButton* _pushButtonGetFromXML;

    QGridLayout* _paroiLayout;
    QGridLayout* _groupBox4Layout;
    QGridLayout* _groupBoxMatStructLayout;
    QGridLayout* _groupBoxMatFace1Layout;
    QGridLayout* _groupBoxMatFace2Layout;

    TYElementWidget* _elmW;
};


#endif // __TY_PAROI_WIDGET__
