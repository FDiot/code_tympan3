/*
 * Copyright (C) <2012> <EDF-R&D> <FRANCE>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#ifndef __TYINPROFACESELECTOR__
#define __TYINPROFACESELECTOR__

class AcousticProblemModel;

/**
 * \class TYANIME3DFaceSelector
 * \brief Building class of the faces list used by the ray tracer and the acoustic solver
 */
class TYANIME3DFaceSelector
{
public:
	/// Constructor
    TYANIME3DFaceSelector( const tympan::AcousticProblemModel& aproblem_ );
    /// Destructor
    virtual ~TYANIME3DFaceSelector();

    /*!
     * \fn bool exec(TYStructSurfIntersect* tabPolygon, unsigned int& tabPolygonSize);
     * \brief Build list of faces
     * \param tabPolygon Array containing the polygons
     * \param tabPolygonSize Size of the tabPolygon array
     * \return true
     */
    bool exec(TYStructSurfIntersect *&tabPolygon, size_t& tabPolygonSize);

private :
    bool buildCalcStruct(TYStructSurfIntersect *&tabPolygon, size_t& tabPolygonSize);

protected :
    const tympan::AcousticProblemModel& aproblem; //!< Reference to the acoustic problem
};

#endif // __TYANIME3DFACESELECTOR__
