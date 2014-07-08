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
 */



#include "OGLCamera.h"
#include "Tympan/models/common/3d.h"

#if _WIN32
#include <windows.h>
#endif //_WIN32

#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif

OGLCamera::OGLCamera(NxReal* _from, NxReal* _to, NxReal* _up, int w, int h, CameraType eCameraType)
{
    m_w = w;
    m_h = h;
    setFromToUp(_from, _to, _up);
    setAllAngleStep(0.1);
    setModeLock(false, false, false);
    m_zoomFactor = 1.0;
    m_eCameraType = eCameraType;

    switch (m_eCameraType)
    {
        case PARALLEL:
        {
            m_defaultZoomFactor = 0.29; // le facteur par defaut est pour un monde de 400 de MAX(larg,haut) et un viewport 700 de MIN(larg,haut)
        }
        break;
        case PERSPECTIVE:
        {
            m_defaultZoomFactor = 0.5; // le facteur par defaut est pour un monde de 400 de MAX(larg,haut) et un viewport 700 de MIN(larg,haut)
        }
        break;
        case FREE:
        {
            m_defaultZoomFactor = 1; // le facteur par defaut est pour un monde de 400 de MAX(larg,haut) et un viewport 700 de MIN(larg,haut)
        }
        break;
    }

    m_angleAz = 0;
    m_angleEl = 0;
    m_angleRo = 0;
    m_translate = NxVec3(0, 0, 0);
}

void OGLCamera::setSize(int w, int h)
{
    m_w = w;
    m_h = h;
}

void OGLCamera::lookAt()
{
    switch (m_eCameraType)
    {
        case PARALLEL:
        {
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(-m_w * m_zoomFactor, m_w * m_zoomFactor, -m_h * m_zoomFactor, m_h * m_zoomFactor, 1, 1000);
            gluLookAt(from.x, from.y, from.z, to.x, to.y, to.z, up.x, up.y, up.z);
        }
        break;
        case PERSPECTIVE:
        {
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(30 * m_zoomFactor, (double)m_w / m_h, 10, 50000); // Initialement 1, 50000
            gluLookAt(from.x, from.y, from.z, to.x, to.y, to.z, up.x, up.y, up.z);

            //Roll
            glRotatef((float)m_angleRo, (float)front.x, (float)front.y, (float)front.z);/* orbit the front axis */
            //Elevation
            NxVec3 elevationVector = up.cross(fromTo);
            glRotatef((float) - m_angleEl, (float)elevationVector.x, (float)elevationVector.y, (float)elevationVector.z); /* orbit the UP axis */
            //Azhimut
            glRotatef((float) - m_angleAz, (float)up.x, (float)up.y, (float)up.z); /* orbit the Y*Z axis */
        }
        break;
        case FREE:
        {
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(30 * m_zoomFactor, (double)m_w / m_h, 1, 5000); // Initialement 1, 50000
            gluLookAt(from.x, from.y, from.z, to.x, to.y, to.z, up.x, up.y, up.z);

            glTranslatef((float) - flyFromTo.x, (float) - flyFromTo.y, (float) - flyFromTo.z);

            //Roll
            glRotatef((float)m_angleRo, (float)front.x, (float)front.y, (float)front.z);/* orbit the front axis */
            //Elevation
            NxVec3 elevationVector = up.cross(fromTo);
            glRotatef((float) - m_angleEl, (float)elevationVector.x, (float)elevationVector.y, (float)elevationVector.z); /* orbit the UP axis */
            //Azhimut
            glRotatef((float) - m_angleAz, (float)up.x, (float)up.y, (float)up.z); /* orbit the Y*Z axis */

            glTranslatef((float)flyFromTo.x, (float)flyFromTo.y, (float)flyFromTo.z);

            glTranslatef((float) - m_translate.x, (float) - m_translate.y, (float) - m_translate.z);
        }
        break;
    }
}

void OGLCamera::setCameraType(CameraType eCameraType)
{
    m_eCameraType = eCameraType;
}

void OGLCamera::move(NxReal* _direction)
{
    from.x += _direction[0]; from.y += _direction[1]; from.z += _direction[2];
    to.x += _direction[0]; to.y += _direction[1]; to.z += _direction[2];
}

