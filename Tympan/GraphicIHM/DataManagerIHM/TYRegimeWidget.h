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
 * \file TYRegimeWidget.h
 * \brief outil IHM pour un regime (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_REGIME_WIDGET__
#define __TY_REGIME_WIDGET__


#include "TYWidget.h"


class TYRegime;
class TYElementWidget;


/**
 * \class TYRegimeWidget
 * \brief Widget pour la classe TYRegime
 *
 * Cree a partir de TYDirectiviteWidget
 * Classe inutile, uniquement cree pour compatibilite avec les macros
 */
class TYRegimeWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYRegime)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYRegimeWidget(TYRegime* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYRegimeWidget();


public slots:
    virtual void updateContent();
    virtual void apply();

    /**
     * Affiche un menu contextuel.
     */
    void showContextMenu(int row, int col, const QPoint& pos);


    // Membres
protected:

};


#endif // __TY_DIRECTIVITE_WIDGET__
