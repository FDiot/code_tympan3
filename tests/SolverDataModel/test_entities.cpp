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

#include "TympanTestsConfig.hpp"

#include <iostream>
#include <fstream>
#include <deque>

#include <boost/math/special_functions/fpclassify.hpp>

#include "gtest/gtest.h"

#include "test_utils/misc.hpp"
#include "TympanTestsConfig.hpp"
#include "Tympan/MetierSolver/SolverDataModel/entities.hpp"
#include "Tympan/MetierSolver/SolverDataModel/acoustic_problem_model.hpp"

using namespace std;
tympan::AtmosphericConditions functionnalResults_initAtmosphereFromRow(const deque<double>& row)
{
    double pressure = row[0];
	double temperature = row[1];
	double hygrometry = row[2];

	return tympan::AtmosphericConditions(pressure, temperature, hygrometry);
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
		tympan::AtmosphericConditions atm = functionnalResults_initAtmosphereFromRow(row);
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
    tympan::AtmosphericConditions atm(101325., 43., 70.);
    double estimated_sound_speed = 331. + 0.6 * 43.;
    double computed_sound_speed = atm.compute_c();

    EXPECT_NEAR( estimated_sound_speed, computed_sound_speed, 1.0);
}
