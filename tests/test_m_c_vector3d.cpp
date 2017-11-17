/**
 * \file test_OGeometrie.cpp
 * \test Testing of OGeometrie libs used by lot of TYMPAN features
 *
 *  Created on: november 6, 2013
 *      Author: Denis THOMASSON <denis.thomasson@edf.fr>
 */

#include <cstdlib>

#include "gtest/gtest.h"
#include "Tympan/models/common/3d.h"

using std::cout;
using std::cerr;
using std::endl;

TEST(sum, dumpenv)
{
    // Création des vecteurs
    OPoint3D p1 = OPoint3D(2.0, 3.0, 4.0);
    OPoint3D p2 = OPoint3D(1.0, 2.0, 3.0);

    OVector3D v1(p1), v2(p2);

    // Calcul de la somme
    OVector3D v3 = v1 + v2;

    ASSERT_TRUE(v3._x == 3.0);
    ASSERT_TRUE(v3._y == 5.0);
    ASSERT_TRUE(v3._z == 7.0);
}

TEST(subst, dumpenv)
{
    // Création des vecteurs
    OPoint3D p1 = OPoint3D(2.0, 3.0, 4.0);
    OPoint3D p2 = OPoint3D(1.0, 2.0, 3.0);


    OVector3D v1(p1), v2(p2);

    // Calcul de la soustraction
    OVector3D v3 = v1 - v2;

    ASSERT_TRUE(v3._x == 1.0);
    ASSERT_TRUE(v3._y == 1.0);
    ASSERT_TRUE(v3._z == 1.0);
}

TEST(prod1, dumpenv)
{
    // Création des vecteurs
    OPoint3D p1 = OPoint3D(2.0, 3.0, 4.0);
    OPoint3D p2 = OPoint3D(1.0, 2.0, 3.0);


    OVector3D v1(p1), v2(p2);

    // Calcul du produit
    OVector3D v3 = v1 * v2;

    ASSERT_TRUE(v3._x == 2.0);
    ASSERT_TRUE(v3._y == 6.0);
    ASSERT_TRUE(v3._z == 12.0);
}

TEST(prod2, dumpenv)
{
    // Création des vecteurs
    OPoint3D p1 = OPoint3D(2.0, 3.0, 4.0);
    double a = 2.0;

    OVector3D v1(p1);

    // Calcul du produit
    OVector3D v3 = a * v1;

    ASSERT_TRUE(v3._x == 4.0);
    ASSERT_TRUE(v3._y == 6.0);
    ASSERT_TRUE(v3._z == 8.0);
}

TEST(cross, dumpenv)
{
    // Création des vecteurs
    OPoint3D p1 = OPoint3D(2.0, 4.0, 3.0);
    OPoint3D p2 = OPoint3D(2.0, 2.0, 2.0);

    OVector3D v1(p1), v2(p2);

    // Calcul du produit vectoriel
    OVector3D v3 = v1.cross(v2);

    ASSERT_TRUE(v3._x ==  2.0);
    ASSERT_TRUE(v3._y ==  2.0);
    ASSERT_TRUE(v3._z == -4.0);
}

TEST(scalar, dumpenv)
{
    // Création des vecteurs
    OPoint3D p1 = OPoint3D(2.0, 3.0, 4.0);
    OPoint3D p2 = OPoint3D(1.0, 2.0, 3.0);


    OVector3D v1(p1), v2(p2);

    // Calcul du produit scalaire
    double res = v1.scalar(v2);

    ASSERT_TRUE(res == 20.0);
}

TEST(norme, dumpenv)
{
    // Création des vecteurs
    OPoint3D p1 = OPoint3D(3.0, 4.0, 0.0);

    OVector3D v1(p1);

    // Calcul de la soustraction
    double res = v1.norme();

    ASSERT_TRUE(res == 5.0);
}

TEST(normal, dumpenv)
{
    // Création des vecteurs
    OPoint3D p1 = OPoint3D(3.0, 2.0, 3.0);
    OPoint3D p2 = OPoint3D(4.0, 2.0, 3.0);
    OPoint3D p3 = OPoint3D(3.0, 3.0, 3.0);

    OVector3D v1(p1), v2(p2), v3(p3);

    // Calcul de la normale
    OVector3D v4 = v1.normal(v2, v3);

    ASSERT_TRUE(v4._x == 0.0);
    ASSERT_TRUE(v4._y == 0.0);
    ASSERT_TRUE(v4._z == 1.0);
}

TEST(normalize, dumpenv)
{
    // Création des vecteurs
    OPoint3D p1 = OPoint3D(3.0, 4.0, 0.0);

    OVector3D v1(p1);

    // Calcul de la soustraction
    v1.normalize();

    ASSERT_TRUE(v1._x == 0.6);
    ASSERT_TRUE(v1._y == 0.8);
    ASSERT_TRUE(v1._z == 0.0);
}

