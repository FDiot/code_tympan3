/**
*
* @brief Functional tests of the TYCalculParcours class  
* 
*  Created on: january 18, 2018 
*  Author: Pierre LEDAC <pierre.ledac@c-s.fr>
*
*/
#include "gtest/gtest.h"
#include "Tympan/geometric_methods/ConvexHullFinder/TYCalculParcours.h"
#include "Tympan/geometric_methods/ConvexHullFinder/TYSetGeometriqueParcours.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/3d.h"

/**
* @brief Fixture which provides an instance of TYCalculParcours
*/
class TYCalculParcoursTest :
    public testing::Test
{
    private:
        TYCalculParcours CalculParcours;
};

//TEST_F(TYCalculParcoursTest, create_instance)
//{
//}

// DONE
// Testing the TYCalculParcours::InitChangementVariable2D3D method
TEST(test_TYCalculParcours, InitChangementVariable2D3D)
{
   // Data setup:
   TYCalculParcours CalculParcoursVertical1(10, true);
   TYCalculParcours CalculParcoursVertical2(10, true);
   TYCalculParcours CalculParcoursHorizontal(10, false);

   // Call to the tested method:
   CalculParcoursVertical1.InitChangementVariable2D3D(true);
   CalculParcoursVertical2.InitChangementVariable2D3D(false);
   CalculParcoursHorizontal.InitChangementVariable2D3D(NULL);
   
   // Check result:
   EXPECT_EQ(1, CalculParcoursVertical1._indexXInOut);
   EXPECT_EQ(2, CalculParcoursVertical1._indexYInOut);
   EXPECT_EQ(0, CalculParcoursVertical1._indexZInOut);

   EXPECT_EQ(0, CalculParcoursVertical2._indexXInOut);
   EXPECT_EQ(2, CalculParcoursVertical2._indexYInOut);
   EXPECT_EQ(1, CalculParcoursVertical2._indexZInOut);

   EXPECT_EQ(0, CalculParcoursHorizontal._indexXInOut);
   EXPECT_EQ(1, CalculParcoursHorizontal._indexYInOut);
   EXPECT_EQ(2, CalculParcoursHorizontal._indexZInOut);


}

// DONE
// Testing the TYCalculParcours::AjouterSegmentCoupe method
TEST(test_TYCalculParcours, AjouterSegmentCoupe)
{

    // Data setup:
    // Segment to add:
    OPoint3D ptA(0.0, 0.0, 0.0);
    OPoint3D ptB(10.0, 10.0, 0.0);

    OPoint3D ptC(1.0, 3.0, 0.0);
    OPoint3D ptD(4.0, 7.0, 0.0);

    TYCalculParcours CalculParcoursVertical(10, false);
    CalculParcoursVertical.AjouterSegmentSR(ptA, ptB);

    // Call to the tested method:
    CalculParcoursVertical.AjouterSegmentCoupe(ptC, ptD, true, true);

    // Check result:
    EXPECT_EQ(2, CalculParcoursVertical._geoImporterDXF->_nNbPointTotal);
    EXPECT_EQ(1.0, CalculParcoursVertical._geoImporterDXF->_ListePolylines[0].pointPtr(0)->x);
    EXPECT_EQ(3.0, CalculParcoursVertical._geoImporterDXF->_ListePolylines[0].pointPtr(0)->y);
    EXPECT_EQ(0.0, CalculParcoursVertical._geoImporterDXF->_ListePolylines[0].pointPtr(0)->z);
    EXPECT_EQ(4.0, CalculParcoursVertical._geoImporterDXF->_ListePolylines[0].pointPtr(1)->x);
    EXPECT_EQ(7.0, CalculParcoursVertical._geoImporterDXF->_ListePolylines[0].pointPtr(1)->y);
    EXPECT_EQ(0.0, CalculParcoursVertical._geoImporterDXF->_ListePolylines[0].pointPtr(1)->z);


}


// DONE
// Testing the TYCalculParcours::AjouterSegmentSR method
TEST(test_TYCalculParcours, AjouterSegmentSR)
{

   // Data setup:
   TYCalculParcours Parcours(10, true);
   // Segment to add:
   OPoint3D ptA(-10.0, 23.0, 0.0);
   OPoint3D ptB(+10.0, 23.0, 5.0);

   // Call to the tested method:
   Parcours.AjouterSegmentSR(ptA, ptB);
   
   // Check result (a segment has been added):
   EXPECT_EQ(1, Parcours._geoSR->_nNbPolylines);
   EXPECT_EQ(2, Parcours._geoSR->_nNbPointTotal);
   EXPECT_EQ(2, Parcours._geoSR->_ListePolylines->nombreDePoint());
   EXPECT_EQ(0, Parcours._geoSR->_ListePolylines[0].indexePoint1());
   EXPECT_EQ(1, Parcours._geoSR->_ListePolylines[0].indexePoint2());

}

