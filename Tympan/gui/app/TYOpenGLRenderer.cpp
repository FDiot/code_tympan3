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
 * \file TYOpenGLRenderer.cpp
 * \brief Realise le rendu VTK et le rendu OpenGL
 *
 *
 */




#include "Tympan/models/business/TYProjet.h"
#include "Tympan/models/business/geometry/TYGeometryNode.h"//az++ pour updateDisplayListOverlay

/*
#if defined(WIN32)
#include <crtdbg.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif
*/
///La tolerance pour le picking.
static const float ElementPickerTolerance = 0.01f;

bool GetADisplayList(GLuint& displayList)
{
    if (0 == displayList)
    {
        displayList = glGenLists(1);
        if (0 == displayList)
        {
            GLenum errorCode = glGetError();
            const unsigned char* sError = gluErrorString(errorCode);
            printf("Erreur OpenGL %d : %s\n", errorCode, sError);
            return false;
        }
    }
    return true;
}

TYOpenGLRenderer::TYOpenGLRenderer()//az++
{
    _pElement = NULL;
    _displayList = 0;//az++
    _displayListOverlay = 0;//az++
    _pActiveCamera = NULL;
    _pBackgroundColor = new double[3];
    m_renderType = 0;
}

TYOpenGLRenderer::~TYOpenGLRenderer()//az++
{
    if (_displayList) //az++
    {
        glDeleteLists(_displayList, 1);
    }
    if (_displayListOverlay) //az++
    {
        glDeleteLists(_displayListOverlay, 1);
    }
    delete [] _pBackgroundColor;
}

void TYOpenGLRenderer::init(int width, int height)
{
    //  glClearColor(1.0, 1.0, 1.0, 1.0);
    glClearColor(_pBackgroundColor[0], _pBackgroundColor[1], _pBackgroundColor[2], 1.0);
    glColor3f(1.0, 0.0, 0.0);

    // Lights
    glEnable(GL_LIGHTING);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    static GLfloat ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    static GLfloat ambientBIS[] = { 0.8f, 0.8f, 0.8f, 0.8f };
    static GLfloat diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    static GLfloat specular[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    static GLfloat shininess = 50.;
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientBIS);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);


    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    m_width = width;
    m_height = height;
    if (_pActiveCamera)
    {
        _pActiveCamera->setSize(m_width, m_height);
    }
}

void TYOpenGLRenderer::updateDisplayListOverlay(TYGeometryNode* pElement, TYGeometryNode* pDansCeRepere)
{
    if (GetADisplayList(_displayListOverlay))
    {
        if (pElement)
        {
            //Chercher le GeoNode correspondant a pElement (le repere dans lequel est exprime pElement)
            TYGeometryNode* pGeoNode = pElement;
            if (pElement->isA("TYGeometryNode"))
            {
                pGeoNode = (TYGeometryNode*)pElement;
            }
            else
            {
                pGeoNode = (TYGeometryNode*)pElement->GetGeoNodeParent();
            }

            if (pGeoNode)
            {
                TYElementGraphic* pTYElementGraphic = (TYElementGraphic*)pGeoNode->getGraphicObject();
                TYGeometryNodeGraphic* pGeoNodeGraphic = (TYGeometryNodeGraphic*)pTYElementGraphic;
                pGeoNodeGraphic->displayPushingParentMatrix(GL_COMPILE, pDansCeRepere);
            }
        }
    }
}

void TYOpenGLRenderer::updateDisplayList(void)
{
    //Gestion de la BB globale d'affichage : a revoir
    OBox reset;//az++
    TYElementGraphic::_globalBoundingBox = reset;//az++
    //Affectation de la displayList principale:
    if (GetADisplayList(_displayList))
    {
        glNewList(_displayList, GL_COMPILE);
        drawElement();
        glEndList();
    }
}

void TYOpenGLRenderer::Render()
{ OpenGLRender(); }

void TYOpenGLRenderer::OpenGLRender(GLenum mode /*= GL_RENDER*/, int x/* = 0*/, int y/* = 0*/)
{
    GLint polygon_mode[2];
    glGetIntegerv(GL_POLYGON_MODE, polygon_mode);

    // do the render library specific stuff
    this->OpenGLDeviceRender(mode, x, y);

    glPolygonMode(GL_FRONT_AND_BACK, polygon_mode[0]);
}




void TYOpenGLRenderer::OpenGLDeviceRender(GLenum mode /*= GL_RENDER*/, int x/* = 0*/, int y/* = 0*/)
{
    if (_pActiveCamera)
    {
        _pActiveCamera->lookAt();
    }

    // set matrix mode for actors
    glMatrixMode(GL_MODELVIEW);

    this->OpenGLUpdateGeometry(mode, x, y);

    // clean up the model view matrix set up by the camera
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

}

