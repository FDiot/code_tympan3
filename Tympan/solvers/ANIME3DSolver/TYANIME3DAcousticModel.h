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

#ifndef __TYANIME3DACOUSTICMODEL__
#define __TYANIME3DACOUSTICMODEL__

#include "Tympan/core/interfaces.h"
#include "Tympan/solvers/ANIME3DSolver/TYANIME3DSolver.h"

// For the use of standard library STL.

///Type for complex value
typedef std::complex<double> OComplex;

///Type for array of double
typedef std::vector<double> OTabDouble;

///Type for array of complex
typedef std::vector<OComplex> OTabComplex;

///Type for array of complex spectrum
typedef std::vector<OSpectreComplex> OTabSpectreComplex;

///Type for array of points
typedef std::vector<OPoint3D> OTabPoints;

///Type for array of ray
typedef std::vector<Ray*> TabRays;

class OBox2;
class AcousticProblemModel;
class AtmosphericConditions;

/**
 * \class TYANIME3DAcousticModel
 * \brief Acoustic model of ANIME3D method
 */
class TYANIME3DAcousticModel : public AcousticModelInterface
{
public:
    /**
     * \brief Constructor
     * \param tabRayons Array of acoustic paths
     * \param tabStruct Array containing all the informations relative to a site geometry and associated material to each face
     * \param aproblem Acoustic problem
     * \param atmos Atmospheric conditions object
     */
    TYANIME3DAcousticModel( tab_acoustic_path& tabRayons, 
                            TYStructSurfIntersect* tabStruct,
                            const tympan::AcousticProblemModel& aproblem,
                            AtmosphericConditions& atmos);

    /// Destructor
    virtual ~TYANIME3DAcousticModel();

    /**
     * \fn ComputeAcousticModel()
     * \brief Complete calculation of the acoustic model ANIME3D
     */
    OTab2DSpectreComplex ComputeAcousticModel();

    /**
     * \fn void ComputePressionAcoustEff()
     * \brief Calculation of the effective acoustic pressure per ray and per frequency
     */
    void ComputePressionAcoustEff();

    /**
     *\fn  void ComputePressionAcoustTotalLevel()
     *\brief Calculation of the total quadratic pressure for a source/receptor - calculation with partial coherence form
     */
    OTab2DSpectreComplex ComputePressionAcoustTotalLevel();


protected:

    /**
     * \fn void ComputeAbsAtm();
     * \brief Calculation of the atmospheric absorption
     */
    void ComputeAbsAtm();

    /**
     * \fn void ComputeAbsRefl(TYCalcul & calcul, TYSiteNode & site)
     * \brief Calculation of the absorption by reflection
     */
    void ComputeAbsRefl();

    // * \fn OBox2 ComputeFresnelArea(double angle, OPoint3D Pprec, OPoint3D Prefl, OPoint3D Psuiv, int rayNbr, int reflIndice)
    // * \brief Calculation of triangles in the Fresnel area
    // */
    //OBox2 ComputeFresnelArea(double angle, OPoint3D Pprec, OPoint3D Prefl, OPoint3D Psuiv, int rayNbr, int reflIndice);

    // * \fn OTabDouble ComputeFresnelWeighting(double angle, OPoint3D Pprec, OPoint3D Prefl, OPoint3D Psuiv, int rayNbr, int reflIndice, TYTabPoint3D& triangleCentre);
    // * \brief Calculation Fresnel weighting associated to the Fresnel area
    // */
    //OTabDouble ComputeFresnelWeighting(double angle, OPoint3D Pprec, OPoint3D Prefl, OPoint3D Psuiv, int rayNbr, int reflIndice, TYTabPoint3D& triangleCentre);

    // * \fn std::vector<OTriangle> ComputeTriangulation(const TYTabPoint& points, const double& delaunay);
    // * \brief Computes triangulation inside the bounding box
    // * \brief Needs 4 points which are the box corners' projection on the ground
    // */
    //std::vector<OTriangle> ComputeTriangulation(const TYTabPoint& points, const double& delaunay);

    /**
    * \fn void ComputeAbsDiff()
    * \brief Calculation of absorption by diffraction
    */
    void ComputeAbsDiff();



private :
    /*
     * \fn OSpectre computeFc(const double& dd, const double& dr);
     * \brief compute transition frequency spectrum fc
     */
    OSpectre computeFc(const double& dd, const double& dr);

protected :

    // Test : vector de triangles
//    std::vector<OTriangle> _oTriangles;
    //std::vector<OPoint3D>& _oVertex;
//    QList<OTriangle> triangles;

    /// Array of effective acoustic pressure per ray
    OTabSpectreComplex _pressAcoustEff;

    /// Array of atmospheric absorptions per ray
    OTabSpectreComplex _absAtm;

    /// Array of absorptions by reflection per rayon
    OTabSpectreComplex _absRefl;

    /// Array of absorptions by diffraction per rayon
    OTabSpectreComplex _absDiff;

    /// Rays number
    int _nbRays;

    /// Array of all TYMPAN rays
    tab_acoustic_path& _tabTYRays;

    TYStructSurfIntersect* _tabSurfIntersect; //!< Array containing all the informations relative to a site geometry and associated material to each face

    // la liste des triangles de la topo
    // TYTabLPPolygon _listeTrianglesBox;

    /// Sound speed
    double _c;

    /// Wave number
    OSpectre _K;

    /// Wavelength
    OSpectre _lambda;

    /// Flag to take into account or not the Fresnel area
    bool _useFresnelArea;

private:
    /// Problem data
    const tympan::AcousticProblemModel& _aproblem;
    /// Atmospheric conditions
    AtmosphericConditions& _atmos;
};


#endif // __TYANIME3DACOUSTICMODEL__
