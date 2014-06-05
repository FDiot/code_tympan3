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
 * \file TYBatimentToolbar.h
 * \brief Barre d'outils Batiment (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 *
 */

#ifndef __TY_BATIMENT_TOOLBAR__
#define __TY_BATIMENT_TOOLBAR__

#include <qtoolbar.h>

class QMainWindow;
class QToolButton;
class QButtonGroup;


/**
 * \class TYBatimentToolbar
 * \brief Barre d'outils Batiment
 */
class TYBatimentToolbar : public QToolBar
{
    Q_OBJECT

public:
    /**
     * \fn TYBatimentToolbar(QButtonGroup * pBtnGroup, QMainWindow * parent = 0, QString title = "" )
     * \brief Constructeur par defaut.
     */
    TYBatimentToolbar(QButtonGroup* pBtnGroup, QMainWindow* parent = 0, QString title = "");
    /**
     * \fn virtual ~TYBatimentToolbar()
     * \brief Destructeur.
     */
    virtual ~TYBatimentToolbar();

public slots:
    /**
     * \fn void activeButtons(bool active)
     * \brief Active/Desactive chaque boutons.
     */
    void activeButtons(bool active);

protected:
    ///Bouton etage.
    QToolButton* _toolButtonEtage;

    ///Bouton silos.
    QToolButton* _toolButtonSilos;

    /// Bouton source ponctuelle
    QToolButton* _toolButtonSource;
};

#endif //__TY_BATIMENT_TOOLBAR__
