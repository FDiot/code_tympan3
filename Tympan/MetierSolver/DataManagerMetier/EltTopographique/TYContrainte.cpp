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

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "TYContrainte.h"

TYContrainte::TYContrainte(const TYContrainte& other)
{
    for (unsigned int i = 0; i < other.pointsAltimetrie.size(); i++)
    {
        pointsAltimetrie.push_back(other.pointsAltimetrie.at(i));
    }
    for (unsigned int i = 0; i < other.enveloppes.size(); i++)
    {
        TYTabPoint points;
        for (unsigned int j = 0; j < other.enveloppes.at(i).size(); j++)
        {
            points.push_back(other.enveloppes.at(i).at(j));
        }
        enveloppes.push_back(points);
    }
}
