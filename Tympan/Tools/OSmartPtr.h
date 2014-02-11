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
 */

#ifndef __SMART_PTR_H__
#define __SMART_PTR_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
 * If you wrap a non-class with the SmartPtr class, you will receive
 * this warning. i.e.  int, short, etc...
 * It is a warning you may ignore.
 */
#ifdef _MSC_VER
#pragma warning( disable : 4284 )
#endif

/**
 * This warning is generated when compiling in debug mode.
 * Debug symbols cannot be longer than 255 but when using templates
 * it is usual to have debug symbols longer tahn 255.
 * You may ignore this warning.
 */
#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif

/**
 * This class implements the reference counting. When the
 * reference count drops down to 0 the referenced object
 * is freed.
 */
class IRefCount
{
public:
    /**
     * Default constructor.
     */
    IRefCount() : m_refCount(0) { };
    /**
     * Destructor.
     */
    virtual ~IRefCount() {}

    /**
     * Increases the reference count for this object.
     *
     * @return The number of reference for this object.
     */
    virtual int incRef() {  return m_refCount++; };

    /**
     * Decreases the reference count for this object.
     * If there is no more reference to this object,
     * this object is deleted.
     *
     * @return The number of reference for this object.
     */
    virtual int decRef()
    {
        --m_refCount;
        if (!m_refCount)
        {
            delete this;
            return 0;
        }
        return m_refCount;
    };

    /**
     * Returns the number of reference for this object.
     *
     * @return The number of reference for this object.
     */
    int getRefCount() const { return m_refCount; }


protected:
    ///The reference counter.
    int m_refCount;
};


/**
 * This is the smart pointer template. It assumes that
 * the referenced object supports a reference count interface
 * via IRefCount.
 */
template <class T> class SmartPtr
{
public:
    /**
     * Default constructor.
     */
    SmartPtr() { _pObj = 0; }

    /**
     * Constructor.
     */
    SmartPtr(T* pObj)
    {
        _pObj = pObj;
        if (_pObj != 0)
        {
            _pObj->incRef();
        }
    }

    /**
     * Copy constructor from an inherited type of T.
     */
    template <class U> SmartPtr(const SmartPtr<U>& ptr)
    {
        _pObj = ptr._pObj;
        if (_pObj != 0)
        {
            _pObj->incRef();
        }
    }

    /**
     * Copy constructor.
     */
    SmartPtr(const SmartPtr& ptr)
    {
        _pObj = ptr._pObj;
        if (_pObj != 0)
        {
            _pObj->incRef();
        }
    }

    /**
     * Destructor.
     */
    ~SmartPtr()
    {
        if (_pObj != 0)
        {
            if (!_pObj->decRef())
            {
                _pObj = 0;
            }
        }
    }

    /**
     * Cast operator.
     */
    operator T* () const { return _pObj; }

    /**
     * Cast operator.
     */
    T& operator* () { return *_pObj; }

    /**
     * Pointer operator.
     */
    T* operator-> () const { return _pObj; }

    /**
     * Copy operator from a derived type smart pointer.
     */
    template <class U> SmartPtr& operator= (const SmartPtr<U>& ptr)
    {
        if (_pObj != ptr._pObj)
        {
            if (_pObj != 0)
            {
                _pObj->decRef();
            }

            _pObj = ptr._pObj;

            if (_pObj != 0)
            {
                _pObj->incRef();
            }
        }

        return *this;
    }

    /**
     * Copy operator from a smart pointer.
     */
    SmartPtr& operator= (const SmartPtr& ptr)
    {
        *this = ptr._pObj;
        return *this;
    }

    /**
     * Copy operator from a real pointer.
     */
    SmartPtr& operator= (T* pObj)
    {
        if (pObj != 0)
        {
            pObj->incRef();
        }

        if (_pObj != 0)
        {
            _pObj->decRef();
        }

        _pObj = pObj;
        return *this;
    };

    /**
     * Equality operator with a smart pointer.
     */
    bool operator == (const SmartPtr& ptr) const
    {
        return _pObj == ptr._pObj;
    }

    /**
     * Equality operator with a real pointer.
     */
    bool operator == (T* pObj) const
    {
        return _pObj == pObj;
    }

    /**
     * Equality operator with a smart pointer.
     */
    bool operator != (const SmartPtr& ptr) const
    {
        return _pObj != ptr._pObj;
    }

    /**
     * Equality operator with a real pointer.
     */
    bool operator != (T* pObj) const
    {
        return _pObj != pObj;
    }

    /**
     * Comparaison operator with a smart pointer.
     */
    bool operator > (const SmartPtr& ptr) const
    {
        return _pObj > ptr._pObj;
    }
    /**
     * Comparaison operator with a real pointer.
     */
    bool operator > (T* pObj) const
    {
        return _pObj > pObj;
    }

    /**
     * Comparaison operator with a smart pointer.
     */
    bool operator < (const SmartPtr& ptr) const
    {
        return _pObj < ptr._pObj;
    }
    /**
     * Comparaison operator with a real pointer.
     */
    bool operator < (T* pObj) const
    {
        return _pObj < pObj;
    }

    /**
     * Gets the real pointer.
     * @return The real pointer.
     */
    T* getRealPointer() { return _pObj; }
    /**
     * Gets the real pointer.
     * @return The real pointer.
     */
    const T* getRealPointer() const { return _pObj; }

    //Members
public:
    ///The real pointer, must derived IRefCount.
    T* _pObj;
};

#endif // __SMART_PTR_H__

