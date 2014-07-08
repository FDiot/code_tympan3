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

/*
 *
 *
 *
 *
 */

#ifndef __O_GLCAMERA__
#define __O_GLCAMERA__

#include "Tympan/core/smartptr.h"

#if _WIN32
#include <windows.h>
#endif //_WIN32

#include <GL/gl.h>
#include <GL/glu.h>
#include "NxVec3.h"
#include "Tympan/models/common/3d.h"

typedef enum {PARALLEL, PERSPECTIVE, FREE} CameraType;

class OGLCamera
{

public:
    OGLCamera(NxReal* _from, NxReal* _to, NxReal* _up, int w, int h, CameraType eCameraType);
    void setCameraType(CameraType eCameraType);
    void lookAt();
    void moveFront();
    void moveBack();
    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();
    void rotateUpLockSrc();
    void rotateDownLockSrc();
    void rotateUpLockSrcKeepUpAndFront();
    void rotateDownLockSrcKeepUpAndFront();
    void rotateLeftLockSrc();
    void rotateRightLockSrc();
    void rotateUpLockDst();
    void rotateDownLockDst();
    void rotateLeftLockDst();
    void rotateRightLockDst();
    void rotateSideLeft();
    void rotateSideRight();
    void move(NxReal* _direction);
    void moveFront(NxReal _distance);
    void moveBack(NxReal _distance);
    void moveUp(NxReal _distance);
    void moveDown(NxReal _distance);
    void moveLeft(NxReal _distance);
    void moveRight(NxReal _distance);
    void rotateUpLockSrc(NxReal _angle);
    void rotateDownLockSrc(NxReal _angle);
    void rotateUpLockSrcKeepUpAndFront(NxReal _angle);
    void rotateDownLockSrcKeepUpAndFront(NxReal _angle);
    void rotateLeftLockSrc(NxReal _angle);
    void rotateRightLockSrc(NxReal _angle);
    void rotateUpLockDst(NxReal _angle);
    void rotateDownLockDst(NxReal _angle);
    void rotateLeftLockDst(NxReal _angle);
    void rotateRightLockDst(NxReal _angle);
    void rotateSideLeft(NxReal _angle);
    void rotateSideRight(NxReal _angle);
    void setDistanceStep(NxReal _magnitudeStepUp, NxReal _magnitudeStepFront, NxReal _magnitudeStepLeft);
    void setAllAngleStep(NxReal _angle);
    void setAngleStep(NxReal _stepAngleUpDown, NxReal _stepAngleLeftRight, NxReal _stepAngleSide);
    void setFrom(NxReal* _from);
    void setTo(NxReal* _to);
    void setUp(NxReal* _up);
    void setFromToUp(NxReal* _from, NxReal* _to, NxReal* _up);
    void setFromToUp();
    void setDistanceFromToLockFrom(NxReal _distanceFromTo);
    void setDistanceFromToLockTo(NxReal _distanceFromTo);
    void setModeLock(bool _modeLockUpDown, bool _modeLockLeftRight, bool _modeLockSide);
    void setMinMaxCurrentUpDown(NxReal _minUpDown, NxReal _maxUpDown, NxReal _currentUpDown);
    void setMinMaxCurrentLeftRight(NxReal _minLeftRight, NxReal _maxLeftRight, NxReal _currentLeftRight);
    void setMinMaxCurrentSide(NxReal _minSide, NxReal _maxSide, NxReal _currentSide);
    void setAllMinMaxCurrent(NxReal _angle);
    void setSize(int w, int h);
    void calculateStepVectors();
    static NxVec3 displayToWorld(NxVec3 display);
    static NxVec3 worldToDisplay(NxVec3 world);
    NxVec3 getCenter(int sizeX, int sizeY);
    void getViewPort(double* vp);
    void zoom(double zoomFactor);
    void azimuth(NxReal _angle);
    void elevation(NxReal _angle);
    void roll(NxReal _angle);
    void resetZoom(int w = -1, int h = -1);
    void resetTranslation();
    void setTranslation(double x, double y, double z);
    void getTranslation(double& x, double& y, double& z);
    void resetRotations();
    void setDefaultZoomFactor(double defaultZoomFactor);
    void getPosition(double& x, double& y, double& z);

    NxVec3  // points:
    from,
    to,
    // normalized vectors:
    up,
    left,
    front,
    // balanced vectors:
    stepUp,
    stepLeft,
    stepFront,
    flyTo,
    flyFrom,
    flyFromTo,
    flyUp,
    flyLeft,
    flyFront;

    NxReal  sinUpDown,
            cosUpDown,
            stepAngleUpDown,
            sinLeftRight,
            cosLeftRight,
            stepAngleLeftRight,
            sinSide,
            cosSide,
            stepAngleSide;
    // distance and vector from - to :
    NxReal  distanceFromTo;
    NxVec3  fromTo;

    bool    modeLockUpDown,
            modeLockLeftRight,
            modeLockSide;

    NxReal  magnitudeStepUp,
            magnitudeStepFront,
            magnitudeStepLeft,
            maxUpDown,
            minUpDown,
            currentUpDown,
            maxLeftRight,
            minLeftRight,
            currentLeftRight,
            maxSide,
            minSide,
            currentSide;

    int     m_w, m_h;
    double  m_zoomFactor, m_defaultZoomFactor, m_angleAz, m_angleEl, m_angleRo;
    CameraType  m_eCameraType;
    NxVec3  m_translate;
};

// Smart pointer sur OGLCamera.
typedef SmartPtr<OGLCamera> LPOGLCamera;

#endif // __O_GLCAMERA__
