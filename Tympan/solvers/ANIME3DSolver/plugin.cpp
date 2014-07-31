/*
 * Copyright (C) <2012-2014> <EDF-R&D> <FRANCE>
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

/**
 * \brief Le plugin associe au solveur ANIME3D
 */

#include "Tympan/core/plugin.h"
#include "Tympan/solvers/ANIME3DSolver/TYANIME3DSolver.h"

Plugin* plugin;

extern "C" PLUGIN_DECL void startPlugin()
{
    plugin = new Plugin();
    // Information sur le plugin
    plugin->setName("ANIME3DSolver");
    plugin->setAuthor("Projet_Tympan");
    plugin->setVersion("1.0");
    plugin->setDescription("Plugin de la methode ANIME3D");
    plugin->setUUID(OGenID("{2f0b51a7-37bd-414e-8908-baea85acef2c}"));
    // Creation du solver
    plugin->setSolver(new TYANIME3DSolver());
}

extern "C" PLUGIN_DECL Plugin* getPlugin()
{
    return plugin;
}

extern "C" PLUGIN_DECL void stopPlugin()
{
    delete plugin;
}
