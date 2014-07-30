//! \test test_ply_altimetry_reader
#include "gtest/gtest.h"

#include "Tympan/models/business/altimetry_file_reader.h"

#include "TympanTestsConfig.hpp"

using namespace tympan;

TEST(TestAltimetryReader, instanciation)
{
    // PLY files to read.
    std::string alti_file = tympan::path_to_test_data("trivial.ply");

    std::unique_ptr<IMeshReader> p_reader = make_altimetry_ply_reader(alti_file);
    EXPECT_TRUE(p_reader != nullptr);

    ASSERT_THROW(make_altimetry_ply_reader("nonexistent.ply"),
                 tympan::mesh_io_error);
}


TEST(TestAltimetryReader, trivial_read)
{
    // PLY files to read.
    std::string alti_file = tympan::path_to_test_data("trivial.ply");

    std::unique_ptr<IMeshReader> p_reader = make_altimetry_ply_reader(alti_file);
    ASSERT_TRUE(p_reader != nullptr);
}
