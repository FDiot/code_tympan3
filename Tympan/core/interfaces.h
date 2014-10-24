/*
 * Copyright (C) <2012-2014> <EDF-R&D> <FRANCE>
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

#ifndef TY_INTERFACES
#define TY_INTERFACES

namespace tympan {
    class AcousticProblemModel;
    class AcousticResultModel;
} // namespace tympan

class SolverInterface
{
public:
    SolverInterface() { }
    virtual ~SolverInterface() { }

    virtual bool solve(const tympan::AcousticProblemModel& aproblem,
                       tympan::AcousticResultModel& aresult) = 0;

    virtual void purge() { }
};


class AcousticModelInterface
{
public:
    AcousticModelInterface() { }
    virtual ~AcousticModelInterface() { }
};


#endif // TY_INTERFACES
