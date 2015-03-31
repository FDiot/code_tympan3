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

#include <iostream>
#include <sstream>
#include <cmath>
#include <vector>

#include "Tympan/models/common/3d.h"
#include "Tympan/models/common/delaunay_maker.h"
#include "Tympan/models/common/spectrum_matrix.h"
#include "Tympan/models/solver/config.h"
#include "Tympan/models/solver/acoustic_problem_model.hpp"
#include "Tympan/models/solver/acoustic_result_model.hpp"
#include "Tympan/solvers/ANIME3DSolver/TYANIME3DAcousticModel.h"
#include "Tympan/solvers/ANIME3DSolver/TYANIME3DAcousticPathFinder.h"
#include "Tympan/solvers/ANIME3DSolver/TYANIME3DFaceSelector.h"
#include "TYANIME3DSolver.h"

#define __ONLY_RAYS__ // To test only raytracing without acoustics

TYANIME3DSolver::TYANIME3DSolver()
{
    _tabPolygon = NULL;
}

TYANIME3DSolver::~TYANIME3DSolver()
{
    purge();
}

void TYANIME3DSolver::purge()
{
    if (_tabPolygon)
    {
        delete [] _tabPolygon;
    }

    _tabPolygon = NULL;
}

void TYANIME3DSolver::init()
{
    tympan::LPSolverConfiguration config = tympan::SolverConfiguration::get();
    _pAtmos = std::unique_ptr<AtmosphericConditions>( new AtmosphericConditions(config->AtmosPressure, config->AtmosTemperature, config->AtmosHygrometry) );
}

bool TYANIME3DSolver::solve(const tympan::AcousticProblemModel& aproblem,
                            tympan::AcousticResultModel& aresult,
                            tympan::LPSolverConfiguration configuration)
{
    tympan::SolverConfiguration::set(configuration);
    tympan::LPSolverConfiguration config = tympan::SolverConfiguration::get();
    // Recupration (once for all) des sources et des rcepteurs
    init();

    // Recuperation du tableau de rayon de la structure resultat
    tab_acoustic_path& tabRays = aresult.get_path_data();

    // Construction de la liste des faces utilise pour le calcul
    TYANIME3DFaceSelector fs(aproblem);
    bool bRet = fs.exec(_tabPolygon, _tabPolygonSize);
    if (!bRet) { return false; }

    // Ray tracing computation
    TYANIME3DAcousticPathFinder apf(_tabPolygon, _tabPolygonSize, aproblem, tabRays);
    if ( !apf.exec() ) { return false; }

#ifndef __ONLY_RAYS__

    ////////////////////////////////////////////////////////////
    // Calculs acoustiques sur les rayons via la methode ANIME3D
    ////////////////////////////////////////////////////////////


    TYANIME3DAcousticModel aam(tabRays, _tabPolygon, aproblem, *_pAtmos);

    // calcul de la matrice de pression totale pour chaque couple (S,R)
    OTab2DSpectreComplex tabSpectre = aam.ComputeAcousticModel();
    OSpectre sLP; // spectre de pression pour chaque couple (S,R)

    tympan::SpectrumMatrix& matrix = aresult.get_data();
    matrix.resize(aproblem.nreceptors(), aproblem.nsources());

    for (int i = 0; i < static_cast<int>(aproblem.nsources()); i++) // boucle sur les sources
    {
        for (int j = 0; j < static_cast<int>(aproblem.nreceptors()); j++) // boucle sur les recepteurs
        {
            tabSpectre[i][j].setEtat(SPECTRE_ETAT_LIN);
            sLP = tabSpectre[i][j];
            sLP.setType(SPECTRE_TYPE_LP);

            tabSpectre[i][j] = sLP.toDB();  // conversion du tableau resultat en dB
            matrix(j, i) = tabSpectre[i][j];
        }
    }

#else

    tympan::SpectrumMatrix& matrix = aresult.get_data();
    matrix.resize(aproblem.nreceptors(), aproblem.nsources());

    size_t nb_srcs = aproblem.nsources();
    size_t nb_rcpt = aproblem.nreceptors();

    for (int i = 0; i < static_cast<int>(aproblem.nsources()); i++) // boucle sur les sources
    {
        for (int j = 0; j < static_cast<int>(aproblem.nreceptors()); j++) // boucle sur les recepteurs
        {
            tympan::Spectrum sLP;
            sLP.setType(SPECTRE_TYPE_LP);

            matrix(j, i) = sLP;
        }
    }

#endif //__ONLY_RAYS__

    //// Allow to watch curved rays (as in meteo field) instead of right rays
	//if (config->UseMeteo && config->OverSampleD)
    //{
    //    for (unsigned int i = 0; i < tabRays.size(); i++)
    //    {
    //        tabRays[i]->tyRayCorrection( apf.get_geometry_modifier() );
    //    }
    //}

    return true;
}

