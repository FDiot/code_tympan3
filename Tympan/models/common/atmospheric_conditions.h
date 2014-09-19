#ifndef TYMPAN_MODELS_COMMON_ATMOSPHERIC_CONDITIONS
#define TYMPAN_MODELS_COMMON_ATMOSPHERIC_CONDITIONS

#include "Tympan/models/common/spectre.h"


/// XXX \todo Add the entity 'Atmosphere' with attr: pression, temperature,
/// hygrometry (\note can find these values in the TYCalcul instead of TYSite).
class AtmosphericConditions
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

    OSpectre get_absorption_spectrum() const { return absorption_spectrum; }

    double compute_c() const; //!< compute sound speed

    void compute_k(); //!< compute wave number

    double compute_z(); //!< compute impedance

    OSpectre compute_length_absorption(double length) const;

    /*!
     * Get / Set
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
    double static_pressure;
    double temperature;
    double hygrometry;
    OSpectre wave_number;
    OSpectre absorption_spectrum;

public:
    static const double Z_ref; //!< reference impedance 

    static const double reference_pressure;
    static const double reference_temperature;
    static const double absolute_zero;

};

#endif
