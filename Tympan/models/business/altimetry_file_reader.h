/**
 * @file altimetry_file_reader.h
 *
 * @brief Provides for reading back an altimetry written in a file
 *
 * @author Anthony Truchet <anthony.truchet@logilab.fr>
 *
 */

#ifndef TYMPAN__ALTIMETRYREADER_H__INCLUDED
#define TYMPAN__ALTIMETRYREADER_H__INCLUDED

#include <memory>
#include <vector>
#include <string>

#include "Tympan/core/exceptions.h"
#include "Tympan/models/common/3d.h"
#include "Tympan/models/common/triangle.h"


namespace tympan {

    struct mesh_io_error: /*virtual*/ std::runtime_error, virtual tympan::exception
    {
        mesh_io_error(const std::string& desc) : std::runtime_error(desc) {};
    };


    class IMeshReader
    {
    public:
        /// @brief Type of the points array returned
        typedef std::vector<OPoint3D> points_array_t;
        /// @brief Type of the faces array returned
        typedef std::vector<OTriangle> faces_array_t;

        /**
         * @brief Getter for the points read
         */
        virtual const points_array_t& points() const = 0;

        /**
         * @brief Getter for the faces read
         */
        virtual const faces_array_t& faces() const = 0;

        /**
         * @brief read the file whose name was given at reader's construction time
         */
        virtual void read() = 0;
    };

    /// @ Makes a reader for the file name given as argument
    std::unique_ptr<IMeshReader>
    make_altimetry_ply_reader(const std::string filename);

}; // namespace tympan

#endif // TYMPAN__ALTIMETRYREADER_H__INCLUDED
