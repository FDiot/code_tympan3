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

#include <QMutex>
#include <QMutexLocker>
#include "Selector.h"
#include <map>
#include<vector>


enum TYPEHISTORY
{
    HISTORY_FACE = 0,
    HISTORY_PRIMITIVE,
    HISTORY_BUILDING
};

template<typename T>
class FaceSelector : public Selector<T>
{

public:
    FaceSelector(TYPEHISTORY _modeHistory = HISTORY_FACE) : Selector<T>() { modeHistory = _modeHistory; }
    virtual ~FaceSelector() {}

    virtual Selector<T>* Copy()
    {
        FaceSelector* newSelector = new FaceSelector(modeHistory);
        newSelector->setIsDeletable(this->deletable);
        return newSelector;
    }
    virtual void reset() { selectedPath.clear(); return; }

    TYPEHISTORY getModeHistory() { return modeHistory; }
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
        typename std::map<vector<unsigned int>, T*, CompareToKey>::iterator it;
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

		it = selectedPath.find(path);

		if (it != selectedPath.end())
        {
            r->computeLongueur();
            double currentDistance = r->getLongueur();
            if (currentDistance < it->second->getLongueur())
            {
                replace = it->second->constructId;
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
        typename std::map<vector<unsigned int>, T*, CompareToKey>::iterator it;
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

        it = selectedPath.find(path);
        r->computeLongueur();

        if (it != selectedPath.end()) //Il y avait deja un rayon avec le meme historique
        {
            it->second = r;

            return;
        }
        else
        {
            selectedPath.insert(std::pair<vector<unsigned int>, T*>(path, r));
        }

		return ;
    }

    virtual bool insertWithTest(T* r)
    {
        typename std::map<vector<unsigned int>, T*, CompareToKey>::iterator it;
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

		it = selectedPath.find(path);
        r->computeLongueur();
        double currentDistance = r->getLongueur();

        if (it != selectedPath.end()) //Il y avait deja un rayon avec le meme historique
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
            selectedPath.insert(std::pair<vector<unsigned int>, T*>(path, r));
            return true;
        }
    }

protected:
    std::map<std::vector<unsigned int>, T*, CompareToKey> selectedPath;
    TYPEHISTORY modeHistory;
};

#endif
