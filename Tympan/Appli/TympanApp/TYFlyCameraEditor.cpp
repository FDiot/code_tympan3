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
 * \file TYFlyCameraEditor.cpp
 * \brief Indique que la position ou l'orientation de la camera a ete modifiee
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include <QtOpenGL>

#include <qtimer.h>
#include <qcursor.h>

#include "Tympan/MetierSolver/DataManagerCore/TYDefines.h"
#include "Tympan/models/common/3d.h"

using namespace Qt;

TYFlyCameraEditor::TYFlyCameraEditor(TYModelerFrame* pModeler)
    : TYAbstractSceneEditor(pModeler)
{
    _firstCall = true;

    setLeftButtonFunction(SLOT(doNothing()));
    setMiddleButtonFunction(SLOT(doNothing()));
    setRightButtonFunction(SLOT(doNothing()));
    setShiftLeftButtonFunction(SLOT(doNothing()));
    setShiftRightButtonFunction(SLOT(doNothing()));

    setLeftButtonFunction3D(SLOT(cameraRotate()));
    setMiddleButtonFunction3D(SLOT(doNothing()));
    setRightButtonFunction3D(SLOT(doNothing()));
    setShiftLeftButtonFunction3D(SLOT(doNothing()));
    setShiftRightButtonFunction3D(SLOT(doNothing()));

    _mouseEventActive = false;
    _keyEventActive = 0;
    setInteractionTime(10);
    setSensitivity(20.0);

    _bKeyUp = false;
    _bKeyDown = false;
    _bKeyLeft = false;
    _bKeyRight = false;
    _bKeyPageUp = false;
    _bKeyPageDown = false;

    _pStepTimer = new QTimer(this);
    Q_CHECK_PTR(_pStepTimer);

    _pCurrentCamera = _pInteractor->getRenderer()->getActiveCamera();
    Q_CHECK_PTR(_pCurrentCamera);

    _zoomStep = 2.0f;
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "CameraZoomStep"))
    {
        _zoomStep = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, "CameraZoomStep");
    }
    else
    {
        TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, "CameraZoomStep", _zoomStep);
    }

    _translateStep = 5.0f;
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "CameraTranslateStep"))
    {
        _translateStep = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, "CameraTranslateStep");
    }
    else
    {
        TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, "CameraTranslateStep", _translateStep);
    }

    _rotateStep = 10.0f;
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "CameraRotateStep"))
    {
        _rotateStep = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, "CameraRotateStep");
    }
    else
    {
        TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, "CameraRotateStep", _rotateStep);
    }


    // Pour la mise a jour de l'orientation et la position des axes et de la grille
    QObject::connect(this, SIGNAL(cameraUpdated()), pModeler, SLOT(updateAxes()));
    QObject::connect(this, SIGNAL(cameraUpdated()), pModeler, SLOT(updateGrid()));
    // Pour le passage en mode wireframe lors du deplacement de la cam si l'option est active
    QObject::connect(this, SIGNAL(startMovingCamera()), pModeler, SLOT(startMovingRenderMode()));
    QObject::connect(this, SIGNAL(stopMovingCamera()), pModeler, SLOT(stopMovingRenderMode()));
    // L'echelle est mise a jour a la fin de chaque deplacement de camera
    QObject::connect(this, SIGNAL(stopMovingCamera()), pModeler, SLOT(updateScale()));

    setToNavigation3D();
}

TYFlyCameraEditor::~TYFlyCameraEditor()
{
}

void TYFlyCameraEditor::setLeftButtonFunction(const char* function)
{
    if (function == NULL)
    {
        strncpy(_leftButtonFunction, SLOT(doNothing()), 100);
    }
    else
    {
        strncpy(_leftButtonFunction, function, 100);
    }
}

