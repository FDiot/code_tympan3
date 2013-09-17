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

#ifndef TYPOINT_PARCOURS_H
#define TYPOINT_PARCOURS_H

#define SEUIL_DISTANCE_POINTS_CONFONDUS 0.02//en-dessous de 2 cm, les points sont consideres comme confondus
#define SEUIL_DETERMNANT_COLINEAIRE     (1E-10)//en-dessous de cette valeur (absolue), le determinant n'est plus un critere valide pour juger de la colinearite de 2 vecteurs

struct TYPointParcours
{
    double x;
    double y;
    double z;
	double _abcisse; //  Abscisse curviligne sur SR
    int Identifiant;
    bool isInfra;
    bool isEcran;
    static const double _dSeuilDistanceCarre;

    bool operator == (TYPointParcours& p);

    double normeCarree();

    static bool Confondus(TYPointParcours* P1, TYPointParcours* P2);
    static double ZCross(TYPointParcours SR, TYPointParcours SP);
    static bool IntersectionDroites(TYPointParcours& P1, TYPointParcours& P2, TYPointParcours& P3, TYPointParcours& P4, TYPointParcours& P);
    static bool IntersectionSegments(TYPointParcours& P1, TYPointParcours& P2, TYPointParcours& P3, TYPointParcours& P4, TYPointParcours& P);
    static double Scalaire(TYPointParcours& P1, TYPointParcours& P2, TYPointParcours& P3, TYPointParcours& P4);
    static double Scalaire(TYPointParcours& vecteur1, TYPointParcours& vecteur2);
    static TYPointParcours vecteur2D(TYPointParcours& P1, TYPointParcours& P2);
    static double AbscisseCurviligneCarreSurSR(TYPointParcours& P, TYPointParcours& S, TYPointParcours& R);
};

#endif//TYPOINT_PARCOURS_H
