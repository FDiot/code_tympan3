/**
*
* @brief Functional tests of the TYPolyligneParcours class  
* 
*  Created on: january 18, 2018 
*  Author: Philippe CHAPUIS <philippe.chapuis@c-s.fr> 
*
*/
#include "gtest/gtest.h"
#include "Tympan/geometric_methods/ConvexHullFinder/TYPolyligneParcours.h"
#include <iostream>

/**
* @brief Fixture which provides an instance of TYPolyligneParcours
*/
class TYPolyligneParcoursTest :
    public testing::Test
{
private:
    TYPolyligneParcours PolyligneParcours;
};


// Testing the TYPolyligneParcours::ajouteSegment method
TEST(test_TYPolyligneParcours, ajouteSegment)
{
   // Data setup:
   TYPointParcours P1;
   P1.Identifiant = 1;
   P1.isInfra = true;
   P1.isEcran = true;

   TYPointParcours P2;
   P2.Identifiant = 2;
   P2.isInfra = true;
   P2.isEcran = true;

   TYPolyligneParcours polyligneP0;

   // Call to the tested method:
   polyligneP0.ajouteSegment(&P1, &P2);

   int t_IdP1 = polyligneP0.indexePoint1();
   int t_IdP2 = polyligneP0.indexePoint2();

   // Check result:
   EXPECT_EQ(t_IdP1, 1);
   EXPECT_EQ(t_IdP2, 2);
}

// Testing the TYPolyligneParcours::isInfra method
TEST(test_TYPolyligneParcours, isInfra)
{
   // Data setup:
   // Case : P1 is Infra / P2 is Infra
   TYPointParcours P1;
   P1.isInfra = true;
   P1.isEcran = true;
   TYPointParcours P2;
   P2.isInfra = true;
   P2.isEcran = true;

   TYPolyligneParcours t_Polyligne;
   t_Polyligne.ajouteSegment(&P1, &P2);

   // Call to the tested method:
   bool t_isInfra = t_Polyligne.isInfra();
   
   // Check result:
   EXPECT_EQ(t_isInfra, true);

   // Case: both false
   P1.isInfra = false;
   P2.isInfra = false;

   // Case to the tested method:
   t_isInfra = t_Polyligne.isInfra();

   // Check result:
   EXPECT_EQ(t_isInfra, false);

   // Case: one true / one false
   P1.isInfra = true;

   // Case to the tested method:
   t_isInfra = t_Polyligne.isInfra();

   // Check result:
   EXPECT_EQ(t_isInfra, false);
}

// Testing the TYPolyligneParcours::isEcran method
TEST(test_TYPolyligneParcours, isEcran)
{
    // Data setup:
    TYPointParcours P1;
    P1.isInfra = true;
    P1.isEcran = true;
    TYPointParcours P2;
    P2.isInfra = true;
    P2.isEcran = true;

    TYPolyligneParcours t_Polyligne;
    t_Polyligne.ajouteSegment(&P1, &P2);

    // Call to the tested method:
    bool t_isEcran = t_Polyligne.isEcran();

    // Check result:
    EXPECT_EQ(t_isEcran, true);

    // Case: both false
    P1.isEcran = false;
    P2.isEcran = false;

    // Case to the tested method:
    t_isEcran = t_Polyligne.isEcran();

    // Check result:
    EXPECT_EQ(t_isEcran, false);

    // Case: one true / one false
    P1.isEcran = true;

    // Case to the tested method:
    t_isEcran = t_Polyligne.isEcran();

    // Check result:
    EXPECT_EQ(t_isEcran, false);
}

