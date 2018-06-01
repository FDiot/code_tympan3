/**
*
* @brief Functional tests of the TYPointParcours class  
* 
*  Created on: January 18, 2018
*  Author: Pierre LEDAC <pierre.ledac@c-s.fr>
*
*/
#include "gtest/gtest.h"
#include "Tympan/geometric_methods/ConvexHullFinder/TYPointParcours.h"

/**
* @brief Fixture which provides an instance of TYPointParcours
*/
class TYPointParcoursTest :
    public testing::Test
{
private:
    TYPointParcours PointParcours;
};

//TEST_F(TYPointParcoursTest, create_instance)
//{
//}

// Testing the TYPointParcours::vecteur2D method
TEST(test_TYPointParcours, vecteur2D)
{
	// Data setup:
	TYPointParcours P1;
	P1.x = 1.0;
	P1.y = 1.0;
	TYPointParcours P2;
	P2.x = 10.0;
	P2.y = 10.0;

	// Call to the tested method:
	TYPointParcours P12 = TYPointParcours::vecteur2D(P1, P2);
   
	// Check result:
	EXPECT_EQ(9.0, P12.x);
	EXPECT_EQ(9.0, P12.y);
}

// Testing the TYPointParcours::ZCross method
TEST(test_TYPointParcours, ZCross)
{
	// Data setup:
	TYPointParcours P1;
	P1.x = 2.0;
	P1.y = 2.0;
	TYPointParcours P2;
	P2.x = 11.0;
	P2.y = 10.0;

	// Call to the tested method:
	double zcross = TYPointParcours::ZCross(P1, P2);
   
	// Check result:
	EXPECT_EQ(-2.0, zcross);
}

// Testing the TYPointParcours::normeCarree method
TEST(test_TYPointParcours, normeCarree)
{
	// Data setup:
	TYPointParcours P;
	P.x = 3.0;
    P.y = -2.0;

   // Call to the tested method:
   double norm = P.normeCarree();
   
   // Check result:
   EXPECT_EQ(13.0, norm);
}

// Testing the TYPointParcours::Scalaire method
TEST(test_TYPointParcours, Scalaire)
{
	// Data setup:
	TYPointParcours P1;
	P1.x = 2.0;
	P1.y = 3.0;
	TYPointParcours P2;
	P2.x = 4.0;
	P2.y = 5.0;
	TYPointParcours P3;
	P3.x = -1.0;
	P3.y = 2.0;
	TYPointParcours P4;
	P4.x = 4.0;
	P4.y = -2.0;

	// Call to the tested method:
	double scalar_product1 = TYPointParcours::Scalaire(P1, P2);
	double scalar_product2 = TYPointParcours::Scalaire(P1, P2, P3, P4);
   
	// Check result:
	EXPECT_EQ(23.0, scalar_product1);
	EXPECT_EQ(2.0, scalar_product2);
}

// Testing the TYPointParcours::Confondus method
TEST(test_TYPointParcours, Confondus)
{
	// Data setup:
	TYPointParcours P1;
	P1.x = 2.0;
	P1.y = 2.99999;
	TYPointParcours P2;
	P2.x = 2.00001;
	P2.y = 3.0;

	// Call to the tested method:
	bool points_confondus = TYPointParcours::Confondus(&P1, &P2);
	EXPECT_EQ(points_confondus, P1 == P2); // Check also operator ==

	P2.x += 0.1; // We move slightly the point so not the same location anymore
	bool points_non_confondus = !TYPointParcours::Confondus(&P1, &P2);
   
	// Check result:
	EXPECT_EQ(true, points_confondus);
	EXPECT_EQ(true, points_non_confondus);
}

// Testing the TYPointParcours::AbscisseCurviligneCarreSurSR method
TEST(test_TYPointParcours, AbscisseCurviligneCarreSurSR)
{
	// Data setup:
	TYPointParcours P;
	P.x = 3.0;
	P.y = 4.0;
	TYPointParcours S;
	S.x = 2.0;
	S.y = 3.0;
	TYPointParcours R;
	R.x = 4.0;
	R.y = 5.0;

	// Call to the tested method:
	double carre_abs_curv = TYPointParcours::AbscisseCurviligneCarreSurSR(P, S, R);
   
	// Check result:
	EXPECT_EQ(0.25, carre_abs_curv); // P is the middle of S and R
	EXPECT_EQ(1.0, TYPointParcours::AbscisseCurviligneCarreSurSR(R, S, R)); // P=R so 1
	EXPECT_EQ(0.0, TYPointParcours::AbscisseCurviligneCarreSurSR(S, S, R)); // P=S so 0
	P.x = 0.0;
	P.y = 1.0;
	EXPECT_EQ(-1.0, TYPointParcours::AbscisseCurviligneCarreSurSR(P, S, R)); // S is now the middle of P and R
}

