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
 * \file TYINProAcousticPathFinder.cpp
 * \brief La recherche de chemins acoustiques associee a la methode INPro
 * \author Projet_Tympan
 * \date 04 avril 2011
 */


#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYTrajet.h"

#include "TYANIME3DAcousticPathFinder.h"


TYANIME3DAcousticPathFinder::TYANIME3DAcousticPathFinder()
{

}

TYANIME3DAcousticPathFinder::~TYANIME3DAcousticPathFinder()
{

}

TYTabPoint3D TYANIME3DAcousticPathFinder::getPtSources(TYCalcul& calcul, const TYSiteNode& site)
{
    TYTabPoint3D tabPtSources; // tableau de pts des sources

    // Recuperation des sources surfaciques
    TYMapElementTabSources& mapElementSources = calcul.getResultat()->getMapEmetteurSrcs();
    site.getInfrastructure()->getAllSrcs(&calcul, mapElementSources);

    // Recuperation des sources ponctuelles
    TYTabSourcePonctuelleGeoNode tabSources;
    calcul.getAllSources(mapElementSources, tabSources);

    TYSourcePonctuelle* source;
    OMatrix matrice;
    TYPoint posS;

    for (int i = 0; i < tabSources.size(); i++)
    {
        source = TYSourcePonctuelle::safeDownCast(tabSources.at(i)->getElement());
        posS = *source->getPos();

        matrice = tabSources.at(i)->getMatrix();

        tabPtSources.push_back(TYPoint(matrice * posS));
    }
    return tabPtSources;
}

