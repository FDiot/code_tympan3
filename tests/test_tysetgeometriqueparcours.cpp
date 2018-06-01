/**
*
* @brief Functional tests of the TYSetGeometriqueParcours class  
* 
*  Created on: january 18, 2018 
*  Author: Philippe CHAPUIS <philippe.chapuis@c-s.fr> 
*
*/
#include "gtest/gtest.h"
#include "Tympan/geometric_methods/ConvexHullFinder/TYSetGeometriqueParcours.h"
#include <iostream>

/**
* @brief Fixture which provides an instance of TYSetGeometriqueParcours
*/
class TYSetGeometriqueParcoursTest :
    public testing::Test
{
private:
    TYSetGeometriqueParcours SetGeometriqueParcours;
};

//TEST_F(TYSetGeometriqueParcoursTest, create_instance)
//{
//}

// PRIVATE
// Testing the TYSetGeometriqueParcours::compareTYPolyligneParcours method
TEST(test_TYSetGeometriqueParcours, compareTYPolyligneParcours)
{
   // Data setup:

   // Call to the tested method:
   
   // Check result:
   //EXPECT_EQ(result, expected_result);
}

// PRIVATE
// Testing the TYSetGeometriqueParcours::compareAbscissesCurvilignes method
TEST(test_TYSetGeometriqueParcours, compareAbscissesCurvilignes)
{
   // Data setup:
   // To do

   // Call to the tested method:
   // int integer = TYSetGeometriqueParcours::compareAbscisseCurvilignes()...
   
   // Check result:
   //EXPECT_EQ(result, expected_result);
}

// DONE
// Testing the TYSetGeometriqueParcours::Copy method
TEST(test_TYSetGeometriqueParcours, Copy)
{
    /*
    // Data setup:
    TYPointParcours S;
    S.Identifiant = 0;
    S.x = 1.0;
    S.y = 1.0;
    S.isEcran = S.isInfra = true;
    TYPointParcours R;
    R.Identifiant = 1;
    R.x = 10.0;
    R.y = 10.0;
    R.isEcran = R.isInfra = true;

    // Polylignes
    TYPolyligneParcours polyligneSR;
    polyligneSR.ajouteSegment(&S, &R);

    // Geometrique Parcours
    TYSetGeometriqueParcours geoParcoursP0, copieGeoParcoursP0;

    geoParcoursP0._nNbPointTotal = 2;
    geoParcoursP0._nNbPolylines = 1;
    geoParcoursP0._ListePolylines = new TYPolyligneParcours[geoParcoursP0._nNbPolylines];
    geoParcoursP0._ListePoint = new TYPointParcours[geoParcoursP0._nNbPointTotal];
    geoParcoursP0._ListePoint[0] = S;
    geoParcoursP0._ListePoint[1] = R;
    geoParcoursP0._ListePolylines[0].Copy(polyligneSR);

    // Call to the tested method:
    copieGeoParcoursP0.Copy(geoParcoursP0);

    // Check result:
    bool copy_check = true;
    if(copieGeoParcoursP0._nNbPointTotal != geoParcoursP0._nNbPointTotal){
        copy_check = false;
    }
    if(copieGeoParcoursP0._nNbPolylines != geoParcoursP0._nNbPolylines){
        copy_check = false;
    }
    // Check if the lists are initialized
    if ((copieGeoParcoursP0._ListePoint == NULL) || (copieGeoParcoursP0._ListePolylines == NULL)){
        copy_check = false;
    }
    if (copieGeoParcoursP0._ListePoint[0].Identifiant != 0){
        copy_check = false;
    }
    if (copieGeoParcoursP0._ListePoint[1].Identifiant != 1){
        copy_check = false;
    }
    if (copieGeoParcoursP0._ListePolylines[0].indexePoint1() != 0){
        copy_check = false;
    }
    if (copieGeoParcoursP0._ListePolylines[0].indexePoint2() != 1){
        copy_check = false;
    }

    EXPECT_EQ(true, copy_check);

    //destruction
    geoParcoursP0.~TYSetGeometriqueParcours();
    copieGeoParcoursP0.~TYSetGeometriqueParcours();
    polyligneSR.~TYPolyligneParcours();
    */
}

// PRIVATE
// Testing the TYSetGeometriqueParcours::SwapPolyligne method
TEST(test_TYSetGeometriqueParcours, SwapPolyligne)
{
   // Data setup:
   // To Do

   // Call to the tested method:
   // TYSetGeometriqueParcours::SwapPolyligne()...
   
   // Check result:
   //EXPECT_EQ(result, expected_result);
}

// DONE
// Testing the TYSetGeometriqueParcours::SupressionPolylignesRedondantes method
TEST(test_TYSetGeometriqueParcours, SupressionPolylignesRedondantes)
{
    /*
    // Data setup: (redondante)
    TYPointParcours S;
    S.Identifiant = 0;
    S.isEcran = S.isInfra = true;
    S.x = 2.0;
    S.y = 3.0;

    TYPointParcours R;
    R.Identifiant = 2;
    R.x = 4.0;
    R.y = 5.0;
    R.isEcran = R.isInfra = true;

    TYPointParcours P1;
    P1.Identifiant = 1;
    P1.isEcran = P1.isInfra = true;
    P1.x = 3.0;
    P1.y = 4.0;

    // Data pour le test redondant
    TYPolyligneParcours polyligneR1;
    polyligneR1.ajouteSegment(&S, &P1);
    TYPolyligneParcours polyligneR2;
    polyligneR2.ajouteSegment(&P1, &R);
    TYPolyligneParcours polyligneR3;
    polyligneR3.ajouteSegment(&R, &P1);

    polyligneR1._PolyligneP1 = &polyligneR2;
    polyligneR2._PolyligneP0 = &polyligneR1;
    polyligneR2._PolyligneP1 = &polyligneR3;
    polyligneR3._PolyligneP0 = &polyligneR2;
    polyligneR3._PolyligneP1 = &polyligneR2;

    TYSetGeometriqueParcours geoParcoursRevient;
    //geoParcoursRevient.AllouerPolylignes(3);
    geoParcoursRevient._nNbPolylines = 3;
    geoParcoursRevient._ListePolylines = new TYPolyligneParcours[3];
    geoParcoursRevient._ListePolylines[0].Copy(polyligneR1);
    geoParcoursRevient._ListePolylines[1].Copy(polyligneR2);
    geoParcoursRevient._ListePolylines[2].Copy(polyligneR3);
    geoParcoursRevient._nNbPointTotal = 3;
    geoParcoursRevient._ListePoint = new TYPointParcours[3];
    geoParcoursRevient._ListePoint[0] = S;
    geoParcoursRevient._ListePoint[1] = P1;
    geoParcoursRevient._ListePoint[2] = R;

    // Call to the tested method:
    int nRevient = geoParcoursRevient.SupressionPolylignesRedondantes();

    // Data Setup: (Boucle)
    TYPolyligneParcours polyligneB1;
    polyligneB1.ajouteSegment(&S, &P1);
    TYPolyligneParcours polyligneB2;
    polyligneB2.ajouteSegment(&P1, &P1);

    polyligneB1._PolyligneP1 = &polyligneB2;
    polyligneB2._PolyligneP0 = &polyligneB1;
    polyligneB2._PolyligneP1 = &polyligneB2;

    // Geometrique Parcours
    TYSetGeometriqueParcours geoParcoursBoucle;

    geoParcoursBoucle._nNbPolylines = 2;
    geoParcoursBoucle._ListePolylines = new TYPolyligneParcours[2];
    geoParcoursBoucle._ListePolylines[0].Copy(polyligneB1);
    geoParcoursBoucle._ListePolylines[1].Copy(polyligneB2);
    geoParcoursBoucle._nNbPointTotal = 2;
    geoParcoursBoucle._ListePoint = new TYPointParcours[2];
    geoParcoursBoucle._ListePoint[0] = S;
    geoParcoursBoucle._ListePoint[1] = P1;

    // Call to the tested method:
    int nBoucle = geoParcoursBoucle.SupressionPolylignesRedondantes();

    // Data Setup: (boucle + redondance)

    TYPolyligneParcours polyligneP1;
    polyligneP1.ajouteSegment(&S, &P1);
    TYPolyligneParcours polyligneP2;        //boucle
    polyligneP2.ajouteSegment(&P1, &P1);
    TYPolyligneParcours polyligneP3;
    polyligneP3.ajouteSegment(&P1, &R);
    TYPolyligneParcours polyligneP4;        //redondance
    polyligneP4.ajouteSegment(&R, &P1);

    polyligneP1._PolyligneP1 = &polyligneP3;
    polyligneP2._PolyligneP0 = &polyligneP1;
    polyligneP2._PolyligneP1 = &polyligneP2;
    polyligneP3._PolyligneP0 = &polyligneP1;
    polyligneP4._PolyligneP0 = &polyligneP3;

    TYSetGeometriqueParcours geoParcours;
    geoParcours._nNbPolylines = 4;
    geoParcours._ListePolylines = new TYPolyligneParcours[4];
    geoParcours._ListePolylines[0].Copy(polyligneP1);
    geoParcours._ListePolylines[1].Copy(polyligneP2);
    geoParcours._ListePolylines[2].Copy(polyligneP3);
    geoParcours._ListePolylines[3].Copy(polyligneP4);
    geoParcours._nNbPointTotal = 3;
    geoParcours._ListePoint = new TYPointParcours[3];
    geoParcours._ListePoint[0] = S;
    geoParcours._ListePoint[1] = P1;
    geoParcours._ListePoint[2] = R;

    // Call to the tested method:
    int nDouble = geoParcours.SupressionPolylignesRedondantes();

    // Check result:
    bool bGeoParcoursP1, bGeoParcoursP2;
    bGeoParcoursP1 = true;
    bGeoParcoursP2 = true;
    EXPECT_EQ(1, nBoucle);
    EXPECT_EQ(1, nRevient);
    EXPECT_EQ(2, nDouble);
    EXPECT_EQ(2, geoParcours._nNbPolylines);

    if ((geoParcours._ListePolylines[0].indexePoint1() != 0) || (geoParcours._ListePolylines[0].indexePoint2() != 1)){
        bGeoParcoursP1 = false;
    }
    if ((geoParcours._ListePolylines[1].indexePoint1() != 1) || (geoParcours._ListePolylines[1].indexePoint2() != 2)){
        bGeoParcoursP2 = false;
    }
    EXPECT_EQ(true, bGeoParcoursP1);
    EXPECT_EQ(true, bGeoParcoursP2);

    // Destructions des objets
    geoParcours.~TYSetGeometriqueParcours();
    geoParcoursBoucle.~TYSetGeometriqueParcours();
    geoParcoursRevient.~TYSetGeometriqueParcours();
    polyligneB1.~TYPolyligneParcours();
    polyligneB2.~TYPolyligneParcours();
    polyligneP1.~TYPolyligneParcours();
    polyligneP2.~TYPolyligneParcours();
    polyligneP3.~TYPolyligneParcours();
    polyligneP4.~TYPolyligneParcours();
    polyligneR1.~TYPolyligneParcours();
    polyligneR2.~TYPolyligneParcours();
    polyligneR3.~TYPolyligneParcours();
    */
}

