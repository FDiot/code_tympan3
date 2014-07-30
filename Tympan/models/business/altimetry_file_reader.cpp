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
    static int vertex_cb(p_ply_argument argument){return 1;};
    static int face_cb(p_ply_argument argument){return 1;};
} // extern C

namespace tympan {

    /**
     * @brief read an Altimetry from a PLY file.
     */
    class AltimetryPLYReader:
        public IMeshReader
    {
    public:
        AltimetryPLYReader(const std::string filename);
        virtual ~AltimetryPLYReader();

        /// Implements interface \c IMeshReader
        /// @{
        virtual const points_array_t& points() {return _points;};
        virtual const faces_array_t&  faces()  {return _faces;};
        virtual void read();
        /// @}

    private: // Data attributes
        points_array_t _points;
        faces_array_t _faces;
        std::string _filename;
        p_ply _ply;

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
        // XXX TODO
    }

}; // namespace tympan
