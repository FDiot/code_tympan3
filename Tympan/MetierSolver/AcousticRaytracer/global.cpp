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
 
#include "global.h"

////////////////////////////
// General Values
////////////////////////////
int globalMaxProfondeur;        //Nombre d'evenements autorises pour un rayon, globalMaxProfondeur inclu
int globalNbRaysPerSource;      //Nombre de rayons lances par les sources
float globalSizeReceiver;       //Diametre de la sphere representant le recepteur
int globalAccelerator;          //Choix de la structure acceleratrice. 0 : BruteForce, 1 : GridAccelerator, 2 : BVH, 3 : KdTree, other : GridAccelerator
int globalMaxTreeDepth;         //Profondeur maximale autorisee pour le BVH ou KdTree.
bool globalUseSol;              //Utilisation du sol (ou pas -cas NMPB-)
bool globalKeepDebugRay;        //Permet de conserver les rayons qui ont ete invalides pendant la propagation.
float globalMaxLength;          //Longueur maximale autorisee pour un rayon, globalMaxLength inclu
float globalSampleGround2D;     //Echantillonage sur sol pour la description de la topographie 2D sous le rayon. (NMPB)
int globalRayTracingOrder;      //[0-2]Sens de traitement des rayon source-recepteur ou inverse (0 = SR / 1 =RS / 2 = auto)
float globalAnalyticAnglePhi;   // Angle de tir vertical (phi) des rayons
int globalDiscretization;		//Permet de choisir entre des rayons aléatoires ou déterministes (discretisation source)
int globalN1;					//Longitude : nbr of parts
int globalRayAsked;		// Nbr of rays initially asked by the user

////////////////////////////
// Reflexion
///////////////////////////
int globalMaxReflexion;         //Nombre de reflexions speculaires autorisees pour un rayon, globalMaxReflexion inclu

////////////////////////////
// Diffraction
///////////////////////////
int globalMaxDiffraction;       //Nombre de diffractions autorisees pour un rayon, globalMaxDiffraction inclu
int globalNbRayWithDiffraction; //Nombre de rayons relances lors d'un evenement diffraction
float globalAngleDiffMin;       //Angle minimal a prendre en compte entre 2 faces pour ajouter une arrete de diffraction
float globalCylindreThick;      //Epaisseur des aretes de diffraction.

/////////////////////////////
// Targeting system + NMPB
/////////////////////////////
bool globalEnableTargets;       //Active la recherche de cible pour les sources. Pour le moment lie au solver NMPB.
bool globalEnableFullTargets;   //Active la recherche de cible apres un evenement diffu. Pour le moment lie au solver NMPB.
float globalTargetsDensity;     //Densite pour l'echantillonnage des zones interessantes.

///////////////
// meteo
//////////////
bool globalUseMeteo;            // Prise en compte (ou non) de la meteo
double globalAnalyticDMax;      // Distance de propagation maximale des rayons courbes
double globalAnalyticTMax;      // Temps de propagation maximal des rayons courbes
double globalAnalyticH;         // Pas de temps de calcul pour la propagation des rayons courbes
int globalAnalyticNbRay;        // Nombre de rayons tires pour le lancer de rayons courbes
double globalAnalyticGradC;     // Gradient vertical de celerite
double globalAnalyticGradV;     // Gradient vertical de vitesse de vent
double globalAnalyticC0;        // Celerite du son initiale
int globalAnalyticTypeTransfo;  // Methode de transformation -- TOUJOURS = 1 -- pas d'autre methode definie
bool globalRestitModifiedGeom;  // Indique si l'on souhaite recuperer la geometrie transformee
double globalOverSampleD;       // [0 +[ (0 pas de surechantillonnage) Indique le taux de surechantillonnage des rayons
double globalWindDirection;		// Direction du vent (un vent a 0 est dirige du nord vers le sud)

bool globalUseFresnelArea;		// take into account the fresnel area
float globalAnime3DSigma;		// incertitude relative sur la taille du rayon au carree
float globalAnime3DForceC;		// Force C à 0.0 -> globalAnime3DForceC=0; 1.0 -> globalAnime3DForceC = 1 ou autre valeur dépendant de globalAnime3DSigma
