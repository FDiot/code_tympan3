/*
 * Copyright (C) <2012-2014> <EDF-R&D> <FRANCE>
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

#include <cmath>

#include "Tympan/models/common/3d.h"
#include "sonie_zwicker_1991.h"


const unsigned short sonie::Nbandes3Oct = 28;
const double sonie::RAP[] = {   45, 55, 65, 71, 80, 90, 100, 120    };

const double sonie::DLL[8][11] = {  { -32, -24, -16, -10, -5, 0, -7, -3, 0, -2, 0},
    { -29, -22, -15, -10, -4, 0, -7, -2, 0, -2, 0},
    { -27, -19, -14,  -9, -4, 0, -6, -2, 0, -2, 0},
    { -25, -17, -12,  -9, -3, 0, -5, -2, 0, -2, 0},
    { -23, -16, -11,  -7, -3, 0, -4, -1, 0, -1, 0},
    { -20, -14, -10,  -6, -3, 0, -4, -1, 0, -1, 0},
    { -18, -12,  -9,  -6, -2, 0, -3, -1, 0, -1, 0},
    { -15, -10,  -8,  -4, -2, 0, -3, -1, 0, -1, 0}
};

const double sonie::LTQ[] = {   30, 18, 12, 8, 7, 6, 5, 4, 3, 3,
                                3,  3,  3, 3, 3, 3, 3, 3, 3, 3
                            };

const double sonie::A0[] = {     0,    0,    0,    0,    0,    0,  0,   0,  0,  0,
                                 -0.5, -1.6, -3.2, -5.4, -5.6, -4, -1.5, 2,  5, 12
                           };

const double sonie::DDF[] = {    0,  0,    0.5,  0.9,  1.2, 1.6, 2.3, 2.8, 3,   2,
                                 0, -1.4, -2,   -1.9, -1,   0.5, 3,   4,   4.3, 4
                            };

const double sonie::DCB[] = {   -0.25, -0.6, -0.8, -0.8, -0.5, 0,   0.5, 1.1, 1.5,  1.7,
                                1.8,   1.8,  1.7,  1.6,  1.4, 1.2, 0.8, 0.5, 0,   -0.5
                            };

const double sonie::ZUP[] = {    0.9,  1.8,  2.8,  3.5,  4.4,  5.4,  6.6,  7.9,  9.2, 10.6,
                                 12.3, 13.8, 15.2, 16.7, 18.1, 19.3, 20.6, 21.8, 22.7, 23.6,
                                 24
                            };

const double sonie::RNS[] = {   21.5,  18,    15.1,  11.5,  9,    6.1,  4.4,  3.1,   2.13,
                                1.36,  0.82,  0.42,  0.30, 0.22, 0.15, 0.10, 0.035, 0
                            };

const double sonie::USL[18][8] = {  { 13,    8.2,  6.3,  5.5,  5.5,  5.5,  5.5,  5.5 },
    {  9,    7.5,  6,    5.1,  4.5,  4.5,  4.5,  4.5 },
    {  7.8,  6.7,  5.6,  4.9,  4.4,  3.9,  3.9,  3.9 },
    {  6.2,  5.4,  4.6,  4.0,  3.5,  3.2,  3.2,  3.2 },
    {  4.5,  3.8,  3.6,  3.2,  2.9,  2.7,  2.7,  2.7 },
    {  3.7,  3.0,  2.8,  2.35, 2.2,  2.2,  2.2,  2.2 },
    {  2.9,  2.3,  2.1,  1.9,  1.8,  1.7,  1.7,  1.7 },
    {  2.4,  1.7,  1.5,  1.35, 1.3,  1.3,  1.3,  1.3 },
    {  1.95, 1.45, 1.3,  1.15, 1.1,  1.1,  1.1,  1.1 },
    {  1.5,  1.2,  0.94, 0.86, 0.82, 0.82, 0.82, 0.82 },
    {  0.72, 0.67, 0.64, 0.63, 0.62, 0.62, 0.62, 0.62 },
    {  0.59, 0.53, 0.51, 0.50, 0.42, 0.42, 0.42, 0.42 },
    {  0.40, 0.33, 0.26, 0.24, 0.24, 0.22, 0.22, 0.22 },
    {  0.27, 0.21, 0.20, 0.18, 0.17, 0.17, 0.17, 0.17 },
    {  0.16, 0.15, 0.14, 0.12, 0.11, 0.11, 0.11, 0.11 },
    {  0.12, 0.11, 0.10, 0.08, 0.08, 0.08, 0.08, 0.08 },
    {  0.09, 0.08, 0.07, 0.06, 0.06, 0.06, 0.06, 0.05 },
    {  0.06, 0.05, 0.03, 0.02, 0.02, 0.02, 0.02, 0.02 }
};

sonie::sonie(double* vectToct, const unsigned short& champ /*= 0*/) : VectNiv3Oct(vectToct),
    Champ(champ)
{
    N_Tot = 0.0;
    LN = 0.0;
    N_Specif = new double[240];
    BarkAxis = new double[240];
    _isOk = validation();

    // Vrification des donnes d'entre et execution si ok
    if (_isOk) { exec(); }
}

