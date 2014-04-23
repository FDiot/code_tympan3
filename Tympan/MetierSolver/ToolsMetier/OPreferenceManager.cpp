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




#include "OPreferenceManager.h"
#include "DOMSave.h"
#include "GlobalMacros.h"

#include <qfile.h>
#include <iostream>
#include <qtextstream.h>

using namespace :: std;

///Taille du buffer pour les conversions vers ou depuis des QString.
static const int _strBufferSize = 64;



//////////////////////////////////////////////////////////////////////////////
// OPreferenceManager


OPreferenceManager::OPreferenceManager()
{
    _nPreferenceModificationStamp = 0;
    setXMLRootNodeName("Preferences");
    setXMLParamNodeName("Preference");
    setCurrentDirectory("Main");
}

OPreferenceManager::OPreferenceManager(const QString& currentDirectory)
{
    _nPreferenceModificationStamp = 0;
    setXMLRootNodeName("Preferences");
    setXMLParamNodeName("Preference");
    setCurrentDirectory(currentDirectory);
}

OPreferenceManager::OPreferenceManager(const QString& rootNodeName,
                                       const QString& paramNodeName)
{
    _nPreferenceModificationStamp = 0;
    setXMLRootNodeName(rootNodeName);
    setXMLParamNodeName(paramNodeName);
    setCurrentDirectory("Main");
}

OPreferenceManager::OPreferenceManager(const QString& rootNodeName,
                                       const QString& paramNodeName,
                                       const QString& currentDirectory)
{
    _nPreferenceModificationStamp = 0;
    setXMLRootNodeName(rootNodeName);
    setXMLParamNodeName(paramNodeName);
    setCurrentDirectory(currentDirectory);
}

OPreferenceManager::~OPreferenceManager()
{
    reset();
}

void OPreferenceManager::loadDefault(const char* defaultParams[][2], int nbParams)
{
    _nPreferenceModificationStamp++;
    // Charge le map avec les valeurs par defaut.
    for (int i = 0; i < nbParams; i++)
    {
        (_prefDirs._dirMap[_curDir])._prefMap[defaultParams[i][0]] = defaultParams[i][1];
    }
}

void OPreferenceManager::setCurrentDirectory(QString dir)
{
    _curDir = dir;
}

void OPreferenceManager::reset()
{
    _nPreferenceModificationStamp++;
    // On vide le tableau, les QStrings sont effaces proprement...
    _prefDirs._dirMap.clear();
}

bool OPreferenceManager::readXML(QString fileName)
{
    _nPreferenceModificationStamp++;
    //    int retval = 0;

    QDomDocument doc;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        std::cerr << "\nError when open '" << fileName.toAscii().data() << "'\n";
        return false;
    }
    QString  errorMsg ;
    int errorLine ;
    int errorColumn ;
    if (!doc.setContent(&file, &errorMsg, &errorLine, &errorColumn))
    {
        std::cerr << "\nError during parsing: '" << fileName.toAscii().data() << "'\n"
                  << "error message is:  \n"
                  <<  errorMsg.toAscii().constData() << "\n"
                  << "in line:" << errorLine << " in column:" << errorColumn << endl;
        file.close();
        return false;
    }
    file.close();
    // On recupere le noeud Document
    QDomDocument domDocument = doc;

    // On recupere l'element XML root du document
    QDomElement rootElement = domDocument.documentElement();

    // Recense tous les elements enfants du root
    QDomNodeList nodeList = rootElement.childNodes();

    QString str;

    int nodecount = nodeList.length();

    // Pour chaque noeud enfant du noeud root
    for (int i = 0; i < nodecount; i++)
    {
        QDomNode curElem = nodeList.item(i);

        if (curElem.isElement())
        {
            // Le nom de la categorie
            QString dir = curElem.nodeName();
            // Recense tous les elements enfants du courant
            QDomNodeList nodeList2 = curElem.childNodes();

            int nodecount2 = nodeList2.length();

            for (int j = 0; j < nodecount2; j++)
            {
                QDomElement curElem2 = nodeList2.item(j).toElement();
                // Si c'est un noeud 'Preference'
                if (!curElem2.isNull() && (_xmlParamNodeName == curElem2.nodeName()))
                {
                    // On recupere les champs 'name' et 'value' du noeud
                    QString name = curElem2.attribute("key");
                    QString value = curElem2.attribute("value");
                    if (value.isEmpty())
                    {
                        value = "0";
                    }


                    // Ajout de l'entree au tableau associatif
                    _prefDirs._dirMap[dir]._prefMap[name] = value;
                }
            }
        }
    }


    return true;
}

