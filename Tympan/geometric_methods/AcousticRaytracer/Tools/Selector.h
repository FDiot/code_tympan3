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

#ifndef SELECTOR_H
#define SELECTOR_H

#include <vector>
#include <algorithm>
#include <iostream>

enum SELECTOR_RESPOND
{
    SELECTOR_ACCEPT = 0,
    SELECTOR_REJECT,
    SELECTOR_REPLACE
};

enum OPERATOR
{
    LESS = 0,
    LESS_OR_EQUAL,
    EQUAL,
    GREATER_OR_EQUAL,
    GREATER
};
struct CompareToKey
{
    bool operator()(std::vector<unsigned int> list1, std::vector<unsigned int> list2) const
    {
        //On compare jusqu'a ce qu'on atteigne le bout du plus petit vecteur
        int minSize = ( list1.size() < list2.size() ) ? list1.size() : list2.size() ; // Ne fonctionnait pas avec std::min
        for (int i = 0; i < minSize; i++)
        {
            if (list1.at(i) < list2.at(i))
            {
                return true;
            }
            else if (list1.at(i) == list2.at(i))
            {
                continue;
            }
            else
            {
                return false;
            }
        }
        //Si les vecteurs ont la meme taille, c'est qu'ils sont egaux
        if (list1.size() == list2.size())
        {
            return false;
        }
        //La liste avec la plus grande taille est superieure a l'autre
        if (list1.size() < list2.size())
        {
            return true;
        }
        return false;
    }
};

/**
 * \brief Base class for Selector (used to keep or disable rays according different criterias)
 */
template<typename T>
class Selector
{

public:
	/// Base constructor
    Selector() : deletable(true) { }
    /// Destructor
    virtual ~Selector() { }
    /// Reset (clear the data) of this Selector
    virtual void reset() { return; }
    /// Copy Selector
    virtual Selector* Copy() { Selector* newSelector = new Selector(); newSelector->setIsDeletable(deletable); return newSelector; }
    /// Return true if the Selector may be deleted
    bool isDeletable() { return deletable; }
    /// Set deletable flag
    void setIsDeletable(bool _isDeletable) { deletable = _isDeletable; }
    /// Check if the ray respects the criteria of this Selector and return a SELECTOR_RESPOND
    virtual SELECTOR_RESPOND canBeInserted(T* r, unsigned long long& replace) { std::cout << "Appel du mauvais selector." << std::endl; return SELECTOR_REJECT; }
    /// Select the ray
    virtual void insert(T* r) { return; }
    /// Select the ray if it respects the criteria of this Selector
    virtual bool insertWithTest(T* r) { return true; }
	// Return the class type of the selector as a string
	virtual const char* getSelectorName(){ return typeid(this).name();}

protected:
    bool deletable; //!< Flag to know if the selector may be deleted or not
};

#endif
