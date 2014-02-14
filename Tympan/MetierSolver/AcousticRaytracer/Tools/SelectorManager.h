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

#ifndef SELECTOR_MANAGER_H
#define SELECTOR_MANAGER_H

#include "Selector.h"
#include <vector>
#include <QMutex>
#include <QMutexLocker>



template<typename T>
class SelectorManager
{

public:
    SelectorManager() : deletable(true) { mutex = new QMutex;}

    SelectorManager(const SelectorManager<T>& manager)
    {
        deletable = manager.deletable;
        mutex = new QMutex;
        for (unsigned int i = 0; i < manager.selectors.size(); i++)
        {
            selectors.push_back(manager.selectors.at(i)->Copy());
        }

    }

    virtual ~SelectorManager() { delete mutex; }


    void setDeletable(bool _isDeletable) { deletable = _isDeletable; }
    bool isDeletable() { return deletable; }

    void addSelector(Selector<T> *selector) { selectors.push_back(selector); }
    std::vector<Selector<T>*>& getSelectors() const { return selectors; }

    void reset()
    {
        for (unsigned int i = 0; i < selectors.size(); i++)
        {
            selectors.at(i)->reset();
        }
        selectors.clear();

        if (!isDeletable)
        {
            selectedData.clear();
        }
        else
        {
            while (!selectedData.empty())
            {
                T* data = selectedData.back();
                selectedData.pop_back();
                delete data;
            }
            selectedData.clear();
        }
    }

    bool appendData(T* data)
    {
        //QMutexLocker locker(mutex);
        vector<unsigned long long> dataToReplace;
        unsigned long long oldData;
        for (unsigned int i = 0; i < selectors.size(); i++)
        {
            switch (selectors.at(i)->canBeInserted(data, oldData))
            {
                case SELECTOR_REJECT:
                    if (deletable)
                    {
                        ;//delete data; // DTn-- 20120514 : Les donnees seron detruites ulterieurement.
                    }
                    else
                    {
                        rejectedData.insert(pair<unsigned long long, T*>(data->constructId, data));
                    }
                    return false;
                    break;
                case SELECTOR_REPLACE:
                    dataToReplace.push_back(oldData);
                    break;
            }
        }

        //cout << "Tous les solveurs (" << selectors.size() << ") ont ete passe avec succes." << endl;
        //Tous les filtres sont passes, le rayon est valide
        //On commence par deplacer/supprimer les rayons a remplacer
        for (unsigned int i = 0; i < dataToReplace.size(); i++)
        {
            typename std::map<unsigned long long, T*>::iterator it = selectedData.find(dataToReplace.at(i));
            it = selectedData.find(dataToReplace.at(i));
            if (it != selectedData.end())
            {
                T* previousData = it->second;
                selectedData.erase(it);
                //cout << "Le rayon " << previousData->constructId << " est supprime ou deplace." << endl;
                if (deletable)
                {
                    delete previousData;
                }
                else
                {
                    rejectedData.insert(pair<unsigned long long, T*>(previousData->constructId, previousData));
                }
            }
        }

        //Dans chaque filtre, on rajoute le rayon necessaire si un filtre a besoin des rayons precedemments acceptes.
        for (unsigned int i = 0; i < selectors.size(); i++)
        {
            selectors.at(i)->insert(data);
        }

        //Enfin, on rajoute le rayon dans la liste des rayons valides par le filtre
        //std::cout<<"Insertion de l'element "<<data->constructId<<std::endl;
        selectedData.insert(pair<unsigned long long, T*>(data->constructId, data));
        //cout << "Insertion du rayon dans chaque solver passe avec succes." << endl;
        return true;
    }

    std::map<unsigned long long, T*>& getSelectedData() { return selectedData; }

protected:
    bool deletable;
    std::vector<Selector<T>*> selectors;

    std::map<unsigned long long, T*> selectedData;
    std::map<unsigned long long, T*> rejectedData;

    QMutex* mutex;

};

#endif
