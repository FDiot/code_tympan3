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

#ifndef TYSET_GEOMETRIQUE_PARCOURS_H
#define TYSET_GEOMETRIQUE_PARCOURS_H

#include "Tympan/core/macros.h"
#include "TYPointParcours.h"
#include "TYPolyligneParcours.h"

//On devrait pouvoir etendre le nombre de point sur (2pow31) -1, vue que les indentifiants sont codes sur des int:
#define MAX_POINTS  32767

//Afin d'eviter des bugs de synchronisation de differents listes de points,
//traitement basee sur les identifiants (SelectionnePointsEntreSetRetDuCoteDeSR,...),
//on marque les identifiants des points particuliers Source et Recepteur,
//provenant en outre toujours d'une liste separee
#define  INDENTIFIANT_SOURCE    MAX_POINTS
#define  INDENTIFIANT_RECEPTEUR (MAX_POINTS-1)

struct Connexite
{
    int IndexesSegment[2];
    int NbSegmentsConnexes;
};

class TYSetGeometriqueParcours
{
public:
    TYPolyligneParcours* _ListePolylines;
    TYPointParcours* _ListePoint;
    int _nNbPolylines;
    int _nNbPolylineAllouee;
    int _nNbPointTotal;

    //static TYPointParcours* _ListePointQSort;//static pour acces d'une fonction C de quicksort
    //static TYPointParcours* _SrceQSort;//static pour acces d'une fonction C de quicksort
    //static TYPointParcours* _DestQSort;//static pour acces d'une fonction C de quicksort
    //static QMutex _mutex;

    TYSetGeometriqueParcours() { Init();}
    ~TYSetGeometriqueParcours() { Clean();}
    //NB: PolyligneP0 & PolyligneP1 ne sont pas copies:
    void Copy(TYSetGeometriqueParcours& geoIn);

    void AllouerPolylignes(int nNbPolylineAllouee)
    {
        _nNbPolylineAllouee = nNbPolylineAllouee;
        _ListePolylines = new TYPolyligneParcours[nNbPolylineAllouee];
    };
    bool PolylignesInfraFermees();
    int MergePointsDoubles();
    int SupprimeLesPointsDoubles();
    int SupressionPolylignesRedondantes();
    void SeparationDroiteGauche(bool* PointsAGauche, bool* PointsADroite, TYSetGeometriqueParcours& geoGauche, TYSetGeometriqueParcours& geoDroite);
    void MarquePointsADroiteEtAGauche(TYPointParcours& Srce, TYPointParcours& Dest, bool*& PointsAGauche, bool*& PointsADroite);
    void RamenerPointsTraversantLaFrontiere(TYPointParcours& Srce, TYPointParcours& Dest, TYPointParcours** IndexePointsFrontiere, int& NbPointsFrontiere, bool* EstUnPointIntersectant, bool bCoteGauche, bool* PointsAGauche, bool* PointsADroite);
    bool ListerPointsConnexes(Connexite* Connexes);
//    void AffichePolylignes();

    bool PremierePasse(TYPointParcours& Srce, TYPointParcours& Dest, TYPointParcours** IndexePointsFrontiere, int NbPointsFrontiere, bool* EstUnPointIntersectant, Connexite* Connexes, TYSetGeometriqueParcours& geoPremierePasse);
    bool SecondePasse(TYSetGeometriqueParcours& geoPremierePasse, TYSetGeometriqueParcours& geoSecondePasse, bool bTrajetsAGaucheDeSR, TYPointParcours** & pTableauEC, int& nbPtsEC);

    void TriePointsIntersectionSuivantSR(TYPointParcours& Srce, TYPointParcours& Dest, TYPointParcours** IndexePointsFrontiere, int NbPointsFrontiere);
    bool AjoutePointALaPolyLigne(int indexPolyligne, TYPointParcours& P);
    int AjouteLesPointsComprisEntre(TYSetGeometriqueParcours& geoPolySource, int nIndexePoly, int nIndexeNbPremierPointAAjouter, int nIndexeDernierPointAAjouter);
    int ParcourtPolyligneAPartirDe(int IndexPointRacine, TYPolyligneParcours*& PolyligneRacine, bool* EstUnPointIntersectant, TYSetGeometriqueParcours& geoPremierePasse);
    bool intersects(TYPointParcours& P1, TYPointParcours& P2);

    int NbreRefPoint()
    {
        int nNbRefPoint = 0;
        //Nombre de reference aux points dans les polylignes
        for (int i = 0; i < _nNbPolylines; i++)
        {
            nNbRefPoint += _ListePolylines[i].nombreDePoint();
        }
        return nNbRefPoint;
    }

    //Calcul l'enveloppe convexe d'un tableau de point ["TableauDePoints"/nNbPoints], sachant que les premiers de ce tableaux sont les plus bas;
    //le resultat figure dans TableauDePointsECOut; ne nombre renvoye est celui de l'enveloppe convexe.
    //Attention ! Les tableaux doivent etre alloues !
    static int  EnveloppeConvexeLes2PremiersPointsEtant(TYPointParcours** TableauDePoints, int nNbPoints, TYPointParcours** TableauDePointsECOut, bool bPremiersPointsLesPlusHauts);

    //La selection comprend SR:
    int     SelectionnePointsEntreSetRetDuCoteDeSR(TYSetGeometriqueParcours* geoSR, TYPointParcours** TableauDePoints, int nNbPoints);

    //N'est utilise que pour les trajets verticaux
    void    CreerTrajetAPartirDuneListeDePointsTriee(TYPointParcours** TableauDePoints, int nNbPoints, bool bSens, bool bGardeIdentifiant);

    bool AppartienneMemePolyligne(TYPointParcours* a, TYPointParcours* b, TYPointParcours* c);

private:
    //int MergePointsDoubles(bool bLexico);
    void SwapPolyligne(int i, int j);
    void Clean()
    {
        if (_ListePolylines)
        {
            SAFE_DELETE_LIST(_ListePolylines);
        }
        if (_ListePoint)
        {
            SAFE_DELETE_LIST(_ListePoint);
        }
    }
    void Init()
    {
        _ListePolylines = NULL;
        _ListePoint = NULL;
        _nNbPolylines = 0;
        _nNbPointTotal = 0;
        _nNbPolylineAllouee = 0;
    }
    static void InverseOrdreDesPoints(TYPointParcours** ListeDePointsAInverser, int nNbPointsDeLaListe);

};

#endif// TYSET_GEOMETRIQUE_PARCOURS_H
