/**
 * \file entities.hpp
 * \brief This file provides the declaration of the entities of the model
 *
 * \date nov. 6 2012
 * \author Anthony Truchet <anthony.truchet@logilab.fr>
 * \author Damien Garaud <damien.garaud@logilab.fr>
 */

#ifndef TYMPAN__ENTITIES_H__INCLUDED
#define TYMPAN__ENTITIES_H__INCLUDED


#include "data_model_common.hpp"

namespace tympan
{

/// XXX \todo Add the entity 'Atmosphere' with attr: pression, temperature,
/// hygrometry (\note can find these values in the TYCalcul instead of TYSite).

// NB For now an AcousticSpectrum simply IS a OSpectreComplex
class AcousticSpectrum:
        public virtual BaseEntity,
        public Spectrum
{
public:
    AcousticSpectrum(const Spectrum& spectrum) : Spectrum(spectrum) {};
    virtual ~AcousticSpectrum() {};
};

class AcousticMaterialBase:
    public virtual BaseEntity
{
public:
    AcousticMaterialBase(const string& name_);
    string name;
}; // class AcousticMaterialBase

// TODO Or use boost pointers container ?
typedef shared_ptr<AcousticMaterialBase> material_ptr_t;
typedef std::deque<material_ptr_t> material_pool_t;

class AcousticBuildingMaterial:
    public virtual BaseEntity, public AcousticMaterialBase
{
public:
    AcousticBuildingMaterial(const string& name_, const Spectrum& spectrum);
    virtual ~AcousticBuildingMaterial() {};

    AcousticSpectrum spectrum;
};

class AcousticGroundMaterial:
    public virtual BaseEntity, public AcousticMaterialBase
{
public:
    AcousticGroundMaterial(const string& name_, double resistivity_);
    virtual ~AcousticGroundMaterial() {};

    /// XXX \todo put SI unit.
    double resistivity;
};


class AcousticTriangle :
    public virtual BaseEntity
{
public:
    AcousticTriangle(node_idx n1, node_idx n2, node_idx n3 );

    node_idx n[3];

    shared_ptr<AcousticMaterialBase> made_of;
    binary_uuid uuid;
};

typedef std::deque<AcousticTriangle> triangle_pool_t;
typedef size_t triangle_idx;

class DiffractionEdge:
    public virtual BaseEntity
{
public:
    DiffractionEdge(const Point& p1_, const Point& p2_, double angle_);
    virtual ~DiffractionEdge() {};

    Point p1;
    Point p2;
    /// Angle (radian).
    double angle;
};

class AcousticSource:
    public virtual BaseEntity
{
public:
    AcousticSource() {};
    virtual ~AcousticSource() {};

    // XXX Add some attr.?
};

class AcousticReceptor:
    public virtual BaseEntity
{
public:
    AcousticReceptor(const Point& point_);
    virtual ~AcousticReceptor() {};

    Point point;
};

class AcousticEvent:
    public virtual BaseEntity
{
public:
    AcousticEvent(const string& event_type_, const Point& point_);
    virtual ~AcousticEvent() {};

    Point point;
    /// Can be 'reflection', 'diffraction', 'refraction', 'emission', 'reception'.
    string event_type;
};

class AcousticPath:
    public virtual BaseEntity
{
public:
    AcousticPath() {};
    virtual ~AcousticPath() {};

    // XXX Add some attr?
};

class AcousticProblem:
    public virtual BaseEntity
{
public:
    AcousticProblem(const string& name_);
    virtual ~AcousticProblem() {};

    string name;
};


class GlobalContribution:
    public virtual BaseEntity
{
public:
    GlobalContribution() {};
    virtual ~GlobalContribution() {};

    // XXX Add some attrs?
};

class SiteElement:
    public virtual BaseEntity
{
public:
    SiteElement(const binary_uuid& uid_);
    virtual ~SiteElement() {};

    binary_uuid uid;
};


class SiteAcousticReceptor:
    public virtual BaseEntity
{
public:
    SiteAcousticReceptor(unsigned int id_);
    virtual ~SiteAcousticReceptor() {};

    unsigned int id;
};

class Site:
    public virtual BaseEntity
{
public:
    Site(unsigned int id_);
    virtual ~Site() {};

    unsigned int id;
};

class DirectivityCoefficient
{
public:
    DirectivityCoefficient(double value_, double theta_, double phi_,
                           bool solid_angle_);
    virtual ~DirectivityCoefficient() {};

    double value;
    double theta;
    double phi;
    // XXX not sure for the type below.
    bool solid_angle;
};

} /* namespace tympan */

#endif /* TYMPAN__ENTITIES_H__INCLUDED */
