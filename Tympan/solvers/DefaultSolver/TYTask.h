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

#ifndef __TY_TASK__
#define __TY_TASK__

#include <deque>
#include "threading.h"

class TYSolver;
class TYTrajet;
class nodes_pool_t;
class triangle_pool_t;
class material_pool_t;

/**
 * \brief Task of a thread collection for Tympan
 */
class TYTask : public OTask
{
public:
    /**
     * \brief Constructor
     * \param solver Default solver object
     * \param nodes  Nodes
     * \param triangles Triangles
     * \param materials Materials
     * \param trajet Path
     * \param nNbTrajets Path number
     */
    TYTask(TYSolver& solver, const tympan::nodes_pool_t& nodes, const tympan::triangle_pool_t& triangles, const tympan::material_pool_t& materials, TYTrajet& trajet, int nNbTrajets);

    ~TYTask(); //!< Destructor

    void main(); //!< Main procedure to run the task

private:

    TYSolver& _solver; //!< Reference to the solver


    TYTrajet& _trajet; //!< Reference to the path


    unsigned int _nNbTrajets;  //!< Path number


    std::deque<TYSIntersection> _tabIntersect; //!< Array of intersections

    const tympan::nodes_pool_t& _nodes;
    const tympan::triangle_pool_t& _triangles;
    const tympan::material_pool_t& _materials;
};

typedef SmartPtr<TYTask> LPTYTask; //!< Smart Pointer on TYTask.

#endif // __TY_TASK__
