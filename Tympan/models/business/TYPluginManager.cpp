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


#include "Tympan/core/logging.h"
#include "Tympan/models/business/TYCalcul.h"
#include "TYPluginManager.h"

#if TY_COMPILER == TY_COMPILER_MSVC
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#else
extern "C"
{
#   include <dlfcn.h>
}
#endif


// Initialise a NULL.
LPTYPluginManager TYPluginManager::_pInstance = 0;

TYPluginManager::TYPluginManager()
{
}

TYPluginManager::~TYPluginManager()
{
    unloadPlugins();
}

bool TYPluginManager::setAsSingleton()
{
    bool ret = _pInstance ? true : false;
    _pInstance = this;
    return ret;
}

LPTYPluginManager TYPluginManager::get()
{
    if (!_pInstance)
    {
        _pInstance = new TYPluginManager();
    }

    return _pInstance;
}

QFileInfoList TYPluginManager::getPluginFileList(const QDir& directory) const
{
    // Get all available plug-ins.
    QStringList filters;
    filters.append("*.dll");
    filters.append("*.so");
    QFileInfoList file_list = directory.entryInfoList(filters, QDir::Files, QDir::Name);
    return file_list;
}

void TYPluginManager::createPlugins(const QFileInfoList& file_list)
{
    QFileInfoList::const_iterator itfile = file_list.begin();
    for (itfile; itfile != file_list.end(); ++itfile)
    {
        TYPluginData* plugin_data = new TYPluginData();
        plugin_data->filename = itfile->absoluteFilePath();

        OMessageManager::get()->debug(
              "Trying to dynamically load '%s'.\n",
              itfile->absoluteFilePath().toUtf8().data());

        plugin_data->pluginDllInstance =
            (LIB_HANDLE)LIB_LOAD(itfile->absoluteFilePath().toLocal8Bit().data());

        if (plugin_data->pluginDllInstance)
        {
            // Retrieve methods via pointers to member functions of a plugin.
            plugin_data->startPlugin =
                (TYPGStartPlugin)LIB_GETSYM(plugin_data->pluginDllInstance, "startPlugin");
            plugin_data->getPlugin =
                (TYPGGetPlugin)LIB_GETSYM(plugin_data->pluginDllInstance, "getPlugin");
            plugin_data->stopPlugin =
                (TYPGStopPlugin)LIB_GETSYM(plugin_data->pluginDllInstance, "stopPlugin");
        }
        else
        {
            OMessageManager::get()->info(
                "Unable to load the library file '%s'.",
                itfile->absoluteFilePath().toUtf8().data());
            // Do not attempt to unload a library not loaded
            delete plugin_data;
            continue;
        }

        // Check all pointers to methods.
        if (!plugin_data->startPlugin || !plugin_data->getPlugin ||
            !plugin_data->stopPlugin)
        {
            OMessageManager::get()->info(
                "Unable to get methods from the plugin '%s'.",
                itfile->absoluteFilePath().toUtf8().data());
            LIB_UNLOAD(plugin_data->pluginDllInstance);
            delete plugin_data;
            continue;
        }

        // Start the plugin and add it to the list '_plugins'. True if success.
        bool success = startPlugin(plugin_data);
        if (!success)
        {
            OMessageManager::get()->info(
                "Unable to start the plugin from the file '%s'.",
                itfile->absoluteFilePath().toUtf8().data());
            LIB_UNLOAD(plugin_data->pluginDllInstance);
            delete plugin_data;
            continue;
        }
    }
}


bool TYPluginManager::startPlugin(TYPluginData* plugin_data)
{
    // Start the plugin.
    plugin_data->startPlugin();
    // If already exists in the list, stop and unload it.
    if (exist(plugin_data->getPlugin()->getUUID()))
    {
        plugin_data->stopPlugin();
        return false;
    }
    else
    {
        _plugins.push_back(plugin_data);
        return true;
    }
}


bool TYPluginManager::loadPlugins(const QString& directory)
{
    // Get the folder where there are plug-ins (aka solvers).
    QString dirPath = QDir::convertSeparators(directory);
    QDir dir(dirPath);

    // Check if the folder exists.
    if (!dir.exists())
    {
        OMessageManager::get()->info(
            "The directory of plugins '%s' does not exist.",
            dir.absolutePath().toUtf8().data());
        return false;
    }

    QFileInfoList plugin_file_list = getPluginFileList(dir);

    // Load methods related to a plugin, check them and add the related plugin
    // to the list.
    createPlugins(plugin_file_list);

    // Check if there is unless the default plugin.
    if (!exist(OGenID(QString(DEFAULT_SOLVER_UUID))))
    {
        OMessageManager::get()->info("WARNING: Default solver is not present !");
    }

    if (_plugins.size() == 0)
    {
        OMessageManager::get()->info("No plugins found.");
        return false;
    }

    return true;
}


void TYPluginManager::unloadPlugins()
{
    // On decharge la memoire
    for (TYPluginList::iterator it = _plugins.begin(); it != _plugins.end(); ++it)
    {
        (*it)->stopPlugin();
        LIB_UNLOAD((*it)->pluginDllInstance);
        delete(*it);
    }

    // On vide la liste
    _plugins.clear();
}

SolverInterface* TYPluginManager::getSolver(const OGenID& uuid) const
{
    Plugin* plugin = getPlugin(uuid);
    if (plugin != nullptr)
        return plugin->getSolver();
    return nullptr;
}

Plugin* TYPluginManager::getPlugin(const OGenID& uuid) const
{
    for (TYPluginList::const_iterator it = _plugins.begin(); it != _plugins.end(); ++it)
        if ((*it)->getPlugin()->getUUID() == uuid)
        {
            return (*it)->getPlugin();
        }

    return nullptr;
}

void TYPluginManager::getInfos(pluginInfos* pInfos, const OGenID& uuid) const
{
    Plugin *plugin = getPlugin(uuid);
    if (plugin)
    {
        plugin->getInfos(pInfos);
    }
}

QString TYPluginManager::getInfo(const QString& info, const OGenID& uuid) const
{
    pluginInfos* pInfos = new pluginInfos();
    getInfos(pInfos, uuid);
    if (info == "author")
    {
        return pInfos->_author;
    }
    else if (info == "version")
    {
        return pInfos->_version;
    }
    else if (info == "description")
    {
        return pInfos->_description;
    }
    else // name
    {
        return pInfos->_name;
    }
}


TYPluginManager::TYPluginList& TYPluginManager::getPluginList()
{
    return _plugins;
}

bool TYPluginManager::exist(const OGenID& uuid)
{
    // Si on trouve l'uuid, on retourne vrai
    for (TYPluginList::iterator it = _plugins.begin(); it != _plugins.end(); ++it)
        if ((*it)->getPlugin()->getUUID() == uuid)
        {
            return true;
        }

    // Sinon, on retourne faux
    return false;
}

bool TYPluginManager::exist(const QString& solverName)
{
    // Si on trouve l'uuid, on retourne vrai
    for (TYPluginList::iterator it = _plugins.begin(); it != _plugins.end(); ++it)
        if ((*it)->getPlugin()->getName() == solverName)
        {
            return true;
        }

    // Sinon, on retourne faux
    return false;
}

namespace tympan
{
    void load_solver(const char *path, TYCalcul *comp)
    {
        LPTYPluginManager plugin_manager = TYPluginManager::get();
        plugin_manager->unloadPlugins();
        plugin_manager->loadPlugins(path);
        comp->setPlugin(plugin_manager->getPlugin(comp->getSolverId()));
    }
} /* namespace tympan */
