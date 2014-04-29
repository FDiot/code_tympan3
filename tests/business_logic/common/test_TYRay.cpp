/**
 * \file test_OBox.cpp
 * \test Testing of OBox libs used by lot of TYMPAN features
 *
 *  Created on: april 21, 2013
 *      Author: Denis THOMASSON <denis.thomasson@edf.fr>
 */

#include <cstdlib>
#include <vector>

#include "gtest/gtest.h"
#include "Tympan/MetierSolver/CommonTools/OGeometrie.h"
#include "Tympan/MetierSolver/CommonTools/OCoord3D.h"
#include "Tympan/MetierSolver/CommonTools/OPoint3D.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYPoint.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYRay.h"

using std::cout;
using std::cerr;
using std::endl;

TYRay* createRay(void)
{
    TYRay* ray = new TYRay();

    // On ajoute les évènements utiles

    // EVENEMENT 0
    TYPoint pt(0.0, 0.0, 0.0);
    TYRayEvent* ev = new TYRayEvent(pt);
    TY_RAY_EVENT type_ev = TYSOURCE;
    ev->angle = 5.;
    ev->type = TYSOURCE;

    ray->addEvent(ev);

    // EVENEMENT 1
    pt._x = 10.0;
    ev = new TYRayEvent(pt);
    ev->angle = 7.;
    ev->type = TYDIFFRACTION;

    ray->addEvent(ev);

    // EVENEMENT 2
    pt._x = 20.0;
    ev = new TYRayEvent(pt);
    ev->angle = 0.;
    ev->type = TYDIFFRACTION;

    ray->addEvent(ev);

    // EVENEMENT 3
    pt._x = 30.0;
    ev = new TYRayEvent(pt);
    ev->angle = 0.;
    ev->type = TYREFLEXION;

    ray->addEvent(ev);

    // EVENEMENT 4
    pt._x = 40.0;
    ev = new TYRayEvent(pt);
    ev->angle = 0.;
    ev->type = TYDIFFRACTION;

    ray->addEvent(ev);

    // EVENEMENT 5
    pt._x = 50.0;
    ev = new TYRayEvent(pt);
    ev->angle = 0.;
    ev->type = TYRECEPTEUR;

    ray->addEvent(ev);

    return ray;
}

TEST(IndexOfEvents, dumpenv)
{
    // Create altimetry
    TYRay* tyRay = createRay();

    // TEST 1 : type d'évènement simple
    std::vector<int> tabIndex = tyRay->getIndexOfEvents(TYDIFFRACTION);
    ASSERT_TRUE(tabIndex.size() == 3); // Bon nombre d'evenement trouve
    ASSERT_TRUE(tabIndex[2] = 4);       // Bon repérage de l'emplacement des evenements
    tabIndex.clear();

    // TEST 2 : combinaison de type d'évènement
    tabIndex = tyRay->getIndexOfEvents(TYREFLEXION | TYRECEPTEUR);
    ASSERT_TRUE(tabIndex.size() == 2); // Bon nombre d'evenement trouve
    ASSERT_TRUE(tabIndex[1] = 5);       // Bon repérage de l'emplacement des evenements

    // TEST 3 : Test evenement non existant
    tabIndex = tyRay->getIndexOfEvents(TYREFRACTION);
    ASSERT_TRUE(tabIndex.size() == 0); // Bon nombre d'evenement trouve

    delete tyRay;
}
