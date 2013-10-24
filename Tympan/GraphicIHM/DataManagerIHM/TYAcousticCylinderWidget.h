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
 * \file TYAcousticCylinderWidget.h
 * \brief outil IHM pour un cylindre acoustique (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_ACOUSTICCYLINDER_WIDGET__
#define __TY_ACOUSTICCYLINDER_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYAcousticCylinder;
class TYAcousticVolumeWidget;
class QLineEdit;
class QGridLayout;
class QLabel;
class QGroupBox;
class QPushButton;
class QCheckBox;


/**
 * \class TYAcousticCylinderWidget
 * \brief classe de l'objet IHM pour un cylindre acoustique
 */
class TYAcousticCylinderWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYAcousticCylinder)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYAcousticCylinderWidget(TYAcousticCylinder* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYAcousticCylinderWidget();


public slots:
    virtual void updateContent();
    virtual void apply();

    /**
     * Edite la widget de la source surfacique.
     */
    void editEnveloppe();
    /**
     * Edite la widget du disque du dessus.
     */
    void editCircTop();
    /**
     * Edite la widget du disque du dessous.
     */
    void editCircBottom();

    // Membres
protected:

    QGroupBox* _groupBoxEnveloppe;
    QLineEdit* _lineEditNomEnveloppe;
    QPushButton* _pushButtonEnveloppe;
    QCheckBox* _checkBoxEnveloppe;
    QGroupBox* _groupBoxCircTop;
    QLineEdit* _lineEditNomCircTop;
    QPushButton* _pushButtonCircTop;
    QCheckBox* _checkBoxCircTop;
    QGroupBox* _groupBoxCircBottom;
    QPushButton* _pushButtonCircBottom;
    QCheckBox* _checkBoxCircBottom;
    QLineEdit* _lineEditNomCircBottom;
    TYAcousticVolumeWidget* _elmW;

    QGridLayout* _acousticCylinderLayout;
    QGridLayout* _groupBoxEnveloppeLayout;
    QGridLayout* _groupBoxCircTopLayout;
    QGridLayout* _groupBoxCircBottomLayout;

    QGroupBox* _groupBoxDimensions;
    QGridLayout* _groupBoxDimensionsLayout;
    QLabel*     _dimensionsDiamLabel;
    QLineEdit*  _dimensionsDiamLineEdit;
    QLabel*     _dimensionsHauteurLabel;
    QLineEdit*  _dimensionsHauteurLineEdit;
};


#endif // __TY_ACOUSTICCYLINDER_WIDGET__
