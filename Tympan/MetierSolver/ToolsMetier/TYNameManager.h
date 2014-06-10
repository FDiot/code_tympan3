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

#ifndef __TYNAMEMANAGER__
#define __TYNAMEMANAGER__

#include "Tympan/Tools/OSmartPtr.h"
#include <map>
#include <string>
#include <utility>

// Smart pointer sur TYNameManager.
class TYNameManager;
typedef SmartPtr<TYNameManager> LPTYNameManager;

/**
 * \file TYNameManager.h
 * \class TYNameManager
 * \brief Generateur de nom unique.
 *
 * \author Projet_Tympan
 *
 */
class TYNameManager : public IRefCount
{
public:
    /**
     * Destructeur.
     */
    ~TYNameManager();

    /**
     * \fn static TYNameManager* get();
     * \brief Retourne l'instance singleton.
     */
    static TYNameManager* get();

    /**
     * \fn QString generateName(const char *classname);
     * \brief Retourne le nom de la classe associe a un nombre
     */
    QString generateName(const char* classname);

    /**
     * \fn void enable(bool enable) { _enable = enable; }
     * \brief Active la generation de nom
     */
    void enable(bool enable) { _enable = enable; }

    /**
     * \fn bool isEnable() const { return _enable; }
     * \brief Retourne si vrai si la generation de nom est active, faux sinon
     */
    bool isEnable() const { return _enable; }

private:
    // Instance unique.
    static LPTYNameManager _pInstance;

    // Map contenant le nom de la classe et l'id genere
    typedef std::pair<std::string, unsigned int> IDCounter;
    typedef std::map<std::string, IDCounter > IDMap;
    IDMap _idMap;

    // Flag d'activation
    bool _enable;

    /**
     * Constructeur par default.
     */
    TYNameManager();

    // Copy prohibited
    TYNameManager(const TYNameManager&);
    TYNameManager& operator=(const TYNameManager&);
};

#endif // __TYNAMEMANAGER__
