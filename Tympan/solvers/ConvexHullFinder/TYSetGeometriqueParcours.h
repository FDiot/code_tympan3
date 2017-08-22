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
#include <qmutex.h>

//On devrait pouvoir etendre le nombre de point sur (2pow31) -1, vue que les indentifiants sont codes sur des int:
#define MAX_POINTS  32767

//Afin d'eviter des bugs de synchronisation de differents listes de points,
//traitement basee sur les identifiants (SelectionnePointsEntreSetRetDuCoteDeSR,...),
//on marque les identifiants des points particuliers Source et Recepteur,
//provenant en outre toujours d'une liste separee
#define  INDENTIFIANT_SOURCE    MAX_POINTS
#define  INDENTIFIANT_RECEPTEUR (MAX_POINTS-1)

/**
 * \struct Connexite
 * \brief Connectivity between points and segments
 */
struct Connexite
{
    int IndexesSegment[2];  //!< Two indexes of the segment
    int NbSegmentsConnexes;	//!< Related segments number
};

/**
 * \brief Class to build a geometric path used by the TYCalculParcours class
 */
class TYSetGeometriqueParcours
{
public:
    TYPolyligneParcours* _ListePolylines;  	//!< Geometric path as a polylines
    TYPointParcours* _ListePoint;			//!< List of points on the path
    int _nNbPolylines;						//!< Polylines number
    int _nNbPolylineAllouee;				//!< Allocated polylines
    int _nNbPointTotal;						//!< Total number of points

    static TYPointParcours* _ListePointQSort; 	//!< static access to the C function of quicksort
    static TYPointParcours* _SrceQSort;			//!< static access to the C function of quicksort
    static TYPointParcours* _DestQSort;			//!< static access to the C function of quicksort
    static QMutex _mutex;						//!< mutex used to serialize TriePointsIntersectionSuivantSR method
    /// Constructor
    TYSetGeometriqueParcours() { Init();}
    /// Destructor
    ~TYSetGeometriqueParcours() { Clean();}
    // NB: PolyligneP0 & PolyligneP1 ne sont pas copies:
    /// Copy operator
    void Copy(TYSetGeometriqueParcours& geoIn);
    /// Allocation of the polylines list
    void AllouerPolylignes(int nNbPolylineAllouee)
    {
		_nNbPolylineAllouee = nNbPolylineAllouee; 
		_ListePolylines = new TYPolyligneParcours[nNbPolylineAllouee];
	};
    /// Return true if all polylines from infrastructure are closed
    bool PolylignesInfraFermees();
    /// Detect and fix double points
    int MergePointsDoubles();
    // Undefined method:
    // int SupprimeLesPointsDoubles();
    /// Suppress useless polylines
    int SupressionPolylignesRedondantes();
    /**
     * @brief Separate left and right polylines with two geometric paths
     * @param [in] PointsAGauche Boolean array marking the points on the left
     * @param [in] PointsADroite Boolean array marking the points on the right
     * @param [out] geoGauche Geometric path on the left
     * @param [out] geoDroite Geometric path on the right
     */
    void SeparationDroiteGauche(bool* PointsAGauche, bool* PointsADroite, TYSetGeometriqueParcours& geoGauche, TYSetGeometriqueParcours& geoDroite);
    /**
     * @brief Mark points on the left and on the right of the current geometric path
     * @param [in] Srce Source point
     * @param [in] Dest Receptor point
     * @param [out] PointsAGauche Boolean array marking the points on the left
     * @param [out] PointsADroite Boolean array marking the points on the right
     */
    void MarquePointsADroiteEtAGauche(TYPointParcours& Srce, TYPointParcours& Dest, bool*& PointsAGauche, bool*& PointsADroite);
    /// To be commented
    void RamenerPointsTraversantLaFrontiere(TYPointParcours& Srce, TYPointParcours& Dest, int* IndexePointsFrontiere, int& NbPointsFrontiere, bool* EstUnPointIntersectant, bool bCoteGauche, bool* PointsAGauche, bool* PointsADroite);
    /**
     * \brief Fill for each point the connectivity with segments
     * \param Connexes Array of connectivity
     */
    bool ListerPointsConnexes(Connexite *& Connexes);

