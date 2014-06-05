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


#include "OThreadPool.h"

#include "OMutexLocker.h"
#include "OSlaveThread.h"
#include "OTask.h"
#include "Tympan/Tools/TYProgressManager.h"
#include <QThread>

OThreadPool::OThreadPool(unsigned int slaves)
    : _totalCount(0), _counter(0)
{
    // Allocate slave threads.
    for (unsigned int i = 0 ; i < slaves ; ++i)
    {
        OSlaveThread* thread = new OSlaveThread(this);
        push_back(thread);
    }
}

OThreadPool::~OThreadPool()
{
    TY_LOCK_SHARED_MUTEX(this)

    // Wait for queue to become empty.
    while (!_tasks.empty())
    {
        // Wait for last task in queue to at least start running.
        LPOTask task = _tasks.back();
        task->wait(this);
    }

    // Now terminate all threads.
    unsigned int i = 0;
    for (i = 0 ; i < size() ; ++i)
    {
        (*this)[i]->_bToEnd = true;
    }

    // Signal them to wake up.
    wakeAll();
    TY_UNLOCK_SHARED_MUTEX(this)

    // Then delete them (the thread destructor will wait for thread completion).
    for (i = 0 ; i < size() ; ++i)
    {
        delete(*this)[i];
    }
}


void OThreadPool::push(OTask* task)
{
    // Reset task flags.
    task->reset();

    // Push task onto queue and signal availability.
    TY_LOCK_SHARED_MUTEX(this)
    _tasks.push(task);
    wakeOne();
    TY_UNLOCK_SHARED_MUTEX(this)
}

unsigned int OThreadPool::getTotalCount() const
{
    TY_OMUTEXLOCKER_SHARED_MUTEX(this);
    return _totalCount;
}

unsigned int OThreadPool::getCount() const
{
    TY_OMUTEXLOCKER_SHARED_MUTEX(this);
    return _counter;
}

void OThreadPool::begin(unsigned int count)
{
    TY_LOCK_SHARED_MUTEX(this);
    _totalCount = count;
    _counter = 0;
    TY_UNLOCK_SHARED_MUTEX(this);
}

bool OThreadPool::end()
{
    unsigned int totalCount = getTotalCount();

#if TY_USE_IHM
    TYProgressManager::setMessage("Calcul des trajets");
    TYProgressManager::set(totalCount + 1);
    TYProgressManager::step();
#endif // TY_USE_IHM

    int last = 0;
    bool cancel = false;
    while (last < totalCount)
    {
        int current = getCount();
        for (size_t i = 0; i < current - last; ++i)
        {
#if TY_USE_IHM
            TYProgressManager::step(cancel);
#endif // TY_USE_IHM
            if (cancel)
            {
                stop();
#if TY_USE_IHM
                TYProgressManager::stepToEnd();
#endif // TY_USE_IHM
                return false;
            }
        }
        last = current;
    }
#if TY_USE_IHM
    TYProgressManager::stepToEnd();
#endif // TY_USE_IHM

    return true;
}

void OThreadPool::stop()
{
    TY_LOCK_SHARED_MUTEX(this);

    // For each task
    for (size_t i = 0; i < _tasks.size(); ++i)
    {
        // Dequeue next task
        LPOTask task = _tasks.front();
        _tasks.pop();

        // Cancel task
        TY_LOCK_SHARED_MUTEX(task)
        task->_canceled = true;
        task->wakeAll();
        TY_UNLOCK_SHARED_MUTEX(task)

        // Increment counter
        _counter++;
    }

    TY_UNLOCK_SHARED_MUTEX(this);

    // Waiting for thread termination
    while (getCount() < getTotalCount())
    {
        OSleeper::msleep(1);
    }
}
