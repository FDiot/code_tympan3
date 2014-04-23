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

#ifndef __O_THREADPOOL__
#define __O_THREADPOOL__


#include <queue>
#include <vector>

#include "ThreadingSupport.h"
#include "OTask.h"

class OSlaveThread;

/**
 * \file OThreadPool.h
 * \class OThreadPool
 * \brief Collection de threads esclaves.
 *
 * Cette classe definie une collection de threads esclaves qui execute les tâches presentent dans une file.
 * Une tâche est un simple objet qui contient la methode Task::main.
 * Simple example :
 *
 *  #include "OTask.h"
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
 *  #include "OThreadPool.h"
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

#endif // __O_THREADPOOL__
