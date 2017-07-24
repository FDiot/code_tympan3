/**
 * \file test_test_Sonie.cpp
 * \test Testing of sonie computing adapted from matlab code
 *
 *  Created on: june 4, 2013
 *      Author: Denis THOMASSON <denis.thomasson@edf.fr>
 */


#include <cstdlib>

#include "gtest/gtest.h"

#include <iostream>
#include <fstream>
#include <iomanip>

#include "Tympan/models/common/sonie_zwicker_1991.h"

double* createSpectre()
{
    //               1    2    3    4    5    6    7   8     9   10   11   12   13   14   15   16   17   18   19   20   21   22   23   24   25   26   27   28
    //double vals[] = {-60, -60, -60, -60, -60, -60, -60, -60, -60, -60, -60, -60, -60, -60, -60, -60,  40, -60, -60, -60, -60, -60, -60, -60, -60, -60, -60, -60};
    //  double vals[] = {-60, -60, 78, 79, 89, 72, 80, 89, 75, 87, 85, 79, 86, 80, 71, 70, 72, 71, 72, 74, 69, 65, 67, 77, 68, 58, 45, 30};
    //  double vals[] = {-60, -60,  -60,-60,-60,-60, -60, -60, -60, -60, -60, -60, -60, -60, -60, -60,   40,  -60,  -60,   -60,  -60,  -60,  -60,  -60,  -60,  -60,   -60,   -60};
    //                { 25,  31.5, 40, 50, 63, 80, 100, 125, 160, 200, 250, 315, 400, 500, 630, 800, 1000, 1250, 1600, 2000, 2500, 3150, 4000, 5000, 6300, 8000, 10000, 12500}
    double vals[] = {   54.7,    50.9,   48.2,   45,     40.1,  37.6,   40.9,
                        32.9,    31.9,   31.9,   30.6,   32.1,  32.3,   32.5,
                        33,      27.7,   26.4,   25.3,   18.7,  13.5,    6.5,
                        -03.9,  -19.5,  -38.3,  -60,    -60,   -60,    -60
                    };

    double* LT = new double[28] ;
    for (unsigned int i = 0; i < 28; i++)
    {
        LT[i] = vals[i];
    }

    return LT;
}

TEST(SonieTest, DISABLED_SonieTest)
{
    double* LT = createSpectre();

    // Test if input spectre is right
    // 1st spectrum is not ok (a value is greater than 120)
    double tmp = LT[10];
    LT[10] = 160.;
    sonie testSonie(LT, 0);
    bool isOk = testSonie.isOk();
    ASSERT_FALSE(isOk);

    // 2nd spectrum is ok
    LT[10] = tmp;
    testSonie = sonie(LT, 0);
    isOk = testSonie.isOk();
    ASSERT_TRUE(isOk);

    // Test of the values returned by sonie computing
    if (testSonie.isOk())
    {
        double N = testSonie.getNTot();
        ASSERT_TRUE(N == 1.92); // Value to be set by Stephanie

        double LN = testSonie.getLN();
        ASSERT_TRUE(LN == 49.4111); // Value to be set by Stephanie

        // Export de la sonie spécifique pour comparaison avec le résultat MATLAB (OCTAVE)
        //double* n_specif = testSonie.getNSpecif();
        testSonie.getNSpecif();
    }

    delete [] LT;
}
