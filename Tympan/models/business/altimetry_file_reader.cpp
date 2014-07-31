/**
 * @file altimetry_file_reader.cpp
 *
 * @brief Provides for reading back an altimetry written in a file
 *
 * @author Anthony Truchet <anthony.truchet@logilab.fr>
 *
 */
#include <cassert>

#include <boost/exception/errinfo_file_name.hpp>
#include <boost/exception/errinfo_at_line.hpp>
#include <boost/exception/errinfo_errno.hpp>

#include "rply.h"

#include "altimetry_file_reader_impl.h"

// section with C linkage for RPLY callbacks
extern "C" {
    static int vertex_cb(p_ply_argument argument);
    static int face_cb(p_ply_argument argument);
} // extern C



namespace tympan {


    //========== Implementation of free functions ==========

    std::unique_ptr<IMeshReader>
    make_altimetry_ply_reader(const std::string filename)
    {
        return std::unique_ptr<IMeshReader>(new AltimetryPLYReader(filename));
    } //make_altimetry_ply_reader

    //========== Implementation of AltimetryPLYReader methods ==========

    AltimetryPLYReader::AltimetryPLYReader(const std::string filename) :
        _filename(filename),
        _ply(nullptr), _nvertices(-1), _nfaces(-1)
    {
        // Opens the PLY file, no error callback is given
        _ply = ply_open(_filename.c_str(), NULL, 0, NULL);
        if(_ply==NULL)
            throw mesh_io_error("Opening PLY file")
                << tympan_source_loc
                << boost::errinfo_file_name(filename);
        // Reads the header
        if(!ply_read_header(_ply))
            throw mesh_io_error("Reading the PLY header")
                << tympan_source_loc
                << boost::errinfo_file_name(filename);
    }

    AltimetryPLYReader::~AltimetryPLYReader()
    {
        ply_close(_ply);
    }

    void AltimetryPLYReader::read()
    {
        setup_callbacks();
        init_data();
        read_data();
    }

    void AltimetryPLYReader::setup_callbacks()
    {
        long n;
        assert(_nvertices==-1 && _nfaces==-1);
        n = ply_set_read_cb(_ply, "vertex", "x", ::vertex_cb, this, X);
        _nvertices = n;
        n = ply_set_read_cb(_ply, "vertex", "y", ::vertex_cb, this, Y);
        assert(n==_nvertices);
        n = ply_set_read_cb(_ply, "vertex", "z", ::vertex_cb, this, Z);
        assert(n==_nvertices);
        _nfaces = ply_set_read_cb(_ply, "face", "vertex_indices",
                                  ::face_cb, this, VertexIndices);
    }

    void AltimetryPLYReader::init_data()
    {
        _points.reserve(_nvertices);
        _faces.reserve(_nfaces);
    }

    void AltimetryPLYReader::read_data()
    {
        if(!ply_read(_ply))
            throw mesh_io_error("Reading the PLY data")
                << tympan_source_loc
                << boost::errinfo_file_name(_filename);
    }

    bool  AltimetryPLYReader::vertex_cb(vertex_properties property, unsigned vertex_index,
                                        double value)
    {
        return true;
    }

    bool  AltimetryPLYReader::face_cb(face_properties property, unsigned face_index,
                                      unsigned nproperties, int property_index,
                                      double value)
    {
        return true;
    }

}; // namespace tympan

//========== C callback must be defined outside the namespace tympan ==========

using namespace tympan;

/**
 * @brief macro which extract all relevant callback information form RPLY
 */
#define DECLARE_AND_FETCH_PLY_CALLBACK_INFO                             \
    long idata;                                                         \
    void* pdata;                                                        \
    tympan::AltimetryPLYReader *p_reader;                               \
    long instance_index;                                                \
    p_ply_element element;                                              \
    double value;                                                       \
    ply_get_argument_user_data(argument, &pdata, &idata);               \
    p_reader = (tympan::AltimetryPLYReader*) pdata;                     \
    ply_get_argument_element(argument, &element, &instance_index);      \
    value = ply_get_argument_value(argument);
// END OF DECLARE_AND_FETCH_PLY_CALLBACK_INFO

/// @brief C callback called by RPLY :Extracts information from the PLY
/// structure and delegate to vertex_cb method
static int vertex_cb(p_ply_argument argument) {
    DECLARE_AND_FETCH_PLY_CALLBACK_INFO
    AltimetryPLYReader::vertex_properties vertex_property;
    assert(0 <= idata && idata < AltimetryPLYReader::NUM_vertex_properties);
    vertex_property = (AltimetryPLYReader::vertex_properties) idata;
    return p_reader->vertex_cb(vertex_property, instance_index, value);
}

/// @brief C callback called by RPLY :Extracts information from the PLY
/// structure and delegate to face_cb method
static int face_cb(p_ply_argument argument) {
    DECLARE_AND_FETCH_PLY_CALLBACK_INFO
    AltimetryPLYReader::face_properties face_property;
    assert(0 <= idata && idata < AltimetryPLYReader::NUM_face_properties);
    face_property = (AltimetryPLYReader::face_properties) idata;
    // Access to property list
    long length, value_index;
    p_ply_property property;
    ply_get_argument_property(argument, &property, &length, &value_index);
    return p_reader->face_cb(face_property, instance_index,
                             length, value_index, value);
}
