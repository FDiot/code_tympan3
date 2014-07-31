/**
 * @file altimetry_file_reader_impl.h
 *
 * @brief Implementation details header for altimetry_reader.cpp
 *
 * @author Anthony Truchet <anthony.truchet@logilab.fr>
 *
 */

#ifndef TYMPAN__ALTIMETRYREADERIMPL_H__INCLUDED
#define TYMPAN__ALTIMETRYREADERIMPL_H__INCLUDED

typedef struct t_ply_ *p_ply; // Avoid including rply.h here

#include "altimetry_file_reader.h"

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
        virtual const points_array_t& points() const {return _points;};
        virtual const faces_array_t&  faces()  const {return _faces;};
        virtual void read();
        /// @}

        /// Internals methods for steps of the processing
        /// @{
        void setup_callbacks();
        void init_data();
        void read_data();
        /// @}

        /// Accessors
        /// @{
        long nvertices() const {return _nvertices;}
        long nfaces()    const {return _nfaces;}
        /// @}

        /// Methods redirected to by the callbacks
        /// @{
        bool vertex_cb(vertex_properties property, unsigned vertex_index,
                       double value);
        bool face_cb(face_properties property, unsigned face_index,
                     unsigned nproperties, int property_index,
                     double value);
        void error_cb(p_ply ply, const char *message);
        /// @}

        /// Data attributes
        /// @{
    public:
        const std::string _filename;
    protected:
        points_array_t _points;
        faces_array_t _faces;
        p_ply _ply;
        long _nvertices;
        long _nfaces;
        /// @}


    }; // AltimetryPLYReader
}; // namespace tympan

#endif // TYMPAN__ALTIMETRYREADERIMPL_H__INCLUDED
