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


#ifndef __O_SINGLETON__
#define __O_SINGLETON__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/**
 * If you wrap a non-class with the SmartPtr class, you will receive
 * this warning. i.e.  int, short, etc...
 * It is a warning you may ignore.
 */
#pragma warning( disable : 4284 )

/**
 * This warning is generated when compiling in debug mode.
 * Debug symbols cannot be longer than 255 but when using templates
 * it is usual to have debug symbols longer than 255.
 * You may ignore this warning.
 */
#pragma warning( disable : 4786 )


#include "OSmartPtr.h"


/**
 * A Singleton Pattern class which uses template.
 * This implementation use Smart Pointer to avoid
 * memory leak.
 *
 * @author Projet_Tympan
 */
template <class T >
class OSingleton
{
public:
    ///Singleton type.
    typedef SmartPtr<T > LPT;


    /**
     * Set the single instance of type T.
     */
    static void set(T* pInstance)
    {
        _pInstance = pInstance;
    }
    /**
     * Set the single instance of type T.
     */
    static void set(LPT pInstance)
    {
        _pInstance = pInstance;
    }

    /**
     * Get the single instance of type T.
     */
    static LPT get()
    {
        if (!_pInstance)
        {
            _pInstance = new T();
        }
        return _pInstance;
    }


protected:
    OSingleton() {}
    virtual ~OSingleton() {}


private:
    OSingleton(const OSingleton& other);
    OSingleton& operator=(const OSingleton& other);

    ///The single instance.
    static LPT _pInstance;
};


// Init with NULL.
template <class T >
SmartPtr<T > OSingleton<T >::_pInstance = 0;


#endif // __O_SINGLETON__
