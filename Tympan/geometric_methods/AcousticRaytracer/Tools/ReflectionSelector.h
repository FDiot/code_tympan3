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
#include "Acoustic/SpecularReflexion.h"
#include "Geometry/Shape.h"

/*!
 * \brief : To disable the rays which have a number of reflection events greater than a given threshold
            or reflecting on the ground if it is disabled.
 */
template<typename T>
class ReflectionSelector : public Selector<T>
{
public :
	/// Constructor
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

        // Check if the ray is reflected on the ground if necessary
        if (!acceptGround){
			
			//loop on evets
			std::vector<std::shared_ptr<Event> >* tabEvent = r->getEvents();
			for(unsigned int i=0;i<tabEvent->size();i++){

				//check if the ith event is a reflexion on the ground
				SpecularReflexion* reflex = dynamic_cast<SpecularReflexion*>(tabEvent->at(i).get());
				if (reflex && reflex->getShape()->isSol()){
					return SELECTOR_REJECT;
				}
			}
		}
        return SELECTOR_ACCEPT;
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

        // Check if the ray is reflected on the ground if necessary
        if (!acceptGround){
			
			//loop on evets
			std::vector<std::shared_ptr<Event> >* tabEvent = r->getEvents();
			for(unsigned int i=0;i<tabEvent->size();i++){

				//check if the ith event is a reflexion on the ground
				SpecularReflexion* reflex = dynamic_cast<SpecularReflexion*>(tabEvent->at(i).get());
				if (reflex && reflex->getShape()->isSol()){
					return false;
				}
			}
		}
        return true;
    }
   
    /**
    * \brief Get the reflection maximal number
    */  
    int getMaximumReflectionOrder() { return maxReflectionOrder; }
    
    /**
    * \brief Set the reflection maximal number
    */  
    void setMaximumReflectionOrder(int _maxReflectionOrder) { maxReflectionOrder = _maxReflectionOrder; }
    
     /**
    * \brief Return true if ground reflection is accepted
    */  
    bool isGroundAccepted() { return acceptGround; }

    /**
    * \brief  Set flag acceptGround
    */  
    void setGroundAccepted(bool _acceptGround) { acceptGround = _acceptGround; }

    /**
    * \brief Get the OPERATOR of this Selector
    */   
    OPERATOR getOperator() { return op; }

    /**
    * \brief Set the OPERATOR of this Selector 
    */ 
    void setOperator(OPERATOR _op) { op = _op; }


	/**
	* \brief Return the class type of the selector
	*/
	virtual const char* getSelectorName(){
		return typeid(this).name();
	}

protected:
    int maxReflectionOrder;		//!< Reflection maximal number for this Selector criteria
    bool acceptGround;			//!< Flag to accept or not the reflection on the ground
    OPERATOR op;				//!< OPERATOR being used for the criteria (by default, LESS_OR_EQUAL)

};

#endif
