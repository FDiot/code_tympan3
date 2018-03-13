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

#ifndef FACE_SELECTOR
#define FACE_SELECTOR

#include "Selector.h"
#include <map>
#include <vector>


enum TYPEHISTORY
{
    HISTORY_FACE = 0,
    HISTORY_PRIMITIVE,
    HISTORY_BUILDING
};

/*!
 * \brief : To keep only one from two or more rays which have the same history (events on the same primitive)
 */
template<typename T>
class FaceSelector : public Selector<T>
{
public:
	/// Constructor
    FaceSelector(TYPEHISTORY _modeHistory = HISTORY_FACE) : Selector<T>() { modeHistory = _modeHistory; }
    /// Destructor
    virtual ~FaceSelector() {}

    virtual Selector<T>* Copy()
    {
        FaceSelector* newSelector = new FaceSelector(modeHistory);
        newSelector->setIsDeletable(this->deletable);
        return newSelector;
    }
    virtual void reset() { selectedPath.clear(); return; }

    /**
    * \brief Get the TYPEHISTORY of this Selector
    */  
    TYPEHISTORY getModeHistory() { return modeHistory; }

    /**
    * \brief Set the TYPEHISTORY of this Selector
    */  
    void setModeHistory(TYPEHISTORY _modeHistory)
    {
        if (modeHistory != _modeHistory)
        {
            modeHistory = _modeHistory;
            selectedPath.clear();
        }
    }

    virtual SELECTOR_RESPOND canBeInserted(T* r, unsigned long long& replace)
    {
        typename std::map<std::vector<unsigned int>, T*, CompareToKey>::iterator it;
        std::vector<unsigned int> path;

        //Get the history according to the TYPEHISTORY selected
        switch (modeHistory)
        {
            case HISTORY_FACE :
                path = r->getFaceHistory();
                break;
            case HISTORY_PRIMITIVE :
                path = r->getPrimitiveHistory();
                break;
            default:
                path = r->getFaceHistory();
                break;
        }

		it = selectedPath.find(path); // search for an equivalent history among the histories of already selected rays

        // if there already is a ray with the same history
		if (it != selectedPath.end())
        {
            r->computeLongueur();
            double currentDistance = r->getLongueur();
            // if current ray has a shorter length than the already selected one
            if (currentDistance < it->second->getLongueur())
            {
                //replace the older ray by the new one
                replace = it->second->getConstructId();
                return SELECTOR_REPLACE;
            }
            else
            {     
                return SELECTOR_REJECT;
            }

        }
        return SELECTOR_ACCEPT;
    }

    virtual void insert(T* r)
    {
        typename std::map<std::vector<unsigned int>, T*, CompareToKey>::iterator it;
        std::vector<unsigned int> path;
        switch (modeHistory)
        {
            case HISTORY_FACE :
                path = r->getFaceHistory();
                break;
            case HISTORY_PRIMITIVE :
                path = r->getPrimitiveHistory();
                break;
            default:
                path = r->getFaceHistory();
                break;
        }

       
        it = selectedPath.find(path); // search for an equivalent history among the histories of already selected rays
        r->computeLongueur();

        // if there already is a ray with the same history
        if (it != selectedPath.end()) 
        {
            it->second = r;

            return;
        }
        else
        {
            // if none of the already selected rays has the same history than r, then add the ray and its hsitory to selectedPath
            selectedPath.insert(std::pair<std::vector<unsigned int>, T*>(path, r));
        }

		return ;
    }

    virtual bool insertWithTest(T* r)
    {
        typename std::map<std::vector<unsigned int>, T*, CompareToKey>::iterator it;
        std::vector<unsigned int> path;
        switch (modeHistory)
        {
            case HISTORY_FACE :
                path = r->getFaceHistory();
                break;
            case HISTORY_PRIMITIVE :
                path = r->getPrimitiveHistory();
                break;
            default:
                path = r->getFaceHistory();
                break;
        }

		it = selectedPath.find(path); // search for an equivalent history among the histories of already selected rays
        r->computeLongueur();
        double currentDistance = r->getLongueur();

        // if there already is a ray with the same history
        if (it != selectedPath.end())
        {
            if (currentDistance < it->second->getLongueur())
            {
                it->second = r;
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            // if none of the already selected rays has the same history than r, then add the ray and its hsitory to selectedPath
            selectedPath.insert(std::pair<std::vector<unsigned int>, T*>(path, r));
            return true;
        }
    }

	/**
	* \brief Return the class type of the selector
	*/
	virtual const char* getSelectorName(){
		return typeid(this).name();
	}

protected:
    std::map<std::vector<unsigned int>, T*, CompareToKey> selectedPath; //!< Histories of all selected rays so far
    TYPEHISTORY modeHistory;	//!< TYPEHISTORY used by this Selector (by default, HISTORY_FACE)
};

#endif
