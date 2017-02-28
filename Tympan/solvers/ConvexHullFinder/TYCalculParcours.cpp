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


#include <stdio.h>
#include <math.h>

#include "Tympan/models/common/3d.h"
#include "TYCalculParcours.h"
#include <assert.h>


int nNbAbandonsParPointNuples = 0;

TYCalculParcours::TYCalculParcours(int nNbSegMax, bool bVertical)
{
    _bVertical = bVertical;
    _nNbSegMax = nNbSegMax;
    //Creation d'un set geometrique pour les donnees d'entree:
    _geoImporterDXF = new TYSetGeometriqueParcours;
    _geoImporterDXF->_nNbPolylines = 0;
    _geoImporterDXF->AllouerPolylignes(_nNbSegMax);
    _geoImporterDXF->_nNbPointTotal = 0;
    _geoImporterDXF->_ListePoint = new TYPointParcours[2 * _nNbSegMax];

    //Creation d'un set geometrique pour le segment d'entree Source-Recepteur:
    _geoSR = new TYSetGeometriqueParcours;
    _geoSR->_nNbPolylines = 0;
    _geoSR->AllouerPolylignes(1);
    _geoSR->_nNbPointTotal = 0;
    _geoSR->_ListePoint = new TYPointParcours[2];
}

TYCalculParcours::~TYCalculParcours()
{
    SAFE_DELETE(_geoImporterDXF);
    SAFE_DELETE(_geoSR);
}

void TYCalculParcours::InitChangementVariable2D3D(bool bAxeXMoinsSignifiant)
{
    //Changement de variable en entree pour traitement 2D (une coordonnee ne sera pas prise en compte)
    if (_bVertical)
    {
        //Lorsque le rayon Source-Recepteur est plus oriente suivant Y que X (repere Tympan),
        //on garde Y (au lieu de X) comme seconde composante (apres Z, l'altitude = la premiere composante 2D).
        //Remarques :
        //* la composante ignoree est tout de meme stockee et restituee a la fin (cgangement de variable)
        //* on ne fait que choisir des points existants
        //(on n'en calcule pas de nouveaux -du moins pour le calcul vertical-)
        //* pour les calculs horizontaux, seuls les points sur le trajet Source-Recepteur sont susceptibles
        //d'etre issus de calculs au lieu de selection. La coordonnee Z de ces points resulte alors d'une interpollation (cf methode IntersectionDroites)
        //Exemple: le rayon Source-Recepteur est parallele a l'axe Y; dans ce cas, le plan 2D est (0y, 0z)
        if (bAxeXMoinsSignifiant)
        {
            _indexXInOut = 1;//Y->X
            _indexYInOut = 2;//Z->Y
            _indexZInOut = 0;//X->Z
        }
        else
        {
            _indexXInOut = 0;
            _indexYInOut = 2;//Z->Y
            _indexZInOut = 1;//Y->Z
        }
    }
    else
    {
        _indexXInOut = 0;
        _indexYInOut = 1;
        _indexZInOut = 2;
    }
}

TYCalculParcours::TYCalculParcours(TYSetGeometriqueParcours* geoImporterDXF, TYSetGeometriqueParcours* geoSR, bool bVertical)
{
    _geoSR = geoSR;
    _geoImporterDXF = geoImporterDXF;
    _bVertical = bVertical;
    _nNbSegMax = -1;//on se souvient que les polylignes ont ete importees petit a petit
}


