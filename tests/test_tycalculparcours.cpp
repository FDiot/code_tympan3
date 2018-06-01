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

// Testing the TYCalculParcours::AjouterSegmentCoupe method
TEST(test_TYCalculParcours, AjouterSegmentCoupe)
{
   // Data setup:
   // ToDo

   // Call to the tested method:
   

   // Check result:
   //EXPECT_EQ(result, expected_result);
}

// Testing the TYCalculParcours::AjouterSegmentSR method
TEST(test_TYCalculParcours, AjouterSegmentSR)
{
   // Data setup:
   TYCalculParcours CalculParcoursVertical(10, true);
   // Segment to add:
   OPoint3D ptA(-10.0, 23.0, 0.0);
   OPoint3D ptB(+10.0, 23.0, 5.0);

   // Call to the tested method:
   CalculParcoursVertical.AjouterSegmentSR(ptA, ptB);
   
   // Check result (a segment has been added):
   TYSetGeometriqueParcours* geo = CalculParcoursVertical._geoSR;
   EXPECT_EQ(1, geo->_nNbPolylines);
   EXPECT_EQ(2, geo->_nNbPointTotal);
   EXPECT_EQ(2, geo->_ListePolylines->nombreDePoint());
   EXPECT_EQ(0, geo->_ListePolylines[0].indexePoint1());
   EXPECT_EQ(1, geo->_ListePolylines[0].indexePoint2());
}

// Testing the TYCalculParcours::PointTrajetGauche method
TEST(test_TYCalculParcours, PointTrajetGauche)
{
   // Data setup:
   // ToDo

   // Call to the tested method:
   //TYCalculParcours::PointTrajetGauche(result, ...);
   
   // Check result:
   //EXPECT_EQ(result, expected_result);
}

// Testing the TYCalculParcours::PointTrajetDroite method
TEST(test_TYCalculParcours, PointTrajetDroite)
{
   // Data setup:
   // ToDo

   // Call to the tested method:
   //TYCalculParcours::PointTrajetDroite(result, ...);
   
   // Check result:
   //EXPECT_EQ(result, expected_result);
}

// Testing the TYCalculParcours::Traitement method
TEST(test_TYCalculParcours, Traitement)
{
   // Data setup:
   // ToDo

   // Call to the tested method:
   //TYCalculParcours::Traitement(result, ...);
   
   // Check result:
   //EXPECT_EQ(result, expected_result);
}
