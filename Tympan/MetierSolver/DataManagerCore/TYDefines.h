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

#ifndef __TY_DEFINES__
#define __TY_DEFINES__

#include "Tympan/core/defines.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Tympan/Tools/OSmartPtr.h"

// Pour l'utilisation de la librairie standard STL.
#include <map>
#include <list>
#include <vector>
#include <deque>
#include <string>
#include <fstream>
#include <iomanip>
#include <QString>

/**
 * Declarations des types utilises par les methodes virtuelle d'extension
 * de TYElement. Ces methodes sont :
 * <pre>
 *      TYEditWidget *      TYElement::getEditWidget();
 * </pre>
 */

///Declarations de types pour l'API IHM et l'Impression.
#if TY_USE_IHM
class QWidget;

///Type element IHM de base.
typedef QWidget     TYEditWidget;
#else
///Type element IHM de base.
typedef void        TYEditWidget;
#endif



////////////////////////////////////////////////////////////////////////////////
// Declarations des Smart Pointers pour une meilleur lisibilite dans le code.
// Les classes sont simplement prefixees LP (pour Long Pointer).
////////////////////////////////////////////////////////////////////////////////

class TYElement;
typedef class OGenID TYUUID;


///Smart pointer sur TYElement.
typedef SmartPtr<TYElement> LPTYElement;

class TYDirectivite;
///Smart pointer sur TYDirectivite.
typedef SmartPtr<TYDirectivite> LPTYDirectivite;

class TYTrafic;
///Smart pointer sur TYTrafic.
typedef SmartPtr<TYTrafic> LPTYTrafic;

class TYAttenuateur;
///Smart pointer sur TYAttenuateur.
typedef SmartPtr<TYAttenuateur> LPTYAttenuateur;

class TYSource;
///Smart pointer sur TYSource.
typedef SmartPtr<TYSource> LPTYSource;

class TYSourceLineic;
///Smart pointer sur TYSourceLineic.
typedef SmartPtr<TYSourceLineic> LPTYSourceLineic;

class TYSourcePonctuelle;
///Smart pointer sur TYSourcePonctuelle.
typedef SmartPtr<TYSourcePonctuelle> LPTYSourcePonctuelle;

class TYSourceSurfacic;
///Smart pointer sur TYSourceSurfacic.
typedef SmartPtr<TYSourceSurfacic> LPTYSourceSurfacic;

class TYSpectre;
///Smart pointer sur TYSpectre.
typedef SmartPtr<TYSpectre> LPTYSpectre;

class TYBox;
///Smart pointer sur TYBox.
typedef SmartPtr<TYBox> LPTYBox;

class TYFaceSet;
///Smart pointer sur TYBox.
typedef SmartPtr<TYFaceSet> LPTYFaceSet;

class TYPoint;
///Smart pointer sur TYPoint.
typedef SmartPtr<TYPoint> LPTYPoint;

class TYPointCalcul;
///Smart pointer sur TYPointCalcul.
typedef SmartPtr<TYPointCalcul> LPTYPointCalcul;

class TYRectangle;
///Smart pointer sur TYRectangle.
typedef SmartPtr<TYRectangle> LPTYRectangle;

class TYRepere;
///Smart pointer sur TYRepere.
typedef SmartPtr<TYRepere> LPTYRepere;

class TYSegment;
///Smart pointer sur TYSegment.
typedef SmartPtr<TYSegment> LPTYSegment;

class TYSurfaceInterface;
///Smart pointer sur TYSurfaceInterface.
typedef SmartPtr<TYSurfaceInterface> LPTYSurfaceInterface;

class TYVolumeInterface;
///Smart pointer sur TYVolumeInterface.
typedef SmartPtr<TYVolumeInterface> LPTYVolumeInterface;

class TYPolygon;
///Smart pointer sur TYPolygon.
typedef SmartPtr<TYPolygon> LPTYPolygon;

class TYAcousticBox;
///Smart pointer sur TYAcousticBox.
typedef SmartPtr<TYAcousticBox> LPTYAcousticBox;

class TYAcousticCircle;
///Smart pointer sur TYAcousticCircle.
typedef SmartPtr<TYAcousticCircle> LPTYAcousticCircle;

class TYAcousticCylinder;
///Smart pointer sur TYAcousticCylinder.
typedef SmartPtr<TYAcousticCylinder> LPTYAcousticCylinder;

class TYAcousticFaceSet;
///Smart pointer sur TYAcousticCylinder.
typedef SmartPtr<TYAcousticFaceSet> LPTYAcousticFaceSet;

class TYAcousticInterface;
///Smart pointer sur TYAcousticInterface.
typedef SmartPtr<TYAcousticInterface> LPTYAcousticInterface;

class TYAcousticLine;
///Smart pointer sur TYAcousticLine.
typedef SmartPtr<TYAcousticLine> LPTYAcousticLine;

