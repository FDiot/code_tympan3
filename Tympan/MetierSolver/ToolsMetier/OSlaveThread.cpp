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


#include "OSlaveThread.h"

#include "OTask.h"
#include "OThreadPool.h"

OSlaveThread::OSlaveThread(OThreadPool* pool)
    : _pool(pool)
{
    _bToEnd = false;
    start();
}

OSlaveThread::~OSlaveThread()
{
    wait();
}

void OSlaveThread::run()
{
    while (isRunning() && !_bToEnd)
    {
        // Wait for available task.
        TY_LOCK_SHARED_MUTEX(_pool)
        while (_pool->_tasks.empty() && isRunning() && !_bToEnd)
        {
            //Maintenant (QT 4) wait unlock (atomique) le mutex et relock (atomique) le mutex
            _pool->wait(_pool);
        }

        if (isRunning() && !_bToEnd)
        {
            // Dequeue next task.
            LPOTask task = _pool->_tasks.front();
            _pool->_tasks.pop();

            TY_UNLOCK_SHARED_MUTEX(_pool)

            // Signal all that task is running.
            TY_LOCK_SHARED_MUTEX(task)
            task->_running = true;
            task->wakeAll();
            TY_UNLOCK_SHARED_MUTEX(task)

            // Run task.
            task->main();

            // Signal all that task is completed.
            TY_LOCK_SHARED_MUTEX(task)
            task->_running = false;
            task->_completed = true;
            task->wakeAll();
            TY_UNLOCK_SHARED_MUTEX(task)

            // Increment pool counter
            TY_LOCK_SHARED_MUTEX(_pool)
            _pool->_counter++;
            TY_UNLOCK_SHARED_MUTEX(_pool)
        }
        else
            TY_UNLOCK_SHARED_MUTEX(_pool)
        }
}
