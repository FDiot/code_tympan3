/**
 * @file altimetry_file_reader.cpp
 *
 * @brief Provides for reading back an altimetry written in a file
 *
 * @author Anthony Truchet <anthony.truchet@logilab.fr>
 *
 */

#include "altimetry_file_reader.h"

#include "Tympan/core/exceptions.h"

namespace tympan {

    /**
     * @brief read an Altimetry from a PLY file.
     */
    class AltimetryPLYReader:
        public IMeshReader
    {
    public:
        AltimetryPLYReader(const std::string filename) :
            _filename(filename) {}

        /// Implements interface \c IMeshReader
        /// @{
        virtual const points_array_t& points() {return _points;};
        virtual const faces_array_t&  faces()  {return _faces;};
        virtual void read();
        /// @}

    protected:
        points_array_t _points;
        faces_array_t _faces;
        std::string _filename;
    }; // class AltimetryPLYReader

    std::unique_ptr<IMeshReader>
    make_altimetry_ply_reader(const std::string filename)
    {
        return std::unique_ptr<IMeshReader>(new AltimetryPLYReader(filename));
    } //make_altimetry_ply_reader


    void AltimetryPLYReader::read()
    {
        // XXX TODO
    } // AltimetryPLYReader::read()

}; // namespace tympan