void TYFlyCameraEditor::setMiddleButtonFunction(const char* function)
{
    if (function == NULL)
    {
        strncpy(_middleButtonFunction, SLOT(doNothing()), 100);
    }
    else
    {
        strncpy(_middleButtonFunction, function, 100);
    }
}

void TYFlyCameraEditor::setRightButtonFunction(const char* function)
{
    if (function == NULL)
    {
        strncpy(_rightButtonFunction, SLOT(doNothing()), 100);
    }
    else
    {
        strncpy(_rightButtonFunction, function, 100);
    }
}

void TYFlyCameraEditor::setShiftLeftButtonFunction(const char* function)
{
    if (function == NULL)
    {
        strncpy(_shiftLeftButtonFunction, SLOT(doNothing()), 100);
    }
    else
    {
        strncpy(_shiftLeftButtonFunction, function, 100);
    }
}

void TYFlyCameraEditor::setShiftRightButtonFunction(const char* function)
{
    if (function == NULL)
    {
        strncpy(_shiftRightButtonFunction, SLOT(doNothing()), 100);
    }
    else
    {
        strncpy(_shiftRightButtonFunction, function, 100);
    }
}

void TYFlyCameraEditor::setLeftButtonFunction3D(const char* function)
{
    if (function == NULL)
    {
        strncpy(_leftButtonFunction3D, SLOT(doNothing()), 100);
    }
    else
    {
        strncpy(_leftButtonFunction3D, function, 100);
    }
}

void TYFlyCameraEditor::setMiddleButtonFunction3D(const char* function)
{
    if (function == NULL)
    {
        strncpy(_middleButtonFunction3D, SLOT(doNothing()), 100);
    }
    else
    {
        strncpy(_middleButtonFunction3D, function, 100);
    }
}

void TYFlyCameraEditor::setRightButtonFunction3D(const char* function)
{
    if (function == NULL)
    {
        strncpy(_rightButtonFunction3D, SLOT(doNothing()), 100);
    }
    else
    {
        strncpy(_rightButtonFunction3D, function, 100);
    }
}

void TYFlyCameraEditor::setShiftLeftButtonFunction3D(const char* function)
{
    if (function == NULL)
    {
        strncpy(_shiftLeftButtonFunction3D, SLOT(doNothing()), 100);
    }
    else
    {
        strncpy(_shiftLeftButtonFunction3D, function, 100);
    }
}

void TYFlyCameraEditor::setShiftRightButtonFunction3D(const char* function)
{
    if (function == NULL)
    {
        strncpy(_shiftRightButtonFunction3D, SLOT(doNothing()), 100);
    }
    else
    {
        strncpy(_shiftRightButtonFunction3D, function, 100);
    }
}


void TYFlyCameraEditor::slotKeyPressed(int key)
{
    // Update la cam courante
    _pCurrentCamera = _pInteractor->getRenderer()->getActiveCamera();
    _pCurrentCamera->setDistanceStep(5, 5, 5);
    _pCurrentCamera->setCameraType(FREE);

    switch (key)
    {
        case Qt::Key_Up:
        {
            _bKeyUp = true;
            _keyEventActive++;
            if (_keyEventActive > 1) { return; }
            else { QObject::connect(_pStepTimer, SIGNAL(timeout()), this, SLOT(cameraTranslateKey())); }
        }
        break;
        case Qt::Key_Down:
        {
            _bKeyDown = true;
            _keyEventActive++;
            if (_keyEventActive > 1) { return; }
            else { QObject::connect(_pStepTimer, SIGNAL(timeout()), this, SLOT(cameraTranslateKey())); }
        }
        break;
        case Qt::Key_Left:
        {
            _bKeyLeft = true;
            _keyEventActive++;
            if (_keyEventActive > 1) { return; }
            else { QObject::connect(_pStepTimer, SIGNAL(timeout()), this, SLOT(cameraTranslateKey())); }
        }
        break;
        case Qt::Key_Right:
        {
            _bKeyRight = true;
            _keyEventActive++;
            if (_keyEventActive > 1) { return; }
            else { QObject::connect(_pStepTimer, SIGNAL(timeout()), this, SLOT(cameraTranslateKey())); }
        }
        break;
        case Qt::Key_PageUp:
        {
            _bKeyPageUp = true;
            _keyEventActive++;
            if (_keyEventActive > 1) { return; }
            else { QObject::connect(_pStepTimer, SIGNAL(timeout()), this, SLOT(cameraTranslateKey())); }
        }
        break;
        case Qt::Key_PageDown:
        {
            _bKeyPageDown = true;
            _keyEventActive++;
            if (_keyEventActive > 1) { return; }
            else { QObject::connect(_pStepTimer, SIGNAL(timeout()), this, SLOT(cameraTranslateKey())); }
        }
        break;
    }

    if (_keyEventActive > 0)
    {
        _firstCall = true;
        _pStepTimer->start(_interactionTime);
        emit startMovingCamera();
    }
}