void TYCalculParcours::AjouterSegment(double* ptA, double* ptB, bool isInfra, bool isEcran, TYSetGeometriqueParcours* geo)
{
    //On enregistre les coordonnees des 2 nouveaux points:
    TYPointParcours* p1 = &(geo->_ListePoint[geo->_nNbPointTotal]);
    p1->isInfra = isInfra;  p1->isEcran = isEcran;
    p1->x = ptA[_indexXInOut]; p1->y = ptA[_indexYInOut]; p1->z = ptA[_indexZInOut]; p1->Identifiant = geo->_nNbPointTotal;
    geo->_nNbPointTotal++;

    TYPointParcours* p2 = &(geo->_ListePoint[geo->_nNbPointTotal]);
    p2->isInfra = isInfra;  p2->isEcran = isEcran;
    p2->x = ptB[_indexXInOut]; p2->y = ptB[_indexYInOut]; p2->z = ptB[_indexZInOut]; p2->Identifiant = geo->_nNbPointTotal;
    geo->_nNbPointTotal++;

    //On ajoute une polyligne a geo:
    geo->_ListePolylines[geo->_nNbPolylines].ajouteSegment(p1, p2);
    geo->_nNbPolylines++;
}

void TYCalculParcours::AjouterSegmentCoupe(double* ptA, double* ptB, bool isInfra, bool isEcran)
{
    assert(_geoSR->_nNbPolylines <= _nNbSegMax);
    AjouterSegment(ptA, ptB, isInfra, isEcran, _geoImporterDXF);
}

void TYCalculParcours::AjouterSegmentSR(double* ptA, double* ptB)
{
    assert(_geoSR->_nNbPointTotal == 0);
    assert(_geoSR->_nNbPolylines == 0);
    //On fait ici le choix du systeme de coordonnee:
    double dDeltaX = fabs(ptA[0] - ptB[0]);
    double dDeltaY = fabs(ptA[1] - ptB[1]);
    InitChangementVariable2D3D(dDeltaX < dDeltaY);
    //InitChangementVariable2D3D(ptA[0] == ptB[0]);
    AjouterSegment(ptA, ptB, false, false, _geoSR);
}

void TYCalculParcours::PointTrajetGauche(int i, double* pt)
{
    PointTrajet(i, pt, _geoTrajetGauche);
}

void TYCalculParcours::PointTrajetDroite(int i, double* pt)
{
    PointTrajet(i, pt, _geoTrajetDroite);
}

void TYCalculParcours::PointTrajet(int i, double* pt, TYSetGeometriqueParcours* geo)
{
    assert(i < geo->_nNbPointTotal);
    pt[_indexXInOut] = geo->_ListePoint[i].x;
    pt[_indexYInOut] = geo->_ListePoint[i].y;
    pt[_indexZInOut] = geo->_ListePoint[i].z;
}

int  TYCalculParcours::NombrePointsTrajetDroite()
{
    if (NULL == _geoTrajetDroite)
    {
        return 0;
    }

    double ptA[3];
    double ptB[3];
    PointTrajet(0, ptA, _geoSR);
    PointTrajet(1, ptB, _geoSR);

    return _geoTrajetDroite->_nNbPointTotal;
}

int  TYCalculParcours::NombrePointsTrajetGauche()
{
    if (NULL == _geoTrajetGauche)
    {
        return 0;
    }

    double ptA[3];
    double ptB[3];
    PointTrajet(0, ptA, _geoSR);
    PointTrajet(1, ptB, _geoSR);

    return _geoTrajetGauche->_nNbPointTotal;
}


bool TYCalculParcours::Traitement()
{
    _geoTrajetGauche = NULL;
    _geoTrajetDroite = NULL;

    bool resTraitement = Traite(_geoSecondePasseGauche, _geoSecondePasseDroite, _geoTrajetGauche, _geoTrajetDroite);

    return resTraitement;
}

