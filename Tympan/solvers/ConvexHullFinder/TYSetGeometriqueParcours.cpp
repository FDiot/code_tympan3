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



#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Tympan/models/common/3d.h"
#include "TYSetGeometriqueParcours.h"
#include "TYPointParcours.h"

//Return Value Description for Qsort compare
//< 0 elem1 less than elem2
//0 elem1 equivalent to elem2
//> 0 elem1 greater than elem2
int compareTYPolyligneParcours(const void* p1, const void* p2)
{
    TYPolyligneParcours* P1 = (TYPolyligneParcours*)p1;
    TYPolyligneParcours* P2 = (TYPolyligneParcours*)p2;
    int indexPointPoly1 = P1->indexePoint1();
    int indexPointPoly2 = P2->indexePoint1();
    //assert(indexPointPoly1 >= 0);
    //assert(indexPointPoly2 >= 0);
    //Si les 2 premiers indexes sont egaux, on regarde les suivants:
    if (indexPointPoly1 == indexPointPoly2)
    {
        indexPointPoly1 = P1->indexePoint2();
        indexPointPoly2 = P2->indexePoint2();
    }
    return (indexPointPoly1 - indexPointPoly2);
}

void TYSetGeometriqueParcours::Copy(TYSetGeometriqueParcours& geoIn)
{
    //Copie des points
    _ListePoint     = new TYPointParcours[geoIn._nNbPointTotal];
    _nNbPointTotal = geoIn._nNbPointTotal;
    int i, j;
    for (i = 0; i < _nNbPointTotal; i++)
    {
        _ListePoint[i] = geoIn._ListePoint[i];
    }
    //Copie des polylignes
    _ListePolylines = new TYPolyligneParcours[geoIn._nNbPolylines];
    _nNbPolylines = geoIn._nNbPolylines;
    for (i = 0; i < _nNbPolylines; i++)
    {
        _ListePolylines[i].allouer(geoIn._ListePolylines[i].nombreDePoint());
        for (j = 0; j < geoIn._ListePolylines[i].nombreDePoint(); j++)
        {
            //Indexe du point dans la liste de point:
            int indexePoint = geoIn._ListePolylines[i].indexePoint(j);
            //Copie de la reference:
            _ListePolylines[i].ajoutePoint(j, &(_ListePoint[indexePoint]));
        }
    }
}

void TYSetGeometriqueParcours::SwapPolyligne(int i, int j)
{
    TYPolyligneParcours tmp;
    tmp = _ListePolylines[i];
    _ListePolylines[i] = _ListePolylines[j];
    _ListePolylines[j] = tmp;
}

int TYSetGeometriqueParcours::SupressionPolylignesRedondantes()
{
    int nNbDoublons = 0;
    //Cette methode ellimine 2 sortes de segments:
    //1. segments bouclant sur le meme point
    //2. segments doubles (Ex: [P1,P2] & [P2,P1])
    //Pour ce traitement, on trie la liste de polyligne de 2 facons:
    //1. l'indice du premier point doit etre le plus faible (swap au besoin dans chaque polyligne)
    //2. suivant la valeur du premier indice

    //1. Swap des indexes de points si necessaire
    int indexPoint1, indexPoint2, i, j;
    for (i = 0; i < _nNbPolylines; i++)
    {
        //assert(_ListePolylines[i].nombreDePoint() == 2);
        indexPoint1 = _ListePolylines[i].indexePoint1();
        indexPoint2 = _ListePolylines[i].indexePoint2();
        //assert(indexPoint1 >= 0);
        //assert(indexPoint2 >= 0);
        if (indexPoint1 > indexPoint2)
        {
            //Swap
            _ListePolylines[i].setPoint(0, &(_ListePoint[indexPoint2]));
            _ListePolylines[i].setPoint(1, &(_ListePoint[indexPoint1]));
        }
    }
    //2. QSort sur les polylignes (base sur le fait que toutes les polylignes contiennent 2 points exactement)
    qsort((void*)_ListePolylines, (size_t)_nNbPolylines, sizeof(TYPolyligneParcours), compareTYPolyligneParcours);

    //3."Suppression" des segments ayant les memes indexes
    i = 0;
    j = 1;
    int nOldNbPolylines = _nNbPolylines;
    while (i < _nNbPolylines)
    {
        indexPoint1 = _ListePolylines[i].indexePoint1();
        indexPoint2 = _ListePolylines[i].indexePoint2();
        while (j <   nOldNbPolylines &&
               indexPoint1 == _ListePolylines[j].indexePoint1() &&
               indexPoint2 == _ListePolylines[j].indexePoint2())
        {
            _nNbPolylines--;
            nNbDoublons++;
            j++;
        }
        i++;
        if (i != j && j < nOldNbPolylines)
        {
            _ListePolylines[i].Copy(_ListePolylines[j]);
        }
        j++;
    }

    //4."Suppression" des segments bouclant sur le meme point
    for (i = 0; i < _nNbPolylines; i++)
    {
        indexPoint1 = _ListePolylines[i].indexePoint1();
        indexPoint2 = _ListePolylines[i].indexePoint2();
        if (indexPoint1 == indexPoint2)
        {
            if (i != (_nNbPolylines - 1))
            {
                _ListePolylines[i].Copy(_ListePolylines[_nNbPolylines - 1]);
            }
            i--;//reverifier si la nouvelle n'est pas redondante
            _nNbPolylines--;
            nNbDoublons++;
        }
    }

    return nNbDoublons;
}

