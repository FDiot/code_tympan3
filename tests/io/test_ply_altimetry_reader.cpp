//! \test test_ply_altimetry_reader
#include "gtest/gtest.h"

#include "Tympan/models/business/altimetry_file_reader.h"
#include "Tympan/models/business/altimetry_file_reader_impl.h"

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


TEST(TestAltimetryReader, trivial_setup_callbacks)
{
    // PLY files to read.
    std::string alti_file = tympan::path_to_test_data("trivial.ply");
    AltimetryPLYReader reader(alti_file);

    reader.setup_callbacks();

    EXPECT_EQ(3, reader.nvertices());
    EXPECT_GT(3, reader.points().capacity());
    EXPECT_EQ(1, reader.nfaces());
    EXPECT_GT(1, reader.faces().capacity());
}

TEST(TestAltimetryReader, vertex_cb)
{
    // PLY files to read.
    std::string alti_file = tympan::path_to_test_data("trivial.ply");
    AltimetryPLYReader reader(alti_file);
    const OPoint3D p(1.0, 2.0, 3.0);
    ASSERT_EQ(0, reader.points().size());
    bool ok;

    ok = reader.vertex_cb(AltimetryPLYReader::X, 0, p._x);
    ASSERT_TRUE(ok);
    EXPECT_EQ(1, reader.points().size());

    ok =  reader.vertex_cb(AltimetryPLYReader::Y, 0, p._y);
    ok &= reader.vertex_cb(AltimetryPLYReader::Z, 0, p._z);
    ASSERT_TRUE(ok);
    EXPECT_EQ(1, reader.points().size());
    EXPECT_EQ(p, reader.points().back());
}

TEST(TestAltimetryReader, face_cb)
{
    // PLY files to read.
    std::string alti_file = tympan::path_to_test_data("trivial.ply");
    AltimetryPLYReader reader(alti_file);
    const OPoint3D points[3] = {
        OPoint3D(0.0, 0.0, 0.0),
        OPoint3D(1.0, 0.0, 0.0),
        OPoint3D(0.0, 2.0, 0.0)};
    OTriangle triangle(0, 1, 2); // Expected triangle defined by indices

    bool ok = true;
    // Simulate input of the points
    unsigned vertex_index = 0;
    for(const auto& p : points)
    {
        for(unsigned i=0; i<3; ++i)
            ok &= reader.vertex_cb((AltimetryPLYReader::vertex_properties)i,
                                   vertex_index, p._value[i]);
        triangle.vertex(vertex_index) = p; // Build the expected triangle vertices
        ++vertex_index;
    }
    ASSERT_EQ(3, reader.points().size());

    ok = true;
    ok &= reader.face_cb(AltimetryPLYReader::VertexIndices, 0, 3, -1, 3.0);
    ASSERT_TRUE(ok);
    ASSERT_EQ(1, reader.faces().size());
    for(unsigned i=0; i<3; ++i)
        ok &= reader.face_cb(AltimetryPLYReader::VertexIndices, 0, 3, i, triangle.index(i));
    ASSERT_EQ(1, reader.faces().size());
    EXPECT_EQ(triangle, reader.faces().back());
}
