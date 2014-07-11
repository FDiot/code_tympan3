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
 * \file TYBoxEditor.cpp
 * \brief gestion de l'edition d'une box
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/GraphicIHM/ToolsGraphic/OGLRectangleElement.h"

#include "Tympan/models/business/geoacoustic/TYAcousticBox.h"

#include "Tympan/GraphicIHM/DataManagerGraphic/TYPointGraphic.h"

#include "Tympan/models/business/geometry/TYRepere.h"
#include "Tympan/Tools/OLocalizator.h"

#include <qinputdialog.h>

#if defined(WIN32)
#include <crtdbg.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif


#define TR(id) OLocalizator::getString("TYBoxEditor", (id))

TYBoxEditor::TYBoxEditor(TYMachineModelerFrame* pModeler) :
    TYAbstractSceneEditor(pModeler)
{
    _pOGLRectangleElement = new OGLRectangleElement();
    _pOGLRectangleElement->setVisibility(false);
    OColor oLineColor;
    oLineColor.r = 1.0;
    oLineColor.g = 0.0;
    oLineColor.b = 0.0;
    _pOGLRectangleElement->setColor(oLineColor);
    _pOGLRectangleElement->setPoint0(OPoint3D(0.0, 0.0, 0.0));
    _pOGLRectangleElement->setPoint1(OPoint3D(0.0, 0.0, 0.0));
    _pOGLRectangleElement->setPoint2(OPoint3D(0.0, 0.0, 0.0));
    _pOGLRectangleElement->setPoint3(OPoint3D(0.0, 0.0, 0.0));
    _pInteractor->getRenderer()->addOGLElement(_pOGLRectangleElement);

    _moving = false;
    _active = false;
    slotViewTypeChanged(_pModeler->getCurrentView());
}

TYBoxEditor::~TYBoxEditor()
{
    _pInteractor->getRenderer()->removeOGLElement(_pOGLRectangleElement);
    delete _pOGLRectangleElement;
}

void TYBoxEditor::init()
{
}

void TYBoxEditor::cancel()
{
    _moving = false;
    _active = false;

    showText(false);

    _pOGLRectangleElement->setVisibility(false);
    _pInteractor->updateGL();
}

void TYBoxEditor::close()
{
    cancel();
}

void TYBoxEditor::slotKeyPressed(int key)
{
}

void TYBoxEditor::slotViewTypeChanged(int view)
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

