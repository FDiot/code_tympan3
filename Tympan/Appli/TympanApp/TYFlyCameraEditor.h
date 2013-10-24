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
 * \file TYFlyCameraEditor.h
 * \brief Indique que la position ou l'orientation de la camera a ete modifiee (fichier header)
 *
 *
 *
 *
 *
 *
 */


#ifndef __TY_FLYCAMERA_EDITOR__
#define __TY_FLYCAMERA_EDITOR__


#include "TYAppDefines.h"
#include "TYAbstractSceneEditor.h"

class QTimer;
class OGLCamera;


/**
 * \class TYFlyCameraEditor
 * \brief Indique que la position ou l'orientation de la camera a ete modifiee
 */
class TYFlyCameraEditor : public TYAbstractSceneEditor
{
    Q_OBJECT

public:
    TYFlyCameraEditor(TYModelerFrame* pModeler);
    ~TYFlyCameraEditor();

    void setLeftButtonFunction(const char* function);
    void setMiddleButtonFunction(const char* function);
    void setRightButtonFunction(const char* function);
    void setShiftLeftButtonFunction(const char* function);
    void setShiftRightButtonFunction(const char* function);

    void setLeftButtonFunction3D(const char* function);
    void setMiddleButtonFunction3D(const char* function);
    void setRightButtonFunction3D(const char* function);
    void setShiftLeftButtonFunction3D(const char* function);
    void setShiftRightButtonFunction3D(const char* function);

    void setInteractionTime(int time) { _interactionTime = time; }
    int getInteractionTime() { return _interactionTime; }

    void setSensitivity(float t) { _trackballFactor = t; }
    float getSensitivity() { return _trackballFactor; }


signals:
    /**
     * Indique que la position ou l'orientation de la camera a ete modifiee.
     */
    void cameraUpdated();
    /**
     * Indique le debut du deplacement de la camera.
     */
    void startMovingCamera();
    /**
     * Indique la fin du deplacement de la camera.
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

    void setToNavigation3D();

protected slots:
    void cameraRotate();
    void cameraTranslate();
    void cameraTranslateKey();
    void cameraFrontBack();
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
    float       _wheelStep;
    int     _currentKey;

    bool _bKeyUp;
    bool _bKeyDown;
    bool _bKeyLeft;
    bool _bKeyRight;
    bool _bKeyPageUp;
    bool _bKeyPageDown;


    char _leftButtonFunction[200];
    char _middleButtonFunction[200];
    char _rightButtonFunction[200];
    char _shiftLeftButtonFunction[200];
    char _shiftRightButtonFunction[200];

    char _leftButtonFunction3D[200];
    char _middleButtonFunction3D[200];
    char _rightButtonFunction3D[200];
    char _shiftLeftButtonFunction3D[200];
    char _shiftRightButtonFunction3D[200];

    float _zoomStep;
    float _translateStep;
    float _rotateStep;
    float _yf;
};

#endif  // __TY_FLYCAMERA_EDITOR__
