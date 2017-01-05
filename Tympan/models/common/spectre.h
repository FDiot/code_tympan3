/*
 * Copyright (C) <2012-2014> <EDF-R&D> <FRANCE>
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

#ifndef TY_MC_SPECTRE
#define TY_MC_SPECTRE

#include <vector>
#include <map>
#include <ostream>

#include "Tympan/core/macros.h"

///Spectrum type.
enum TYSpectreType { SPECTRE_TYPE_ATT, SPECTRE_TYPE_ABSO, SPECTRE_TYPE_LW, SPECTRE_TYPE_LP, SPECTRE_TYPE_AUTRE };
///Spectrum representation.
enum TYSpectreForm { SPECTRE_FORM_TIERS, SPECTRE_FORM_OCT, SPECTRE_FORM_CST_DF, SPECTRE_FORM_UNSHAPED };
///Spectrum state (dB/grandeur physique).
enum TYSpectreEtat { SPECTRE_ETAT_DB, SPECTRE_ETAT_LIN };


///Number of frequencies
static const unsigned int TY_SPECTRE_DEFAULT_NB_ELMT = 31;

///Default value for the spectrum.
static const double TY_SPECTRE_DEFAULT_VALUE = -100.0;

///Frequencies collection
typedef std::vector<double> OTabFreq;


/**
 * \brief Store acoustic power values for different frequencies.
 *
 * @author Projet_Tympan
 *
 */
class OSpectre
{
    // Methods
public:
	/// Default constructor, the spectrum module is defined by the _defaultValue
    OSpectre();
    /// Constructor, the spectrum module is defined by the defaultValue given in parameter
    OSpectre(double defaultValue);
    /// Copy constructor
    OSpectre(const OSpectre& other);
    /**
     * \brief Constructor from an array (needed for the Harmonoise model)
     * \param valeurs Array of values per frequency
     * \param nbVal  Array size
     * \param decalage Frequency offset relative to the standard band TYMPAN (16-16000)
     */
    OSpectre(const double* valeurs, unsigned nbVal, unsigned decalage);

    virtual ~OSpectre();

    virtual OSpectre& operator= (const OSpectre& other);
    virtual bool operator== (const OSpectre& other) const;
    virtual bool operator != (const OSpectre& other) const;
    /// Multiplication by a Spectre spectrum
    OSpectre operator * (const OSpectre& spectre) const;
    /// Multiplication by a double coefficient
    OSpectre operator * (const double& coefficient) const;
    /// Add a constant value to all the spectrum
    virtual OSpectre operator + (const double& valeur) const;
    /// Arithmetic sum of two spectrums in one-third Octave.
    virtual OSpectre operator + (const OSpectre& spectre) const;
    /// Arithmetic subtraction of two spectrums in one-third Octave.
    virtual OSpectre operator - (const OSpectre& spectre) const;

    /**
     * \brief Check the spectrum validity.
     * Invalidity is caused by: corrupted data, impossible calculation.
     *
     * @return Spectrum state (valid = true / non valid = false).
     */
    virtual bool isValid() const { return _valid; }

    /**
     * Force the validity state of the spectrum.
     */
    virtual void setValid(const bool& valid = true) { _valid = valid; }

    /// Get/Set the spectrum type.
    virtual TYSpectreType getType() const { return _type; }
    virtual void setType(TYSpectreType type) { _type = type; }

    /// Get the spectrum state.
    virtual TYSpectreEtat getEtat() const {return _etat; } ;
    virtual const TYSpectreEtat getEtat() {return _etat;};

    /// Force the spectrum state (to use carefully ...)
    void setEtat(TYSpectreEtat etat) { _etat = etat; }

    /// XXX These are the modulus to put into the solver model.
    /// Get the array of real values
    virtual double* getTabValReel() { return _module; }
    virtual const double* getTabValReel() const {return _module; }

    /// Number of values in the spectrum
    virtual unsigned int getNbValues() const ;

    /**
     * Initialize a spectrum to a default value.
     * @param valeur Value by default.
     */
    virtual void setDefaultValue(const double& valeur = TY_SPECTRE_DEFAULT_VALUE);

    /**
     * Set a new value to the frequencies array.
     *
     * @param freq Frequency to which a new value is given.
     * @param reel Value.
     */
    virtual void setValue(const float& freq, const double& reel = 0.0);

    /**
     * Get the value for a frequency.
     *
     * @param freq Frequency.
     *
     * @return The real number.
     */
    virtual double getValueReal(double freq);

    /**
    * Get an interval of real values from the frequencies/complex array
    *
    * @param[out] valeurs Values array. It should be allocated before.
    * @param[in] nbVal Number of values to retrieve from the spectrum
    * @param[in] decalage Index of the first value to retrieve
    */
    virtual void getRangeValueReal(double* valeurs, const short& nbVal, const short& decalage);

