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
 * \file TYPointRefEditor.cpp
 * \brief gestion de l'edition des points de reference pour le placement des images de fond
 *
 *
 */





#include "Tympan/models/business/OLocalizator.h"


#if defined(WIN32)
#include <crtdbg.h>
#endif

#if defined(WIN32)
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif


#define TR(id) OLocalizator::getString("TYPointCalculEditor", (id))


TYPointRefEditor::TYPointRefEditor(TYSiteModelerFrame* pModeler) :
    TYAbstractSceneEditor(pModeler)
{
    _active = false;

    slotViewTypeChanged(_pModeler->getCurrentView());
}

TYPointRefEditor::~TYPointRefEditor()
{
}

void TYPointRefEditor::init()
{
}

void TYPointRefEditor::close()
{
}

void TYPointRefEditor::slotViewTypeChanged(int view)
{
    if (view == TYModelerFrame::TopView)
    {
        _active = true;
    }
    else
    {
        _active = false;
    }
}

void TYPointRefEditor::slotMousePressed(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
{
    if ((button == Qt::LeftButton) && _active)
    {
    }
}

void TYPointRefEditor::slotMouseMoved(int x, int y, Qt::MouseButtons button, Qt::KeyboardModifiers state)
{
    if ((button == Qt::LeftButton) && _active)
    {
    }
}

void TYPointRefEditor::slotMouseReleased(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
{
    if ((button == Qt::LeftButton) && _active)
    {
        double point[4];
        TYPoint pointRef;
        LPTYSiteNode pSite = ((TYSiteModelerFrame*)_pModeler)->getSite();
        TYPoint oldPos = pSite->getPosition();

        // Pt Ref
        displayToWorld(x, _pInteractor->height() - y, 0.0, point);
        pointRef.setFromOGL(point);
        pointRef._x *= -1;
        pointRef._y *= -1;
        pointRef._z = 0.0;

        TYAction* pAction = new TYSetPointRefSiteAction(pSite, pSite->getPosition(), pointRef, _pModeler, TR("id_action_setptref"));
        _pModeler->getActionManager()->addAction(pAction);

        pointRef._x += oldPos._x;
        pointRef._y += oldPos._y;

        pSite->setPosition(pointRef);
        pSite->updateGraphic();
        _pModeler->getView()->getRenderer()->updateDisplayList();
        _pModeler->updateView();

        // La scene a ete modifiee
        TYElement::setIsSavedOk(true);
    }
}
