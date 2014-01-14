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

#ifndef __TY_PHMETIER__
#define __TY_PHMETIER__

/* Headers which will be used for precompilation if the compiler
 * supports it.
 */

#define TY_PRODUCT_XMLTAG_      "Tympan"
#define TY_PRODUCT_VERSION_     "3.8.0" // TODO This should be configured at build time

#include "Tympan/common_precompiled.h"

#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/OSpectre.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/OSpectreComplex.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticBox.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticCircle.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticCylinder.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticEdge.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticFaceSet.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticInterface.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticLine.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticPolygon.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticRectangle.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticRectangleNode.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticSemiCircle.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticSemiCylinder.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticSurface.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticSurfaceNode.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticVolume.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticVolumeNode.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltTopographique/TYAltimetrie.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltMateriaux/TYAtmosphere.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYAttenuateur.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYBatiment.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYBoucheSurface.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYBox.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYCalcul.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYChemineeSurface.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltTopographique/TYCourbeNiveau.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltTopographique/TYCoursEau.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYDalle.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYDirectivite.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYEcran.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYEtage.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYFaceSet.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYGeometryNode.h"
#include "Tympan/MetierSolver/DataManagerMetier/Site/TYInfrastructure.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYLinearMaillage.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYMachine.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYMaillage.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltMateriaux/TYMateriauConstruction.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYMur.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYMurElement.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYPalette.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYPanel.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYParoi.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltTopographique/TYPlanEau.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYPoint.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYPointCalcul.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYPointControl.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYPolygon.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYProjet.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYRay.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYRectangle.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYRectangularMaillage.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYBoundaryNoiseMap.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYRegime.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYRepere.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYReseauTransport.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYResultat.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYRoute.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYRouteDeblai.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYSegment.h"

#include "Tympan/MetierSolver/DataManagerMetier/Site/TYSiteNode.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltMateriaux/TYSol.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSource.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSourceBafflee.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSourceCheminee.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSourceLineic.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSourcePonctuelle.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSourceSurfacic.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSpectre.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYSurfaceInterface.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltTopographique/TYTerrain.h"
#include "Tympan/MetierSolver/DataManagerMetier/Site/TYTopographie.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYTrafic.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYUserSourcePonctuelle.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYUserSrcRegime.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltMateriaux/TYVegetation.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYVolumeInterface.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYEtape.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYChemin.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYTrajet.h"

#endif // __TY_PHMETIER__
