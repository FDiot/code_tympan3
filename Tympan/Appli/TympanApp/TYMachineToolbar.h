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
 * \file TYMachineToolbar.h
 * \brief Barre d'outils Machine (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_MACHINE_TOOLBAR__
#define __TY_MACHINE_TOOLBAR__


#include <qtoolbar.h>

class QMainWindow;
class QToolButton;
class QButtonGroup;


/**
 * \class TYMachineToolbar
 * \brief Barre d'outils Machine
 */
class TYMachineToolbar : public QToolBar
{
    Q_OBJECT

public:
    /**
     * Constructeur par defaut.
     */
    TYMachineToolbar(QButtonGroup* pBtnGroup, QMainWindow* parent = 0, QString title = "");
    /**
     * Destructeur.
     */
    virtual ~TYMachineToolbar();

public slots:
    /**
     * Active/Desactive chaque boutons.
     */
    void activeButtons(bool active);

protected:
    ///Bouton box.
    QToolButton* _toolButtonBox;
    ///Bouton cylindre.
    QToolButton* _toolButtonCyl;
    ///Bouton demi cylindre.
    QToolButton* _toolButtonSemiCyl;
};

#endif //__TY_MACHINE_TOOLBAR__
