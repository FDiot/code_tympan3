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
 
/*
 *
 */

/**
 * \file TYAbstractSceneEditor.h
 * \brief gestion de l'interaction entre la vue graphique (2D ou 3D) et le clavier et la souris (fichier header)
 * \author Projet_Tympan
 */

#ifndef __TY_ABSTRACT_SCENE_EDITOR__
#define __TY_ABSTRACT_SCENE_EDITOR__


#include "TYAppDefines.h"
#include <qobject.h>

class OGLTextElement;

/**
 * \class TYAbstractSceneEditor
 * \brief Classe abstraite pour la gestion de l'interaction entre la vue graphique (2D ou 3D) et le clavier et la souris.
 */
class TYAbstractSceneEditor : public QObject
{
    Q_OBJECT

public:
    /**
     * \fn virtual ~TYAbstractSceneEditor();
     * \brief Destructeur.
     */
    virtual ~TYAbstractSceneEditor();


    /**
     * \fn void updateText(QString msg = "", int posX = 0, int posY = 0, bool show = true)
     * \brief Mets a jour le texte informatif sur la vue 3D.
     * \param msg Le texte a afficher.
     * \param x La position en X du texte a afficher.
     * \param y La position en Y du texte a afficher.
     * \param show Affiche ou pas le texte.
     */
    void updateText(QString msg = "", int posX = 0, int posY = 0, bool show = true);
    /**
     * \fn void showText(bool show = true)
     * \brief Affiche ou pas le texte informatif sur la vue 3D.
     * \param show Affiche ou pas le texte.
     */
    void showText(bool show = true);


    /**
     * \fn static void snapToGrid(float& x, float& y, float& z)
     * \brief Methode utilitaire qui adapte les coordonnees d'un point pour
     * que celui-ci soit aligne avec la grille imaginaire de positionnement.
     * \param x Coord x du point a convertir.
     * \param y Coord y du point a convertir.
     * \param z Coord z du point a convertir.
     */
    static void snapToGrid(float& x, float& y, float& z);
    /**
     * \fn static void snapToGrid(double& x, double& y, double& z)
     * \brief Methode utilitaire qui adapte les coordonnees d'un point pour
     * que celui-ci soit aligne avec la grille imaginaire de positionnement.
     * \param x Coord x du point a convertir.
     * \param y Coord y du point a convertir.
     * \param z Coord z du point a convertir.
     */
    static void snapToGrid(double& x, double& y, double& z);


public slots:
    /**
     * \fn virtual void connect()
     * \brief Connecte cet editor a l'interactor associe.
     */
    virtual void connect();
    /**
     * \fn virtual void disconnect()
     * \brief Deconnecte cet editor a l'interactor associe.
     */
    virtual void disconnect();


    /**
     * \name Interface des Editors
     */
    //@{

    /**
     * \fn virtual void init()
     * \brief Appeler avant l'utilisation de l'editor.
     */
    virtual void init()     { }
    /**
     * \fn  virtual void cancel()
     * \brief Annule l'action courante de l'editor.
     */
    virtual void cancel()   { }
    /**
     * \fn virtual void close()
     * \brief Appeler apres l'utilisation de l'editor.
     */
    virtual void close()    { }
    /**
     * \fn virtual void slotMousePressed(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
     * \brief Une touche de la souris a ete enfoncee.
     * \param x Position du curseur en x.
     * \param y Position du curseur en y.
     * \param button Le bouton a l'origine de cet evenement.
     * \param state L'etat des boutons.
     */
    virtual void slotMousePressed(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)    { }
    /**
     * \fn virtual void slotMouseReleased(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
     * \brief Une touche de la souris a ete relachee.
     * \param x Position du curseur en x.
     * \param y Position du curseur en y.
     * \param button Le bouton a l'origine de cet evenement.
     * \param state L'etat des boutons.
     */
    virtual void slotMouseReleased(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)   { }
    /**
     * \fn virtual void slotMouseDoubleClicked(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
     * \brief Une touche de la souris a ete double clickee.
     * \param x Position du curseur en x.
     * \param y Position du curseur en y.
     * \param button Le bouton a l'origine de cet evenement.
     * \param state L'etat des boutons.
     */
    virtual void slotMouseDoubleClicked(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)  { }
    /**
     * \fn virtual void slotMouseMoved(int x, int y, Qt::MouseButtons button, Qt::KeyboardModifiers state)
     * \brief La souris a ete deplacee.
     * \param x Position du curseur en x.
     * \param y Position du curseur en y.
     * \param button Le bouton a l'origine de cet evenement.
     * \param state L'etat des boutons.
     */
    virtual void slotMouseMoved(int x, int y, Qt::MouseButtons button, Qt::KeyboardModifiers state) { }
    /**
     * \fn virtual void slotWheeled(int x, int y, int delta, Qt::KeyboardModifiers state)
     * \brief La molette de la souris a ete utilisee.
     * \param x Position du curseur en x.
     * \param y Position du curseur en y.
     * \param delta Le nombre de crans de molette.
     * \param state L'etat des boutons.
     */
    virtual void slotWheeled(int x, int y, int delta, Qt::KeyboardModifiers state) { }

