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
 * \file TYElementGraphic.h
 * \brief Representation graphique d'un element de base (fichier header)
 * \author Projet_Tympan
 */

#ifndef __TY_ELEMENT_GRAPHIC__
#define __TY_ELEMENT_GRAPHIC__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _MSC_VER
#pragma warning( disable : 4284 )
#endif

#if TY_USE_IHM
#include <QtOpenGL>
#include <GL/gl.h>
#include <GL/glu.h>
/* #include <qobject.h> */
/* #include <qgl.h> */
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif // TY_USE_IHM

#include "Tympan/models/common/3d.h"
#include "Tympan/Tools/OColor.h"
#include "Tympan/Tools/OSmartPtr.h"

class TYElement;


#define TY_DECL_METIER_GRAPHIC(classname) \
    public: \
    inline classname * getElement() const { return (classname *) _pElement; }


#include <list>
// Pour l'utilisation de la librairie standard STL.
using std::list;

class TYElementGraphic;
///List de pointeur de TYElement.
typedef list<TYElementGraphic*> TYListPtrTYElementGraphic;

/**
 * \class TYElementGraphic
 * \brief classe graphique pour un element de base
 */
class TYElementGraphic : public IRefCount
{
    // Methodes
public:
    /**
     * Destructeur.
     */
    virtual ~TYElementGraphic();

    /**
     * Definit l'element associe a cet objet.
     */
    void setElement(TYElement* pElt);
    /**
     * Renvoie l'element associe a cet objet (la methode getElement n'est pas membre de la classe de base TYElementGraphic!).
     */
    TYElement* getTYElement() { return _pElement; }

    /**
     * Mets a jour le graph de scene.
     * Si l'element a un parent l'update est propage dans le graph.
     */
    void updateTree();

    /**
     * Mets a jour l'objet 3D associe a cet objet graphique metier.
     * Remarque : Lorsque cette methode est surchargee, un appel de
     * l'implementation parent (TYElementGraphic::update()) doit etre
     * fait a la fin.
     *
     * @param force Outrepasse l'etat du flag 'modified' et effectue la mise a jour.
     */
    virtual void update(bool force = false);

    /**
     * Effectue le trace openGL de l'objet graphique
     *
     * @param mode Gere les modes rendu et select pour le picking.
     */
    virtual void display(GLenum mode = GL_RENDER);

    /**
     * Set/Get du flag indiquant si l'element associe a ete modifie.
     */
    void setModified(bool modified = true) { _modified = modified;}
    /**
     * Set/Get du flag indiquant si l'element associe a ete modifie.
     */
    bool getModified(bool andReset = false);

    /**
     * Set/Get du flag indiquant si l'element est visible.
     */
    void setVisible(bool visible = true) { _visible = visible; }
    /**
     * Set/Get du flag indiquant si l'element est visible.
     */
    bool getVisible() { return _visible;}
    /**
     * Set/Get du flag indiquant si l'element est visible recursivement.
     */
    void setRecursiveVisible(bool visible = true);

    /**
     * Set/Get du flag indiquant si l'element est texture.
     */
    void setTexture(bool state = true) { _texture = state; }
    /**
     * Set/Get du flag indiquant si l'element est texture.
     */
    bool getTexture() { return _texture;}

    /**
     * Get des extremites de la boite englobante de l'objet.
     */
    OBox GetBox() { return _boundingBox;}

    virtual void computeBoundingBox();
    virtual void getChilds(TYListPtrTYElementGraphic& childs, bool recursif = true) {};

    /**
     * Highlight la bounding box de l'objet graphique de l'element metier
     * ou de l'ensemble des objets graphiques le composant.
     */
    void highlight(bool state = true);

    /**
     * Retourne l'etat du Highlight.
     */
    bool getHighlightState();

    /**
     * Affiche uniquement la bounding box.
     */
    void viewOnlyHighlight(bool state = true);
    /**
     * Retourn l'etat d'affichage de la bounding box seule.
     */
    bool getOnlyHighlight() { return _viewOnlyHighlight; }


public:

#if TY_USE_IHM
    virtual void activateSignal() {};

    /**
     * Connecte au signal de notification de mise a jour.
     */
    virtual void connectUpdateSignal(QObject* pReceiver, const char* member) {};
    /**
     * Deconnecte au signal de notification de mise a jour.
     */
    virtual void disconnectUpdateSignal(QObject* pReceiver, const char* member) {};
#endif // TY_USE_IHM

    static OBox _globalBoundingBox;


protected:
    /**
     * Constructeur.
     */
    TYElementGraphic(TYElement* pElt);

    void displayNormal(OVector3D normal, OPoint3D p1);

    void glVertex3fAndEnlargeBB(GLfloat x, GLfloat y, GLfloat z);
    void glVertex3dAndEnlargeBB(GLdouble x, GLdouble y, GLdouble z);
    void gluDiskAndEnlargeBB(GLUquadric*          qobj,
                             GLdouble            innerRadius,
                             GLdouble            outerRadius,
                             GLint               slices,
                             GLint               loops);
    void gluCylinderAndEnlargeBB(GLUquadric*          qobj,
                                 GLdouble            baseRadius,
                                 GLdouble            topRadius,
                                 GLdouble            height,
                                 GLint               slices,
                                 GLint               stacks);
    void gluPartialDiskAndEnlargeBB(GLUquadric*          qobj,
                                    GLdouble            innerRadius,
                                    GLdouble            outerRadius,
                                    GLint               slices,
                                    GLint               loops,
                                    GLdouble            startAngle,
                                    GLdouble            sweepAngle);
    void drawBoundingBox();
    void drawLineBoundingBox();
    void drawName();

public:
    ///Indique si les normals doivent etre visible
    static bool _gDrawNormals;

    ///Indique si les plafonds doivent etre visible
    static bool _gDrawPlafond;

private:
    ///Indique si l'element associe a ete modifie.
    bool _modified;

    ///Inique si on affiche la bounding box seule.
    bool _viewOnlyHighlight;
protected:
    OBox _boundingBox;
    ///L'element metier auquel cet element graphique metier est associe.
    TYElement* _pElement;

    ///Indique si le highlight est active pour cet element.
    bool _highlight;

    ///Indique si le texturing est active pour cet element.
    bool _texture;

    ///Inique si l'element est visible.
    bool _visible;

    bool _bFirstDisp;
};


///Smart pointer sur TYElementGraphic.
typedef SmartPtr<TYElementGraphic> LPTYElementGraphic;


#endif // __TY_ELEMENT_GRAPHIC__
