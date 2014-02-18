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
 * \file TYToolButton.h
 * \brief outil IHM pour la gestion de la couleur des ToolButton (fichier header)
 *
 *
 *
 *
 *
 */

#ifndef __TY_TOOLBUTTON__
#define __TY_TOOLBUTTON__

#include <qtoolbutton.h>
class QColor;

/**
 * \class TYToolButton
 * \brief Permet de gerer la couleur des ToolButtons.
 */
class TYToolButton : public QToolButton
{
    Q_OBJECT

public:
    /**
     * Constructeur par defaut.
     */
    TYToolButton(QWidget* parent = 0, const char* name = 0);
    /**
     * Destructeur.
     */
    virtual ~TYToolButton();

    /**
     * Set/Get de la couleur.
     */
    QColor getColor() { return _color;}
    /**
     * Set/Get de la couleur.
     */
    void setColor(QColor color) { _color = color; updateColor();}

public slots:
    /**
     * Mise a jour de la couleur du bouton.
     */
    virtual void updateColor();

    /**
     * Selection de la couleur dans une boite de dialogue.
     */
    virtual void selectColor();


signals:
    void colorAccepted(QColor c);
    void colorRejected();


protected:
    ///Couleur du bouton.
    QColor _color;
};

#endif // __TY_TOOLBUTTON__