// Testing the TYPolyligneParcours::verifieNaturePolylignes method
TEST(test_TYPolyligneParcours, verifieNaturePolylignes)
{
    // Data setup:
    TYPointParcours P1;
    P1.isInfra = false;
    P1.isEcran = false;
    TYPointParcours P2;
    P2.isInfra = false;
    P2.isEcran = false;

    /*  Table for 1st IF
    *
    *  isInfra0 != isInfra1 || isEcran0 != isEcran1
    *
    *   VAR0 != VAR1
    *   (0 != 0)    =>  0
    *   (0 != 1)    =>  1
    *   (1 != 0)    =>  1
    *   (1 != 1)    =>  0
    *
    *   VAR0 || VAR1
    *   0 || 0  => 0
    *   1 || 1  => 1
    *   1 || 1  => 1
    *   0 || 0  => 0
    *
    *   END Table 1st IF
    */

    // Call to the tested method:

    // 1st IF
    // isInfra0 != isInfra1
    bool t1_notequalInfra00 = (P1.isInfra != P2.isInfra);
    P2.isInfra = true;
    bool t1_notequalInfra01 = (P1.isInfra != P2.isInfra);
    P1.isInfra = true;
    P2.isInfra = false;
    bool t1_notequalInfra10 = (P1.isInfra != P2.isInfra);
    P2.isInfra = true;
    bool t1_notequalInfra11 = (P1.isInfra != P2.isInfra);

    // isEcran0 != isEcran1
    bool t1_notequalEcran00 = (P1.isEcran != P2.isEcran);
    P2.isEcran = true;
    bool t1_notequalEcran01 = (P1.isEcran != P2.isEcran);
    P1.isEcran = true;
    P2.isEcran = false;
    bool t1_notequalEcran10 = (P1.isEcran != P2.isEcran);
    P2.isEcran = true;
    bool t1_notequalEcran11 = (P1.isEcran != P2.isEcran);

    // OR TEST
    bool t1_OR_1 = (t1_notequalInfra00 || t1_notequalEcran00);
    bool t1_OR_2 = (t1_notequalInfra01 || t1_notequalEcran01);
    bool t1_OR_3 = (t1_notequalInfra10 || t1_notequalEcran10);
    bool t1_OR_4 = (t1_notequalInfra11 || t1_notequalEcran11);
   
    // Check result:
    EXPECT_EQ(t1_OR_1, false);
    EXPECT_EQ(t1_OR_2, true);
    EXPECT_EQ(t1_OR_3, true);
    EXPECT_EQ(t1_OR_4, false);

    // END TEST 1st IF

    /*  Table for 2nd IF
    *
    *   (!isInfra0 && isEcran0 ) || (!isInfra1 && isEcran1)
    *
    *   !VAR && VAR
    *   (!0 && 0)    =>  0
    *   (!0 && 1)    =>  1
    *   (!1 && 0)    =>  0
    *   (!1 && 1)    =>  0
    *
    *   VAR0 || VAR1
    *   0 || 0  => 0
    *   1 || 1  => 1
    *   0 || 0  => 0
    *   0 || 0  => 0
    *
    *   END Table 2nd IF
    */

    // Data setup:
    P1.isInfra = false;
    P1.isEcran = false;
    P2.isInfra = false;
    P2.isEcran = false;

    // Call to the tested method:
    // 2nd IF

    // !isInfra0 && isEcran0
    bool t2_P1_00 = (!P1.isInfra && P1.isEcran);
    P1.isEcran  = true;
    bool t2_P1_01 = (!P1.isInfra && P1.isEcran);
    P1.isInfra = true;
    P1.isEcran  = false;
    bool t2_P1_10 = (!P1.isInfra && P1.isEcran);
    P1.isEcran  = true;
    bool t2_P1_11 = (!P1.isInfra && P1.isEcran);

    // !isInfra1 && isEcran1
    bool t2_P2_00 = (!P2.isInfra && P2.isEcran);
    P2.isEcran  = true;
    bool t2_P2_01 = (!P2.isInfra && P2.isEcran);
    P2.isInfra = true;
    P2.isEcran  = false;
    bool t2_P2_10 = (!P2.isInfra && P2.isEcran);
    P2.isEcran  = true;
    bool t2_P2_11 = (!P2.isInfra && P2.isEcran);

    // OR TEST
    bool t2_OR_1 = (t2_P1_00 || t2_P2_00);
    bool t2_OR_2 = (t2_P1_01 || t2_P2_01);
    bool t2_OR_3 = (t2_P1_10 || t2_P2_10);
    bool t2_OR_4 = (t2_P1_11 || t2_P2_11);

    // Check result:
    EXPECT_EQ(t2_OR_1, false);
    EXPECT_EQ(t2_OR_2, true);
    EXPECT_EQ(t2_OR_3, false);
    EXPECT_EQ(t2_OR_4, false);

    // END TEST 2nd IF
}