    /**
     * @brief First pass to build a path along all the intersecting polylines
     * @param Srce Source
     * @param Dest Receptor
     * @param IndexePointsFrontiere Array of boundary point indices
     * @param NbPointsFrontiere Boundary points number
     * @param EstUnPointIntersectant True if it is a intersecting point
     * @param Connexes Connectivity points array
     * @param geoPremierePasse First geometric path
     * @return True if succeeds
     */
    bool PremierePasse(TYPointParcours& Srce, TYPointParcours& Dest, int* IndexePointsFrontiere, int NbPointsFrontiere, bool* EstUnPointIntersectant, Connexite* Connexes, TYSetGeometriqueParcours& geoPremierePasse);
    /**
     * @brief Second pass
     * @param geoPremierePasse First geometric path
     * @param geoSecondePasse Second geometric path
     * @param bTrajetsAGaucheDeSR Flag to define paths at the left of source-receptor direction
     * @param pTableauEC Array of points for the convex hull
     * @param nbPtsEC Points number of the convex hull
     * @return True if succeeds
     */
    bool SecondePasse(TYSetGeometriqueParcours& geoPremierePasse, TYSetGeometriqueParcours& geoSecondePasse, bool bTrajetsAGaucheDeSR, TYPointParcours** & pTableauEC, int& nbPtsEC);
    /// To be commented
    void TriePointsIntersectionSuivantSR(TYPointParcours& Srce, TYPointParcours& Dest, int* IndexePointsFrontiere, int NbPointsFrontiere);
    /// Add a point P to the polyline indexPolyligne
    bool AjoutePointALaPolyLigne(int indexPolyligne, TYPointParcours& P);
    /// Add some points of the nIndexePoly polyline from the geoPolySource geometric path
    int AjouteLesPointsComprisEntre(TYSetGeometriqueParcours& geoPolySource, int nIndexePoly, int nIndexeNbPremierPointAAjouter, int nIndexeDernierPointAAjouter);
    /// To be commented
    int ParcourtPolyligneAPartirDe(int IndexPointRacine, TYPolyligneParcours*& PolyligneRacine, bool* EstUnPointIntersectant, TYSetGeometriqueParcours& geoPremierePasse);
    /**
     * @brief Check if [P1P2] segment can intersect the geometric path
     * @param P1 First point
     * @param P2 Second point
     * @return True if [P1P2] intersects
     */
    bool intersects(TYPointParcours& P1, TYPointParcours& P2);

    /* Unused method:
    int NbreRefPoint()
    {
        int nNbRefPoint = 0;
        //Nombre de reference aux points dans les polylignes
        for (int i = 0; i < _nNbPolylines; i++)
        {
            nNbRefPoint += _ListePolylines[i].nombreDePoint();
        }
        return nNbRefPoint;
    }*/

    /**
     * @brief Compute the convex hull (arrays should be allocated before the call)
     * @param TableauDePoints [in] Points array (first points are the lower ones)
     * @param nNbPoints [in] Size of previous array
     * @param TableauDePointsECOut [out] Points array of the convex hull
     * @param bPremiersPointsLesPlusHauts [in] True if the first points are the higher ones
     * @return Size array of TableauDePointsECOut
     */
    static int EnveloppeConvexeLes2PremiersPointsEtant(TYPointParcours** TableauDePoints, int nNbPoints, TYPointParcours** TableauDePointsECOut, bool bPremiersPointsLesPlusHauts);
    /**
     * @brief Select points from the current geometric path which are between source and receptor of the geoSR geometric path
     * @param geoSR Geometric path from which the selection happens
     * @param TableauDePoints Array of selected points (will contain the source and receptor points of geoSR)
     * @param nNbPoints Points number
     * @return Number of selected points
     */
    int SelectionnePointsEntreSetRetDuCoteDeSR(TYSetGeometriqueParcours* geoSR, TYPointParcours** TableauDePoints, int nNbPoints);
    /// Create paths from a sorted points list (Used only for vertical paths)
    void CreerTrajetAPartirDuneListeDePointsTriee(TYPointParcours** TableauDePoints, int nNbPoints, bool bSens, bool bGardeIdentifiant);
    /// Check if the points a, b, c belong to the same polyline
    bool AppartienneMemePolyligne(TYPointParcours* a, TYPointParcours* b, TYPointParcours* c);

private:
    //int MergePointsDoubles(bool bLexico);
    /// Swap polylines i and j
    void SwapPolyligne(int i, int j);
    /// Delete polylines list and points list
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
    /// Initialize data
    void Init()
    {
        _ListePolylines = NULL;
        _ListePoint = NULL;
        _nNbPolylines = 0;
        _nNbPointTotal = 0;
        _nNbPolylineAllouee = 0;
    }
    /**
     * @brief Invert a list of points
     * @param ListeDePointsAInverser [In] The list of points to be inverted [out] The inverted list
     * @param nNbPointsDeLaListe Points number in the list
     */
    static void InverseOrdreDesPoints(TYPointParcours** ListeDePointsAInverser, int nNbPointsDeLaListe);

};

#endif// TYSET_GEOMETRIQUE_PARCOURS_H