    /**
     * \fn  virtual void slotKeyReleased(int key)
     * \brief Une touche a ete relachee.
     * \param key La touche concernee.
     */
    virtual void slotKeyReleased(int key)   { }
    /**
     * \fn virtual void slotViewTypeChanged(int view)
     * \brief La vue courante du modeler a change.
     * \param view L'indice de la nouvelle vue.
     */
    virtual void slotViewTypeChanged(int view)  { }
    /**
     * \fn virtual void slotMouseEnter()
     * \brief Le curseur de souris est entree dans la widget.
     */
    virtual void slotMouseEnter()   { }
    /**
     * \fn virtual void slotMouseLeave()
     * \brief Le curseur de souris est sortie dans la widget.
     */
    virtual void slotMouseLeave()   { }

    //@}


protected:
    /**
     * \fn TYAbstractSceneEditor(TYModelerFrame * pModeler)
     * \brief Constructeur par defaut.
     * \param pModeler Le modeler dans lequel participe cet editor.
     */
    TYAbstractSceneEditor(TYModelerFrame* pModeler);

    /**
     * \fn void displayToWorld(double x, double y, double z, double* worldPoint)
     * \brief Methode utilitaire pour convertir un point en coordonnees ecran
     * en coordonnees globale.
     * \param x Coord x du point a convertir.
     * \param y Coord y du point a convertir.
     * \param z Coord z du point a convertir.
     * \param worldPoint Le resultat de la conversion.
     */
    void displayToWorld(double x, double y, double z, double* worldPoint);
    /**
     * \fn void worldToDisplay(double x, double y, double z, double * displayPoint)
     * \brief Methode utilitaire pour convertir un point en coordonnees globale
     * en coordonnees ecran.
     * \param x Coord x du point a convertir.
     * \param y Coord y du point a convertir.
     * \param z Coord z du point a convertir.
     * \param displayPoint Le resultat de la conversion.
     */
    void worldToDisplay(double x, double y, double z, double* displayPoint);
    /**
     * \fn void refreshSiteFrame()
     * \brief Rafraichit l'arborescence du TYSiteFrame.
     */
    void refreshSiteFrame();
    /**
     * \fn void refreshProjectFrame()
     * \brief Rafraichit l'arborescence du TYProjectFrame
     */
    void refreshProjectFrame();

    /**
     * \fn void updateSiteFrame()
     * \brief Reconstruit l'arborescence du TYSiteFrame.
     */
    void updateSiteFrame();

protected:
    ///Le modeler associe a cet editor.
    TYModelerFrame* _pModeler;

    ///La vue graphique associee a cet editor.
    TYRenderWindowInteractor* _pInteractor;

    ///Affichage du texte 2D sur la vue 3D
    OGLTextElement* _pOGLTextElement;
};

#endif // __TY_ABSTRACT_SCENE_EDITOR__