sonie::~sonie()
{
    delete [] N_Specif;
    N_Specif = 0;
    delete [] BarkAxis;
    BarkAxis = 0;
}

bool sonie::validation()
{
    // Vrification de la valeur de Champ (0 ou 1)
    if ((Champ != 0) && (Champ != 1)) { return false; }

    // Vrification du maximum et du minimum
    for (unsigned short i = 0; i < 28; i++)
    {
        if ((VectNiv3Oct[i] > 120.0) || (VectNiv3Oct[i] < -60.0)) { return false; }
    }

    return true;
}

double sonie::calcIsoSonie(const double& val)
{
    if (val >= 1) // Calcul du niveau d'isosonie pour pour LN >= 40 phones ou N >= 1 sone
    {
        return 10 * ::log10(val) / ::log10(2.0) + 40;
    }
    else // Calcul du niveau d'isosonie pour LN < 40 phones ou N < 1 sone
    {
        double phone = 40 * ::pow((val + 0.0005), 0.35);
        return phone < 3.0 ? 3.0 : phone;
    }

    return 0.0;
}

void sonie::exec()
{
    // Cration et initialisation des tableaux (dans l'ordre de leur mise
    // en oeuvre dans la version MATLAB
    double TI[11]  = {  0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0   };
    double GI[3]   = {  0.0, 0.0, 0.0   };
    double FNGI[3] = {  0.0, 0.0, 0.0   };
    double LCB[3]  = {  0.0, 0.0, 0.0   };
    double LE[20]  = {  0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
                     };
    double NM[21]  = {  0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
                     };
    double NS[240];
    for (unsigned short i = 0; i < 240; i++) { NS[i] = 0.0; } // Initialisation

    // Correction des niveaux des bandes de tiers d'octave d'aprs
    // les niveaux d'isosonie (Xp) et calcul des intensits sonores
    // pour les bandes basses frquences jusqu' 315 Hz
    double XP = 0.0;
    for (unsigned short i = 0, j = 0; i < 11; i++)
    {
        j = 0;
        while ((VectNiv3Oct[i] > (RAP[j] - DLL[j][i])) && j < 7) { j++; }

        XP = VectNiv3Oct[i] + DLL[j][i];
        TI[i] = ::pow(10.0, XP / 10.0);
    }

    // Dtermination des niveaux (LCB) dans les 3 premires bandes critiques
    GI[0] = TI[0] + TI[1] + TI[2] + TI[3] + TI[4] + TI[5];
    GI[1] = TI[6] + TI[7] + TI[8];
    GI[2] = TI[9] + TI[10];

    for (unsigned short i = 0; i < 3; i++) { FNGI[i] = 10 * ::log10(GI[i]); }

    for (unsigned short i = 0; i < 3; i++)
    {
        if (GI[i] > 0.0) { LCB[i] = FNGI[i]; }
    }

    // Calcul de la sonie principale

    const unsigned short NbandesCritiques = 20;
    const double S = 0.25;
    const double BarkStep = 0.1;

    for (unsigned short i = 0; i < NbandesCritiques + 1; i++)
    {
        LE[i] = VectNiv3Oct[i + 8];

        if (i < 3) { LE[i] = LCB[i]; }

        LE[i] = LE[i] - A0[i];
        NM[i] = 0; // Cette ligne est elle utile dans la mesure ou NM a t initialise ?

        if (Champ == 1) { LE[i] = LE[i] + DDF[i]; }

        if (LE[i] > LTQ[i])
        {
            LE[i] = LE[i] - DCB[i];
            double MP1   = 0.0635 * ::pow(10.0, 0.025 * LTQ[i]);
            double MP2_1 = ::pow(10.0, (LE[i] - LTQ[i]) / 10);
            double MP2   = ::pow((1.0 - S + (S * MP2_1)), 0.25) - 1.0;

            NM[i] = MP1 * MP2;

            if (NM[i] <= 0.0) { NM[i] = 0.0; }
        }
    }

    // Correction sur la sonie spcifique dans la premire bande critique en prenant
    // en compte la dpendance au seuil absolu d'audition pour le niveau de cette bande
    double KORRY = 0.4 + (0.32 * ::pow(NM[0], 0.2));
    if (KORRY > 1.0) { KORRY = 1.0; }

    NM[0] = NM[0] * KORRY;

    // Dfinition de la valeur initiale
    unsigned short IZ = 0;
    unsigned short IG = 0;
    double Z  = 0.1;
    double Z1 = 0.0;
    double Z2 = 0.0;
    double DZ = 0.0;
    double N  = 0.0;
    double N1 = 0.0;
    double N2 = 0.0;
    double k  = 0.0;
    double ZUPI = 0.0;

    // valeurs pour toutes les bandes critiques
    for (unsigned short i = 0, j = 0; i < NbandesCritiques; i++)
    {
        ZUPI = ZUP[i] + 0.0001;
        if (i > 0) { IG = i - 1; } // A vrifier
        if (IG > 7) { IG = 7; } // Les valeurs des pentes (USL) pour les bandes suprieures  l'indice 8 sont identiques

        while (Z1 < ZUPI)
        {
            if (N1 <= NM[i]) // pour les parties plates de la courbe de sonie spcifique
            {
                if (N1 < NM[i])
                {
                    j = 0;
                    // on dtermine ici le niveau  considrer pour choisir la pente
                    // (pour les cas potentiels suivants ou N1 > NM(i))
                    while ((RNS[j] > NM[i]) && (j < 17)) { j++; }
                }

                Z2 = ZUPI;
                N2 = NM[i];
                N  = N + N2 * (Z2 - Z1);
                k = Z; // Initialisation de k

                while (k <= Z2)
                {
                    NS[IZ] = N2;
                    IZ = IZ + 1;
                    k = k + BarkStep;
                }

                Z = k;
            }
            else if (N1 > NM[i])
            {
                N2 = RNS[j];

                if (N2 < NM[i]) { N2 = NM[i]; }

                DZ = (N1 - N2) / USL[j][IG];
                Z2 = Z1 + DZ;

                if (Z2 > ZUPI)
                {
                    Z2 = ZUPI;
                    DZ = Z2 - Z1;
                    N2 = N1 - DZ * USL[j][IG];
                }
                N = N + DZ * (N1 + N2) / 2;
                k = Z; // Initialisation de k

                while (k <= Z2)
                {
                    NS[IZ] = N1 - (k - Z1) * USL[j][IG];
                    IZ     = IZ + 1;
                    k      = k + BarkStep;
                }

                Z = k;
            }

            while ((N2 <= RNS[j]) && (j < 17)) { j++; }

            if ((N2 <= RNS[j]) && (j >= 17)) { j = 17; }

            // Initialisation des variables N1 et Z1 pour l'itration suivante.
            Z1 = Z2;
            N1 = N2;
        }
    }

    if (N < 0.0) { N = 0.0; }

    // On arrondit la valeur a 10^(-3) prs
    if (N <= 16.0)
    {
        N = static_cast<double>(ROUND(N * 1000)) / 1000.0; // On ajoute pas O.5 car c'est fait par ROUND
    }
    else
    {
        N = static_cast<double>(ROUND(N * 100)) / 100.0; // On ajoute pas O.5 car c'est fait par ROUND
    }

    // Recopie de NS dans N_Specif
    for (unsigned short i = 0; i < 240; i++) { N_Specif[i] = NS[i]; }

    N_Tot = N;

    // Calcul du niveau d'isosonie (en phones)
    LN = calcIsoSonie(N_Tot);
}
