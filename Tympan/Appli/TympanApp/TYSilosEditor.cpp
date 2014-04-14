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
 * \file TYSilosEditor.cpp
 * \brief gestion de l'edition d'un silo
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/GraphicIHM/ToolsGraphic/OGLRectangleElement.h"

#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticCylinder.h"

#include "Tympan/Tools/OMessageManager.h"
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


#define TR(id) OLocalizator::getString("TYSilosEditor", (id))


TYSilosEditor::TYSilosEditor(TYModelerFrame* pModeler) :
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

TYSilosEditor::~TYSilosEditor()
{
    _pInteractor->getRenderer()->removeOGLElement(_pOGLRectangleElement);
    delete _pOGLRectangleElement;
}

void TYSilosEditor::init()
{
}

void TYSilosEditor::cancel()
{
    _moving = false;
    _active = false;

    showText(false);

    _pOGLRectangleElement->setVisibility(false);
    _pInteractor->updateGL();
}

void TYSilosEditor::close()
{
    cancel();
}

void TYSilosEditor::slotKeyPressed(int key)
{
}

void TYSilosEditor::slotViewTypeChanged(int view)
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

void TYSilosEditor::slotMousePressed(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
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

void TYSilosEditor::slotMouseReleased(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
{
    if ((button == Qt::LeftButton) && _active && _moving)
    {
        _moving = false;

        float pt0[2], pt1[2], pt2[2];
        NxVec3 point0 = OGLCamera::displayToWorld(NxVec3(_pOGLRectangleElement->getPoint0()[0], _pOGLRectangleElement->getPoint0()[1], _pOGLRectangleElement->getPoint0()[2]));
        NxVec3 point1 = OGLCamera::displayToWorld(NxVec3(_pOGLRectangleElement->getPoint1()[0], _pOGLRectangleElement->getPoint1()[1], _pOGLRectangleElement->getPoint1()[2]));
        NxVec3 point2 = OGLCamera::displayToWorld(NxVec3(_pOGLRectangleElement->getPoint2()[0], _pOGLRectangleElement->getPoint2()[1], _pOGLRectangleElement->getPoint2()[2]));
        pt0[0] = point0.x;
        pt0[1] = point0.z;
        pt1[0] = point1.x;
        pt1[1] = point1.z;
        pt2[0] = point2.x;
        pt2[1] = point2.z;

        double diameter = ABS(pt1[0] - pt0[0]);

        // Si la grille magnetique est activee
        if (_pModeler->getSnapGridActive())
        {
            double dummy;
            snapToGrid(diameter, dummy, dummy);
        }

        if (diameter == 0.0)
        {
            return;
        }
        else if (diameter != 0.0)
        {
            TYAction* pAction = NULL;
            // 2 cas :
            //      - le modeleur est un modeleur de site
            //      - le modeleur est un modeleur de batiment
            if (QString(_pModeler->metaObject()->className()).compare("TYBatimentModelerFrame") == 0)
            {
                // Nouveau cylindre
                LPTYAcousticCylinderGeoNode pCylGeoNode = new TYAcousticCylinderGeoNode(new TYAcousticCylinder);
                LPTYAcousticCylinder pCyl = TYAcousticCylinder::safeDownCast(pCylGeoNode->getElement());
                pCyl->setDiameter(diameter);
                pCyl->setHauteur(2.0);

                // Position
                TYPoint org((pt0[0] + (pt1[0] - pt0[0]) / 2.0),
                            -(pt1[1] + (pt2[1] - pt1[1]) / 2.0),
                            pCyl->getHauteur() / 2.0);

                // Si la grille magnetique est activee
                if (_pModeler->getSnapGridActive())
                {
                    snapToGrid(org._x, org._y, org._z);
                }

                ORepere3D repere = pCylGeoNode->getRepere();

                repere._origin = org;

                pCylGeoNode->setRepere(repere);

                // Ajout au batiment
                LPTYBatiment pBat = getBatiment();
                if (pBat != NULL)
                {
                    pBat->addAcousticVol((LPTYAcousticVolumeGeoNode&) pCylGeoNode);
                    pBat->updateGraphicTree();
                }

                TYAction* pAction = new TYAddAccVolToAccVolNodeAction((LPTYAcousticVolumeGeoNode&) pCylGeoNode, getBatiment(), _pModeler, TR("id_action_addcyl"));
            }
            else if (QString(_pModeler->metaObject()->className()).compare("TYSiteModelerFrame") == 0)
            {
                // Creation du cylindre
                LPTYAcousticCylinderGeoNode pCylGeoNode = new TYAcousticCylinderGeoNode(new TYAcousticCylinder);
                LPTYAcousticCylinder pCyl = TYAcousticCylinder::safeDownCast(pCylGeoNode->getElement());
                pCyl->setDiameter(diameter);
                pCyl->setHauteur(2.0);

                // Creation d'un nouveau batiment a partir du silos
                LPTYBatimentGeoNode pBatNode = new TYBatimentGeoNode(new TYBatiment());

                // Ajout du cylindre au batiment
                TYBatiment::safeDownCast(pBatNode->getElement())->addAcousticVol(pCylGeoNode, false);

                // Gestion de la position de l'objet
                // Position
                TYPoint org((pt0[0] + (pt1[0] - pt0[0]) / 2.0),
                            -(pt1[1] + (pt2[1] - pt1[1]) / 2.0),
                            pCyl->getHauteur() / 2.0);

                // Si la grille magnetique est activee
                if (_pModeler->getSnapGridActive())
                {
                    snapToGrid(org._x, org._y, org._z);
                }

                ORepere3D repere = pBatNode->getRepere();

                repere._origin = org;

                pBatNode->setRepere(repere);

                // Ajout dans l'infrastructure
                LPTYSiteNode pSite = getSite();
                if (pSite != NULL)
                {
                    // Ajout du atiment dans l'infrastructure
                    pSite->getInfrastructure()->addBatiment(pBatNode);

                    pSite->updateGraphicTree();
                }

                TYAction* pAction = new TYAddElementToInfraAction(pBatNode, pSite->getInfrastructure(), _pModeler, TR("id_action_addetage"));

            }

            if (pAction != NULL) { _pModeler->getActionManager()->addAction(pAction); }
            TYElement::setIsSavedOk(true);

            // repasse en mode camera selection
            getTYMainWnd()->setDefaultCameraMode();

        }

        showText(false);

        _pOGLRectangleElement->setVisibility(false);
        refreshSiteFrame();

        _pModeler->getView()->getRenderer()->updateDisplayList();

        _pModeler->updateView();
    }
}


void TYSilosEditor::slotMouseMoved(int x, int y, Qt::MouseButtons button, Qt::KeyboardModifiers state)
{
    if (_active && _moving)
    {
        float w;

        w = ABS(x - _pOGLRectangleElement->getPoint0()[0]);

        _pOGLRectangleElement->setPoint1(OPoint3D(x, _pOGLRectangleElement->getPoint0()[1], 0.0));
        _pOGLRectangleElement->setPoint2(OPoint3D(x, _pOGLRectangleElement->getPoint0()[1] - w, 0.0));
        _pOGLRectangleElement->setPoint3(OPoint3D(_pOGLRectangleElement->getPoint0()[0], _pOGLRectangleElement->getPoint0()[1] - w, 0.0));

        float pt0[2], pt1[2], pt2[2];
        NxVec3 point0 = OGLCamera::displayToWorld(NxVec3(_pOGLRectangleElement->getPoint0()[0], _pOGLRectangleElement->getPoint0()[1], _pOGLRectangleElement->getPoint0()[2]));
        NxVec3 point1 = OGLCamera::displayToWorld(NxVec3(_pOGLRectangleElement->getPoint1()[0], _pOGLRectangleElement->getPoint1()[1], _pOGLRectangleElement->getPoint1()[2]));
        NxVec3 point2 = OGLCamera::displayToWorld(NxVec3(_pOGLRectangleElement->getPoint2()[0], _pOGLRectangleElement->getPoint2()[1], _pOGLRectangleElement->getPoint2()[2]));
        pt0[0] = point0.x;
        pt0[1] = point0.z;
        pt1[0] = point1.x;
        pt1[1] = point1.z;
        pt2[0] = point2.x;
        pt2[1] = point2.z;

        double diameter = ABS(pt1[0] - pt0[0]);

        // Si la grille magnetique est activee
        if (_pModeler->getSnapGridActive())
        {
            double dummy;
            snapToGrid(diameter, dummy, dummy);
        }

        updateText(QString(TR("id_diameter_info")).arg(diameter, 0, 'f', 2),
                   (int)(_pOGLRectangleElement->getPoint0()[0] + (_pOGLRectangleElement->getPoint1()[0] - _pOGLRectangleElement->getPoint0()[0]) / 2.0),
                   (int)(_pOGLRectangleElement->getPoint1()[1] + (_pOGLRectangleElement->getPoint2()[1] - _pOGLRectangleElement->getPoint1()[1]) / 2.0));

        _pOGLRectangleElement->setVisibility(true);
        _pInteractor->updateGL();
    }
}

LPTYBatiment TYSilosEditor::getBatiment()
{
    Q_CHECK_PTR(_pModeler);
    LPTYBatiment pBatiment = NULL;

    if (QString(_pModeler->metaObject()->className()).compare("TYBatimentModelerFrame") == 0)
    {
        pBatiment = ((TYBatimentModelerFrame*) _pModeler)->getBatiment();
    }

    Q_CHECK_PTR(pBatiment);

    return pBatiment;
}

LPTYSiteNode TYSilosEditor::getSite()
{
    Q_CHECK_PTR(_pModeler);

    LPTYSiteNode pSite = NULL;

    if (QString(_pModeler->metaObject()->className()).compare("TYSiteModelerFrame") == 0)
    {
        pSite = ((TYSiteModelerFrame*) _pModeler)->getSite();
    }

    Q_CHECK_PTR(pSite);

    return pSite;
}
