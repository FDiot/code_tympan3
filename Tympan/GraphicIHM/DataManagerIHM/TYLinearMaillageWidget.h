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
 * \file TYLinearMaillageWidget.h
 * \brief Outil IHM pour un maillage lineaire (fichier header)
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

#ifndef __TY_LINEAR_MAILLAGE_WIDGET__
#define __TY_LINEAR_MAILLAGE_WIDGET__


#include "TYWidget.h"

class TYLinearMaillage;
class QLineEdit;
class TYMaillageWidget;

/**
 * \class TYLinearMaillageWidget
 * \brief classe de l'objet IHM pour un maillage lineaire
 */
class TYLinearMaillageWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYLinearMaillage)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYLinearMaillageWidget(TYLinearMaillage* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYLinearMaillageWidget();


public slots:
    virtual void updateContent();
    virtual void apply();

    /**
     * Mets a jour le nombre de points que contiendra le maillage
     * en fonction des parametres entres.
     */
    void updateNbPoints();

    /**
     * Edite la widget du segment.
     */
    void editSegment();


    // Membres
protected:
    QLineEdit* _lineEditNomSegment;
    QLineEdit* _lineEditLongueur;
    QLineEdit* _lineEditDensite;

    TYMaillageWidget* _maillageW;
};


#endif // __TY_LINEAR_MAILLAGE_WIDGET__