int TYSetGeometriqueParcours::MergePointsDoubles()
{
    int i, j, index, doublons = 0;
    //1. On modifie les identifiants des points doubles (on suppose qu'ils sont dans l'ordre):
    for (i = 0; i < _nNbPointTotal; i++)
    {
        for (j = i; j < _nNbPointTotal; j++)
        {
            //Ne pas merger les points appartenant a 2 types de courbes (infra & topo)
            bool bPointAppartenantAuMemeType = ((_ListePoint[i].isEcran == _ListePoint[j].isEcran) && (_ListePoint[i].isInfra == _ListePoint[j].isInfra));
            if ((bPointAppartenantAuMemeType) && (_ListePoint[j].Identifiant > i) && (TYPointParcours::Confondus(&(_ListePoint[i]), &(_ListePoint[j]))))
            {
                _ListePoint[j].Identifiant = -i;//les points inutilises ont des indexes negatifs
                doublons++;
            }
        }
    }
    //2. On modifie les ptr sur points des polylignes:
    for (i = 0; i < _nNbPolylines; i++)
    {
        for (j = 0; j < _ListePolylines[i].nombreDePoint(); j++)
        {
            index = _ListePolylines[i].indexePoint(j);
            do//attention, le point peut faire reference a un point lui-meme doublon (donc ayant un id negatif)
            {
                index = abs(index);
                index = _ListePoint[index].Identifiant;
            }
            while (index < 0);
            _ListePolylines[i].setPoint(j, &(_ListePoint[index]));
        }
    }
    return doublons;
}

void TYSetGeometriqueParcours::RamenerPointsTraversantLaFrontiere(TYPointParcours& Srce, TYPointParcours& Dest, TYPointParcours** IndexePointsFrontiere, int& NbPointsFrontiere, bool* EstUnPointIntersectant, bool bCoteGauche, bool* PointsAGauche, bool* PointsADroite)
{
    int i, indexePoint, indexePoint1, indexePoint2;
    int nAncienNbPointTotal = _nNbPointTotal / 2; //cf SeparationDroiteGauche
    //Initialisation
    NbPointsFrontiere = 0;
    for (i = 0; i < _nNbPointTotal; i++)
    {
        EstUnPointIntersectant[i] = false;
    }

    for (i = 0; i < _nNbPolylines; i++)
    {
        //1. Recherche des segments traversant [SR]
        //Attention !! On considere que les polylignes ne sont que des segments (nb point = 2).
        //assert(2 == _ListePolylines[i].nombreDePoint());
        //Indexe du point dans la liste de point:
        indexePoint1 = _ListePolylines[i].indexePoint1();
        indexePoint2 = _ListePolylines[i].indexePoint2();
        //Est-ce qu'un des points est des deux cotes ?
        if (PointsAGauche[indexePoint1] == PointsADroite[indexePoint1])
        {
            //Oui le marquer
            EstUnPointIntersectant[indexePoint1] = true;
            //            IndexePointsFrontiere[NbPointsFrontiere] = indexePoint1;
            IndexePointsFrontiere[NbPointsFrontiere] = &_ListePoint[indexePoint1];
            NbPointsFrontiere++;
            continue;
        }
        if (PointsAGauche[indexePoint2] == PointsADroite[indexePoint2])
        {
            //Oui le marquer
            EstUnPointIntersectant[indexePoint2] = true;
            //            IndexePointsFrontiere[NbPointsFrontiere] = indexePoint2;
            IndexePointsFrontiere[NbPointsFrontiere] = &_ListePoint[indexePoint2];
            NbPointsFrontiere++;
            continue;
        }
        //Y a-t-il passage de frontiere ?
        bool b2PointsAGauche = PointsAGauche[indexePoint1] && PointsAGauche[indexePoint2];
        bool b2PointsADroite = PointsADroite[indexePoint1] && PointsADroite[indexePoint2];
        if (b2PointsAGauche || b2PointsADroite)
        {
            continue;    //non
        }
        int nIndexePointFrontiereDansSegment;
        if (bCoteGauche)
        {
            nIndexePointFrontiereDansSegment = PointsADroite[indexePoint1] ? 0 : 1;
        }
        else
        {
            nIndexePointFrontiereDansSegment = PointsAGauche[indexePoint1] ? 0 : 1;
        }
        indexePoint = _ListePolylines[i].indexePoint(nIndexePointFrontiereDansSegment);

        //2. Modification du point donnant lieu a un point frontiere
        //Ce passage de frontiere peut donner lieu a 2 points d'intersections sur SR,
        //si une autre polyligne rejoint ce point (indexePoint) de l'autre ci申te
        //=> on cree un nouveau point ayant les memes coordonnees:
        _ListePoint[nAncienNbPointTotal] = _ListePoint[indexePoint];
        _ListePoint[nAncienNbPointTotal].Identifiant = nAncienNbPointTotal;

        //Modifier la reference dans la polyligne
        _ListePolylines[i].setPoint(nIndexePointFrontiereDansSegment, &(_ListePoint[nAncienNbPointTotal]));
        indexePoint = nAncienNbPointTotal;
        nAncienNbPointTotal++;

        //Marquer le point frontiere:
        //IndexePointsFrontiere[NbPointsFrontiere] = indexePoint;
        IndexePointsFrontiere[NbPointsFrontiere] = &_ListePoint[indexePoint];

        EstUnPointIntersectant[indexePoint] = true;

        //Calculer l'intersection avec [SR]:
        TYPointParcours::IntersectionDroites(Srce, Dest, _ListePoint[indexePoint1], _ListePoint[indexePoint2], _ListePoint[indexePoint]);
        NbPointsFrontiere++;
    }
}

void TYSetGeometriqueParcours::MarquePointsADroiteEtAGauche(TYPointParcours& Srce, TYPointParcours& Dest, bool*& PointsAGauche, bool*& PointsADroite)
{
    //1. Etablir une liste des points a droite, une autre de ceux a gauche
    //Rq: on a 2 listes car on prend aussi les points sur la frontiere
    //Inversion S-R si necessaire
    //bool bSAGaucheDeR = ListePointSR[0].x < ListePointSR[1].x;
    // Signe de l'angle
    //sign = (vec1.cross(vec2)._z > 0) ? -1 : 1;
    PointsAGauche = new bool[_nNbPointTotal];
    PointsADroite = new bool[_nNbPointTotal];
    TYPointParcours SR = Dest;
    SR.x -= Srce.x;
    SR.y -= Srce.y;
    for (int i = 0; i < _nNbPointTotal; i++)
    {
        //Est-ce un point racine ?
        if (_ListePoint[i].Identifiant == i)
        {
            TYPointParcours SP = _ListePoint[i];
            SP.x -= Srce.x;
            SP.y -= Srce.y;
            PointsAGauche[i] = TYPointParcours::ZCross(SR, SP) > 0;
            PointsADroite[i] = TYPointParcours::ZCross(SR, SP) < 0;
        }
    }
}

