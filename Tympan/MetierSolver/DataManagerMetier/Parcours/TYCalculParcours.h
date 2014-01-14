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

#include "TYPointParcours.h"
#include "TYSetGeometriqueParcours.h"

class TYCalculParcours
{
    int  _nNbSegMax;
    bool _bVertical;
    int _indexXInOut;
    int _indexYInOut;
    int _indexZInOut;
    TYSetGeometriqueParcours* _geoImporterDXF;
    TYSetGeometriqueParcours* _geoSR;
    TYSetGeometriqueParcours _geoSecondePasseGauche;
    TYSetGeometriqueParcours _geoSecondePasseDroite;
    TYSetGeometriqueParcours* _geoTrajetGauche;
    TYSetGeometriqueParcours* _geoTrajetDroite;
public:
    TYCalculParcours(int nNbSegMax, bool _bVertical);
    TYCalculParcours(TYSetGeometriqueParcours* geoImporterDXF, TYSetGeometriqueParcours* geoSR, bool _bVertical);
    ~TYCalculParcours();
    void InitChangementVariable2D3D(bool bAxeXMoinsSignifiant);

    int NumPlanCoupe;

    void AjouterSegmentCoupe(double* ptA, double* ptB, bool isInfra, bool isEcran);
    void AjouterSegmentSR(double* ptA, double* ptB);
    void PointTrajetGauche(int i, double* pt);
    void PointTrajetDroite(int i, double* pt);
    int  NombrePointsTrajetDroite();
    int  NombrePointsTrajetGauche();

    bool Traitement();

	int Traite(
        TYSetGeometriqueParcours& geoSecondePasseGauche,
        TYSetGeometriqueParcours& geoSecondePasseDroite,
        TYSetGeometriqueParcours*& geoTrajetGauche,
        TYSetGeometriqueParcours*& geoTrajetDroite );

private:
    void PointTrajet(int i, double* pt, TYSetGeometriqueParcours* geo);
    void AjouterSegment(double* ptA, double* ptB, bool isInfra, bool isEcran, TYSetGeometriqueParcours* geo);
    bool CalculTrajet(TYSetGeometriqueParcours& geoCourant,
                      bool bCoteGauche,
                      bool* PointsAGauche,
                      bool* PointsADroite,
                      TYSetGeometriqueParcours& geoPremierePasse,
                      TYSetGeometriqueParcours*& geoTrajet);

};

#endif// TYCALCUL_PARCOURS_H
