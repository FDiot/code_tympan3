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
 * \file TYAbstractSceneEditor.cpp
 * \brief getion de l'edition de
 */


#include "Tympan/models/business/TYPreferenceManager.h"
#include "Tympan/gui/tools/OGLTextElement.h"
#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/gui/app/TYRenderWindowInteractor.h"
#include "Tympan/gui/app/TYModelerFrame.h"
#include "Tympan/gui/app/TYSiteFrame.h"
#include "Tympan/gui/app/TYProjetFrame.h"
#include "Tympan/gui/app/TYApplication.h"
#include "Tympan/gui/app/TYMainWindow.h"
#include "TYAbstractSceneEditor.h"



///Pas de la grille imaginaire de positionnement.
static const double SnapGridStep = 1.0;

#define IMG(id) OLocalizator::getPicture("TYAbstractSceneEditor", (id))

TYAbstractSceneEditor::TYAbstractSceneEditor(TYModelerFrame* pModeler)
{
    Q_CHECK_PTR(pModeler);
    _pModeler = pModeler;
    _pInteractor = _pModeler->getView();

    // Text
    _pOGLTextElement = new OGLTextElement();
    _pOGLTextElement->setDisplayPosition(0, 0);
    _pOGLTextElement->setTextToDisplay("");
    _pOGLTextElement->setFont(IMG("id_font"));
    _pOGLTextElement->setVisibility(false);
    _pInteractor->getRenderer()->addOGLElement(_pOGLTextElement);
}

TYAbstractSceneEditor::~TYAbstractSceneEditor()
{
    _pInteractor->getRenderer()->removeOGLElement(_pOGLTextElement);
    delete _pOGLTextElement;
}

void TYAbstractSceneEditor::refreshSiteFrame()
{
    getTYMainWnd()->getSiteFrame()->updateList();
}

void TYAbstractSceneEditor::refreshProjectFrame()
{
    getTYMainWnd()->getProjetFrame()->updateList();
}

void TYAbstractSceneEditor::updateSiteFrame()
{
    getTYMainWnd()->getSiteFrame()->updateList();
}
void TYAbstractSceneEditor::connect()
{
    // Connections...
    QObject::connect(_pInteractor, SIGNAL(mousePressed(int, int, Qt::MouseButton, Qt::KeyboardModifiers)), this, SLOT(slotMousePressed(int, int, Qt::MouseButton, Qt::KeyboardModifiers)));
    QObject::connect(_pInteractor, SIGNAL(mouseReleased(int, int, Qt::MouseButton, Qt::KeyboardModifiers)), this, SLOT(slotMouseReleased(int, int, Qt::MouseButton, Qt::KeyboardModifiers)));
    QObject::connect(_pInteractor, SIGNAL(mouseDoubleClicked(int, int, Qt::MouseButton, Qt::KeyboardModifiers)), this, SLOT(slotMouseDoubleClicked(int, int, Qt::MouseButton, Qt::KeyboardModifiers)));
    QObject::connect(_pInteractor, SIGNAL(mouseMoved(int, int, Qt::MouseButtons, Qt::KeyboardModifiers)), this, SLOT(slotMouseMoved(int, int, Qt::MouseButtons, Qt::KeyboardModifiers)));
    QObject::connect(_pInteractor, SIGNAL(wheeled(int, int, int, Qt::KeyboardModifiers)), this, SLOT(slotWheeled(int, int, int, Qt::KeyboardModifiers)));
    QObject::connect(_pInteractor, SIGNAL(keyPressed(int)), this, SLOT(slotKeyPressed(int)));
    QObject::connect(_pInteractor, SIGNAL(keyReleased(int)), this, SLOT(slotKeyReleased(int)));
    QObject::connect(_pModeler, SIGNAL(viewTypeChanged(int)), this, SLOT(slotViewTypeChanged(int)));
    QObject::connect(_pModeler, SIGNAL(mouseEnter()), this, SLOT(slotMouseEnter()));
    QObject::connect(_pModeler, SIGNAL(mouseLeave()), this, SLOT(slotMouseLeave()));

    float r = 94.0f, g = 110.0f, b = 255.0f;

    // Couleur de la police
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "FontColorR"))
    {
        TYPreferenceManager::getColor(TYDIRPREFERENCEMANAGER, "FontColor", r, g, b);
    }
    else
    {
        TYPreferenceManager::setColor(TYDIRPREFERENCEMANAGER, "FontColor", r, g, b);
    }

    OColor oColor;
    oColor.r = r / 255.0f;
    oColor.g = g / 255.0f;
    oColor.b = b / 255.0f;
    _pOGLTextElement->setColor(oColor);

    // Update du type de vue actif
    slotViewTypeChanged(_pModeler->getCurrentView());
}

