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
    static void error_cb(p_ply ply, const char *message);
} // extern C


static const double NaN = std::numeric_limits<double>::quiet_NaN();

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
        _ply(nullptr), _nvertices(-1), _nfaces(-1), _nmaterials(-1)
    {
        // Opens the PLY file, no error callback is given
        _ply = ply_open(_filename.c_str(), ::error_cb, 0 /*unused*/, this);
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
        build_material_by_face();
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
        _nmaterials=1; //XXX Stub
    }

    void AltimetryPLYReader::init_data()
    {
        _points.reserve(_nvertices);
        _faces.reserve(_nfaces);
        _material_indices.reserve(_nfaces);
        _materials.push_back("XXX STUB");
    }

    void AltimetryPLYReader::read_data()
    {
        if(!ply_read(_ply))
            throw mesh_io_error("Reading the PLY data")
                << tympan_source_loc
                << boost::errinfo_file_name(_filename);
    }

    void AltimetryPLYReader::build_material_by_face()
    {
        assert(_material_by_face.empty());
        _material_by_face.reserve(_nfaces);
        for(unsigned material_index : _material_indices)
        {
            material_index=0; //XXX Stub
            _material_by_face.push_back(_materials[material_index]);
        }
    }

    bool  AltimetryPLYReader::vertex_cb(vertex_properties property, unsigned vertex_index,
                                        double value)
    {
        switch(property)
        {
        case X:
            // Insert a new point
            _points.push_back(OPoint3D(NaN, NaN, NaN));
            // Deliberate fall-through
        case Y:
        case Z:
            // Store the coordinate
            // NB the numerical value of ``property`` is the index of the vertex
            assert(_points.size()-1 == vertex_index); // Index consistency
            _points.back()._value[(unsigned)property] = value;
            break;
        default:
            return false;
        }
        return true;
    }

    bool AltimetryPLYReader::face_cb(face_properties property, unsigned face_index,
                                     unsigned nproperties, int property_index,
                                     double value)
    {
        switch(property)
        {
        case VertexIndices:
            // Reading vertices index for the face, expected to be a triangle
            if (nproperties != 3) {
                // Should probably NOT raise a exception from within a C callback
                // TODO properly report, e.g. with an error status in the reader
                return false;
            }
            if (property_index==-1) { // RPLY is giving us the length
                assert(value==3.0);   // Value should be equal to length
                _faces.push_back(OTriangle(-1, -1, -1));
                // Ensure _material_indices and _faces have same size
                _material_indices.push_back(-1);
            }
            else {
                assert(property_index<3);
                assert(_faces.size()-1 == face_index); // Index consistency
                const unsigned vertex_index = value;
                OTriangle& triangle = _faces.back();
                triangle.index(property_index) = vertex_index;
                // Ensure consistency of the OTriangle redundant representation
                triangle.vertex(property_index) = _points[vertex_index];
            }
            break;
        case MaterialIndex:
            assert(_material_indices.back()==-1);
            assert(_material_indices.size()-1 == face_index); // Index consistency

            _material_indices.back() = (unsigned) value;
            break;
        default:
            return false;
        }
        return true;
    }

    void AltimetryPLYReader::error_cb(p_ply ply, const char *message)
    {
        assert(_ply==NULL || ply==_ply);
        throw mesh_io_error(message)
            << tympan_source_loc
            << boost::errinfo_file_name(_filename);
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

/// @brief C callback called by RPLY in case of erro
void error_cb(p_ply ply, const char *message)
{
    long idata;
    void* pdata;
    tympan::AltimetryPLYReader *p_reader;
    ply_get_ply_user_data(ply, &pdata, &idata);
    p_reader = (tympan::AltimetryPLYReader*) pdata;
    p_reader->error_cb(ply, message);
}
