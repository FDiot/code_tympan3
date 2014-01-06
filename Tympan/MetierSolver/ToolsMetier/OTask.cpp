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


#include "OTask.h"

#include "OMutexLocker.h"

OTask::OTask()
    : _running(false), _completed(false), _canceled(false)
{

}

OTask::~OTask()
{
    while (!isCompleted() && !isCanceled())
    {
        wait(this);
    }
}

bool OTask::isRunning() const
{
    TY_OMUTEXLOCKER_SHARED_MUTEX(this)
    return _running;
}

bool OTask::isCompleted() const
{
    TY_OMUTEXLOCKER_SHARED_MUTEX(this)
    return _completed;
}

bool OTask::isCanceled() const
{
    TY_OMUTEXLOCKER_SHARED_MUTEX(this)
    return _canceled;
}

void OTask::reset()
{
    TY_OMUTEXLOCKER_SHARED_MUTEX(this)
    _running = _completed = _canceled = false;
}

