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

#ifndef __TY_PLUGINMANAGER__
#define __TY_PLUGINMANAGER__

#include <list>
#include <string>

#include <qdir.h>
#include <qstringlist.h>

#include "Tympan/core/defines.h"
#include "Tympan/core/interfaces.h"
#include "Tympan/core/smartptr.h"
#include "Tympan/core/plugin.h"

#if TY_COMPILER == TY_COMPILER_MSVC
#    define LIB_HANDLE hInstance
#    define LIB_LOAD(a) LoadLibraryA(a)
#    define LIB_GETSYM(a, b) GetProcAddress(a, b)
#    define LIB_UNLOAD(a) FreeLibrary(a)

struct HINSTANCE__;
typedef struct HINSTANCE__* hInstance;

#else
#    define LIB_HANDLE void*
#    define LIB_LOAD(a) dlopen( a, RTLD_LAZY | RTLD_LOCAL)
#    define LIB_GETSYM(a, b) dlsym(a, b)
#    define LIB_UNLOAD(a) dlclose(a)
#endif

typedef void (*TYPGStartPlugin)();
typedef Plugin* (*TYPGGetPlugin)();
typedef void (*TYPGStopPlugin)();

struct TYPluginData
{
    LIB_HANDLE pluginDllInstance;

    TYPGStartPlugin startPlugin;
    TYPGGetPlugin getPlugin;
    TYPGStopPlugin stopPlugin;

    QString filename;
};

class TYPluginManager;

// Smart pointer sur TYPluginManager.
typedef SmartPtr<TYPluginManager> LPTYPluginManager;

/**
 * Gestionnaire de plug-ins.
 */

// UUID of the Default Solver
#define DEFAULT_SOLVER_UUID "{A98B320C-44C4-47a9-B689-1DD352DAA8B2}"

class TYPluginManager : public IRefCount
{
public:
    // Liste des plug-ins
    typedef std::list<TYPluginData*> TYPluginList;

public:
    /**
     * Constructeur par default.
     */
    TYPluginManager();

    /**
     * Destructeur.
     */
    ~TYPluginManager();

    /**
     * Definit cette instance comme singleton.
     * Attention : cette instance doit absolument avoir ete creee
     * sur la heap (operateur new).
     *
     * @return Indique si un singleton etait deja defini ou pas.
     */
    bool setAsSingleton();

    /**
     * Retourne l'instance singleton.
     */
    static LPTYPluginManager get();

    //! Get the list of dynamic library files to load.
    /*! These files are different available solvers.
     \param directory The directory where there are the library files.
     \return The list of the dynamic library files.
     */
    QFileInfoList getPluginFileList(const QDir& directory) const;

    //! Create \c TYPluginData related to library files to load.
    /*! Create a \c TYPluginData for each library file and update the list
        \c _plugins.
     \param file_list List of files to load.
     */
    void createPlugins(const QFileInfoList& file_list);

    //! Check the loaded plugin.
    /*! Check if the loaded object has the good methods.
     \param plugin_data The Plugin data related to the plugin to create.
     \return Start with success?
     */
    bool startPlugin(TYPluginData* plugin_data);

    //! Load, check, create and start plugins.
    /*!
     \param directory Path to the dynamic library files (aka plugins) to load.
     */
    bool loadPlugins(const QString& directory);

    // Decharge les plugins charges de la memoire
    void unloadPlugins();

    Plugin* getPlugin(const OGenID& uuid) const;

    // Retourne un SolverInterface
    SolverInterface* getSolver(const OGenID& uuid) const;

    // Retourne les infos du solveur
    void getInfos(pluginInfos* pInfos, const OGenID& uuid) const;

    /// Retourne une info spécifique du solveur
    QString getInfo(const QString& info, const OGenID& uuid) const;

    // Get plug-ins list
    TYPluginList& getPluginList();

    // Verifie si le plugin uuid existe
    bool exist(const OGenID& uuid);

    // Verifie si le plugin existe
    bool exist(const QString& solverName);

    // Retourne l'identifiant de solver correspondant au nom `solverName`
    OGenID solverID(const QString& solverName);

private:
    TYPluginList _plugins;

    // Instance unique
    static LPTYPluginManager _pInstance;
};


class TYCalcul;

namespace tympan
{
    //! Load the solver plugins found in `path`
    /*!
     * \param path Path to the solver libraries (plugins) to load, containing
     *      among others the default solver
     */
    void load_solvers(const char *path);

    //! Select a solver amongst the loaded solvers (a previous call to tympan::load_solvers() is
    // needed)
    /*!
     * \param uuid identifier of the solver to load
     */
    SolverInterface* select_solver(OGenID uuid);

    //! Return the id of the solver named `name` (a previous call to tympan::load_solvers() is
    // needed)
    OGenID solver_id(std::string name);

} /* namespace tympan */

#endif // __TY_PLUGINMANAGER__
