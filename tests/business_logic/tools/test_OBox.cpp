/**
 * \file test_OBox.cpp
 * \test Testing of OBox libs used by lot of TYMPAN features
 *
 *  Created on: april 21, 2013
 *      Author: Denis THOMASSON <denis.thomasson@edf.fr>
 */

#include <cstdlib>

#include "gtest/gtest.h"
#include "Tympan/models/common/3d.h"

using std::cout;
using std::cerr;
using std::endl;

TEST(IsInsideTest, dumpenv)
{
    // Création des points min et max
    OCoord3D pt1(-2.0, -4.0, -3.0);
    OCoord3D pt2(5.0, 2.0, 1.0);

    // Création de la box
    OBox box(pt1, pt2);

    // Création du point à tester
    OPoint3D pt(-1.0, -3.0, -2.0);

    // Test positif
    bool resu = box.isInside(pt);
    ASSERT_TRUE(resu == true);

    // Test négatif
    pt._z = 3.0;
    resu = box.isInside(pt);
    ASSERT_FALSE(resu == true);
}

TEST(IsInside2DTest, dumpenv)
{
    // Création des points min et max
    OCoord3D pt1(-2.0, -4.0, -3.0);
    OCoord3D pt2(5.0, 2.0, 1.0);

    // Création de la box
    OBox box(pt1, pt2);

    // Création du point à tester
    OPoint3D pt(-1.0, -3.0, -2.0);

    // Test positif (1) : Point is inside 2D and 3D box
    bool resu = box.isInside2D(pt);
    ASSERT_TRUE(resu == true);

    // Test positif (2) : Point is inside 2D but not 3D box
    pt._z = 3.0;
    resu = box.isInside2D(pt);
    ASSERT_TRUE(resu == true);

    // Test négatif : Point is outside 2D box
    pt._x = -5;
    resu = box.isInside2D(pt);
    ASSERT_FALSE(resu == true);
}

TEST(IsInContactTest, dumpenv)
{
    // 1st box creation
    OCoord3D pt1(-2.0, -4.0, -3.0);
    OCoord3D pt2(5.0, 2.0, 1.0);
    OBox box1(pt1, pt2);

    // 2nd box creation
    OCoord3D pt3(4.0, 1.0, 0.0);
    OCoord3D pt4(7.0, 3.0, 2.0);
    OBox box2(pt3, pt4);

    // Test positif (1) : Some point of box2 are inside box1
    bool resu = box1.isInContact(box2);
    ASSERT_TRUE(resu == true);

    // Test positif (2) : box1 & box2 have a face in contact
    box2._min = OPoint3D(5.0, 0.0, -1.0);
    box2._max = OPoint3D(8.0, 5.0, 3.0);
    resu = box1.isInContact(box2);
    ASSERT_TRUE(resu == true);

    // Test négatif : box1 do not intersects with box2
    box2._min._x = 7.0;
    resu = box1.isInContact(box2);
    ASSERT_FALSE(resu == true);
}

TEST(EnlargePointTest, dumpenv)
{
    // Création d'une boite vide
    OBox box;

    // Ajout des points
    box.Enlarge(OPoint3D(1.0, 2.0, -5.0));
    box.Enlarge(OPoint3D(5.0, -3.0, 1.0));

    // Test de la taille de la boite
    ASSERT_TRUE(box._min == OPoint3D(1.0, -3.0, -5.0));
    ASSERT_TRUE(box._max == OPoint3D(5.0, 2.0, 1.0));

    // Test positif : ajout d'un point qui étend la boite
    box.Enlarge(OPoint3D(-3.0, 1.0, 0.0));
    ASSERT_TRUE(box._min == OPoint3D(-3.0, -3.0, -5.0));

    // Test négatif : Ajout d'un point contenu dans la boite
    box.Enlarge(OPoint3D(-1.0, 1.0, -2.0));
    ASSERT_TRUE(box._min == OPoint3D(-3.0, -3.0, -5.0));
}

TEST(EnlargeBoxTest, dumpenv)
{
    // 1st box creation
    OCoord3D pt1(-2.0, -4.0, -3.0);
    OCoord3D pt2(5.0, 2.0, 1.0);
    OBox box1(pt1, pt2);

    // 2nd box creation
    OCoord3D pt3(4.0, 1.0, 0.0);
    OCoord3D pt4(7.0, 3.0, 2.0);
    OBox box2(pt3, pt4);

    // Redimensionnement de la box 1 avec la box 2
    box1.Enlarge(box2);

    // Test de la taille de la boite résultante
    ASSERT_TRUE(box1._min == OPoint3D(-2.0, -4.0, -3.0));
    ASSERT_TRUE(box1._max == OPoint3D(7.0, 3.0, 2.0));
}

TEST(TranslateTest, dumpenv)
{
    // Création de la box initiale
    OCoord3D pt1(-2.0, -4.0, -3.0);
    OCoord3D pt2(5.0, 2.0, 1.0);
    OBox box1(pt1, pt2);

    // Vecteur de translation
    OVector3D vec(5.0, 3.0, -2.0);

    // Action de translation
    box1.Translate(vec);

    // Vérification de nouvelles limites de la boite
    ASSERT_TRUE(box1._min == OPoint3D(3.0, -1.0, -5.0));
    ASSERT_TRUE(box1._max == OPoint3D(10.0, 5.0, -1.0));
}
