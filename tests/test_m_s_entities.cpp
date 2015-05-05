/**
 * \file test_entities.cpp
 * \test Test the representation of entities of the model.
 *
 * \date nov. 6 2012
 * \author Anthony Truchet <anthony.truchet@logilab.fr>
 * \author Damien Garaud <damien.garaud@logilab.fr>
 */
#include <boost/foreach.hpp>

#include <string>

#include "TympanTestsConfig.h"

#include <iostream>
#include <fstream>
#include <deque>

#include <boost/math/special_functions/fpclassify.hpp>

#include "gtest/gtest.h"

#include "testutils.h"
#include "TympanTestsConfig.h"
#include "Tympan/models/solver/entities.hpp"
#include "Tympan/models/solver/acoustic_problem_model.hpp"
#include "Tympan/models/solver/data_model_common.hpp"
#include "Tympan/models/common/atmospheric_conditions.h"
#include "Tympan/models/common/spectre.h"

using namespace std;
AtmosphericConditions functionnalResults_initAtmosphereFromRow(const deque<double>& row)
{
    double pressure = row[0];
	double temperature = row[1];
	double hygrometry = row[2];

	return AtmosphericConditions(pressure, temperature, hygrometry);
}

TEST(AcousticEntities, AtmosphericAbsorption)
{
    const double precision = 0.1; // in dB/km

    // Locate CSV data file
    string data_file_name = tympan::path_to_test_data("Atmospheric_absorption.csv");

    ifstream file(data_file_name.c_str());
    ASSERT_TRUE(file.is_open()) <<
        "Can not open test data file : " << data_file_name.c_str();
    string header;
    getline(file, header); // Read and check the header line
    ASSERT_FALSE(file.fail())<<
        "Failure while reading test data file : " << data_file_name.c_str();
    EXPECT_EQ("Pressure,Temperature,Hygrometry,Frequency,dB/km", header);
    deque<deque<double> > table = readCsvAsTableOf<double>(file);
    ASSERT_EQ(90, table.size()) <<
        "Bad number of rows in test data file " << data_file_name.c_str();

    unsigned row_num = 0;
	double computed_value = 0.;
    BOOST_FOREACH(const deque<double>& row, table)
    {
        ++row_num;
		AtmosphericConditions atm = functionnalResults_initAtmosphereFromRow(row);
		computed_value = atm.get_absorption_value( row[3] );

        // Tabbed value is done in dB/km computed must be multiplied by 1000 for comparison
        EXPECT_NEAR( row[4], computed_value*1000., precision) <<
                                                       "Incorrect results for the row #" << row_num << " (header is #0)"
                                                       " of the test data file : " << data_file_name.c_str();
    }
    file.close();
}

TEST(AcousticEntities, SoundSpeed)
{
    AtmosphericConditions atm(101325., 43., 70.);
    double estimated_sound_speed = 331. + 0.6 * 43.;
    double computed_sound_speed = atm.compute_c();

    EXPECT_NEAR( estimated_sound_speed, computed_sound_speed, 1.0);
}

TEST(AcousticCentroid, Centroid)
{
    tympan::source_pool_t tabSources;
    tympan::Point ref(0., 0., 0.);
    tympan::Spectrum sp(100.);
    tympan::SphericalSourceDirectivity *sd = new tympan::SphericalSourceDirectivity();

    tympan::AcousticSource  s1(ref, sp, sd),
                            s2(ref, sp, sd), 
                            s3(ref, sp, sd), 
                            s4(ref, sp, sd), 
                            s5(ref, sp, sd), 
                            s6(ref, sp, sd), 
                            s7(ref, sp, sd), 
                            s8(ref, sp, sd);

    s1.position._x =  1., s1.position._y =  1., s1.position._z =  1.; tabSources.push_back(s1);
    s2.position._x =  1., s2.position._y = -1., s2.position._z =  1.; tabSources.push_back(s2);
    s3.position._x = -1., s3.position._y = -1., s3.position._z =  1.; tabSources.push_back(s3);
    s4.position._x = -1., s4.position._y =  1., s4.position._z =  1.; tabSources.push_back(s4);
    s5.position._x =  1., s5.position._y =  1., s5.position._z = -1.; tabSources.push_back(s5);
    s6.position._x =  1., s6.position._y = -1., s6.position._z = -1.; tabSources.push_back(s6);
    s7.position._x = -1., s7.position._y = -1., s7.position._z = -1.; tabSources.push_back(s7);
    s8.position._x = -1., s8.position._y =  1., s8.position._z = -1.; tabSources.push_back(s8);

    for (unsigned int i=0; i<tabSources.size(); i++)
    {
        tabSources[i].spectrum = sp;
    }

    tympan::Point centroid = tympan::ComputeAcousticCentroid(tabSources);

    EXPECT_EQ(0., centroid._x);
    EXPECT_EQ(0., centroid._y);
    EXPECT_EQ(0., centroid._z);
}
