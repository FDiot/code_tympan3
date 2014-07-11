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
 * \file TYAcousticRectangleWidget.h
 * \brief outil IHM pour un rectangle acoustique (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_ACOUSTICRECTANGLE_WIDGET__
#define __TY_ACOUSTICRECTANGLE_WIDGET__


#include "TYWidget.h"
#include <qlayout.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYAcousticRectangle;
class QLineEdit;
class QGridLayout;
class QLabel;
class TYAcousticSurfaceWidget;

/**
 * \class TYAcousticRectangleWidget
 * \brief classe d'un objet IHM pour un rectangle acoustique
 */
class TYAcousticRectangleWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYAcousticRectangle)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYAcousticRectangleWidget(TYAcousticRectangle* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYAcousticRectangleWidget();

    /**
     * Get du sous-element acousticSurfaceWidget.
     */
    TYAcousticSurfaceWidget* getSurfaceWidget() {return _elmW;}

    /**
     * Permet de modifier les parametres du layout.
     *
     * @param margin La valeur de la marge.
     * @param spacing La valeur de l'espacement.
     */
    void setLayoutSpacing(int margin, int spacing)
    {
        _acousticRectangleLayout->setMargin(margin);
        _acousticRectangleLayout->setSpacing(spacing);
    }


public slots:
    virtual void updateContent();
    virtual void apply();


    // Membres
protected:

    QGridLayout* _acousticRectangleLayout;
    TYAcousticSurfaceWidget* _elmW;
};


#endif // __TY_ACOUSTICRECTANGLE_WIDGET__