void OGLCamera::moveFront()
{
    if (m_eCameraType == FREE)
    {
        m_translate += stepFront;
        calculateStepVectors();
    }
    else
    {
        from += stepFront;
        to += stepFront;
    }
}

void OGLCamera::moveBack()
{
    if (m_eCameraType == FREE)
    {
        m_translate -= stepFront;
        calculateStepVectors();
    }
    else
    {
        from -= stepFront;
        to -= stepFront;
    }
}

void OGLCamera::moveUp()
{
    if (m_eCameraType == FREE)
    {
        m_translate += stepUp;
        calculateStepVectors();
    }
    else
    {
        from += stepUp;
        to += stepUp;
    }
}

void OGLCamera::moveDown()
{
    if (m_eCameraType == FREE)
    {
        m_translate -= stepUp;
        calculateStepVectors();
    }
    else
    {
        from -= stepUp;
        to -= stepUp;
    }
}

void OGLCamera::moveLeft()
{
    if (m_eCameraType == FREE)
    {
        m_translate += stepLeft;
        calculateStepVectors();
    }
    else
    {
        from += stepLeft;
        to += stepLeft;
    }
}

void OGLCamera::moveRight()
{
    if (m_eCameraType == FREE)
    {
        m_translate -= stepLeft;
        calculateStepVectors();
    }
    else
    {
        from -= stepLeft;
        to -= stepLeft;
    }
}

void OGLCamera::rotateUpLockSrc()
{
    if (!modeLockUpDown || currentUpDown + stepAngleUpDown <= maxUpDown && (currentUpDown += stepAngleUpDown))
    {
        up.rotate(-sinUpDown, cosUpDown, left);
        front.rotate(left);
        fromTo = front * distanceFromTo;
        to = from + fromTo;
        calculateStepVectors();
    }
}
void OGLCamera::rotateDownLockSrc()
{
    if (!modeLockUpDown || currentUpDown - stepAngleUpDown >= minUpDown && (currentUpDown -= stepAngleUpDown))
    {
        up.rotate(sinUpDown, cosUpDown, left);
        front.rotate(left);
        fromTo = front * distanceFromTo;
        to = from + fromTo;
        calculateStepVectors();
    }
}
void OGLCamera::rotateUpLockSrcKeepUpAndFront()
{
    if (!modeLockUpDown || currentUpDown + stepAngleUpDown <= maxUpDown && (currentUpDown += stepAngleUpDown))
    {
        fromTo.rotate(-sinUpDown, cosUpDown, left);
        to = from + fromTo;
    }
}
void OGLCamera::rotateDownLockSrcKeepUpAndFront()
{
    if (!modeLockUpDown || currentUpDown - stepAngleUpDown >= minUpDown && (currentUpDown -= stepAngleUpDown))
    {
        fromTo.rotate(sinUpDown, cosUpDown, left);
        to = from + fromTo;
    }
}

void OGLCamera::rotateLeftLockSrc()
{
    if (!modeLockLeftRight || currentLeftRight + stepAngleLeftRight <= maxLeftRight && (currentLeftRight += stepAngleLeftRight))
    {
        left.rotate(sinLeftRight, cosLeftRight, up);
        front.rotate(up);
        fromTo.rotate(up);
        to = from + fromTo;
        calculateStepVectors();
    }
}

void OGLCamera::rotateRightLockSrc()
{
    if (!modeLockLeftRight || currentLeftRight - stepAngleLeftRight >= minLeftRight && (currentLeftRight -= stepAngleLeftRight))
    {
        left.rotate(-sinLeftRight, cosLeftRight, up);
        front.rotate(up);
        fromTo.rotate(up);
        to = from + fromTo;
        calculateStepVectors();
    }
}

void OGLCamera::rotateUpLockDst()
{
    if (!modeLockUpDown || currentUpDown + stepAngleUpDown <= maxUpDown && (currentUpDown += stepAngleUpDown))
    {
        up.rotate(-sinUpDown, cosUpDown, left);
        front.rotate(left);
        fromTo = front * distanceFromTo;
        from = to - fromTo;
        calculateStepVectors();
    }
}
void OGLCamera::rotateDownLockDst()
{
    if (!modeLockUpDown || currentUpDown - stepAngleUpDown >= minUpDown && (currentUpDown -= stepAngleUpDown))
    {
        up.rotate(sinUpDown, cosUpDown, left);
        front.rotate(left);
        fromTo = front * distanceFromTo;
        from = to - fromTo;
        calculateStepVectors();
    }
}

