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

#include <string>
using std::string;

#include "data_model_common.hpp"

#include "yams/yams.hh"

namespace tympan
{

/// XXX \todo Add the entity 'Atmosphere' with attr: pression, temperature,
/// hygrometry (\note can find these values in the TYCalcul instead of TYSite).


class Node:
    public Entity<Node>
{
public:
    Node(const Point& p_);
    virtual ~Node() {};

    Point p;
};

class DiffractionEdge:
    public Entity<DiffractionEdge>
{
public:
    DiffractionEdge(const Point& p1_, const Point& p2_, double angle_);
    virtual ~DiffractionEdge() {};

    Point p1;
    Point p2;
    /// Angle (radian).
    double angle;


};

/* The surface_has_node relation is supposed to have cardinality 3..n
 * and the normal given has attribute is supposed to be one of the
 * two possible unit normal vector for the triangle.
 * Those constraints can't be formaly expressed in the schema.
 */
class AcousticSurface:
    public Entity<AcousticSurface>
{
public:
    AcousticSurface() {};
    // AcousticSurface(const Vector& normal_);
    virtual ~AcousticSurface() {};

    // XXX Normal should be computed by the solver. It's only relevant to have a
    // normal on the triangle surface after a Delaunay triangulation (not
    // before).
    // Vector normal;
};

class AcousticBuildingMaterial:
    public Entity<AcousticBuildingMaterial>
{
public:
    AcousticBuildingMaterial();
    AcousticBuildingMaterial(const string& name_);
    virtual ~AcousticBuildingMaterial() {};

    string name;
};

class AcousticGroundMaterial:
    public Entity<AcousticGroundMaterial>
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
    public Entity<AcousticSpectrum>
{
public:
    AcousticSpectrum() {};
    virtual ~AcousticSpectrum() {};

    // XXX How do you define a \c AcousticSpectrum ?
    // XXX Add some attr?
};


class AcousticSource:
    public Entity<AcousticSource>
{
public:
    AcousticSource() {};
    virtual ~AcousticSource() {};

    // XXX Add some attr.?
};

class AcousticReceptor:
    public Entity<AcousticReceptor>
{
public:
    AcousticReceptor(const Point& point_);
    virtual ~AcousticReceptor() {};

    Point point;
};

class AcousticEvent:
    public Entity<AcousticEvent>
{
public:
    AcousticEvent(const string& event_type_, const Point& point_);
    virtual ~AcousticEvent() {};

    Point point;
    /// Can be 'reflection', 'diffraction', 'refraction', 'emission', 'reception'.
    string event_type;
};

class AcousticPath:
    public Entity<AcousticPath>
{
public:
    AcousticPath() {};
    virtual ~AcousticPath() {};

    // XXX Add some attr?
};

class AcousticProblem:
    public Entity<AcousticProblem>
{
public:
    AcousticProblem(const string& name_);
    virtual ~AcousticProblem() {};

    string name;
};


class GlobalContribution:
    public Entity<GlobalContribution>
{
public:
    GlobalContribution() {};
    virtual ~GlobalContribution() {};

    // XXX Add some attrs?
};

class SiteElement:
    public Entity<SiteElement>
{
public:
    SiteElement(const string& id_);
    virtual ~SiteElement() {};

    string id;
    // XXX Add an attribute which will be a ref. to a \c TYElement??
    // Other attributes? How do you define a \c SiteElement?
};

class SiteUserAcousticSource:
    public Entity<SiteUserAcousticSource>
{
public:
    SiteUserAcousticSource(unsigned int id_);
    virtual ~SiteUserAcousticSource() {};

    unsigned int id;
};

class SiteAcousticReceptor:
    public Entity<SiteAcousticReceptor>
{
public:
    SiteAcousticReceptor(unsigned int id_);
    virtual ~SiteAcousticReceptor() {};

    unsigned int id;
};

class Site: public Entity<Site>
{
public:
    Site(unsigned int id_);
    virtual ~Site() {};

    unsigned int id;
};


//! A sample of a spectrum. One spectrum sample for each frequency value.
class SpectrumSample:
    public Entity<SpectrumSample>
{
public:
    SpectrumSample(double modulus_, const string& type_, double phase_=0.);
    virtual ~SpectrumSample() {};

    /// Modulus.
    double modulus;
    /// Phase. For now, set to zero.
    double phase;
    /// Representation type: third octave, nth octave, etc.
    string type;
};

class DirectivityCoefficient:
    public Entity<DirectivityCoefficient>
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

class Frequency:
    public Entity<Frequency>
{
public:
    Frequency(double value_);
    virtual ~Frequency() {};

    double value;
};


} /* namespace tympan */

#endif /* TYMPAN__ENTITIES_H__INCLUDED */
