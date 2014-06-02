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
class AtmosphericConditions : 
    public virtual BaseEntity
{
public:
    AtmosphericConditions(double static_pressure_, double temperature_, double hygrometry_);
    virtual ~AtmosphericConditions() {}

    /*!
     * \fn void compute_absorption_spectrum();
     * \brief Compute absorption spectrum in dB/m
     * \brief This spectrum will be used to compute absorption for a given distance
     */
    void compute_absorption_spectrum(); 

    double compute_c(); //!< compute sound speed

    void compute_k(); //!< compute wave number

    double compute_z(); //!< compute impedance

    Spectrum compute_length_absorption(double length);

    /*!
     * Get / Set
     */
    const Spectrum& get_k() const { return wave_number; }

    /*!
     * \fn double get_absorption_value( double freq )
     * \brief return absorption value at a given frequency
     */
    double get_absorption_value( double freq ) {  return absorption_spectrum.getValueReal(freq); }


private :
    double compute_hm() const; //!< compute molar hygrometry coefficient

protected:
    double static_pressure;
    double temperature;
    double hygrometry;
    Spectrum wave_number;
    Spectrum absorption_spectrum;

public:
    static const double Z_ref; //!< reference impedance 

    static const double reference_pressure;
    static const double reference_temperature;
    static const double absolute_zero;

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

// -------------------

class AcousticBuildingMaterial:
    public virtual BaseEntity, public AcousticMaterialBase
{
public:
    AcousticBuildingMaterial(const string& name_, const ComplexSpectrum& spectrum);
    virtual ~AcousticBuildingMaterial() {};

    /*!
     * \brief : get material absorption at reflexion point
     * \fn ComplexSpectrum get_absorption (const double& incidence_angle)
     */    
    virtual ComplexSpectrum get_absorption (const double& incidence_angle, double length) { return spectrum; }

    ComplexSpectrum spectrum;
};

// -------------------

class AcousticGroundMaterial:
    public virtual BaseEntity, public AcousticMaterialBase
{
public:
    AcousticGroundMaterial(const string& name_, double resistivity_);
    virtual ~AcousticGroundMaterial() {};

    /*!
     * \brief : get material absorption at reflexion point
     * \fn ComplexSpectrum get_absorption (const double& incidence_angle)
     */    
    virtual ComplexSpectrum get_absorption (double incidence_angle, double length);

    const ComplexSpectrum& get_Zc() const { return Zc; }
    const ComplexSpectrum& get_K() const  { return K; }

    void set_thickness( double thickness_ ) { thickness = thickness_; }

    double get_ISO9613_G();

    static void set_atmosphere( AtmosphericConditions *atmosphere_ ) { atmosphere = atmosphere_; }

protected:
    void computeZc(); //!< compute characteristic impedance
    void computeK();  //!< compute wave nuber in ground
    void computeZs(double angle, ComplexSpectrum& spectrum); //!< compute specific impedance
    void computeRp(double angle, const ComplexSpectrum& Zs, ComplexSpectrum& Rp);  //!<compute reflexion coefficient for plane waves
    void computeW(double angle, double length, const ComplexSpectrum& Zs, ComplexSpectrum &W); //compute numeric distance
    void computeFw(ComplexSpectrum localW, ComplexSpectrum& Fw); // Compute function of numeric distance
    void computeQ(double angle, ComplexSpectrum &Rp, ComplexSpectrum &Fw, ComplexSpectrum &Q); // compute reflexion coefficient


private:
    /*!
     * \brief : Functions used in Fw computation
     */
    TYComplex erfcCas1(const TYComplex& wValue) const;
    TYComplex erfcCas2(const TYComplex& wValue) const;
    TYComplex erfcCas3(const TYComplex& wValue) const;

    /*!
     * \brief : function used in G computation
     */
    TYComplex sgnReIm(const TYComplex& W, const TYComplex& G) const ;

    void limit_W_values(ComplexSpectrum& localW);
    void erfc_G_computation(const ComplexSpectrum& localW, ComplexSpectrum& G);
    void sgn_G_computation(const ComplexSpectrum& localW, ComplexSpectrum& G);

protected :
    /// XXX \todo put SI unit.
    double resistivity;
    double thickness;

    ComplexSpectrum Zc; //!< Characteriestic impedance
    ComplexSpectrum K;  //!< Wave number

    static AtmosphericConditions *atmosphere;
};

// -------------------

class AcousticTriangle :
    public virtual BaseEntity
{
public:
    AcousticTriangle(node_idx n1, node_idx n2, node_idx n3);

    node_idx n[3];

    shared_ptr<AcousticMaterialBase> made_of;
};

typedef std::deque<AcousticTriangle> triangle_pool_t;
typedef size_t triangle_idx;

class SourceDirectivityInterface
{
public:
    virtual Spectrum lwAdjustment(Vector direction) = 0;
};

class SphericalSourceDirectivity :
      public virtual BaseEntity,
      public SourceDirectivityInterface
{
public:
     virtual Spectrum lwAdjustment(Vector direction)
     { return Spectrum(1.0); }
};

class AcousticSource:
    public virtual BaseEntity
{
public:
    AcousticSource(
            const Point& point_,
            const Spectrum& spectrum_);

    virtual ~AcousticSource() {};

    Point position;
    Spectrum spectrum;
    SourceDirectivityInterface* directivity;
};

typedef std::deque<AcousticSource> source_pool_t;
typedef size_t source_idx;


class AcousticReceptor:
    public virtual BaseEntity
{
public:
    AcousticReceptor(const Point& position_);
    virtual ~AcousticReceptor() {};

    Point position;
};

typedef std::deque<AcousticReceptor> receptor_pool_t;
typedef size_t receptor_idx;


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
    SiteElement();
    virtual ~SiteElement() {};

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