void OGLCamera::rotateLeftLockDst()
{
    if (!modeLockLeftRight || currentLeftRight + stepAngleLeftRight <= maxLeftRight && (currentLeftRight += stepAngleLeftRight))
    {
        left.rotate(sinLeftRight, cosLeftRight, up);
        front.rotate(up);
        fromTo.rotate(up);
        from = to - fromTo;
        calculateStepVectors();
    }
}

void OGLCamera::rotateRightLockDst()
{
    if (!modeLockLeftRight || currentLeftRight - stepAngleLeftRight >= minLeftRight && (currentLeftRight -= stepAngleLeftRight))
    {
        left.rotate(-sinLeftRight, cosLeftRight, up);
        front.rotate(up);
        fromTo.rotate(up);
        from = to - fromTo;
        calculateStepVectors();
    }
}

void OGLCamera::rotateSideLeft()
{
    if (!modeLockSide || currentSide + stepAngleSide <= maxSide && (currentSide += stepAngleSide))
    {
        up.rotate(sinSide, cosSide, front);
        left.rotate(front);
        calculateStepVectors();
    }
}

void OGLCamera::rotateSideRight()
{
    if (!modeLockSide || currentSide - stepAngleSide >= minSide && (currentSide -= stepAngleSide))
    {
        up.rotate(-sinSide, cosSide, front);
        left.rotate(front);
        calculateStepVectors();
    }
}

void OGLCamera::moveUp(NxReal _distance)
{
    from += up * _distance;
    to += up * _distance;
}

void OGLCamera::moveDown(NxReal _distance)
{
    from -= up * _distance;
    to -= up * _distance;
}

void OGLCamera::moveFront(NxReal _distance)
{
    from += front * _distance;
    to += front * _distance;
}

void OGLCamera::moveBack(NxReal _distance)
{
    from -= front * _distance;
    to -= front * _distance;
}

void OGLCamera::moveLeft(NxReal _distance)
{
    from += left * _distance;
    to += left * _distance;
}

void OGLCamera::moveRight(NxReal _distance)
{
    from -= left * _distance;
    to -= left * _distance;
}

void OGLCamera::rotateUpLockSrc(NxReal _angle)
{
    if (!modeLockUpDown || currentUpDown + _angle <= maxUpDown && (currentUpDown += _angle))
    {
        up.rotate(-_angle, left);
        front.rotate(left);
        fromTo = front * distanceFromTo;
        to = from + fromTo;
        calculateStepVectors();
    }
}
void OGLCamera::rotateUpLockDst(NxReal _angle)
{
    if (!modeLockUpDown || currentUpDown + _angle <= maxUpDown && (currentUpDown += _angle))
    {
        up.rotate(-_angle, left);
        front.rotate(left);
        fromTo = front * distanceFromTo;
        from = to - fromTo;
        calculateStepVectors();
    }
}
void OGLCamera::rotateDownLockSrc(NxReal _angle)
{
    if (!modeLockUpDown || currentUpDown - _angle >= minUpDown && (currentUpDown -= _angle))
    {
        up.rotate(_angle, left);
        front.rotate(left);
        fromTo = front * distanceFromTo;
        to = from + fromTo;
        calculateStepVectors();
    }
}

void OGLCamera::rotateUpLockSrcKeepUpAndFront(NxReal _angle)
{
    if (!modeLockUpDown || currentUpDown + _angle <= maxUpDown && (currentUpDown += _angle))
    {
        fromTo.rotate(-_angle, left);
        to = from + fromTo;
    }
}

void OGLCamera::rotateDownLockSrcKeepUpAndFront(NxReal _angle)
{
    if (!modeLockUpDown || currentUpDown - _angle >= minUpDown && (currentUpDown -= _angle))
    {
        fromTo.rotate(_angle, left);
        to = from + fromTo;
    }
}

void OGLCamera::rotateLeftLockSrc(NxReal _angle)
{
    if (!modeLockLeftRight || currentLeftRight + _angle <= maxLeftRight && (currentLeftRight += _angle))
    {
        left.rotate(_angle, up);
        front.rotate(up);
        fromTo = front * distanceFromTo;
        to = from + fromTo;
        calculateStepVectors();
    }
}

