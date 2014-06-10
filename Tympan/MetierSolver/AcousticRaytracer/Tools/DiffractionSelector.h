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

#ifndef DIFFRACTION_SELECTOR
#define DIFFRACTION_SELECTOR

#include "Selector.h"
#include "Tympan/MetierSolver/AcousticRaytracer/global.h"

template<typename T>
class DiffractionSelector : public Selector<T>
{

public :
    DiffractionSelector(int _maxDiffractionOrder = 1, OPERATOR _op = LESS_OR_EQUAL) : Selector<T>()
    {
        maxDiffractionOrder = _maxDiffractionOrder;
        op = _op;
    }

    virtual Selector<T>* Copy()
    {
        //      logs <<"Copie d'un filtre de diffraction avec "<<maxDiffractionOrder<<" diff"<<endl;
        DiffractionSelector* newSelector = new DiffractionSelector(maxDiffractionOrder);
        newSelector->setIsDeletable(this->deletable);
        newSelector->setOperator(op);
        return newSelector;
    }

    virtual SELECTOR_RESPOND canBeInserted(T* r, unsigned long long& replace)
    {
        switch (op)
        {
            case LESS:
                if (r->getDiff() >= static_cast<unsigned int>(maxDiffractionOrder))
                {
                    return SELECTOR_REJECT;
                }
                break;
            case LESS_OR_EQUAL:
                if (r->getDiff() > static_cast<unsigned int>(maxDiffractionOrder))
                {
                    return SELECTOR_REJECT;
                }
                break;
            case EQUAL:
                if (r->getDiff() != static_cast<unsigned int>(maxDiffractionOrder))
                {
                    return SELECTOR_REJECT;
                }
                break;
            case GREATER_OR_EQUAL:
                if (r->getDiff() < static_cast<unsigned int>(maxDiffractionOrder))
                {
                    return SELECTOR_REJECT;
                }
                break;
            case GREATER:
                if (r->getDiff() <= static_cast<unsigned int>(maxDiffractionOrder))
                {
                    return SELECTOR_REJECT;
                }
                break;
        }
        return SELECTOR_ACCEPT;
    }

    virtual void insert(T* r) { return; }
    virtual bool insertWithTest(T* r)
    {
        switch (op)
        {
            case LESS:
                if (r->getDiff() >= static_cast<unsigned int>(maxDiffractionOrder))
                {
                    return false;
                }
                break;
            case LESS_OR_EQUAL:
                if (r->getDiff() > static_cast<unsigned int>(maxDiffractionOrder))
                {
                    return false;
                }
                break;
            case EQUAL:
                if (r->getDiff() != static_cast<unsigned int>(maxDiffractionOrder))
                {
                    return false;
                }
                break;
            case GREATER_OR_EQUAL:
                if (r->getDiff() < static_cast<unsigned int>(maxDiffractionOrder))
                {
                    return false;
                }
                break;
            case GREATER:
                if (r->getDiff() <= static_cast<unsigned int>(maxDiffractionOrder))
                {
                    return false;
                }
                break;
        }
        return true;
    }

    int getMaximumDiffractionOrder() { return maxDiffractionOrder; }
    void setMaximumDiffractionOrder(int _maxDiffractionOrder) { maxDiffractionOrder = _maxDiffractionOrder; }

    OPERATOR getOperator() { return op; }
    void setOperator(OPERATOR _op) { op = _op; }

protected:
    int maxDiffractionOrder;
    OPERATOR op;
};

#endif
