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


#include "Tympan/models/business/TYCalcul.h"
#include "Tympan/models/solver/config.h"
#include "Tympan/solvers/ConvexHullFinder/TYCalculParcours.h"
#include "TYSolver.h"
#include "TYAcousticPathFinder.h"

TYAcousticPathFinder::TYAcousticPathFinder(TYSolver& solver)
    : _solver(solver),
      _bCalcTrajetHorizontaux(false)
{
}

TYAcousticPathFinder::~TYAcousticPathFinder()
{
}

void TYAcousticPathFinder::init()
{
    _bCalcTrajetHorizontaux = tympan::SolverConfiguration::get()->UseLateralDiffraction;
}

void TYAcousticPathFinder::computePath(const std::deque<TYSIntersection>& tabIntersect, const OSegment3D& rayon, TabPoint3D& ptsTop, TabPoint3D& ptsLeft, TabPoint3D& ptsRight)
{
    bool dessus = true, lateral = false;

    // Determination du parcours et calcul des chemins
    //1. Parcours vertical
    //le dernier ptsTop n'est pas affecte
    computeParcoursLateraux(tabIntersect, rayon, dessus, ptsTop, ptsTop);

    // Parcours gauche & droits si necessaire
    computeParcoursLateraux(tabIntersect, rayon, lateral, ptsLeft, ptsRight);
}

bool TYAcousticPathFinder::computeParcoursLateraux(const std::deque<TYSIntersection>& tabIntersect, const OSegment3D& rayon, const bool& dessus, TabPoint3D& ptsLeft, TabPoint3D& ptsRight) const
{
    // Si on est en trajet lateral non demande par le calcul
    if (!_bCalcTrajetHorizontaux && !dessus) { return false; }

    unsigned int nbFaces = tabIntersect.size();

    short indiceIntersect = dessus ? 0 : 1;
    TYCalculParcours Parcours(nbFaces, dessus != 0); //dessus = vue de cote

    //Quoiqu'il arrive, on ajoute la source:
    ptsLeft.push_back(rayon._ptA);
    //Si on a une vue de haut, on doit aussi calculer les trajets a droite:
    if (!dessus)
    {
        ptsRight.push_back(rayon._ptA);
    }

    // Creation du segment SR
    Parcours.AjouterSegmentSR(const_cast<double*>(rayon._ptA._value), const_cast<double*>(rayon._ptB._value));

    for (unsigned int i = 0 ; i < nbFaces; i++)
    {
        if (tabIntersect[i].bIntersect[indiceIntersect])
        {
            TYSIntersection inter = tabIntersect[i];
            Parcours.AjouterSegmentCoupe((double*)inter.segInter[indiceIntersect]._ptA._value,
                                         (double*)inter.segInter[indiceIntersect]._ptB._value,
                                         inter.isInfra, inter.isEcran);
        }
    }

    // Traitement du parcours
    Parcours.Traitement();

    //La liste a retourner ne contient pas le dernier point (Recepteur)
    //On retourne 2 listes si on est en vue de haut (parcours lateraux),
    //sinon une seule (vue de face => trajet par dessus les obstacles)
    double dPointTemp[3];
    //Que l'on soit en vue de haut ou non, il faut le trajet gauche
    int nNbrePointsTrajetGauche = Parcours.NombrePointsTrajetGauche();
    OPoint3D pt;
    for (int j = 1; j < (nNbrePointsTrajetGauche - 1); j++) //-1: pas de recepteur en sortie; 1 car la source a deja ete ajoutee
    {
        Parcours.PointTrajetGauche(j, dPointTemp);
        pt._x = dPointTemp[0];
        pt._y = dPointTemp[1];
        pt._z = dPointTemp[2];//x y z
        ptsLeft.push_back(pt);
    }

    if (!dessus)
    {
        int nNbrePointsTrajetDroite = Parcours.NombrePointsTrajetDroite();
        OPoint3D pt;
        for (int j = 1; j < (nNbrePointsTrajetDroite - 1); j++) //-1: pas de recepteur en sortie; 1 car la source a deja ete ajoutee
        {
            Parcours.PointTrajetDroite(j, dPointTemp);
            pt._x = dPointTemp[0];
            pt._y = dPointTemp[1];
            pt._z = dPointTemp[2];//x y z
            ptsRight.push_back(pt);
        }
    }

    return true;
}
