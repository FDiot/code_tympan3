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

/**
 * @brief factory for any Entity
 *
 * TODO to be moved into Yams++
 *
 * @tparam EntityT the class of the entity to be created
 * @return a (smart) pointer to new new, default constructed, entity of class \c EntityT
 */
template<class EntityT>
typename EntityT::pointer
make()
{
	return typename  EntityT::pointer(new EntityT());
}


//! Simple representation of an UUID (Universal Unique Identifier).
/*! Do the correspondance between \c TYElement uuid and the uuid of schema
    solver entities. It can match with a \c QUuid (UUID from Qt) and thus with a
    \c TYUUID (aka \c OGenID).
 */
struct binary_uuid {
    union {
        struct {
            unsigned int   data1;
            unsigned short data2;
            unsigned short data3;
            unsigned char  data4[8];
        } s;
        unsigned char t[16];
    };
};


/// XXX \todo Add the entity 'Atmosphere' with attr: pression, temperature,
/// hygrometry (\note can find these values in the TYCalcul instead of TYSite).


class Node:
    public Entity<Node>
{
public:
    Node(const Point& p_);
    virtual ~Node() {};

    Point p;

private:
    /// @brief private copy constructor to avoid copying Nodes
    Node(const Node&);
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

class AcousticTriangle:
    public Entity<AcousticTriangle>
{
public:
    virtual ~AcousticTriangle() {};

    /**
     * @brief Factory for an \c AcousticTriangle from three \c Node
     *
     * In order to have a canonical representation of triangles,
     * the order of the three points can be rotated, but the surface orientation
     * is preserved.
     *
     * @param p1 a node
     * @param p2 an other node
     * @param p3 an third node
     * @return a new instance od AcousticTriangle
     */
    static AcousticTriangle::pointer
    make_triangle(
    		const Node::pointer& p1,
    		const Node::pointer& p2,
    		const Node::pointer& p3);

    /**
     * @brief Factory for an \c AcousticTriangle from three \c Node
     *
     * In order to have a canonical representation of triangles,
     * the order of the three points can be rotated, but the surface orientation
     * is preserved.
     *
     * @param points a C array of three \c Node::pointer
     * @return a new instance od AcousticTriangle
     */
    static AcousticTriangle::pointer
    make_triangle(Node::pointer (* const points)[3]);

// TODO the make() generic factory needs to be moved into Yams++ for this to be possible
// protected:
    // This constructor does not build a valid face,
    // Please use the factory functions make_triangle instead.
    AcousticTriangle() {};
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
    SiteElement(const binary_uuid& uid_);
    virtual ~SiteElement() {};

    binary_uuid uid;
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