void TYFlyCameraEditor::slotKeyReleased(int key)
{
    if (_keyEventActive > 0)
    {
        _keyEventActive--;
        switch (key)
        {
            case Qt::Key_Up:
            {
                _bKeyUp = false;
            }
            break;
            case Qt::Key_Down:
            {
                _bKeyDown = false;
            }
            break;
            case Qt::Key_Left:
            {
                _bKeyLeft = false;
            }
            break;
            case Qt::Key_Right:
            {
                _bKeyRight = false;
            }
            break;
            case Qt::Key_PageUp:
            {
                _bKeyPageUp = false;
            }
            break;
            case Qt::Key_PageDown:
            {
                _bKeyPageDown = false;
            }
            break;
        }

        if (_keyEventActive == 0)
        {
            if (!_mouseEventActive)
            {
                _pStepTimer->stop();
                QObject::disconnect(_pStepTimer, 0, this, 0);

                emit stopMovingCamera();
            }
        }
    }
}


void TYFlyCameraEditor::slotMousePressed(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
{
    if (_mouseEventActive)
    {
        return;
    }

    // Update la cam courante
    _pCurrentCamera = _pInteractor->getRenderer()->getActiveCamera();
    _pCurrentCamera->setDistanceStep(5, 5, 5);
    _pCurrentCamera->setCameraType(FREE);

    if (button & LeftButton)
    {
        // Left mouse button
        _mouseEventActive = true;

        if (state & ShiftModifier)
        {
            // With Shift button
            QObject::connect(_pStepTimer, SIGNAL(timeout()), this, _shiftLeftButtonFunction);
        }
        else
        {
            // Without shift button
            QObject::connect(_pStepTimer, SIGNAL(timeout()), this, _leftButtonFunction);
        }
    }
    else if (button & MidButton)
    {
        // Middle mouse button
        _mouseEventActive = true;
        QObject::connect(_pStepTimer, SIGNAL(timeout()), this, _middleButtonFunction);
    }
    else if (button & RightButton)
    {
        // Right mouse button
        _mouseEventActive = true;

        if (state & ShiftModifier)
        {
            // With Shift button
            QObject::connect(_pStepTimer, SIGNAL(timeout()), this, _shiftRightButtonFunction);
        }
        else
        {
            // Without Shift button
            QObject::connect(_pStepTimer, SIGNAL(timeout()), this, _rightButtonFunction);
        }
    }

    if (_mouseEventActive)
    {
        _currentMousePos[0] = x;
        _currentMousePos[1] = y;
        _lastMousePos[0] = x;
        _lastMousePos[1] = y;
        _firstCall = true;
        _pStepTimer->start(_interactionTime);

        emit startMovingCamera();
    }
}

void TYFlyCameraEditor::slotMouseReleased(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
{
    if (_mouseEventActive)
    {
        _mouseEventActive = false;
        if (_keyEventActive == 0)
        {
            _pStepTimer->stop();
            QObject::disconnect(_pStepTimer, 0, this, 0);

            emit stopMovingCamera();
        }
    }
}

void TYFlyCameraEditor::slotMouseMoved(int x, int y, Qt::MouseButtons button, Qt::KeyboardModifiers state)
{
    _currentMousePos[0] = x;
    _currentMousePos[1] = y;
}

void TYFlyCameraEditor::slotWheeled(int x, int y, int delta, Qt::KeyboardModifiers state)
{
    //do nothing
}

void TYFlyCameraEditor::slotViewTypeChanged(int view)
{
    //  setNavigationOnViewType(view);
}

void TYFlyCameraEditor::setToNavigation3D()
{
    setLeftButtonFunction(_leftButtonFunction3D);
    setMiddleButtonFunction(_middleButtonFunction3D);
    setRightButtonFunction(_rightButtonFunction3D);
    setShiftLeftButtonFunction(_shiftLeftButtonFunction3D);
    setShiftRightButtonFunction(_shiftRightButtonFunction3D);
}

void TYFlyCameraEditor::cameraRotate()
{
    float xf;
    float yf;

    // do nothing if mouse is still on the same pos
    if ((_currentMousePos[0] == _lastMousePos[0]) &&
        (_currentMousePos[1] == _lastMousePos[1]))
    {
        return;
    }

    // first time we do some preprocessing
    if (_firstCall)
    {
        QSize size = _pInteractor->size();
        double vp[4];
        if (_pCurrentCamera)
        {
            _pCurrentCamera->getViewPort(vp);

            _deltaAzimuth = -20.0 / ((vp[2] - vp[0]) * size.width());
            _deltaElevation = -20.0 / ((vp[3] - vp[1]) * size.height());
        }
    }

    if (_pCurrentCamera)
    {
        xf = (_currentMousePos[0] - _lastMousePos[0]) * _deltaAzimuth
             * _trackballFactor;
        yf = (_lastMousePos[1] - _currentMousePos[1]) * _deltaElevation
             * _trackballFactor;

        _pCurrentCamera->azimuth(xf);
        _pCurrentCamera->elevation(yf);
    }

    if (!_firstCall)
    {
    }
    else
    {
        _firstCall = false;
    }

    emit cameraUpdated();

    // update
    _pModeler->updateView();

    // save mouse pos
    _lastMousePos[0] = _currentMousePos[0];
    _lastMousePos[1] = _currentMousePos[1];
}

void TYFlyCameraEditor::cameraTranslate()
{
    double viewFocus[4];
    double motionVector[3];

    // do nothing if mouse is still on the same pos
    if ((_currentMousePos[0] == _lastMousePos[0]) &&
        (_currentMousePos[1] == _lastMousePos[1]))
    {
        return;
    }

    if (_pCurrentCamera)
    {
        NxVec3 ret = OGLCamera::worldToDisplay(_pCurrentCamera->to);
        viewFocus[0] = ret.x;
        viewFocus[1] = ret.y;
        viewFocus[2] = ret.z;
        double focalDepth = viewFocus[2];
        NxVec3 retNew = OGLCamera::displayToWorld(NxVec3(float(_pInteractor->width() / 2.0 + (_currentMousePos[0] - _lastMousePos[0])),
                                                         float(_pInteractor->height() / 2.0 - (_currentMousePos[1] - _lastMousePos[1])),
                                                         focalDepth));

        motionVector[0] = _trackballFactor / 10.0 * (_pCurrentCamera->to.x - retNew.x);
        motionVector[1] = _trackballFactor / 10.0 * (_pCurrentCamera->to.y - retNew.y);
        motionVector[2] = _trackballFactor / 10.0 * (_pCurrentCamera->to.z - retNew.z);

        _pCurrentCamera->to.x += motionVector[0];
        _pCurrentCamera->to.y += motionVector[1];
        _pCurrentCamera->to.z += motionVector[2];

        _pCurrentCamera->from.x += motionVector[0];
        _pCurrentCamera->from.y += motionVector[1];
        _pCurrentCamera->from.z += motionVector[2];
    }

    emit cameraUpdated();

    // update
    _pModeler->updateView();

    // save mouse pos
    _lastMousePos[0] = _currentMousePos[0];
    _lastMousePos[1] = _currentMousePos[1];
}

void TYFlyCameraEditor::cameraTranslateKey()
{
    if (_pCurrentCamera)
    {
        _pCurrentCamera->calculateStepVectors();
        if (_bKeyUp)
        {
            _pCurrentCamera->moveFront();
        }
        if (_bKeyDown)
        {
            _pCurrentCamera->moveBack();
        }
        if (_bKeyLeft)
        {
            _pCurrentCamera->moveLeft();
        }
        if (_bKeyRight)
        {
            _pCurrentCamera->moveRight();
        }
        if (_bKeyPageUp)
        {
            _pCurrentCamera->moveUp();
        }
        if (_bKeyPageDown)
        {
            _pCurrentCamera->moveDown();
        }
    }

    emit cameraUpdated();

    // update
    _pModeler->updateView();
}


void TYFlyCameraEditor::cameraFrontBack()
{
    // do nothing if mouse is still on the same y position
    if (_currentMousePos[1] == _lastMousePos[1])
    {
        //don't change yf
    }
    else
    {
        _yf = float(_currentMousePos[1] - _lastMousePos[1]) /
              float(_pInteractor->height()) * _trackballFactor;
        // if yf < 0, we need to be between 0..1
        _yf = pow(1.1f, _yf);
    }

    if (_pCurrentCamera)
    {
        _pCurrentCamera->calculateStepVectors();
        if (_yf > 1)
        {
            _pCurrentCamera->moveBack();
        }
        else
        {
            _pCurrentCamera->moveFront();
        }
    }

    emit cameraUpdated();

    // update
    _pModeler->updateView(false, _pModeler->getShowGrid());

    // save mouse pos
    _lastMousePos[0] = _currentMousePos[0];
    _lastMousePos[1] = _currentMousePos[1];
}

void TYFlyCameraEditor::cameraRoll()
{
    float angle;

    // do nothing if mouse is still on the same pos
    if ((_currentMousePos[0] == _lastMousePos[0]) &&
        (_currentMousePos[1] == _lastMousePos[1]))
    {
        return;
    }

    if (_pCurrentCamera)
    {
        // first time we do some preprocessing
        if (_firstCall)
        {
            _renCenter[0] = _pCurrentCamera->getCenter(_pInteractor->width(), _pInteractor->height()).x;
            _renCenter[1] = _pCurrentCamera->getCenter(_pInteractor->width(), _pInteractor->height()).y;
            _renCenter[2] = _pCurrentCamera->getCenter(_pInteractor->width(), _pInteractor->height()).z;
            _firstCall = false;
        }

        // calculate the angle in radians and roll the camera
        int diffX1 = _currentMousePos[0] - int(_renCenter[0]);
        int diffY1 = _currentMousePos[1] - int(_renCenter[1]);
        int diffX2 = _lastMousePos[0] - int(_renCenter[0]);
        int diffY2 = _lastMousePos[1] - int(_renCenter[1]);

        double a1 = atan2(double(diffY1), double(diffX1));
        double a2 = atan2(double(diffY2), double(diffX2));
        angle = (a2 - a1) / (2.0 * 3.1415926535) * 360.0 / 10.0 * _trackballFactor;

        _pCurrentCamera->roll(angle);

        emit cameraUpdated();
    }

    // update
    _pModeler->updateView();

    // save mouse pos
    _lastMousePos[0] = _currentMousePos[0];
    _lastMousePos[1] = _currentMousePos[1];
}

void TYFlyCameraEditor::doNothing()
{
}
