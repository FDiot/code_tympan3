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
 * \file TYPolyLineEditor.cpp
 * \brief gestion de l'edition d'une polyligne
 */


#include <qcursor.h>
#include <QList>

#include "Tympan/models/common/3d.h"
#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/geometry/TYPoint.h"
#include "Tympan/gui/tools/OGLLineElement.h"
#include "Tympan/gui/tools/OGLSphereElement.h"
#include "Tympan/gui/app/TYCameraEditor.h"
#include "Tympan/gui/app/TYModelerFrame.h"
#include "Tympan/gui/app/TYRenderWindowInteractor.h"
#include "TYPolyLineEditor.h"


#define TR(id) OLocalizator::getString("TYPolyLineEditor", (id))


TYPolyLineEditor::TYPolyLineEditor(TYModelerFrame* pModeler) :
    TYAbstractSceneEditor(pModeler)
{
    _pCamEditor = new TYCameraEditor(pModeler);

    _pCamEditor->setLeftButtonFunction2D(SLOT(doNothing()));
    _pCamEditor->setMiddleButtonFunction2D(SLOT(cameraTranslate()));
    _pCamEditor->setRightButtonFunction2D(SLOT(doNothing()));
    _pCamEditor->setShiftLeftButtonFunction2D(SLOT(cameraTranslate()));
    _pCamEditor->setShiftRightButtonFunction2D(SLOT(cameraZoom()));

    _shiftOn = false;
    _active = false;
    _useAngleStep = false;
    _dispDist = false;

    // Taille des points differente selon le type de modeler
    if (QString(pModeler->metaObject()->className()).compare("TYSiteModelerFrame") == 0)
    {
        _pointSize = 1.0f;
    }
    else if (QString(pModeler->metaObject()->className()).compare("TYBatimentModelerFrame") == 0)
    {
        _pointSize = 0.2f;
    }
    else if (QString(pModeler->metaObject()->className()).compare("TYMachineModelerFrame") == 0)
    {
        _pointSize = 0.1f;
    }
    else
    {
        _pointSize = 0.5f;
    }

    _pOGLLineElement = NULL;
}

TYPolyLineEditor::~TYPolyLineEditor()
{
    cancel();

    delete _pCamEditor;

    if (_pOGLLineElement)
    {
        _pInteractor->getRenderer()->removeOGLElement(_pOGLLineElement);
        delete _pOGLLineElement;
        _pOGLLineElement = NULL;
    }

    if (_OGLSphereElementList.count() > 0)
    {
        for (QList<OGLSphereElement*>::iterator ite = _OGLSphereElementList.begin(); ite != _OGLSphereElementList.end(); ite++)
        {
            _pInteractor->getRenderer()->removeOGLElement(*ite);
            delete *ite;
        }
        _OGLSphereElementList.clear();
    }

    if (_OGLLineElementList.count() > 0)
    {
        for (QList<OGLLineElement*>::iterator ite = _OGLLineElementList.begin(); ite != _OGLLineElementList.end(); ite++)
        {
            _pInteractor->getRenderer()->removeOGLElement(*ite);
            delete *ite;
        }
        _OGLLineElementList.clear();
    }
}

void TYPolyLineEditor::connect()
{
    TYAbstractSceneEditor::connect();
}

void TYPolyLineEditor::disconnect()
{
    TYAbstractSceneEditor::disconnect();
    _pCamEditor->disconnect();
}

void TYPolyLineEditor::init()
{
    _savedPoints.clear();
    if (_OGLSphereElementList.count() > 0)
    {
        for (QList<OGLSphereElement*>::iterator ite = _OGLSphereElementList.begin(); ite != _OGLSphereElementList.end(); ite++)
        {
            _pInteractor->getRenderer()->removeOGLElement(*ite);
            delete *ite;
        }
        _OGLSphereElementList.clear();
    }

    if (_OGLLineElementList.count() > 0)
    {
        for (QList<OGLLineElement*>::iterator ite = _OGLLineElementList.begin(); ite != _OGLLineElementList.end(); ite++)
        {
            _pInteractor->getRenderer()->removeOGLElement(*ite);
            delete *ite;
        }
        _OGLLineElementList.clear();
    }

    updateMovingLine(0, 0);
    showText(false);

    _pInteractor->updateGL();
}

void TYPolyLineEditor::cancel()
{
    init();
}

void TYPolyLineEditor::close()
{
    _savedPoints.clear();
}

