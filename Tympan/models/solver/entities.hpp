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
#include "Tympan/models/common/atmospheric_conditions.h"

namespace tympan
{
/**
 * @brief Base class for material
 */
class AcousticMaterialBase:
    public virtual BaseEntity
{
public:
	/// Constructor
    AcousticMaterialBase(const string& name_);
    /// Virtual method to return material absorption at reflection point
    virtual ComplexSpectrum get_absorption (double incidence_angle, double length) { return ComplexSpectrum(); }
    string name; //!< Material name

}; // class AcousticMaterialBase

// TODO Or use boost pointers container ?
typedef shared_ptr<AcousticMaterialBase> material_ptr_t;
typedef std::deque<material_ptr_t> material_pool_t;

// -------------------

/**
 * @brief Describes building material
 */
class AcousticBuildingMaterial:
    public AcousticMaterialBase
{
public:
    AcousticBuildingMaterial(const string& name_, const ComplexSpectrum& spectrum);
    virtual ~AcousticBuildingMaterial() {};

    /*!
     * \brief : Get material absorption at reflection point
     * \fn ComplexSpectrum get_absorption (const double& incidence_angle)
     */    
    virtual ComplexSpectrum get_absorption (double incidence_angle, double length) { return spectrum; }

    ComplexSpectrum asEyring() const; //!< Returns a spectrum with Eyring formulae


    ComplexSpectrum spectrum; //!< Spectrum to store acoustic values at different frequencies
};

// -------------------

/**
 * @brief  Describes the ground material, a specific AcousticBuildingMaterial
 */
class AcousticGroundMaterial:
    public AcousticMaterialBase
{
public:
	/// Constructor
    AcousticGroundMaterial(const string& name_, double resistivity_, double deviation_, double length_);
    virtual ~AcousticGroundMaterial() {};

    /*!
     * \fn ComplexSpectrum get_absorption (double incidence_angle, double length)
     * \brief : Get material absorption at reflection point
     * \param incidence_angle Angle incidence
     * \param length Wave length
     * \return A ComplexSpectrum
     */    
    virtual ComplexSpectrum get_absorption (double incidence_angle, double length);

    const ComplexSpectrum& get_Zc() const { return Zc; } //!< Get characteristic impedance
    const ComplexSpectrum& get_K() const  { return K; }  //!< Get wave number

    void set_thickness( double thickness_ ) { thickness = thickness_; }  //!< Set thickness
	void set_deviation( double deviation_) { deviation = deviation_;}    //!< Set deviation
	void set_length( double length_) { length = length_;}                //!< Set wave length
    double get_ISO9613_G();                                              //!< Absorption given by ISO9613

    static void set_atmosphere( AtmosphericConditions *atmosphere_ ) { atmosphere = atmosphere_; } //!< Set pointer to current atmosphere

protected:
    void computeZc(); //!< Compute characteristic impedance
    void computeK();  //!< Compute wave number
    void computeZs(double angle, ComplexSpectrum Z, ComplexSpectrum& spectrum); //!< Compute specific impedance
	void computeZf(double angle, ComplexSpectrum Zs); //!< Compute effective impedance in rough ground
    void computeRp(double angle, const ComplexSpectrum& Zs, ComplexSpectrum& Rp);  //!< Compute reflection coefficient for plane waves
    void computeW(double angle, double length, const ComplexSpectrum& Zs, ComplexSpectrum &W); //!< Compute numeric distance
    void computeFw(ComplexSpectrum localW, ComplexSpectrum& Fw); //!< Compute function of numeric distance
    void computeQ(double angle, ComplexSpectrum &Rp, ComplexSpectrum &Fw, ComplexSpectrum &Q); //!< Compute reflection coefficient

    static AtmosphericConditions *atmosphere; //!< Pointer to current atmosphere

private:
    void init();

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
	double gaussianSpectrum(double const k, double const sigma, double const lc);
	double trapz(std::vector<double> u, std::vector<double> integrande);
protected :
    /// XXX \todo put SI unit.
    double resistivity;
    double thickness;
	double deviation;
	double length;

    ComplexSpectrum Zc; //!< Characteristic impedance
    ComplexSpectrum K;  //!< Wave number
	ComplexSpectrum Zf; //!< Effective impedance
};

// -------------------

/**
 * @brief Describing a triangle
 */
class AcousticTriangle :
    public virtual BaseEntity
{
public:
	/// Constructor with the 3 vertexes of the triangle
    AcousticTriangle(node_idx n1, node_idx n2, node_idx n3);

    node_idx n[3];    //!< Triangle vertexes
    string volume_id; //!< Volume id
    shared_ptr<AcousticMaterialBase> made_of; //!< Pointer to the material
};

typedef std::deque<AcousticTriangle> triangle_pool_t; //!< Array of AcousticTriangle
typedef size_t triangle_idx;

// -------------------

/**
 * @brief Interface for source directivity classes (SphericalSourceDirectivity, CommonFaceDirectivity, ...)
 */
class SourceDirectivityInterface
{
public:
	//!< Pure virtual method to return directivity of the Source
    virtual Spectrum lwAdjustment(Vector direction, double distance) = 0;
};

// -------------------
/**
 * @brief Directivity for a spherical source
 */
class SphericalSourceDirectivity :
      public virtual BaseEntity,
      public SourceDirectivityInterface
{
public:
    SphericalSourceDirectivity(){};

    virtual Spectrum lwAdjustment(Vector direction, double distance)
     { return Spectrum(1.0); } //!< Directivity of a spherical source
};

// -------------------

/**
 * @brief Base class for directivity on face
 */
class CommonFaceDirectivity : 
        public virtual BaseEntity,
        public SourceDirectivityInterface
{
public:
	/// Constructor
    CommonFaceDirectivity(const Vector& support_normal_, double support_size_) : 
                      BaseEntity(), 
                      support_normal(support_normal_),
                      support_size(support_size_) {}

    ~CommonFaceDirectivity() {}

    Vector get_normal(){ return support_normal; }

    static void set_atmosphere( AtmosphericConditions *atmosphere_ ) { atmosphere = atmosphere_; }

protected :
    Vector support_normal;              //! Normal of support face
    double support_size;                //! Characteristic size of support face
    
    static AtmosphericConditions *atmosphere; //!< Pointer to current atmosphere
};

/**
 * @brief Directivity for a volume face
 */
class VolumeFaceDirectivity :
    public CommonFaceDirectivity
{
public:
    VolumeFaceDirectivity(const Vector& support_normal_, double support_size_) : 
                            CommonFaceDirectivity(support_normal_, support_size_) {}
    
     ~VolumeFaceDirectivity() {}

     virtual Spectrum lwAdjustment(Vector direction, double distance); //!< Directivity of a volume face

private:
    static const double _tabRA[];       //!< RA form factor
    static const double _tabCor[];      //!< Correction factors

    double calculC(double distance);    //!< Compute directivity factor
};
//
// ------------------
//
#ifdef NB_KA
#   undef NB_KA
#   define NB_KA 38
#else
#   define NB_KA 38
#endif

#ifdef NB_THETA
#   undef NB_THETA
#   define NB_THETA 21
#else
#   define NB_THETA 21
#endif
/**
 * @brief Directivity for a chimney face
 */
class ChimneyFaceDirectivity :
    public CommonFaceDirectivity
{
public :
    ChimneyFaceDirectivity(const Vector& support_normal_, double support_size_) : 
                            CommonFaceDirectivity(support_normal_, support_size_) {}

    ~ChimneyFaceDirectivity() {}
    virtual Spectrum lwAdjustment(Vector direction, double distance); //!< Directivity of a chimney face source


private :
    double compute_q(int ka_idx, int theta_idx, double ka, double theta);

    static const double _tabQ[NB_KA][NB_THETA];
};
//
// -------------------------
//
/// Number of KA values in the array
#ifdef NB_KA
#   undef NB_KA
#   define NB_KA 9
#else
#   define NB_KA 9
#endif

/// Number of theta values in the array
#ifdef NB_THETA
#   undef NB_THETA
#   define NB_THETA 41
#else
#   define NB_THETA 41
#endif

/**
 * @brief Directivity for a baffled face
 */
class BaffledFaceDirectivity :
    public CommonFaceDirectivity
{
public :

    BaffledFaceDirectivity(const Vector& support_normal_, double support_size_) : 
                            CommonFaceDirectivity(support_normal_, support_size_) {}

    ~BaffledFaceDirectivity() {}
    virtual Spectrum lwAdjustment(Vector direction, double distance); //!< Directivity of a baffled face source

private:
    static const double _tabQ[NB_KA][NB_THETA];
    static const double _tabKa[NB_KA];

    double compute_q(int indice_Ka, int indice_theta, double ka, double theta);

    int find_Ka_idx(double ka);
};

/**
 * @brief Describes an acoustic source
 */
class AcousticSource:
    public virtual BaseEntity
{
public:
	/// Constructor to build a source defined by a point, spectrum, directivity
    AcousticSource(
            const Point& point_,
            const Spectrum& spectrum_,
            SourceDirectivityInterface* directivity);

    virtual ~AcousticSource() {};

    Point position;      //!< Position of the source
    Spectrum spectrum;   //!< Associated spectrum
    SourceDirectivityInterface* directivity; //!< Pointer to the source directivity
    string volume_id;    //!< Volume id
    string face_id;      //!< Face id
};

typedef std::deque<AcousticSource> source_pool_t; //!< Array of sources
typedef size_t source_idx;

// -------------------
/**
 * @brief Describes an acoustic receptor
 */
class AcousticReceptor:
    public virtual BaseEntity
{
public:
	/// Constructor to build a receptor defined by the its position
    AcousticReceptor(const Point& position_);
    virtual ~AcousticReceptor() {};

    Point position; //!< Position of the receptor
};

typedef std::deque<AcousticReceptor> receptor_pool_t; //!< Array of receptors
typedef size_t receptor_idx;

// -------------------

/**
 * @brief Global contribution
 */
class GlobalContribution:
    public virtual BaseEntity
{
public:
    GlobalContribution() {};
    virtual ~GlobalContribution() {};

    // XXX Add some attrs?
};

// -------------------

/**
 * @brief Directivity coefficient
 */
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

Point ComputeAcousticCentroid(const source_pool_t &tabSources_);

} /* namespace tympan */


#endif /* TYMPAN__ENTITIES_H__INCLUDED */