void TYAbstractSceneEditor::disconnect()
{
    // Deconnections...
    QObject::disconnect(_pInteractor, SIGNAL(mousePressed(int, int, Qt::MouseButton, Qt::KeyboardModifiers)), this, SLOT(slotMousePressed(int, int, Qt::MouseButton, Qt::KeyboardModifiers)));
    QObject::disconnect(_pInteractor, SIGNAL(mouseReleased(int, int, Qt::MouseButton, Qt::KeyboardModifiers)), this, SLOT(slotMouseReleased(int, int, Qt::MouseButton, Qt::KeyboardModifiers)));
    QObject::disconnect(_pInteractor, SIGNAL(mouseDoubleClicked(int, int, Qt::MouseButton, Qt::KeyboardModifiers)), this, SLOT(slotMouseDoubleClicked(int, int, Qt::MouseButton, Qt::KeyboardModifiers)));
    QObject::disconnect(_pInteractor, SIGNAL(mouseMoved(int, int, Qt::MouseButtons, Qt::KeyboardModifiers)), this, SLOT(slotMouseMoved(int, int, Qt::MouseButtons, Qt::KeyboardModifiers)));
    QObject::disconnect(_pInteractor, SIGNAL(wheeled(int, int, int, Qt::KeyboardModifiers)), this, SLOT(slotWheeled(int, int, int, Qt::KeyboardModifiers)));
    QObject::disconnect(_pInteractor, SIGNAL(keyPressed(int)), this, SLOT(slotKeyPressed(int)));
    QObject::disconnect(_pInteractor, SIGNAL(keyReleased(int)), this, SLOT(slotKeyReleased(int)));
    QObject::disconnect(_pModeler, SIGNAL(viewTypeChanged(int)), this, SLOT(slotViewTypeChanged(int)));
    QObject::disconnect(_pModeler, SIGNAL(mouseEnter()), this, SLOT(slotMouseEnter()));
    QObject::disconnect(_pModeler, SIGNAL(mouseLeave()), this, SLOT(slotMouseLeave()));
}

void TYAbstractSceneEditor::updateText(QString msg, int posX, int posY, bool show)
{
    _pOGLTextElement->setTextToDisplay(msg);
    _pOGLTextElement->setDisplayPosition(posX, posY);

    if (show)
    {
        _pOGLTextElement->setVisibility(true);
    }
}

void TYAbstractSceneEditor::showText(bool show)
{
    _pOGLTextElement->setVisibility(show);
}

void TYAbstractSceneEditor::displayToWorld(double x, double y, double z, double* worldPoint)
{
    NxVec3 ret = OGLCamera::displayToWorld(NxVec3(x, y, z));
    worldPoint[0] = ret.x;
    worldPoint[1] = ret.y;
    worldPoint[2] = ret.z;
}

void TYAbstractSceneEditor::worldToDisplay(double x, double y, double z, double* displayPoint)
{
    NxVec3 ret = OGLCamera::worldToDisplay(NxVec3(x, y, z));
    displayPoint[0] = ret.x;
    displayPoint[1] = ret.y;
    displayPoint[2] = ret.z;
}

void TYAbstractSceneEditor::snapToGrid(float& x, float& y, float& z)
{
    x = ROUND(x / SnapGridStep) * SnapGridStep;
    y = ROUND(y / SnapGridStep) * SnapGridStep;
    z = ROUND(z / SnapGridStep) * SnapGridStep;
}

void TYAbstractSceneEditor::snapToGrid(double& x, double& y, double& z)
{
    x = ROUND(x / SnapGridStep) * SnapGridStep;
    y = ROUND(y / SnapGridStep) * SnapGridStep;
    z = ROUND(z / SnapGridStep) * SnapGridStep;
}
