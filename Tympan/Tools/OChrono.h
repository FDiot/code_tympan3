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

#ifndef __O_CHRONO__
#define __O_CHRONO__

#include "Tympan/MetierSolver/CommonTools/Defines.h"

#if TY_PLATFORM == TY_PLATFORM_WIN32 || TY_PLATFORM == TY_PLATFORM_WIN64
#include <windows.h>

/**
 * A Class to measure execution time in milliseconds using system clock.
 *
 */
class OChronoTime
{
public:
    OChronoTime()
    {
        _time = ::timeGetTime();
    }
    unsigned long getTime() const
    {
        return _time;
    }
    /**
     * Addition.
     */
    OChronoTime operator+(const OChronoTime& other) const
    {
        return OChronoTime(getTime() + other.getTime());
    }
    /**
     * Substraction.
     */
    OChronoTime operator-(const OChronoTime& other) const
    {
        return OChronoTime(getTime() - other.getTime());
    }
private:
    OChronoTime(unsigned long time)
    {
        _time = time;
    }

    unsigned long _time;
};

#else

#include <sys/timeb.h>

/**
 * A Class to measure execution time in milliseconds using system clock.
 *
 */
class OChronoTime
{
public:
    OChronoTime()
    {

        struct timeb    time_b ;
        ftime(& time_b) ;

        _time = time_b.time * 1000; // TODO: this is not the good way to get time because overflow is possible
        _time += time_b.millitm;

    }
    unsigned long getTime() const
    {
        return _time;
    }
    /**
     * Addition.
     */
    OChronoTime operator+(const OChronoTime& other) const
    {
        return OChronoTime(getTime() + other.getTime());
    }
    /**
     * Substraction.
     */
    OChronoTime operator-(const OChronoTime& other) const
    {
        return OChronoTime(getTime() - other.getTime());
    }
private:
    OChronoTime(unsigned long time)
    {
        _time = time;
    }

    unsigned long _time;
};

#endif

#endif // __O_CHRONO__
