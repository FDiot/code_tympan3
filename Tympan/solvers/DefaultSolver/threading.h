/*
 * Copyright (C) <2012-2014> <EDF-R&D> <FRANCE>
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

#ifndef TY_THREADING
#define TY_THREADING


#include <queue>
#include <vector>

#include "Tympan/core/smartptr.h"

// Useful threading defines
#define TY_AUTO_MUTEX_NAME _mutex

// Activate QT threading support
#ifndef QT_THREAD_SUPPORT
#define QT_THREAD_SUPPORT 1
#endif // QT_THREAD_SUPPORT

#include <QMutex>
#include <QWaitCondition>
#include <QThread>

/**
 * \class OSleeper
 * \brief Access to sleep protected methods of the QThread class
 */
class OSleeper : public QThread
{
public:
    static void sleep(unsigned long secs) { return QThread::sleep(secs); }
    static void msleep(unsigned long msecs) { return QThread::msleep(msecs); }
    static void usleep(unsigned long usecs) { return QThread::usleep(usecs); }
};

///Auto mutex
#define TY_AUTO_MUTEX mutable QMutex TY_AUTO_MUTEX_NAME;
#define TY_LOCK_AUTO_MUTEX TY_AUTO_MUTEX_NAME.lock();
#define TY_UNLOCK_AUTO_MUTEX TY_AUTO_MUTEX_NAME.unlock();
#define TY_OMUTEXLOCKER_AUTO_MUTEX OMutexLocker locker(TY_AUTO_MUTEX_NAME);
///Named mutex
#define TY_MUTEX(name) mutable QMutex name;
#define TY_LOCK_MUTEX(name) name.lock();
#define TY_UNLOCK_MUTEX(name) name.unlock();
#define TY_OMUTEXLOCKER_MUTEX(name) OMutexLocker locker(name);
///Named static mutex
#define TY_STATIC_MUTEX(name) static QMutex name;
#define TY_STATIC_MUTEX_INSTANCE(name) QMutex name;
///Auto shared mutex
#define TY_AUTO_SHARED_MUTEX mutable QMutex *TY_AUTO_MUTEX_NAME;
#define TY_NEW_AUTO_SHARED_MUTEX TY_AUTO_MUTEX_NAME = new QMutex();
#define TY_DELETE_AUTO_SHARED_MUTEX delete TY_AUTO_MUTEX_NAME;
#define TY_LOCK_AUTO_SHARED_MUTEX TY_AUTO_MUTEX_NAME->lock();
#define TY_UNLOCK_AUTO_SHARED_MUTEX TY_AUTO_MUTEX_NAME->unlock();
#define TY_OMUTEXLOCKER_AUTO_SHARED_MUTEX OMutexLocker locker(*TY_AUTO_MUTEX_NAME);
///Named shared mutex
#define TY_SHARED_MUTEX(name) mutable QMutex *name;
#define TY_NEW_SHARED_MUTEX(name) name = new QMutex();
#define TY_DELETE_SHARED_MUTEX(name) delete name;
#define TY_LOCK_SHARED_MUTEX(name) name->lock();
#define TY_UNLOCK_SHARED_MUTEX(name) name->unlock();
#define TY_OMUTEXLOCKER_SHARED_MUTEX(name) OMutexLocker locker(*name);


/**
 * \file threading.h
 * \class OMutexLocker
 * \brief Class used as RAII object (Resource Acquisition Is Initialization)
 * \author Projet_Tympan
 */

class OMutexLocker
{
public:
    /// Constructors
    OMutexLocker(QMutex& mutex);
    OMutexLocker(const QMutex& mutex);

    /// Destructor
    ~OMutexLocker();

private:
    QMutex* _mutex;
};

inline OMutexLocker::OMutexLocker(QMutex& mutex)
{
    _mutex = &mutex;
    TY_LOCK_SHARED_MUTEX(_mutex)
}

inline OMutexLocker::OMutexLocker(const QMutex& mutex)
{
    _mutex = const_cast<QMutex*>(&mutex);
    TY_LOCK_SHARED_MUTEX(_mutex)
}

inline OMutexLocker::~OMutexLocker()
{
    TY_UNLOCK_SHARED_MUTEX(_mutex)
}


class OThreadPool;

