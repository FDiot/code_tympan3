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
class AcousticMaterialBase:
    public virtual BaseEntity
{
public:
    AcousticMaterialBase(const string& name_);
    virtual ComplexSpectrum get_absorption (double incidence_angle, double length) { return ComplexSpectrum(); }
    string name;

}; // class AcousticMaterialBase

// TODO Or use boost pointers container ?
typedef shared_ptr<AcousticMaterialBase> material_ptr_t;
typedef std::deque<material_ptr_t> material_pool_t;

// -------------------

class AcousticBuildingMaterial:
    public AcousticMaterialBase
{
public:
    AcousticBuildingMaterial(const string& name_, const ComplexSpectrum& spectrum);
    virtual ~AcousticBuildingMaterial() {};

    /*!
     * \brief : get material absorption at reflexion point
     * \fn ComplexSpectrum get_absorption (const double& incidence_angle)
     */    
    virtual ComplexSpectrum get_absorption (double incidence_angle, double length) { return spectrum; }

    ComplexSpectrum asEyring() const;


    ComplexSpectrum spectrum;
};

// -------------------

class AcousticGroundMaterial:
    public AcousticMaterialBase
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
	void set_deviation( double deviation_) { deviation = deviation_;}
	void set_length( double length_) { length = length_;}
    double get_ISO9613_G();

    static void set_atmosphere( AtmosphericConditions *atmosphere_ ) { atmosphere = atmosphere_; }

protected:
    void computeZc(); //!< compute characteristic impedance
    void computeK();  //!< compute wave nuber 
	void computeZf(double angle); //!< compute effective impedance in rough ground
    void computeZs(double angle, ComplexSpectrum& spectrum); //!< compute specific impedance
    void computeRp(double angle, const ComplexSpectrum& Zs, ComplexSpectrum& Rp);  //!<compute reflexion coefficient for plane waves
    void computeW(double angle, double length, const ComplexSpectrum& Zs, ComplexSpectrum &W); //compute numeric distance
    void computeFw(ComplexSpectrum localW, ComplexSpectrum& Fw); // Compute function of numeric distance
    void computeQ(double angle, ComplexSpectrum &Rp, ComplexSpectrum &Fw, ComplexSpectrum &Q); // compute reflexion coefficient

    static AtmosphericConditions *atmosphere;

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
	double GaussianSpectrum(double const k, double const sigma, double const lc);
	double trapz(std::vector<double> u, std::vector<double> integrande);
protected :
    /// XXX \todo put SI unit.
    double resistivity;
    double thickness;
	double deviation;
	double length;

    ComplexSpectrum Zc; //!< Characteriestic impedance
    ComplexSpectrum K;  //!< Wave number
	ComplexSpectrum Zf; //!< Effective impedance
};

// -------------------

class AcousticTriangle :
    public virtual BaseEntity
{
public:
    AcousticTriangle(node_idx n1, node_idx n2, node_idx n3);

    node_idx n[3];
    string volume_id;
    shared_ptr<AcousticMaterialBase> made_of;
};

typedef std::deque<AcousticTriangle> triangle_pool_t;
typedef size_t triangle_idx;

// -------------------

class SourceDirectivityInterface
{
public:
    virtual Spectrum lwAdjustment(Vector direction, double distance) = 0;
};

// -------------------

class SphericalSourceDirectivity :
      public virtual BaseEntity,
      public SourceDirectivityInterface
{
public:
    SphericalSourceDirectivity(){};
    virtual Spectrum lwAdjustment(Vector direction, double distance)
     { return Spectrum(1.0); }
};

// -------------------

class CommonFaceDirectivity : 
        public virtual BaseEntity,
        public SourceDirectivityInterface
{
public:
    CommonFaceDirectivity(const Vector& support_normal_, double support_size_) : 
                      BaseEntity(), 
                      support_normal(support_normal_),
                      support_size(support_size_) {}

    ~CommonFaceDirectivity() {}

    Vector get_normal(){ return support_normal; }

    static void set_atmosphere( AtmosphericConditions *atmosphere_ ) { atmosphere = atmosphere_; }

protected :
    Vector support_normal;              /*! Normal of support face */
    double support_size;                /*! Characteristic size of support face */
    
    static AtmosphericConditions *atmosphere;
};

class VolumeFaceDirectivity :
    public CommonFaceDirectivity
{
public:
    VolumeFaceDirectivity(const Vector& support_normal_, double support_size_) : 
                            CommonFaceDirectivity(support_normal_, support_size_) {}
    
     ~VolumeFaceDirectivity() {}

     virtual Spectrum lwAdjustment(Vector direction, double distance);

private:
    static const double _tabRA[];       /*<! RA form factor */
    static const double _tabCor[];      /*<! Correction factors */

    double calculC(double distance);    /*! Compute directivity factor */
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
class ChimneyFaceDirectivity :
    public CommonFaceDirectivity
{
public :
    ChimneyFaceDirectivity(const Vector& support_normal_, double support_size_) : 
                            CommonFaceDirectivity(support_normal_, support_size_) {}

    ~ChimneyFaceDirectivity() {}
    virtual Spectrum lwAdjustment(Vector direction, double distance);


private :
    double compute_q(int ka_idx, int theta_idx, double ka, double theta);

    static const double _tabQ[NB_KA][NB_THETA];
};
//
// -------------------------
//
#ifdef NB_KA
#   undef NB_KA
#   define NB_KA 9
#else
#   define NB_KA 9
#endif

// nombre de valeurs de theta dans le tableau
#ifdef NB_THETA
#   undef NB_THETA
#   define NB_THETA 41
#else
#   define NB_THETA 41
#endif

class BaffledFaceDirectivity :
    public CommonFaceDirectivity
{
public :
    BaffledFaceDirectivity(const Vector& support_normal_, double support_size_) : 
                            CommonFaceDirectivity(support_normal_, support_size_) {}

    ~BaffledFaceDirectivity() {}
    virtual Spectrum lwAdjustment(Vector direction, double distance);

private:
    static const double _tabQ[NB_KA][NB_THETA];
    static const double _tabKa[NB_KA];

    double compute_q(int indice_Ka, int indice_theta, double ka, double theta);

    int find_Ka_idx(double ka);
};


class AcousticSource:
    public virtual BaseEntity
{
public:
    AcousticSource(
            const Point& point_,
            const Spectrum& spectrum_,
            SourceDirectivityInterface* directivity);

    virtual ~AcousticSource() {};

    Point position;
    Spectrum spectrum;
    SourceDirectivityInterface* directivity;
    string volume_id;
    string face_id;
};

typedef std::deque<AcousticSource> source_pool_t;
typedef size_t source_idx;

// -------------------

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

// -------------------

class GlobalContribution:
    public virtual BaseEntity
{
public:
    GlobalContribution() {};
    virtual ~GlobalContribution() {};

    // XXX Add some attrs?
};

// -------------------

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
