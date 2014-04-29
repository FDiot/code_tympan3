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

#ifndef __TYPLUGIN__
#define __TYPLUGIN__

#pragma warning(disable: 4251)

#include "Tympan/Tools/TYConfigure.h"

#if TY_COMPILER == TY_COMPILER_MSVC
#   ifdef _PLUGIN_DLL
#       define PLUGIN_DECL __declspec(dllexport)
#   else
#       define PLUGIN_DECL __declspec(dllimport)
#   endif
#else
#   define PLUGIN_DECL
#endif

#include <string>

#include "Tympan/Tools/OGenID.h"
#include "TYSolverInterface.h"

typedef struct
{
    QString _name;
    QString _author;
    QString _version;
    QString _description;
} pluginInfos;

class TYPlugin
{
public:
    TYPlugin() : _pSolver(0) { }
    ~TYPlugin() { if (_pSolver) { delete _pSolver; } }

    // Set des informations
    void setName(const QString& name) { _name = name; }
    void setAuthor(const QString& author) { _author = author; }
    void setVersion(const QString& version) { _version = version; }
    void setDescription(const QString& description) { _description = description; }
    void setUUID(const OGenID& uuid) { _uuid = uuid; }

    // Get des informations
    QString getName() const { return _name; }
    QString getAuthor() const { return _author; }
    QString getVersion() const { return _version; }
    QString getDescription() const { return _description; }

    void getInfos(pluginInfos* pInfos)
    {
        pInfos->_name    = _name;
        pInfos->_author  = _author;
        pInfos->_version = _version;
        pInfos->_description = _description;
    }

    OGenID getUUID() const { return _uuid; }

    // Set du solver
    void setSolver(TYSolverInterface* solver) { _pSolver = solver; }

    // Get du solver
    TYSolverInterface* getSolver() const { return _pSolver; }

private:
    // Pointeur sur le TYSolverInterface
    TYSolverInterface* _pSolver;

    // Informations sur le plugin
    QString _name;
    QString _author;
    QString _version;
    QString _description;
    OGenID _uuid;
};

#endif // __TYPLUGIN__