void TYSetGeometriqueParcours::SeparationDroiteGauche(bool* PointsAGauche, bool* PointsADroite, TYSetGeometriqueParcours& geoGauche, TYSetGeometriqueParcours& geoDroite)
{
    int i, j, indexePoint;
    //2.  Marquer les polylignes oi申 au moins un point est a gauche (idem pour la droite)
    bool* bAuMoinsUnPointAGauche = new bool[_nNbPolylines];
    bool* bAuMoinsUnPointADroite = new bool[_nNbPolylines];
    for (i = 0; i < _nNbPolylines; i++)
    {
        bAuMoinsUnPointAGauche[i] = false;
        bAuMoinsUnPointADroite[i] = false;
        //Cette polyligne a-t-elle au moins un point a gauche (/ droite)?
        for (j = 0; j < _ListePolylines[i].nombreDePoint() && (!bAuMoinsUnPointADroite[i] || !bAuMoinsUnPointAGauche[i]); j++)
        {
            indexePoint = _ListePolylines[i].indexePoint(j);
            bAuMoinsUnPointAGauche[i] = bAuMoinsUnPointAGauche[i] || PointsAGauche[indexePoint];
            bAuMoinsUnPointADroite[i] = bAuMoinsUnPointADroite[i] || PointsADroite[indexePoint];
        }
    }
    //3. Les listes (geo) gauche, doite et principale doivent etre independantes
    //=>on en cree de nouvelles
    //Copie des points
    //facteur 2 : precaution due au fait que les points a ramener a la frontiere peuvent donner 2 points d'intersection
    geoGauche._ListePoint       = new TYPointParcours[2 * _nNbPointTotal];
    geoDroite._ListePoint       = new TYPointParcours[2 * _nNbPointTotal];
    geoGauche._nNbPointTotal = 2 * _nNbPointTotal;
    geoDroite._nNbPointTotal = 2 * _nNbPointTotal;
    for (i = 0; i < _nNbPointTotal; i++)
    {
        geoGauche._ListePoint[i] = _ListePoint[i];
        geoDroite._ListePoint[i] = _ListePoint[i];
    }
    //Copie des polylignes
    geoGauche._ListePolylines   = new TYPolyligneParcours[_nNbPolylines];
    geoDroite._ListePolylines   = new TYPolyligneParcours[_nNbPolylines];
    geoGauche._nNbPolylines = 0;
    geoDroite._nNbPolylines = 0;
    for (i = 0; i < _nNbPolylines; i++)
    {
        if (bAuMoinsUnPointAGauche[i])
        {
            //On ajoute cette polyligne
            geoGauche._ListePolylines[geoGauche._nNbPolylines].allouer(_ListePolylines[i].nombreDePoint());
            //Copie des references de point
            for (j = 0; j < _ListePolylines[i].nombreDePoint(); j++)
            {
                //Indexe du point dans la liste de point:
                indexePoint = _ListePolylines[i].indexePoint(j);
                //Copie de la reference:
                geoGauche._ListePolylines[geoGauche._nNbPolylines].ajoutePoint(j, &(geoGauche._ListePoint[indexePoint]));
            }
            geoGauche._nNbPolylines++;
        }
        if (bAuMoinsUnPointADroite[i])
        {
            //On ajoute cette polyligne
            geoDroite._ListePolylines[geoDroite._nNbPolylines].allouer(_ListePolylines[i].nombreDePoint());
            //Copie des references de point
            for (j = 0; j < _ListePolylines[i].nombreDePoint(); j++)
            {
                //Indexe du point dans la liste de point:
                indexePoint = _ListePolylines[i].indexePoint(j);
                //Copie de la reference:
                geoDroite._ListePolylines[geoDroite._nNbPolylines].ajoutePoint(j, &(geoDroite._ListePoint[indexePoint]));
            }
            geoDroite._nNbPolylines++;
        }
    }
    //CherchePointsAGauche(PointsAGauche, geoGauche._ListePolylines, geoGauche._nNbPolylines, geoGauche._ListePoint, geoGauche._nNbPointTotal);
    SAFE_DELETE_LIST(bAuMoinsUnPointAGauche);
    SAFE_DELETE_LIST(bAuMoinsUnPointADroite);
}

//Return Value Description for Qsort compare
//< 0 elem1 less than elem2
//0 elem1 equivalent to elem2
//> 0 elem1 greater than elem2
int compareAbscissesCurvilignes(const void* p1, const void* p2)
{
    double AP1 = ((TYPointParcours*) p1)->_abcisse;
    double AP2 = ((TYPointParcours*) p2)->_abcisse;
    if (AP1 < AP2) { return -1; }
    if (AP1 > AP2) { return  1; }
    return 0;
}

void TYSetGeometriqueParcours::TriePointsIntersectionSuivantSR(TYPointParcours& Srce, TYPointParcours& Dest, TYPointParcours** IndexePointsFrontiere, int NbPointsFrontiere)
{
    if (NbPointsFrontiere == 0) { return; }

    //Quick Sort
    qsort((void*)IndexePointsFrontiere, (size_t)NbPointsFrontiere, sizeof(TYPointParcours *), compareAbscissesCurvilignes);
}

bool TYSetGeometriqueParcours::AjoutePointALaPolyLigne(int indexPolyligne, TYPointParcours& P)
{
    //assert(_ListePolylines);
    //assert(_ListePoint);
    //Enregistrement du point:
    _ListePoint[_nNbPointTotal] = P;
    _ListePolylines[indexPolyligne].ajoutePoint(_ListePolylines[indexPolyligne].nombreDePoint(), &(_ListePoint[_nNbPointTotal]));
    _nNbPointTotal++;
    return true;
}

