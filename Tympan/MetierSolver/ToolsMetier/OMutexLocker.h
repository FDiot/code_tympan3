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

#ifndef __O_MUTEXLOCKER__
#define __O_MUTEXLOCKER__

#include "Tympan/Tools/TYConfigure.h"

/**
 * \file OMutexLocker.h
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

#endif // __O_MUTEXLOCKER__
