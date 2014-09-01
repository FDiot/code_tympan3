/**
 * @file test_rply.cpp
 *
 * @brief Test the build, link and use of the rply library
 *
 * Adapted from the example etc/dump.c provided with the library
 *
 * @author Anthony Truchet <anthony.truchet@logilab.fr>
 */

#include <cstdio>
#include <cassert>
#include <string>
#include <deque>

using std::string;
using std::deque;

#include "gtest/gtest.h"
#include "TympanTestsConfig.h"

#include "rply.h"

struct spy_t {
    deque<double> vertex[3];
    deque<unsigned> face[3];
};

static int vertex_cb(p_ply_argument argument) {
    long call_id;
    void* pdata;
    spy_t *spy;
    double value;
    ply_get_argument_user_data(argument, &pdata, &call_id);
    spy = (spy_t*) pdata;
    assert(0<= call_id && call_id < 3);
    value = ply_get_argument_value(argument);
    spy->vertex[call_id].push_back(value);
    return 1;
}

static int face_cb(p_ply_argument argument) {
    long call_id;
    void* pdata;
    spy_t *spy;
    double value;
    long length, value_index;
    ply_get_argument_user_data(argument, &pdata, &call_id);
    spy = (spy_t*) pdata;
    ply_get_argument_property(argument, NULL, &length, &value_index);
    switch (value_index) {
    case 0:
    case 1:
    case 2:
        value = ply_get_argument_value(argument);
        spy->face[value_index].push_back(value);
        break;
    default:
        break;
    }
    return 1;
}

TEST(RPLY, read_trivial_file)
{
    std::string data_file_name = tympan::path_to_test_data("trivial_material.ply");
    const long nvertices = 3, ntriangles= 1;
    spy_t spy;

    p_ply ply = ply_open(data_file_name.c_str(), NULL, 0, NULL);
    ASSERT_TRUE(ply) << "Could not open data file " << data_file_name;
    ASSERT_TRUE(ply_read_header(ply)) << "Invalid header for data file" << data_file_name;

    EXPECT_EQ(nvertices,
              ply_set_read_cb(ply, "vertex", "x", vertex_cb, &spy, 0));
    EXPECT_EQ(nvertices,
              ply_set_read_cb(ply, "vertex", "y", vertex_cb, &spy, 1));
    EXPECT_EQ(nvertices,
              ply_set_read_cb(ply, "vertex", "z", vertex_cb, &spy, 2));
    EXPECT_EQ(ntriangles,
              ply_set_read_cb(ply, "face", "vertex_indices", face_cb, &spy, 0));

    ASSERT_TRUE(ply_read(ply)) << "Invalid contents for data file" << data_file_name;
    ply_close(ply);

    for(unsigned i=0; i<3; ++i)
        EXPECT_EQ(ntriangles, spy.face[i].size());
    for(unsigned i=0; i<3; ++i)
        EXPECT_EQ(nvertices, spy.vertex[i].size());
}