int TYSetGeometriqueParcours::ParcourtPolyligneAPartirDe(int IndexPointRacine, TYPolyligneParcours*& PolyligneRacine, bool* EstUnPointIntersectant, TYSetGeometriqueParcours& geoPremierePasse)
{
    int IndexPoint = IndexPointRacine;
    int IndexPointSuivant;
    //Cette methode ajoute les points rencontres sur la polyligne racine, dans le sens impose par le point racine
    //Le point racine est-il le premier ou le second point de la polyligne racine ?
    TYPolyligneParcours* PolyligneCourante = PolyligneRacine;
    TYPolyligneParcours* PolyligneSuivante = PolyligneCourante;
    do
    {
        //Memorisons la derniere polyligne non nulle:
        PolyligneRacine = PolyligneCourante;
        IndexPointSuivant = PolyligneCourante->indexePointSuivant(IndexPoint, PolyligneSuivante);
        //Enregistrer l'autre point du segment
        geoPremierePasse.AjoutePointALaPolyLigne(0, _ListePoint[IndexPointSuivant]);
        IndexPoint = IndexPointSuivant;
    }
    while (NULL != PolyligneSuivante && !EstUnPointIntersectant[IndexPoint] && (PolyligneCourante = PolyligneSuivante));

    return IndexPoint;
}


bool TYSetGeometriqueParcours::PolylignesInfraFermees()
{
    //Verifier que toutes les polylignes d'infra sont fermees, exceptees les ecrans
    for (int i = 0; i < _nNbPolylines; i++)
    {
        if (_ListePolylines[i].isInfra() && !_ListePolylines[i].isEcran())
        {
            if (!_ListePolylines[i].estSurUnParcourFermee())
            {
                return false;
            }
        }
    }
    return true;
}

bool TYSetGeometriqueParcours::ListerPointsConnexes(Connexite* Connexes)
{
    //Cette methode rempli la structure Connexes, attribut de chaque point:
    int i, j;
    //Initialiser la liste
    for (i = 0; i < this->_nNbPointTotal; i++)
    {
        Connexes[i].IndexesSegment[0] = -1;//Premier segment incluant ce point
        Connexes[i].IndexesSegment[1] = -1;//Second segment incluant ce point
        Connexes[i].NbSegmentsConnexes = 0;//Nb de segment incluant ce point
    }
    //Pour chaque polyligne, construire une liste de points connexes :
    //on renseigne les champs NbSegmentsConnexes & IndexesSegment
    int indexePoint;
    for (i = 0; i < _nNbPolylines; i++)
    {
        //assert(_ListePolylines[i].nombreDePoint() == 2);
        for (j = 0; j < _ListePolylines[i].nombreDePoint(); j++)
        {
            indexePoint = _ListePolylines[i].indexePoint(j);
            int NbSegmentsConnexes = Connexes[indexePoint].NbSegmentsConnexes;
            //A-t-on deja 2 segments connexes pour ce point ?
            if (NbSegmentsConnexes >= 2)
            {
                //Oui=>il faudrait dedoubler ce point, mais c'est sans doute trop long
                return false;
            }

            Connexes[indexePoint].IndexesSegment[NbSegmentsConnexes] = i;

            Connexes[indexePoint].NbSegmentsConnexes++;
        }
    }
    //Pour faciliter le parcours, on affecte le ptr sur les polylignes voisines de la courante
    //=> on renseigne les champs _PolyligneP0 & _PolyligneP1
    for (i = 0; i < _nNbPolylines; i++)
    {
        //_PolyligneP0 & _PolyligneP1 pointent imperativement:
        //- sur une autre polyligne que la courante
        //- sur 2 polylignes differentes (sauf si aucune polyligne voisine)
        //assert(_ListePolylines[i].nombreDePoint() == 2);
        //Creons des alias de type tableau sur les voisines _PolyligneP0 & _PolyligneP1
        TYPolyligneParcours** pPolylignesVoisines[2];
        pPolylignesVoisines[0] = &(_ListePolylines[i]._PolyligneP0);
        pPolylignesVoisines[1] = &(_ListePolylines[i]._PolyligneP1);
        int NbSegmentsConnexes = Connexes[indexePoint].NbSegmentsConnexes;//verification supplementaire

        for (j = 0; j < 2; j++) //test enleve pour tester que pPolylignesVoisines[j] vaut bien NULL
        {
            //assert((*pPolylignesVoisines[j]) == NULL);//l'initialisation devrait etre deja faite
            //On s'occupe des segments connexes au point j du segment courant
            int IndexePj = _ListePolylines[i].indexePoint(j);
            //Premier segment connexe
            int indexeSegmentj = Connexes[IndexePj].IndexesSegment[0];
            if (indexeSegmentj != i && indexeSegmentj != -1) //la voisine n'est ni la courante, ni inexistante
            {
                //Le premier segment connexe est un voisin acceptable
                *(pPolylignesVoisines[j]) = &(_ListePolylines[indexeSegmentj]);
            }
            else
            {
                //Testons le second segment:
                indexeSegmentj = Connexes[IndexePj].IndexesSegment[1];
                if (indexeSegmentj != i && indexeSegmentj != -1)
                    //Le second segment connexe est un voisin acceptable
                {
                    *(pPolylignesVoisines[j]) = &(_ListePolylines[indexeSegmentj]);
                }
                else
                {
                    *(pPolylignesVoisines[j]) = NULL;    //par securite
                }

            }
            NbSegmentsConnexes--;
        }
        //Verifions que les voisins des 2 points du segment ne sont pas les memes!
        bool bP0P1PointentSurMemePolyligne = (_ListePolylines[i]._PolyligneP0 == _ListePolylines[i]._PolyligneP1);
        bool bAssert = bP0P1PointentSurMemePolyligne ? (_ListePolylines[i]._PolyligneP0 == NULL) : true;

        //assert(bAssert);
        if (!bAssert)
        {
            bAssert = true;
        }
    }
    return true;
}