bool OPreferenceManager::writeXML(QString fileName)
{
    bool res = false;

    QDomImplementation domImpl;
    // Declaration du type de document (DTD)
    QString qualifiedName = _xmlRootNodeName;       // Nom du noeud root
    QString systemId = "";//"Tympan.dtd ";      // URI de la DTD

    QDomDocumentType docType = domImpl.createDocumentType(qualifiedName, QString(), systemId);

    // Creation d'un Document XML vide (Precision: document != fichier)
    QDomDocument domDocument = domImpl.createDocument(
                                   QString(),                  // root element namespace URI.
                                   _xmlRootNodeName,       // root element name
                                   docType);           // document type object (DTD).

    // Recuperation du noeud root du document
    QDomElement rootElement = domDocument.documentElement();

    // Iteration du tableau associatif des preferences
    ODirMap::ODirMapIt iter;
    for (iter = _prefDirs._dirMap.begin(); iter != _prefDirs._dirMap.end(); iter++)
    {
        // La categorie correspondante
        QString dir = (*iter).first;

        // Creation d'un nouveau noeud
        QDomElement dirElem = domDocument.createElement(dir);
        // Ajout du noeud au noeud root
        rootElement.appendChild(dirElem);

        // Iteration du tableau associatif des preferences
        OPrefMap::OPrefMapIt iter2;
        for (iter2 = _prefDirs._dirMap[dir]._prefMap.begin(); iter2 != _prefDirs._dirMap[dir]._prefMap.end(); iter2++)
        {
            // Creation d'un nouveau noeud
            QDomElement prefElem = domDocument.createElement(_xmlParamNodeName);
            // Ajout du noeud au noeud root
            dirElem.appendChild(prefElem);

            // Ajout des attributs au nouveau noeud
            prefElem.setAttribute("key", ((*iter2).first));
            prefElem.setAttribute("value", ((*iter2).second));
        }
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        return res;
    }

    QTextStream ts(&file);
    ts << domDocument.toString();

    file.close();

    res = true;
    return res;
}

bool OPreferenceManager::exists(const QString& pref)
{
    return exists(_curDir, pref);
}

bool OPreferenceManager::exists(const QString& dir, const QString& pref)
{
    OPrefMap::OPrefMapIt ite;
    ite = _prefDirs._dirMap[dir]._prefMap.find(pref);
    return (ite != _prefDirs._dirMap[dir]._prefMap.end());
}

QString OPreferenceManager::getDirectory(const QString& pref)
{
    ODirMap::ODirMapIt ite;
    QString dir;

    // On parcours la totalite du tableau associatif
    for (ite = _prefDirs._dirMap.begin(); ite != _prefDirs._dirMap.end(); ite++)
    {
        // La categorie courante
        dir = (*ite).first;

        if (exists(dir, pref))
        {
            // On retourne le nom de la categorie
            return dir;
        }
    }

    return dir;
}

void OPreferenceManager::setString(const QString& pref, const QString& value)
{
    setString(_curDir, pref, value);
}

void OPreferenceManager::setString(const QString& dir, const QString& pref, const QString& value)
{
    _nPreferenceModificationStamp++;
    //  ODirMap::iterator it;
    _prefDirs._dirMap[dir]._prefMap.erase(pref);
    //  preference[pref] = value;

    //  QString test = preference[pref];
    //  preference->insert(make_pair(pref, value));
    //  pair<QString, QString> couple = make_pair(pref, value);
    //  it = _prefDirs.find(dir);
    //  *it.insert(couple);
    _prefDirs._dirMap[dir]._prefMap[pref] = value;
}

