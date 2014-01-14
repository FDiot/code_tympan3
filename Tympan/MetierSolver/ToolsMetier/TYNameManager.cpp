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


#include "Tympan/Tools/OLocalizator.h"
#include "TYNameManager.h"
#include <sstream>

// Initialise a NULL.
LPTYNameManager TYNameManager::_pInstance = 0;

TYNameManager::TYNameManager()
    : _enable(true)
{

}

TYNameManager::~TYNameManager()
{
    _idMap.clear();
}

TYNameManager* TYNameManager::get()
{
    if (!_pInstance)
    {
        _pInstance = new TYNameManager();
    }

    return _pInstance;
}

QString TYNameManager::generateName(const char* classname)
{
    if (_enable)
    {
        if (OLocalizator::isRessourceFile())
        {
            std::string displayName;
            unsigned int displayId;

            IDMap::iterator it = _idMap.find(classname);
            if (it == _idMap.end())
            {
                displayName = (const char*)(OLocalizator::getString("DisplayName", classname).toAscii().data());
                displayId = 0;

                _idMap[classname] = IDCounter(displayName, 0);
            }
            else
            {
                displayName = it->second.first;
                displayId = ++it->second.second;
            }

            std::ostringstream oss;
            oss << displayName << " " << displayId;

            return QString(oss.str().c_str());
        }
    }

    return "";
}
