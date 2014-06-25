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


//#ifdef TYMPAN_USE_PRECOMPILED_HEADER
//#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
//#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "TYPointParcours.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

const double TYPointParcours::_dSeuilDistanceCarre = SEUIL_DISTANCE_POINTS_CONFONDUS * SEUIL_DISTANCE_POINTS_CONFONDUS;

TYPointParcours TYPointParcours::vecteur2D(TYPointParcours& P1, TYPointParcours& P2)
{
    TYPointParcours res = P2;
    res.x -= P1.x;
    res.y -= P1.y;
    return res;
}

double TYPointParcours::ZCross(TYPointParcours SR, TYPointParcours SP)
{
    double zCross = (SR.x * SP.y) - (SR.y * SP.x);
    return zCross;
}

double TYPointParcours::normeCarree()
{
    return (x * x + y * y);
}

bool TYPointParcours::operator == (TYPointParcours& p)
{
    return Confondus(this, &p);
}

bool TYPointParcours::Confondus(TYPointParcours* P1, TYPointParcours* P2)
{
    double XDiff = (P1->x - P2->x);
    double YDiff = (P1->y - P2->y);
    double dNormeCarre = XDiff * XDiff + YDiff * YDiff;
    return dNormeCarre < _dSeuilDistanceCarre;
}

double TYPointParcours::AbscisseCurviligneCarreSurSR(TYPointParcours& P, TYPointParcours& S, TYPointParcours& R)
{
    //On calcul a peu de chose pres l'abscisse curviligne de P sur [SR]
    //"a peu de chose pres" = c'est en fait le carre de l'abscisse, en gardant son signe
    //permet d'avoir une relation d'ordre sur [SR] (sqrt est inutile pour cela)
    //P est-il de l'autre ci¿½te de S par rapport a R ?
    bool bDuMemeCoteQueR = Scalaire(S, R, S, P) >= 0;
    TYPointParcours SR = vecteur2D(S, R);
    TYPointParcours SP = vecteur2D(S, P);
    double dNormeCSR = SR.normeCarree();
    if (0 == dNormeCSR)
    {
        return 0;    //evitons les divisions par zero...
    }
    double dNormeCSP = SP.normeCarree();
    double dAbscisseCarree = (bDuMemeCoteQueR ? dNormeCSP / dNormeCSR : -dNormeCSP / dNormeCSR);
    return dAbscisseCarree;
}

double TYPointParcours::Scalaire(TYPointParcours& vecteur1, TYPointParcours& vecteur2)
{
    return vecteur1.x * vecteur2.x + vecteur1.y * vecteur2.y;
}

double TYPointParcours::Scalaire(TYPointParcours& P1, TYPointParcours& P2, TYPointParcours& P3, TYPointParcours& P4)
{
    //return (P2.x - P1.x)*(P4.x - P3.x) + (P2.y - P1.y)*(P4.y - P3.y);
    TYPointParcours aTYPointParcours_P1P2 = vecteur2D(P1, P2), aTYPointParcours_P3P4 = vecteur2D(P3, P4);
    return Scalaire(aTYPointParcours_P1P2, aTYPointParcours_P3P4);
}

bool TYPointParcours::IntersectionSegments(TYPointParcours& P1, TYPointParcours& P2, TYPointParcours& P3, TYPointParcours& P4, TYPointParcours& P)
{
    //2. P2 & P3 sont-ils du meme ci¿½te de P1P2 ?
    TYPointParcours P1P2 = vecteur2D(P1, P2);
    TYPointParcours P1P3 = vecteur2D(P1, P3);
    TYPointParcours P1P4 = vecteur2D(P1, P4);
    double detP3 = ZCross(P1P2, P1P3);
    double detP4 = ZCross(P1P2, P1P4);
    if (detP3 && detP4 && (detP3 * detP4) > 0)
    {
        return false;
    }

    bool bIntersectionDroites = IntersectionDroites(P1, P2, P3, P4, P);
    if (!bIntersectionDroites)
    {
        return false;
    }
    //P est-il confondu avec P1 ?
    if (Confondus(&P1, &P))
    {
        return false;
    }

    //P est-il confondu avec P2 ?
    if (Confondus(&P2, &P))
    {
        return false;
    }

    //Le point P est-il de l'autre ci¿½te de P1 par rapport a P2 ?
    if (Scalaire(P1, P, P1, P2) <= 0)
    {
        return false;
    }

    //Le point P est-il sur P1P2 ?
    TYPointParcours P1P = vecteur2D(P1, P);
    if (P1P.normeCarree() >= P1P2.normeCarree())
    {
        return false;
    }

    return true;
}

bool TYPointParcours::IntersectionDroites(TYPointParcours& P1, TYPointParcours& P2, TYPointParcours& P3, TYPointParcours& P4, TYPointParcours& P)
{
    P.x = P1.x; P.y = P1.y; P.z = P1.z;//conservons l'indentifiant
    double alpha;
    //Eq de droite P1P2
    if (P2.x == P1.x)
    {
        //droite verticale
        P.x = P1.x;
        if (P4.x == P3.x)
        {
            return false;    //colineaire
        }
        //P3P = alphaP3P4
        alpha = (P.x - P3.x) / (P4.x - P3.x);
        P.y = P3.y + alpha * (P4.y - P3.y);
        //Calculons la 3e coordonnee; on sait que les vecteurs ne sont pas colineaires
        P.z = P3.z + alpha * (P4.z - P3.z);
        return true;
    }
    else if (P2.y == P1.y)
    {
        //droite horizontale
        P.y = P1.y;
        if (P4.y == P3.y)
        {
            return false;    //colineaire
        }
        //P3P = alphaP3P4
        alpha = (P.y - P3.y) / (P4.y - P3.y);
        P.x = P3.x + alpha * (P4.x - P3.x);
        //Calculons la 3e coordonnee; on sait que les vecteurs ne sont pas colineaires
        P.z = P3.z + alpha * (P4.z - P3.z);
        return true;
    }
    else if ((P4.y == P3.y) || (P4.x == P3.x))
    {
        return IntersectionDroites(P3, P4, P1, P2, P);
    }

    double a = (P2.y - P1.y) / (P2.x - P1.x);
    double b =  -a * P1.x + P1.y;
    double ap = (P4.y - P3.y) / (P4.x - P3.x);
    double bp = -ap * P3.x + P3.y;

    if (a == ap)
    {
        return false;    //colineaire
    }

    P.x = (bp - b) / (a - ap);
    P.y = a * P.x + b;

    //Calculons la 3e coordonnee; on sait que les vecteurs ne sont pas colineaires
    P.z = P1.z + (P.x - P1.x) * (P2.z - P1.z) / (P2.x - P1.x);

    return true;
}
