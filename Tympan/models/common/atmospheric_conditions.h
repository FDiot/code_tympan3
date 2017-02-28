#ifndef TYMPAN_MODELS_COMMON_ATMOSPHERIC_CONDITIONS
#define TYMPAN_MODELS_COMMON_ATMOSPHERIC_CONDITIONS

#include "Tympan/models/common/spectre.h"


/// XXX \todo Add the entity 'Atmosphere' with attr: pression, temperature,
/// hygrometry (\note can find these values in the TYCalcul instead of TYSite).
/**
 * \brief Class for the definition of atmospheric conditions
 */
class AtmosphericConditions
{
public:
	/// Constructor
    AtmosphericConditions(double static_pressure_, double temperature_, double hygrometry_);
    /// Destructor
    virtual ~AtmosphericConditions() {}

    /*!
     * \brief Compute absorption spectrum in dB/m
     * \brief This spectrum will be used to compute absorption for a given distance
     */
    void compute_absorption_spectrum();
    /// Get absorption spectrum
    OSpectre get_absorption_spectrum() const { return absorption_spectrum; }

    double compute_c() const; //!< compute sound speed

    void compute_k(); //!< compute wave number

    double compute_z(); //!< compute impedance

    OSpectre compute_length_absorption(double length) const;

    /*!
     * \brief Get the wave number spectrum
     */
    const OSpectre& get_k() const { return wave_number; }

    /*!
     * \fn double get_absorption_value( double freq )
     * \brief return absorption value at a given frequency
     */
    double get_absorption_value( double freq ) {  return absorption_spectrum.getValueReal(freq); }


private :
    double compute_hm() const; //!< compute molar hygrometry coefficient

protected:
    double static_pressure;			//!< Static pressure [Pa]
    double temperature;				//!< Temperature [°C]
    double hygrometry;				//!< Hygrometry
    OSpectre wave_number;			//!< Wave number spectrum
    OSpectre absorption_spectrum;	//!< Absorption spectrum

public:
    static const double Z_ref; //!< reference impedance 

    static const double reference_pressure;		//!< Reference pressure [Pa]
    static const double reference_temperature;	//!< Reference temperature [K]
    static const double absolute_zero;			//!< 273.15 K

};

#endif
