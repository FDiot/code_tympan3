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


#include "TYAcousticPathFinder.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYCalcul.h"
#include "Tympan/MetierSolver/DataManagerMetier/Parcours/TYCalculParcours.h"
#include "TYSolver.h"

TYAcousticPathFinder::TYAcousticPathFinder(TYSolver& solver)
    : _solver(solver),
      _bCalcTrajetHorizontaux(false)
{

}

TYAcousticPathFinder::~TYAcousticPathFinder()
{

}

void TYAcousticPathFinder::init(TYCalcul& calcul)
{
    _bCalcTrajetHorizontaux = calcul.getCalculTrajetsHorizontaux();
}

void TYAcousticPathFinder::computePath(const TYSIntersection* tabIntersect, const OSegment3D& rayon, TYTabPoint3D& ptsTop, TYTabPoint3D& ptsLeft, TYTabPoint3D& ptsRight, unsigned int nNbTrajet)
{
    bool dessus = true, lateral = false;

    // Determination du parcours et calcul des chemins
    //1. Parcours vertical
    //le dernier ptsTop n'est pas affecte
    computeParcoursLateraux(tabIntersect, rayon, dessus, ptsTop, ptsTop, nNbTrajet);

    // Parcours gauche & droits si necessaire
    computeParcoursLateraux(tabIntersect, rayon, lateral, ptsLeft, ptsRight, nNbTrajet);
}

bool TYAcousticPathFinder::computeParcoursLateraux(const TYSIntersection* tabIntersect, const OSegment3D& rayon, const bool& dessus, TYTabPoint3D& ptsLeft, TYTabPoint3D& ptsRight, unsigned int nNbTrajet) const
{
    // Si on est en trajet lateral non demande par le calcul
    if (!_bCalcTrajetHorizontaux && !dessus) { return false; }

    unsigned int nbFaces = static_cast<uint32>(_solver.getTabPolygonSize());

    short indiceIntersect = dessus ? 0 : 1;
    TYCalculParcours Parcours(nbFaces, dessus != 0); //dessus = vue de cote
    Parcours.NumPlanCoupe = nNbTrajet;

    //Quoiqu'il arrive, on ajoute la source:
    ptsLeft.push_back(rayon._ptA);
    //Si on a une vue de haut, on doit aussi calculer les trajets a droite:
    if (!dessus)
    {
        ptsRight.push_back(rayon._ptA);
    }

    TYTabPoint3D ptsTemp;

    // Creation du segment SR
    Parcours.AjouterSegmentSR(const_cast<double*>(rayon._ptA._value), const_cast<double*>(rayon._ptB._value));

    for (unsigned int i = 0 ; i < nbFaces; i++)
    {
        if (tabIntersect[i].bIntersect[indiceIntersect])
        {
            const TYStructSurfIntersect& SI = _solver.getTabPolygon()[i];
            if (SI.tabPoint.size() == 0) { continue; }
            Parcours.AjouterSegmentCoupe((double*)tabIntersect[i].segInter[indiceIntersect]._ptA._value,
                                         (double*)tabIntersect[i].segInter[indiceIntersect]._ptB._value,
                                         SI.isInfra, SI.isEcran);
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
