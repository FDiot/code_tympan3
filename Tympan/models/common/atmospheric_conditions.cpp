#include "Tympan/models/common/atmospheric_conditions.h"

#ifndef M_PI
  #define M_PI        3.1415926535897932384626433832795
#endif

const double AtmosphericConditions::Z_ref = 400.0;
const double AtmosphericConditions::reference_pressure = 101325.0;
const double AtmosphericConditions::reference_temperature = 293.15;
const double AtmosphericConditions::absolute_zero = 273.15;

AtmosphericConditions::AtmosphericConditions(
    double static_pressure_, double temperature_, double hygrometry_ ) :
    static_pressure(static_pressure_), temperature(temperature_), hygrometry(hygrometry_)
{
    compute_k();
    compute_absorption_spectrum();
}

void AtmosphericConditions::compute_absorption_spectrum()
{
    double f2;                                      // Carre de la frequence de travail
    double tA_classic, tA_N, tA_O;                  // Termes d'absoption classique, Azote Oxygene

    const double tK = temperature + absolute_zero;       // temperature absolue exprimee en Kelvin
    const double pr_rel = static_pressure / reference_pressure; // Pression relative
    const double t_rel  = tK / reference_temperature;     // Temperature relative

    const double h_molaire = compute_hm();

    // Frequences de relaxation de l'oxygene (frO) et de l'azote (frN)
    const double frO = pr_rel * (24.0 + (4.04 * 10000.0 * h_molaire * ((0.02 + h_molaire) / (0.391 + h_molaire))));
    const double frN = pr_rel * pow(t_rel, -0.5) * (9.0 + 280.0 * h_molaire * exp((-4.170 * (pow(t_rel, (-1.0 / 3.0)) - 1))));

    OTabFreq tabFreq = OSpectre::getTabFreqExact();


    for (unsigned int i = 0; i < absorption_spectrum.getNbValues() ; i++)
    {
        f2 = tabFreq[i] * tabFreq[i];

        tA_classic = 1.84e-11 * (1.0 / pr_rel) * sqrt(t_rel);
        tA_O = 0.01275 * exp(-2239.1 / tK) * (1.0 / (frO + (f2 / frO)));
        tA_N = 0.1068  * exp(-3352.0 / tK) * (1.0 / (frN + (f2 / frN)));

        absorption_spectrum.getTabValReel()[i] = (8.686 * f2 * (tA_classic + (pow(t_rel, -2.5) * (tA_O + tA_N))));
    }
}

double AtmosphericConditions::compute_c() const
{
    const double gamma = 1.41;
    const double R = 8.31441;
    const double M = 0.029;
    const double tK = temperature + absolute_zero;

    return sqrt((gamma * R * tK) / M);
}

double AtmosphericConditions::compute_z()
{
    const double R = 8.31441;                       // Constante des gazs parfaits
    const double tK = temperature + absolute_zero;
    const double M = 0.029;                         // Masse molaire de l'air

    return ( M * ( static_pressure / ( R * tK ) ) ) * compute_c();
}

void AtmosphericConditions::compute_k()
{
    double soundSpeed = compute_c();
    OTabFreq tabFreq = OSpectre::getTabFreqExact();

    for (unsigned int i = 0 ; i < tabFreq.size() ; i++)
    {
        wave_number.getTabValReel()[i] = (2 * M_PI * tabFreq[i]) / soundSpeed ;
    }
}

double AtmosphericConditions::compute_hm() const
{
    double T01 = absolute_zero + 0.01   ;       // Temperature isotherme au point triple
    double tK = temperature + absolute_zero;   // Temperature en Kelvin
    double pr_rel = static_pressure / reference_pressure;  // Pression relative

    double C = -6.8346 * pow((T01 / tK), 1.261) + 4.6151;

    double prSat_prRef = pow(10, C);

    return hygrometry * prSat_prRef * pr_rel;
}

OSpectre AtmosphericConditions::compute_length_absorption(double length) const
{
    return absorption_spectrum.mult(-(length / 2.0)).toGPhy();
}



