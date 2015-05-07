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

//Useful threading defines
#define TY_AUTO_MUTEX_NAME _mutex

// Activate QT threading support
#ifndef QT_THREAD_SUPPORT
#define QT_THREAD_SUPPORT 1
#endif // QT_THREAD_SUPPORT

#include <QMutex>
#include <QWaitCondition>
#include <QThread>

// Classe pour l'acces aux methodes sleep protegees de la classe QThread
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
 * \brief Class utilisee comme objet RAII (Resource Acquisition Is Initialization)
 * \author Projet_Tympan
 */

class OMutexLocker
{
public:
    // Constructeurs
    OMutexLocker(QMutex& mutex);
    OMutexLocker(const QMutex& mutex);

    // Destructeur
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
 * \brief Cette classe definie un thread specialise pour l'execution de tâche dans une collection de thread. Thread esclave pour les collections de threads
 * \author Projet_Tympan
 */
class OSlaveThread : public QThread
{
public:
    // Construit un thread esclave pour une collection de thread
    OSlaveThread(OThreadPool* pool);

    // Detruit le thread esclave; attent la fin du thread.
    ~OSlaveThread();

    bool _bToEnd;

protected:
    /// Pointeur sur la collection de thread parente
    OThreadPool* _pool;

    /**
     * \fn void run();
     * \brief Execute une tâche en attente.
     */
    void run();
};


/**
 * \file threading.h
 * \class OTask
 * \brief Tâche d'une collection de thread
 *
 * Cette classe abstraite encapsule une tâche primitive pouvant etre executee dans une collection de thread.
 * Il est necessaire de la surcharger en implementant la methode virtuelle pure OTask::main qui
 * definie la tâche a effectuer.
 * Une tâche est poussee dans la collection de thread avec la methode OThreadPool::push,
 * elle commence a s'effectuer des lors qu'un thread est disponible.
 * Une tâche ne peut pas etre annulee apres son execution.
 *
 * \author Projet_Tympan
 */
class OTask : public IRefCount, public QWaitCondition, public QMutex
{
public:
    // Constructeur par default
    OTask();

    // Destructeur : attend la fin de la tâche pour detruire
    virtual ~OTask();

    /**
     * \fn bool isRunning() const;
     * \brief Retourne true si la tâche est en cours d'execution, faux sinon
     */
    bool isRunning() const;

    /**
     * \fn bool isCompleted() const;
     * \brief Retourne true si la tâche est terminee, faux sinon
     */
    bool isCompleted() const;

    /**
     * \fn bool isCanceled() const;
     * \brief Retourne true si la tâche a ete annulee, faux sinon
     */
    bool isCanceled() const;

    /**
     * \fn virtual void main() = 0;
     * \brief Procedure principale de la tâche.
     * Cette methode virtuelle pure doit etre surchargee dans une classe derivee pour decrire la tâche a effectuer
     */
    virtual void main() = 0;

    /**
     * \fn void reset();
     * \brief Reset le statut de la tâche (_running=false et _completed=false)
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
 * \brief Collection de threads esclaves.
 *
 * Cette classe definie une collection de threads esclaves qui execute les tâches presentent dans une file.
 * Une tâche est un simple objet qui contient la methode Task::main.
 * Simple example :
 *
 *  #include "threading.h"
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
    // Construit une collection de thread et alloue "slaves" thread esclaves
    OThreadPool(unsigned int slaves);

    // Destructeur
    virtual ~OThreadPool();

    /**
     * \fn virtual void push(OTask* task);
     * \brief Ajoute une tâche dans la queue
     */
    virtual void push(OTask* task);

    /**
     * \fn unsigned int getTotalCount() const;
     * \brief Retourne le nombre total de tâche
     */
    unsigned int getTotalCount() const;

    /**
     * \fn unsigned int getCount() const;
     * \brief Retourne le compteur
     */
    unsigned int getCount() const;

    /**
     * \fn void begin(unsigned int count);
     * \brief Debut du solveur
     */
    void begin(unsigned int count);

    /**
     * \fn bool end();
     * \brief Fin du solveur
     */
    bool end();

protected:
    // Annule les tâches restantes
    void stop();

    /// Queue de tâches
    std::queue<LPOTask> _tasks;

    /// Total de tâches a effectuer
    unsigned int _totalCount;

    /// Compteur des tâches terminees
    unsigned int _counter;

    friend class OSlaveThread;
};

#endif // TY_THREADING