bool TYSetGeometriqueParcours::PremierePasse(TYPointParcours& Srce, TYPointParcours& Dest, TYPointParcours** IndexePointsFrontiere, int NbPointsFrontiere, bool* EstUnPointIntersectant, Connexite* Connexes, TYSetGeometriqueParcours& geoPremierePasse)
{

    //Retourne false si on est enferme
    //La premiere passe consiste en la creation d'un parcours longeant toutes les polylignes intersectantes
    //1. Ordonner les points d'intersection

    TriePointsIntersectionSuivantSR(Srce, Dest, IndexePointsFrontiere, NbPointsFrontiere);

    //2.1 Trouver le premier point d'intersection apres S
    int i = 0;
    //while (i < NbPointsFrontiere && TYPointParcours::Scalaire(Srce, _ListePoint[IndexePointsFrontiere[i]], Srce, Dest) < 0) { i++; }
    while (i < NbPointsFrontiere && TYPointParcours::Scalaire(Srce, *IndexePointsFrontiere[i], Srce, Dest) < 0) { i++; }

    int PremierPointIntersection = i;
    //while (i < NbPointsFrontiere && TYPointParcours::Scalaire(Dest, _ListePoint[IndexePointsFrontiere[i]], Dest, Srce) > 0) { i++; }
    while (i < NbPointsFrontiere && TYPointParcours::Scalaire(Dest, *IndexePointsFrontiere[i], Dest, Srce) > 0) { i++; }

    int DernierPointIntersection = i;

    //3. Allouer de la memoire pour les points du trajet
    geoPremierePasse._ListePolylines    = new TYPolyligneParcours[1];
    geoPremierePasse._ListePolylines[0].allouer(4 * _nNbPointTotal);
    geoPremierePasse._ListePoint        = new TYPointParcours[4 * _nNbPointTotal]; //au pire, on parcours tout 2 fois
    geoPremierePasse._nNbPointTotal = 0;
    geoPremierePasse._nNbPolylines = 1;

    //4. Parcourir les polylignes
    geoPremierePasse.AjoutePointALaPolyLigne(0, Srce);
    for (i = PremierPointIntersection; i < DernierPointIntersection; i++)
    {
        //Le point suivant du parcourt est la prochaine intersection:
        int IndexPoint = IndexePointsFrontiere[i]->Identifiant;
        //A quelle polyligne appartient ce point ?
        int indexPolyligne = Connexes[IndexPoint].IndexesSegment[0];
        TYPolyligneParcours* PolyligneCourante = &(_ListePolylines[indexPolyligne]);
        //Il faut parcourir cette polyligne et ses connexes pour en enregistrer les points
        //Attention au sens, car le point d'intersection peut etre le second de la polyligne
        //On enregistre pas le point racine:
        geoPremierePasse.AjoutePointALaPolyLigne(0, _ListePoint[IndexPoint]);
        IndexPoint = ParcourtPolyligneAPartirDe(IndexPoint, PolyligneCourante, EstUnPointIntersectant, geoPremierePasse);
        //Le dernier point est-il un point d'intersection ?
        if (EstUnPointIntersectant[IndexPoint])
        {
            //On finit par un point d'intersection
            //Est-il derriere S ?
            if (TYPointParcours::Scalaire(Srce, _ListePoint[IndexPoint], Srce, Dest) < 0)
                //Oui=>retourner faux car on est emprisonne
            {
                return false;
            }
            //Est-il devant R ?
            if (TYPointParcours::Scalaire(Dest, _ListePoint[IndexPoint], Dest, Srce) < 0)
                //Oui=>retourner faux car on est emprisonne
            {
                return false;
            }
            //On a peut etre passe d'autres points d'intersection: tant mieux !
            //Mais il faut savoir oi申 on en est dans les points d'intersection:
            while (i < NbPointsFrontiere && IndexPoint != IndexePointsFrontiere[i]->Identifiant) { i++; }
        }
        else
        {
            //On ne finit pas par un point d'intersection
            if (!PolyligneCourante->isEcran())
            {
                return false;//on considere que le rayon est parti dans la nature
            }
            //=>il faut parcourir la polyligne courante dans l'autre sens
            IndexPoint = ParcourtPolyligneAPartirDe(IndexPoint, PolyligneCourante, EstUnPointIntersectant, geoPremierePasse);
        }
    }
    //Ajout du recepteur
    geoPremierePasse.AjoutePointALaPolyLigne(0, Dest);
    return true;
}