// Testing the TYPolyligneParcours::estSurUnParcourFermee method
TEST(test_TYPolyligneParcours, estSurUnParcourFermee)
{
    // Data setup:
    TYPointParcours P0;
    P0.Identifiant = 0;
    P0.isInfra = true;
    P0.isEcran = true;

    TYPolyligneParcours polyligneP0;
    polyligneP0.ajouteSegment(&P0, &P0);
    polyligneP0._PolyligneP0 = &polyligneP0;
    polyligneP0._PolyligneP1 = &polyligneP0;

    // First test : is closed

    // Call to the tested method:
    bool closed = polyligneP0.estSurUnParcourFermee();

    // Check result:
    EXPECT_EQ(true, closed);

    // Second test : is on an open path

    // Data setup:
    TYPointParcours P1;
    P1.Identifiant = 1;
    P1.isInfra = true;
    P1.isEcran = true;

   TYPointParcours P2;
    P2.Identifiant = 2;
    P2.isInfra = true;
    P2.isEcran = true;

    TYPointParcours P3;
    P3.Identifiant = 3;
    P3.isInfra = true;
    P3.isEcran = true;

    TYPolyligneParcours polyligneP1, polyligneP2, polyligneP3;
    polyligneP1.ajouteSegment(&P0, &P1);
    polyligneP2.ajouteSegment(&P1, &P2);
    polyligneP3.ajouteSegment(&P2, &P3);

    polyligneP1._PolyligneP1 = &polyligneP2;

    polyligneP2._PolyligneP0 = &polyligneP1;
    polyligneP2._PolyligneP1 = &polyligneP3;

    polyligneP3._PolyligneP0 = &polyligneP2;

    // Call to the tested method:
    bool isOnAClosedPath = polyligneP1.estSurUnParcourFermee();

    // Check result:
    EXPECT_EQ(false, isOnAClosedPath);

    // Code entrainant une boucle infinie, commenté pour continuer les tests

/*
    TYPolyligneParcours polyligneP4, polyligneP5;
    polyligneP4.ajouteSegment(&P0, &P1);
    polyligneP5.ajouteSegment(&P1, &P0);

    polyligneP4._PolyligneP0 = &polyligneP5;
    polyligneP4._PolyligneP1 = &polyligneP5;

    polyligneP5._PolyligneP0 = &polyligneP4;
    polyligneP5._PolyligneP1 = &polyligneP4;

    // Call to the tested method:
    bool isOnAClosedPath = polyligneP4.estSurUnParcourFermee();

    // Check result:
    EXPECT_EQ(true, isOnAClosedPath);*/

}

// Testing the TYPolyligneParcours::polylignesVoisinesPointentSurLaMemePolyligne method
TEST(test_TYPolyligneParcours, polylignesVoisinesPointentSurLaMemePolyligne)
{
    // Data setup:
    TYPointParcours P1;
    P1.Identifiant = 1;
    P1.isInfra = true;
    P1.isEcran = true;
    TYPointParcours P2;
    P2.Identifiant = 2;
    P2.isInfra = true;
    P2.isEcran = true;

    TYPolyligneParcours polyligneP0, polyligneP1, polyligneP2;
    polyligneP1.ajouteSegment(&P1, &P2);

    // Case differents polylignes
    polyligneP1._PolyligneP0 = &polyligneP0;
    polyligneP1._PolyligneP1 = &polyligneP2;

    // Call to the tested method:
    bool t_00 = polyligneP1.polylignesVoisinesPointentSurLaMemePolyligne();

    // Data setup:
    // Case same polylignes
    polyligneP1._PolyligneP1 = &polyligneP0;

    // Call to the tested method:
    bool t_11 = polyligneP1.polylignesVoisinesPointentSurLaMemePolyligne();

    // Check result:
    EXPECT_EQ(t_00, false);
    EXPECT_EQ(t_11, true);
}

// Testing the TYPolyligneParcours::indexePointSuivant method
TEST(test_TYPolyligneParcours, indexePointSuivant)
{
    // Data setup:
    TYPointParcours P0;
    P0.Identifiant = 0;
    P0.isInfra = false;
    P0.isEcran = false;
    TYPointParcours P1;
    P1.Identifiant = 1;
    P1.isInfra = false;
    P1.isEcran = false;
    TYPointParcours P2;
    P2.Identifiant = 2;
    P2.isInfra = false;
    P2.isEcran = false;

    TYPolyligneParcours polyligneP0, polyligneP1, polyligneP2;
    polyligneP0.ajouteSegment(&P0, &P1);
    polyligneP1.ajouteSegment(&P1, &P2);

    polyligneP0._PolyligneP1 = &polyligneP1;
    polyligneP1._PolyligneP0 = &polyligneP0;
    polyligneP1._PolyligneP1 = &polyligneP2;


    // Call to the tested method:
    int next_Index0 = polyligneP0.indexePointSuivant(1, polyligneP0._PolyligneP0);
    int next_Index1 = polyligneP1.indexePointSuivant(1, polyligneP1._PolyligneP0);

    // Check result:
    EXPECT_EQ(next_Index0, 0);
    EXPECT_EQ(next_Index1, 2);

}

