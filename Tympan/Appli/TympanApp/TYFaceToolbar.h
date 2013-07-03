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
 * \file TYFaceToolbar.h
 * \brief Barre d'outils Face (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_FACE_TOOLBAR__
#define __TY_FACE_TOOLBAR__


#include <qtoolbar.h>

class QMainWindow;
class QToolButton;
class QButtonGroup;


/**
 * \class TYFaceToolbar
 * \brief Barre d'outils Face.
 */
class TYFaceToolbar : public QToolBar
{
    Q_OBJECT

public:
    /**
     * \fn TYFaceToolbar(QButtonGroup * pBtnGroup, QMainWindow * parent = 0, QString title = "")
     * \brief Constructeur par defaut.
     */
    TYFaceToolbar(QButtonGroup* pBtnGroup, QMainWindow* parent = 0, QString title = "");
    /**
     * \fn virtual ~TYFaceToolbar()
     * \brief Destructeur.
     */
    virtual ~TYFaceToolbar();

public slots:
    /**
     * \fn void activeButtons(bool active)
     * \brief Active/Desactive chaque boutons.
     */
    void activeButtons(bool active);

    void activeBouche(bool active);
    void activeCheminee(bool active);
    void activeFenetre(bool active);

protected:
    ///Bouton Bouche.
    QToolButton* _toolButtonBouche;
    bool _boucheActive;
    ///Bouton Cheminee.
    QToolButton* _toolButtonCheminee;
    bool _chemineeActive;
    ///Bouton Fenetre.
    QToolButton* _toolButtonFenetre;
    bool _fenetreActive;
};

#endif //__TY_FACE_TOOLBAR__