TEST(invert, dumpenv)
{
    // Création des vecteurs
    OPoint3D p1 = OPoint3D(3.0, -4.0, 5.0);

    OVector3D v1(p1);

    // Calcul du vecteur inverse
    v1.invert();

    ASSERT_TRUE(v1._x == -3.0);
    ASSERT_TRUE(v1._y ==  4.0);
    ASSERT_TRUE(v1._z == -5.0);
}
TEST(angle, dumpenv)
{
    // Création des vecteurs
    OPoint3D p1 = OPoint3D(sqrt(2.0) / 2.0, sqrt(2.0) / 2.0, 0.0);
    OPoint3D p2 = OPoint3D(-sqrt(2.0) / 2.0, sqrt(2.0) / 2.0, 0.0);

    OVector3D v1(p1), v2(p2);

    // Calcul de l'angle
    double res = v1.angle(v2);

	EXPECT_DOUBLE_EQ(M_PI / 2.0, res);
}

TEST(rotOz, dumpenv)
{
    // Création des vecteurs
    OPoint3D p1 = OPoint3D(sqrt(2.0) / 2.0, 0.0, sqrt(2.0) / 2.0);
    double alpha = M_PI;

    OVector3D v1(p1);

    // Calcul du vecteur tourné par Oz
    OVector3D v2 = v1.getRotationOz(alpha);

    ASSERT_TRUE(ABS(v2._x - (-sqrt(2.0) / 2.0)) < 1.0E-6) ;   //(v2._x == ( -sqrt(2.0)/2.0 ) );
    ASSERT_TRUE(ABS(v2._y) < 1.0E-6);
    ASSERT_TRUE(ABS(v2._z - (sqrt(2.0) / 2.0)) < 1.0E-6);      // v2._z == (  sqrt(2.0)/2.0 ) );
}

TEST(rotOy, dumpenv)
{
    // Création des vecteurs
    OPoint3D p1 = OPoint3D(sqrt(2.0) / 2.0, 0.0, sqrt(2.0) / 2.0);
    double alpha = M_PI;

    OVector3D v1(p1);

    // Calcul du vecteur tourné par Oz
    OVector3D v2 = v1.getRotationOy(alpha);

    ASSERT_TRUE(ABS(v2._x - (-sqrt(2.0) / 2.0)) < 1.0E-6);
    ASSERT_TRUE(ABS(v2._y) < 1.0E-6);
    ASSERT_TRUE(ABS(v2._z - (-sqrt(2.0) / 2.0)) < 1.0E-6);
}

TEST(rotOzBase2, dumpenv)
{
    // Création des vecteurs
    OPoint3D p1 = OPoint3D(sqrt(2.0) / 2.0, 0.0, sqrt(2.0) / 2.0);
    double alpha = M_PI;

    OVector3D v1(p1);

    // Calcul du vecteur tourné par Oz
    OVector3D v2 = v1.getRotationOzBase2(alpha);

    ASSERT_TRUE(ABS(v2._x - (-sqrt(2.0) / 2.0)) < 1.0E-6);
    ASSERT_TRUE(ABS(v2._y) < 1.0E-6);
    ASSERT_TRUE(ABS(v2._z - (sqrt(2.0) / 2.0)) < 1.0E-6);
}

TEST(rotOyBase2, dumpenv)
{
    // Création des vecteurs
    OPoint3D p1 = OPoint3D(sqrt(2.0) / 2.0, 0.0, sqrt(2.0) / 2.0);
    double alpha = M_PI;

    OVector3D v1(p1);

    // Calcul du vecteur tourné par Oz
    OVector3D v2 = v1.getRotationOyBase2(alpha);

    ASSERT_TRUE(ABS(v2._x - (-sqrt(2.0) / 2.0)) < 1.0E-6);
    ASSERT_TRUE(ABS(v2._y) < 1.0E-6);
    ASSERT_TRUE(ABS(v2._z - (-sqrt(2.0) / 2.0)) < 1.0E-6);
}

TEST(rotOzOy, dumpenv)
{
    // Création des vecteurs
    OPoint3D p1 = OPoint3D(sqrt(2.0) / 2.0, 0.0, sqrt(2.0) / 2.0);
    double alpha = M_PI, theta = M_PI / 2;

    OVector3D v1(p1);

    // Calcul du vecteur tourné par Oz puis Oy
    OVector3D v2 = v1.getRotationOzOy(alpha, theta);

    ASSERT_TRUE(ABS(v2._x - (sqrt(2.0) / 2.0)) < 1.0E-6);
    ASSERT_TRUE(ABS(v2._y) < 1.0E-6);
    ASSERT_TRUE(ABS(v2._z - (sqrt(2.0) / 2.0)) < 1.0E-6);
}