void OGLCamera::rotateRightLockSrc(NxReal _angle)
{
    if (!modeLockLeftRight || currentLeftRight - _angle >= minLeftRight && (currentLeftRight -= _angle))
    {
        left.rotate(-_angle, up);
        front.rotate(up);
        fromTo = front * distanceFromTo;
        to = from + fromTo;
        calculateStepVectors();
    }
}



void OGLCamera::rotateDownLockDst(NxReal _angle)
{
    if (!modeLockUpDown || currentUpDown - _angle >= minUpDown && (currentUpDown -= _angle))
    {
        up.rotate(_angle, left);
        front.rotate(left);
        fromTo = front * distanceFromTo;
        from = to - fromTo;
        calculateStepVectors();
    }
}

void OGLCamera::rotateLeftLockDst(NxReal _angle)
{
    if (!modeLockLeftRight || currentLeftRight + _angle <= maxLeftRight && (currentLeftRight += _angle))
    {
        left.rotate(_angle, up);
        front.rotate(up);
        fromTo.rotate(up);
        from = to - fromTo;
        calculateStepVectors();
    }
}

void OGLCamera::rotateRightLockDst(NxReal _angle)
{
    if (!modeLockLeftRight || currentLeftRight - _angle >= minLeftRight && (currentLeftRight -= _angle))
    {
        left.rotate(-_angle, up);
        front.rotate(up);
        fromTo.rotate(up);
        from = to - fromTo;
        calculateStepVectors();
    }
}

void OGLCamera::rotateSideLeft(NxReal _angle)
{
    if (!modeLockSide || currentSide + _angle <= maxSide && (currentSide += _angle))
    {
        up.rotate(_angle, front);
        left.rotate(front);
        calculateStepVectors();
    }
}

void OGLCamera::rotateSideRight(NxReal _angle)
{
    if (!modeLockSide || currentSide - _angle >= minSide && (currentSide -= _angle))
    {
        up.rotate(-_angle, front);
        left.rotate(front);
        calculateStepVectors();
    }
}

void OGLCamera::setDistanceStep(NxReal _magnitudeStepUp, NxReal _magnitudeStepFront, NxReal _magnitudeStepLeft)
{
    stepUp /= magnitudeStepUp;
    magnitudeStepUp = _magnitudeStepUp;
    stepUp *= magnitudeStepUp;
    stepFront /= magnitudeStepFront;
    magnitudeStepFront = _magnitudeStepFront;
    stepFront *= magnitudeStepFront;
    stepLeft /= magnitudeStepLeft;
    magnitudeStepLeft = _magnitudeStepLeft;
    stepLeft *= magnitudeStepLeft;
}

void OGLCamera::setAllAngleStep(NxReal _angle)
{
    NxReal s = sin(_angle), c = cos(_angle);
    sinUpDown = sinLeftRight = sinSide = s;
    cosUpDown = cosLeftRight = cosSide = c;
    stepAngleUpDown = stepAngleLeftRight = stepAngleSide = _angle;
}

void OGLCamera::setAngleStep(NxReal _stepAngleUpDown, NxReal _stepAngleLeftRight, NxReal _stepAngleSide)
{
    sinUpDown = sin(_stepAngleUpDown);
    sinLeftRight = sin(_stepAngleLeftRight);
    stepAngleUpDown = _stepAngleUpDown;
    sinSide = sin(_stepAngleSide);
    cosUpDown = cos(_stepAngleUpDown);
    stepAngleLeftRight = _stepAngleLeftRight;
    cosLeftRight = cos(_stepAngleLeftRight);
    cosSide = cos(_stepAngleSide);
    stepAngleSide = _stepAngleSide;
}

void OGLCamera::setFrom(NxReal* _from)
{
    to += NxVec3(_from) - from;
    from = NxVec3(_from);
    calculateStepVectors();
}

void OGLCamera::setTo(NxReal* _to)
{
    from += NxVec3(_to) - to;
    to = NxVec3(_to);
    calculateStepVectors();
}

void OGLCamera::setUp(NxReal* _up)
{
    up = NxVec3(_up);
    calculateStepVectors();
}