    /// Compute the global value dB[Lin] of a one-third Octave spectrum.
    virtual double valGlobDBLin() const;

    /// Compute the global value dB[A] of a one-third Octave spectrum.
    virtual double valGlobDBA() const;

    /// Converts to dB.
    virtual OSpectre toDB() const;

    /// Converts to physical quantity.
    virtual OSpectre toGPhy() const;

    /// Arithmetic sum of two spectrums in one-third Octave.
    virtual OSpectre sum(const OSpectre& spectre) const;

    /// Add a constant value to this spectrum
    virtual OSpectre sum(const double& valeur) const;

    /// Energetic sum of two spectrums in one-third Octave.
    virtual OSpectre sumdB(const OSpectre& spectre) const;

    /// Arithmetic subtract of two spectrums in one-third Octave.
    virtual OSpectre subst(const OSpectre& spectre) const;

    /// Subtract a constant value to this spectrum
    virtual OSpectre subst(const double& valeur) const;

    /// Multiplication of two spectrums.
    virtual OSpectre mult(const OSpectre& spectre) const;

    /// Multiply this spectrum par a double scalar.
    virtual OSpectre mult(const double& coefficient) const;

    /// Division of two spectrums.
    virtual OSpectre div(const OSpectre& spectre) const;

    /// Division of this spectrum by a double scalar.
    virtual OSpectre div(const double& coefficient) const;

    /// Division of a double constant by this spectrum.
    virtual OSpectre invMult(const double& coefficient = 1.0) const;

    /// Division of one by this spectrum.
    virtual OSpectre inv() const;

    /// Return a spectrum as this spectrum raised to a double power.
    virtual OSpectre power(const double& puissance) const;

    /// Compute the log base n of this spectrum (n=10 by default).
    virtual OSpectre log(const double& base = 10.0) const;

    /// Compute the root square of this spectrum.
    virtual OSpectre racine() const;

    /// Compute e^(coef * spectre) of this spectrum.
    virtual OSpectre exp(const double coef = 1.0);

    /// Compute the sin of this spectrum
    virtual OSpectre sin() const;

    /// Compute the cos of this spectrum
    virtual OSpectre cos() const;

    /// Return the absolute value of this spectrum
    virtual OSpectre abs() const;

    /**
     * \fn OSpectre sqrt() const
     * \brief Return the root square of a spectrum
     */
    virtual OSpectre sqrt() const;

    /**
     * \fn double valMax();
     * \brief Return the maximum value of a spectrum
     */
    virtual double valMax();

    /// Limit the spectrum values (min and max)
    virtual OSpectre seuillage(const double& min = -200.0, const double max = 200.0);

    /// Sum the values of the spectrum
    virtual double sigma();
    virtual const double sigma() const;

    /// Existence d'une tonalite marquee
    virtual bool isTonalite()const;

    /// Converts to one-third Octave.
    virtual OSpectre toTOct() const;

    /// Converts to Octave.
    virtual OSpectre toOct() const;

    // === STATIC FUNCTIONS

    /// Create a physical quantity spectrum
    static OSpectre getEmptyLinSpectre(const double& valInit = 1.0E-20);

    /// Make a spectrum in Octave.
    static OSpectre makeOctSpect();

    /// Return the index associated to a frequency.
    static int getIndice(const float& freq) { return _mapFreqIndice[freq]; }

    /// Define minimal frequency
    static void setFMin(const float& fMin) { _fMin = fMin; }
    /// Define maximal frequency
    static void setFMax(const float& fMax) { _fMax = fMax; }

    /**
     * \fn OTabFreq getTabFreqExact()
     * Return the array of exact frequencies.
     * The calling method should free the allocated memory for this array.
     *
     * \return The array of exact frequencies.
     */
    static OTabFreq getTabFreqExact(); // XXX These are the frequencies to use in solver

    /**
     * \fn OSpectre getOSpectreFreqExact()
     * \brief Return the spectrum of the exact frequencies.
     * \return An OSpectre
     */
    static OSpectre getOSpectreFreqExact();

    /// Construction du tableau frequence/indice
    static std::map<double, int>  setMapFreqIndice();

    /// Build a weighted spectrum A.
    static OSpectre pondA();

    /// Build a weighted spectrum B.
    static OSpectre pondB();

    /// Build a weighted spectrum C
    static OSpectre pondC();
    /// Print the spectrum
    virtual void printme() const;
    /**
     * Return a spectrum representating the wavelength associated to each frequency
     * @param c Wave propagating speed
     * @return An OSpectre
     */
    static OSpectre getLambda(const double& c);

    // Members
public:

protected:

    // ==== Static members
    // CAUTION Check how those static members behave in shared libraries