/**
 * \file threading.h
 * \class OSlaveThread
 * \brief This class defines a thread for running tasks in a threads collection. Slave thread for the threads collection.
 * \author Projet_Tympan
 */
class OSlaveThread : public QThread
{
public:
    /// Build a slave thread for a threads collection
    OSlaveThread(OThreadPool* pool);

    /// Destroy the slave thread; wait for the end of the thread.
    ~OSlaveThread();

    bool _bToEnd;

protected:
    /// Pointer on the parent threads collection
    OThreadPool* _pool;

    /**
     * \fn void run();
     * \brief Run a waiting task.
     */
    void run();
};


/**
 * \file threading.h
 * \class OTask
 * \brief Task of a threads collection
 *
 * This abstract class encapsulates a primitive task which can be run in a threads collections.
 * It is necessary to overload it by the implementation of a pure virtual methode OTask::main
 * which defines the task to run.
 * A task is pushed into the threads collection with the OThreadPool::push method,
 * which starts as soon as a thread is available.
 * A task can't be cancelled after its run.
 *
 * \author Projet_Tympan
 */
class OTask : public IRefCount, public QWaitCondition, public QMutex
{
public:
    /// Default constructor
    OTask();

    /// Destructor : waits for the end of the task to destroy it
    virtual ~OTask();

    /**
     * \fn bool isRunning() const;
     * \brief Return true if the task is running, false otherwise
     */
    bool isRunning() const;

    /**
     * \fn bool isCompleted() const;
     * \brief Return true if the task is completed, false otherwise
     */
    bool isCompleted() const;

    /**
     * \fn bool isCanceled() const;
     * \brief Return true if the task has been cancelled, false otherwise
     */
    bool isCanceled() const;

    /**
     * \fn virtual void main() = 0;
     * \brief Main routine of the task.
     * This pure virtual method should be overloaded into an inherited class to describe the task to do.
     */
    virtual void main() = 0;

    /**
     * \fn void reset();
     * \brief Reset the task status (_running=false and _completed=false)
     */
    void reset();

protected:
    /// Running flag.
    bool _running;

    /// Completed flag.
    bool _completed;

    /// Cancel flag.
    bool _canceled;

    friend class OSlaveThread;
    friend class OThreadPool;
};

///Smart Pointer sur OTask.
typedef SmartPtr<OTask> LPOTask;


/**
 * \file threading.h
 * \class OThreadPool
 * \brief Slave threads collection.
 *
 * This class define a slave threads collection which run tasks pending in a queue.
 * A task is a simple object which contains the Task::main method.
 * Simple example :
 *
 *  \#include "threading.h"
 *
 *  class MyTask : public OTask
 *  {
 *  public:
 *      void main(void)
 *      {
 *          std::cout << "Hello world task!" << std::endl;
 *      }
 *  };
 *
 *
 *  int main(int argc, char** argv)
 *  {
 *      OThreadPool pool(10);
 *      MyTask task;
 *      pool.push(task);
 *      ...
 *      task.wait();
 *      return 0;
 *  }
 *
 * \author Projet_Tympan
 */

class OThreadPool : public std::vector<OSlaveThread*>, public QWaitCondition, public QMutex
{
public:
    /// Build a threads collection and allocate "slaves" thread
    OThreadPool(unsigned int slaves);

    /// Destructor
    virtual ~OThreadPool();

    /**
     * \fn virtual void push(OTask* task);
     * \brief Add a task to the queue
     */
    virtual void push(OTask* task);

    /**
     * \fn unsigned int getTotalCount() const;
     * \brief Return the total number of tasks
     */
    unsigned int getTotalCount() const;

    /**
     * \fn unsigned int getCount() const;
     * \brief Return the counter
     */
    unsigned int getCount() const;

    /**
     * \fn void begin(unsigned int count);
     * \brief Begin solver
     */
    void begin(unsigned int count);

    /**
     * \fn bool end();
     * \brief End solver
     */
    bool end();

protected:
    /// Cancel the pending tasks
    void stop();

    /// Tasks queue
    std::queue<LPOTask> _tasks;

    /// Total number of tasks to run
    unsigned int _totalCount;

    /// Total number of ended tasks
    unsigned int _counter;

    friend class OSlaveThread;
};

#endif // TY_THREADING
