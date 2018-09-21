/**
*
* @brief Functional tests of the TYAcousticPathFinder class  
* 
*  Created on: january 18, 2018 
*  Author: Philippe CHAPUIS <philippe.chapuis@c-s.fr> 
*
*/
#include "gtest/gtest.h"
#include "Tympan/solvers/DefaultSolver/TYAcousticPathFinder.h"
#include "Tympan/models/solver/entities.hpp"
#include "Tympan/solvers/DefaultSolver/TYTrajet.h"
#include "Tympan/solvers/DefaultSolver/TYSolver.h"

/**
* @brief Fixture which provides an instance of TYAcousticPathFinder
*/
class TYAcousticPathFinderTest :
    public testing::Test
{
private:
    TYAcousticPathFinder AcousticPathFinder;
};

// Testing the TYAcousticPathFinder::computePath method
TEST(test_TYAcousticPathFinder, computePath)
    {
    // Data setup:
    // Position de la source
    const OPoint3D S(0, 0, 50);
    // Position du recepteur
    const OPoint3D R(100, 50, 50);
    // Initialisation du spectre
    OSpectre Spectre;
    Spectre.setDefaultValue(90);

    OVector3D direct(1, 1, 1);
    tympan::SourceDirectivityInterface *Directivity = new tympan::SphericalSourceDirectivity();
    Directivity->lwAdjustment(direct, 100.0);

    tympan::AcousticSource Source(S, Spectre, Directivity);
    tympan::AcousticReceptor Receptor(R);

    TYTrajet trajet(Source, Receptor);

    // Description d'un plan en TOP / Gauche / Droit
    TabPoint3D ptsTop, ptsLeft, ptsRight;

    std::deque<TYSIntersection> tabIntersect;
    OPoint3D ptS11(50, 0, 0);
    OPoint3D ptS12(50, 0, 10);
    OPoint3D ptS21(50, 0, 0);
    OPoint3D ptS22(50, 100, 0);
    OPoint3D ptS31(0, 50, 0);
    OPoint3D ptS32(150, 50, 0);
    OSegment3D segV(ptS11, ptS12);
    OSegment3D segH(ptS21, ptS22);
    OSegment3D segH2(ptS31, ptS32);
    TYSIntersection m_Intersection01,m_Intersection02;
    m_Intersection01.isEcran = true;
    m_Intersection01.isInfra = true;
    m_Intersection01.segInter[0] = segV;
    m_Intersection01.segInter[1] = segH;

    m_Intersection02.isEcran = true;
    m_Intersection02.isInfra = true;
    m_Intersection02.segInter[0] = segH;
    m_Intersection02.segInter[1] = segH2;

    tabIntersect.push_back(m_Intersection01);
    tabIntersect.push_back(m_Intersection02);

    TYSolver solver; //= trajet;
    TYAcousticPathFinder pathFinder(solver);

    EXPECT_EQ(0, ptsTop.size());
    EXPECT_EQ(0, ptsLeft.size());
    EXPECT_EQ(0, ptsRight.size());

    // Call to the tested method:
    pathFinder.init();
    pathFinder.computePath(tabIntersect, trajet, ptsTop, ptsLeft, ptsRight);

    // Check result
    EXPECT_EQ(2, tabIntersect.size());

}
