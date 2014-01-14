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
 * \file TYFaceSetWidget.h
 * \brief Outil IHM pour un ensemble de faces (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_FACESET_WIDGET__
#define __TY_FACESET_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYFaceSet;
class QLineEdit;
class QGridLayout;
class QLabel;
class QGroupBox;
class TYAcousticVolumeWidget;

/**
 * \class TYFaceSetWidget
 * \brief classe de l'objet IHM pour un ensemble de faces
 */
class TYFaceSetWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYFaceSet)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYFaceSetWidget(TYFaceSet* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYFaceSetWidget();


public slots:
    virtual void updateContent();
    virtual void apply();


    // Membres
protected:
    QGroupBox* _groupBoxVolume;
    QGroupBox* _groupBoxSurface;
    QGridLayout* _groupBoxVolumeLayout;
    QGridLayout* _groupBoxSurfaceLayout;

    QLineEdit* _lineEditVolume;
    QLabel* _labelVolume;
    QLineEdit* _lineEditSurface;
    QLabel* _labelSurface;

    QGridLayout* _faceSetLayout;
};


#endif // __TY_FACESET_WIDGET__