void TYPolyLineEditor::slotKeyPressed(int key)
{
    switch (key)
    {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            if (_active)
            {
                int nbpoints = static_cast<int>(_savedPoints.size());

                if (nbpoints >= 3)
                {
                    OVector3D normale;
                    OPoint3D* pts = new OPoint3D[nbpoints];

                    for (unsigned int i = 0; i < nbpoints; i++)
                    {
                        pts[i] = _savedPoints[i];
                    }

                    OGeometrie::computeNormal(pts, nbpoints, normale);

                    OVector3D viewnormale;
                    OGLCamera* camera = _pInteractor->getRenderer()->getActiveCamera();
                    if (!camera->fromTo.sameDirection(NxVec3(normale._x, normale._y, normale._z)))
                    {
                        _savedPoints.clear();

                        for (long i = (nbpoints - 1); i >= 0; i--)
                        {
                            _savedPoints.push_back(pts[i]);
                        }
                    }

                    delete [] pts;

                }

                emit(endedSavingPoints());

                init();

                TYElement::setIsSavedOk(true);

            }
            break;
        case Qt::Key_Shift:
            if (_pModeler->getCurrentView() == TYModelerFrame::TopView)
            {
                _pCamEditor->connect();
            }
            _shiftOn = true;
            break;
        case Qt::Key_Escape:
            if (_active)
            {
                cancel();
            }
            break;
        case Qt::Key_Delete:
            if (_active)
            {
                removeLastPoint();
            }
            break;
    }
}

void TYPolyLineEditor::slotKeyReleased(int key)
{
    if (key == Qt::Key_Shift)
    {
        if (_pModeler->getCurrentView() == TYModelerFrame::TopView)
        {
            _pCamEditor->disconnect();
        }
        _shiftOn = false;
    }
}

void TYPolyLineEditor::slotViewTypeChanged(int view)
{
    if (view == TYModelerFrame::TopView)
    {
        _pCamEditor->disconnect();
        _active = true;
    }
    else
    {
        _pCamEditor->connect();
        _active = false;

        showText(false);
        _pInteractor->updateGL();
    }

    // On relaie le signal au CameraEditor
    _pCamEditor->slotViewTypeChanged(view);
}

void TYPolyLineEditor::slotMouseMoved(int x, int y, Qt::MouseButtons button, Qt::KeyboardModifiers state)
{
    updateMovingLine(x, y);
}