    /// Array of center frequencies (Hz) normalized in one-third Octave
    static const double _freqNorm[];

    /// Minimal frequency
    static double _fMin;

    /// Maximal frequency
    static double _fMax;

    /// Default value for the spectrum
    static double _defaultValue;

    /// Mapping between frequency and array index
    static std::map<double, int> _mapFreqIndice;

    // ==== Other members

    /// Spectrum validity
    bool _valid;

    /// Spectrum type
    TYSpectreType _type;

    /// Spectrum state (physical quantity or dB).
    TYSpectreEtat _etat;

    /// Representation of the spectrum: one-third Octave, Octave, constant delta f, unstructured.
    TYSpectreForm _form;

    /// Real values array for module
    double _module[TY_SPECTRE_DEFAULT_NB_ELMT];
};

::std::ostream& operator<<(::std::ostream& os, const OSpectre& s);

typedef std::vector<OSpectre> OTabSpectre; /// Spectrums vector
typedef std::vector<std::vector<OSpectre> > OTab2DSpectre; /// Spectrums 2D array

/**
 * Define a module/phase spectrum
*
 */
class OSpectreComplex : public OSpectre
{
    // Methods
public:
    OSpectreComplex();
    OSpectreComplex(const TYComplex& defaultValue);
    OSpectreComplex(const OSpectreComplex& other);
    /**
     * Constructor from a OSpectre (which is the module of the complex spectrum)
     */
    OSpectreComplex(const OSpectre& other);
    /**
     * Constructor from 2 OSpectre : module and phase
     */
    OSpectreComplex(const OSpectre& spectre1, const OSpectre& spectre2);

    virtual ~OSpectreComplex();
    /// operators
    OSpectreComplex& operator= (const OSpectreComplex& other);
    bool operator== (const OSpectreComplex& other) const;
    bool operator != (const OSpectreComplex& other) const;
    OSpectreComplex operator + (const OSpectreComplex& spectre) const;
    /// Product of two complex spectrums (module/phase)
    OSpectreComplex operator * (const OSpectreComplex& spectre) const;
    /// Computes the operation between a complex spectrum times a spectrum
    OSpectreComplex operator * (const OSpectre& spectre) const;
    /// Multiply a  complex spectrum by a double coefficient
    OSpectreComplex operator * (const double& coefficient) const;
    /// Divide a complex spectrum by another one
    virtual OSpectreComplex operator / (const OSpectreComplex& spectre) const;
    virtual OSpectreComplex div(const OSpectreComplex& spectre) const;

    /// Get an array of the real values of the spectrum
    virtual double* getTabValReel() { return _module; }
    virtual const double* getTabValReel() const {return _module; }
    /// Get an array of the imaginary values of the spectrum
    double* getTabValImag() { return _phase; }
    const double* getTabValImag() const { return _phase; }

    /**
     * Set a complex value to the array Frequency/Complex.
     *
     * @param freq Frequency.
     * @param reel Real number.
     * @param imag Imaginary number.
     */
    virtual void setValue(const float& freq, const double& reel, const double& imag = 0.0);
    /**
     * Set a complex value to the array Frequency/Complex.
     *
     * @param freq Frequency.
     * @param cplx A TYComplex complex number
     */
    virtual void setValue(const float& freq, const TYComplex& cplx);
    /**
     * Get for a frequency the imaginary number from the Frequency/Complex array
     *
     * @param freq Frequency.
     * @param pValid False if the frequency is not valid for this spectrum.
     *
     * @return Imaginary number
     */
    double getValueImag(float freq, bool* pValid = 0);
    /**
     * Set the phase to a spectrum.
     * @param spectre A OSpectre.
     */
    void setPhase(const OSpectre& spectre);
    /**
     * Set the phase to this spectrum.
     * @param valeur A double.
     */
    void setPhase(const double& valeur = 0.0);
    /**
     * Get the phase of this spectrum.
     * @return A OSpectre.
     */
    OSpectre getPhase() const;
    /**
    * Get the module of this spectrum.
    */
    OSpectre getModule() const;

    /// Conversion to module/phase
    OSpectreComplex toModulePhase() const;

    // === STATIC FUNCTIONS

    /// Build a OSpectreComplex complex spectrum
    static OSpectreComplex getCplxSpectre(const double& valInit = 1.0E-20);
    // Members
public:

protected:
    /// Array of the imaginary numbers (phase)
    double _phase[TY_SPECTRE_DEFAULT_NB_ELMT];
};

typedef std::vector<OSpectreComplex> OTabSpectreComplex; /// OTabSpectreComplex vector
typedef std::vector<std::vector<OSpectreComplex> > OTab2DSpectreComplex; /// OTabSpectreComplex 2D array

#endif // TY_MC_SPECTRE
