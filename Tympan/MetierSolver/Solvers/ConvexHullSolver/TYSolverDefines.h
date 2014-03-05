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

#ifndef __TY_SOLVERDEFINES__
#define __TY_SOLVERDEFINES__

#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticSurface.h"
#include "Tympan/MetierSolver/ToolsMetier/OSegment3D.h"

struct TYStructSurfIntersect
{
    TYAcousticSurfaceGeoNode* pSurfGeoNode; //Geonode de la surface
    OMatrix matInv; // Matrice inverse pour les faces d'infrastructure
    TYTabPoint tabPoint; // Tableau de point utilise pour la preselection
    bool isEcran;   // Est un ecran
    bool isInfra; // Face d'infrastructure
};

// Structure de donnees simple pour la gestion des intersections
struct TYSIntersection
{
	TYAcousticSurface* pSurface; // Surface qui intersecte
	TYAcousticVolume* pVolume;   // Volume parent de la surface
	bool isInfra;
	bool isEcran;
    OSegment3D segInter[2]; // Segment d'intersection face/plan vertical[0] ou horizontal[1]
    bool noIntersect; // Indique qu'une face ne doit pas etre testee pour l'intersection
    bool bIntersect[2]; // Booleen qui indique si la face coupe le plan vertical ([0]) ou horizontal([1])
};

// Structure de donnees simple pour decrire le plan pour conserver les point qui definissent le plan de travail
typedef struct
{
    OPoint3D pt1;
    OPoint3D pt2;
    OPoint3D pt3;
} TYSPlan;

// Structures contenant un segment et un spectre
typedef struct
{
    OSegment3D _segment;
    TYSpectre* _spectreAtt;
} TYStructSegLPSp;

// Tableau de structures contenant un segment et un spectre.
typedef std::vector<TYStructSegLPSp> TYTabStructSegLPSp;

///Rayon du cercle utilise dans le calcul de l'attenuation de la vegetation.
#define TY_EDF_FOREST_RAYON 5000
///Limite pour la prise en compte de l'effet de diffraction (=0.1dB).
#define TY_EDF_DIFFRACTION_LIMIT 1.023293

#endif // __TY_SOLVERDEFINES__
