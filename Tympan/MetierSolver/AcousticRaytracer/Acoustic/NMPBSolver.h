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

#ifndef NMPBSOLVER_H
#define NMPBSOLVER_H

#include "Solver.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/SelectorManager.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/TargetManager.h"

class NMPBSolver : public Solver
{

public:

    virtual void initGlobalValues();

    virtual bool postTreatmentScene(Scene* scene, std::vector<Source>& sources, std::vector<Recepteur>& recepteurs);

    virtual bool valideIntersection(Ray* r, Intersection* inter);

    virtual bool invalidRayon(Ray* r);

    virtual bool valideRayon(Ray* r);

    virtual void finish();

protected:
    SelectorManager<Ray> selectorManagerIntersection;
    SelectorManager<Ray> selectorManagerValidation;
    TargetManager targetManager;
};

#endif
