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
 * \file TYRectangleWidget.h
 * \brief outil IHM pour un rectangle (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_RECTANGLE_WIDGET__
#define __TY_RECTANGLE_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYRectangle;
class QLineEdit;
class QGridLayout;
class QLabel;
class QTreeWidget;
class QGroupBox;
class QPushButton;
class TYElementWidget;
class TYSurfaceInterfaceWidget;

/**
 * \class TYRectangleWidget
 * \brief classe de l'objet IHM pour un rectangle
 */
class TYRectangleWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYRectangle)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYRectangleWidget(TYRectangle* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYRectangleWidget();


public slots:
    virtual void updateContent();
    virtual void apply();

    // Membres
protected:

    QGroupBox* _groupBox;
    QTreeWidget* _listViewTabPt;
    QGridLayout* _rectangleLayout;
    QGridLayout* _groupBoxLayout;

    TYElementWidget* _elmW;
    TYSurfaceInterfaceWidget* _surfaceW;
};


#endif // __TY_RECTANGLE_WIDGET__
