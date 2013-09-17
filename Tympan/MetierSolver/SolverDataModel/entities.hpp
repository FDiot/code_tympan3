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


class AcousticBuildingMaterial:
    public virtual BaseEntity
{
public:
    AcousticBuildingMaterial();
    AcousticBuildingMaterial(const string& name_);
    virtual ~AcousticBuildingMaterial() {};

    string name;
};

class AcousticGroundMaterial:
    public virtual BaseEntity
{
public:
    AcousticGroundMaterial(const string& name_, double resistivity_);
    virtual ~AcousticGroundMaterial() {};

    string name;
    /// XXX \todo put SI unit.
    double resistivity;
};

// XXX Add some method to easy use & get freq. and spectrum attributes. See
// class \c OSpectre.
class AcousticSpectrum:
    public virtual BaseEntity
{
public:
    AcousticSpectrum() {};
    virtual ~AcousticSpectrum() {};

    // XXX How do you define a \c AcousticSpectrum ?
    // XXX Add some attr?
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
