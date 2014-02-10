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




#include "TYConfigure.h"
//#if TY_USE_IHM

#include <qstring.h>
#include <qdom.h>

#include "OXMLTreeManager.h"

#include "OLocalizator.h"


//----------------------------------------------------------------------------
// public methods
//----------------------------------------------------------------------------

bool OLocalizator::setRessourceFile(const QString& filename)
{
    _stringMap.empty();
    _isRessourceFile = _treeManager.load(filename);
    return _isRessourceFile;
}


QString OLocalizator::getString(const QString& className, const QString& stringId)
{
    QString string = "";

    if (_isRessourceFile)
    {
        QString keyForMap = className + stringId;
        OptimMap::Iterator it;
        if (_stringMap.find(keyForMap) != _stringMap.end())
        {
            return _stringMap[keyForMap];
        }

        if (selectClassDirectory(className))
        {
            if (_treeManager.setCurrentDirectory("message"))
            {
                findIdAndSetValue("string", stringId, string);
            }
        }
        _stringMap[keyForMap] = string;
    }

    return string;
}

QString OLocalizator::getPicture(const QString& className, const QString& pictureId)
{
    QString string = "";
    if (_isRessourceFile)
    {
        if (selectClassDirectory(className))
        {
            if (_treeManager.setCurrentDirectory("picture"))
            {
                if (findIdAndSetValue("image", pictureId, string))
                {
                    string.prepend(_ressourcePath);
                }
            }
        }
    }

    return string;
}

//----------------------------------------------------------------------------
// private members
//----------------------------------------------------------------------------

bool OLocalizator::_isRessourceFile = false;

QString OLocalizator::_ressourcePath;
OXMLTreeManager OLocalizator::_treeManager;
OLocalizator::OptimMap OLocalizator::_stringMap;

//----------------------------------------------------------------------------
// private methods
//----------------------------------------------------------------------------

bool OLocalizator::selectClassDirectory(const QString& classname)
{
    bool bFound = FALSE;

    _treeManager.setCurrentDirectory("", TRUE);

    unsigned int i = 0;
    unsigned int classcount = _treeManager.getSubDirectoryCount("class");

    while ((i < classcount) && (bFound == FALSE))
    {
        QString str;

        if (_treeManager.setCurrentDirectory("class", TRUE, i))
        {
            if (_treeManager.getFieldStringValue("name", str))
            {
                if (str == classname)
                {
                    bFound = TRUE;
                }
            }
        }
        i++;
    }

    return bFound;
}

bool OLocalizator::findIdAndSetValue(const QString& directory, const QString& stringId, QString& returnString)
{
    bool bFound = FALSE;

    unsigned int i = 0;
    unsigned int directorycount = _treeManager.getSubDirectoryCount(directory);

    while ((i < directorycount) && (bFound == FALSE))
    {
        QString objectid;

        if (_treeManager.setCurrentDirectory(directory, FALSE, i))
        {
            if (_treeManager.getFieldStringValue("id", objectid))
            {
                if (objectid == stringId)
                {
                    bFound = _treeManager.getFieldStringValue("value", returnString);
                }
            }
            _treeManager.setCurrentDirectoryUp();
        }
        i++;
    }

    return bFound;
}


//#endif // TY_USE_IHM