void OGLCamera::setFromToUp(NxReal* _from, NxReal* _to, NxReal* _up)
{
    from = NxVec3(_from);
    to = NxVec3(_to);
    up = NxVec3(_up, true);
    front = NxVec3(to - from, true);
    fromTo = to - from;
    left.cross(up, front, true);

    distanceFromTo = fromTo.magnitude();
    magnitudeStepUp = 1;
    magnitudeStepFront = 1;
    magnitudeStepLeft = 1;
    calculateStepVectors();
}

void OGLCamera::setFromToUp()
{
    setFromToUp(from.get(), to.get(), up.get());
}

void OGLCamera::setDistanceFromToLockFrom(NxReal _distanceFromTo)
{
    fromTo *= _distanceFromTo / distanceFromTo;
    distanceFromTo = _distanceFromTo;
    to = from + fromTo;
}

void OGLCamera::setDistanceFromToLockTo(NxReal _distanceFromTo)
{
    fromTo *= _distanceFromTo / distanceFromTo;
    distanceFromTo = _distanceFromTo;
    from = to - fromTo;
}

void OGLCamera::setModeLock(bool _modeLockUpDown, bool _modeLockLeftRight, bool _modeLockSide)
{
    modeLockUpDown = _modeLockUpDown;
    modeLockLeftRight = _modeLockLeftRight;
    modeLockSide = _modeLockSide;
}

void OGLCamera::setMinMaxCurrentUpDown(NxReal _minUpDown, NxReal _maxUpDown, NxReal _currentUpDown)
{
    minUpDown = _minUpDown;
    maxUpDown = _maxUpDown;
    currentUpDown = _currentUpDown;
}

void OGLCamera::setMinMaxCurrentLeftRight(NxReal _minLeftRight, NxReal _maxLeftRight, NxReal _currentLeftRight)
{
    minLeftRight = _minLeftRight;
    maxLeftRight = _maxLeftRight;
    currentLeftRight = _currentLeftRight;
}

void OGLCamera::setMinMaxCurrentSide(NxReal _minSide, NxReal _maxSide, NxReal _currentSide)
{
    minSide = _minSide;
    maxSide = _maxSide;
    currentSide = _currentSide;
}

void OGLCamera::setAllMinMaxCurrent(NxReal _angle)
{
    minUpDown
    = maxUpDown
      = currentUpDown
        = minLeftRight
          = maxLeftRight
            = currentLeftRight
              = minSide
                = maxSide
                  = currentSide
                    = _angle;
}

void OGLCamera::calculateStepVectors()
{
    if (m_eCameraType != FREE)
    {
        double dist = from.distance(NxVec3(0, 0, 0));
        stepLeft = left * magnitudeStepLeft * ((dist + 1) / 1000);
        stepFront = front * magnitudeStepFront * ((dist + 1) / 1000);
        stepUp = up * magnitudeStepUp * ((dist + 1) / 1000);
    }
    else
    {
        flyTo = to + m_translate;
        flyFrom = from + m_translate;
        flyFromTo = flyTo - flyFrom;
        flyUp = up;// + m_translate;
        NxVec3 elevationVector = flyUp.cross(flyFromTo);
        flyFront = NxVec3(flyTo - flyFrom, true);

        flyLeft.cross(flyUp, flyFront, true);
        flyLeft.rotate((M_PI * m_angleAz) / 180, flyUp);
        flyFront.rotate((M_PI * m_angleAz) / 180, flyUp);
        flyFront.rotate((M_PI * m_angleEl) / 180, flyLeft);

        stepLeft = flyLeft * magnitudeStepLeft;
        stepFront = flyFront * magnitudeStepFront;
        stepUp = flyUp * magnitudeStepUp;
    }
}

NxVec3 OGLCamera::displayToWorld(NxVec3 display)
{
    NxVec3 retValue;
    retValue.x = retValue.y = retValue.z = 0;

    NxVec3 retValueTmp;
    retValueTmp.x = retValueTmp.y = retValueTmp.z = 0;

    glLoadIdentity();
    GLfloat winX, winY, winZ;
    GLint viewport[4];
    GLdouble mvmatrix[16], projmatrix[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);
    glGetIntegerv(GL_VIEWPORT, viewport);
    winX = (float)display.x;
    winY = (float)display.y;
    winZ = (float)display.z;
    GLint result = gluUnProject(winX, winY, winZ, mvmatrix, projmatrix, viewport, &retValue.x, &retValue.y, &retValue.z);

    return retValue;
}