// DONE
// Testing the TYSetGeometriqueParcours::MergePointsDoubles method
TEST(test_TYSetGeometriqueParcours, MergePointsDoubles)
{
    /*
    // Data setup:
    // Points
    TYPointParcours S;
    S.Identifiant= 0;
    S.isInfra = S.isEcran = true;
    S.x = 1.0;
    S.y = 1.0;

    TYPointParcours A;
    A.Identifiant = 1;
    A.isEcran = A.isInfra = true;
    A.x = 5.0;
    A.y = 4.99999;

    TYPointParcours B;
    B.Identifiant = 2;
    B.isInfra = B.isEcran = true;
    B.x = 10.0;
    B.y = 10.0;

    TYPointParcours C;
    C.Identifiant = 3;
    C.isInfra = C.isEcran = true;
    C.x = 4.99999;
    C.y = 5.0;

    TYPointParcours R;
    R.Identifiant = 4;
    R.isInfra = R.isEcran = true;
    R.x = 1.1;
    R.y = 1.1;

    TYPointParcours S2;
    S2.Identifiant = 0;
    S2.isEcran = S2.isInfra = false;
    S2.x = 1.0;
    S2.y = 1.0;

    TYPointParcours B2;
    B2.Identifiant = 2;
    B2.isEcran = B2.isInfra = false;
    B2.x = 10.0;
    B2.y = 10.0;

    // Polylignes
    TYPolyligneParcours polyligneP1;
    polyligneP1.ajouteSegment(&S, &A);
    TYPolyligneParcours polyligneP2;
    polyligneP2.ajouteSegment(&A, &B);
    TYPolyligneParcours polyligneP3;
    polyligneP3.ajouteSegment(&B, &C);
    TYPolyligneParcours polyligneP4;
    polyligneP4.ajouteSegment(&C, &R);
    TYPolyligneParcours polyligneP5;
    polyligneP5.ajouteSegment(&S2, &B2);

    polyligneP1._PolyligneP1 = &polyligneP2;
    polyligneP2._PolyligneP0 = &polyligneP1;
    polyligneP2._PolyligneP1 = &polyligneP3;
    polyligneP3._PolyligneP0 = &polyligneP2;
    polyligneP3._PolyligneP1 = &polyligneP4;
    polyligneP4._PolyligneP0 = &polyligneP3;
    polyligneP5._PolyligneP1 = &polyligneP3;

    // Geometriques Parcours
    TYSetGeometriqueParcours geoParcours;
    geoParcours._nNbPointTotal = 7;
    geoParcours._nNbPolylines = 5;
    geoParcours._ListePolylines = new TYPolyligneParcours[5];
    geoParcours._ListePolylines[0].Copy(polyligneP1);
    geoParcours._ListePolylines[1].Copy(polyligneP2);
    geoParcours._ListePolylines[2].Copy(polyligneP3);
    geoParcours._ListePolylines[3].Copy(polyligneP4);
    geoParcours._ListePolylines[4].Copy(polyligneP5);
    geoParcours._ListePoint = new TYPointParcours[geoParcours._nNbPointTotal];
    geoParcours._ListePoint[0] = S;
    geoParcours._ListePoint[1] = A;
    geoParcours._ListePoint[2] = B;
    geoParcours._ListePoint[3] = C;
    geoParcours._ListePoint[4] = R;
    geoParcours._ListePoint[5] = S2;
    geoParcours._ListePoint[6] = B2;

    // Call to the tested method:
    int nDoublons = geoParcours.MergePointsDoubles();

    // Check result:
    EXPECT_EQ(1, nDoublons);
    EXPECT_EQ(1, geoParcours._ListePolylines[2].indexePoint2());
    EXPECT_EQ(1, geoParcours._ListePolylines[3].indexePoint1());
    EXPECT_EQ(4, geoParcours._ListePolylines[3].indexePoint2());
    EXPECT_EQ(-1, geoParcours._ListePoint[3].Identifiant);
    EXPECT_EQ(0, geoParcours._ListePoint[5].Identifiant);
    EXPECT_EQ(2, geoParcours._ListePoint[6].Identifiant);
    EXPECT_EQ(0, geoParcours._ListePolylines[4].indexePoint1());
    EXPECT_EQ(2, geoParcours._ListePolylines[4].indexePoint2());

    // Destructions
    geoParcours.~TYSetGeometriqueParcours();
    polyligneP1.~TYPolyligneParcours();
    polyligneP2.~TYPolyligneParcours();
    polyligneP3.~TYPolyligneParcours();
    polyligneP4.~TYPolyligneParcours();
    polyligneP5.~TYPolyligneParcours();
    */
}

// DONE
// Testing the TYSetGeometriqueParcours::RamenerPointsTraversantLaFrontiere method
TEST(test_TYSetGeometriqueParcours, RamenerPointsTraversantLaFrontiere)
{
    /*
    // Data setup:
    // Points
    TYPointParcours S;
    S.Identifiant = 0;
    S.x = 1.0;
    S.y = 1.0;
    S.isEcran = S.isInfra = false;
    TYPointParcours D;
    D.Identifiant = 5;
    D.x = 10.0;
    D.y = 10.0;
    D.isEcran = D.isInfra = false;
    TYPointParcours P1;
    P1.Identifiant = 1;
    P1.x = 2.0;
    P1.y = 4.0;
    P1.isEcran = P1.isInfra = false;
    TYPointParcours P2;
    P2.Identifiant = 2;
    P2.x = 5.0;
    P2.y = 5.0;
    P2.isEcran = P2.isInfra = false;
    TYPointParcours P3;
    P3.Identifiant = 3;
    P3.x = 7.0;
    P3.y = 3.0;
    P3.isEcran = P3.isInfra = false;
    TYPointParcours P4;
    P4.Identifiant = 4;
    P4.x = 8.0;
    P4.y = 9.0;
    P4.isEcran = P4.isInfra = false;


    // Polylignes
    //TYPolyligneParcours polyligneS;
    //polyligneS.ajouteSegment(&S, &D);
    TYPolyligneParcours polyligneP1;
    polyligneP1.ajouteSegment(&S, &P1);
    TYPolyligneParcours polyligneP2;
    polyligneP2.ajouteSegment(&P1, &P2);
    TYPolyligneParcours polyligneP3;
    polyligneP3.ajouteSegment(&P2, &P3);
    TYPolyligneParcours polyligneP4;
    polyligneP4.ajouteSegment(&P3, &P4);
    TYPolyligneParcours polyligneP5;
    polyligneP5.ajouteSegment(&P4, &D);

    // Geometrique Parcours
    TYSetGeometriqueParcours geoParcours, geoGauche, geoDroite;
    //geoParcours.AllouerPolylignes(5);
    geoParcours._nNbPointTotal = 6;
    geoParcours._ListePoint = new TYPointParcours[6];
    geoParcours._ListePoint[0] = S;
    geoParcours._ListePoint[1] = P1;
    geoParcours._ListePoint[2] = P2;
    geoParcours._ListePoint[3] = P3;
    geoParcours._ListePoint[4] = P4;
    geoParcours._ListePoint[5] = D;
    geoParcours._nNbPolylines = 5;
    geoParcours._ListePolylines = new TYPolyligneParcours[5];
    geoParcours._ListePolylines[0].Copy(polyligneP1);
    geoParcours._ListePolylines[1].Copy(polyligneP2);
    geoParcours._ListePolylines[2].Copy(polyligneP3);
    geoParcours._ListePolylines[3].Copy(polyligneP4);
    geoParcours._ListePolylines[4].Copy(polyligneP5);

    bool* PointAGauche;
    bool* PointADroite;
    int* IndexPointFrontiere = new int[100];
    int NbPointFrontiere = 0;
    bool* EstUnPointIntersectant = new bool[100];
    bool bCoteGauche = true;

   geoParcours.MarquePointsADroiteEtAGauche(S, D, PointAGauche, PointADroite);

   // Call to the tested method:
   geoParcours.RamenerPointsTraversantLaFrontiere(S, D, IndexPointFrontiere, NbPointFrontiere, EstUnPointIntersectant, bCoteGauche, PointAGauche, PointADroite);
   
   // Check result:
   EXPECT_EQ(3, NbPointFrontiere);
   EXPECT_EQ(2, IndexPointFrontiere[0]);

   // Destruction
   geoDroite.~TYSetGeometriqueParcours();
   geoGauche.~TYSetGeometriqueParcours();
   geoParcours.~TYSetGeometriqueParcours();
   polyligneP1.~TYPolyligneParcours();
   polyligneP2.~TYPolyligneParcours();
   polyligneP3.~TYPolyligneParcours();
   polyligneP4.~TYPolyligneParcours();
   polyligneP5.~TYPolyligneParcours();

   delete PointADroite;
   delete PointAGauche;
   delete IndexPointFrontiere;
   delete EstUnPointIntersectant;
   */
}

