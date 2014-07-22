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
 * \file TYModelerToolbar.h
 * \brief Barre d'outils generale pour les modelers (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_MODELER_TOOLBAR__
#define __TY_MODELER_TOOLBAR__


#include "TYAppDefines.h"
#include <qtoolbar.h>

class QMainWindow;
class QToolButton;
class QButtonGroup;


/**
 * \class TYModelerToolbar
 * \brief Barre d'outils generale pour les modelers.
 */
class TYModelerToolbar : public QToolBar
{
    Q_OBJECT

public:
    /**
     * Constructeur par defaut.
     */
    TYModelerToolbar(QButtonGroup* pBtnGroup = 0, QMainWindow* parent = 0, QString title = "");
    /**
     * Destructeur.
     */
    virtual ~TYModelerToolbar();
    /**
     * Active/Desactive les boutons de mode camera suivant la vue.
     */
    void activeCameraButtons(int view);

public slots:
    /**
     * Active/Desactive chaque boutons.
     */
    void activeButtons(bool active);

    /**
     * Active/desactive les boutons qui permettent une modification
     */
    void activeModif(bool active);

    /**
     * Associe un bouton group a cette toolbar.
     */
    void addToButtonGroup(QButtonGroup* pBtnGroup);
    /**
     * Supprime l'association d'un bouton group a cette toolbar.
     */
    void removeFromButtonGroup(QButtonGroup* pBtnGroup);
    /**
     * Changement de vue.
     */
    void viewTypeChanged(int view);

protected:
    ///Bouton navigation (camera).
    QToolButton* _toolButtonCamera;
    ///Bouton fly navigation (flyCamera).
    QToolButton* _toolButtonFlyCamera;
    ///Bouton navigation par zones.
    QToolButton* _toolButtonCameraZone;
    ///Bouton mesure de distance.
    QToolButton* _toolButtonDistance;
    ///Bouton deplacement d'objets.
    QToolButton* _toolButtonMoving;
    ///Bouton rotation d'objets.
    QToolButton* _toolButtonRotation;
    ///Bouton edition d'objets.
    QToolButton* _toolButtonEdition;
};

#endif //__TY_MODELER_TOOLBAR__
