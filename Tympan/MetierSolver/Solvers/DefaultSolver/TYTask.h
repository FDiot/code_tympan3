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

#ifndef __TY_TASK__
#define __TY_TASK__

#include "Tympan/MetierSolver/ToolsMetier/OTask.h"

class TYSolver;
class TYTrajet;

/**
 * Tâche d'une collection de thread pour le solveur Tympan
 *
 * @author Projet_Tympan
 */
class TYTask : public OTask
{
public:
    // Constructeur
    TYTask(TYSolver& solver, TYTrajet& trajet, int nNbTrajets);

    // Destructeur
    ~TYTask();

    // Procedure principale de la tâche (Cf. Tympan/MetierSolver/ToolsMetier/OTask.h)
    void main();

private:
    // Reference sur le solver
    TYSolver& _solver;

    // Reference sur le trajet
    TYTrajet& _trajet;

    // Numero de trajet
    unsigned int _nNbTrajets;

    // Tableau des intersections
    TYSIntersection* _tabIntersect;
};

///Smart Pointer sur TYTask.
typedef SmartPtr<TYTask> LPTYTask;

#endif // __TY_TASK__