// DONE
// Testing the TYSetGeometriqueParcours::MarquePointsADroiteEtAGauche method
TEST(test_TYSetGeometriqueParcours, MarquePointsADroiteEtAGauche)
{
    /*
    // Data setup:
    TYPointParcours S;
    S.Identifiant = 0;
    S.x = 5.0;
    S.y = 4.0;
    S.isEcran = S.isInfra = true;
    TYPointParcours D;
    D.Identifiant = 5;
    D.x = 3.0;
    D.y = 7.0;
    D.isEcran = D.isInfra = true;
    TYPointParcours P1;
    P1.Identifiant = 1;
    P1.x = 2.0;
    P1.y = 2.0;
    P1.isEcran = P1.isInfra = true;
    TYPointParcours P2;
    P2.Identifiant = 2;
    P2.x = 11.0;
    P2.y = 10.0;
    P2.isEcran = P2.isInfra = true;
    TYPointParcours P3;
    P3.Identifiant = 3;
    P3.x = -4.0;
    P3.y = -1.0;
    P3.isEcran = P3.isInfra = true;
    TYPointParcours P4;
    P4.Identifiant = 4;
    P4.x = -1.0;
    P4.y = 2.0;
    P4.isEcran = P4.isInfra = true;

    // Polylignes
    TYPolyligneParcours polyligneS;
    polyligneS.ajouteSegment(&S, &D);
    TYPolyligneParcours polyligneP1;
    polyligneP1.ajouteSegment(&P2, &P3);
    TYPolyligneParcours polyligneP2;
    polyligneP2.ajouteSegment(&P1, &P4);
    TYPolyligneParcours polyligneP3;
    polyligneP3.ajouteSegment(&P2, &P4);

    // Geometrique Parcours
    TYSetGeometriqueParcours geoParcours, geoGauche, geoDroite;
    //geoParcours.AllouerPolylignes(5);
    geoParcours._nNbPointTotal = 6;
    geoParcours._ListePoint = new TYPointParcours[geoParcours._nNbPointTotal];
    geoParcours._ListePoint[0] = S;
    geoParcours._ListePoint[1] = P1;
    geoParcours._ListePoint[2] = P2;
    geoParcours._ListePoint[3] = P3;
    geoParcours._ListePoint[4] = P4;
    geoParcours._ListePoint[5] = D;
    geoParcours._nNbPolylines = 4;
    geoParcours._ListePolylines = new TYPolyligneParcours[geoParcours._nNbPolylines];
    geoParcours._ListePolylines[0].Copy(polyligneS);
    geoParcours._ListePolylines[1].Copy(polyligneP1);
    geoParcours._ListePolylines[2].Copy(polyligneP2);
    geoParcours._ListePolylines[3].Copy(polyligneP3);

    bool* PointAGauche;
    bool* PointADroite;

   // Call to the tested method:
   geoParcours.MarquePointsADroiteEtAGauche(S, D, PointAGauche, PointADroite);
   
   // Check result:
   int gauche, droite;
   gauche = droite = 0;
   for(int i = 0; i< geoParcours._nNbPointTotal; i++){
        if(PointADroite[i] == 1){
            droite++;
        }
        if(PointAGauche[i] == 1){
            gauche++;
        }
   }

   EXPECT_EQ(3, gauche);
   EXPECT_EQ(1, droite);
   EXPECT_EQ(1, PointADroite[2]);
   EXPECT_EQ(1, PointAGauche[1]);
   EXPECT_EQ(1, PointAGauche[3]);

   // Destruction

   geoDroite.~TYSetGeometriqueParcours();
   geoGauche.~TYSetGeometriqueParcours();
   geoParcours.~TYSetGeometriqueParcours();
   polyligneP1.~TYPolyligneParcours();
   polyligneP2.~TYPolyligneParcours();
   polyligneP3.~TYPolyligneParcours();
   polyligneS.~TYPolyligneParcours();

   delete PointADroite;
   delete PointAGauche;
   */
}

// DONE
// Testing the TYSetGeometriqueParcours::SeparationDroiteGauche method
TEST(test_TYSetGeometriqueParcours, SeparationDroiteGauche)
{
    /*
    // Data setup:
    // Points
    TYPointParcours S;
    S.Identifiant = 0;
    S.x = 7.0;
    S.y = 3.0;
    S.isEcran = S.isInfra = false;
    TYPointParcours D;
    D.Identifiant = 6;
    D.x = 3.0;
    D.y = 7.0;
    D.isEcran = D.isInfra = false;
    TYPointParcours P1;
    P1.Identifiant = 1;
    P1.x = 4.0;
    P1.y = 1.0;
    P1.isEcran = P1.isInfra = false;
    TYPointParcours P2;
    P2.Identifiant = 2;
    P2.x = 11.0;
    P2.y = 10.0;
    P2.isEcran = P2.isInfra = false;
    TYPointParcours P3;
    P3.Identifiant = 3;
    P3.x = 9.0;
    P3.y = 5.0;
    P3.isEcran = P3.isInfra = false;
    TYPointParcours P4;
    P4.Identifiant = 4;
    P4.x = -1.0;
    P4.y = 2.0;
    P4.isEcran = P4.isInfra = false;
    TYPointParcours P5;
    P5.Identifiant = 5;
    P5.x = 5.0;
    P5.y = 5.0;
    P5.isEcran = P5.isInfra = false;

    // Polylignes
    TYPolyligneParcours polyligneS;
    polyligneS.ajouteSegment(&S, &D);
    TYPolyligneParcours polyligneP1;
    polyligneP1.ajouteSegment(&P2, &P3);
    TYPolyligneParcours polyligneP2;
    polyligneP2.ajouteSegment(&P1, &P4);
    TYPolyligneParcours polyligneP3;
    polyligneP3.ajouteSegment(&P2, &P4);
    TYPolyligneParcours polyligneP4;
    polyligneP4.ajouteSegment(&P1, &P5);

    // Geometrique Parcours
    TYSetGeometriqueParcours geoParcours, geoGauche, geoDroite;
    //geoParcours.AllouerPolylignes(5);
    geoParcours._nNbPointTotal = 7;
    geoParcours._ListePoint = new TYPointParcours[7];
    geoParcours._ListePoint[0] = S;
    geoParcours._ListePoint[1] = P1;
    geoParcours._ListePoint[2] = P2;
    geoParcours._ListePoint[3] = P3;
    geoParcours._ListePoint[4] = P4;
    geoParcours._ListePoint[5] = P5;
    geoParcours._ListePoint[6] = D;
    geoParcours._nNbPolylines = 5;
    geoParcours._ListePolylines = new TYPolyligneParcours[5];
    geoParcours._ListePolylines[0].Copy(polyligneS);
    geoParcours._ListePolylines[1].Copy(polyligneP1);
    geoParcours._ListePolylines[2].Copy(polyligneP2);
    geoParcours._ListePolylines[3].Copy(polyligneP3);
    geoParcours._ListePolylines[4].Copy(polyligneP4);

    bool* PointAGauche;
    bool* PointADroite;

    // Call to the tested method:
    geoParcours.MarquePointsADroiteEtAGauche(S, D, PointAGauche, PointADroite);
    geoParcours.SeparationDroiteGauche(PointAGauche, PointADroite, geoGauche, geoDroite);

    // Check result:
    // DROITE
    EXPECT_EQ(2, geoDroite._nNbPolylines);
    // Pour la 1ère Polyligne Droite
    EXPECT_EQ(2, geoDroite._ListePolylines[0].indexePoint1());
    EXPECT_EQ(3, geoDroite._ListePolylines[0].indexePoint2());
    // Pour la 2ème Polyligne Droite
    EXPECT_EQ(2, geoDroite._ListePolylines[1].indexePoint1());
    EXPECT_EQ(4, geoDroite._ListePolylines[1].indexePoint2());
    // GAUCHE
    EXPECT_EQ(3, geoGauche._nNbPolylines);
    // Pour la 1ère Polyligne Gauche
    EXPECT_EQ(1, geoGauche._ListePolylines[0].indexePoint1());
    EXPECT_EQ(4, geoGauche._ListePolylines[0].indexePoint2());
    // Pour la 2ème Polyligne Gauche
    EXPECT_EQ(2, geoGauche._ListePolylines[1].indexePoint1());
    EXPECT_EQ(4, geoGauche._ListePolylines[1].indexePoint2());
    // Pour la 3ème Polyligne Gauche
    EXPECT_EQ(1, geoGauche._ListePolylines[2].indexePoint1());
    EXPECT_EQ(5, geoGauche._ListePolylines[2].indexePoint2());

    // Destruction
    geoDroite.~TYSetGeometriqueParcours();
    geoGauche.~TYSetGeometriqueParcours();
    geoParcours.~TYSetGeometriqueParcours();
    polyligneP1.~TYPolyligneParcours();
    polyligneP2.~TYPolyligneParcours();
    polyligneP3.~TYPolyligneParcours();
    polyligneP4.~TYPolyligneParcours();
    polyligneS.~TYPolyligneParcours();
    delete PointADroite;
    delete PointAGauche;
    */
}