class TYAcousticRectangle;
///Smart pointer sur TYAcousticRectangle.
typedef SmartPtr<TYAcousticRectangle> LPTYAcousticRectangle;

class TYAcousticRectangleNode;
///Smart pointer sur TYAcousticRectangleNode.
typedef SmartPtr<TYAcousticRectangleNode> LPTYAcousticRectangleNode;

class TYAcousticSemiCircle;
///Smart pointer sur TYAcousticSemiCircle.
typedef SmartPtr<TYAcousticSemiCircle> LPTYAcousticSemiCircle;

class TYAcousticSemiCylinder;
///Smart pointer sur TYAcousticSemiCylinder.
typedef SmartPtr<TYAcousticSemiCylinder> LPTYAcousticSemiCylinder;

class TYAcousticSurface;
///Smart pointer sur TYAcousticSurface.
typedef SmartPtr<TYAcousticSurface> LPTYAcousticSurface;

class TYAcousticSurfaceNode;
///Smart pointer sur TYAcousticSurfaceNode.
typedef SmartPtr<TYAcousticSurfaceNode> LPTYAcousticSurfaceNode;

class TYAcousticVolume;
///Smart pointer sur TYAcousticVolume.
typedef SmartPtr<TYAcousticVolume> LPTYAcousticVolume;

class TYAcousticVolumeNode;
///Smart pointer sur TYAcousticVolumeNode.
typedef SmartPtr<TYAcousticVolumeNode> LPTYAcousticVolumeNode;

class TYBatiment;
///Smart pointer sur TYBatiment.
typedef SmartPtr<TYBatiment> LPTYBatiment;

class TYEtage;
///Smart pointer sur TYEtage.
typedef SmartPtr<TYEtage> LPTYEtage;

class TYEcran;
///Smart pointer sur TYEcran.
typedef SmartPtr<TYEcran> LPTYEcran;

class TYMachine;
///Smart pointer sur TYMachine.
typedef SmartPtr<TYMachine> LPTYMachine;

class TYMur;
///Smart pointer sur TYMur.
typedef SmartPtr<TYMur> LPTYMur;

class TYMurElement;
///Smart pointer sur TYMurElement.
typedef SmartPtr<TYMurElement> LPTYMurElement;

class TYParoi;
///Smart pointer sur TYParoi.
typedef SmartPtr<TYParoi> LPTYParoi;

class TYReseauTransport;
///Smart pointer sur TYReseauTransport.
typedef SmartPtr<TYReseauTransport> LPTYReseauTransport;

class TYRoute;
///Smart pointer sur TYRoute.
typedef SmartPtr<TYRoute> LPTYRoute;

class TYRay;
// SmartPOinter sur TYRay
typedef SmartPtr<TYRay> LPTYRay;

class TYDalle;
///Smart pointer sur TYDalle.
typedef SmartPtr<TYDalle> LPTYDalle;

class TYAltimetrie;
///Smart pointer sur TYAltimetrie.
typedef SmartPtr<TYAltimetrie> LPTYAltimetrie;

class TYCourbeNiveau;
///Smart pointer sur TYCourbeNiveau.
typedef SmartPtr<TYCourbeNiveau> LPTYCourbeNiveau;

class TYCoursEau;
///Smart pointer sur TYCoursEau.
typedef SmartPtr<TYCoursEau> LPTYCoursEau;

class TYTerrain;
///Smart pointer sur TYTerrain.
typedef SmartPtr<TYTerrain> LPTYTerrain;

class TYPlanEau;
///Smart pointer sur TYPlanEau.
typedef SmartPtr<TYPlanEau> LPTYPlanEau;

class TYCalcul;
///Smart pointer sur TYCalcul.
typedef SmartPtr<TYCalcul> LPTYCalcul;

class TYMaillage;
///Smart pointer sur TYMaillage.
typedef SmartPtr<TYMaillage> LPTYMaillage;

class TYRectangularMaillage;
///Smart pointer sur TYRectangularMaillage.
typedef SmartPtr<TYRectangularMaillage> LPTYRectangularMaillage;

class TYBoundaryNoiseMap;
///Smart pointer on TYBoundaryNoiseMap.
typedef SmartPtr<TYBoundaryNoiseMap> LPTYBoundaryNoiseMap;

class TYLinearMaillage;
///Smart pointer sur TYLinearMaillage.
typedef SmartPtr<TYLinearMaillage> LPTYLinearMaillage;

class TYProjet;
///Smart pointer sur TYProjet.
typedef SmartPtr<TYProjet> LPTYProjet;

class TYResultat;
///Smart pointer sur TYResultat.
typedef SmartPtr<TYResultat> LPTYResultat;

class TYAtmosphere;
///Smart pointer sur TYAtmosphere.
typedef SmartPtr<TYAtmosphere> LPTYAtmosphere;

