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
 * \file TYSurfaceInterfaceWidget.h
 * \brief outil IHM pour la gestion des objets de type SurfaceInterface (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_SURFACE_INTERFACE_WIDGET__
#define __TY_SURFACE_INTERFACE_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYSurfaceInterface;
class QLineEdit;
class QGridLayout;
class QLabel;
class QGroupBox;

/**
 * \class TYSurfaceInterfaceWidget
 * \brief classe de l'objet IHM pour la gestion des objets de type SurfaceInterface
 */
class TYSurfaceInterfaceWidget : public TYWidget
{
    Q_OBJECT

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYSurfaceInterfaceWidget(TYSurfaceInterface* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYSurfaceInterfaceWidget();


    TYSurfaceInterface* getElement() { return _pElement; }  //sm++ Correction de la macro TY_DECL_METIER_WIDGET


public slots:
    virtual void updateContent();
    virtual void apply();


    // Membres
protected:
    TYSurfaceInterface* _pElement; //sm++ Correction de la macro TY_DECL_METIER_WIDGET

    QGroupBox* _groupBox;
    QLineEdit* _lineEditAire;
    QLabel* _labelAire;

    QGridLayout* _groupBoxLayout;
    QGridLayout* _surfaceInterfaceLayout;
};


#endif // __TY_SURFACE_INTERFACE_WIDGET__