bool TYSetGeometriqueParcours::SecondePasse(TYSetGeometriqueParcours& geoPremierePasse, TYSetGeometriqueParcours& geoSecondePasse, bool bTrajetsAGaucheDeSR, TYPointParcours** & pTableauEC, int& nbPtsEC)
{
    int i;

    //1. Calcul de l'EC de geoPremierePasse
    //1.1 Recuperer les points de la premiere passe
    int nNbPointsPremierePasse = geoPremierePasse._nNbPointTotal;

    // XBH : 12/10/2004 - if nothing to compute, exit now!
    if (nNbPointsPremierePasse == 0) { return false; }

    TYPointParcours** TableauDePointsPremierePasse = new TYPointParcours*[nNbPointsPremierePasse];
    for (i = 0; i < nNbPointsPremierePasse; i++)
    {
        TableauDePointsPremierePasse[i] = &(geoPremierePasse._ListePoint[i]);
    }
    //calcul de l'EC de cet ensemble
    TYPointParcours** TableauDePointsEC = new TYPointParcours*[nNbPointsPremierePasse];
    int nNbPointsEC;

    //Mettons R en seconde position dans le tableau (exige pour le calcul d'EC)
    TYPointParcours* pRecepteur = TableauDePointsPremierePasse[nNbPointsPremierePasse - 1];
    TableauDePointsPremierePasse[nNbPointsPremierePasse - 1] = TableauDePointsPremierePasse[1];
    TableauDePointsPremierePasse[1] = pRecepteur;

    //Sens de parcours S->R
    int indexS = 0;
    int indexR = 1;
    bool bSaGaucheDeR = TableauDePointsPremierePasse[indexS]->x < TableauDePointsPremierePasse[indexR]->x;

    if (bTrajetsAGaucheDeSR)
    {
        if (bSaGaucheDeR)
        {
            nNbPointsEC = TYSetGeometriqueParcours::EnveloppeConvexeLes2PremiersPointsEtant(TableauDePointsPremierePasse, nNbPointsPremierePasse, TableauDePointsEC, false);

        }
        else
        {
            nNbPointsEC = TYSetGeometriqueParcours::EnveloppeConvexeLes2PremiersPointsEtant(TableauDePointsPremierePasse, nNbPointsPremierePasse, TableauDePointsEC, true);
        }
    }
    else
    {
        if (bSaGaucheDeR)
        {
            nNbPointsEC = TYSetGeometriqueParcours::EnveloppeConvexeLes2PremiersPointsEtant(TableauDePointsPremierePasse, nNbPointsPremierePasse, TableauDePointsEC, true);
        }
        else
        {
            nNbPointsEC = TYSetGeometriqueParcours::EnveloppeConvexeLes2PremiersPointsEtant(TableauDePointsPremierePasse, nNbPointsPremierePasse, TableauDePointsEC, false);
        }
    }
    SAFE_DELETE_LIST(TableauDePointsPremierePasse);

    //2. Parcours de l'EC, et choix du trajet sans intersection
    //2.1. Allouer de la memoire pour les points du trajet
    int nNbPointAlloue = nNbPointsEC * nNbPointsPremierePasse;
    geoSecondePasse._ListePolylines = new TYPolyligneParcours[1];//1 seule polyligne
    geoSecondePasse._ListePolylines[0].allouer(nNbPointAlloue);//allouer assez de points!
    geoSecondePasse._ListePoint     = new TYPointParcours[nNbPointAlloue];
    geoSecondePasse._nNbPointTotal = 0;
    geoSecondePasse._nNbPolylines = 1;
    //2.2. Ajouter les points de S vers R
    //Les points doivent etre ordonnes de S vers R
    bool bOrdonneDeSversR = (TableauDePointsEC[0]->Identifiant == geoPremierePasse._ListePolylines[0].indexePoint(0));
    if (!bOrdonneDeSversR)
    {
        InverseOrdreDesPoints(TableauDePointsEC, nNbPointsEC);
    }
    geoSecondePasse.AjoutePointALaPolyLigne(0, *TableauDePointsEC[0]);
    int nVerifNbPointsAjoutes = 1;
    for (i = 1; i < nNbPointsEC; i++)
    {
        if (this->intersects(*TableauDePointsEC[i - 1], *TableauDePointsEC[i]))
        {
            //Il y a un obstacle sur cette portion d'enveloppe convexe;
            //contournons-le en prenant l'itineraire de la premiere passe:
            // xbh: S'il y a un obstacle entre deux points successifs de l'enveloppe convexe, on insere entre les deux
            // un contournement de l'obstacle calcule a la premier passe.
            nVerifNbPointsAjoutes += geoSecondePasse.AjouteLesPointsComprisEntre(geoPremierePasse, 0, TableauDePointsEC[i - 1]->Identifiant, TableauDePointsEC[i]->Identifiant);
        }
        else
        {
            geoSecondePasse.AjoutePointALaPolyLigne(0, *TableauDePointsEC[i]);
            nVerifNbPointsAjoutes++;
        }
    }

    pTableauEC = TableauDePointsEC;
    nbPtsEC = nNbPointsEC;
    //  xbh: do not delete here as long as we returned the pointer outside this function
    //  delete [] TableauDePointsEC;
    return true;
}

void TYSetGeometriqueParcours::InverseOrdreDesPoints(TYPointParcours** ListeDePointsAInverser, int nNbPointsDeLaListe)
{
    for (int i = 0; i < nNbPointsDeLaListe / 2; i++)
    {
        TYPointParcours* pTmp = ListeDePointsAInverser[nNbPointsDeLaListe - 1 - i];
        ListeDePointsAInverser[nNbPointsDeLaListe - 1 - i] = ListeDePointsAInverser[i];
        ListeDePointsAInverser[i] = pTmp;
    }
}

int TYSetGeometriqueParcours::AjouteLesPointsComprisEntre(TYSetGeometriqueParcours& geoPolySource, int nIndexePoly, int nIndexeNbPremierPointAAjouter, int nIndexeDernierPointAAjouter)
{
    int nNbPointsAjoutes = 0;
    //1 Parcourir la polyligne source jusqu'a se positionner sur le premier point
    TYPolyligneParcours* PolyligneSource   = &(geoPolySource._ListePolylines[nIndexePoly]);

    int i;
    for (i = 0;   i < PolyligneSource->nombreDePoint() && PolyligneSource->indexePoint(i) != nIndexeNbPremierPointAAjouter; i++) { ; }
    for (i = i + 1; i < PolyligneSource->nombreDePoint(); i++)
    {
        TYPointParcours aTYPointParcours = PolyligneSource->point(i);
        AjoutePointALaPolyLigne(0, aTYPointParcours);
        nNbPointsAjoutes++;
        if (PolyligneSource->indexePoint(i) == nIndexeDernierPointAAjouter)
        {
            break;
        }
    }
    return nNbPointsAjoutes;
}


bool TYSetGeometriqueParcours::intersects(TYPointParcours& P1, TYPointParcours& P2)
{
    //On regarde les intersections avec tous les segments du set
    TYPointParcours P;
    int indexPoint1In = P1.Identifiant;
    int indexPoint2In = P2.Identifiant;
    for (int i = 0; i < _nNbPolylines; i++)
    {
        //assert(_ListePolylines[i].nombreDePoint() == 2);
        int indexPoint1 = _ListePolylines[i].indexePoint1();
        int indexPoint2 = _ListePolylines[i].indexePoint2();

        //les 2 segments ont ils au moins un point en commun ?
        bool bMemePoint1 = (indexPoint1 == indexPoint1In) || (indexPoint1 == indexPoint2In);
        bool bMemePoint2 = (indexPoint2 == indexPoint2In) || (indexPoint2 == indexPoint1In);
        bool bMemePoints = bMemePoint1 || bMemePoint2;

        if (!bMemePoints)
        {
            //Non => on peut faire le test
            TYPointParcours& P3 = _ListePoint[indexPoint1];
            TYPointParcours& P4 = _ListePoint[indexPoint2];
            if (TYPointParcours::IntersectionSegments(P1, P2, P3, P4, P))
                //if(TYPointParcours::IntersectionSegments(P1, P2, _ListePoint[indexPoint1], _ListePoint[indexPoint2], P))
            {
                return true;
            }
        }
    }
    return false;
}

//indexePointADroite=>indexePointDuBonCote
//test du bon cote : a gauche => det >0 a droite det < 0
//texte de debug   : en dessous <-> au dessus
bool SecondVecteurADroiteDuPremier(TYPointParcours& V1, TYPointParcours& V2, double& determinant)
{
    determinant = TYPointParcours::ZCross(V1, V2);
    return (determinant < 0);
}

