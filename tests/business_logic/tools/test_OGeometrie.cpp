/**
 * \file test_OGeometrie.cpp
 * \test Testing of OGeometrie libs used by lot of TYMPAN features
 *
 *  Created on: april 17, 2013
 *      Author: Denis THOMASSON <denis.thomasson@edf.fr>
 */

#include <cstdlib>

#include "gtest/gtest.h"
#include "Tympan/MetierSolver/ToolsMetier/OGeometrie.h"
#include "Tympan/MetierSolver/ToolsMetier/OPoint3D.h"
#include "Tympan/MetierSolver/ToolsMetier/OVector3D.h"

using std::cout;
using std::cerr;
using std::endl;

TEST(NormaleTest, dumpenv) {
	// Création du tableau de points
	OPoint3D pts[3];
	pts[0] = OPoint3D(0.0, 0.0, 0.0);
	pts[1] = OPoint3D(1.0, 0.0, 0.0);
	pts[2] = OPoint3D(0.0, 1.0, 0.0);
	
	// Déclaration du vecteur resultat
	OVector3D normale;
	
	// Calcul de la normale
	OGeometrie::computeNormal(pts, 3, normale);

	ASSERT_TRUE(normale._z == 1);
}

TEST(SymPtDroite, dumpenv) {
	// Création des points
	OPoint3D ptA(-20.0, 0.0, 0.0), ptB(20.0, 0.0, 0.0), ptP(-20.0, 0.0, 2.0), ptI;

	// Calcul du point symétrique à PtP par rapport à la droite PtA-ptB
	double k = OGeometrie::symPointDroite(ptA, ptB, ptP, ptI);

	ASSERT_TRUE(ptI == OPoint3D(-20.0, 0.0, -2.0));
	ASSERT_TRUE(k == 0.0);

}

TEST(intersDemiSegmentAvecSegment, dumpenv) {
	// Création des points du segment et du point ptS
	OPoint3D ptA(-20.0, -10.0, 5.0), ptB(20.0, 12.0, 5.0), ptS(-20.0, 3.0, 5.0);

	// Test positif (le segment coupe l'horizontale)
	bool resu = OGeometrie::intersDemiSegmentAvecSegment(ptS, ptA, ptB);
	ASSERT_TRUE(resu == true);

	// Test négatif (le segment ne coupe pas l'horizontale)
	ptS._z = -3;
	resu = OGeometrie::intersDemiSegmentAvecSegment(ptS, ptA, ptB);
	ASSERT_FALSE(resu == false);
}

TEST(intersDroitesPoints, dumpenv) {
	// Création des points des 2 segments
	OPoint3D ptA(-10.0, 23.0, 0.0), ptB(10.0, -17.0, 0.0), ptC(-10.0, -32.0, 0.0), ptD(10.0, 28.0, 0.0), ptI;

	// Test positif (les 2 lignes se croisent)
	int resu = OGeometrie::intersDroitesPoints(ptA, ptB, ptC, ptD, ptI);
	ASSERT_TRUE(resu == 1);
	ASSERT_TRUE(ptI == OPoint3D(1.0, 1.0, 0.0) );

	// Test négatif (les 2 lignes sont parallèles)
	ptA._y = -1030, ptB._y = -970;
	resu = OGeometrie::intersDroitesPoints(ptA, ptB, ptC, ptD, ptI);
	ASSERT_TRUE(resu == 0);
}

TEST(pointInPolygonAngleSum, dumpenv) {
	// Définition du tableau de points
	OPoint3D pts[6];
	pts[0] = OPoint3D(1.0, 1.0, 0.0);
	pts[1] = OPoint3D(5.0, 6.0, 0.0);
	pts[2] = OPoint3D(-1.0, 10.0, 0.0);
	pts[3] = OPoint3D(-10.0, -2.0, 0.0);
	pts[4] = OPoint3D(1.0, -8.0, 0.0);
	pts[5] = OPoint3D(6.0, -3.0, 0.0);

	// Définition du point à tester
	OPoint3D ptP(-3.0, 3.0, 0.0);
	bool resu = OGeometrie::pointInPolygonAngleSum(ptP, pts, 6);

	// Test positif, le point est dans le polygone
	ASSERT_TRUE(resu == true);

	// Test négatif, les point n'est pas dans le polygone
	ptP._x = 4.0;
	resu = OGeometrie::pointInPolygonAngleSum(ptP, pts, 6);
	ASSERT_FALSE(resu == true);
}

