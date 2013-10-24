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
 * \file TYAcousticCircleWidget.h
 * \brief outil IHM pour un cercle acoustique (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_ACOUSTICCIRCLE_WIDGET__
#define __TY_ACOUSTICCIRCLE_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYAcousticCircle;
class QLineEdit;
class QGridLayout;
class QLabel;
class TYAcousticSurfaceWidget;

/**
 * \class TYAcousticCircleWidget
 * \brief classe de l'objet IHM pour une surface acoustique
 */
class TYAcousticCircleWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYAcousticCircle)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYAcousticCircleWidget(TYAcousticCircle* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYAcousticCircleWidget();


public slots:
    virtual void updateContent();
    virtual void apply();


    // Membres
protected:

    QGridLayout* acousticCircleLayout;

    TYAcousticSurfaceWidget* _elmW;
};


#endif // __TY_ACOUSTICCIRCLE_WIDGET__