QString OPreferenceManager::getString(const QString& pref)
{
    return getString(_curDir, pref);
}

QString OPreferenceManager::getString(const QString& dir, const QString& pref)
{
    return _prefDirs._dirMap[dir]._prefMap[pref];
}

void OPreferenceManager::setStringArray(const QString& pref, const QString* valueArray, const int& sizeArray)
{
    setStringArray(_curDir, pref, valueArray, sizeArray);
}

void OPreferenceManager::setStringArray(const QString& dir, const QString& pref, const QString* valueArray, const int& sizeArray)
{
    if (sizeArray <= 0) { return; }

    for (int i = 0; i < sizeArray; i++)
    {
        setString(dir, pref + intToString(i), valueArray[i]);
    }
}

QString* OPreferenceManager::getStringArray(const QString& pref, int& sizeArray)
{
    return getStringArray(_curDir, pref, sizeArray);
}

QString* OPreferenceManager::getStringArray(const QString& dir, const QString& pref, int& sizeArray)
{
    sizeArray = 0;

    // Recherche de la taille du tableau
    while (exists(dir, pref + intToString(sizeArray)))
    {
        ++sizeArray;
    }

    if (sizeArray <= 0) { return NULL; }

    // Tableau de QString a retourner
    QString* strs = new QString[sizeArray];

    for (int i = 0; i < sizeArray; i++)
    {
        strs[i] = getString(dir, pref + intToString(i));
    }

    return strs;
}

#if TY_ARCH_TYPE == TY_ARCHITECTURE_64
void OPreferenceManager::setUInt(const QString& pref, const unsigned int& value)
{
    setUInt(_curDir, pref, value);
}

void OPreferenceManager::setUInt(const QString& dir, const QString& pref, const unsigned int& value)
{
    _nPreferenceModificationStamp++;
    _prefDirs._dirMap[dir]._prefMap[pref] = uintToString(value);
}

void OPreferenceManager::setUInt(const QString& pref, const size_t& value)
{
    setUInt(_curDir, pref, value);
}

void OPreferenceManager::setUInt(const QString& dir, const QString& pref, const size_t& value)
{
    _nPreferenceModificationStamp++;
    _prefDirs._dirMap[dir]._prefMap[pref] = uintToString(value);
}

#if TY_COMPILER == TY_COMPILER_MSVC
void OPreferenceManager::setUInt(const QString& pref, const unsigned long& value)
{
    setUInt(_curDir, pref, value);
}

void OPreferenceManager::setUInt(const QString& dir, const QString& pref, const unsigned long& value)
{
    _nPreferenceModificationStamp++;
    _prefDirs._dirMap[dir]._prefMap[pref] = uintToString(value);
}
#endif
#else
void OPreferenceManager::setUInt(const QString& pref, const size_t& value)
{
    setUInt(_curDir, pref, value);
}

void OPreferenceManager::setUInt(const QString& dir, const QString& pref, const size_t& value)
{
    _nPreferenceModificationStamp++;
    _prefDirs._dirMap[dir]._prefMap[pref] = uintToString(value);
}

void OPreferenceManager::setUInt(const QString& pref, const unsigned long& value)
{
    setUInt(_curDir, pref, value);
}

void OPreferenceManager::setUInt(const QString& dir, const QString& pref, const unsigned long& value)
{
    _nPreferenceModificationStamp++;
    _prefDirs._dirMap[dir]._prefMap[pref] = uintToString(value);
}
#endif

void OPreferenceManager::setInt(const QString& pref, const int& value)
{
    setInt(_curDir, pref, value);
}

void OPreferenceManager::setInt(const QString& dir, const QString& pref, const int& value)
{
    _nPreferenceModificationStamp++;
    _prefDirs._dirMap[dir]._prefMap[pref] = intToString(value);
}

int OPreferenceManager::getInt(const QString& pref)
{
    return getInt(_curDir, pref);
}

int OPreferenceManager::getInt(const QString& dir, const QString& pref)
{
    QString str = _prefDirs._dirMap[dir]._prefMap[pref];
    int res = str.toFloat();
    return res;
}

