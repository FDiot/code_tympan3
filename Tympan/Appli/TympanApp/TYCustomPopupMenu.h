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
 * \file TYCustomPopupMenu.h
 * \brief Definit un popup menu, necessaire pour maitriser l'ouverture automtique apres un createPopupMenu (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */


#ifndef __TY_CUSTOM_POPUP_MENU__
#define __TY_CUSTOM_POPUP_MENU__

#include <qmenu.h>

/**
 * \class TYCustomPopupMenu
 * \brief Definit un popup menu, necessaire pour maitriser l'ouverture automtique apres un createPopupMenu.
 */
class TYCustomPopupMenu : public QMenu
{
public:
    /**
     * \fn TYCustomPopupMenu(QWidget * parent = 0 )
     * \brief Constructeur.
     */
    TYCustomPopupMenu(QWidget* parent = 0);

    /**
     * \fn TYCustomPopupMenu( const QString & title, QWidget * parent = 0 )
     * \brief Constructeur.
     */
    TYCustomPopupMenu(const QString& title, QWidget* parent = 0);

signals :
    void aboutToShow();
public slots :
    void onAboutToShow();
    virtual void setVisible(bool visible);
};


#endif // __TY_CUSTOM_POPUP_MENU__