bool SecondVecteurAGaucheDuPremier(TYPointParcours& V1, TYPointParcours& V2, double& determinant)
{
    determinant = TYPointParcours::ZCross(V1, V2);
    return (determinant > 0);
}

int  TYSetGeometriqueParcours::EnveloppeConvexeLes2PremiersPointsEtant(TYPointParcours** TableauDePoints, int nNbPoints, TYPointParcours** TableauDePointsECOut, bool bPremiersPointsLesPlusHauts)
{
    bool (*pDuBonCote)(TYPointParcours & V1, TYPointParcours & V2, double & determinant);
    pDuBonCote = bPremiersPointsLesPlusHauts ? &SecondVecteurADroiteDuPremier : &SecondVecteurAGaucheDuPremier;

    int nNbPointsEC = 0;
    //On teste chaque point pour voir s'il est a gauche de SP courant (en fait, si l'angle est inferieur a PI)
    //Le calcul d'enveloppe convexe suppose que les 2 premiers points sont sur l'EC, et qu'ils sont les plus bas
    //Si le premier n'est pas le plus a gauche, on swap avec le second
    bool bSaGaucheDeR = TableauDePoints[0]->x < TableauDePoints[1]->x;
    int indexS = bSaGaucheDeR ? 0 : 1;
    int indexR = bSaGaucheDeR ? 1 : 0;

    //Vecteur SR:
    TYPointParcours S = *(TableauDePoints[indexS]);
    TYPointParcours R = *(TableauDePoints[indexR]);

    //S est le premier point de l'EC:
    TableauDePointsECOut[nNbPointsEC] = TableauDePoints[indexS];

    nNbPointsEC++;

    TYPointParcours P = S;
    TYPointParcours P1 = R;

    int i;
    TYPointParcours P2;
    TYPointParcours PP1;
    TYPointParcours PP2;
    int indexePointDuBonCote = -1;
    do
    {
        //On calcule PP1
        PP1 = TYPointParcours::vecteur2D(P, P1);

        //Cherchons le point le plus a gauche de PP1
        indexePointDuBonCote = -1;//on ne l'a pas encore trouve
        for (i = 2; i < nNbPoints; i++)
        {
            P2 = (*TableauDePoints[i]);
            PP2 = TYPointParcours::vecteur2D(P, P2);
            //Ne pas tester le meme point
            if (P1.Identifiant != P2.Identifiant)
            {
                double determinant;// = TYPointParcours::ZCross(PP1, PP2);
                bool bDuBonCote = (*pDuBonCote)(PP1, PP2, determinant);
                //Attention ! Lorsque les vecteurs sont colineaires, le determinant peut etre non nul mais tres proche de 0.
                //=> seuil epsilon pour le determinant
                //static const double epsilonDeterminant = 1E-10;
                bool bColineraires = (fabs(determinant) < SEUIL_DETERMNANT_COLINEAIRE);
                bool bBonCandidatPourRemplacement;
                if (bColineraires)
                {
                    //Verifions que P2 est du meme ci申te que P1
                    if (TYPointParcours::Scalaire(PP1, PP2) < 0) // xbh: A koi sert ce test si les vecteurs sont colineaires ???
                    {
                        bBonCandidatPourRemplacement = false;
                    }
                    else
                    {
                        bBonCandidatPourRemplacement = PP2.normeCarree() > PP1.normeCarree();
                    }
                }
                else
                {
                    bBonCandidatPourRemplacement = bDuBonCote;
                }
                if (bBonCandidatPourRemplacement)
                {
                    //P2 a gauche de P1 => on remplace P1 par P2
                    P1 = P2;
                    indexePointDuBonCote = i;
                    //On recalcule PP1
                    PP1 = TYPointParcours::vecteur2D(P, P1);
                }
            }
        }
        TYPointParcours* pNouveauPointEC = NULL;
        if (indexePointDuBonCote >= 0)
        {
            //On a trouve un point plus a gauche
            pNouveauPointEC = TableauDePoints[indexePointDuBonCote];
            //Cherchons le point suivant de l'EC
            P = P1;
            P1 = R;
        }
        else
        {
            //le dernier point de l'EC est R:
            pNouveauPointEC = TableauDePoints[indexR];
        }
        //Ajoutons-le a EC
        TableauDePointsECOut[nNbPointsEC] = pNouveauPointEC;
        nNbPointsEC++;
    }
    while (nNbPointsEC < nNbPoints && indexePointDuBonCote >= 0);

    return nNbPointsEC;
}