// DONE
// Testing the TYSetGeometriqueParcours::TriePointsIntersectionSuivantSR method
TEST(test_TYSetGeometriqueParcours, TriePointsIntersectionSuivantSR)
{
    /*
    // Data setup:
    // Points
    TYPointParcours S;
    S.Identifiant = 0;
    S.x = 7.0;
    S.y = 3.0;
    S.isEcran = S.isInfra = false;
    TYPointParcours R;
    R.Identifiant = 6;
    R.x = 3.0;
    R.y = 7.0;
    R.isEcran = R.isInfra = false;
    TYPointParcours A;
    A.Identifiant = 1;
    A.x = 5.0;
    A.y = 5.0;
    A.isEcran = A.isInfra = false;
    TYPointParcours D;
    D.Identifiant = 4;
    D.x = 10.0;
    D.y = 6.0;
    D.isEcran = D.isInfra = false;
    TYPointParcours E;
    E.Identifiant = 5;
    E.x = 2.0;
    E.y = 3.0;
    E.isEcran = E.isInfra = false;

    // Polylignes
    TYPolyligneParcours polyligneSR;
    polyligneSR.ajouteSegment(&S, &R);
    TYPolyligneParcours polyDroite;
    polyDroite.ajouteSegment(&S, &D);
    TYPolyligneParcours polyGauche;
    polyGauche.ajouteSegment(&S, &E);
    TYPolyligneParcours polyligneP1;
    polyligneP1.ajouteSegment(&S, &A);
    TYPolyligneParcours polyligneP2;
    polyligneP2.ajouteSegment(&A, &R);

    // Geometrique Parcours
    TYSetGeometriqueParcours geoParcours;
    geoParcours._nNbPointTotal = 5;
    geoParcours._ListePoint = new TYPointParcours[5];
    geoParcours._ListePoint[0] = S;
    geoParcours._ListePoint[1] = A;
    geoParcours._ListePoint[2] = D;
    geoParcours._ListePoint[3] = E;
    geoParcours._ListePoint[4] = R;
    geoParcours._nNbPolylines = 5;
    geoParcours.AllouerPolylignes(5);
    geoParcours._ListePolylines[0].Copy(polyligneSR);
    geoParcours._ListePolylines[1].Copy(polyDroite);
    geoParcours._ListePolylines[2].Copy(polyGauche);
    geoParcours._ListePolylines[3].Copy(polyligneP1);
    geoParcours._ListePolylines[4].Copy(polyligneP2);

    bool* PointsAGauche;
    bool* PointsADroite;
    int* IndexePointsFrontiere = new int[geoParcours._nNbPointTotal];
    int NbPointFrontiere = 0;
    bool* EstUnPointIntersectant = new bool[geoParcours._nNbPointTotal];

    // Call to the tested method:
    geoParcours.MarquePointsADroiteEtAGauche(S, R, PointsADroite, PointsAGauche);
    geoParcours.RamenerPointsTraversantLaFrontiere(S, D, IndexePointsFrontiere, NbPointFrontiere, EstUnPointIntersectant, true, PointsAGauche, PointsADroite);

    // Check result:
    EXPECT_EQ(1, NbPointFrontiere);

    // Destruction
    geoParcours.~TYSetGeometriqueParcours();
    polyDroite.~TYPolyligneParcours();
    polyGauche.~TYPolyligneParcours();
    polyligneP1.~TYPolyligneParcours();
    polyligneP2.~TYPolyligneParcours();
    polyligneSR.~TYPolyligneParcours();

    delete PointsADroite;
    delete PointsAGauche;
    delete IndexePointsFrontiere;
    delete EstUnPointIntersectant;
    */
}

// DONE
// Testing the TYSetGeometriqueParcours::AjoutePointALaPolyLigne method
TEST(test_TYSetGeometriqueParcours, AjoutePointALaPolyLigne)
{
    /*
    // Data setup:
    // Points
    TYPointParcours P0;
    P0.Identifiant = 0;
    P0.isInfra = true;
    P0.isEcran = true;

    TYPointParcours P1;
    P1.Identifiant = 1;
    P1.isEcran = false;
    P1.isInfra = true;

    TYPointParcours P2;
    P2.Identifiant = 2;
    P2.isEcran = true;
    P2.isInfra = true;

    // Polylignes
    TYPolyligneParcours polyligneP0;
    polyligneP0.ajouteSegment(&P0, &P2);

    // Geometrique Parcours
    TYSetGeometriqueParcours geoParcours;
    geoParcours._nNbPolylines = 1;
    geoParcours._ListePolylines = new TYPolyligneParcours[1];
    geoParcours._ListePolylines[0].Copy(polyligneP0);
    geoParcours._nNbPointTotal = 2;
    geoParcours._ListePoint = new TYPointParcours[100];
    geoParcours._ListePoint[0] = P0;
    geoParcours._ListePoint[1] = P2;

    // Call to the tested method:
    bool t_ajout = geoParcours.AjoutePointALaPolyLigne(0, P1);

    // Check result:
    EXPECT_EQ(true, t_ajout);
    EXPECT_EQ(3, geoParcours._nNbPointTotal);
    EXPECT_EQ(1, geoParcours._ListePolylines[0].indexePoint(2));

    // Destruction
    geoParcours.~TYSetGeometriqueParcours();
    polyligneP0.~TYPolyligneParcours();
    */

}

// TO TEST
// Testing the TYSetGeometriqueParcours::AjouteLesPointsComprisEntre method
TEST(test_TYSetGeometriqueParcours, AjouteLesPointsComprisEntre)
{
    // Data setup:
    TYPointParcours S;
    S.Identifiant = 0;
    S.x = 1.0;
    S.y = 1.0;
    S.isEcran = S.isInfra = false;
    TYPointParcours R;
    R.Identifiant = 1;
    R.x = 5.0;
    R.y = 5.0;
    R.isEcran = R.isInfra = false;
    TYPointParcours P1;
    P1.Identifiant = 2;
    P1.x = 2.0;
    P1.y = 2.0;
    P1.isEcran = P1.isInfra = false;
    TYPointParcours P2;
    P2.Identifiant = 3;
    P2.x = 3.0;
    P2.y = 3.0;
    P2.isEcran = P2.isInfra = false;
    //std::cout << " Initialisation des points fait" << std::endl;

    // Polylignes
    TYPolyligneParcours polyligneSR;
    polyligneSR.ajouteSegment(&S, &R);
    polyligneSR.ajoutePoint(2, &P1);
    //polyligneSR.ajoutePoint(3, &P2);
    TYPolyligneParcours polyligneAjout;
    polyligneAjout.ajouteSegment(&S, &R);
    //std::cout << " Initialisation des polylignes fait" << std::endl;

    // Geometrique Parcours
    TYSetGeometriqueParcours geoParcours;
    geoParcours._nNbPointTotal = 4;
    geoParcours._nNbPolylines = 1;
    geoParcours._ListePoint = new TYPointParcours[4];
    geoParcours._ListePoint[0] = S;
    geoParcours._ListePoint[1] = R;
    geoParcours._ListePoint[2] = P1;
    geoParcours._ListePoint[3] = P2;
    geoParcours._ListePolylines = new TYPolyligneParcours[2];
    geoParcours._ListePolylines[0].Copy(polyligneSR);
    //std::cout << " Initialisation des parcours geometriques fait" << std::endl;

    TYSetGeometriqueParcours geoAjoute;
    geoAjoute._nNbPointTotal = 3;
    geoAjoute._ListePoint = new TYPointParcours[3];
    geoAjoute._nNbPolylines = 2;
    geoAjoute._ListePolylines = new TYPolyligneParcours[2];
    geoAjoute._ListePolylines[0].Copy(polyligneAjout);

    //std::cout << std::endl;
    // Call to the tested method:_nNbPointTotal
    //int PointsAjoutes = geoAjoute.AjouteLesPointsComprisEntre(geoParcours, 0, 2, 3);

    // Check result:
    //EXPECT_EQ(1, PointsAjoutes);

}

