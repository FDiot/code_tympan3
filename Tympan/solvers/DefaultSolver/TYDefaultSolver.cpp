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

#include "TYSolver.h"
#include "TYDefaultSolver.h"

// Variable globale
TYPlugin* plugin;

void startPlugin(bool console)
{
    plugin = new TYPlugin();

    // Information sur le plugin
    plugin->setName("DefaultSolver");
    plugin->setAuthor("Projet_Tympan");
    plugin->setVersion("1.0");
    plugin->setDescription("Solveur acoustique par defaut integre a l'application Tympan");
    plugin->setUUID(OGenID("{A98B320C-44C4-47a9-B689-1DD352DAA8B2}"));

    // Creation du solver
    plugin->setSolver(new TYSolver());
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
