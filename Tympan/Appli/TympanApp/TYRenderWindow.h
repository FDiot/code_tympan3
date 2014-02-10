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
 * \file TYRenderWindow.h
 * \brief association du graphic (OpenGL) au modeleur (fichier header)
 *
 *
 *
 *
 *
 */


#ifndef __TY_RENDER_WINDOW__
#define __TY_RENDER_WINDOW__


#include <qgl.h>

#include "TYOpenGLRenderer.h"

class OGLTextElement;

/**
 * \class TYRenderWindow
 * \brief Combine QGLWidget de Qt pour Tympan.
 */
class TYRenderWindow : public QGLWidget
{
    Q_OBJECT

public:
    /**
     * Constructeur.
     *
     * @param glf Le QGLFormat, qui va directement etre passe au QGLWidget herite.
     * @param pParent Directement passe au QGLWidget herite.
     * @param name Directement passe au QGLWidget herite.
     */
    TYRenderWindow(QGLFormat glf, QWidget* pParent = 0, const char* name = 0);

    /**
     * Constructeur.
     *
     * @param pParent Directement passe au QGLWidget herite.
     * @param name Directement passe au QGLWidget herite.
     */
    TYRenderWindow(QWidget* pParent = 0, const char* name = 0);

    /**
    * Destructeur.
    */
    ~TYRenderWindow();


    /**
     * Retourne un pointeur sur le OpenGLRenderer utilise.
     *
     * @return Un pointeur sur le OpenGLRenderer utilise.
     */
    TYOpenGLRenderer* getRenderer();

    /**
     * Indique si les informations de rendu sont affichees.
     */
    bool getShowInfos() { return _showInfos; }


public slots:
    /**
     * Met a jour OpenGL.
     */
    virtual void updateGL();
    /**
     * Affiche des informations de rendu en overlay.
     */
    void showInfos(bool state);

protected:
    /**
     * Initialise la fenetre OpenGL.
     */
    virtual void initializeGL();
    /**
     * Redimensionne la fenetre OpenGL.
     */
    virtual void resizeGL(int w, int h);
    /**
     * Lance le rendu de la scene.
     */
    virtual void paintGL();

    inline QSizePolicy sizePolicy() const
    {
        return QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    }
    inline QSize sizeHint() const {return QSize(50, 50); }
    inline QSize minimumSizeHint() const  { return QSize(50, 50); }

private:
    /**
     * Initialise ce TYRenderWindow.
     * Cette methode est appelee par les constructeurs.
     */
    void initRenderWindow();


private:
    bool _initNeeded;
    bool _showInfos;
    TYOpenGLRenderer*     _pRenderer;
    OGLTextElement* _pOGLTextInfoLabel;
};

#endif // __TY_RENDER_WINDOW__