// TO TEST
// Testing the TYSetGeometriqueParcours::ParcourtPolyligneAPartirDe method
TEST(test_TYSetGeometriqueParcours, ParcourtPolyligneAPartirDe)
{
    // Data setup:
    // Points
    TYPointParcours S;
    S.Identifiant = 0;
    S.x = 0.0;
    S.y = 0.0;
    S.isEcran = S.isInfra = false;
    TYPointParcours D;
    D.Identifiant = 1;
    D.x = 10.0;
    D.y = 10.0;
    D.isEcran = D.isInfra = false;
    TYPointParcours P1;
    P1.Identifiant = 2;
    P1.x = 2.0;
    P1.y = 4.0;
    P1.isEcran = P1.isInfra = false;
    TYPointParcours P2;
    P2.Identifiant = 3;
    P2.x = 8.0;
    P2.y = 6.0;
    P2.isEcran = P2.isInfra = false;

    // Polylignes
    TYPolyligneParcours polyligneSR;
    polyligneSR.ajouteSegment(&S, &D);
    TYPolyligneParcours polyligneP1;
    polyligneP1.ajouteSegment(&S, &P1);
    TYPolyligneParcours polyligneP2;
    polyligneP2.ajouteSegment(&P1, &P2);
    TYPolyligneParcours polyligneP3;
    polyligneP3.ajouteSegment(&P2, &D);

    polyligneP1._PolyligneP1 = &polyligneP2;
    polyligneP2._PolyligneP0 = &polyligneP1;
    polyligneP2._PolyligneP1 = &polyligneP3;
    polyligneP3._PolyligneP0 = &polyligneP2;

    // Geometrique Parcours
    TYSetGeometriqueParcours geoParcours;
    geoParcours._nNbPointTotal = 4;
    geoParcours._nNbPolylines = 4;
    geoParcours._ListePoint = new TYPointParcours[geoParcours._nNbPointTotal];
    geoParcours._ListePoint[0] = S;
    geoParcours._ListePoint[1] = D;
    geoParcours._ListePoint[2] = P1;
    geoParcours._ListePoint[3] = P2;
    geoParcours._ListePolylines = new TYPolyligneParcours[geoParcours._nNbPolylines];
    geoParcours._ListePolylines[0].Copy(polyligneSR);
    geoParcours._ListePolylines[1].Copy(polyligneP1);
    geoParcours._ListePolylines[2].Copy(polyligneP2);
    geoParcours._ListePolylines[3].Copy(polyligneP3);

    TYSetGeometriqueParcours geoPremierePasse;
    geoPremierePasse._nNbPointTotal = 4;
    geoPremierePasse._nNbPolylines = 4;
    geoPremierePasse.AllouerPolylignes(4);
    geoPremierePasse._ListePoint = new TYPointParcours[4];
    geoPremierePasse._ListePolylines[0].Copy(polyligneSR);

    bool* PointsAGauche;
    bool* PointsADroite;
    int* IndexePointsFrontiere = new int[geoParcours._nNbPointTotal];
    int NbPointFrontiere = 0;
    bool* EstUnPointIntersectant = new bool[geoParcours._nNbPointTotal];

    geoParcours.MarquePointsADroiteEtAGauche(S, D, PointsAGauche, PointsADroite);
    geoParcours.RamenerPointsTraversantLaFrontiere(S, D, IndexePointsFrontiere, NbPointFrontiere, EstUnPointIntersectant, true, PointsAGauche, PointsADroite);

    // Call to the tested method:
    std::cout << " Nombre de point total de GeoParcours : " << geoParcours._nNbPointTotal << std::endl;
    int IndexPoint = geoParcours.ParcourtPolyligneAPartirDe(0, geoParcours._ListePolylines, EstUnPointIntersectant, geoPremierePasse);
    //std::cout << "Fini : " << IndexPoint << std::endl;
    int i;
    for (i=0; i<geoPremierePasse._nNbPointTotal; i++){
        std::cout << "Identifiant du point " << i << " : " << geoPremierePasse._ListePoint[i].Identifiant << std::endl;
    }

    // Check result:
    // on doit arriver au dernier point avant la destination
    EXPECT_EQ(3, IndexPoint);
    EXPECT_EQ(2 , geoPremierePasse._nNbPointTotal);

    // Destruction
    geoParcours.~TYSetGeometriqueParcours();
    polyligneP1.~TYPolyligneParcours();
    polyligneP2.~TYPolyligneParcours();
    polyligneP3.~TYPolyligneParcours();

    delete PointsADroite;
    delete PointsAGauche;
    delete IndexePointsFrontiere;
    delete EstUnPointIntersectant;
}

// DONE
// Testing the TYSetGeometriqueParcours::PolylignesInfraFermees method
TEST(test_TYSetGeometriqueParcours, PolylignesInfraFermees)
{
    /*
    // Data setup:
    // Points
    TYPointParcours P0;
    P0.Identifiant = 0;
    P0.isInfra = true;
    P0.isEcran = false;
    TYPointParcours P1;
    P1.Identifiant = 1;
    P1.isInfra = true;
    P1.isEcran = false;
    TYPointParcours P2;
    P2.Identifiant = 2;
    P2.isInfra = true;
    P2.isEcran = false;

    // Polylignes
    TYPolyligneParcours polyligneP0, polyligneP1, polyligneP2;
    polyligneP0.ajouteSegment(&P0, &P1);
    polyligneP1.ajouteSegment(&P1, &P2);
    polyligneP2.ajouteSegment(&P2, &P2);
    polyligneP0._PolyligneP1 = &polyligneP1;
    polyligneP1._PolyligneP0 = &polyligneP0;
    polyligneP1._PolyligneP1 = &polyligneP2;
    polyligneP2._PolyligneP0 = &polyligneP2;
    polyligneP2._PolyligneP1 = &polyligneP2;

    // Geometrique Parcours
    TYSetGeometriqueParcours geoParcoursP0, geoParcoursP1;
    geoParcoursP0._nNbPolylines = 3;
    geoParcoursP0._ListePolylines = new TYPolyligneParcours[3];
    geoParcoursP0._ListePolylines[0].Copy(polyligneP0);
    geoParcoursP0._ListePolylines[1].Copy(polyligneP1);
    geoParcoursP0._ListePolylines[2].Copy(polyligneP2);
    geoParcoursP1._nNbPolylines = 2;
    geoParcoursP1._ListePolylines = new TYPolyligneParcours[2];
    geoParcoursP1._ListePolylines[0] = polyligneP0;
    geoParcoursP1._ListePolylines[1] = polyligneP1;

    // Call to the tested method:
    bool t_closed = geoParcoursP0.PolylignesInfraFermees();
    bool t_open = geoParcoursP1.PolylignesInfraFermees();

    // Check result:
    EXPECT_EQ(false, t_closed);
    EXPECT_EQ(false, t_open);

    // Destruction
    geoParcoursP0.~TYSetGeometriqueParcours();
    geoParcoursP1.~TYSetGeometriqueParcours();
    polyligneP0.~TYPolyligneParcours();
    polyligneP1.~TYPolyligneParcours();
    polyligneP2.~TYPolyligneParcours();
    */

}

// DONE
// Testing the TYSetGeometriqueParcours::ListerPointsConnexes method
TEST(test_TYSetGeometriqueParcours, ListerPointsConnexes)
{
    /*
    // Data setup:
    // Points
    TYPointParcours S;
    S.Identifiant = 0;
    S.x = 1.0;
    S.y = 1.0;
    S.isEcran = S.isInfra = true;
    TYPointParcours D;
    D.Identifiant = 1;
    D.x = 16.0;
    D.y = 2.0;
    D.isEcran = D.isInfra = true;
    TYPointParcours P1;
    P1.Identifiant = 2;
    P1.x = 5.0;
    P1.y = 4.0;
    P1.isEcran = P1.isInfra = true;
    TYPointParcours P2;
    P2.Identifiant = 3;
    P2.x = 10.0;
    P2.y = 5.0;
    P2.isEcran = P2.isInfra = true;
    TYPointParcours P3;
    P3.Identifiant = 4;
    P3.x = 4.0;
    P3.y = 9.0;
    P3.isEcran = P3.isInfra = true;
    TYPointParcours P4;
    P4.Identifiant = 5;
    P4.x = 8.0;
    P4.y = 11.0;
    P4.isEcran = P4.isInfra = true;
    TYPointParcours P5;
    P5.Identifiant = 6;
    P5.x = 14.0;
    P5.y = 7.0;
    P5.isEcran = P5.isInfra = true;

    TYPolyligneParcours polyligneP1;
    polyligneP1.ajouteSegment(&S, &P1);
    TYPolyligneParcours polyligneP2;
    polyligneP2.ajouteSegment(&P1, &P2);
    TYPolyligneParcours polyligneP3;
    polyligneP3.ajouteSegment(&P1, &P3);
    TYPolyligneParcours polyligneP4;
    polyligneP4.ajouteSegment(&P1, &P4);
    TYPolyligneParcours polyligneP5;
    polyligneP5.ajouteSegment(&P2, &P5);
    TYPolyligneParcours polyligneP6;
    polyligneP6.ajouteSegment(&P2, &D);

    // Geometrique Parcours
    TYSetGeometriqueParcours geoParcours;
    geoParcours._nNbPointTotal = 8;
    geoParcours._ListePoint = new TYPointParcours[geoParcours._nNbPointTotal];
    geoParcours._ListePoint[0] = S;
    geoParcours._ListePoint[1] = D;
    geoParcours._ListePoint[2] = P1;
    geoParcours._ListePoint[3] = P2;
    geoParcours._ListePoint[4] = P3;
    geoParcours._ListePoint[5] = P4;
    geoParcours._ListePoint[6] = P5;
    geoParcours._nNbPolylines = 6;
    geoParcours._ListePolylines = new TYPolyligneParcours[geoParcours._nNbPolylines];
    geoParcours._ListePolylines[0].Copy(polyligneP1);
    geoParcours._ListePolylines[1].Copy(polyligneP2);
    geoParcours._ListePolylines[2].Copy(polyligneP3);
    geoParcours._ListePolylines[3].Copy(polyligneP4);
    geoParcours._ListePolylines[4].Copy(polyligneP5);
    geoParcours._ListePolylines[5].Copy(polyligneP6);

    Connexite* Connexes = new Connexite[geoParcours._nNbPointTotal];

    // Call to the tested method:
    bool t_connexe = geoParcours.ListerPointsConnexes(Connexes);

    // Check result:
    EXPECT_EQ(true, t_connexe);
    EXPECT_EQ(1, Connexes[0].NbSegmentsConnexes);
    EXPECT_EQ(1, Connexes[1].NbSegmentsConnexes);
    EXPECT_EQ(2, Connexes[2].NbSegmentsConnexes);
    EXPECT_EQ(1, Connexes[3].NbSegmentsConnexes);
    EXPECT_EQ(1, Connexes[4].NbSegmentsConnexes);
    EXPECT_EQ(1, Connexes[5].NbSegmentsConnexes);
    EXPECT_EQ(1, Connexes[6].NbSegmentsConnexes);

    // Destruction
    geoParcours.~TYSetGeometriqueParcours();
    polyligneP1.~TYPolyligneParcours();
    polyligneP2.~TYPolyligneParcours();
    polyligneP3.~TYPolyligneParcours();
    polyligneP4.~TYPolyligneParcours();
    polyligneP5.~TYPolyligneParcours();
    polyligneP6.~TYPolyligneParcours();
    delete Connexes;
    */
}