TEST(pointInPolygonRayCasting, dumpenv) {
	// Définition du tableau de points
	OPoint3D pts[6];
	pts[0] = OPoint3D(1.0, 1.0, 0.0);
	pts[1] = OPoint3D(5.0, 6.0, 0.0);
	pts[2] = OPoint3D(-1.0, 10.0, 0.0);
	pts[3] = OPoint3D(-10.0, -2.0, 0.0);
	pts[4] = OPoint3D(1.0, -8.0, 0.0);
	pts[5] = OPoint3D(6.0, -3.0, 0.0);

	// Calcul du vecteur normal à la face
	OVector3D normale;
	OGeometrie::computeNormal(pts, 6, normale);

	// Définition de la bounding box
	OBox box(OCoord3D(-10.0, -8.0, 0.0), OCoord3D(6.0, 10.0, 0.0));

	// Définition du point à tester
	OPoint3D ptP(-3.0, 3.0, 0.0);
	bool resu = OGeometrie::pointInPolygonRayCasting(ptP, pts, 6);

	// Test positif, le point est dans le polygone
	ASSERT_TRUE(resu == true);

	// Test négatif, les point n'est pas dans le polygone
	ptP._x = 4.0;
	resu = OGeometrie::pointInPolygonRayCasting(ptP, pts, 6);
	ASSERT_FALSE(resu == true);
}

TEST(shortestSegBetween2Lines, dumpenv) {
	// Création des points des 2 segments et du segment retourné
	OPoint3D pt1(-10.0, 23.0, 0.0), pt2(10.0, -17.0, 0.0), pt3(-10.0, -32.0, 3.0), pt4(10.0, 28.0, 3.0), ptA, ptB;

	// Déclaration de mua et mub
	double* mua = new double;
	double* mub = new double;

	// Cas 1 : les deux segments se croisent à une certaine distance
	bool resu = OGeometrie::shortestSegBetween2Lines(pt1, pt2, pt3, pt4, ptA, ptB, mua, mub);
	ASSERT_TRUE(resu == true);
	ASSERT_TRUE( ptA == OPoint3D(1.0, 1.0, 0.0) );
	ASSERT_TRUE( ptB == OPoint3D(1.0, 1.0, 3.0) );

	// Cas 2 : les deux segments ont une intersection
	pt3._z = pt4._z = 0.0;
	resu = OGeometrie::shortestSegBetween2Lines(pt1, pt2, pt3, pt4, ptA, ptB, mua, mub);
	ASSERT_TRUE(resu == true);
	ASSERT_TRUE( ptA == OPoint3D(1.0, 1.0, 0.0) );
	ASSERT_TRUE( ptB == OPoint3D(1.0, 1.0, 0.0) );
}

TEST(boundingBox, dumpenv) {
	// Définition du tableau de points
	OPoint3D pts[6];
	pts[0] = OPoint3D(1.0, 1.0, 5.0);
	pts[1] = OPoint3D(5.0, 6.0, 0.0);
	pts[2] = OPoint3D(-1.0, 10.0, 0.0);
	pts[3] = OPoint3D(-10.0, -2.0, 0.0);
	pts[4] = OPoint3D(1.0, -8.0, 0.0);
	pts[5] = OPoint3D(6.0, -3.0, 0.0);

	// Déclaration ptMin et ptMax
	OPoint3D ptMin, ptMax;

	// Calcul des points
	OGeometrie::boundingBox(pts, 6, ptMin, ptMax);

	ASSERT_TRUE(ptMin == OPoint3D(-10.0, -8.0, 0.0));
	ASSERT_TRUE(ptMax == OPoint3D(6.0, 10.0, 5.0));
}

TEST(intersDroitesPointVecteur, dumpenv) {
	// Définition des points
	OPoint3D ptA0(-10.0, 23.0, 0.0), ptA1(10.0, -17.0, 0.0), ptB0(-10.0, -32.0, 0.0), ptB1(10.0, 28.0, 0.0), ptI;

	// Création des vecteurs
	OVector3D vecA(ptA0, ptA1), vecB(ptB0, ptB1);

	// Test positif (les 2 lignes se croisent en 1,1)
	int resu = OGeometrie::intersDroitesPointVecteur(ptA0, vecA, ptB0, vecB, ptI);
	ASSERT_TRUE(resu == 1);
	ASSERT_TRUE(ptI == OPoint3D(1.0, 1.0, 0.0) );

	// Test négatif (les 2 lignes sont parallèles)
	ptA0._y = -1030, ptA1._y = -970;
	vecA = vecB;
	resu = OGeometrie::intersDroitesPointVecteur(ptA0, vecA, ptB0, vecB, ptI);
	ASSERT_TRUE(resu == 0);
}