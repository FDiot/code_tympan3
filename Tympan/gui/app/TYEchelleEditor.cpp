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
 * \file TYEchelleEditor.cpp
 * \brief gestion de l'edition de l'echelle
 *
 *
 */



#include "Tympan/gui/tools/OGLLineElement.h"
#include "Tympan/models/business/OLocalizator.h"

#include <qinputdialog.h>


#if defined(WIN32)
#include <crtdbg.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif

#define TR(id) OLocalizator::getString("TYEchelleEditor", (id))

TYEchelleEditor::TYEchelleEditor(TYSiteModelerFrame* pModeler) :
    TYAbstractSceneEditor(pModeler)
{
    _active = false;

    _pOGLLineElement = new OGLLineElement();
    _pOGLLineElement->setVisibility(false);
    OColor oLineColor;
    oLineColor.r = 1.0;
    oLineColor.g = 0.0;
    oLineColor.b = 0.0;
    _pOGLLineElement->setColor(oLineColor);
    _pInteractor->getRenderer()->addOGLElement(_pOGLLineElement);

    slotViewTypeChanged(_pModeler->getCurrentView());
}

TYEchelleEditor::~TYEchelleEditor()
{
    _pInteractor->getRenderer()->removeOGLElement(_pOGLLineElement);
    delete _pOGLLineElement;
}

void TYEchelleEditor::init()
{
    // Init seg a 0
    _pOGLLineElement->setPoint1(OPoint3D(0.0, 0.0, 0.0));
    _pOGLLineElement->setPoint2(OPoint3D(0.0, 0.0, 0.0));
}

void TYEchelleEditor::close()
{
    _pOGLLineElement->setVisibility(false);
    _pInteractor->updateGL();
}

void TYEchelleEditor::slotViewTypeChanged(int view)
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

void TYEchelleEditor::slotMousePressed(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
{
    if ((button == Qt::LeftButton) && _active)
    {
        _pOGLLineElement->setPoint1(OPoint3D(x, _pInteractor->height() - y, 0.0));
        _pOGLLineElement->setPoint2(OPoint3D(x, _pInteractor->height() - y, 0.0));
    }
}

void TYEchelleEditor::slotMouseMoved(int x, int y, Qt::MouseButtons button, Qt::KeyboardModifiers state)
{
    if ((button == Qt::LeftButton) && _active)
    {
        _pOGLLineElement->setPoint2(OPoint3D(x, _pInteractor->height() - y, 0.0));
        _pOGLLineElement->setVisibility(true);
        _pInteractor->updateGL();
    }
}

void TYEchelleEditor::slotMouseReleased(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
{
    if ((button == Qt::LeftButton) && _active)
    {
        double point[4];
        TYSegment segEchelle;
        LPTYSiteNode pSite = ((TYSiteModelerFrame*)_pModeler)->getSite();

        // Pt 1
        displayToWorld(_pOGLLineElement->getPoint1()[0],
                       _pOGLLineElement->getPoint1()[1],
                       _pOGLLineElement->getPoint1()[2],
                       point);
        segEchelle._ptA.setFromOGL(point[0], point[1], point[2]);

        // Pt 2
        displayToWorld(_pOGLLineElement->getPoint2()[0],
                       _pOGLLineElement->getPoint2()[1],
                       _pOGLLineElement->getPoint2()[2],
                       point);
        segEchelle._ptB.setFromOGL(point[0], point[1], point[2]);

        // Longueur actuelle en coord World
        double size = segEchelle.longueur();
        // Longueur en prenant compte l'ancienne echelle
        size /= pSite->getEchelle();

        // Obtention de la longeur reelle World
        bool ok = false;
        double distance = QInputDialog::getDouble(_pModeler, "Tympan", "Distance", 0, 0, 2000000, 2, &ok);

        if (ok && size)
        {
            // Calcul de la nouvelle echelle
            float echelle = distance / size;

            TYAction* pAction = new TYSetEchelleSiteAction(pSite, pSite->getEchelle(), echelle, _pModeler, TR("id_action_setechelle"));
            _pModeler->getActionManager()->addAction(pAction);

            pSite->setEchelle(echelle);
            pSite->updateGraphic();
        }

        _pOGLLineElement->setVisibility(false);

        _pModeler->getView()->getRenderer()->updateDisplayList();

        _pModeler->updateView();

        // La scene a ete modifiee
        TYElement::setIsSavedOk(true);
    }
}
