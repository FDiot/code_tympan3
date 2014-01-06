//! \test test_boundary_noise_map.cpp

#include <iostream>

#include "gtest/gtest.h"

#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYBoundaryNoiseMap.h"

using std::cout;
using std::endl;

TEST(TestBoundaryNoiseMap, rectangular_polyline) {

    TYTabPoint table_points(4);
    const double z = 0.;
    table_points[0] = TYPoint(0., 0., z);
    table_points[1] = TYPoint(110., 0., z);
    table_points[2] = TYPoint(110., 70., z);
    table_points[3] = TYPoint(0., 70., z);

    const double density = 0.1;
    const double thickness = 10.;

    const bool closed = true;

    TYBoundaryNoiseMap boundary_map;

    // Build closed polyline...
    boundary_map.make(table_points, thickness, closed, density);
    ASSERT_EQ(68, boundary_map.getPtsCalcul().size());

    // Build open polyline...
    boundary_map.make(table_points, thickness, !closed, density);
    ASSERT_EQ(56, boundary_map.getPtsCalcul().size());
}

TEST(TestBoundaryNoiseMap, triangular_polyline) {

    TYTabPoint table_points(3);
    const double z = 0.;
    table_points[0] = TYPoint(0., 0., z);
    table_points[1] = TYPoint(60., 20., z);
    table_points[2] = TYPoint(20., 70., z);

    const double density = 0.1;
    const double thickness = 10.;

    const bool closed = true;

    TYBoundaryNoiseMap boundary_map;

    // Build closed polyline...
    boundary_map.make(table_points, thickness, closed, density);
    ASSERT_EQ(19, boundary_map.getPtsCalcul().size());

    // Build open polyline...
    boundary_map.make(table_points, thickness, !closed, density);
    ASSERT_EQ(12, boundary_map.getPtsCalcul().size());
}

