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
 * \file TYBoucheSurfaceWidget.h
 * \brief outil IHM pour une bouche de ventilation (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_BOUCHE_SURFACE_WIDGET__
#define __TY_BOUCHE_SURFACE_WIDGET__


#include "TYWidget.h"
#include <qlayout.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYBoucheSurface;
class QLineEdit;
class QGridLayout;
class QGroupBox;
class QLabel;
class TYAcousticRectangleWidget;


/**
 * \class TYBoucheSurfaceWidget
 * \brief classe de l'objet IHM pour une bouche de ventilation
 */
class TYBoucheSurfaceWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYBoucheSurface)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYBoucheSurfaceWidget(TYBoucheSurface* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYBoucheSurfaceWidget();

    /**
     * Get du sous-element AcousticRectangleWidget.
     */
    TYAcousticRectangleWidget* getSurfaceWidget() { return _elmW; }

    /**
     * Permet de modifier les parametres du layout.
     *
     * @param margin La valeur de la marge.
     * @param spacing La valeur de l'espacement.
     */
    /*
        void setLayoutSpacing(int margin, int spacing) {
            _pBoucheSurfaceLayout->setMargin(margin);
            _pBoucheSurfaceLayout->setSpacing(spacing);
        }
    */

public slots:
    virtual void updateContent();
    virtual void apply();

    /**
     * Edite la widget de la source bafflee.
     */
    //  void editSource();


    // Membres
protected:
    QGridLayout* _pBoucheSurfaceLayout;
    //  QLineEdit * _pLineEditSrc;

    QGroupBox* _groupBoxDimensions;
    QGridLayout* _groupBoxDimensionsLayout;
    QLabel*     _dimensionsXLabel;
    QLineEdit*  _dimensionsXLineEdit;
    QLabel*     _dimensionsYLabel;
    QLineEdit*  _dimensionsYLineEdit;


    TYAcousticRectangleWidget* _elmW;
};


#endif // __TY_BOUCHE_SURFACE_WIDGET__
