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

/*!
* \file Lancer.h
* \author Projet_Tympan
*/

#ifndef __LANCER_H
#define __LANCER_H

#include "Tympan/models/common/mathlib.h"
#include "Step.h"


using namespace std;

class RayCourb;
class Sampler;
class meteo;

/*! \class Lancer
* \brief Describes analytical ray curve tracing
*/
class Lancer
{
public:
	/// Constructor
    Lancer();
    /// Copy constructor
    Lancer(Lancer& L);
    /// Destructor
    ~Lancer();

    //--------------------------------------------------------------------------------------------------------//
    // Member functions :
    //--------------------------------------------------------------------------------------------------------//

    /// Set the pointer to the weather object
    void setMeteo(const meteo* Meteo) { _weather = const_cast<meteo*>(Meteo); }

    /// Clear the memory of MatRes object
    void purgeMatRes();

    /// Clear all the arrays
    void clear() { sources.clear(); recepteurs.clear(); _plan.clear(); temps.clear(); purgeMatRes(); }

    /// Set the rays number to launch
    void setNbRay(const unsigned int& nb) {nbRay = nb;}

    /// Change the maximal propagation time
    void setTMax(const double& TmpMax) { TMax = (decimal)TmpMax; }

    /// Set the maximal distance
    void setDMax(const double& DistMax) { dmax = (decimal)DistMax; }

    /// Set the discretization step
    void setTimeStep(const decimal& tt) { h = tt; }

    /// Add a triangle to the geometry
    void setTriangle(vec3* triangle) { _plan.push_back(triangle); }

    /// Compute and return the maximal distance between sources and receptors
    decimal distance_max();

    /// Build the time vector (different times solved)
    void createTemps();

    /*!
    * \brief Function to define the eikonal equation
    * \param y0 Initial vector
    * \return A point of the ray under the form of a Step (position, normal)
    */
    Step EqRay(const Step& y0);

    /*!
    * \brief Compute the intersection point between a plane and a line
    * \param [in] S Source
    * \param [in] R Receptor
    * \param [in] A Plane: array containing the 3 vertices of the face (all the vertices should be coplanar)
    * \param [out] reflexion True if intersection has been founded, False if not
    * \param [in] nExt_plan Exterior normal relative to the plane
    * \param [in] SR Vector between source and receptor
    * \return Return the intersection point between the plane A and the SR line.
    */
    vec3 valideIntersection(const vec3& S, const vec3& R, const vec3* A, int& reflexion, const vec3& nExt_plan, const vec3& SR);

    void intersection(const unsigned int& timer, RayCourb& current, Step& Y_t0, Step& Y_t1);

    /*!
    * \brief Fourth order Runge-Kutta algorithm
    * \param y0 Initial vector
    * \return Return a ray (list of points)
    */
    RayCourb RK4(const Step& y0);

    /// Fill the MatRes matrix containing the ray curves
    void RemplirMat();

    /// Run the calculation
    void run() { createTemps(); RemplirMat(); }

    /*!
     * \brief Read the file containing the starting angles of the rays
     * \param tableau_norm Vector of the normals
     */
    void loadRayFile(vector<vec3>& tableau_norm);

    /// Set a pointer to the ray generator
    void setSampler(Sampler* generator) { _sampler = generator; }

    /// Get ray generator for modification
    Sampler* getSampler() { return _sampler; }

    /// Set how rays will be created (launch type)
    void setLaunchType(const unsigned int& launchType) { _launchType = launchType; init_sampler(); }

    /// Add a source into the sources list
    void addSource(const vec3& source) { sources.push_back(source); }


private :
    /// Initialize ray generator
    void init_sampler();

    /// Save rays to a file
    void save();

    /// Compute next step taking account of the weather
    Step compute_next_step(const Step& current_step);


public :
    vector<vec3> sources;           //!< Sources vector
    vector<vec3> recepteurs;        //!< Receptors vector
    vector<vec3*> _plan;            //!< List of objects defined by 3 points
    meteo* _weather;                //!< Pointer to weather
    Sampler* _sampler;              //!< Pointer to a ray generator

    decimal h;                      //!< Discretization step
    decimal TMax;                   //!< Maximal propagation time
    vector<decimal> temps;          //!< [0:h:TMax] Vector containing the different times solved
    decimal dmax;                   //!< Maximal distance traveled by the rays
    decimal initialAngleTheta;      //!< Initial shot angle according theta
    decimal finalAngleTheta;        //!< Final shot angle according theta
    decimal initialAnglePhi;        //!< Initial shot angle according phi
    decimal finalAnglePhi;          //!< Final shot angle according phi

    unsigned int nbRay;             //!< Launched rays number
    unsigned int _launchType;       //!< Launch type with 1:horizontal / 2:vertical / 3:spherical / 4:file
    bool wantOutFile;               //!< True if an output file is wanted
    string ray_fileName;            //!< Filename of file containing angles of rays
    string out_fileName;            //!< Filename of the output file

    vector<RayCourb*> MatRes;       //!< Array containing the resulting rays for each source
};

/*!
* \brief Compute the shooting angle ray in order to travel the given minimal distance
* \param a Sound speed gradient
* \param c Ground sound speed
* \param d Minimal distance to travel
* \param h Source height
* \return Shot angle of the ray.
*/
decimal angle_depart(const decimal& a, const decimal& c, const decimal& d, const decimal& h);

#endif // __LANCER_H