void OPreferenceManager::setBool(const QString& pref, const bool& value)
{
    setBool(_curDir, pref, value);
}

void OPreferenceManager::setBool(const QString& dir, const QString& pref, const bool& value)
{
    _nPreferenceModificationStamp++;
    _prefDirs._dirMap[dir]._prefMap[pref] = value ? "true" : "false";
}

bool OPreferenceManager::getBool(const QString& pref)
{
    return getBool(_curDir, pref);
}

bool OPreferenceManager::getBool(const QString& dir, const QString& pref)
{
    if (!exists(dir, pref)) //az++
    {
        return false;
    }
    QString sValue = _prefDirs._dirMap[dir]._prefMap[pref];//az++
    if (sValue.isEmpty())
    {
        return false;
    }
    return (_prefDirs._dirMap[dir]._prefMap[pref] == "true") ? true : false;
}

void OPreferenceManager::setFloat(const QString& pref, const float& value)
{
    setFloat(_curDir, pref, value);
}

void OPreferenceManager::setFloat(const QString& dir, const QString& pref, const float& value)
{
    _nPreferenceModificationStamp++;
    _prefDirs._dirMap[dir]._prefMap[pref] = floatToString(value);
}

float OPreferenceManager::getFloat(const QString& pref)
{
    return getFloat(_curDir, pref);
}

float OPreferenceManager::getFloat(const QString& dir, const QString& pref)
{
    return _prefDirs._dirMap[dir]._prefMap[pref].toFloat();
}

void OPreferenceManager::setDouble(const QString& pref, const double& value)
{
    setDouble(_curDir, pref, value);
}

void OPreferenceManager::setDouble(const QString& dir, const QString& pref, const double& value)
{
    _nPreferenceModificationStamp++;
    _prefDirs._dirMap[dir]._prefMap[pref] = doubleToString(value);
}

double OPreferenceManager::getDouble(const QString& pref)
{
    return getDouble(_curDir, pref);
}

double OPreferenceManager::getDouble(const QString& dir, const QString& pref)
{
    return _prefDirs._dirMap[dir]._prefMap[pref].toDouble();
}

void OPreferenceManager::setFrame(const QString& pref, const int& posX, const int& posY, const int& sizeX, const int& sizeY)
{
    setFrame(_curDir, pref, posX, posY, sizeX, sizeY);
}

void OPreferenceManager::setFrame(const QString& dir, const QString& pref, const int& posX, const int& posY, const int& sizeX, const int& sizeY)
{
    _nPreferenceModificationStamp++;
    _prefDirs._dirMap[dir]._prefMap[pref + "PosX"] = intToString(posX);
    _prefDirs._dirMap[dir]._prefMap[pref + "PosY"] = intToString(posY);
    _prefDirs._dirMap[dir]._prefMap[pref + "SizeX"] = intToString(sizeX);
    _prefDirs._dirMap[dir]._prefMap[pref + "SizeY"] = intToString(sizeY);
}

void OPreferenceManager::getFrame(const QString& pref, int& posX, int& posY, int& sizeX, int& sizeY)
{
    getFrame(_curDir, pref, posX, posY, sizeX, sizeY);
}

void OPreferenceManager::getFrame(const QString& dir, const QString& pref, int& posX, int& posY, int& sizeX, int& sizeY)
{
    if (_prefDirs._dirMap[dir]._prefMap[pref + "PosX"].isEmpty())
    {
        posX = 0;
    }
    else
    {
        posX = _prefDirs._dirMap[dir]._prefMap[pref + "PosX"].toInt();
    }

    if (_prefDirs._dirMap[dir]._prefMap[pref + "PosY"].isEmpty())
    {
        posY = 0;
    }
    else
    {
        posY = _prefDirs._dirMap[dir]._prefMap[pref + "PosY"].toInt();
    }

    if (_prefDirs._dirMap[dir]._prefMap[pref + "SizeX"].isEmpty())
    {
        sizeX = 400;
    }
    else
    {
        sizeX = _prefDirs._dirMap[dir]._prefMap[pref + "SizeX"].toInt();
    }

    if (_prefDirs._dirMap[dir]._prefMap[pref + "SizeY"].isEmpty())
    {
        sizeY = 400;
    }
    else
    {
        sizeY = _prefDirs._dirMap[dir]._prefMap[pref + "SizeY"].toInt();
    }
}

