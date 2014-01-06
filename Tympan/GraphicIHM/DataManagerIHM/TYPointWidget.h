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
 * \file TYPointWidget.h
 * \brief outil IHM pour un point (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_POINT_WIDGET__
#define __TY_POINT_WIDGET__


#include "TYWidget.h"
#include <qlayout.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYPoint;
class QLineEdit;
class QGridLayout;
class QLabel;
class QGroupBox;
class TYElementWidget;
class TYColorInterfaceWidget;

/**
 * \class TYPointWidget
 * \brief classe de l'objet IHM pour un point
 */
class TYPointWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYPoint)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYPointWidget(TYPoint* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYPointWidget();

    /**
     * Permet de modifier les parametres du layout.
     *
     * @param margin La valeur de la marge.
     * @param spacing La valeur de l'espacement.
     */
    void setLayoutSpacing(int margin, int spacing)
    {
        _pointLayout->setMargin(margin);
        _pointLayout->setSpacing(spacing);
    }

    void disableZ();
    void enableZ();


public slots:
    virtual void updateContent();
    virtual void apply();


    // Membres
protected:

    QGroupBox* _groupBox;
    QLabel* _labelX;
    QLabel* _labelZ;
    QLabel* _labelY;
    QLineEdit* _lineEditX;
    QLineEdit* _lineEditY;
    QLineEdit* _lineEditZ;

    QGridLayout* _pointLayout;
    QGridLayout* _groupBoxLayout;

    //  TYElementWidget * _elmW;
    TYColorInterfaceWidget* _colorW;
};


#endif // __TY_POINT_WIDGET__