TYTabPoint3D TYANIME3DAcousticPathFinder::getPtRecepteurs(TYCalcul& calcul)
{
    TYTabPoint3D tabPtRecept; // tableau de pts des recepteurs

    TYTabLPPointControl tabRecepteurs = calcul.getProjet()->getPointsControl(); // tableau des recepteurs

    TYPointControl* recep;
    double coordRecepteur[3];

    for (int i = 0; i < tabRecepteurs.size(); i++)
    {
        recep = TYPointControl::safeDownCast(tabRecepteurs.at(i));
        recep->getCoords(coordRecepteur);

        tabPtRecept.push_back(TYPoint(coordRecepteur[0], coordRecepteur[1], coordRecepteur[2]));
    }
    return tabPtRecept;
}
/*
void TYANIME3DAcousticPathFinder::rayConstruct(TYCalcul & calcul, const TYSiteNode & site)
{
    // Recuperation de la source et du recepteur - cas avec 1 source et 1 recepteur

    TYTabLPPointControl tabRecepteurs;
    TYTabSourcePonctuelleGeoNode tabSources;

    TYMapElementTabSources& mapElementSources=calcul.getResultat()->getMapEmetteurSrcs();
    site.getInfrastructure()->getAllSrcs(&calcul, mapElementSources);
    calcul.getAllSources(mapElementSources, tabSources);

    tabRecepteurs = calcul.getProjet()->getPointsControl();

    TYPointControl     *recep  = NULL;
    TYSourcePonctuelle *source = NULL;

    TYRayEvent s, r;
    s.type = TYSOURCE;
    r.type = TYRECEPTEUR;
    unsigned int i;

    for (i = 0; i<tabRecepteurs.size() ; i++)
    {
        if(LPTYPointCalcul(tabRecepteurs.at(i))->getEtat() == TYPointCalcul::Actif)
            //if (tabRecepteurs.at(i)->getState() == TYPointCalcul::Actif)
        {
            recep  = TYPointControl::safeDownCast(tabRecepteurs.at(i));
            r.pos = getPtRecepteurs(calcul)[i];
        }
    }

    source = TYSourcePonctuelle::safeDownCast(tabSources.at(0)->getElement());
    s.pos = getPtSources(calcul, site)[0];

    int test = 3; // choix du cas test
    int id=70; // identifiant

    bool reflexSol = FALSE;
    bool reflexParois = FALSE;
    bool diffrac = TRUE;

    switch(test)
    {
        case 1 : // 1er cas test
        {
            //-10.00    3.00    2.00 Source
            //  4.00    3.00    2.00 Recepteur


            // rayon n°1
            // -3.00    3.00    0.00 Reflexion

            std::vector<TYRayEvent> events1; // liste d'evenements du rayon 1

            TYRayEvent e11;
            e11.type = TYREFLEXION;
            e11.pos = TYPoint(-3, 3, 0);

            events1.push_back(s);
            events1.push_back(e11);
            events1.push_back(r);

            LPTYRay ray1 = new TYRay(source, recep, events1);
            ray1->setIdentifiant(id);
            calcul.addRay(ray1);

            // rayon n°2
            // -3.00    5.00    2.00 Reflexion

            std::vector<TYRayEvent> events2; // liste d'evenements du rayon 2

            TYRayEvent e21;
            e21.type = TYREFLEXION;
            e21.pos = TYPoint(-3, 5, 2);

            events2.push_back(s);
            events2.push_back(e21);
            events2.push_back(r);

            LPTYRay ray2 = new TYRay(source, recep, events2);
            ray2->setIdentifiant(id++);
            calcul.addRay(ray2);

            // rayon n°3
            // -3.00    0.00    2.00 Reflexion

            std::vector<TYRayEvent> events3; // liste d'evenements du rayon 3

            TYRayEvent e31;
            e31.type = TYREFLEXION;
            e31.pos = TYPoint(-3, 0, 2);

            events3.push_back(s);
            events3.push_back(e31);
            events3.push_back(r);

            LPTYRay ray3 = new TYRay(source, recep, events3);
            ray3->setIdentifiant(id++);
            calcul.addRay(ray3);

            // rayon n°4
            // -3.00    0.00    7.00 Diffraction

            std::vector<TYRayEvent> events4; // liste d'evenements du rayon 4

            TYRayEvent e41;
            e41.type = TYDIFFRACTION;
            e41.pos = TYPoint(-3, 0, 7);

            events4.push_back(s);
            events4.push_back(e41);
            events4.push_back(r);

            LPTYRay ray4 = new TYRay(source, recep, events4);
            ray4->setIdentifiant(id++);
            calcul.addRay(ray4);

            //rayon n°5
            //-3.00    5.00    0.00 Reflexion (sol)
            //-3.00    5.00    0.00 Reflexion (face)

            std::vector<TYRayEvent> events5; // liste d'evenements du rayon 5

            TYRayEvent e51, e52;
            e51.type = TYREFLEXION;
            e51.pos = TYPoint(-3, 5, 0);

            e52.type = TYREFLEXION;
            e52.pos = TYPoint(-3, 5, 0);

            events5.push_back(s);
            events5.push_back(e51);
            events5.push_back(e52);
            events5.push_back(r);

            LPTYRay ray5 = new TYRay(source, recep, events5);
            ray5->setIdentifiant(id++);
            calcul.addRay(ray5);

            // rayon n°6
            // -8.07    2.33    0.00 Reflexion
            // -1.33    0.00    7.00 Diffraction

            std::vector<TYRayEvent> events6; // liste d'evenements du rayon 6

            TYRayEvent e61, e62;
            e61.type = TYREFLEXION;
            e61.pos = TYPoint(-8.07, 2.33, 0);

            e62.type = TYDIFFRACTION;
            e62.pos = TYPoint(-1.33, 0, 7);

            events6.push_back(s);
            events6.push_back(e61);
            events6.push_back(e62);
            events6.push_back(r);

            LPTYRay ray6 = new TYRay(source, recep, events6);
            ray6->setIdentifiant(id++);
            calcul.addRay(ray6);

            //rayon n°7
            //  -5.80    0.00    2.00 Reflexion
            //   1.20    5.00    2.00 Reflexion

            std::vector<TYRayEvent> events7; // liste d'evenements du rayon 6

            TYRayEvent e71, e72;
            e71.type = TYREFLEXION;
            e71.pos = TYPoint(-5.8, 0, 2);

            e72.type = TYREFLEXION;
            e72.pos = TYPoint(1.2, 5, 2);

            events7.push_back(s);
            events7.push_back(e71);
            events7.push_back(e72);
            events7.push_back(r);

            LPTYRay ray7 = new TYRay(source, recep, events7);
            ray7->setIdentifiant(id++);
            calcul.addRay(ray7);

            // rayon n°8
            //  -7.90    0.00    2.00 Reflexion
            //  -4.40    5.00    2.00 Reflexion
            //  -0.90    0.00    2.00 Reflexion
            //   2.60    5.00    2.00 Reflexion

            std::vector<TYRayEvent> events8; // liste d'evenements du rayon 6

            TYRayEvent e81, e82, e83, e84;
            e81.type = TYREFLEXION;
            e81.pos = TYPoint(-7.9, 0, 2);

            e82.type = TYREFLEXION;
            e82.pos = TYPoint(-4.4, 5, 2);

            e83.type = TYREFLEXION;
            e83.pos = TYPoint(-0.9, 0, 2);

            e84.type = TYREFLEXION;
            e84.pos = TYPoint(2.6, 5, 2);

            events8.push_back(s);
            events8.push_back(e81);
            events8.push_back(e82);
            events8.push_back(e83);
            events8.push_back(e84);
            events8.push_back(r);

            LPTYRay ray8 = new TYRay(source, recep, events8);
            ray8->setIdentifiant(id++);
            calcul.addRay(ray8);

            // rayon n°9
            //  -4.34    0.00    7.00 Diffraction
            //   1.62    5.00    3.43 Reflexion

            std::vector<TYRayEvent> events9; // liste d'evenements du rayon 6

            TYRayEvent e91, e92;
            e91.type = TYDIFFRACTION;
            e91.pos = TYPoint(-4.34, 0, 7);

            e92.type = TYREFLEXION;
            e92.pos = TYPoint(1.62, 5, 3.43);

            events9.push_back(s);
            events9.push_back(e91);
            events9.push_back(e92);
            events9.push_back(r);

            LPTYRay ray9 = new TYRay(source, recep, events9);
            ray9->setIdentifiant(id++);
            calcul.addRay(ray9);

            // rayon n°10
            //  -6.71    0.00    7.00 Diffraction
            //  -3.89    5.00    7.00 Diffraction
            //  -1.07    0.00    7.00 Diffraction
            //   1.97    5.00    5.00 Diffraction

            std::vector<TYRayEvent> events10; // liste d'evenements du rayon 6

            TYRayEvent e101, e102, e103, e104;
            e101.type = TYDIFFRACTION;
            e101.pos = TYPoint(-6.71, 0, 7);

            e102.type = TYREFLEXION;
            e102.pos = TYPoint(-3.89, 5, 7);

            e103.type = TYDIFFRACTION;
            e103.pos = TYPoint(-1.07, 0, 7);

            e104.type = TYREFLEXION;
            e104.pos = TYPoint(1.97, 5, 5);

            events10.push_back(s);
            events10.push_back(e101);
            events10.push_back(e102);
            events10.push_back(e103);
            events10.push_back(e104);
            events10.push_back(r);

            LPTYRay ray10 = new TYRay(source, recep, events10);
            ray10->setIdentifiant(id++);
            calcul.addRay(ray10);
        }

        case 2: // 2e cas test
        {

            // -2.00  -12.00   30.00 Source
            //  2.00   15.00   30.00 Recepteur

            // rayon n°1
            //  -0.42   -5.00   10.00 Diffraction
            //  0.33    5.00   10.00 Diffraction

            if(diffrac)
            {
                std::vector<TYRayEvent> events1;

                TYRayEvent e11, e12;
                e11.type = TYDIFFRACTION;
                e11.pos = TYPoint(-0.42, -5.0, 10.0);

                e12.type = TYDIFFRACTION;
                e12.pos = TYPoint(0.33, 5.0, 10.0);

                events1.push_back(s);
                events1.push_back(e11);
                events1.push_back(e12);
                events1.push_back(r);

                LPTYRay ray1 = new TYRay(source, recep, events1);
                ray1->setIdentifiant(id);
                calcul.addRay(ray1);
            }

            //rayon n°2
            // -0.33   -6.75    0.00 Reflexion
            //  0.23   -5.00   10.00 Diffraction
            //  0.77    5.00   10.00 Diffraction

            if(reflexSol && diffrac)
            {
                std::vector<TYRayEvent> events2;

                TYRayEvent e21, e22, e23;
                e21.type = TYREFLEXION;
                e21.pos  = TYPoint(-0.33, -6.75, 0.0);

                e22.type = TYDIFFRACTION;
                e22.pos  = TYPoint(0.23, -5.0, 10.0);

                e23.type = TYDIFFRACTION;
                e23.pos  = TYPoint(0.77, 5.0, 10.0);

                events2.push_back(s);
                events2.push_back(e21);
                events2.push_back(e22);
                events2.push_back(e23);
                events2.push_back(r);

                LPTYRay ray2 = new TYRay(source, recep, events2);
                ray2->setIdentifiant(id++);
                calcul.addRay(ray2);
            }

            // rayon n°3
            // -0.83   -5.00   10.00 Diffraction
            //  -0.28    5.00   10.00 Diffraction
            //   0.29    7.50    0.00 Reflexion

            if(diffrac && reflexSol)
            {
                std::vector<TYRayEvent> events3;

                TYRayEvent e31, e32, e33;

                e31.type = TYDIFFRACTION;
                e31.pos  = TYPoint(-0.83, -5.00, 10.00);

                e32.type = TYDIFFRACTION;
                e32.pos  = TYPoint(-0.28, 5.00, 10.00);

                e33.type = TYREFLEXION;
                e33.pos  = TYPoint(0.29, 7.50, 0.00);

                events3.push_back(s);
                events3.push_back(e31);
                events3.push_back(e32);
                events3.push_back(e33);
                events3.push_back(r);

                LPTYRay ray3 = new TYRay(source, recep, events3);
                ray3->setIdentifiant(id++);
                calcul.addRay(ray3);
            }

            //rayon n°4
             // -0.67   -6.75    0.00 Reflexion
             // -0.23   -5.00   10.00 Diffraction
             //  0.20    5.00   10.00 Diffraction
             //  0.65    7.50    0.00 Reflexion

            if(diffrac && reflexSol)
            {
                std::vector<TYRayEvent> events4;

                TYRayEvent e41, e42, e43, e44;
                e41.type = TYREFLEXION;
                e41.pos  = TYPoint(-0.67, -6.75, 0.00);

                e42.type = TYDIFFRACTION;
                e42.pos  = TYPoint(-0.23, -5.00, 10.00);

                e43.type = TYDIFFRACTION;
                e43.pos  = TYPoint(0.20, 5.00, 10.00);

                e44.type = TYREFLEXION;
                e44.pos  = TYPoint(0.65, 7.50, 0.00);

                events4.push_back(s);
                events4.push_back(e41);
                events4.push_back(e42);
                events4.push_back(e43);
                events4.push_back(e44);
                events4.push_back(r);

                LPTYRay ray4 = new TYRay(source, recep, events4);
                ray4->setIdentifiant(id++);
                calcul.addRay(ray4);
            }

            //rayon n°5
             //  0.00    1.50   10.00 Reflexion

            if(reflexParois)
            {
                std::vector<TYRayEvent> events5;

                TYRayEvent e51;
                e51.type = TYREFLEXION;
                e51.pos  = TYPoint(0.0, 1.5, 10.0);

                events5.push_back(s);
                events5.push_back(e51);
                events5.push_back(r);

                LPTYRay ray5 = new TYRay(source, recep, events5);
                ray5->setIdentifiant(id++);
                calcul.addRay(ray5);
            }

        }*/

