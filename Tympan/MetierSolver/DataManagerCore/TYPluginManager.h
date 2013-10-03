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

#ifndef __TY_PLUGINMANAGER__
#define __TY_PLUGINMANAGER__

#include <list>

#include <qdir.h>
#include <qstringlist.h>

#include "Tympan/Tools/TYConfigure.h"
#include "Tympan/Tools/OSmartPtr.h"
#include "TYPlugin.h"

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

typedef void (*TYPGStartPlugin)(bool console);
typedef TYPlugin* (*TYPGGetPlugin)();
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
 *
 * @author Projet_Tympan
 *
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
     \param with_graphical Is the graphical interface used?
     */
    void createPlugins(const QFileInfoList& file_list, bool with_graphical);

    //! Check the loaded plugin.
    /*! Check if the loaded object has the good methods.
     \param plugin_data The Plugin data related to the plugin to create.
     \param with_graphical Is the graphical interface used?
     \return Start with success?
     */
    bool startPlugin(TYPluginData* plugin_data, bool with_graphical);

    //! Load, check, create and start plugins.
    /*!
     \param directory Path to the dynamic library files (aka plugins) to load.
     \param with_graphical Is the graphical interface used?
     */
    bool loadPlugins(const QString& directory, bool with_graphical=true);

    //! Load plugins (aka solvers) from the Qt interface.
    bool loadPluginsGraphicMode(const QString& directory);

    // Decharge les plugins charges de la memoire
    void unloadPlugins();

    // Fonction de haut-niveau permettant le retour directe de l'objet TYSolver courant
    TYSolverInterface* getSolver() const;

    // Retourne un TYSolverInterface
    TYSolverInterface* getSolver(const OGenID& uuid) const;

    // Retourne les infos du solveur
    void getInfos(pluginInfos* pInfos) const;

    // Retourne les infos du solveur defini par uuid
    void getInfos(pluginInfos* pInfos, const OGenID& uuid) const;
	
	/// Retourne l'info demandee
    QString getInfo(const QString& info);

	/// Retourne l'info demandée sur un solveur spécifique
	QString getInfo(const QString& info, const OGenID& uuid) const;

    // Set current solver uuid
    void setCurrent(const OGenID& uuid);

    // Set current solver name
    void setCurrent(const QString& solverName);

    // Get current solver uuid
    OGenID getCurrent() const;

    // Get plug-ins list
    TYPluginList& getPluginList();

    // Verifie si le plugin uuid existe
    bool exist(const OGenID& uuid);

    // Verifie si le plugin existe
    bool exist(const QString& solverName);

private:
    TYPluginList _plugins;

    // Current plugin used
    OGenID _current;

    // Instance unique
    static LPTYPluginManager _pInstance;
};

#endif // __TY_PLUGINMANAGER__
