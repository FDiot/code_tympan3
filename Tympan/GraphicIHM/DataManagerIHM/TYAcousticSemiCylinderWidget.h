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
 * \file TYAcousticSemiCylinderWidget.h
 * \brief outil IHM pour un demi cylindre acoustique (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 */

#ifndef __TY_ACOUSTICSEMICYLINDER_WIDGET__
#define __TY_ACOUSTICSEMICYLINDER_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYAcousticSemiCylinder;
class QLineEdit;
class QGridLayout;
class QLabel;
class QGroupBox;
class QPushButton;
class TYAcousticVolumeWidget;
class QCheckBox;

/**
 * \class TYAcousticSemiCylinderWidget
 * \brief objet IHM pour un 1/2 cylindre acoustique
 */
class TYAcousticSemiCylinderWidget : public TYWidget
{
    Q_OBJECT


    TY_DECL_METIER_WIDGET(TYAcousticSemiCylinder)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYAcousticSemiCylinderWidget(TYAcousticSemiCylinder* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYAcousticSemiCylinderWidget();


public slots:
    virtual void updateContent();
    virtual void apply();

    /**
     * Edite la widget de la source surfacique.
     */
    void editEnveloppe();
    /**
     * Edite la widget du demi-disque du dessus.
     */
    void editSemiCircTop();
    /**
     * Edite la widget du demi-disque du dessous.
     */
    void editSemiCircBottom();
    /**
     * Edite la widget du rectangle.
     */
    void editRect();


    // Membres
protected:

    QGroupBox* _groupBoxEnveloppe;
    QPushButton* _pushButtonEnveloppe;
    QLineEdit* _lineEditNomEnveloppe;
    QCheckBox* _checkBoxEnveloppe;
    QGroupBox* _groupBoxCircTop;
    QLineEdit* _lineEditNomCircTop;
    QPushButton* _pushButtonSemicircTop;
    QCheckBox* _checkBoxSemiCircTop;
    QGroupBox* _groupBoxCircBottom;
    QLineEdit* _lineEditNomCircBottom;
    QPushButton* _pushButtonSemiCircBottom;
    QCheckBox* _checkBoxSemiCircBottom;
    QGroupBox* _groupBoxRect;
    QLineEdit* _lineEditNomRect;
    QPushButton* _pushButtonRect;
    QCheckBox* _checkBoxRect;

    QGridLayout* _acousticSemiCylinderLayout;
    QGridLayout* _groupBoxEnveloppeLayout;
    QGridLayout* _groupBoxCircTopLayout;
    QGridLayout* _groupBoxCircBottomLayout;
    QGridLayout* _groupBoxRectLayout;

    QGroupBox* _groupBoxDimensions;
    QGridLayout* _groupBoxDimensionsLayout;
    QLabel*     _dimensionsDiamLabel;
    QLineEdit*  _dimensionsDiamLineEdit;
    QLabel*     _dimensionsHauteurLabel;
    QLineEdit*  _dimensionsHauteurLineEdit;

    TYAcousticVolumeWidget* _elmW;
};


#endif // __TY_ACOUSTICSEMICYLINDER_WIDGET__