/*Projet Scene3
rayon n°1
          -2.00  -12.00    2.00 Source
          -0.79   -5.00   10.00 Diffraction
           0.35    5.00   10.00 Diffraction
           2.00   17.00    2.00 Recepteur
rayon n°2
          -2.00  -12.00    2.00 Source
          -5.00   -5.00    2.00 Diffraction
          -5.00    5.00    2.00 Diffraction
           2.00   17.00    2.00 Recepteur
rayon n°3
          -2.00  -12.00    2.00 Source
           5.00   -5.00    2.00 Diffraction
           5.00    5.00    2.00 Diffraction
           2.00   17.00    2.00 Recepteur
rayon n°4
          -2.00  -12.00    2.00 Source
          -1.76  -10.83    0.00 Reflexion
          -0.55   -5.00   10.00 Diffraction
           0.49    5.00   10.00 Diffraction
           2.00   17.00    2.00 Recepteur
rayon n°5
          -2.00  -12.00    2.00 Source
          -0.87   -5.00   10.00 Diffraction
           0.19    5.00   10.00 Diffraction
           1.70   15.00    0.00 Reflexion
           2.00   17.00    2.00 Recepteur
rayon n°6
          -2.00  -12.00    2.00 Source
          -1.77  -10.83    0.00 Reflexion
          -0.64   -5.00   10.00 Diffraction
           0.34    5.00   10.00 Diffraction
           1.72   15.00    0.00 Reflexion
           2.00   17.00    2.00 Recepteur
Fin de fichier*/


