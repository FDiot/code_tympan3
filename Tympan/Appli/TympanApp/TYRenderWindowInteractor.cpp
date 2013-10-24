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
 
//Added by qt3to4:
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
/**
 * \file TYRenderWindowInteractor.cpp
 * \brief Le role de cette classe est limite a emettre des signaux pouvant etre utilise pour interagir sur le renderer avec le clavier et la souris.
 *
 *
 *
 */



#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include <qcursor.h>


TYRenderWindowInteractor::TYRenderWindowInteractor(QWidget* pParent, const char* name) :
    TYRenderWindow(pParent, name)
{
    init();
}

TYRenderWindowInteractor::TYRenderWindowInteractor(QGLFormat glf,  QWidget* pParent, const char* name) :
    TYRenderWindow(glf, pParent, name)
{
    init();
}

void TYRenderWindowInteractor::init()
{
    setMouseTracking(true);

    setCursor(Qt::CrossCursor);
}

TYRenderWindowInteractor::~TYRenderWindowInteractor()
{
}

void TYRenderWindowInteractor::mousePressEvent(QMouseEvent* pEvent)
{
    emit(mousePressed(pEvent->x(), pEvent->y(), pEvent->button(), pEvent->modifiers()));
}

void TYRenderWindowInteractor::mouseReleaseEvent(QMouseEvent* pEvent)
{
    emit(mouseReleased(pEvent->x(), pEvent->y(), pEvent->button(), pEvent->modifiers()));
}

void TYRenderWindowInteractor::mouseDoubleClickEvent(QMouseEvent* pEvent)
{
    emit(mouseDoubleClicked(pEvent->x(), pEvent->y(), pEvent->button(), pEvent->modifiers()));
}

void TYRenderWindowInteractor::mouseMoveEvent(QMouseEvent* pEvent)
{
    emit(mouseMoved(pEvent->x(), pEvent->y(), pEvent->buttons(), pEvent->modifiers()));
}

void TYRenderWindowInteractor::wheelEvent(QWheelEvent* pEvent)
{
    if (hasFocus())
    {
        emit(wheeled(pEvent->x(), pEvent->y(), pEvent->delta(), pEvent->modifiers()));
    }
}

void TYRenderWindowInteractor::keyPressEvent(QKeyEvent* pEvent)
{
    if (pEvent->isAutoRepeat())
    {
        //pEvent->ignore();
    }
    else
    {
        emit(keyPressed(pEvent->key()));
        //  pEvent->accept();
    }
}

void TYRenderWindowInteractor::keyReleaseEvent(QKeyEvent* pEvent)
{
    if (pEvent->isAutoRepeat())
    {
        //pEvent->ignore();
    }
    else
    {
        emit(keyReleased(pEvent->key()));
        //  pEvent->accept();
    }
}