// Testing the TYPointParcours::IntersectionSegments method
TEST(test_TYPointParcours, IntersectionSegments)
{
	// Data setup:
	TYPointParcours P1;
	P1.x = 1.0;
	P1.y = 2.0;
	P1.z = 100.;
	TYPointParcours P2;
	P2.x = 5.0;
	P2.y = 6.0;
	P2.z = 100.;
	TYPointParcours P3;
	P3.x = 3.0;
	P3.y = 0.0;
	P3.z = 150.; // Different altitude for P3 and P4
	TYPointParcours P4;
	P4.x = 1.0;
	P4.y = 6.0;
	P4.z = 50.; // Different altitude for P3 and P4
	TYPointParcours P;

	// Call to the tested method:
	bool intersection = TYPointParcours::IntersectionSegments(P1, P2, P3, P4, P);
	// Check result:
    EXPECT_EQ(true, intersection);
	EXPECT_EQ(2., P.x);
	EXPECT_EQ(3., P.y);
	EXPECT_EQ(100., P.z);
	// Basic checks (intersection can't be one of the 4 points):
	EXPECT_EQ(false, TYPointParcours::IntersectionSegments(P1, P2, P3, P1, P));
	EXPECT_EQ(false, TYPointParcours::IntersectionSegments(P1, P2, P3, P2, P));
	EXPECT_EQ(false, TYPointParcours::IntersectionSegments(P1, P2, P1, P4, P));
	EXPECT_EQ(false, TYPointParcours::IntersectionSegments(P1, P2, P2, P4, P));

	P3.x += 10; // Move P3 and P4 to prevent from intersecting
	P4.x += 10;
	EXPECT_EQ(false, TYPointParcours::IntersectionSegments(P1, P2, P3, P4, P)); // Check non intersection
}

// Testing the TYPointParcours::IntersectionDroites method
TEST(test_TYPointParcours, IntersectionDroites)
{
	// Data setup:
	TYPointParcours P1;
	P1.x = 1.0;
	P1.y = 2.0;
	P1.z = 100.;
	TYPointParcours P2;
	P2.x = 5.0;
	P2.y = 6.0;
	P2.z = 100.;
	TYPointParcours P3;
	P3.x = 3.0;
	P3.y = 0.0;
	P3.z = 150.; // Different altitude for P3 and P4
	TYPointParcours P4;
	P4.x = 1.0;
	P4.y = 6.0;
	P4.z = 50.; // Different altitude for P3 and P4
	TYPointParcours P;

	// Call to the tested method:
	bool intersection = TYPointParcours::IntersectionDroites(P1, P2, P3, P4, P);
	// Check result:
	EXPECT_EQ(true, intersection);
	EXPECT_EQ(2., P.x);
	EXPECT_EQ(3., P.y);
	EXPECT_EQ(100., P.z);
	// Same results for P but set P3P4 vertical then horizontal to cover all the method:
    P3.x = 2.0;
    P4.x = P3.x;
    TYPointParcours Q;
    EXPECT_EQ(true, TYPointParcours::IntersectionDroites(P1, P2, P3, P4, Q));
    EXPECT_EQ(true, P == Q);
    P3.x = 0.0;
    P3.y = 3.0;
    P4.x = 4.0;
    P4.y = P3.y;
    EXPECT_EQ(true, TYPointParcours::IntersectionDroites(P1, P2, P3, P4, Q));
    EXPECT_EQ(true, P == Q);
	// Basic checks
	EXPECT_EQ(false, TYPointParcours::IntersectionDroites(P1, P2, P1, P2, P)); // Colinear so no single intersection
	EXPECT_EQ(true, TYPointParcours::IntersectionDroites(P1, P1, P1, P2, P)); // P1P1 intersect P1P1
	EXPECT_EQ(true, P == P1);
	P3 = P1;
	P4 = P2;
	P3.y += 1.0;
	P4.y += 1.0;
	EXPECT_EQ(false, TYPointParcours::IntersectionDroites(P1, P2, P3, P4, P)); // Parallel so no intersection
}
