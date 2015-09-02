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


#ifndef __TY_ANIME3DSOLVER__
#define __TY_ANIME3DSOLVER__

#include <memory>
#include <QList>

#include "Tympan/core/interfaces.h"
#include "Tympan/models/common/3d.h"
#include "Tympan/models/common/triangle.h"
#include "Tympan/models/common/acoustic_path.h"
#include "Tympan/models/solver/entities.hpp"
#include "Tympan/models/solver/config.h"
#include "Tympan/solvers/AcousticRaytracer/Tools/Logger.h"

class TYANIME3DAcousticModel;
class TYANIME3DAcousticPathFinder;
class TYANIME3DFaceSelector;
class Lancer;

/**
* \struct TYStructSurfIntersect
* Structure storing all the informations in Tympan in a format easily convertible.
* Informations idFace, idBuilding, idEtage, spectreAbsoMat, G are not mandatory.
* It is also possible to add informations to answer to a specific need for a new acoustic method
* For instance : the developer might choose to keep resistivity instead of G coefficient.
*/
struct TYStructSurfIntersect
{
    OMatrix matInv;                         //!< Inverse matrix used for the infrastructure faces
    TabPoint3D tabPoint;                    //!< Points array used during the pre-selection
    bool isEcran;                           //!< Flag to detect screen face
    bool isInfra;                           //!< Flag to detect infrastructure face
    OVector3D normal;                       //!< Surface normal vector
    QList<OTriangle> triangles;             //!< Triangles list included into the surface after triangulation
    QList<OPoint3D> realVertex;             //!< Vertex list of the surface (global coordinates)
    int idFace;                             //!< Face index
    int idBuilding;                         //!< Building index in TYSite
    int idEtage;                            //!< Floor index
    OSpectreComplex spectreAbso;            //!< Absorption spectrum
    double G;                               //!< Impedance coefficient
    tympan::AcousticMaterialBase *material; //!< Triangle material
};

/**
 * \class TYANIME3DSolver
 * \brief Associated solver to the ANIME3D method
 */
class TYANIME3DSolver : public SolverInterface
{
public:
    /**
    * \fn TYANIME3DSolver()
    * \brief Constructor of ANIME3DSolver
    */
    TYANIME3DSolver();

    /*!
    * \fn ~TYANIME3DSolver()
    * \brief Destructor of ANIME3DSolver
    */
    ~TYANIME3DSolver();

    /*!
    * \fn bool solve(const AcousticProblemModel&, AcousticResultModel&)
    * \brief Resolution with the ANIME3D method
    */
    virtual bool solve(const tympan::AcousticProblemModel& aproblem,
                       tympan::AcousticResultModel& aresult,
                       tympan::LPSolverConfiguration configuration);

    /*!
    * \fn void init()
    * \brief Initialize some data structures used by ANIME3D
    */
    void init();


    /*!
     * \fn virtual void purge();
     * \brief Clean memory after being used in a calculation
     */
    virtual void purge();

protected:

    TYStructSurfIntersect* _tabPolygon; //!< Array containing all the informations relative to a site geometry and associated material to each face

    size_t _tabPolygonSize; //!< Array size of _tabPolygon

    //Logger logger;
    //FILE logs;

    // List of sources used by the solver
    //TYTabSourcePonctuelleGeoNode _tabSources;

    // List of receptors used by the solver
    //TYTabPointCalculGeoNode _tabRecepteurs;

    tympan::source_pool_t all_sources;
    tympan::receptor_pool_t all_receptors;

private:
   std::unique_ptr<AtmosphericConditions> _pAtmos;
};

#endif // __TY_ANIME3DSOLVER__
