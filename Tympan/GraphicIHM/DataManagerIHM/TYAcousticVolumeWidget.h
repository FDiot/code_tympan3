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
 * \file TYAcousticVolumeWidget.h
 * \brief outil IHM pour un volume acoustique (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_ACOUSTICVOLUME_WIDGET__
#define __TY_ACOUSTICVOLUME_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYAcousticVolume;
class QLineEdit;
class QGridLayout;
class QLabel;
class TYColorInterfaceWidget;
class TYAcousticInterfaceWidget;
class TYVolumeInterfaceWidget;
class TYElementWidget;
class QGroupBox;
class QPushButton;

/**
 * \class TYAcousticVolumeWidget
 * \brief classe de l'objet IHM pour un volume acoustique
 */
class TYAcousticVolumeWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYAcousticVolume)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYAcousticVolumeWidget(TYAcousticVolume* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYAcousticVolumeWidget();


public slots:
    virtual void updateContent();
    virtual void apply();
    virtual void reject();

    /**
     * Edite la widget du materiau.
     */
    void editMat();

    /**
     * Indique que la couleur a ete modifiee.
     */
    void modifyColor();

signals:
    void colorModified();

    // Membres
protected:

    bool _isColorModified;

    QGridLayout* _acousticVolumeLayout;
    QGridLayout* _groupBoxMatLayout;
    QGroupBox* _groupBoxMat;
    QLineEdit* _lineEditNomMat;
    QPushButton* _pushButtonMat;

    TYAcousticInterfaceWidget* _interW;
    TYVolumeInterfaceWidget* _volumeW;
    TYColorInterfaceWidget* _colorW;
    TYElementWidget* _elmW;
};


#endif // __TY_ACOUSTICVOLUME_WIDGET__
