/**
 * @file altimetry_file_reader.cpp
 *
 * @brief Provides for reading back an altimetry written in a file
 *
 * @author Anthony Truchet <anthony.truchet@logilab.fr>
 *
 */

#include <boost/exception/errinfo_file_name.hpp>
#include <boost/exception/errinfo_at_line.hpp>
#include <boost/exception/errinfo_errno.hpp>

#include "rply.h"

#include "altimetry_file_reader.h"

// section with C linkage for RPLY callbacks
extern "C" {
    static int vertex_cb(p_ply_argument argument);
    static int face_cb(p_ply_argument argument);
} // extern C

namespace tympan {

    /**
     * @brief read an Altimetry from a PLY file.
     */
    class AltimetryPLYReader:
        public IMeshReader
    {
    public:
        typedef enum {X, Y, Z, NUM_vertex_properties} vertex_properties;
        typedef enum {VertexIndices, NUM_face_properties} face_properties;

        AltimetryPLYReader(const std::string filename);
        virtual ~AltimetryPLYReader();


        /// Implements interface \c IMeshReader
        /// @{
        virtual const points_array_t& points() {return _points;};
        virtual const faces_array_t&  faces()  {return _faces;};
        virtual void read();
        /// @}

        /// Data attributes
        /// @{
    public:
        const std::string _filename;
    protected:
        points_array_t _points;
        faces_array_t _faces;
        p_ply _ply;
        /// @}

    }; // AltimetryPLYReader

    //========== Implementation of free functions ==========

    std::unique_ptr<IMeshReader>
    make_altimetry_ply_reader(const std::string filename)
    {
        return std::unique_ptr<IMeshReader>(new AltimetryPLYReader(filename));
    } //make_altimetry_ply_reader

    //========== Implementation of AltimetryPLYReader methods ==========

    AltimetryPLYReader::AltimetryPLYReader(const std::string filename) :
        _filename(filename)
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
        if(!ply_read(_ply))
            throw mesh_io_error("Reading the PLY data")
                << tympan_source_loc
                << boost::errinfo_file_name(_filename);
    }

}; // namespace tympan

//========== C callback must be defined outside the namespace ==========

using namespace tympan;

/**
 * @brief macro which extract all relevant callback information form RPLY
 */
#define DECLARE_AND_FETCH_PLY_CALLBACK_INFO                     \
    long idata;                                                 \
    void* pdata;                                                \
    tympan::AltimetryPLYReader *p_reader;                       \
    long instance_index;                                        \
    p_ply_element element;                                      \
    double value;                                               \
    ply_get_argument_user_data(argument, &pdata, &idata);       \
    p_reader = (tympan::AltimetryPLYReader*) pdata;             \
    ply_get_argument_element(argument, &element, &instance_index);\
    value = ply_get_argument_value(argument);
// END OF DECLARE_AND_FETCH_PLY_CALLBACK_INFO

static int vertex_cb(p_ply_argument argument) {
    DECLARE_AND_FETCH_PLY_CALLBACK_INFO
    AltimetryPLYReader::vertex_properties vertex_property;
    assert(0 <= idata && idata < AltimetryPLYReader::NUM_vertex_properties);
    vertex_property = (AltimetryPLYReader::vertex_properties) idata;
    // TODO Dispatch
    return 1;
}

static int face_cb(p_ply_argument argument) {
    DECLARE_AND_FETCH_PLY_CALLBACK_INFO
    AltimetryPLYReader::face_properties face_property;
    assert(0 <= idata && idata < AltimetryPLYReader::NUM_face_properties);
    face_property = (AltimetryPLYReader::face_properties) idata;
    // TODO Dispatch
    return 1;
}
