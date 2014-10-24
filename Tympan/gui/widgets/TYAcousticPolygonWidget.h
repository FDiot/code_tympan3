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
 * \file TYAcousticPolygonWidget.h
 * \brief objet IHM pour un polygone acoustique (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_ACOUSTIC_POLYGON_WIDGET__
#define __TY_ACOUSTIC_POLYGON_WIDGET__


#include "TYWidget.h"
#include <qlayout.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYAcousticPolygon;
class QLineEdit;
class QGridLayout;
class QLabel;
class QGroupBox;
class QPushButton;
class TYAcousticSurfaceWidget;


/**
 * \class TYAcousticPolygonWidget
 * \brief classe de l'objet IHM pour un polygone acoustique
 */
class TYAcousticPolygonWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYAcousticPolygon)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYAcousticPolygonWidget(TYAcousticPolygon* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYAcousticPolygonWidget();

    /**
     * Empeche l'affichage de la boite de dialogue materiau (pour la dalle dans le cas d'un bâtiment)
     */
    virtual void hideMat();

    /**
     * Permet de modifier les parametres du layout.
     *
     * @param margin La valeur de la marge.
     * @param spacing La valeur de l'espacement.
     */
    /*
        void setLayoutSpacing(int margin, int spacing) {
            _accPolygonLayout->setMargin(margin);
            _accPolygonLayout->setSpacing(spacing);
        }
    */

public slots:
    virtual void updateContent();
    virtual void apply();

    /**
     * Edite la widget du polygone.
     */
    //  void editPolygon();


    // Membres
protected:
    //  QGroupBox* _groupBoxPolygon;
    //  QLineEdit* _lineEditNomPolygon;
    //  QPushButton* _pushButtonPolygon;

    QGridLayout* _accPolygonLayout;
    QGridLayout* _groupBoxPolygonLayout;
    QGridLayout* _groupBoxParoiLayout;

    TYAcousticSurfaceWidget* _accSurfW;
};


#endif // __TY_ACOUSTIC_POLYGON_WIDGET__
