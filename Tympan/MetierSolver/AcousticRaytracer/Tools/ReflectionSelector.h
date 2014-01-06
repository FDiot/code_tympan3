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
 
#ifndef REFLECTION_SELECTOR
#define REFLECTION_SELECTOR

#include "Selector.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Acoustic/SpecularReflexion.h"

template<typename T>
class ReflectionSelector : public Selector<T>
{

public :
    ReflectionSelector(int _maxReflectionOrder = 1, bool _acceptGround = false, OPERATOR _op = LESS_OR_EQUAL) : Selector<T>()
    {
        maxReflectionOrder = _maxReflectionOrder;
        acceptGround = _acceptGround;
        op = _op;
    }

    virtual Selector<T>* Copy()
    {
        ReflectionSelector* newSelector = new ReflectionSelector(maxReflectionOrder, acceptGround);
        newSelector->setIsDeletable(this->deletable);
        newSelector->setOperator(op);
        return newSelector;
    }

    virtual SELECTOR_RESPOND canBeInserted(T* r, unsigned long long& replace)
    {
        switch (op)
        {
            case LESS:
                if (r->getReflex() >= static_cast<unsigned int>(maxReflectionOrder))
                {
                    return SELECTOR_REJECT;
                }
                break;
            case LESS_OR_EQUAL:
                if (r->getReflex() > static_cast<unsigned int>(maxReflectionOrder))
                {
                    return SELECTOR_REJECT;
                }
                break;
            case EQUAL:
                if (r->getReflex() != static_cast<unsigned int>(maxReflectionOrder))
                {
                    return SELECTOR_REJECT;
                }
                break;
            case GREATER_OR_EQUAL:
                if (r->getReflex() < static_cast<unsigned int>(maxReflectionOrder))
                {
                    return SELECTOR_REJECT;
                }
                break;
            case GREATER:
                if (r->getReflex() <= static_cast<unsigned int>(maxReflectionOrder))
                {
                    return SELECTOR_REJECT;
                }
                break;

        }

        // Si on accepte les reflexions sur le sol, toutes les reflexions sont bonnes
        if (acceptGround)
        {
            return SELECTOR_ACCEPT;
        }

        // sinon, on s'assure que la reflexion ne se fait pas sur le sol
        // ====================
        // DTn++ 20120511
        // ====================
        std::vector<QSharedPointer<Event> >* tabEvent = r->getEvents();
        QSharedPointer<Event> pEvent = tabEvent->back();
        SpecularReflexion* reflex = dynamic_cast<SpecularReflexion*>(pEvent.data());
        // ====================

        //      SpecularReflexion *reflex = dynamic_cast<Event*> (r->getEvents()->back());
        //      if(!reflex || (reflex && !reflex->getShape()->getMaterial()->isNatural))
        if (!reflex || (reflex && !reflex->getShape()->isSol()))
        {
            return SELECTOR_ACCEPT;
        }

        return SELECTOR_REJECT;
    }

    virtual void insert(T* r) { return; }

    virtual bool insertWithTest(T* r)
    {
        switch (op)
        {
            case LESS:
                if (r->getReflex() >= static_cast<unsigned int>(maxReflectionOrder))
                {
                    return false;
                }
                break;
            case LESS_OR_EQUAL:
                if (r->getReflex() > static_cast<unsigned int>(maxReflectionOrder))
                {
                    return false;
                }
                break;
            case EQUAL:
                if (r->getReflex() != static_cast<unsigned int>(maxReflectionOrder))
                {
                    return false;
                }
                break;
            case GREATER_OR_EQUAL:
                if (r->getReflex() < static_cast<unsigned int>(maxReflectionOrder))
                {
                    return false;
                }
                break;
            case GREATER:
                if (r->getReflex() <= static_cast<unsigned int>(maxReflectionOrder))
                {
                    return false;
                }
                break;

        }

        // Si on accepte les reflexions sur le sol, toutes les reflexions sont bonnes
        if (acceptGround)
        {
            return true;
        }

        // sinon, on s'assure que la reflexion ne se fait pas sur le sol
        // ====================
        // DTn++ 20120511
        // ====================
        std::vector<QSharedPointer<Event> >* tabEvent = r->getEvents();
        QSharedPointer<Event> pEvent = tabEvent->back();
        SpecularReflexion* reflex = dynamic_cast<SpecularReflexion*>(pEvent.data());
        // ====================

        //      Reflexion *reflex = dynamic_cast<Reflexion*>(r->getEvents()->back());
        if (!reflex || (reflex && !reflex->getShape()->getMaterial()->isNatural))
            if (!reflex || (reflex && !reflex->getShape()->isSol()))
            {
                return true;
            }
        return false;
    }

    int getMaximumReflectionOrder() { return maxReflectionOrder; }
    void setMaximumReflectionOrder(int _maxReflectionOrder) { maxReflectionOrder = _maxReflectionOrder; }

    bool isGroundAccepted() { return acceptGround; }
    void setGroundAccepted(bool _acceptGround) { acceptGround = _acceptGround; }

    OPERATOR getOperator() { return op; }
    void setOperator(OPERATOR _op) { op = _op; }

protected:
    int maxReflectionOrder;
    bool acceptGround;
    OPERATOR op;

};

#endif