/*Projet Scene4
rayon n°1
          -2.00  -12.00    2.00 Source
          -1.03   -5.00   10.00 Diffraction
          -0.12    5.00   10.00 Diffraction
           2.00   17.00   30.00 Recepteur
rayon n°2
          -2.00  -12.00    2.00 Source
          -1.80  -10.83    0.00 Reflexion
          -0.82   -5.00   10.00 Diffraction
           0.02    5.00   10.00 Diffraction
           2.00   17.00   30.00 Recepteur
rayon n°3
          -2.00  -12.00    2.00 Source
          -1.32   -5.00   10.00 Diffraction
          -0.68    5.00   10.00 Diffraction
          -0.01    8.00    0.00 Reflexion
           2.00   17.00   30.00 Recepteur
rayon n°4
          -2.00  -12.00    2.00 Source
          -1.86  -10.83    0.00 Reflexion
          -1.15   -5.00   10.00 Diffraction
          -0.54    5.00   10.00 Diffraction
           0.09    8.00    0.00 Reflexion
           2.00   17.00   30.00 Recepteur*/

//}
//}

/*
void TYANIME3DAcousticPathFinder::rayConstruct2(TYCalcul & calcul, const TYSiteNode & site)
{
    int id=70; // identifiant

    // -2.00  -12.00   30.00 Source
     // 2.00   15.00   30.00 Recepteur

    const int nb  = 500; // nbr de rayons par source
    Source s    = Source(vec3(-2.,12.,30.), nb);
    Recepteur r = Recepteur(vec3(2.,15.,30.));

    //rayon n°1
    // -0.42   -5.00   10.00 Diffraction
    // 0.33    5.00   10.00 Diffraction


    Event e11 = Event();
    Event e12 = Event();

    e11.setImpact(&vec3(-0.42, -5.0, 10.0));
    e12.setImpact(&vec3( 0.33,  5.0, 10.0));
    e11.setType(DIFFRACTION);
    e12.setType(DIFFRACTION);

    Ray* ray1 = new Ray();
    ray1->setSource(&s);
    ray1->setRecepteur(&r);
    ray1->addEvent(&e11);
    ray1->addEvent(&e12);

    // rayon n°2
    // -0.33   -6.75    0.00 Reflexion
    // 0.23   -5.00   10.00 Diffraction
    // 0.77    5.00   10.00 Diffraction

    Event e21 = Event();
    Event e22 = Event();
    Event e23 = Event();

    e21.setImpact(&vec3(-0.33, -6.75, 0.0));
    e22.setImpact(&vec3( 0.23, -5.0, 10.0));
    e23.setImpact(&vec3( 0.77,  5.0, 10.0));

    e21.setType(SPECULARREFLEXION);
    e22.setType(DIFFRACTION);
    e23.setType(DIFFRACTION);

    Ray* ray2 = new Ray();
    ray2->setSource(&s);
    ray2->setRecepteur(&r);
    ray2->addEvent(&e21);
    ray2->addEvent(&e22);
    ray2->addEvent(&e23);

    // rayon n°3
    // -0.83   -5.00   10.00 Diffraction
    // -0.28    5.00   10.00 Diffraction
    //  0.29    7.50    0.00 Reflexion

    Event e31 = Event();
    Event e32 = Event();
    Event e33 = Event();

    e31.setImpact(&vec3(-0.83, -5.0, 10.0));
    e32.setImpact(&vec3(-0.28,  5.0, 10.0));
    e33.setImpact(&vec3( 0.29,  7.5, 0.0));

    e31.setType(DIFFRACTION);
    e32.setType(DIFFRACTION);
    e33.setType(SPECULARREFLEXION);

    Ray* ray3 = new Ray();
    ray3->setSource(&s);
    ray3->setRecepteur(&r);
    ray3->addEvent(&e31);
    ray3->addEvent(&e32);
    ray3->addEvent(&e33);

    //rayon n°4
    // -0.67   -6.75    0.00 Reflexion
    // -0.23   -5.00   10.00 Diffraction
    //  0.20    5.00   10.00 Diffraction
    //  0.65    7.50    0.00 Reflexion

    Event e41 = Event();
    Event e42 = Event();
    Event e43 = Event();
    Event e44 = Event();

    e41.setImpact(&vec3(-0.67, -6.75, 0.0));
    e42.setImpact(&vec3(-0.23, -5.0, 10.0));
    e43.setImpact(&vec3( 0.2,   5.0, 10.0));
    e44.setImpact(&vec3( 0.65,  7.5, 0.0));

    e41.setType(SPECULARREFLEXION);
    e42.setType(DIFFRACTION);
    e43.setType(DIFFRACTION);
    e44.setType(SPECULARREFLEXION);

    Ray* ray4 = new Ray();
    ray4->setSource(&s);
    ray4->setRecepteur(&r);
    ray4->addEvent(&e41);
    ray4->addEvent(&e42);
    ray4->addEvent(&e43);
    ray4->addEvent(&e44);

    //rayon n°5
    //  0.00    1.50   10.00 Reflexion
    Event e51 = Event();
    e51.setImpact(&vec3(0.0, 1.5, 10.0));
    e51.setType(SPECULARREFLEXION);
    Ray* ray5 = new Ray();
    ray5->setSource(&s);
    ray5->setRecepteur(&r);
    ray5->addEvent(&e51);

    _simulation = new RayTracerSimulation();
    _simulation->addRay(ray1);
    _simulation->addRay(ray2);
    _simulation->addRay(ray3);
    _simulation->addRay(ray4);
    _simulation->addRay(ray5);
    appendRayToTYCalcul(site, calcul);
}*/