// Methode Privee
// Testing the TYSetGeometriqueParcours::InverseOrdreDesPoints method
TEST(test_TYSetGeometriqueParcours, InverseOrdreDesPoints)
{
   // Data setup:
   // ToDo

   // Call to the tested method:
    //TYSetGeometriqueParcours::InverseOrdreDesPoints(result, ...);
   
   // Check result:
   //EXPECT_EQ(result, expected_result);
}

// Methode Privee
// Testing the TYSetGeometriqueParcours::SecondVecteurADroiteDuPremier method
TEST(test_TYSetGeometriqueParcours, SecondVecteurADroiteDuPremier)
{
   // Data setup:
   // ToDo

   // Call to the tested method:
   //TYSetGeometriqueParcours::SecondVecteurADroiteDuPremier(result, ...);
   
   // Check result:
   //EXPECT_EQ(result, expected_result);
}

// Methode Privee
// Testing the TYSetGeometriqueParcours::SecondVecteurAGaucheDuPremier method
TEST(test_TYSetGeometriqueParcours, SecondVecteurAGaucheDuPremier)
{
   // Data setup:
   // ToDo

   // Call to the tested method:
   //TYSetGeometriqueParcours::SecondVecteurAGaucheDuPremier(result, ...);
   
   // Check result:
   //EXPECT_EQ(result, expected_result);
}

// DONE
// Testing the TYSetGeometriqueParcours::intersects method
TEST(test_TYSetGeometriqueParcours, intersects)
{
    /*
    // Data setup:
    TYPointParcours S;
    S.Identifiant = 0;
    S.x = 1.0;
    S.y = 1.0;
    S.isEcran = S.isInfra = false;
    TYPointParcours A;
    A.Identifiant = 1;
    A.x = 5.0;
    A.y = 6.0;
    A.isEcran = A.isInfra = false;
    TYPointParcours B;
    B.Identifiant = 2;
    B.x = 4.0;
    B.y = 6.0;
    B.isEcran = B.isInfra = false;
    TYPointParcours C;
    C.Identifiant = 3;
    C.x = 3.0;
    C.y = 1.0;
    C.isEcran = C.isInfra = false;
    TYPointParcours R;
    R.Identifiant = 4;
    R.x = 6.0;
    R.y = 2.0;
    R.isEcran = R.isInfra = false;

    TYPolyligneParcours polyligneSR;
    polyligneSR.ajouteSegment(&S, &R);
    TYPolyligneParcours polyligneP1;
    polyligneP1.ajouteSegment(&A, &B);
    TYPolyligneParcours polyligneP2;
    polyligneP2.ajouteSegment(&B, &C);

    polyligneP1._PolyligneP0 = NULL;
    polyligneP1._PolyligneP1 = &polyligneP2;
    polyligneP2._PolyligneP0 = &polyligneP1;
    polyligneP2._PolyligneP1 = NULL;

    TYSetGeometriqueParcours geoParcours;
    geoParcours._nNbPointTotal = 5;
    geoParcours._ListePoint = new TYPointParcours[geoParcours._nNbPointTotal];
    geoParcours._ListePoint[0] = S;
    geoParcours._ListePoint[1] = A;
    geoParcours._ListePoint[2] = B;
    geoParcours._ListePoint[3] = C;
    geoParcours._ListePoint[4] = R;
    geoParcours._nNbPolylines = 3;
    geoParcours.AllouerPolylignes(3);
    geoParcours._ListePolylines = new TYPolyligneParcours[3];
    geoParcours._ListePolylines[0].Copy(polyligneSR);
    geoParcours._ListePolylines[1].Copy(polyligneP1);
    geoParcours._ListePolylines[2].Copy(polyligneP2);

    // Call to the tested method:
    bool t_isNotIntersect = geoParcours.intersects(A, B);
    bool t_isIntersect = geoParcours.intersects(B, C);

    // Check result
    EXPECT_EQ(false, t_isNotIntersect);
    EXPECT_EQ(true, t_isIntersect);

    // Destruction
    geoParcours.~TYSetGeometriqueParcours();
    */
}

// DONE
// Testing the TYSetGeometriqueParcours::AppartienneMemePolyligne method
TEST(test_TYSetGeometriqueParcours, AppartienneMemePolyligne)
{
    /*
    // Data setup:
    // Points
    TYPointParcours P0;
    P0.Identifiant = 0;
    P0.x = 7.0;
    P0.y = 3.0;
    P0.isInfra = P0.isEcran = false;
    TYPointParcours P1;
    P1.Identifiant = 1;
    P1.x = 3.0;
    P1.y = 7.0;
    P1.isEcran = P1.isInfra = false;
    TYPointParcours P2;
    P2.Identifiant = 2;
    P2.x = 5.0;
    P2.y = 5.0;
    P2.isEcran = P2.isInfra = false;
    TYPointParcours P3;
    P3.Identifiant = 3;
    P3.x = 9.0;
    P3.y = 5.0;
    P3.isEcran = P3.isInfra = false;

    // Polylignes
    TYPolyligneParcours polyligneP0;
    polyligneP0.ajouteSegment(&P0, &P1);
    polyligneP0.ajoutePoint(2, &P2);
    TYPolyligneParcours polyligneP1;
    polyligneP1.ajouteSegment(&P0, &P3);

    // Geometrique parcours
    TYSetGeometriqueParcours geoParcours;
    geoParcours._nNbPointTotal = 4;
    geoParcours._ListePoint = new TYPointParcours[4];
    geoParcours._ListePoint[0] = P0;
    geoParcours._ListePoint[1] = P1;
    geoParcours._ListePoint[2] = P2;
    geoParcours._ListePoint[3] = P3;
    geoParcours._nNbPolylines = 2;
    geoParcours._ListePolylines = new TYPolyligneParcours[2];
    geoParcours._ListePolylines[0].Copy(polyligneP0);
    geoParcours._ListePolylines[1].Copy(polyligneP1);

    // Call to the tested method:
    bool t_samePolyline = geoParcours.AppartienneMemePolyligne(&P0, &P1, &P2);
    bool t_noteSamePolyline = geoParcours.AppartienneMemePolyligne(&P0, &P1, &P3);

    // Check result:
    EXPECT_EQ(true, t_samePolyline);
    EXPECT_EQ(false, t_noteSamePolyline);

    // Destruction
    geoParcours.~TYSetGeometriqueParcours();
    */
}

