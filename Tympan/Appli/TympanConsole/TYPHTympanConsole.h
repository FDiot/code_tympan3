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
 */

#ifndef __TY_PHTYMPANCONSOLE__
#define  __TY_PHTYMPANCONSOLE__
#pragma once

#ifndef _WIN32_WINNT        // Autorise l'utilisation des fonctionnalites specifiques a Windows XP ou version ulterieure.
#define _WIN32_WINNT 0x0501 // Attribuez la valeur appropriee a cet element pour cibler d'autres versions de Windows.
#endif

#include "Tympan/common_precompiled.h"

#include "Tympan/MetierSolver/DataManagerCore/TYDefines.h"
#include "TYConsole.h"
#include "Tympan/MetierSolver/DataManagerCore/TYElement.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYProjet.h"

#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYLinearMaillage.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYMaillage.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYPalette.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYPanel.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYRectangularMaillage.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSourceBafflee.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSourceCheminee.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSourceLineic.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYTrafic.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticBox.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticFaceSet.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticLine.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYBoucheSurface.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYChemineeSurface.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYFaceSet.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYBatiment.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYEcran.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYReseauTransport.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYRoute.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltTopographique/TYCoursEau.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltTopographique/TYPlanEau.h"

#include "Tympan/Tools/OMessageManager.h"
#include "Tympan/Tools/OLocalizator.h"

// TODO : faites reference ici aux en-tetes supplementaires necessaires au programme
#endif // __TY_PHTYMPANCONSOLE__
