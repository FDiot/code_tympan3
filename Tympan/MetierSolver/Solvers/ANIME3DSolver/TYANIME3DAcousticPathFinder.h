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
 * Copyright (c) 2011 EDF. All Rights Reserved.
 * All Rights Reserved. No part of this software and its documentation may
 * be used, copied, modified, distributed and transmitted, in any form or by
 * any means, without the prior written permission of EDF.
 * EDF-R&D Departement AMA - 1, avenue du general de Gaulle, 92141 CLAMART
 */

/**
 * \file TYANIME3DAcousticPathFinder.h
 * \brief La recherche de chemins acoustiques associee a la methode ANIME3D (header)
 * \author Projet_Tympan
 * \date 04 avril 2011
 */

#ifndef __TYANIME3DACOUSTICPATHFINDER__
#define __TYANIME3DACOUSTICPATHFINDER__

#include "Tympan/MetierSolver/DataManagerCore/TYAcousticPathFinderInterface.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYCalcul.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Engine/Simulation.h"

///Type pour les tableaux de rays
typedef std::vector<Ray*> TabRays;

//class Simulation;

/**
 * \class TYANIME3DAcousticPathFinder
 * \brief La recherche de chemins acoustiques associee a la methode ANIME3D
 */
class TYANIME3DAcousticPathFinder : public TYAcousticPathFinderInterface
{
public:
    TYANIME3DAcousticPathFinder();
    virtual ~TYANIME3DAcousticPathFinder();

    void rayConstruct(TYCalcul& calcul, const TYSiteNode& site);

    void rayConstruct2(TYCalcul& calcul, const TYSiteNode& site);

    //TYTabRay getTabRays(){return _tabRay;}

    TYTabPoint3D getPtSources(TYCalcul& calcul, const TYSiteNode& site);

    TYTabPoint3D getPtRecepteurs(TYCalcul& calcul);

    /*!
    * \fn bool appendRayToTYCalcul(const TYSite& site,TYCalcul& calcul)
    * \brief Convertie les rayons du lancer de rayons en rayons Tympan et les ajoute dans le tableau contenu dans le calcul.
    * \return Renvoie vrai si l'ensemble des rayons ont bien ete ajoutes au calcul.
    */
//    bool appendRayToTYCalcul(const TYSiteNode& site, TYCalcul& calcul);

//    Simulation* getSimulation() {return _simulation;}

//    TabRays* getTabRays() {return _simulation->getSolver()->getValidRays();}

private:
    //TYRayTracer* _rayTracer;

    // Simulation* _simulation;
};

#endif // __TYANIME3DACOUSTICPATHFINDER__
