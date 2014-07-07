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
 * \file TYRectangleEditor.cpp
 * \brief gestion de l'edition d'un rectangle
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/GraphicIHM/ToolsGraphic/OGLRectangleElement.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticRectangle.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYBoucheSurface.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYChemineeSurface.h"

#include "Tympan/GraphicIHM/DataManagerGraphic/TYPointGraphic.h"

#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYRepere.h"
#include "Tympan/Tools/OLocalizator.h"

#include "Tympan/core/exceptions.h"

#include <iostream>
#include <qmessagebox.h>

#if defined(WIN32)
#include <crtdbg.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif


#define TR(id) OLocalizator::getString("TYRectangleEditor", (id))


TYRectangleEditor::TYRectangleEditor(TYFaceModelerFrame* pModeler, int mode /*=Bouche*/) :
    TYAbstractSceneEditor(pModeler)
{
    setMode(mode);
    _pRectangle = NULL;

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

TYRectangleEditor::~TYRectangleEditor()
{
    _pInteractor->getRenderer()->removeOGLElement(_pOGLRectangleElement);
    delete _pOGLRectangleElement;
}

void TYRectangleEditor::init()
{
}

void TYRectangleEditor::cancel()
{
    _moving = false;
    _active = false;

    showText(false);

    _pOGLRectangleElement->setVisibility(false);
    _pInteractor->updateGL();
}

void TYRectangleEditor::close()
{
    cancel();
}

void TYRectangleEditor::slotKeyPressed(int key)
{
}

void TYRectangleEditor::slotViewTypeChanged(int view)
{
    if (view == TYModelerFrame::TopView)
    {
        _active = true;
    }
    else
    {
        _active = false;
    }
    _active = true;
}

void TYRectangleEditor::slotMousePressed(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
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

void TYRectangleEditor::slotMouseReleased(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
{
    if ((button == Qt::LeftButton) && _active && _moving)
    {
        if (_pModeler->askForResetResultat())
        {
            _moving = false;

            TYPoint pt0, pt1, pt2;
            NxVec3 point0 = OGLCamera::displayToWorld(NxVec3(_pOGLRectangleElement->getPoint0()[0], _pOGLRectangleElement->getPoint0()[1], _pOGLRectangleElement->getPoint0()[2]));
            NxVec3 point1 = OGLCamera::displayToWorld(NxVec3(_pOGLRectangleElement->getPoint1()[0], _pOGLRectangleElement->getPoint1()[1], _pOGLRectangleElement->getPoint1()[2]));
            NxVec3 point2 = OGLCamera::displayToWorld(NxVec3(_pOGLRectangleElement->getPoint2()[0], _pOGLRectangleElement->getPoint2()[1], _pOGLRectangleElement->getPoint2()[2]));
            pt0._x = point0.x;
            pt0._y = -point0.z;
            pt0._z  = point0.y;
            pt1._x = point1.x;
            pt1._y = -point1.z;
            pt1._z  = point1.y;
            pt2._x = point2.x;
            pt2._y = -point2.z;
            pt2._z  = point2.y;

            double sizeX = OVector3D(pt0, pt1).norme();
            double sizeY = OVector3D(pt1, pt2).norme();
            double sizeZ = 0;

            // Si la grille magnetique est activee
            if (_pModeler->getSnapGridActive())
            {
                snapToGrid(sizeX, sizeY, sizeZ);
            }

            if (sizeX <= 0.0001 || sizeY <= 0.0001) { return; } // Securite : eviter les surfaces nulles.

            // Position
            TYPoint org = OVector3D(OVector3D(pt0) + (OVector3D(pt2) - OVector3D(pt0)) * 0.5);
            org._z = 0.0;

            // Si la grille magnetique est activee
            if (_pModeler->getSnapGridActive())
            {
                snapToGrid(org._x, org._y, org._z);
            }

            TYRepere rep;
            rep._origin  = org;

            // Necessaire pr Prototype Factory...
            {
                TYBoucheSurface dummyBouche;
                TYChemineeSurface dummyCheminee;
            }

            // Nouvelle sous-face
            const char* childType = NULL;
            switch (_mode)
            {
                case Bouche:    childType = "TYBoucheSurface";  break;
                case Cheminee:  childType = "TYChemineeSurface";    break;
                case Fenetre:   childType = "TYMurElement"; break;
                default:    childType = "";
            }

            // Instanciation du type choisi
            _pRectangle = NULL;
            try
            {
                _pRectangle = dynamic_cast<TYAcousticRectangle*>(TYElement::findAndClone(childType));
            }
            catch(tympan::invalid_data& exc) {};

            if (_pRectangle != nullptr)
            {
                _pRectangle->getBoundingRect()->setDimension(sizeX, sizeY);

                // Ajout a l'AcousticRectangleNode
                bool ret = getAcousticRectangleNode()->addSubRect(_pRectangle, rep);

                if (ret)
                {
                    // Update
                    getAcousticRectangleNode()->updateGraphicTree();

                    TYAction* pAction = new TYAddAccSurfToAccSurfNodeAction(
                            (LPTYAcousticSurface&) _pRectangle,
                            getAcousticRectangleNode(), _pModeler,
                            TR("id_action_remface"));
                    _pModeler->getActionManager()->addAction(pAction);
                }

                TYElement::setIsSavedOk(true);
            }
        }

        showText(false);

        _pOGLRectangleElement->setVisibility(false);
        _pModeler->getView()->getRenderer()->updateDisplayList();
        _pInteractor->updateGL();
    }
}

void TYRectangleEditor::slotMouseMoved(int x, int y, Qt::MouseButtons button, Qt::KeyboardModifiers state)
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
        pt0[2]  = -point0.y;
        pt1[0] = point1.x;
        pt1[1] = point1.z;
        pt1[2]  = -point1.y;
        pt2[0] = point2.x;
        pt2[1] = point2.z;
        pt2[2]  = -point2.y;

        double sizeX = ABS(pt1[0] - pt0[0]);
        double sizeY = ABS(pt2[1] - pt0[1]);
        double sizeZ = ABS(pt2[2] - pt0[2]);

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

LPTYAcousticRectangleNode TYRectangleEditor::getAcousticRectangleNode()
{
    Q_CHECK_PTR(_pModeler);

    LPTYAcousticRectangleNode pAcousticRectangleNode = ((TYFaceModelerFrame*) _pModeler)->getFace();
    Q_CHECK_PTR(pAcousticRectangleNode);

    return pAcousticRectangleNode;
}