bool TYCalculParcours::CalculTrajet(TYSetGeometriqueParcours& geoCourant, bool bCoteGauche, bool* PointsAGauche, bool* PointsADroite, TYSetGeometriqueParcours& geoPremierePasse, TYSetGeometriqueParcours*& geoTrajet)
{
    bool bPasEnferme = true;
    //3.4 Calcul des trajets
    //A partir de maintenant, on ne traite plus qu'un seul cote
    //bool bCoteGauche = true;
    //3.4.1 Ramener les points traversant la frontiere sur la frontiere
    int* IndexePointsFrontiere = new int[geoCourant._nNbPointTotal];
    int NbPointsFrontiere = 0;
    //=>Construire un tableau donnant directement la propriete "intersection"
    bool* EstUnPointIntersectant = new bool[geoCourant._nNbPointTotal];

    //on considere ici que les polylignes sont en fait des segments (vrai venant de Tympan)
    geoCourant.RamenerPointsTraversantLaFrontiere(_geoSR->_ListePoint[0], _geoSR->_ListePoint[1], IndexePointsFrontiere, NbPointsFrontiere, EstUnPointIntersectant, bCoteGauche, PointsAGauche, PointsADroite);

    //On va plus loin que s'il y a des points d'intersection (sinon, c'est SR)
    if (NbPointsFrontiere)
    {
        //3.4.2 Mettre bout a bout les polylignes
        //Merger les segments: on considere ici que chaque point ne peut appartenir a plus de 2 polylignes (a verifier en venant de Tympan)
        Connexite* Connexes = new Connexite[geoCourant._nNbPointTotal];

        bool bOk = geoCourant.ListerPointsConnexes(Connexes);
        if (!bOk)
        {
            bPasEnferme = false;//on a trouve des points n-uples; comme on ne sait pas les gerer, on pretexte un enfermemnt
            nNbAbandonsParPointNuples++;
        }
        else
        {
            //Est-on enferme ? C'est le cas si en suivant une polyligne intersectante,
            //on se retrouve avant S ou apres R sur [S,R]
            //Comme on doit parcourir toutes les polylignes intersectantes, on en profite pour calculer les trajets:
            bPasEnferme = geoCourant.PremierePasse(_geoSR->_ListePoint[0], _geoSR->_ListePoint[1], IndexePointsFrontiere, NbPointsFrontiere, EstUnPointIntersectant, Connexes, geoPremierePasse);
        }
        if (bPasEnferme)
        {
            geoTrajet = &geoPremierePasse;
        }

        SAFE_DELETE_LIST(Connexes);
    }
    SAFE_DELETE_LIST(EstUnPointIntersectant);
    SAFE_DELETE_LIST(IndexePointsFrontiere);

    return bPasEnferme;
}

