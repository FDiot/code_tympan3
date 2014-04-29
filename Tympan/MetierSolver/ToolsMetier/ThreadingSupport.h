#ifndef __THREADING_SUPPORT__
#define __THREADING_SUPPORT__

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


#endif // __THREADING_SUPPORT__