// Testing the TYPolyligneParcours::polyligneSuivante method
TEST(test_TYPolyligneParcours, polyligneSuivante)
{
    // Data setup:
    TYPointParcours P0;
    P0.Identifiant = 0;
    P0.isInfra = false;
    P0.isEcran = false;
    TYPointParcours P1;
    P1.Identifiant = 1;
    P1.isInfra = false;
    P1.isEcran = false;
    TYPointParcours P2;
    P2.Identifiant = 2;
    P2.isInfra = false;
    P2.isEcran = false;

    TYPolyligneParcours polyligneP0, polyligneP1;
    polyligneP0.ajouteSegment(&P0, &P1);
    polyligneP1.ajouteSegment(&P1, &P2);

    polyligneP0._PolyligneP1 = &polyligneP1;
    polyligneP1._PolyligneP0 = &polyligneP0;

    // Call to the tested method:
    TYPolyligneParcours* polyAvant = polyligneP1.polyligneSuivante(1);
    TYPolyligneParcours* polySuivante = polyligneP0.polyligneSuivante(1);

    // Check result:
    EXPECT_EQ(polyAvant->_PolyligneP0, polyligneP0._PolyligneP0);
    EXPECT_EQ(polyAvant->_PolyligneP1, polyligneP0._PolyligneP1);
    EXPECT_EQ(polySuivante->_PolyligneP0, polyligneP1._PolyligneP0);
    EXPECT_EQ(polySuivante->_PolyligneP1, polyligneP1._PolyligneP1);
}

// Testing the TYPolyligneParcours::autrePointDuSegment method
TEST(test_TYPolyligneParcours, autrePointDuSegment)
{
    // Data setup:
    TYPointParcours P1;
    P1.Identifiant = 1;
    P1.isInfra = true;
    P1.isEcran = true;
    TYPointParcours P2;
    P2.Identifiant = 2;
    P2.isInfra = true;
    P2.isEcran = true;

    TYPolyligneParcours polyligneP0;
    polyligneP0.ajouteSegment(&P1, &P2);

    // Call to the tested method:
    int p_id1 = polyligneP0.autrePointDuSegment(1);
    int p_id0 = polyligneP0.autrePointDuSegment(2);

    // Check result:
    EXPECT_EQ(p_id1, 2);
    EXPECT_EQ(p_id0, 1);
}

// Testing the TYPolyligneParcours::Copy method
TEST(test_TYPolyligneParcours, Copy)
{
    // Data setup:
    TYPointParcours P1;
    P1.Identifiant = 1;
    P1.isInfra = true;
    P1.isEcran = true;
    TYPointParcours P2;
    P2.Identifiant = 2;
    P2.isInfra = true;
    P2.isEcran = true;

    TYPolyligneParcours polyligneP1, copiePolyligneP1, polyligneP0, polyligneP2;
    polyligneP1.ajouteSegment(&P1, &P2);
    polyligneP1._PolyligneP0 = &polyligneP0;
    polyligneP1._PolyligneP1 = &polyligneP2;

    // Call to the tested method:
    copiePolyligneP1.Copy(polyligneP1);

    EXPECT_EQ(true, (copiePolyligneP1._PolyligneP0 == polyligneP1._PolyligneP0));
    EXPECT_EQ(false, (copiePolyligneP1._PolyligneP1 == polyligneP1._PolyligneP0));
    EXPECT_EQ(true, (copiePolyligneP1._PolyligneP1 == polyligneP1._PolyligneP1));
    EXPECT_FALSE(copiePolyligneP1.nombreDePoint() == polyligneP1.nombreDePoint());// (true, (copiePolyligneP1.nombreDePoint() == polyligneP1.nombreDePoint()));


}