int TYCalculParcours::Traite(
    TYSetGeometriqueParcours& geoSecondePasseGauche,
    TYSetGeometriqueParcours& geoSecondePasseDroite,
    TYSetGeometriqueParcours*& geoTrajetGauche,
    TYSetGeometriqueParcours*& geoTrajetDroite)
{
    // Preparation des donnees
    //1. Le segment SR doit etre present
    assert(NULL != _geoSR);
    if (NULL == _geoSR->_ListePoint)
    {
        return -1;
    }
    //2. On attribue des identifiants speciaux aux points S & R, pour ne pas les melanger a d'autres
    _geoSR->_ListePoint[0].Identifiant = INDENTIFIANT_SOURCE;
    _geoSR->_ListePoint[1].Identifiant = INDENTIFIANT_RECEPTEUR;

    //S'il n'y a pas d'obstacle:
    if (0 == _geoImporterDXF->_nNbPointTotal)
    {
        //On copie _geoSR
        geoSecondePasseGauche.Copy(*_geoSR);
        geoSecondePasseDroite.Copy(*_geoSR);

        //On n'ecrit pas le fichier
        return 0;
    }

    //3.1 Filtrage
    //3.1.1 Filtrage sur les points
    int nNbDoublons = _geoImporterDXF->MergePointsDoubles();

    //3.1.1 Filtrage sur les polylignes
    if (!_bVertical)
    {
        TYPointParcours** pTableauECD = NULL;
        int nbPtsECD = 0;
        TYPointParcours** pTableauECG = NULL;
        int nbPtsECG = 0;
        TYSetGeometriqueParcours geoGauche;
        TYSetGeometriqueParcours geoDroite;

        int nNbPolylignes = _geoImporterDXF->_nNbPolylines;
        nNbDoublons = _geoImporterDXF->SupressionPolylignesRedondantes();

        //3.2 Marquage des points a gauche ou a droite
        bool* PointsAGauche = NULL;
        bool* PointsADroite = NULL;
        _geoImporterDXF->MarquePointsADroiteEtAGauche(_geoSR->_ListePoint[0], _geoSR->_ListePoint[1], PointsAGauche, PointsADroite);

        //3.3 Separation des points suivants le ci��te droit au gauche
        //Cette separation donne deja les segments intersectant [SR]

        bool bPolylignesInfraFermees = _geoImporterDXF->PolylignesInfraFermees();
        //assert(bPolylignesInfraFermees);

        _geoImporterDXF->SeparationDroiteGauche(PointsAGauche, PointsADroite, geoGauche, geoDroite);

        //3.4 Calcul des trajets
        TYSetGeometriqueParcours geoPremierePasseGauche;
        bool bPasEnfermeAGauche = CalculTrajet(geoGauche, true, PointsAGauche, PointsADroite, geoPremierePasseGauche, geoTrajetGauche);
        if (bPasEnfermeAGauche)
        {
            bool bSecondePasse = geoGauche.SecondePasse(geoPremierePasseGauche, geoSecondePasseGauche, true, pTableauECG, nbPtsECG);
            geoTrajetGauche = &geoSecondePasseGauche;
        }

        TYSetGeometriqueParcours geoPremierePasseDroite;
        bool bPasEnfermeADroite = CalculTrajet(geoDroite, false, PointsAGauche, PointsADroite, geoPremierePasseDroite, geoTrajetDroite);
        if (bPasEnfermeADroite)
        {
            bool bSecondePasse = geoDroite.SecondePasse(geoPremierePasseDroite, geoSecondePasseDroite, false, pTableauECD, nbPtsECD);
            geoTrajetDroite = &geoSecondePasseDroite;
        }

        SAFE_DELETE_LIST(PointsAGauche);
        SAFE_DELETE_LIST(PointsADroite);

        SAFE_DELETE_LIST(pTableauECD);
        SAFE_DELETE_LIST(pTableauECG);
    }
    else
    {
        //Il suffit de calculer l'EC de l'ensemble des points formes par ceux d'abscisse compris entre Sx & Rx,
        //et d'en prendre la partie superieure
        //Avant, on termine le merge entrepris avant, ie qu'on ne reordonne la liste pour mettre en premier les nons doublons
        //_geoImporterDXF->GenerePointNonConfondus();
        //Hypotheses:
        //On suppose que ni source ni recepteur ne sont dans un batiment
        TYPointParcours** TableauDePointsSelectionnes = new TYPointParcours*[_geoImporterDXF->_nNbPointTotal + 2]; // +2 pour prendre en compte les points S et R qui sont ajoutes
        int nNbPointsSelectiones = _geoImporterDXF->SelectionnePointsEntreSetRetDuCoteDeSR(_geoSR, TableauDePointsSelectionnes, _geoImporterDXF->_nNbPointTotal);

        TYPointParcours** TableauDePointsEC = new TYPointParcours*[nNbPointsSelectiones];
        //int nNbPointsEC = TYSetGeometriqueParcours::EnveloppeConvexeLes2PremiersPointsEtantLesPlusBas(TableauDePointsSelectionnes, nNbPointsSelectiones, TableauDePointsEC);

        int nNbPointsEC;
        bool bWrong = false;
        int idx = 0;
        TYPointParcours A, B;

        // xbh: corrige le pb de trajet vu sur le projet UMP 1.7 insono.xml
        // Ce n'est vraiment pas optimal, mais je n'ai pas trouve de meilleure solution
        // avec l'algorithme actuel que d'eliminer les points posant probleme et de recalculer
        // l'enveloppe convexe a partir du nouvel ensemble de points.
        // Cela a l'air de mieux fonctionner, mais il doit rester des cas ou des problemes persistent.
        // En particulier, on suppose dans le filtrage de l'enveloppe que l'on n'a pas de points de rebroussements.
        // Ce qui est faux dans le cas ou on passe par dessus un mur d'epaisseur nulle (mais ce cas est-il possible dans Tympan??)
        do
        {
            nNbPointsEC = TYSetGeometriqueParcours::EnveloppeConvexeLes2PremiersPointsEtant(TableauDePointsSelectionnes, nNbPointsSelectiones, TableauDePointsEC, false);

            // xbh: ajout d'une deuxieme passe pour filter l'enveloppe convexe en eliminant les points de rebroussements
            bWrong = false;
            idx = 0;
            for (int i = 0; i < nNbPointsEC; i++)
            {
                if (i > 1)
                {
                    A = TYPointParcours::vecteur2D(*TableauDePointsEC[i - 1], *TableauDePointsEC[i - 2]);
                    B = TYPointParcours::vecteur2D(*TableauDePointsEC[i - 1], *TableauDePointsEC[i]);

                    if (TYPointParcours::Scalaire(A, B) > 0) // point de rebroussement dans l'enveloppe, on l'oublie pour lisser l'enveloppe
                    {
                        bWrong = true;
                        idx = i - 1;
                        break; // on s'arrete ici...
                    }
                    else if (ABS(TYPointParcours::Scalaire(A, B)) < EPSILON_13)
                    {
                        // on autorise les angles a 90i�� que lors du suivis du contour d'un obstacle
                        if (!_geoImporterDXF->AppartienneMemePolyligne(TableauDePointsEC[i - 2], TableauDePointsEC[i - 1], TableauDePointsEC[i]))
                        {
                            bWrong = true;
                            idx = i - 1;
                            break; // on s'arrete ici...
                        }
                    }
                    else if ((A.x == 0.0f) || (B.x == 0.0f))
                    {
                        // on autorise les segments verticaux que lors du suivis du contour d'un obstacle
                        if (!_geoImporterDXF->AppartienneMemePolyligne(TableauDePointsEC[i - 2], TableauDePointsEC[i - 1], TableauDePointsEC[i]))
                        {
                            bWrong = true;
                            idx = i - 1;
                            break; // on s'arrete ici...
                        }
                    }
                }
            }

            if (bWrong) // Si un point pose pb, on l'elimine et on recommence le calcul de l'enveloppe...
            {
                int k;
                for (k = 0; k < nNbPointsSelectiones; k++)
                {
                    if (TableauDePointsEC[idx] == TableauDePointsSelectionnes[k])
                    {
                        idx = k;
                        break; // on a trouve le point, on stop ici
                    }
                }
                for (k = idx; k < nNbPointsSelectiones - 1; k++)
                {
                    // On decale tout le tableau
                    TableauDePointsSelectionnes[k] = TableauDePointsSelectionnes[k + 1];
                }
                nNbPointsSelectiones--;
            }
        }
        while (bWrong);

        //Les points doivent etre ordonnes pour le trajet; on sait que le premier point est S, le second R
        //Si S est a droite de R, il faudra reordonner les points pour le trajet
        bool bAGaucheDeSR = (_geoSR->_ListePoint[0].x < _geoSR->_ListePoint[1].x);
        geoSecondePasseGauche.CreerTrajetAPartirDuneListeDePointsTriee(TableauDePointsEC, nNbPointsEC, bAGaucheDeSR, true);
        geoTrajetGauche = &geoSecondePasseGauche;

        SAFE_DELETE_LIST(TableauDePointsEC);
        SAFE_DELETE_LIST(TableauDePointsSelectionnes);
    }

    return 0;
}


