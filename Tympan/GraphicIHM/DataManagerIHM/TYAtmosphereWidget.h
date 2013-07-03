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
 * \file TYAtmosphereWidget.h
 * \brief outil IHM pour l'objet atmosphere (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_ATMOSPHERE_WIDGET__
#define __TY_ATMOSPHERE_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYAtmosphere;
class QLineEdit;
class QGridLayout;
class QLabel;
class QPushButton;
class QGroupBox;
class TYElementWidget;
class QComboBox;

/**
 * \class TYAtmosphereWidget
 * \brief classe de l'objet IHM pour l'objet atmosphere
 */
class TYAtmosphereWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYAtmosphere)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYAtmosphereWidget(TYAtmosphere* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYAtmosphereWidget();


public slots:
    virtual void updateContent();
    virtual void apply();
    virtual void changeCelerite();

    /**
     * Edite la widget du spectre d'attenuation.
     */
    void editSpectre();

protected :
    void updateBoxClasseMeteo();

    // Membres
protected:
    QGroupBox* _groupBox;
    QLineEdit* _lineEditPression;
    QLabel* _labelPression;
    QLabel* _labelTemperature;
    QLineEdit* _lineEditTemperature;
    QLabel* _labelHygrometrie;
    QLineEdit* _lineEditHygrometrie;
    QLabel* _labelVitSon;
    QLineEdit* _lineEditVitSon;
    QLineEdit* _lineEditVitVent;
    QLabel* _labelVitVent;
    QLineEdit* _lineEditDirVent;
    QLabel* _labelDirVent;
    QGroupBox* _groupBoxSpectreAtt;
    QLineEdit* _lineEditNomSpectre;
    QPushButton* _pushButtonSpectre;

    QGridLayout* _atmosphereLayout;
    QGridLayout* _groupBoxLayout;
    QGridLayout* _groupBoxSpectreAttLayout;

    TYElementWidget* _elmW;

    /// Specifique harmonoise
    QLabel* _labelA;
    QLabel* _labelB;
    QLabel* _labelC;
    QLabel* _labelD;
    QLineEdit* _lineEditA;
    QLineEdit* _lineEditB;
    QLineEdit* _lineEditC;
    QLineEdit* _lineEditD;

    QLabel* _labelClasseMeteo;
    QComboBox* _comboBoxClasseMeteo;

    QGroupBox* _groupBoxMeteo1;
    QGroupBox* _groupBoxMeteo2;
};


#endif // __TY_ATMOSPHERE_WIDGET__
