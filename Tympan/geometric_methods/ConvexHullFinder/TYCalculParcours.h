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

#ifndef TYCALCUL_PARCOURS_H
#define TYCALCUL_PARCOURS_H

#include <gtest/gtest_prod.h>

#include "TYPointParcours.h"
#include "TYSetGeometriqueParcours.h"

/**
 * @brief Class for computing path used by TYAcousticPathFinder
 */
class TYCalculParcours
{
public:
    /**
     * @brief Constructor
     * @param nNbSegMax Max number of segments for the path
     * @param _bVertical True if vertical paths are computed, false if horizontal paths are computed
     */
    TYCalculParcours(int nNbSegMax, bool _bVertical);
    /// Other constructor
    TYCalculParcours(TYSetGeometriqueParcours* geoImporterDXF, TYSetGeometriqueParcours* geoSR, bool _bVertical);
    /// Destructor
    ~TYCalculParcours();
    /**
     * \brief Select the 2D plane and set _indexXInOut, _indexYInOut, _indexZInOut
     * \param bAxeXMoinsSignifiant True for horizontal view if Source-Receptor is more Y oriented than X oriented
     */
    void InitChangementVariable2D3D(bool bAxeXMoinsSignifiant);

    // Unused attribute:
    // int NumPlanCoupe;

    /**
     * @brief Add a segment defined by 2 points
     * @param ptA Point A
     * @param ptB Point B
     * @param isInfra Boolean true if infra
     * @param isEcran Boolean trie if screen
     */
    void AjouterSegmentCoupe(double* ptA, double* ptB, bool isInfra, bool isEcran);
    /// Add points A (source) and B (receptor)
    void AjouterSegmentSR(double* ptA, double* ptB);
    /// Return the ith point of the left geometric path
    void PointTrajetGauche(int i, double* pt);
    /// Return the ith point of the right geometric path
    void PointTrajetDroite(int i, double* pt);
    /// Return the points number of the left geometric path
    int  NombrePointsTrajetDroite();
    /// Return the points number of the right geometric path
    int  NombrePointsTrajetGauche();
    /// Build the left and right geometric paths
    bool Traitement();

private:
    FRIEND_TEST(test_TYCalculParcours, InitChangementVariable2D3D);
    FRIEND_TEST(test_TYCalculParcours, AjouterSegmentSR);
    FRIEND_TEST(test_TYCalculParcours, AjouterSegmentCoupe);
    FRIEND_TEST(test_TYCalculParcours, PointTrajetGauche);
    FRIEND_TEST(test_TYCalculParcours, PointTrajetDroite);
    FRIEND_TEST(test_TYCalculParcours, Traitement);
    void PointTrajet(int i, double* pt, TYSetGeometriqueParcours* geo);
    void AjouterSegment(double* ptA, double* ptB, bool isInfra, bool isEcran, TYSetGeometriqueParcours* geo);
    bool CalculTrajet(TYSetGeometriqueParcours& geoCourant,
                      bool bCoteGauche,
                      bool* PointsAGauche,
                      bool* PointsADroite,
                      TYSetGeometriqueParcours& geoPremierePasse,
                      TYSetGeometriqueParcours*& geoTrajet);
	int Traite(
        TYSetGeometriqueParcours& geoSecondePasseGauche,
        TYSetGeometriqueParcours& geoSecondePasseDroite,
        TYSetGeometriqueParcours*& geoTrajetGauche,
        TYSetGeometriqueParcours*& geoTrajetDroite );

    int  _nNbSegMax;		//!< Number of segments (encountered faces)
    bool _bVertical;		//!< True if horizontal view
    int _indexXInOut;
    int _indexYInOut;
    int _indexZInOut;
    TYSetGeometriqueParcours* _geoImporterDXF;
    TYSetGeometriqueParcours* _geoSR;
    TYSetGeometriqueParcours _geoSecondePasseGauche;
    TYSetGeometriqueParcours _geoSecondePasseDroite;
    TYSetGeometriqueParcours* _geoTrajetGauche;		//!< Paths list on the left
    TYSetGeometriqueParcours* _geoTrajetDroite;		//!< Paths list on the right

};

#endif// TYCALCUL_PARCOURS_H