int TYSetGeometriqueParcours::SelectionnePointsEntreSetRetDuCoteDeSR(TYSetGeometriqueParcours* geoSR, TYPointParcours** TableauDePoints, int nNbPoints)
{
    if (NULL == TableauDePoints || 0 == nNbPoints)
    {
        return 0;
    }
    int nNbPointsSelectiones = 2;

    //on ajoute les points S & R en premier:
    //ajout de S
    TableauDePoints[0] = geoSR->_ListePolylines[0].pointPtr(0);

    //ajout de R
    TableauDePoints[1] = geoSR->_ListePolylines[0].pointPtr(1);

    //Limites en X: quel est le point le plus a gauche entre S & R ?
    bool bSaGaucheDeR = TableauDePoints[0]->x < TableauDePoints[1]->x;
    TYPointParcours G, D;
    if (bSaGaucheDeR)
    {
        G = *TableauDePoints[0];
        D = *TableauDePoints[1];
    }
    else
    {
        G = *TableauDePoints[1];
        D = *TableauDePoints[0];
    }
    //Vecteur GD (Point limite Gauche & Point limite Droit)
    TYPointParcours GD = TYPointParcours::vecteur2D(G, D);

    int i;
    TYPointParcours* p1, *p2;
    TYPointParcours p;
    int nbPts = 0;

    // 1. GD intersecte une des polylignes...
    bool noIntersection = true;
    for (i = 0; (i < _nNbPolylines) && (noIntersection); i++)
    {
        nbPts = _ListePolylines[i].nombreDePoint();
        for (int j = 0; (j < nbPts - 1) && (noIntersection); j++)
        {
            p1 = _ListePolylines[i].pointPtr(j);
            p2 = _ListePolylines[i].pointPtr(j + 1);

            if (TYPointParcours::IntersectionSegments(G, D, *p1, *p2, p))
            {
                noIntersection = false;
            }
        }
    }

    // Si on n'a trouve aucune intersection, il existe un trajet direct.
    if (noIntersection) { return nNbPointsSelectiones; }

    // 2. On selectionne les points interessants...
    //MinMax en largeur
    double MaxX = D.x;
    double MinX = G.x;

    //Comme le merge des points doubles a consistes a marquer en negatifs les points inutiles, on les ecartes
    int racine = 0;
    //  int nNbPointRacine = 0;
    bool bEntreSetR;
    TYPointParcours GP;

    for (i = 0; i < nNbPoints; i++)
    {
        //Est-ce un doublon ?
        int nIdentifiantRacine = _ListePoint[racine].Identifiant;
        while (i != 0 && i < nNbPoints && _ListePoint[i].Identifiant <= nIdentifiantRacine) { i++; }
        if (i >= nNbPoints)
        {
            break;
        }
        racine = i;

        bEntreSetR = (_ListePoint[i].x >= MinX && _ListePoint[i].x <= MaxX);

        //A gauche ?
        GP = TYPointParcours::vecteur2D(G, _ListePoint[i]);
        if (bEntreSetR && (TYPointParcours::ZCross(GD, GP) >= 0))
        {
            TableauDePoints[nNbPointsSelectiones] = &(_ListePoint[i]);
            nNbPointsSelectiones++;
        }
    }

    return nNbPointsSelectiones;
}


void TYSetGeometriqueParcours::CreerTrajetAPartirDuneListeDePointsTriee(TYPointParcours** TableauDePoints, int nNbPoints, bool bSens, bool bGardeIdentifiant)
{
    if (NULL == TableauDePoints || 0 == nNbPoints)
    {
        return;
    }
    //assert(_ListePolylines == NULL);
    //assert(_ListePoint == NULL);
    //assert(_nNbPolylines == 0);
    //assert(_nNbPointTotal == 0);

    //1. Creer les points
    _ListePoint     = new TYPointParcours[nNbPoints];
    _nNbPointTotal  = nNbPoints;

    //2. Creer la polyligne
    _ListePolylines = new TYPolyligneParcours[1];
    _nNbPolylines = 1;
    _ListePolylines[0].allouer(nNbPoints);

    //Copie des points
    int i;
    if (bSens)
    {
        for (i = 0; i < nNbPoints; i++)
        {
            _ListePoint[i] = *TableauDePoints[i];
            _ListePolylines[0].ajoutePoint(i, &(_ListePoint[i]));
        }
        if (!bGardeIdentifiant) //a l'exterieur, on ne fait qu'un seul test...
        {
            for (i = 0; i < nNbPoints; i++)
            {
                _ListePoint[i].Identifiant = i;
            }
        }
    }
    else
    {
        for (i = 0; i < nNbPoints; i++)
        {
            _ListePoint[i] = *TableauDePoints[nNbPoints - i - 1];
            _ListePolylines[0].ajoutePoint(i, &(_ListePoint[nNbPoints - i - 1]));
        }
        if (!bGardeIdentifiant)
        {
            for (i = 0; i < nNbPoints; i++)
            {
                _ListePoint[i].Identifiant = nNbPoints - i - 1;
            }
        }
    }
}

bool TYSetGeometriqueParcours::AppartienneMemePolyligne(TYPointParcours* a, TYPointParcours* b, TYPointParcours* c)
{
    OVector3D v1(b->x - a->x, b->y - a->y, 0.0f);
    OVector3D v2(c->x - b->x, c->y - b->y, 0.0f);
    TYPointParcours* p, *pp, *pn;
    int nbPts = 0;
    double norme1;
    double norme2;
    double cosVal1;
    double cosVal2;

    bool t1, t2;
    t1 = t2 = false;

    // 1. on regarde si b appartienne a une polyligne
    for (int i = 0; i < _nNbPolylines; i++)
    {
        nbPts = _ListePolylines[i].nombreDePoint();
        for (int j = 0; j < nbPts; j++)
        {
            p = _ListePolylines[i].pointPtr(j);

            if (TYPointParcours::Confondus(p, b))
            {
                // On regarde si les vecteurs ab et bc sont colineaires aux segments prec et suiv
                // de la polyligne

                if (j > 0)
                {
                    pp = _ListePolylines[i].pointPtr(j - 1);
                    OVector3D v3(p->x - pp->x, p->y - pp->y, 0.0f);

                    norme1 = (v1.norme() * v3.norme());
                    cosVal1 = v1.scalar(v3) / norme1; // should be > 0
                    if (ABS(cosVal1 - 1.0f) < EPSILON_13)
                    {
                        t1 = true;
                    }

                    v3._x = -v3._x;
                    v3._y = -v3._y;

                    norme2 = (v2.norme() * v3.norme());
                    cosVal2 = v2.scalar(v3) / norme2; // should be > 0
                    if (ABS(cosVal2 - 1.0f) < EPSILON_13)
                    {
                        t2 = true;
                    }
                }
                if (j < nbPts - 1)
                {
                    pn = _ListePolylines[i].pointPtr(j + 1);
                    OVector3D v4(pn->x - p->x, pn->y - p->y, 0.0f);

                    norme2 = (v2.norme() * v4.norme());
                    cosVal2 = v2.scalar(v4) / norme2; // should be > 0
                    if (ABS(cosVal2 - 1.0f) < EPSILON_13)
                    {
                        t2 = true;
                    }

                    v4._x = -v4._x;
                    v4._y = -v4._y;

                    norme1 = (v1.norme() * v4.norme());
                    cosVal1 = v1.scalar(v4) / norme1; // should be > 0
                    if (ABS(cosVal1 - 1.0f) < EPSILON_13)
                    {
                        t1 = true;
                    }
                }
            }
        }

        if (t1 && t2)
        {
            break;
        }
    }

    return (t1 && t2);
}

