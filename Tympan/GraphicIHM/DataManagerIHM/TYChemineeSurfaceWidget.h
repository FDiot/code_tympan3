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
 *
 * \file TYChemineeSurfaceWidget.h
 * \brief outil IHM pour une surface de cheminee (fichier header)
 * \author Projet_Tympan
 *
 * All Rights Reserved. No part of this software and its documentation may
 * be used, copied, modified, distributed and transmitted, in any form or by
 * any means, without the prior written permission of EDF.
 *
 * EDF-R&D Departement AMA - 1, avenue du general de Gaulle, 92141 CLAMART
 *
 *
 *
 *
 */

#ifndef __TY_CHEMINEE_SURFACE_WIDGET__
#define __TY_CHEMINEE_SURFACE_WIDGET__


#include "TYWidget.h"
#include <qlayout.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYChemineeSurface;
class QLineEdit;
class QGridLayout;
class QLabel;
class QGroupBox;
class TYAcousticRectangleWidget;


/**
 * \class TYChemineeSurfaceWidget
 * \brief classe de l'objet IHM pour une surface de cheminee
 */
class TYChemineeSurfaceWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYChemineeSurface)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYChemineeSurfaceWidget(TYChemineeSurface* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYChemineeSurfaceWidget();

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
            _pChemineeSurfaceLayout->setMargin(margin);
            _pChemineeSurfaceLayout->setSpacing(spacing);
        }
    */

public slots:
    virtual void updateContent();
    virtual void apply();

    /**
     * Edite la widget de la source cheminee.
     */
    //  void editSource();


    // Membres
protected:
    QGridLayout* _pChemineeSurfaceLayout;
    //  QLineEdit * _pLineEditSrc;

    QGroupBox* _groupBoxDimensions;
    QGridLayout* _groupBoxDimensionsLayout;
    QLabel*     _dimensionsXLabel;
    QLineEdit*  _dimensionsXLineEdit;
    QLabel*     _dimensionsYLabel;
    QLineEdit*  _dimensionsYLineEdit;

    TYAcousticRectangleWidget* _elmW;
};


#endif // __TY_CHEMINEE_SURFACE_WIDGET__
