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
 * \file TYVegetationWidget.h
 * \brief outil IHM pour une vegetation (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_VEGETATION_WIDGET__
#define __TY_VEGETATION_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYVegetation;
class QLineEdit;
class QGridLayout;
class QLabel;
class QPushButton;
class QGroupBox;
class TYElementWidget;

/**
 * \class TYVegetationWidget
 * \brief classe de l'objet IHM pour une vegetation
 */
class TYVegetationWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYVegetation)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYVegetationWidget(TYVegetation* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYVegetationWidget();


public slots:
    virtual void updateContent();
    virtual void apply();

    /**
     * Edite la widget du spectre d'attenuation.
     */
    void editSpectre();

    // Membres
protected:

    QGroupBox* _groupBox;
    QLabel* _labelHauteur;
    QLineEdit* _lineEditHauteur;
    QGroupBox* _groupBoxSpectreAtt;
    QLineEdit* _lineEditNomSpectreAtt;
    QPushButton* _pushButtonSpectreAtt;

    QGridLayout* _vegetationLayout;
    QGridLayout* _groupBoxLayout;
    QGridLayout* _groupBoxSpectreAttLayout;

    TYElementWidget* _elmW;
};


#endif // __TY_VEGETATION_WIDGET__