void TYPolyLineEditor::slotMouseReleased(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
{
    if (_active && !_shiftOn)
    {
        if (button == Qt::LeftButton)
        {
            addPoint(x, y);
        }
        else if ((button == Qt::RightButton) && (_savedPoints.size() > 0))
        {
            removeLastPoint();
        }
    }
}

void TYPolyLineEditor::addPoint(int x, int y)
{
    NxVec3 pt = OGLCamera::displayToWorld(NxVec3(x, _pInteractor->height() - y, 0.1));
    TYPoint pt2(pt.x, -pt.z, pt.y);
    validatePoint(pt2);

    // Changement de repere Disp->World
    if (_savedPoints.size() >= 1)
    {
        OGLLineElement* _pOGLLineElement = new OGLLineElement();
        _pOGLLineElement->setVisibility(true);
        OColor oLineColor;
        oLineColor.r = 1.0;
        oLineColor.g = 0.0;
        oLineColor.b = 0.0;
        _pOGLLineElement->setColor(oLineColor);
        _pOGLLineElement->setIs3D(true);
        _pOGLLineElement->setPoint1(_OGLSphereElementList.last()->getCenter());
        _pOGLLineElement->setPoint2(OPoint3D(pt2._x, pt2._z, -pt2._y));
        _pInteractor->getRenderer()->addOGLElement(_pOGLLineElement);
        _OGLLineElementList.append(_pOGLLineElement);
    }

    // Ajout du nouveau point a la collection
    _savedPoints.push_back(pt2);

    OGLSphereElement* _pOGLSphereElement = new OGLSphereElement();
    _pOGLSphereElement->setCenter(OPoint3D(pt2._x,  pt2._z, -pt2._y));
    _pOGLSphereElement->setRadius(_pointSize);
    _pOGLSphereElement->setIs3D(true);
    _pOGLSphereElement->setColor(OColor(1.0, 0.0, 0.0));
    _pOGLSphereElement->setVisibility(true);
    _pInteractor->getRenderer()->addOGLElement(_pOGLSphereElement);
    _OGLSphereElementList.append(_pOGLSphereElement);

    _pInteractor->updateGL();
}

bool TYPolyLineEditor::removeLastPoint()
{
    bool res = true;

    // Suppression du point precedent
    _savedPoints.pop_back();

    if (!_savedPoints.size())
    {
        res = false;
    }

    if (_OGLSphereElementList.count() == 0)
    {
        return false;
    }
    else
    {
        _pInteractor->getRenderer()->removeOGLElement(_OGLSphereElementList.last());
        delete _OGLSphereElementList.last();
        _OGLSphereElementList.removeLast();

        if (_OGLLineElementList.count() == 0)
        {
            return false;
        }
        else
        {
            _pInteractor->getRenderer()->removeOGLElement(_OGLLineElementList.last());
            delete _OGLLineElementList.last();
            _OGLLineElementList.removeLast();
            if (!updateMovingLine(0, 0))
            {
                _pInteractor->updateGL();
            }
        }
    }

    return res;
}

bool TYPolyLineEditor::updateMovingLine(int x, int y)
{
    if (!_active)
    {
        return false;
    }
    if (_OGLSphereElementList.count() == 0)
    {
        if (_pOGLLineElement)
        {
            _pOGLLineElement->setVisibility(false);
        }

        showText(false);

        return false;
    }

    QPoint pos = _pInteractor->mapFromGlobal(QCursor::pos());


    // Changement de repere Disp->World
    // On recupere le pt precedent
    TYPoint pt1 = _savedPoints.back();

    NxVec3 pt;
    if ((x == 0) && (y == 0))
    {
        pt = OGLCamera::displayToWorld(NxVec3(pos.x(), _pInteractor->height() - pos.y(), 0.1));
    }
    else
    {
        pt = OGLCamera::displayToWorld(NxVec3(x, _pInteractor->height() - y, 0.1));
    }
    TYPoint pt2(pt.x, -pt.z, pt.y);
    validatePoint(pt2);

    // Init des objets graphiques
    if (!_pOGLLineElement)
    {
        _pOGLLineElement = new OGLLineElement();
        _pOGLLineElement->setVisibility(false);
        _pOGLLineElement->setIs3D(true);
        OColor oLineColor;
        oLineColor.r = 1.0;
        oLineColor.g = 0.0;
        oLineColor.b = 0.0;
        _pOGLLineElement->setColor(oLineColor);

        _pInteractor->getRenderer()->addOGLElement(_pOGLLineElement);
    }

    // On trace une ligne avec les 2 pts
    _pOGLLineElement->setVisibility(true);
    _pOGLLineElement->setPoint1(OPoint3D(pt1._x, pt1._z, -pt1._y));
    _pOGLLineElement->setPoint2(OPoint3D(pt2._x,  pt2._z, -pt2._y));


    if (_dispDist)
    {
        // Affichage de la distance entre les 2 points
        double dist = OVector3D(pt1, pt2).norme();
        QString msg = QString(TR("id_output_dist")).arg(dist, 0, 'f', 2);

        double textPos[4];
        worldToDisplay((_pOGLLineElement->getPoint1()[0] + (_pOGLLineElement->getPoint2()[0] - _pOGLLineElement->getPoint1()[0]) / 2.0),
                       (_pOGLLineElement->getPoint2()[1] + (_pOGLLineElement->getPoint1()[1] - _pOGLLineElement->getPoint2()[1]) / 2.0),
                       (_pOGLLineElement->getPoint2()[2] + (_pOGLLineElement->getPoint1()[2] - _pOGLLineElement->getPoint2()[2]) / 2.0),
                       textPos);
        updateText(msg, (int)textPos[0], (int)textPos[1]);
    }

    // Redraw
    _pInteractor->updateGL();

    return true;
}

void TYPolyLineEditor::validatePoint(TYPoint& pt)
{
    // Si la grille magnetique est activee
    if (_pModeler->getSnapGridActive())
    {
        if (_useAngleStep && (_savedPoints.size() >= 1))
        {
            // On recupere le pt precedent
            TYPoint pt1 = _savedPoints.back();

            // Angle step par defaut
            float angleStep = 15.0f;

            // Chargement de l'angle defini dans les preferences
            //          static const char prefName[] = "Angle";
            if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "Angle"))
            {
                angleStep = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, "Angle");
            }
            else
            {
                TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, "Angle", angleStep);
            }

            // Angle en radians
            float angleStepRad = DEGTORAD(angleStep);

            // Calcul de l'angle
            OVector3D vec(pt1, pt);
            double angle = SIGNE(vec._y) * acos(vec._x / vec.norme());

            // Angle "rond"
            angle = ROUND(angle / angleStepRad) * angleStepRad;

            // Nouvelle coord du point courant
            pt._x = (vec.norme() * cos(angle)) + pt1._x;
            pt._y = (vec.norme() * sin(angle)) + pt1._y;
        }

        snapToGrid(pt._x, pt._y, pt._z);
    }
}
