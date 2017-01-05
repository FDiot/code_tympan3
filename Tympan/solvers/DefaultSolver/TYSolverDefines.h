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

/*
 *
 *
 *
 *
 */

#ifndef __TY_SOLVERDEFINES__
#define __TY_SOLVERDEFINES__

#include <deque>

#include "Tympan/models/common/3d.h"
#include "Tympan/models/solver/entities.hpp"
#include "Tympan/models/common/3d.h"

/**
 * \struct TYStructSurfIntersect
 * \brief Describe surface intersections
 */
struct TYStructSurfIntersect
{
    string volume_id;    //!< Volume id
    TabPoint3D tabPoint; //!< Array of points used for the preselection
    OVector3D normal;    //!< Normal vector to a face
    bool isEcran;        //!< Flag to define if it is a screen
    tympan::AcousticMaterialBase* material; //!< Reference to a material

    /**
     * \brief Detect if a face is on a infrastructure (has a material)
     */
    bool is_infra() const 
    { 
        tympan::AcousticBuildingMaterial* pMat = dynamic_cast<tympan::AcousticBuildingMaterial*>(material);

        return (pMat == nullptr) ? false : true; 
    }
};

/**
 * \brief Data structure for intersections
 */
struct TYSIntersection
{
    OSegment3D segInter[2]; //!< Intersection segment between face and vertical plane ([0]) and horizontal plane ([1])
    bool noIntersect;       //!< Flag to indicate that the face should not be tested for intersection
    bool bIntersect[2];     //!< Flag to indicate the face cuts vertical plane ([0]) or horizontal plane ([1])
    bool isEcran;           //!< Flag to define if is a screen face
    bool isInfra;           //!< Flag to define if is a infrastructure face
    tympan::AcousticMaterialBase* material; //!< Pointer to a material
};

/**
 * \brief Structure to describe a plan defined with 3 points
 */
typedef struct
{
    OPoint3D pt1;
    OPoint3D pt2;
    OPoint3D pt3;
} TYSPlan;

/**
 * \brief Structure containing a segment and a spectrum
 */
typedef struct
{
    OSegment3D _segment;
    OSpectre* _spectreAtt;
} TYStructSegLPSp;

//!< Array of struct contening a segment and a spectrum
typedef std::deque<TYStructSegLPSp> TYTabStructSegLPSp;

//!< Rayon du cercle utilise dans le calcul de l'attenuation de la vegetation.
#define TY_EDF_FOREST_RAYON 5000
//!< Limite pour la prise en compte de l'effet de diffraction (=0.1dB).
#define TY_EDF_DIFFRACTION_LIMIT 1.023293

#endif // __TY_SOLVERDEFINES__
