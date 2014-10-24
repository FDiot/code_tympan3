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
 * \file TYInfraToolbar.h
 * \brief Barre d'outil infastructure (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_INFRA_TOOLBAR__
#define __TY_INFRA_TOOLBAR__


#include <qtoolbar.h>

class QMainWindow;
class QToolButton;
class QButtonGroup;


/**
 * \class TYInfraToolbar
 * \brief Barre d'outil infastructure.
 */
class TYInfraToolbar : public QToolBar
{
    Q_OBJECT

public:
    /**
     * Constructeur par defaut.
     */
    TYInfraToolbar(QButtonGroup* pBtnGroup, QMainWindow* parent = 0, QString title = "");
    /**
     * Destructeur.
     */
    virtual ~TYInfraToolbar();


public slots:
    /**
     * Active/Desactive chaque boutons.
     */
    void activeButtons(bool active);

protected:
    ///Bouton source ponctuelle.
    QToolButton* _toolButtonSrcPonct;
    ///Bouton etage.
    QToolButton* _toolButtonEtage;
    ///Bouton Silo
    QToolButton* _toolButtonSilos;
    ///Bouton route.
    QToolButton* _toolButtonRoute;
    ///Bouton reseau de transport.
    QToolButton* _toolButtonReseau;
};

#endif //__TY_INFRA_TOOLBAR__