// DONE
// Testing the TYCalculParcours::PointTrajetGauche method
TEST(test_TYCalculParcours, PointTrajetGauche)
{

    // Points
    OPoint3D S(0.0, 0.0, 0.0);
    OPoint3D R(10.0, 10.0, 0.0);

    OPoint3D P1(1.0, 3.0, 0.0);
    OPoint3D P2(3.0, 3.0, 0.0);
    OPoint3D P3(6.0, 4.0, 0.0);
    OPoint3D P4(4.0, 7.0, 0.0);
    OPoint3D P5(7.0, 7.0, 0.0);
    OPoint3D P6(10.0, 7.0, 0.0);

    OPoint3D P7(5.0, 5.0, 0);

    // Construction Parcours avec bVertical = false
    TYCalculParcours Parcours(8, false);
    // Rajout de la Source et du Recepteur
    Parcours.AjouterSegmentSR(S, R);

    Parcours.AjouterSegmentCoupe(S, P1, true, true);
    Parcours.AjouterSegmentCoupe(P1, P2, true, true);
    Parcours.AjouterSegmentCoupe(P2, P3, true, true);
    Parcours.AjouterSegmentCoupe(P3, P4, true, true);
    Parcours.AjouterSegmentCoupe(P4, P5, true, true);
    Parcours.AjouterSegmentCoupe(P5, P6, true, true);
    Parcours.AjouterSegmentCoupe(P6, R, true, true);


    // Call to the tested method
    Parcours.Traitement();
    Parcours.PointTrajetGauche(2, P7);

    // Check result:
    EXPECT_EQ(4, P7._x);
    EXPECT_EQ(7, P7._y);
    EXPECT_EQ(0, P7._z);


}

// DONE
// Testing the TYCalculParcours::PointTrajetDroite method
TEST(test_TYCalculParcours, PointTrajetDroite)
{
    // Points
    OPoint3D S(0.0, 0.0, 0.0);
    OPoint3D R(10.0, 10.0, 0.0);

    OPoint3D P1(1.0, 3.0, 0.0);
    OPoint3D P2(3.0, 3.0, 0.0);
    OPoint3D P3(6.0, 4.0, 0.0);
    OPoint3D P4(4.0, 7.0, 0.0);
    OPoint3D P5(7.0, 7.0, 0.0);
    OPoint3D P6(10.0, 7.0, 0.0);

    OPoint3D P7;

    // Construction Parcours avec bVertical = false
    TYCalculParcours Parcours(8, false);
    // Rajout de la Source et du Recepteur
    Parcours.AjouterSegmentSR(S, R);

    Parcours.AjouterSegmentCoupe(S, P1, true, true);
    Parcours.AjouterSegmentCoupe(P1, P2, true, true);
    Parcours.AjouterSegmentCoupe(P2, P3, true, true);
    Parcours.AjouterSegmentCoupe(P3, P4, true, true);
    Parcours.AjouterSegmentCoupe(P4, P5, true, true);
    Parcours.AjouterSegmentCoupe(P5, P6, true, true);
    Parcours.AjouterSegmentCoupe(P6, R, true, true);


    // Call to the tested method
    Parcours.Traitement();
    Parcours.PointTrajetDroite(2, P7);

    // Check result:
    EXPECT_EQ(10, P7._x);
    EXPECT_EQ(7, P7._y);
    EXPECT_EQ(0, P7._z);



}

