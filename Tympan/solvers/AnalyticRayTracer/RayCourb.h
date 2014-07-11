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

#ifndef __RAYCOURB_H
#define __RAYCOURB_H

#include <vector>
#include <map>
#include "Tympan/models/common/mathlib.h"
#include "Step.h"

using namespace std;

class RayCourb
{
public:
    RayCourb();
    RayCourb(const vec3& a);
    RayCourb(const RayCourb& r);

    ~RayCourb() {}

    /*!
     * \fn void purge()
     * \brief Nettoie les tableaux en desallouant proprement les pointeurs
     */
    void purge();

    /*!
    * \fn void setSize(const unsigned int taille)
    * \brief Reservation de la taille des vecteurs
    */
    void setSize(const unsigned int taille) { etapes.reserve(taille); }

    /*!
    * \fn RayCourb& operator = (RayCourb & P)
    * \brief Operateur d'affectation
    */
    RayCourb& operator= (const RayCourb& P);

public:
    // donnees membres
    vector<Step> etapes;                         /*!< vecteur des pas de temps calculés */
    int nbReflex;                                /*!< nombre de reflexions */
    vector<int> position;                        /*!< liste des indices des points ou il y a une reflexion (le numero du pas de temps) */
    map<int, int> rencontre;                     /*!< couple (pas de temps, numero de la face rencontree) */
};
#endif //__RAYCOURB_H

