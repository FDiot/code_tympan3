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
 * \file TYDalleWidget.h
 * \brief Outil IHM pour une dalle (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_DALLE_WIDGET__
#define __TY_DALLE_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYDalle;
class QLineEdit;
class QGridLayout;
class QLabel;
class QGroupBox;
class QPushButton;
class QCheckBox;
class TYAcousticPolygonWidget;


/**
 * \class TYDalleWidget
 * \brief Classe de l'objet IHM pour une dalle
 */
class TYDalleWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYDalle)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYDalleWidget(TYDalle* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYDalleWidget();


public slots:
    virtual void updateContent();
    virtual void apply();

    /**
     * Edite la widget de la paroi.
     */
    void editParoi();

    // Blocage de la possibilite de changer de materiau
    void updateUseParoi();


    // Membres
protected:
    QGroupBox* _groupBoxParoi;
    QLineEdit* _lineEditNomParoi;
    QPushButton* _pushButtonParoi;

    QGridLayout* _dalleLayout;
    QGridLayout* _groupBoxParoiLayout;

    QLabel* _labeUseEtage;
    QCheckBox* _checkBoxUseEtage;

    TYAcousticPolygonWidget* _accPolygonW;
};


#endif // __TY_DALLE_WIDGET__
