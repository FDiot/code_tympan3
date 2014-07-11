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
 * \file TYOpenGLRenderer.h
 * \brief Realise le rendu VTK et le rendu OpenGL (fichier header)
 *
 *
 *
 *
 *
 *
 */


#ifndef __TY_OPENGL_RENDERER__
#define __TY_OPENGL_RENDERER__



#ifdef _WIN32
#include<windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>


#include "Tympan/models/common/3d.h"

#include "Tympan/models/business/TYElement.h"

#include "Tympan/GraphicIHM/ToolsGraphic/OGLLightElement.h"
#include "Tympan/GraphicIHM/ToolsGraphic/OGLElement.h"
#include "Tympan/GraphicIHM/ToolsGraphic/OGLCamera.h"

class TYGeometryNode;

/**
 * \class TYOpenGLRenderer
 * \brief Realise le rendu VTK et le rendu OpenGL.
 */
class TYOpenGLRenderer
{

public:
private:
    /**
    * Constructeur par copie a ne pas implementer mais
    */
    TYOpenGLRenderer(const TYOpenGLRenderer&);  // Not implemented.
    /**
    * Operateur d'affectaion a ne pas implementer mais
    */
    void operator=(const TYOpenGLRenderer&);  // Not implemented.

public:

    /**
    * Constructeur
    */
    TYOpenGLRenderer();

    /**
    * Destructeur
    */
    ~TYOpenGLRenderer();

    /**
    * Initialize the renderer
    */
    void init(int width, int height);

    /**
    * Met a jour la display liste globale, (remplie dans chaque methode XXXGraphic::display).
    */
    void updateDisplayList(void);
    /**
    * Met a jour la display liste servant a bouger les elements,
    * Cela permet de ne redessiner que l'element quand on le bouge (et pas tout le reste).
    * @param pElement l'element a afficher.
    * @param pRootGeometryNode : definit le GeoNode (par exemple le site root) dans lequel s'exprime les coordonnees de pElement
    * les changements de repere de ses GeoNodeGraphic parents (via push/pop de matrices OpenGL).
    */
    void updateDisplayListOverlay(TYGeometryNode* pElement, TYGeometryNode* pDansCeRepere);

    /**
    * Mise a jour de la geometrie
    *
    * @param mode Le mode de rendu.
    * @param x La position de la souris en x pour le picking.
    * @param y La position de la souris en y pour le picking.
    */
    int OpenGLUpdateGeometry(GLenum mode = GL_RENDER, int x = 0, int y = 0);

    /**
    * Picking
    *
    * @param mode Le mode de rendu.
    * @param x La position de la souris en x pour le picking.
    * @param y La position de la souris en y pour le picking.
    */
    void OpenGLDeviceRender(GLenum mode = GL_RENDER, int x = 0, int y = 0);

    /**
    * Rendu OpenGL
    *
    * @param mode Le mode de rendu.
    * @param x La position de la souris en x pour le picking.
    * @param y La position de la souris en y pour le picking.
    */
    void OpenGLRender(GLenum mode = GL_RENDER, int x = 0, int y = 0);

    /**
    * Appel OpenGLRender
    *
    */
    void Render();

    /**
    * Set de l'element a representer.
    */
    void setElement(LPTYElement pElt) {_pElement = pElt;}

    /**
    * deepCopy de l'element a representer.
    */
    void deepCopyElement(LPTYElement pElt) {_pElement->deepCopy(pElt);}

    /**
    * Get de l'element a representer.
    */
    LPTYElement getElement() { return _pElement;}

    /**
    * Display.
    */
    void drawElement(GLenum mode = GL_RENDER);

    /**
    * Ajoute un OGLElement a la liste.
    *
    * @param pOGLElement Le OGLElement a ajouter.
    */
    void addOGLElement(OGLElement* pOGLElement);

    void addLight(OGLLightElement* pOGLElementLight);
    void removeLights();
    std::vector<OGLLightElement*> getLights();

    void setActiveCamera(OGLCamera* pCamera) { _pActiveCamera = pCamera;};
    OGLCamera* getActiveCamera() { return _pActiveCamera;};

    void setBackground(double* bgColor) {_pBackgroundColor[0] = bgColor[0]; _pBackgroundColor[1] = bgColor[1]; _pBackgroundColor[2] = bgColor[2];};

    /**
    * Enleve un OGLElement a la liste.
    *
    * @param pTYActor2D Le OGLElement a enlever.
    */
    void removeOGLElement(OGLElement* pOGLElement);

    void setRenderType(int renderType) {m_renderType = renderType;};

    void addSelectedElement(TYElement* pElement);
    void removeSelectedElement(TYElement* pElement);

protected :
    ///Element a representer.
    TYElement* _pElement;

    ///Display liste globale.
    GLuint _displayList;
    ///Display liste pour les deplacements.
    GLuint _displayListOverlay;
    ///Mode de rendu.
    GLuint _polygoneMode;

    std::vector<OGLElement*> _tabOGLElement;

    //Elements selectionnes
    std::vector<TYElement*> _tabSelectedElements;

    //Liste des lumieres dans la scene 3D
    std::vector<OGLLightElement*> _tabLights;

    OGLCamera*  _pActiveCamera;
    double*     _pBackgroundColor;
    int m_width;
    int m_height;

    int m_renderType; // 0 --> render all, 1 --> render 2D, 2 --> render 3D
};

#endif // __TY_OPENGL_RENDERER__
