/**
*
* @brief Functional tests of the TYFaceSelector class  
* 
*  Created on: january 18, 2018 
*  Author: Philippe CHAPUIS <philippe.chapuis@c-s.fr> 
*
*/
#include "gtest/gtest.h"
#include "Tympan/solvers/DefaultSolver/TYFaceSelector.h"
#include "Tympan/models/solver/entities.hpp"
#include "Tympan/solvers/DefaultSolver/TYTrajet.h"
#include "Tympan/solvers/DefaultSolver/TYSolver.h"
#include "Tympan/models/solver/acoustic_problem_model.hpp"
#include "Tympan/models/solver/data_model_common.hpp"

/**
* @brief Fixture which provides an instance of TYFaceSelector
*/
class TYFaceSelectorTest :
    public testing::Test
{
private:
    TYFaceSelector FaceSelector;
};

// Testing the TYFaceSelector::selectFaces method
TEST(test_TYFaceSelector, selectFaces)
{
   // Data setup:
   const OPoint3D S(0.0, 30.0, 20.0);
   const OPoint3D R(100.0, 30.0, 20.0);

   OSegment3D segSR(S, R);

   OVector3D direct(1, 1, 1);
   tympan::Spectrum mSpectrum;
   mSpectrum.setDefaultValue(90);
   tympan::SourceDirectivityInterface* directivity = new tympan::SphericalSourceDirectivity();
   directivity->lwAdjustment(direct, 100.0);
   tympan::AcousticSource m_AcousticSource(S, mSpectrum, directivity);
   tympan::AcousticReceptor m_AcousticReceptor(R);

   TYTrajet Rayon(m_AcousticSource, m_AcousticReceptor);
   Rayon.setPtSetPtRfromOSeg3D(segSR);
   Rayon.setDistance(100.0);

   std::deque<TYSIntersection> tabIntersect;
   tabIntersect.clear();

   //objets necessaires ? la création du solver
   tympan::AcousticProblemModel newProblem,newProblem2;
   tympan::AcousticResultModel newResult;
   tympan::LPSolverConfiguration configuration;

   tympan::node_idx x_inter,y_inter,z_inter,x_NoInter,y_NoInter,z_NoInter;
   tympan::triangle_idx idx_triIntersect,idx_triNoIntersect;

   //Test avec un triangle qui doit intercepter le rayon et l'autre non

   x_inter = newProblem.make_node(50,0,0);//node[0]
   y_inter = newProblem.make_node(50,60,0);//node[1]
   z_inter = newProblem.make_node(50,30,50);//node[2]

   x_NoInter = newProblem.make_node(10,50,0);//node[3]
   y_NoInter = newProblem.make_node(10,100,0);//node[4]
   z_NoInter = newProblem.make_node(10,75,5);//node[5]

   idx_triIntersect = newProblem.make_triangle(x_inter,y_inter,z_inter);
   idx_triNoIntersect = newProblem.make_triangle(x_NoInter,y_NoInter,z_NoInter);

   EXPECT_EQ(0, x_inter);
   EXPECT_EQ(1, y_inter);
   EXPECT_EQ(2, z_inter);
   EXPECT_EQ(3, x_NoInter);
   EXPECT_EQ(4, y_NoInter);
   EXPECT_EQ(5, z_NoInter);
   EXPECT_EQ(0, idx_triIntersect);
   EXPECT_EQ(1, idx_triNoIntersect);

   TYSolver mSolver,mSolver2;
   bool result = mSolver.solve(newProblem,newResult,configuration);

   EXPECT_EQ(true, result);


   TYFaceSelector mFaceSelect(mSolver);

   // Call to the tested method:
   mFaceSelect.selectFaces(tabIntersect, Rayon);
   
   EXPECT_EQ(1, tabIntersect.size());


   //Test avec seulement le triangle qui ne doit pas intercepter le rayon

   x_NoInter = newProblem2.make_node(10,50,0);//node[0]
   y_NoInter = newProblem2.make_node(10,100,0);//node[1]
   z_NoInter = newProblem2.make_node(10,75,5);//node[2]

   idx_triNoIntersect = newProblem2.make_triangle(x_NoInter,y_NoInter,z_NoInter);

   EXPECT_EQ(0, x_NoInter);
   EXPECT_EQ(1, y_NoInter);
   EXPECT_EQ(2, z_NoInter);
   EXPECT_EQ(0, idx_triNoIntersect);


   bool result2 = mSolver2.solve(newProblem2,newResult,configuration);
   EXPECT_EQ(true, result2);

   TYFaceSelector mFaceSelect2(mSolver2);

   // Call to the tested method:
   tabIntersect.clear();
   mFaceSelect2.selectFaces(tabIntersect, Rayon);

   EXPECT_EQ(0, tabIntersect.size());


    tabIntersect.clear();
}
