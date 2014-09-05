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
 * \file TYRenderWindow.cpp
 * \brief association du graphic (OpenGL) au modeleur
 */


#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/gui/OSizeManager.h"
#include "Tympan/gui/tools/OGLTextElement.h"
#include "Tympan/gui/app/TYApplication.h"
#include "TYRenderWindow.h"


#define IMG(id) OLocalizator::getPicture("TYRenderWindow", (id))


TYRenderWindow::TYRenderWindow(QGLFormat glf, QWidget* parent, const char* name):
    QGLWidget(glf, parent)
{
    setObjectName(name);
    initRenderWindow();
}

TYRenderWindow::TYRenderWindow(QWidget* parent, const char* name):
    QGLWidget(parent)
{
    setObjectName(name);
    initRenderWindow();
}

void TYRenderWindow::initRenderWindow()
{
    _initNeeded = true;
    _showInfos = false;

    _pRenderer = new TYOpenGLRenderer();

    // Label info
    OColor oColor;
    _pOGLTextInfoLabel = new OGLTextElement();
    _pOGLTextInfoLabel->setTextToDisplay("");
    oColor.r = oColor.g = oColor.b = 0.5;
    _pOGLTextInfoLabel->setColor(oColor);
    _pOGLTextInfoLabel->setFont(IMG("id_font"));
    _pOGLTextInfoLabel->setDisplayPosition(10, 10);
    _pOGLTextInfoLabel->setVisibility(false);
    getRenderer()->addOGLElement(_pOGLTextInfoLabel);

    // just enable one double buffering
    // by default, we use the one of vtk
    /*#ifdef QT_DOUBLEBUFFER
      // use the double buffering of QT
      _pRenderWindow->SetSwapBuffers(0);
    #else
      // use the double buffering of vtk
      setAutoBufferSwap(false);
    #endif*/

    // Gestion du focus
    setFocusProxy(parentWidget());
}

TYRenderWindow::~TYRenderWindow()
{
    delete _pOGLTextInfoLabel;
    //_pInfoLabelMapper->Delete();
    delete _pRenderer;//az++
}

TYOpenGLRenderer* TYRenderWindow::getRenderer()
{
    return _pRenderer;
}

void TYRenderWindow::showInfos(bool state)
{
    _showInfos = state;
    _pOGLTextInfoLabel->setVisibility(state);

    //  if (!_showInfos) {
    //      _pInfoLabelMapper->SetInput("");
    //  }

    updateGL();
}

void TYRenderWindow::initializeGL()
{
    _pRenderer->init(width(), height()); //xbh
}

void TYRenderWindow::resizeGL(int w, int h)
{
    _pRenderer->init(width(), height()); //xbh
    updateGL();
    //    _pRenderer->updateDisplayList();
}

void TYRenderWindow::updateGL()
{
    makeCurrent();
    QGLWidget::updateGL();
}

void TYRenderWindow::paintGL()
{
    // On donne les dimension de la fenetre au SizeManager
    OSizeManager::get()->setWidth(width());
    OSizeManager::get()->setHeight(height());

    clock_t timeBefore = clock();
    _pRenderer->OpenGLRender();
    clock_t timeAfter = clock();


    double timeElaspedInSec = ((double)(timeAfter - timeBefore)) / (double)CLOCKS_PER_SEC;

    if (_showInfos)
    {
        writeOutputMsg(QString("FPS = %1").arg(1.0 / timeElaspedInSec, 0, 'f', 2));
        _pOGLTextInfoLabel->setTextToDisplay(QString("FPS = %1").arg(1.0 / timeElaspedInSec, 0, 'f', 2));
    }
}