// Testing the TYCalculParcours::Traitement method
TEST(test_TYCalculParcours, Traitement)
{

    // Data setup:
    // Points
    OPoint3D S(0.0, 0.0, 0.0);
    OPoint3D R(10.0, 10.0, 0.0);

    OPoint3D P1(1.0, 3.0, 0.0);
    OPoint3D P2(3.0, 3.0, 0.0);
    OPoint3D P3(6.0, 4.0, 0.0);
    OPoint3D P4(4.0, 7.0, 0.0);
    OPoint3D P5(7.0, 7.0, 0.0);
    OPoint3D P6(10.0, 7.0, 0.0);

    // Construction Parcours avec bVertical = false
    TYCalculParcours ParcoursH(8, false);
    // Rajout de la Source et du Recepteur
    ParcoursH.AjouterSegmentSR(S, R);

    ParcoursH.AjouterSegmentCoupe(S, P1, true, true);
    ParcoursH.AjouterSegmentCoupe(P1, P2, true, true);
    ParcoursH.AjouterSegmentCoupe(P2, P3, true, true);
    ParcoursH.AjouterSegmentCoupe(P3, P4, true, true);
    ParcoursH.AjouterSegmentCoupe(P4, P5, true, true);
    ParcoursH.AjouterSegmentCoupe(P5, P6, true, true);
    ParcoursH.AjouterSegmentCoupe(P6, R, true, true);

    // Call to the tested method:
    bool t_traitementH = ParcoursH.Traitement();


    // Check result:

    // HORIZONTAL
    EXPECT_EQ(false, t_traitementH);

    // Nbre LIGNES / POINTS
    EXPECT_EQ(true, ParcoursH._geoTrajetGauche != NULL);
    EXPECT_EQ(true, ParcoursH._geoTrajetDroite != NULL);
    EXPECT_EQ(4, ParcoursH._geoTrajetGauche->_nNbPointTotal);
    EXPECT_EQ(1, ParcoursH._geoTrajetGauche->_nNbPolylines);

    EXPECT_EQ(4, ParcoursH._geoSecondePasseGauche._nNbPointTotal);
    EXPECT_EQ(1, ParcoursH._geoSecondePasseGauche._nNbPolylines);

    EXPECT_EQ(4, ParcoursH._geoTrajetDroite->_nNbPointTotal);
    EXPECT_EQ(1, ParcoursH._geoTrajetDroite->_nNbPolylines);

    EXPECT_EQ(4, ParcoursH._geoSecondePasseDroite._nNbPointTotal);
    EXPECT_EQ(1, ParcoursH._geoSecondePasseDroite._nNbPolylines);



    // VERTICAL

    OPoint3D S_v(0.0, 0.0, 0.0);
    OPoint3D R_v(10.0, 0.0, 10.0);

    OPoint3D P1_v(1.0, 0.0, 3.0);
    OPoint3D P2_v(3.0, 0.0, 3.0);
    OPoint3D P3_v(6.0, 0.0, 4.0);
    OPoint3D P4_v(4.0, 0.0, 7.0);
    OPoint3D P5_v(7.0, 0.0, 7.0);
    OPoint3D P6_v(10.0, 0.0, 7.0);

    // Construction Parcours avec bVertical = true
    TYCalculParcours ParcoursV(8, true);

    // Rajout de la Source et du Recepteur
    ParcoursV.AjouterSegmentSR(S_v, R_v);

    ParcoursV.AjouterSegmentCoupe(S_v, P1_v, true, true);
    ParcoursV.AjouterSegmentCoupe(P1_v, P2_v, true, true);
    ParcoursV.AjouterSegmentCoupe(P2_v, P3_v, true, true);
    ParcoursV.AjouterSegmentCoupe(P3_v, P4_v, true, true);
    ParcoursV.AjouterSegmentCoupe(P4_v, P5_v, true, true);
    ParcoursV.AjouterSegmentCoupe(P5_v, P6_v, true, true);
    ParcoursV.AjouterSegmentCoupe(P6_v, R_v, true, true);

    bool t_traitementV = ParcoursV.Traitement();

    EXPECT_EQ(false, t_traitementV);

    // Nbre LIGNES / POINTS
    EXPECT_EQ(true, ParcoursV._geoTrajetGauche != NULL);
    EXPECT_EQ(true, ParcoursV._geoTrajetDroite != NULL);

    if(ParcoursV._geoTrajetGauche != NULL){
        EXPECT_EQ(4, ParcoursV._geoTrajetGauche->_nNbPointTotal);
        EXPECT_EQ(1, ParcoursV._geoTrajetGauche->_nNbPolylines);
    }

    if(ParcoursV._geoTrajetDroite != NULL){
        EXPECT_EQ(4, ParcoursV._geoTrajetDroite->_nNbPointTotal);
        EXPECT_EQ(1, ParcoursV._geoTrajetDroite->_nNbPolylines);
    }

    EXPECT_EQ(4, ParcoursV._geoSecondePasseGauche._nNbPointTotal);
    EXPECT_EQ(1, ParcoursV._geoSecondePasseGauche._nNbPolylines);

    EXPECT_EQ(4, ParcoursV._geoSecondePasseDroite._nNbPointTotal);
    EXPECT_EQ(1, ParcoursV._geoSecondePasseDroite._nNbPolylines);

}