int TYOpenGLRenderer::OpenGLUpdateGeometry(GLenum mode /*= GL_RENDER*/, int x/* = 0*/, int y/* = 0*/)
{
    // Gestion des lumieres
    if (_pActiveCamera)
    {
        // place la lumiere 0 sur la camera
        double xc, yc, zc;
        _pActiveCamera->getPosition(xc, yc, zc);
        _tabLights[0]->setPosition(OPoint3D(xc, yc, zc));
    }
    for (int il = 0; il < _tabLights.size(); il++)
    {
        _tabLights[il]->render();
    }

    // Materials
    float colorSpec[] = { 0.0, 0.0, 0.0, 1.0 };
    float colorShine[] = { 127.0 };
    glMaterialfv(GL_FRONT, GL_SPECULAR, colorSpec);
    glMaterialfv(GL_FRONT, GL_SHININESS, colorShine);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    // Antialiasing
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);

    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);
    glDepthRange(0, 1);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (mode == GL_SELECT)
    {
        GLint viewport[4];
        GLfloat proj[16];

        glGetFloatv(GL_PROJECTION_MATRIX, proj);

        glGetIntegerv(GL_VIEWPORT, viewport);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluPickMatrix((GLdouble)x, (GLdouble)y, 5, 5, viewport);
        glMultMatrixf(proj);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        drawElement(GL_SELECT);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
    }
    else
    {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        if (_displayList) //az++
        {
            glCallList(_displayList);
        }
        if (_displayListOverlay) //az++
        {
            glCallList(_displayListOverlay);    //az++
        }
        glPopMatrix();
    }

    // CLM-NT35: Init la liste avant d'ajouter les elements
    //display overlay text for selected elements (localized)
    glNewList(_displayListOverlay, GL_COMPILE);
    std::vector<TYElement*>::iterator iterElements;
    for (iterElements = _tabSelectedElements.begin(); iterElements < _tabSelectedElements.end(); iterElements++)
    {
        TYGeometryNode* pGeoNode = TYGeometryNode::GetGeoNode(*iterElements);
        TYGeometryNode* pRootGeometryNode = TYGeometryNode::GetGeoNode(_pElement);
        updateDisplayListOverlay(pGeoNode, pRootGeometryNode);
    }
    glEndList();

    //render the TYAtcor2D list
    std::vector<OGLElement*>::iterator iter;
    for (iter = _tabOGLElement.begin(); iter < _tabOGLElement.end(); iter++)
    {
        glPushMatrix();
        (*iter)->render();
        glPopMatrix();
    }

    return 0;
}

void TYOpenGLRenderer::drawElement(GLenum mode)
{
    LPTYElementGraphic pGraphObj = NULL;
    if (_pElement)
    {
        pGraphObj = _pElement->getGraphicObject();

        if (dynamic_cast<TYSiteNode*>(_pElement) != nullptr)
        {
            TYElement* pParent = _pElement->getParent();
            if (pParent && pParent->isA("TYProjet"))
            {
                pGraphObj = pParent->getGraphicObject();
            }
        }

        glRotatef(-90.0, 1.0, 0.0, 0.0);
        if (pGraphObj) { pGraphObj->display(mode); }
    }
}

void TYOpenGLRenderer::addOGLElement(OGLElement* pOGLElement)
{
    bool _bFinded = false;
    std::vector<OGLElement*>::iterator iter;
    for (iter = _tabOGLElement.begin(); !_bFinded && (iter < _tabOGLElement.end()); iter++)
    {
        if ((OGLElement*)(*iter) == pOGLElement)
        {
            _bFinded = true;
        }
    }
    if (!_bFinded)
    {
        _tabOGLElement.push_back(pOGLElement);
    }
}

void TYOpenGLRenderer::addLight(OGLLightElement* pOGLElement)
{
    _tabLights.push_back(pOGLElement);
}

void TYOpenGLRenderer::removeLights()
{
    _tabLights.clear();
}

std::vector<OGLLightElement*> TYOpenGLRenderer::getLights()
{
    return _tabLights;
}

void TYOpenGLRenderer::removeOGLElement(OGLElement* pOGLElement)
{
    std::vector<OGLElement*>::iterator iter;
    bool _bFinded = false;
    for (iter = _tabOGLElement.begin(); !_bFinded && (iter < _tabOGLElement.end()); iter++)
    {
        if ((OGLElement*)(*iter) == pOGLElement)
        {
            _bFinded = true;
            _tabOGLElement.erase(iter);
            break;
        }
    }
}


void TYOpenGLRenderer::addSelectedElement(TYElement* pElement)
{
    bool _bFinded = false;
    std::vector<TYElement*>::iterator iter;
    for (iter = _tabSelectedElements.begin(); !_bFinded && (iter < _tabSelectedElements.end()); iter++)
    {
        if ((TYElement*)(*iter) == pElement)
        {
            _bFinded = true;
        }
    }
    if (!_bFinded)
    {
        _tabSelectedElements.push_back(pElement);
    }
}

void TYOpenGLRenderer::removeSelectedElement(TYElement* pElement)
{
    std::vector<TYElement*>::iterator iter;
    bool _bFinded = false;
    for (iter = _tabSelectedElements.begin(); !_bFinded && (iter < _tabSelectedElements.end()); iter++)
    {
        if ((TYElement*)(*iter) == pElement)
        {
            _bFinded = true;
            _tabSelectedElements.erase(iter);
            break;
        }
    }
}
