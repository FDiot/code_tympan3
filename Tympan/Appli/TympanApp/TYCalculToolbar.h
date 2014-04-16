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
 * \file TYCalculToolbar.h
 * \brief Barre d'outil calcul (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_CALCUL_TOOLBAR__
#define __TY_CALCUL_TOOLBAR__


#include <qtoolbar.h>

class QMainWindow;
class QToolButton;
class QButtonGroup;


/**
 * \class TYCalculToolbar
 * \brief Barre d'outil calcul.
 */
class TYCalculToolbar : public QToolBar
{
    Q_OBJECT

public:
    /**
     * \fn TYCalculToolbar(QButtonGroup * pBtnGroup, QMainWindow * parent = 0, QString title = "")
     * \brief Constructeur par defaut.
     */
    TYCalculToolbar(QButtonGroup* pBtnGroup, QMainWindow* parent = 0, QString title = "");
    /**
     * \fn virtual ~TYCalculToolbar()
     * \brief Destructeur.
     */
    virtual ~TYCalculToolbar();


public slots:
    /**
     * \fn void activeButtons(bool active)
     * \brief Active/Desactive chaque boutons.
     */
    void activeButtons(bool active);

protected:
    ///Bouton maillage rectangulaire.
    QToolButton* _toolButtonMailRect;
    ///Bouton maillage lineaire.
    QToolButton* _toolButtonMailLin;
    ///Bouton maillage verticale.
    QToolButton* _toolButtonMailVert;
    ///Bouton recepteur.
    QToolButton* _toolButtonRecept;
    ///BoundaryNoiseMapbutton.
    QToolButton* _toolButtonBoundaryNoiseMap;
};

#endif //__TY_CALCUL_TOOLBAR__
