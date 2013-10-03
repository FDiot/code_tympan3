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

#ifndef __O_SLAVETHREAD__
#define __O_SLAVETHREAD__

#include "Tympan/Tools/TYConfigure.h"

class OThreadPool;

/**
 * \file OSlaveThread.h
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

#endif // __O_SLAVETHREAD__
