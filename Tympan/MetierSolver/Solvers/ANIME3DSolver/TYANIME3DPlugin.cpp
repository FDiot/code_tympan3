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

/**
 * \file TYANIME3DPlugin.cpp
 * \brief Le plugin associe a la methode ANIME3D
 * \author Projet_Tympan
 * \date 04 avril 2011
 */

#include "Tympan/Tools/TYProgressManager.h"

#include "Tympan/MetierSolver/ToolsMetier/OMatrix.h"
#include "Tympan/MetierSolver/ToolsMetier/OTriangle.h"
#include "Tympan/MetierSolver/ToolsMetier/OVector3D.h"
#include "Tympan/MetierSolver/ToolsMetier/OPoint3D.h"

#include "Tympan/MetierSolver/DataManagerCore/TYSolverInterface.h"
#include "Tympan/MetierSolver/DataManagerCore/TYPlugin.h"

#include "Tympan/MetierSolver/ToolsMetier/OSpectreComplex.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSourcePonctuelle.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYGeometryNode.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticSurface.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltMateriaux/TYSol.h"
#include "Tympan/MetierSolver/DataManagerMetier/Site/TYSiteNode.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYPointCalcul.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYRay.h"

#include "Tympan/MetierSolver/AcousticRaytracer/Tools/Logger.h"

//#include "TYANIME3DSiteNode.h"
#include "TYANIME3DSolver.h"

// Variable globale
TYPlugin* plugin;

extern "C" PLUGIN_DECL void startPlugin(bool console)
{
    plugin = new TYPlugin();

    // Information sur le plugin
    plugin->setName("ANIME3DSolver");
    plugin->setAuthor("Projet_Tympan");
    plugin->setVersion("1.0");
    plugin->setDescription("Plugin de la methode ANIME3D");
    plugin->setUUID(OGenID("{2f0b51a7-37bd-414e-8908-baea85acef2c}"));

    // Creation du solver
    plugin->setSolver(new TYANIME3DSolver());

#if TY_USE_IHM
    TYProgressManager::setConsoleMode(console);
    TYProgressManager::create(NULL, plugin->getName().toAscii().data());
#endif
}

extern "C" PLUGIN_DECL TYPlugin* getPlugin()
{
    // On renvoi le plugin
    return plugin;
}

extern "C" PLUGIN_DECL void stopPlugin()
{
    // On detruit le plugin
    delete plugin;
}