// DONE
// Testing the TYSetGeometriqueParcours::SelectionnePointsEntreSetRetDuCoteDeSR method
TEST(test_TYSetGeometriqueParcours, SelectionnePointsEntreSetRetDuCoteDeSR)
{
    /*
    // Data setup:
    // Points
    TYPointParcours S;
    S.Identifiant = 0;
    S.x = 0.0;
    S.y = 0.0;
    S.isEcran = S.isInfra = false;
    TYPointParcours D;
    D.Identifiant = 1;
    D.x = 10.0;
    D.y = 10.0;
    D.isEcran = D.isInfra = false;
    TYPointParcours P1;
    P1.Identifiant = 2;
    P1.x = 0.0;
    P1.y = 5.0;
    P1.isEcran = P1.isInfra = false;
    TYPointParcours P2;
    P2.Identifiant = 3;
    P2.x = 5.0;
    P2.y = 5.0;
    P2.isEcran = P2.isInfra = false;
    TYPointParcours P3;
    P3.Identifiant = 4;
    P3.x = 10.0;
    P3.y = 5.0;
    P3.isEcran = P3.isInfra = false;
    TYPointParcours P4;
    P4.Identifiant = 5;
    P4.x = 5.0;
    P4.y = 10.0;
    P4.isEcran = P4.isInfra = false;

    // Polylignes
    // Source
    TYPolyligneParcours polyligneSD;
    polyligneSD.ajouteSegment(&S, &D);
    // Indirectes
    TYPolyligneParcours polyligneP1;
    polyligneP1.ajouteSegment(&S, &P1);
    TYPolyligneParcours polyligneP2;
    polyligneP2.ajouteSegment(&P1, &P2);
    TYPolyligneParcours polyligneP3;
    polyligneP3.ajouteSegment(&P2, &P3);
    TYPolyligneParcours polyligneP4;
    polyligneP4.ajouteSegment(&P3, &P4);
    TYPolyligneParcours polyligneP5;
    polyligneP5.ajouteSegment(&P4, &D);
    // Directes
    TYPolyligneParcours polyligneD1;
    polyligneD1.ajouteSegment(&S, &P1);
    TYPolyligneParcours polyligneD2;
    polyligneD2.ajouteSegment(&P1, &P4);
    TYPolyligneParcours polyligneD3;
    polyligneD3.ajouteSegment(&P4, &D);

    // Geometrique Parcours
    // Parcours Source
    TYSetGeometriqueParcours* geoSR = new TYSetGeometriqueParcours;
    geoSR->_nNbPointTotal = 2;
    geoSR->_nNbPolylines = 1;
    geoSR->_ListePoint = new TYPointParcours[2];
    geoSR->_ListePoint[0] = S;
    geoSR->_ListePoint[1] = D;
    geoSR->_ListePolylines = new TYPolyligneParcours;
    geoSR->_ListePolylines[0].Copy(polyligneSD);
    // Parcours Direct
    TYSetGeometriqueParcours* geoDirect = new TYSetGeometriqueParcours;
    geoDirect->_nNbPolylines = 3;
    geoDirect->_nNbPointTotal = 4;
    geoDirect->_ListePoint = new TYPointParcours[geoDirect->_nNbPointTotal];
    geoDirect->_ListePoint[0] = S;
    geoDirect->_ListePoint[1] = D;
    geoDirect->_ListePoint[2] = P1;
    geoDirect->_ListePoint[3] = P4;
    geoDirect->_ListePolylines = new TYPolyligneParcours[geoDirect->_nNbPolylines];
    geoDirect->_ListePolylines[0].Copy(polyligneD1);
    geoDirect->_ListePolylines[1].Copy(polyligneD2);
    geoDirect->_ListePolylines[2].Copy(polyligneD3);
    //Parcours Indirect
    TYSetGeometriqueParcours* geoParcours = new TYSetGeometriqueParcours;
    geoParcours->_nNbPointTotal = 6;
    geoParcours->_nNbPolylines = 5;
    geoParcours->_ListePoint = new TYPointParcours[geoParcours->_nNbPointTotal];
    geoParcours->_ListePoint[0] = S;
    geoParcours->_ListePoint[1] = D;
    geoParcours->_ListePoint[2] = P1;
    geoParcours->_ListePoint[3] = P2;
    geoParcours->_ListePoint[4] = P3;
    geoParcours->_ListePoint[5] = P4;
    geoParcours->_ListePolylines = new TYPolyligneParcours[geoParcours->_nNbPolylines];
    geoParcours->_ListePolylines[0].Copy(polyligneP1);
    geoParcours->_ListePolylines[1].Copy(polyligneP2);
    geoParcours->_ListePolylines[2].Copy(polyligneP3);
    geoParcours->_ListePolylines[3].Copy(polyligneP4);
    geoParcours->_ListePolylines[4].Copy(polyligneP5);

    // Pour Trajet Indirect
    bool* PointAGauche;
    bool* PointADroite;
    int* IndexPointFrontiere = new int[100];
    int NbPointFrontiere = 0;
    bool* EstUnPointIntersectant = new bool[100];
    bool bCoteGauche = true;

    geoParcours->MarquePointsADroiteEtAGauche(S, D, PointAGauche, PointADroite);
    geoParcours->RamenerPointsTraversantLaFrontiere(S, D, IndexPointFrontiere, NbPointFrontiere, EstUnPointIntersectant, bCoteGauche, PointAGauche, PointADroite);
    geoParcours->TriePointsIntersectionSuivantSR(S, D, IndexPointFrontiere, NbPointFrontiere);

    TYPointParcours** TableauDePoints = new TYPointParcours*[geoParcours->_nNbPointTotal + 2];

    // Pour Trajet Direct
    bool* PointAGaucheD;
    bool* PointADroiteD;
    int* IndexPointFrontiereD = new int[100];
    int NbPointFrontiereD = 0;
    bool* EstUnPointIntersectantD = new bool[100];
    bool bCoteGaucheD = false;

    geoDirect->MarquePointsADroiteEtAGauche(S, D, PointAGaucheD, PointADroiteD);
    geoDirect->RamenerPointsTraversantLaFrontiere(S, D, IndexPointFrontiereD, NbPointFrontiereD, EstUnPointIntersectantD, bCoteGaucheD, PointAGaucheD, PointADroiteD);
    geoDirect->TriePointsIntersectionSuivantSR(S, D, IndexPointFrontiereD, NbPointFrontiereD);

    TYPointParcours** TableauDePointsNull = new TYPointParcours*[(geoDirect->_nNbPointTotal + 2)];

    // Call to the tested method:
    int t_direct = geoDirect->SelectionnePointsEntreSetRetDuCoteDeSR(geoSR, TableauDePointsNull, geoDirect->_nNbPointTotal);
    int t_indirect = geoParcours->SelectionnePointsEntreSetRetDuCoteDeSR(geoSR, TableauDePoints, geoParcours->_nNbPointTotal);

    // Check result:
    EXPECT_EQ(2, t_direct);
    EXPECT_EQ(7, t_indirect);
    EXPECT_EQ(0, TableauDePointsNull[0]->Identifiant);
    EXPECT_EQ(1, TableauDePointsNull[1]->Identifiant);
    EXPECT_EQ(0, TableauDePoints[2]->Identifiant);
    EXPECT_EQ(1, TableauDePoints[3]->Identifiant);
    EXPECT_EQ(2, TableauDePoints[4]->Identifiant);
    EXPECT_EQ(3, TableauDePoints[5]->Identifiant);
    EXPECT_EQ(5, TableauDePoints[6]->Identifiant);

    // Destruction
    geoParcours->~TYSetGeometriqueParcours();
    geoDirect->~TYSetGeometriqueParcours();
    delete PointADroite;
    delete PointAGauche;
    delete IndexPointFrontiere;
    delete EstUnPointIntersectant;
    */
}

// DONE
// Testing the TYSetGeometriqueParcours::CreerTrajetAPartirDuneListeDePointsTriee method
TEST(test_TYSetGeometriqueParcours, CreerTrajetAPartirDuneListeDePointsTriee)
{
    /*
    // Data setup:
    // Points
    TYPointParcours S;
    S.Identifiant = 0;
    S.x = 1.0;
    S.y = 1.0;
    S.isEcran = S.isInfra = false;
    TYPointParcours D;
    D.Identifiant = 3;
    D.x = 16.0;
    D.y = 2.0;
    D.isEcran = D.isInfra = false;
    TYPointParcours P1;
    P1.Identifiant = 1;
    P1.x = 5.0;
    P1.y = 4.0;
    P1.isEcran = P1.isInfra = false;
    TYPointParcours P2;
    P2.Identifiant = 2;
    P2.x = 10.0;
    P2.y = 5.0;
    P2.isEcran = P2.isInfra = false;

    // Geometrique Parcours
    TYSetGeometriqueParcours geoParcoursD;
    geoParcoursD._nNbPointTotal = 0;
    geoParcoursD._nNbPolylines = 0;
    geoParcoursD._ListePoint = NULL;
    geoParcoursD._ListePolylines = NULL;
    TYSetGeometriqueParcours geoParcoursG;
    geoParcoursG._nNbPointTotal = 0;
    geoParcoursG._nNbPolylines = 0;
    geoParcoursG._ListePoint = NULL;
    geoParcoursG._ListePolylines = NULL;

    TYPointParcours* TableauDePoints[4];
    TableauDePoints[0] = &S;
    TableauDePoints[1] = &P1;
    TableauDePoints[2] = &P2;
    TableauDePoints[3] = &D;

    int NbPoints = 4;
    bool GardeIdentifiant = 1;

    // Call to the tested method:
    geoParcoursD.CreerTrajetAPartirDuneListeDePointsTriee(TableauDePoints, NbPoints, 1, GardeIdentifiant);
    geoParcoursG.CreerTrajetAPartirDuneListeDePointsTriee(TableauDePoints, NbPoints, 0, GardeIdentifiant);

    // Check result:
    EXPECT_EQ(4, geoParcoursD._nNbPointTotal);
    EXPECT_EQ(1, geoParcoursD._nNbPolylines);
    EXPECT_EQ(0, geoParcoursD._ListePoint[0].Identifiant);
    EXPECT_EQ(4, geoParcoursG._nNbPointTotal);
    EXPECT_EQ(1, geoParcoursG._nNbPolylines);
    EXPECT_EQ(3, geoParcoursG._ListePoint[0].Identifiant);

    // Destruction
    geoParcoursD.~TYSetGeometriqueParcours();
    geoParcoursG.~TYSetGeometriqueParcours();
    delete TableauDePoints;
    */
}

// TO TEST
// Testing the TYSetGeometriqueParcours::EnveloppeConvexeLes2PremiersPointsEtant method
TEST(test_TYSetGeometriqueParcours, EnveloppeConvexeLes2PremiersPointsEtant)
{
    // Data setup:
    // Points
    TYPointParcours P1;
    P1.Identifiant = 0;
    P1.x = 2.0;
    P1.y = 2.0;
    P1.isEcran = P1.isInfra = false;
    TYPointParcours P2;
    P2.Identifiant = 1;
    P2.x = 10.0;
    P2.y = 12.0;
    P2.isEcran = P2.isInfra = false;
    TYPointParcours P3;
    P3.Identifiant = 2;
    P3.x = 11.0;
    P3.y = 15.0;
    P3.isEcran = P3.isInfra = false;
    TYPointParcours P4;
    P4.Identifiant = 3;
    P4.x = 19.0;
    P4.y = 4.0;
    P4.isEcran = P4.isInfra = false;
    TYPointParcours P5;
    P5.Identifiant = 4;
    P5.x = 14.0;
    P5.y = 9.0;
    P5.isEcran = P5.isInfra = false;
    TYPointParcours P6;
    P6.Identifiant = 5;
    P6.x = 7.0;
    P6.y = 6.0;
    P6.isEcran = P6.isInfra = false;

    // Geometrique Parcours
    TYSetGeometriqueParcours geoParcours;
    geoParcours._nNbPointTotal = 6;
    geoParcours._ListePoint = new TYPointParcours[6];
    geoParcours._ListePoint[0] = P1;
    geoParcours._ListePoint[1] = P3;
    geoParcours._ListePoint[2] = P2;
    geoParcours._ListePoint[3] = P4;
    geoParcours._ListePoint[4] = P5;
    geoParcours._ListePoint[5] = P6;

    // Parametres
    //TYPointParcours* TableauDePointsECOut = new TYPointParcours[6];
    //bool bPremiesPointsLesPlusHauts = false;

    // Call to the tested method:
    //int NbPointEC = TYSetGeometriqueParcours::EnveloppeConvexeLes2PremiersPointsEtant(&geoParcours._ListePoint, 6, &TableauDePointsECOut, false);

    // Check result:
    //EXPECT_EQ(NbPointEC, 3);

    // Destruction
    geoParcours.~TYSetGeometriqueParcours();
}

