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
 * \file TYRenderWindowInteractor.h
 * \brief Le role de cette classe est limite a emettre des signaux pouvant etre utilise pour interagir sur le renderer avec le clavier et la souris.(fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */


#ifndef __TY_RENDER_WINDOW_INTERACTOR__
#define __TY_RENDER_WINDOW_INTERACTOR__
#include <QKeyEvent>


#include "TYRenderWindow.h"
//Added by qt3to4:
#include <QMouseEvent>
#include <QWheelEvent>

/**
 * \class TYRenderWindowInteractor
 * \brief Le role de cette classe est limite a emettre des signaux pouvant
 * etre utilise pour interagir sur le renderer avec le clavier et la
 * souris.
 */
class TYRenderWindowInteractor : public TYRenderWindow
{
    Q_OBJECT

public:
    /**
     * Constructeur.
     */
    TYRenderWindowInteractor(QWidget* pParent = 0, const char* name = 0);
    /**
     * Constructeur.
     */
    TYRenderWindowInteractor(QGLFormat glf, QWidget* pParent = 0, const char* name = 0);

    /**
     * Destructeur.
     */
    ~TYRenderWindowInteractor();

signals:
    /**
     * Un bouton de la souris a ete presse.
     *
     * @param x La coordonnees x du curseur.
     * @param y La coordonnees y du curseur.
     * @param button Le bouton qui a provoque l'action.
     * @param state L'etat des boutons Shift et Control.
     */
    void mousePressed(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state);

    /**
     * Un bouton de la souris a ete relache.
     *
     * @param x La coordonnees x du curseur.
     * @param y La coordonnees y du curseur.
     * @param button Le bouton qui a provoque l'action.
     * @param state L'etat des boutons Shift et Control.
     */
    void mouseReleased(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state);

    /**
     * Un bouton de la souris a ete double clicke.
     *
     * @param x La coordonnees x du curseur.
     * @param y La coordonnees y du curseur.
     * @param button Le bouton qui a provoque l'action.
     * @param state L'etat des boutons Shift et Control.
     */
    void mouseDoubleClicked(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state);

    /**
     * La souris a ete deplacee.
     *
     * @param x La coordonnees x du curseur.
     * @param y La coordonnees y du curseur.
     * @param button Le bouton qui a provoque l'action.
     * @param state L'etat des boutons Shift et Control.
     */
    void mouseMoved(int x, int y, Qt::MouseButtons button, Qt::KeyboardModifiers state);

    /**
     * La molette de la souris a ete activee.
     *
     * @param x La coordonnees x du curseur.
     * @param y La coordonnees y du curseur.
     * @param delta Le nombre de crans de molette parcouru.
     * @param state L'etat des boutons Shift et Control.
     */
    void wheeled(int x, int y, int delta, Qt::KeyboardModifiers state);

    /**
     * Une touche a ete enfoncee.
     *
     * @param key La touche concernee.
     */
    void keyPressed(int key);

    /**
     * Une touche a ete relachee.
     *
     * @param key La touche concernee.
     */
    void keyReleased(int key);

protected:
    /**
     * Appeler par les constructeur.
     */
    void init();

    // Evenements Qt.
    virtual void mousePressEvent(QMouseEvent* pEvent);
    virtual void mouseReleaseEvent(QMouseEvent* pEvent);
    virtual void mouseDoubleClickEvent(QMouseEvent* pEvent);
    virtual void mouseMoveEvent(QMouseEvent* pEvent);
    virtual void wheelEvent(QWheelEvent* pEvent);
    virtual void keyPressEvent(QKeyEvent* pEvent);
    virtual void keyReleaseEvent(QKeyEvent* pEvent);
};

#endif  // __TY_RENDER_WINDOW_INTERACTOR__