void OPreferenceManager::setPoint(const QString& pref, const int& x, const int& y)
{
    setPoint(_curDir, pref, x, y);
}

void OPreferenceManager::setPoint(const QString& dir, const QString& pref, const int& x, const int& y)
{
    _nPreferenceModificationStamp++;
    _prefDirs._dirMap[dir]._prefMap[pref + "X"] = intToString(x);
    _prefDirs._dirMap[dir]._prefMap[pref + "Y"] = intToString(y);
}

void OPreferenceManager::getPoint(const QString& pref, int& x, int& y)
{
    getPoint(_curDir, pref, x, y);
}

void OPreferenceManager::getPoint(const QString& dir, const QString& pref, int& x, int& y)
{
    x = _prefDirs._dirMap[dir]._prefMap[pref + "X"].toInt();
    y = _prefDirs._dirMap[dir]._prefMap[pref + "Y"].toInt();
}

void OPreferenceManager::setColor(const QString& pref, const float& r, const float& g, const float& b)
{
    setColor(_curDir, pref, r, g, b);
}

void OPreferenceManager::setColor(const QString& dir, const QString& pref, const float& r, const float& g, const float& b)
{
    _nPreferenceModificationStamp++;
    _prefDirs._dirMap[dir]._prefMap[pref + "R"] = floatToString(r);
    _prefDirs._dirMap[dir]._prefMap[pref + "G"] = floatToString(g);
    _prefDirs._dirMap[dir]._prefMap[pref + "B"] = floatToString(b);
}

void OPreferenceManager::getColor(const QString& pref, float& r, float& g, float& b)
{
    getColor(_curDir, pref, r, g, b);
}

void OPreferenceManager::getColor(const QString& dir, const QString& pref, float& r, float& g, float& b)
{
    r = _prefDirs._dirMap[dir]._prefMap[pref + "R"].toFloat();
    g = _prefDirs._dirMap[dir]._prefMap[pref + "G"].toFloat();
    b = _prefDirs._dirMap[dir]._prefMap[pref + "B"].toFloat();
}


//////////////////////////////////////////////////////////////////////////////
// Methodes statiques utilitaires.
//////////////////////////////////////////////////////////////////////////////

#if TY_ARCH_TYPE == TY_ARCHITECTURE_64
QString OPreferenceManager::uintToString(const unsigned int& val)
{
    char buffer[_strBufferSize];
    sprintf(buffer, "%d", val);
    QString str(buffer);
    return str;
}

QString OPreferenceManager::uintToString(const size_t& val)
{
    char buffer[_strBufferSize];
    sprintf(buffer, "%d", val);
    QString str(buffer);
    return str;
}
#if TY_COMPILER == TY_COMPILER_MSVC
QString OPreferenceManager::uintToString(const unsigned long& val)
{
    char buffer[_strBufferSize];
    sprintf(buffer, "%d", val);
    QString str(buffer);
    return str;
}
#endif
#else
QString OPreferenceManager::uintToString(const size_t& val)
{
    char buffer[_strBufferSize];
    sprintf(buffer, "%d", val);
    QString str(buffer);
    return str;
}

QString OPreferenceManager::uintToString(const unsigned long& val)
{
    char buffer[_strBufferSize];
    sprintf(buffer, "%d", val);
    QString str(buffer);
    return str;
}
#endif

QString OPreferenceManager::intToString(const int& val)
{
    char buffer[_strBufferSize];
    sprintf(buffer, "%d", val);
    QString str(buffer);
    return str;
}

QString OPreferenceManager::floatToString(const float& val)
{
    char buffer[_strBufferSize];
    sprintf(buffer, "%f", val);
    QString str(buffer);
    return str;
}

QString OPreferenceManager::doubleToString(const double& val)
{
    char buffer[_strBufferSize];
    sprintf(buffer, "%f", val);
    QString str(buffer);
    return str;
}