class TYMateriauConstruction;
///Smart pointer sur TYMateriauConstruction.
typedef SmartPtr<TYMateriauConstruction> LPTYMateriauConstruction;

class TYSol;
///Smart pointer sur TYSol.
typedef SmartPtr<TYSol> LPTYSol;

class TYVegetation;
///Smart pointer sur TYVegetation.
typedef SmartPtr<TYVegetation> LPTYVegetation;

class TYInfrastructure;
///Smart pointer sur TYInfrastructure.
typedef SmartPtr<TYInfrastructure> LPTYInfrastructure;

class TYSite;
///Smart pointer sur TYSite.
typedef SmartPtr<TYSite> LPTYSite;

class TYSiteNode;
///Smart pointer sur TYSiteNode.
typedef SmartPtr<TYSiteNode> LPTYSiteNode;

class TYTopographie;
///Smart pointer sur TYTopographie.
typedef SmartPtr<TYTopographie> LPTYTopographie;

class TYPalette;
///Smart pointer sur TYPalette.
typedef SmartPtr<TYPalette> LPTYPalette;

class TYPanel;
///Smart pointer sur TYPanel.
typedef SmartPtr<TYPanel> LPTYPanel;
////////////////////////////////////////////////////////////////////////////////
// Definition de types pour les collections
////////////////////////////////////////////////////////////////////////////////

///List de pointeur de TYElement.
typedef std::list<TYElement*> TYListPtrElement;

///Collection d'identifiants.
typedef std::list<TYUUID> TYListID;

///Collection de TYSol.
typedef std::vector<TYSol> TYTabSol;

///Collection de TYSpectre.
typedef std::vector<LPTYSpectre> TYTabLPSpectre;

///Collection de TYPoint.
typedef std::vector<TYPoint> TYTabPoint;

///Collection de TYSegment.
typedef std::vector<TYSegment> TYTabSegment;

///Collection de TYRectangle.
typedef std::vector<TYRectangle> TYTabRectangle;

///Collection de pointeurs de TYPolygon.
typedef std::vector<LPTYPolygon> TYTabLPPolygon;

///Collection de pointeurs de TYSurfaceInterface.
typedef std::vector<LPTYSurfaceInterface> TYTabPtrSurface;

///Collection de pointeurs de TYSourcePonctuelle.
typedef std::vector<LPTYSourcePonctuelle> TYTabLPSourcePonctuelle;

///Collection de pointeurs de TYPointCalcul.
typedef std::vector<LPTYPointCalcul> TYTabLPPointCalcul;

///Map de TYElement pour la gestion des identifiants uniques.
typedef std::map<TYUUID, TYElement*> TYMapPtrElementUuid;

///Etat d'utilisation de sources.
typedef std::map<TYSourcePonctuelle*, bool> TYMapPtrSrcePonctBool;

///Tableau associant un spectre a un id (identifiant d'objet)
typedef std::map<QString, TYSpectre*> TYMapIdSpectre;

///Tableau associant un booleen a un id (identifiant d'objet)
typedef std::map<QString, bool> TYMapIdBool;

///Etat d'utilisation des elements (utilise par les calculs)
typedef std::map<TYElement*, bool> TYMapPtrElementBool;

///Tableau associatif des elements et d'un entier (utilise pour gerer les regimes)
typedef std::map<TYElement*, int> TYMapPtrElementInt;

/// Tableau associant un regime a un tableau associatif d'entiers (regime machine in batiments vs regime bat)
typedef std::vector<TYMapPtrElementInt> tabMapPtrElementRegime;

/// Tableau associant un regime et un tableau associatif d'elements et de booleen (etat machine vs regime bat)
typedef std::vector<TYMapPtrElementBool> tabMapPtrElementBool;

///Etat d'utilisation d'attenuateurs.
typedef std::map<TYElement*, LPTYAttenuateur> TYMapPtrElmtPtrAtt;

//Etat d'utilisation des acousticVolumeNode.
typedef std::map<TYAcousticVolumeNode*, bool> TYMapPtrAcVolNodeBool;

///Collection de pointeurs de TYCalcul.
typedef std::vector<LPTYCalcul> TYTabLPCalcul;

class OVector3D;
class OPoint3D;
class OSegment3D;

///Collection de OVector3D.
typedef std::vector<OVector3D> TYTabVector;
///Collection de OPoint3D.
typedef std::deque<OPoint3D> TYTabPoint3D;
///Collection de OSegment3D.
typedef std::vector<OSegment3D> TYTabSegment3D;

////////////////////////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////////////////////////

///Precision pour la representation des cercles par des segments.
#define TYPRECISIONCIRCLE       0.1

///Resolution par defaut pour la representation des cercles par des segments.
#define TYDEFAULTRESOLUTIONIONCIRCLE        20

///Longueur par defaut de la topographie.
#define TAILLETOPOX 400

///Largeur par defaut de la topographie.
#define TAILLETOPOY 400


#endif // __TY_DEFINES__
