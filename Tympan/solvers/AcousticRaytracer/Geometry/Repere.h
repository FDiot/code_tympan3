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

#ifndef REPERE_H
#define REPERE_H

#include "Tympan/models/common/mathlib.h"
#include "Tympan/solvers/AcousticRaytracer/Base.h"

//#ifdef USE_QT
//  #include "RepereGraphic.h"
//#endif

class Repere : public Base
{

    //#ifdef USE_QT
    //  //WIDGET_DECL(Recepteur)
    //  GRAPHIC_DECL(Repere)
    //#endif

public:

    Repere() : Base() { name = "unknown repere";};
    Repere(vec3 _U, vec3 _V, vec3 _W, vec3 _O);
    Repere(const Repere& repere);

    virtual ~Repere() { };

    void set(vec3 _U, vec3 _V, vec3 _W, vec3 _O) { U = vec3(_U); V = vec3(_V); W = vec3(_W); O = vec3(_O); updateMatrices(); }
    void setU(vec3 _U) { U = vec3(_U); updateMatrices(); }
    void setV(vec3 _V) { V = vec3(_V); updateMatrices(); }
    void setW(vec3 _W) { W = vec3(_W); updateMatrices(); }
    void setO(vec3 _O) { O = vec3(_O); updateMatrices(); }

    vec3 getU() const { return U; }
    vec3 getV() const { return V; }
    vec3 getW() const { return W; }
    vec3 getO() const { return O; }

    vec3 vectorFromLocalToGlobal(vec3& local);
    vec3 vectorFromGlobalToLocal(vec3& global);

    vec3 positionFromLocalToGlobal(vec3& local);
    vec3 positionFromGlobalToLocal(vec3& global);


protected:

    void updateMatrices();

    vec3 U;
    vec3 V;
    vec3 W;
    vec3 O;

    decimal LtoG[4][4];
    decimal GtoL[4][4];

};

#endif
