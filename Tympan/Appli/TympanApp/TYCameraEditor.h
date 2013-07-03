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
 * \file TYCameraEditor.h
 * \brief gestion de l'edition de la camera (fichier header)
 *
 *
 *
 *
 *
 */


#ifndef __TY_CAMERA_EDITOR__
#define __TY_CAMERA_EDITOR__


#include "TYAppDefines.h"
#include "TYAbstractSceneEditor.h"

class QTimer;
class OGLCamera;

/**
 * \class TYCameraEditor
 * \brief  Gestion de l'edition en mode camera
 */

class TYCameraEditor : public TYAbstractSceneEditor
{
    Q_OBJECT

public:
    TYCameraEditor(TYModelerFrame* pModeler);
    ~TYCameraEditor();

    void setLeftButtonFunction(const char* function);
    void setMiddleButtonFunction(const char* function);
    void setRightButtonFunction(const char* function);
    void setShiftLeftButtonFunction(const char* function);
    void setShiftRightButtonFunction(const char* function);

    void setLeftButtonFunction2D(const char* function);
    void setMiddleButtonFunction2D(const char* function);
    void setRightButtonFunction2D(const char* function);
    void setShiftLeftButtonFunction2D(const char* function);
    void setShiftRightButtonFunction2D(const char* function);

    void setLeftButtonFunction3D(const char* function);
    void setMiddleButtonFunction3D(const char* function);
    void setRightButtonFunction3D(const char* function);
    void setShiftLeftButtonFunction3D(const char* function);
    void setShiftRightButtonFunction3D(const char* function);

    void setLeftButtonFunctionFree(const char* function);
    void setMiddleButtonFunctionFree(const char* function);
    void setRightButtonFunctionFree(const char* function);
    void setShiftLeftButtonFunctionFree(const char* function);
    void setShiftRightButtonFunctionFree(const char* function);

    void setInteractionTime(int time) { _interactionTime = time; }
    int getInteractionTime() { return _interactionTime; }

    void setSensitivity(float t) { _trackballFactor = t; }
    float getSensitivity() { return _trackballFactor; }

    void setWheelStep(float step) { _wheelStep = step; }
    float getWheelStep() { return _wheelStep; }

    void cameraZoneZoom(const float focalPoint2D[3], double factor);

    void setZoomStep(float step) { _zoomStep = step; }
    float getZoomStep() { return _zoomStep; }

    void setTranslateStep(float step) { _translateStep = step; }
    float getTranslateStep() { return _translateStep; }

    void setRotateStep(float step) { _rotateStep = step; }
    float getRotateStep() { return _rotateStep; }


signals:
    /**
     * \fn void cameraUpdated()
     * \brief Indique que la position ou l'orientation de la camera a ete modifiee.
     */
    void cameraUpdated();
    /**
     * \fn void startMovingCamera()
     * \brief Indique le debut du deplacement de la camera.
     */
    void startMovingCamera();
    /**
     * \fn void stopMovingCamera()
     * \brief Indique la fin du deplacement de la camera.
     */
    void stopMovingCamera();


public slots:
    virtual void slotMousePressed(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state);
    virtual void slotMouseReleased(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state);
    virtual void slotMouseMoved(int x, int y, Qt::MouseButtons button, Qt::KeyboardModifiers state);
    virtual void slotWheeled(int x, int y, int delta, Qt::KeyboardModifiers state);
    virtual void slotViewTypeChanged(int view);
    virtual void slotKeyPressed(int key);
    virtual void slotKeyReleased(int key);

    /**
     * \fn void setNavigationOnViewType(int view)
     * \brief Switch auto entre navi 2D ou 3D.
     */
    void setNavigationOnViewType(int view);

    void setToNavigation2D();
    void setToNavigation3D();
    void setToNavigationFree();

    void cameraStepRotateUp();
    void cameraStepRotateDown();
    void cameraStepRotateLeft();
    void cameraStepRotateRight();
    void cameraStepTranslateUp();
    void cameraStepTranslateDown();
    void cameraStepTranslateLeft();
    void cameraStepTranslateRight();
    void cameraStepZoomIn();
    void cameraStepZoomOut();

protected slots:
    void cameraRotate();
    void cameraTranslate();
    void cameraTranslateKey();
    void cameraZoom();
    void cameraRoll();

    void doNothing();


private:
    OGLCamera*  _pCurrentCamera;

    float _renCenter[3];
    float _deltaElevation;
    float _deltaAzimuth;

    QTimer* _pStepTimer;
    bool    _mouseEventActive;
    int     _keyEventActive;
    bool    _firstCall;
    float   _trackballFactor;
    int     _interactionTime;
    int     _currentMousePos[2];
    int     _lastMousePos[2];
    float   _wheelStep;

    bool _bKeyUp;
    bool _bKeyDown;
    bool _bKeyLeft;
    bool _bKeyRight;
    bool _bKeyPageUp;
    bool _bKeyPageDown;
    bool _bKeyShift;

    char _leftButtonFunction[200];
    char _middleButtonFunction[200];
    char _rightButtonFunction[200];
    char _shiftLeftButtonFunction[200];
    char _shiftRightButtonFunction[200];

    char _leftButtonFunction2D[200];
    char _middleButtonFunction2D[200];
    char _rightButtonFunction2D[200];
    char _shiftLeftButtonFunction2D[200];
    char _shiftRightButtonFunction2D[200];

    char _leftButtonFunction3D[200];
    char _middleButtonFunction3D[200];
    char _rightButtonFunction3D[200];
    char _shiftLeftButtonFunction3D[200];
    char _shiftRightButtonFunction3D[200];

    char _leftButtonFunctionFree[200];
    char _middleButtonFunctionFree[200];
    char _rightButtonFunctionFree[200];
    char _shiftLeftButtonFunctionFree[200];
    char _shiftRightButtonFunctionFree[200];

    float _zoomStep;
    float _translateStep;
    float _rotateStep;
};

#endif  // __TY_CAMERA_EDITOR__
