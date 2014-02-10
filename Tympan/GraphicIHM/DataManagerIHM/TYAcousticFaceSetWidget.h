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
 * \file TYAcousticFaceSetGraphic.h
 * \brief outil IHM d'un ensemble de faces acoustiques (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_ACOUSTICFACESET_WIDGET__
#define __TY_ACOUSTICFACESET_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYAcousticFaceSet;
class QLineEdit;
class QGridLayout;
class QLabel;
class TYAcousticInterfaceWidget;
class TYFaceSetWidget;
class TYColorInterfaceWidget;
class TYElementWidget;
class QGroupBox;
class QPushButton;

/**
 * \class TYAcousticFaceSetWidget
 * \brief classe d'un objet IHM pour un ensemble de faces acoustiques
 *
 *
 */
class TYAcousticFaceSetWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYAcousticFaceSet)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYAcousticFaceSetWidget(TYAcousticFaceSet* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYAcousticFaceSetWidget();


public slots:
    virtual void updateContent();
    virtual void apply();
    virtual void reject();


    /**
     * Edite la widget du materiau.
     */
    void editMat();


    // Membres
protected:

    QGridLayout* _acousticVolumeLayout;
    QGridLayout* _groupBoxMatLayout;
    QGroupBox* _groupBoxMat;
    QLineEdit* _lineEditNomMat;
    QPushButton* _pushButtonMat;

    TYFaceSetWidget* _volumeW;
    TYColorInterfaceWidget* _colorW;
    TYElementWidget* _elmW;
};


#endif // __TY_ACOUSTICFACESET_WIDGET__
