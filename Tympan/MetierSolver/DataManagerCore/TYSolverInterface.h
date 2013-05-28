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

#ifndef __TY_SOLVERINTERFACE__
#define __TY_SOLVERINTERFACE__

class TYCalcul;
class TYSiteNode;

class TYSolverInterface
{
public:
    TYSolverInterface() { }
    virtual ~TYSolverInterface() { }

    virtual bool solve(const TYSiteNode& site, TYCalcul& calcul) = 0;

    virtual void purge() { }

protected:
    //virtual void createFaceSelector() = 0;
    //virtual void createAcousticPathFinder() = 0;
    //virtual void createAcousticModel() = 0;
};

#endif // __TY_SOLVERINTERFACE__
