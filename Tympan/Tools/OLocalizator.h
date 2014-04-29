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
 *
 *
 *
 */

#ifndef __O_LOCALIZATOR__
#define __O_LOCALIZATOR__


#include <qmap.h>
#include <qstring.h>
#include <qdom.h>

#include "OXMLTreeManager.h"


/**
 * A Class to handle application an plugin ressources.
 * It allows to store strings and reference sounds and picture outside the application.
 * It can be used to localize the plugins or the application.
 */

class OLocalizator
{
public:
    /**
     * Load a file containing the ressources for a particular language.
     * @param filename XML filename (Mcs ressource format).
     */
    static bool setRessourceFile(const QString& filename);
    /**
     * Sets the absolute directory path of the sounds and pictures.
     * @param path File path of the picture and sounds to be used in the application.
     */
    static void setRessourcePath(const QString& path) { _ressourcePath = path; }
    /**
     * Return true if ressource file is loaded, false otherwise.
     */
    static bool isRessourceFile() { return _isRessourceFile; }
    /**
     * Returns a string identified by its id  and used by the class Classname.
     * @param classname Name of the class using this string.
     * @param stringId Identifier of this string.
     */
    static QString getString(const QString& classname, const QString& stringId);  // translate a string
    /**
     * Returns a picture full path name, identified by its id and used by the class Classname.
     * @param classname Name of the class using this picture.
     * @param stringId Identifier of this picture
     */
    static QString getPicture(const QString& classname, const QString& pictureId);  // transtate a picture

private:
    static bool selectClassDirectory(const QString& classname);
    static bool findIdAndSetValue(const QString& directory, const QString& stringId, QString& returnString);


private:
    static bool _isRessourceFile;

    typedef QMap<QString, QString> OptimMap;
    static OptimMap         _stringMap;
    static QString          _ressourcePath;
    static OXMLTreeManager  _treeManager;
};

#endif // __O_LOCALIZATOR__