// TO TEST
// Testing the TYSetGeometriqueParcours::PremierePasse method
TEST(test_TYSetGeometriqueParcours, PremierePasse)
{
    // Data setup:
    // Points
    TYPointParcours S;
    S.Identifiant = 0;
    S.x = 1.0;
    S.y = 1.0;
    S.isEcran = S.isInfra = false;
    TYPointParcours D;
    D.Identifiant = 7;
    D.x = 17.0;
    D.y = 12.0;
    D.isEcran = D.isInfra = false;
    TYPointParcours P1;
    P1.Identifiant = 1;
    P1.x = 4.0;
    P1.y = 7.0;
    P1.isEcran = P1.isInfra = false;
    TYPointParcours P2;
    P2.Identifiant = 2;
    P2.x = 7.0;
    P2.y = 1.0;
    P2.isEcran = P2.isInfra = false;
    TYPointParcours P3;
    P3.Identifiant = 3;
    P3.x = 12.0;
    P3.y = 7.0;
    P3.isEcran = P3.isInfra = false;
    TYPointParcours P4;
    P4.Identifiant = 4;
    P4.x = 9.0;
    P4.y = 9.0;
    P4.isEcran = P4.isInfra = false;
    TYPointParcours P5;
    P5.Identifiant = 5;
    P5.x = 10.0;
    P5.y = 14.0;
    P5.isEcran = P5.isInfra = false;
    TYPointParcours P6;
    P6.Identifiant = 6;
    P6.x = 14.0;
    P6.y = 10.0;
    P6.isEcran = P6.isInfra = false;

/*
    // Polylignes
    TYPolyligneParcours polyligneSD;
    polyligneSD.ajouteSegment(&S, &D);
    polyligneSD.ajoutePoint(2, &P1);
    polyligneSD.ajoutePoint(3, &P2);
    polyligneSD.ajoutePoint(4, &P3);
    polyligneSD.ajoutePoint(5, &P4);
    polyligneSD.ajoutePoint(6, &P5);
    polyligneSD.ajoutePoint(7, &P6);

    TYPolyligneParcours polyligneP1;
    polyligneP1.ajouteSegment(&S, &P1);
    TYPolyligneParcours polyligneP2;
    polyligneP2.ajouteSegment(&P1, &P2);
    TYPolyligneParcours polyligneP3;
    polyligneP3.ajouteSegment(&P2, &P3);
    TYPolyligneParcours polyligneP4;
    polyligneP4.ajouteSegment(&P1, &P4);
    TYPolyligneParcours polyligneP5;
    polyligneP5.ajouteSegment(&P4, &P5);
    TYPolyligneParcours polyligneP6;
    polyligneP6.ajouteSegment(&P4, &P6);
    TYPolyligneParcours polyligneP7;
    polyligneP7.ajouteSegment(&P6, &D);

    polyligneP1._PolyligneP1 = &polyligneP4;
    polyligneP2._PolyligneP0 = &polyligneP1;
    polyligneP2._PolyligneP1 = &polyligneP3;
    polyligneP3._PolyligneP0 = &polyligneP2;
    polyligneP4._PolyligneP0 = &polyligneP1;
    polyligneP4._PolyligneP1 = &polyligneP6;
    polyligneP5._PolyligneP0 = &polyligneP4;
    polyligneP6._PolyligneP0 = &polyligneP4;
    polyligneP6._PolyligneP1 = &polyligneP7;
    polyligneP7._PolyligneP0 = &polyligneP6;

    // Geometrique Parcours
    TYSetGeometriqueParcours geoParcours;
    geoParcours._nNbPointTotal = 8;
    geoParcours._nNbPolylines = 8;
    geoParcours._ListePoint = new TYPointParcours[8];
    geoParcours._ListePoint[0] = S;
    geoParcours._ListePoint[1] = P1;
    geoParcours._ListePoint[2] = P2;
    geoParcours._ListePoint[3] = P3;
    geoParcours._ListePoint[4] = P4;
    geoParcours._ListePoint[5] = P5;
    geoParcours._ListePoint[6] = P6;
    geoParcours._ListePoint[7] = D;
    geoParcours._ListePolylines = new TYPolyligneParcours[8];
    geoParcours._ListePolylines[0].Copy(polyligneSD);
    geoParcours._ListePolylines[1].Copy(polyligneP1);
    geoParcours._ListePolylines[2].Copy(polyligneP2);
    geoParcours._ListePolylines[3].Copy(polyligneP3);
    geoParcours._ListePolylines[4].Copy(polyligneP4);
    geoParcours._ListePolylines[5].Copy(polyligneP5);
    geoParcours._ListePolylines[6].Copy(polyligneP6);
    geoParcours._ListePolylines[7].Copy(polyligneP7);

*/
    // Call to the tested method:
    //TYSetGeometriqueParcours::Premierepasse(result, ...);

    // Check result:
    //EXPECT_EQ(result, expected_result);

    // Destruction

}

// TO TEST
// Testing the TYSetGeometriqueParcours::SecondePasse method
TEST(test_TYSetGeometriqueParcours, SecondePasse)
{
    /*
    // Data setup:
    // Points
    TYPointParcours S;
    S.Identifiant = 0;TYPolyligneParcours
    S.x = 1.0;
    S.y = 1.0;
    S.isEcran = S.isInfra = false;
    TYPointParcours D;
    D.Identifiant = 7;
    D.x = 17.0;
    D.y = 12.0;
    D.isEcran = D.isInfra = false;
    TYPointParcours P1;
    P1.Identifiant = 1;
    P1.x = 4.0;
    P1.y = 7.0;
    P1.isEcran = P1.isInfra = false;
    TYPointParcours P2;
    P2.Identifiant = 2;
    P2.x = 7.0;
    P2.y = 1.0;
    P2.isEcran = P2.isInfra = false;
    TYPointParcours P3;
    P3.Identifiant = 3;
    P3.x = 12.0;
    P3.y = 7.0;
    P3.isEcran = P3.isInfra = false;
    TYPointParcours P4;
    P4.Identifiant = 4;
    P4.x = 9.0;
    P4.y = 9.0;
    P4.isEcran = P4.isInfra = false;
    TYPointParcours P5;
    P5.Identifiant = 5;
    P5.x = 10.0;
    P5.y = 14.0;
    P5.isEcran = P5.isInfra = false;
    TYPointParcours P6;
    P6.Identifiant = 6;
    P6.x = 14.0;
    P6.y = 10.0;
    P6.isEcran = P6.isInfra = false;

    // Polylignes
    TYPolyligneParcours polyligneSD;
    polyligneSD.ajouteSegment(&S, &D);
    polyligneSD.ajoutePoint(2, &P1);
    polyligneSD.ajoutePoint(3, &P2);
    polyligneSD.ajoutePoint(4, &P3);
    polyligneSD.ajoutePoint(5, &P4);
    polyligneSD.ajoutePoint(6, &P5);
    polyligneSD.ajoutePoint(7, &P6);

    TYPolyligneParcours polyligneP1;
    polyligneP1.ajouteSegment(&S, &P1);
    TYPolyligneParcours polyligneP2;
    polyligneP2.ajouteSegment(&P1, &P2);
    TYPolyligneParcours polyligneP3;
    polyligneP3.ajouteSegment(&P2, &P3);
    TYPolyligneParcours polyligneP4;
    polyligneP4.ajouteSegment(&P1, &P4);
    TYPolyligneParcours polyligneP5;
    polyligneP5.ajouteSegment(&P4, &P5);
    TYPolyligneParcours polyligneP6;
    polyligneP6.ajouteSegment(&P4, &P6);
    TYPolyligneParcours polyligneP7;
    polyligneP7.ajouteSegment(&P6, &D);

    polyligneP1._PolyligneP1 = &polyligneP4;
    polyligneP2._PolyligneP0 = &polyligneP1;
    polyligneP2._PolyligneP1 = &polyligneP3;
    polyligneP3._PolyligneP0 = &polyligneP2;
    polyligneP4._PolyligneP0 = &polyligneP1;
    polyligneP4._PolyligneP1 = &polyligneP6;
    polyligneP5._PolyligneP0 = &polyligneP4;
    polyligneP6._PolyligneP0 = &polyligneP4;
    polyligneP6._PolyligneP1 = &polyligneP7;
    polyligneP7._PolyligneP0 = &polyligneP6;

    // Geometrique Parcours
    TYSetGeometriqueParcours geoParcours;
    geoParcours._nNbPointTotal = 8;
    geoParcours._nNbPolylines = 8;
    geoParcours._ListePoint = new TYPointParcours[8];
    geoParcours._ListePoint[0] = S;
    geoParcours._ListePoint[1] = P1;
    geoParcours._ListePoint[2] = P2;
    geoParcours._ListePoint[3] = P3;
    geoParcours._ListePoint[4] = P4;
    geoParcours._ListePoint[5] = P5;
    geoParcours._ListePoint[6] = P6;
    geoParcours._ListePoint[7] = D;
    geoParcours._ListePolylines = new TYPolyligneParcours[8];
    geoParcours._ListePolylines[0].Copy(polyligneSD);
    geoParcours._ListePolylines[1].Copy(polyligneP1);
    geoParcours._ListePolylines[2].Copy(polyligneP2);
    geoParcours._ListePolylines[3].Copy(polyligneP3);
    geoParcours._ListePolylines[4].Copy(polyligneP4);
    geoParcours._ListePolylines[5].Copy(polyligneP5);
    geoParcours._ListePolylines[6].Copy(polyligneP6);
    geoParcours._ListePolylines[7].Copy(polyligneP7);
    */
    // Call to the tested method:
    //TYSetGeometriqueParcours::SecondePasse(result, ...);

    // Check result:
    //EXPECT_EQ(result, expected_result);

    // Destruction
}
