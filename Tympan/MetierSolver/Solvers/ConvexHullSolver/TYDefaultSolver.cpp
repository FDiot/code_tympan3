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


#if TY_USE_IHM
#include "Tympan/Tools/TYProgressManager.h"
#endif


#include "TYSolver.h"
#include "TYDefaultSolver.h"

// Variable globale
TYPlugin* plugin;

void startPlugin(bool console)
{
    plugin = new TYPlugin();

    // Information sur le plugin
    plugin->setName("Convex hull solver");
    plugin->setAuthor("Projet_Tympan");
    plugin->setVersion("1.1");
    plugin->setDescription("Version améliorée du solveur acoustique par defaut integre a l'application Tympan");
    plugin->setUUID(OGenID("{aed757de-3d8c-4233-bb58-362a15755492}"));

    // Creation du solver
    plugin->setSolver(new TYSolver());

#if TY_USE_IHM
    TYProgressManager::setConsoleMode(console);
    TYProgressManager::create(NULL, plugin->getName().toAscii().data());
#endif
}

TYPlugin* getPlugin()
{
    // On renvoi le plugin
    return plugin;
}

void stopPlugin()
{
    // On detruit le plugin
    delete plugin;
}
