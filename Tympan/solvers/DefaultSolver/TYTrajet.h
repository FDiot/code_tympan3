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

#ifndef __TY_TRAJET__
#define __TY_TRAJET__


#include "TYChemin.h"
#include "Tympan/models/common/3d.h"
#include "Tympan/models/common/atmospheric_conditions.h"
#include "Tympan/models/solver/entities.hpp"

/**
 * \file TYTrajet.h
 * \class TYTrajet
 * \brief  This class TYTrajet (journey) links a couple Source-Receptor and a collection of paths, in addition to the direct path.
 * \version v 1.1
 * \date 2008/01/21
 * \author Projet_Tympan
 */
class TYTrajet
{

    // Methods
public:
    /**
     * \fn TYTrajet(tympan::AcousticSource& asrc_, tympan::AcousticReceptor& arcpt_)
     * \brief Constructor
     * \param asrc_ Source
     * \param arcpt_ Receptor
     */
    TYTrajet(tympan::AcousticSource& asrc_, tympan::AcousticReceptor& arcpt_);
    
    /**
     * \fn TYTrajet(const TYTrajet& other)
     * \brief Copy constructor
     */
    TYTrajet(const TYTrajet& other);
    /**
     * \fn virtual ~TYTrajet()
     * \brief Destructor
     */
    virtual ~TYTrajet();

    ///Operator =.
    TYTrajet& operator=(const TYTrajet& other);
    ///Operator ==.
    bool operator==(const TYTrajet& other) const;
    ///Operator !=.
    bool operator!=(const TYTrajet& other) const;

    /**
     * \fn void reset()
     * \brief Reset method.
     */
    void reset();

    /**
     * \fn double getDistance()
     *     const double getDistance()
     * \brief Get/Set the distance between source and receptor
     * \return _distance
     */
    double getDistance() { return _distance; }
    const double getDistance() const { return _distance; }

    void setDistance(const double& distance) { _distance = distance; }

    /**
     * \fn size_t getNbChemins()
     * \brief Return the number of path in *this (in addition to the direct path).
     * \return _chemins.size()
     */
    size_t getNbChemins() { return _chemins.size(); }

    /**
     * \fn TYTabChemin& getChemins()
     * \brief Return the collection of paths of *this
     * \return _chemins
     */
    TYTabChemin& getChemins() { return _chemins; }

    /**
     * \fn TYTabChemin& getCheminsDirect()
     * \brief Return an array of the direct paths
     * \return _cheminsDirect
     */
    TYTabChemin& getCheminsDirect() {return  _cheminsDirect; }

    /**
     * \fn void addChemin(const TYChemin & chemin)
     * \brief Add a new path
     */
    void addChemin(const TYChemin& chemin);

    /**
     * \fn void addCheminDirect(const TYChemin & chemin)
     * \brief Add a new path to the array of direct paths
     */
    void addCheminDirect(const TYChemin& chemin);

    /**
     * \fn TYChemin getChemin(int index)
     * \brief Return a path thanks to its index.
     * \return _chemins.at(index)
     */
    TYChemin getChemin(int index) { return _chemins.at(index); }

    /**
     * \fn OSpectre getPNoOp()
     * \brief Return the attenuation without computation (computed by an external function)
     */
    OSpectre getPNoOp();

    /**
     * \fn void setPtSetPtR(const OPoint3D& pt1, const OPoint3D& pt2)
     *     void getPtSetPtR(OPoint3D& pt1, OPoint3D& pt2)
     * \brief Get/Set points for source and receptor
     */
    void setPtSetPtR(const OPoint3D& pt1, const OPoint3D& pt2) { _ptS = pt1; _ptR = pt2; }
    void getPtSetPtR(OPoint3D& pt1, OPoint3D& pt2) { pt1 = _ptS; pt2 = _ptR; }

    /**
     * \fn void setPtSetPtRfromOSeg3D(const OSegment3D& seg)
     *     void getPtSetPtRfromOSeg3D(OSegment3D& seg)
     * \brief Get/Set points for source and receptor with an OSegment3D
     */
    void setPtSetPtRfromOSeg3D(const OSegment3D& seg) { _ptS = seg._ptA; _ptR = seg._ptB; }
    void getPtSetPtRfromOSeg3D(OSegment3D& seg) const { seg._ptA = _ptS, seg._ptB = _ptR; }

    /**
     * \fn TYSpectre& getSpectre()
     *     const TYSpectre getSpectre()
     *     void setSpectre(const TYSpectre& spectre)
     * \brief Get/Set the spectrum at the receptor point
     * \return _sLP
     */
    //TYSpectre& getSpectre() { return _sLP; }
    //const TYSpectre getSpectre() const { return _sLP; }
    //void setSpectre(const TYSpectre& spectre) { _sLP = spectre; }
    OSpectre& getSpectre() { return _sLP; }
    const OSpectre getSpectre() const { return _sLP; }
    void setSpectre(const OSpectre& spectre) { _sLP = spectre; }


    /**
     * \fn OSpectre getPEnergetique(const AtmosphericConditions& atmos)
     * \brief Compute the acoustic pressure (phase modulation) on the journey
     */
    OSpectre getPEnergetique(const AtmosphericConditions& atmos);

    /**
     * \fn OSpectre getPInterference(const AtmosphericConditions& atmos)
     * \brief Compute the quadratic pressure on the journey
     */
    OSpectre getPInterference(const AtmosphericConditions& atmos);

private:
    OSpectre correctTiers(const OSpectreComplex& si, const OSpectreComplex& sj, const AtmosphericConditions& atmos, const double& ri, const double& rj) const;

public :
    /// Business source
    tympan::AcousticSource& asrc;
    tympan::source_idx asrc_idx;

    /// Business receptor
    tympan::AcousticReceptor& arcpt;
    tympan::receptor_idx arcpt_idx;


    // Members
protected:
    /// Source point definition in the site frame
    OPoint3D _ptS;

    /// Receptor point definition in the site frame
    OPoint3D _ptR;

    /// Paths collection
    TYTabChemin _chemins;

    /// Direct paths collection (without obstacles)
    TYTabChemin _cheminsDirect;

    ///Distance between source and receptor
    double _distance;

    /// Spectrum at the receptor point of the journey which integrates geometrical divergence and the source power
    //TYSpectre _sLP;
    OSpectre _sLP;
};
#endif // __TY_TRAJET__