void TYBoxEditor::slotMousePressed(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
{
    if ((button == Qt::LeftButton) && _active)
    {
        // Init points
        _pOGLRectangleElement->setPoint0(OPoint3D(x, _pInteractor->height() - y, 0.0));
        _pOGLRectangleElement->setPoint1(OPoint3D(x, _pInteractor->height() - y, 0.0));
        _pOGLRectangleElement->setPoint2(OPoint3D(x, _pInteractor->height() - y, 0.0));
        _pOGLRectangleElement->setPoint3(OPoint3D(x, _pInteractor->height() - y, 0.0));
        _moving = true;
    }
}

void TYBoxEditor::slotMouseReleased(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
{
    if ((button == Qt::LeftButton) && _active && _moving)
    {
        _moving = false;

        float pt0[3], pt1[3], pt2[3];
        NxVec3 point0 = OGLCamera::displayToWorld(NxVec3(_pOGLRectangleElement->getPoint0()[0], _pOGLRectangleElement->getPoint0()[1], _pOGLRectangleElement->getPoint0()[2]));
        NxVec3 point1 = OGLCamera::displayToWorld(NxVec3(_pOGLRectangleElement->getPoint1()[0], _pOGLRectangleElement->getPoint1()[1], _pOGLRectangleElement->getPoint1()[2]));
        NxVec3 point2 = OGLCamera::displayToWorld(NxVec3(_pOGLRectangleElement->getPoint2()[0], _pOGLRectangleElement->getPoint2()[1], _pOGLRectangleElement->getPoint2()[2]));
        pt0[0] = point0.x;
        pt0[1] = point0.z;
        pt1[0] = point1.x;
        pt1[1] = point1.z;
        pt2[0] = point2.x;
        pt2[1] = point2.z;

        double sizeX = ABS(pt1[0] - pt0[0]);
        double sizeY = ABS(pt2[1] - pt1[1]);
        double sizeZ = (sizeX + sizeY) / 2.0;

        // Si la grille magnetique est activee
        if (_pModeler->getSnapGridActive())
        {
            snapToGrid(sizeX, sizeY, sizeZ);
        }

        if ((sizeX != 0.0) && (sizeY != 0.0))
        {
            if (_pModeler->askForResetResultat())
            {
                // Nouvelle box
                LPTYAcousticBoxGeoNode pBoxGeoNode = new TYAcousticBoxGeoNode(new TYAcousticBox);
                TYAcousticBox::safeDownCast(pBoxGeoNode->getElement())->setDimension(sizeX, sizeY, sizeZ);

                // Position (on "pose" l'element au sol)
                TYPoint org((pt0[0] + (pt1[0] - pt0[0]) / 2.0), -(pt1[1] + (pt2[1] - pt1[1]) / 2.0), sizeZ / 2.0);

                // Si la grille magnetique est activee
                if (_pModeler->getSnapGridActive())
                {
                    // On ne modifie pas la hauteur
                    double dummy = 0.0;
                    snapToGrid(org._x, org._y, dummy);
                }

                ORepere3D repere = pBoxGeoNode->getORepere3D();

                repere._origin = org;

                pBoxGeoNode->setRepere(repere);

                // Ajout a la machine
                getMachine()->addAcousticVol((LPTYAcousticVolumeGeoNode&) pBoxGeoNode);
                getMachine()->updateGraphicTree();

                TYAction* pAction = new TYAddAccVolToAccVolNodeAction((LPTYAcousticVolumeGeoNode&) pBoxGeoNode, getMachine(), _pModeler, TR("id_action_addbox"));
                _pModeler->getActionManager()->addAction(pAction);

                /* Desactivation DT 20070903
                                //az++ temporairement,avant refonte mecanisme de selection
                                // On ajoute cette machine a la selection du calcul courant
                                if (getTYApp()->getCurProjet() && _pModeler->isElementInCurrentProjet())
                                {
                                    LPTYCalcul pCalcul = getTYApp()->getCurProjet()->getCurrentCalcul();
                                    if (pCalcul) {
                                        pCalcul->addToSelection(getMachine());
                                    }
                                }
                */
                // repasse en mode camera selection
                getTYMainWnd()->setDefaultCameraMode();

            }

            TYElement::setIsSavedOk(true);
        }

        showText(false);

        _pOGLRectangleElement->setVisibility(false);

        _pModeler->getView()->getRenderer()->updateDisplayList();
        _pModeler->updateView();
    }
}

void TYBoxEditor::slotMouseMoved(int x, int y, Qt::MouseButtons button, Qt::KeyboardModifiers state)
{
    if (_active && _moving)
    {
        _pOGLRectangleElement->setPoint1(OPoint3D(x, _pOGLRectangleElement->getPoint0()[1], 0.0));
        _pOGLRectangleElement->setPoint2(OPoint3D(x, _pInteractor->height() - y, 0.0));
        _pOGLRectangleElement->setPoint3(OPoint3D(_pOGLRectangleElement->getPoint0()[0], _pInteractor->height() - y, 0.0));

        float pt0[3], pt1[3], pt2[3];
        NxVec3 point0 = OGLCamera::displayToWorld(NxVec3(_pOGLRectangleElement->getPoint0()[0], _pOGLRectangleElement->getPoint0()[1], _pOGLRectangleElement->getPoint0()[2]));
        NxVec3 point1 = OGLCamera::displayToWorld(NxVec3(_pOGLRectangleElement->getPoint1()[0], _pOGLRectangleElement->getPoint1()[1], _pOGLRectangleElement->getPoint1()[2]));
        NxVec3 point2 = OGLCamera::displayToWorld(NxVec3(_pOGLRectangleElement->getPoint2()[0], _pOGLRectangleElement->getPoint2()[1], _pOGLRectangleElement->getPoint2()[2]));
        pt0[0] = point0.x;
        pt0[1] = point0.z;
        pt1[0] = point1.x;
        pt1[1] = point1.z;
        pt2[0] = point2.x;
        pt2[1] = point2.z;

        double sizeX = ABS(pt1[0] - pt0[0]);
        double sizeY = ABS(pt2[1] - pt1[1]);
        double sizeZ = (sizeX + sizeY) / 2.0;

        // Si la grille magnetique est activee
        if (_pModeler->getSnapGridActive())
        {
            snapToGrid(sizeX, sizeY, sizeZ);
        }

        updateText(QString(TR("id_size_info")).arg(sizeX, 0, 'f', 2).arg(sizeY, 0, 'f', 2),
                   (int)(_pOGLRectangleElement->getPoint0()[0] + (_pOGLRectangleElement->getPoint1()[0] - _pOGLRectangleElement->getPoint0()[0]) / 2.0),
                   (int)(_pOGLRectangleElement->getPoint1()[1] + (_pOGLRectangleElement->getPoint2()[1] - _pOGLRectangleElement->getPoint1()[1]) / 2.0));

        _pOGLRectangleElement->setVisibility(true);
        _pInteractor->updateGL();
    }
}

LPTYMachine TYBoxEditor::getMachine()
{
    Q_CHECK_PTR(_pModeler);

    LPTYMachine pMachine = ((TYMachineModelerFrame*) _pModeler)->getMachine();
    Q_CHECK_PTR(pMachine);

    return pMachine;
}