NxVec3 OGLCamera::worldToDisplay(NxVec3 world)
{
    NxVec3 retValue;
    retValue.x = retValue.y = retValue.z = 0;

    //glLoadIdentity();
    GLint viewport[4];
    GLdouble mvmatrix[16], projmatrix[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);
    glGetIntegerv(GL_VIEWPORT, viewport);
    GLint result = gluProject(world.x, world.y, world.z, mvmatrix, projmatrix, viewport, &retValue.x, &retValue.y, &retValue.z);

    return retValue;
}

void OGLCamera::getViewPort(double* vp)
{
    GLdouble viewport[4];
    glGetDoublev(GL_VIEWPORT, viewport);
    vp[0] = viewport[0];
    vp[1] = viewport[1];
    vp[2] = viewport[2];
    vp[3] = viewport[3];
}

NxVec3 OGLCamera::getCenter(int sizeX, int sizeY)
{
    NxVec3 retValue;

    GLdouble viewport[4];
    glGetDoublev(GL_VIEWPORT, viewport);

    retValue.x = ((viewport[2] + viewport[0]) / 2.0 * (double)sizeX);
    retValue.y = ((viewport[3] + viewport[1]) / 2.0 * (double)sizeY);
    return retValue;
}

void OGLCamera::zoom(double zoomFactor)
{
    if (m_eCameraType != PARALLEL)
    {
        calculateStepVectors();
        if (zoomFactor < 1)
        {
            moveFront();
        }
        else
        {
            moveBack();
        }
        calculateStepVectors();
    }
    else
    {
        m_zoomFactor *= zoomFactor;
    }
}

void OGLCamera::azimuth(NxReal _angle)
{
    m_angleAz += (_angle * 500);
    calculateStepVectors();
}

void OGLCamera::elevation(NxReal _angle)
{
    m_angleEl += (_angle * 500);
    calculateStepVectors();
}

void OGLCamera::roll(NxReal _angle)
{
    m_angleRo += (_angle * 500);
    calculateStepVectors();
}

void OGLCamera::resetZoom(int w /*= -1*/, int h /*= -1*/)
{
    if ((w <= 0) && (h <= 0))
    {
        m_zoomFactor = m_defaultZoomFactor ;
    }
    else
    {
        GLdouble viewport[4];
        glGetDoublev(GL_VIEWPORT, viewport);
        double minSizeViewport = MIN(viewport[2], viewport[3]);
        double maxSizeBoundingBox = MAX(w, h);

        if (m_eCameraType == PARALLEL)
        {
            m_zoomFactor = m_defaultZoomFactor * (maxSizeBoundingBox / 400) * (700 / minSizeViewport);
        }
        else
        {
            NxReal fromPersp[3] = {0, 1000 * (maxSizeBoundingBox / 400)* (700 / minSizeViewport)* m_defaultZoomFactor, 1000 * (maxSizeBoundingBox / 400)* (700 / minSizeViewport)* m_defaultZoomFactor};
            setFromToUp(fromPersp, to.get(), up.get());
        }
    }
    m_translate = NxVec3(0, 0, 0);
    calculateStepVectors();
}

void OGLCamera::resetTranslation()
{
    m_translate = NxVec3(0, 0, 0);
    calculateStepVectors();
}

void OGLCamera::setDefaultZoomFactor(double defaultZoomFactor)
{
    m_defaultZoomFactor = defaultZoomFactor ;
}

void OGLCamera::resetRotations()
{
    m_angleAz = 0;
    m_angleEl = 0;
    m_angleRo = 0;
    calculateStepVectors();
}

void OGLCamera::setTranslation(double x, double y, double z)
{
    m_translate = NxVec3(x, y, z);
    calculateStepVectors();
}

void OGLCamera::getTranslation(double& x, double& y, double& z)
{
    x = m_translate.x;
    y = m_translate.y;
    z = m_translate.z;
}

void OGLCamera::getPosition(double& x, double& y, double& z)
{
    x = from.x;
    y = from.y;
    z = from.z;
    if (m_angleAz != 0)
    {
        double d = from.z; //from.magnitude();
        x = d * sin(m_angleAz * M_PI / 180.0f);
        z = d * cos(m_angleAz * M_PI / 180.0f);
    }

    x += m_translate.x;
    y += m_translate.y;
    z += m_translate.z;
}