struct pairComparator
{
    bool operator()(pair<int, int> p1, pair<int, int> p2)
    {
        if (p1.first < p2.first)
        {
            return true;
        }
        if (p1.second < p2.second)
        {
            return true;
        }
        return false;
    };
};

//bool TYANIME3DAcousticPathFinder::appendRayToTYCalcul(const TYSiteNode& site, TYCalcul& calcul)
//{
//    //On nettoie le tableau de trajet que TYCalcul a pu generer au prealable.
//    calcul.getTabTrajet().clear();
//
//    //Generation de la structure qui va faciliter la manipulation des TYTrajet
//    //Cle = idSource,idRecepteur
//    std::map<pair<int, int>, TYTrajet, pairComparator> tableTrajet;
//
//    //Recuperation de la liste des rayons valides pour la _simulation.
//    std::vector<Ray*>* rays = _simulation->getSolver()->getValidRays();
//
//    bool errorType = false;
//
//    //Recuperation de la liste des recepteurs
//    TYTabLPPointControl tabRecepteurs =  calcul.getProjet()->getPointsControl();
//
//    //Recuperation de la liste des sources poncutelles
//    TYMapElementTabSources& mapElementSources = calcul.getResultat()->getMapEmetteurSrcs();
//    site.getInfrastructure()->getAllSrcs(&calcul, mapElementSources);
//    TYTabSourcePonctuelleGeoNode tabSources; // Creation d'une collection des sources
//    calcul.getAllSources(mapElementSources, tabSources);
//
//    TYSourcePonctuelle* source = TYSourcePonctuelle::safeDownCast(tabSources.at(0)->getElement());
//    TYPointControl*     recep   = TYPointControl::safeDownCast(tabRecepteurs.at(0));
//    OMatrix matrice = tabSources.at(0)->getMatrix();
//
//    double CoordSource[3];
//    source->getPos()->getCoords(CoordSource);
//    TYPoint posSourceGlobal = matrice * (*source->getPos());
//    double coordRecepteur[3];
//    recep->getCoords(coordRecepteur);
//
//
//    //Conversion des rayons du lancer en rayons metier Tympan
//    for (unsigned int i = 0; i < rays->size(); i++)
//    {
//        LPTYRay ray;
//        std::vector<TYRayEvent> events;
//        Ray* r = rays->at(i);
//        /*int idRecep = r->getRecepteur()->getId();  // probleme ici
//        int idSource = r->getSource()->getId();  // probleme ici
//
//        //Les identifiants des recepteurs et sources sont construit pour correspondre a l'index des sources et recepteurs dans Tympan.
//        if(idRecep > tabRecepteurs.size() || idSource > sources.size())
//        {
//            continue;
//        }
//        TYSourcePonctuelle *sourceP = TYSourcePonctuelle::safeDownCast(sources.at(idSource)->getElement());
//        TYPointControl *recepP = TYPointControl::safeDownCast(tabRecepteurs.at(idRecep));
//        OMatrix matrice = sources.at(idSource)->getMatrix();
//
//        sourceP = TYSourcePonctuelle::safeDownCast(tabSources.at(0)->getElement());
//        recepP   = TYPointControl::safeDownCast(tabRecepteurs.at(0));
//        OMatrix matrice = tabSources.at(0)->getMatrix();
//
//
//        double CoordSource[3];
//        sourceP->getPos()->getCoords(CoordSource);
//        TYPoint posSourceGlobal = matrice * (*sourceP->getPos());
//        double coordRecepteur[3];
//        recep->getCoords(coordRecepteur);*/
//
//        //Definition des Evenements.
//        TYRayEvent e;
//
//        //Ajout de la source
//        e.type = TYSOURCE;
//        e.pos = TYPoint(posSourceGlobal);
//        events.push_back(e);
//
//        //Creation des evenements de diffractions et reflexions
//        for (unsigned int j = 0; j < r->getEvents()->size(); j++)
//        {
//            Event* ev =  dynamic_cast<Event*>(r->getEvents()->at(j).data());
//            switch (ev->getType())
//            {
//                case SPECULARREFLEXION :
//                    e.type = TYREFLEXION;
//                    break;
//                case DIFFRACTION:
//                    e.type = TYDIFFRACTION;
//                    break;
//                default:
//                    e.type = TY_NO_TYPE;
//                    errorType = true;
//                    break;
//            }
//            e.pos = TYPoint(ev->getPosition().x, ev->getPosition().y, ev->getPosition().z);
//            events.push_back(e);
//        }
//        //Ajout du recepteur
//        e.type = TYRECEPTEUR;
//        e.pos = TYPoint(coordRecepteur[0], coordRecepteur[1], coordRecepteur[2]);
//        events.push_back(e);
//
//        //Construction du rayon et envoie au calcul
//        ray = new TYRay(source, recep, events);
//        calcul.addRay(ray);
//
//        /*std::map<pair<int,int>,TYTrajet,pairComparator>::iterator it;
//
//        it = tableTrajet.find(pair<int,int>(idSource,idRecep));
//        if(it != tableTrajet.end()){
//            //Cas ou un trajet existait deja, on genere un nouveau chemin avec un spectre calcule
//            OSpectreComplex spectre;
//            spectre.setValue(16.,-200.);
//            spectre.setValue(20.,-200.);
//            spectre.setValue(25.,-200.);
//            spectre.setValue(31.5,-200.);
//            spectre.setValue(40.,-200.);
//            spectre.setValue(50.,-200.);
//            spectre.setValue(63.,-200.);
//            spectre.setValue(80.,-200.);
//            spectre.setValue(100.,10.);
//            spectre.setValue(125.,10.);
//            spectre.setValue(160.,10.);
//            spectre.setValue(200.,10.);
//            spectre.setValue(250.,10.);
//            spectre.setValue(315.,10.);
//            spectre.setValue(400.,10.);
//            spectre.setValue(500.,10.);
//            spectre.setValue(630.,10.);
//            spectre.setValue(800.,10.);
//            spectre.setValue(1000.,10.);
//            spectre.setValue(1250.,10.);
//            spectre.setValue(1600.,10.);
//            spectre.setValue(2000.,10.);
//            spectre.setValue(2500.,10.);
//            spectre.setValue(3150.,10.);
//            spectre.setValue(4000.,10.);
//            spectre.setValue(5000.,10.);
//            spectre.setValue(6300.,-200.);
//            spectre.setValue(8000.,-200.);
//            spectre.setValue(10000.,-200.);
//            spectre.setValue(12500.,-200.);
//            spectre.setValue(16000.,-200.);
//
//            TYChemin chemin;
//            chemin.setAttenuation(spectre);
//            it->second.addChemin(chemin);
//        }
//        else {
//            //Cas ou il n'existe pas encore de trajet entre la source et le recepteur.
//            //TYTrajet trajet;
//            //trajet.setSourcePonctuelle(sources.at(idSource));
//            //trajet.setPointCalcul(tabRecepteurs.at(idRecep));
//
//        }*/
//
//
//    }
//    if (errorType)
//    {
//        return false;
//    }
//
//    return true;
//}

