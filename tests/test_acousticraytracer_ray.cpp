/**
 * \file test_OBox.cpp
 * \test Testing of OBox libs used by lot of TYMPAN features
 *
 *  Created on: april 21, 2013
 *      Author: Denis THOMASSON <denis.thomasson@edf.fr>
 */

#include <cstdlib>
#include <vector>
#include <map>

#include "gtest/gtest.h"
#include "Tympan/solvers/AcousticRaytracer/Geometry/Cylindre.h"
#include "Tympan/solvers/AcousticRaytracer/Acoustic/Source.h"
#include "Tympan/solvers/AcousticRaytracer/Acoustic/Recepteur.h"
#include "Tympan/solvers/AcousticRaytracer/Acoustic/Event.h"
#include "Tympan/solvers/AcousticRaytracer/Acoustic/Diffraction.h"
#include "Tympan/solvers/AcousticRaytracer/Acoustic/SpecularReflexion.h"
#include "Tympan/solvers/AcousticRaytracer/Ray/Ray.h"

using std::cout;
using std::cerr;
using std::endl;

Ray* createRay(void)
{
    Ray* ray = new Ray();
    Source* S = new Source();
    S->setId(1536);
    Recepteur* R = new Recepteur();
    R->setId(5435);
    ray->source = S;
    ray->recepteur = (void*) R;

    // On ajoute les vnements utiles

    // EVENEMENT 0  rflexion en position (10, 0, 0)
    Event* ev0 = new Event();
    ev0->setType(SPECULARREFLEXION);
    ev0->setPosition(vec3(10, 0, 0));
    ev0->setIncomingDirection(vec3(0, 0, 5));
    QSharedPointer<Event> QSP = QSharedPointer<Event> (ev0);

    ray->getEvents()->push_back(QSP);

    // EVENEMENT 1 : Diffraction en position (20, 0, 10)
    Event* ev1 = new Event();
    ev1->setType(DIFFRACTION);
    ev1->setPosition(vec3(20, 0, 10));
    ev1->setIncomingDirection(vec3(10, 0, 0));

    QSP = QSharedPointer<Event> (ev1);
    ray->getEvents()->push_back(QSP);

    // EVENEMENT 2  : Diffraction en position (25, 0, 7)
    Event* ev2 = new Event();
    ev2->setType(DIFFRACTION);
    ev2->setPosition(vec3(25, 0, 7));
    ev2->setIncomingDirection(vec3(20, 0, 10));

    QSP = QSharedPointer<Event> (ev2);
    ray->getEvents()->push_back(QSP);

    // EVENEMENT 3  : Rflexion en position (50, 0, 0)
    Event* ev3 = new Event();
    ev3->setType(SPECULARREFLEXION);
    ev3->setPosition(vec3(50, 0, 3));
    ev3->setIncomingDirection(vec3(25, 0, 7));

    QSP = QSharedPointer<Event> (ev3);
    ray->getEvents()->push_back(QSP);

    return ray;
}

TEST(SignatureRay, dumpenv)
{
    // Create a Ray de signature
    Ray* ray = createRay();

    signature sig = ray->getSignature();

    // TEST 1 : Vrification des valeurs SD = 6 (0110)
    unsigned int SR = sig.first, SD = sig.second;
    EXPECT_EQ(6, SD);

    // TEST 2 : Vrification du numro de la source
    unsigned int S = SR >> 20;
    EXPECT_EQ(1536, S);

    // TEST 3 : Vrification du numro du rcepteur
    unsigned int R = (SR << 12) >> 12;
    EXPECT_EQ(5435, R);

    delete ray;
}
