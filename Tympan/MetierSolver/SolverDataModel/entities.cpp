/**
 * \file entities.cpp
 *
 *  \date  nov. 6 2012
 *  \author Anthony Truchet <anthony.truchet@logilab.fr>
 *  \author Damien Garaud <damien.garaud@logilab.fr>
 */

#include "entities.hpp"

#include "relations.hpp"

namespace tympan
{

AcousticMaterialBase::AcousticMaterialBase(
    const string& name_
) : name(name_) {}

// ---------

AcousticBuildingMaterial::AcousticBuildingMaterial(
    const string& name_,
    const ComplexSpectrum& spectrum_
) : AcousticMaterialBase(name_), spectrum(spectrum_) {}

// ---------
AtmosphericConditions *AcousticGroundMaterial::atmosphere = NULL;

AcousticGroundMaterial::AcousticGroundMaterial(
    const string& name_, double resistivity_ ) : 
    AcousticMaterialBase(name_),  resistivity(resistivity_), thickness(1.0) 
{
    computeZc();
    computeK();
}

ComplexSpectrum AcousticGroundMaterial::get_absorption (double incidence_angle, double length)
{
    ComplexSpectrum Zs, Rp, W, Fw, Q;

    computeZs(incidence_angle, Zs);
    computeRp(incidence_angle, Zs, Rp);
    computeW(incidence_angle, length, Zs, W);
    computeFw(W, Fw);
    computeQ(incidence_angle, Rp, Fw, Q);

    Q = Q.toModulePhase();
    Q.setType(SPECTRE_TYPE_ABSO);

    return Q;
}


double AcousticGroundMaterial::get_ISO9613_G()
{
    return MIN( pow(300.0 / resistivity, 0.57), 1.0 );
}

void AcousticGroundMaterial::computeZc()
{
    double sigmaSurF;
    OTabFreq tabFreq = OSpectre::getTabFreqExact();  // On travaille en frequence reelle

    for (unsigned int i = 0 ; i < Zc.getNbValues() ; i++)
    {
        sigmaSurF = resistivity / tabFreq[i];

        Zc.getTabValReel()[i] = 1.0 +  9.08 * pow(sigmaSurF, 0.75);
        Zc.getTabValImag()[i] = 11.9  * pow(sigmaSurF, 0.73);
    }

    Zc.setType(SPECTRE_TYPE_AUTRE);
}

void AcousticGroundMaterial::computeK()
{
    double k_value, FSurSigma;

    OTabFreq tabFreq = OSpectre::getTabFreqExact();  // On travaille en frequence reelle

    for (unsigned int i = 0 ; i < K.getNbValues() ; i++)
    {
        FSurSigma = tabFreq[i] / resistivity;

        k_value = atmosphere->get_k().getTabValReel()[i];

        K.getTabValReel()[i] = k_value * (1 + 10.8 * pow(FSurSigma, -0.7));
        K.getTabValImag()[i] = k_value * (10.3 * pow(FSurSigma,  0.59));
    }

     K.setType(SPECTRE_TYPE_AUTRE);
}

 void AcousticGroundMaterial::computeZs(double angle, ComplexSpectrum& spectrum)
 {
    double k_value;

    TYComplex operand, K_value, Zc_value;
    double cosPhi = cos(angle);

    TYComplex cplxVal;

    for (unsigned int i = 0 ; i < Zc.getNbValues(); i++)
    {
        k_value = atmosphere->get_k().getTabValReel()[i];
        K_value = TYComplex(K.getTabValReel()[i], K.getTabValImag()[i]);
        Zc_value = TYComplex(Zc.getTabValReel()[i], Zc.getTabValImag()[i]);

        operand = std::sqrt(CPLX_UN - ((k_value * cosPhi / K_value) * (k_value * cosPhi / K_value)));

        cplxVal = Zc_value * cotanh(CPLX_MUN * CPLX_J * K_value * thickness * operand) / operand;
        spectrum.getTabValReel()[i] = cplxVal.real();
        spectrum.getTabValImag()[i] = cplxVal.imag();
    }
 }

 void AcousticGroundMaterial::computeRp(double angle, const ComplexSpectrum& Zs, ComplexSpectrum& Rp)
 {
//    const TYComplex CPLX_UN = TYComplex(1.0, 0.0);
    double sinPhi = sin(angle);
    TYComplex Z;

    for (unsigned int i = 0 ; i < Zs.getNbValues() ; i++)
    {
        Z = TYComplex(Zs.getTabValReel()[i], Zs.getTabValImag()[i]) ;

        TYComplex val = (Z * sinPhi - CPLX_UN) / (Z * sinPhi + CPLX_UN);

        Rp.getTabValReel()[i] = val.real();
        Rp.getTabValImag()[i] = val.imag();
    }
}

void AcousticGroundMaterial::computeW(double angle, double length, const ComplexSpectrum& Zs, ComplexSpectrum &W)
{
    TYComplex zs_value, invZs_value, cplxTempo, wTemp;

    double k_value;                       // nombre d'onde acoustique
    double sinPhi = sin(angle);

    for (unsigned int i = 0 ; i < W.getNbValues() ; i++)
    {
        k_value = atmosphere->get_k().getTabValReel()[i];
        zs_value = TYComplex(Zs.getTabValReel()[i], Zs.getTabValImag()[i]);

        invZs_value = 1.0 / zs_value;
        cplxTempo = (sinPhi + invZs_value) * (sinPhi + invZs_value);

        wTemp = std::sqrt(1.0 / 2.0 * CPLX_J * k_value * length * cplxTempo) ;

        W.getTabValReel()[i] = wTemp.real();
        W.getTabValImag()[i] = wTemp.imag();
    }
}

void AcousticGroundMaterial::computeFw(ComplexSpectrum localW, ComplexSpectrum& Fw)
{
   // localW is intentionnaly a copy of w (values may change)

    OSpectreComplex G;

    // Recherche et remplacement des parties reelles et imaginaires < epsilon par epsilon
    limit_W_values(localW);

    // Filtrage des differents cas
    erfc_G_computation(localW, G);

    // Operations en fonction du signe des parties reelles et imaginaires de w
    sgn_G_computation(localW, G);

    // Calcul de la fonction Fw
    double sqrt_pi = sqrt(M_PI);

    for (size_t i = 0; i < localW.getNbValues(); i++)
    {
        TYComplex v1 = TYComplex(localW.getTabValReel()[i], localW.getTabValImag()[i]);
        TYComplex v2 = TYComplex(G.getTabValReel()[i], G.getTabValImag()[i]);
        TYComplex val = CPLX_UN + CPLX_J * sqrt_pi * v1 * v2;
        Fw.getTabValReel()[i] = val.real();
        Fw.getTabValImag()[i] = val.imag();
    }
}

void AcousticGroundMaterial::limit_W_values(ComplexSpectrum& localW)
{
    const double epsilon = 1.0E-20;
    double valeur, tampon;

    for(size_t i = 0; i < localW.getNbValues() ; i++)
    {
        // Si partie reelle < epsilon alors partie reelle = epsilon
        tampon = localW.getTabValReel()[i];
        valeur = (fabs(tampon) < epsilon ? epsilon : tampon) ;
        localW.getTabValReel()[i] = valeur;

        // Si partie imaginaire < epsilon alors partie imaginaire = epsilon
        tampon = localW.getTabValImag()[i];
        valeur = (fabs(tampon) < epsilon ? epsilon : tampon) ;
        localW.getTabValImag()[i] = valeur;
    }
}

void AcousticGroundMaterial::erfc_G_computation(const ComplexSpectrum& localW, ComplexSpectrum& G)
{
    double reelW, imagW;
    TYComplex val;

    for (size_t i = 0 ; i < localW.getNbValues() ; i++)
    {
        reelW = localW.getTabValReel()[i];
        imagW = localW.getTabValImag()[i];

        if (((fabs(reelW) < 3.9) && (fabs(imagW) < 3.0)))                                 // Cas 1
        {
            val = erfcCas1(TYComplex(reelW, imagW));
        }
        else if (((fabs(reelW) >= 3.9) && (fabs(reelW) < 6.0)) ||                         // Cas 2
                 ((fabs(imagW) >= 3.0) && (fabs(imagW) < 6.0)))
        {
            val = erfcCas2(TYComplex(fabs(reelW), fabs(imagW)));
        }
        else                                                                        // Cas 3
        {
            val = erfcCas3(TYComplex(fabs(reelW), fabs(imagW)));
        }

        G.getTabValReel()[i] = val.real();
        G.getTabValImag()[i] = val.imag();
    }
}

void AcousticGroundMaterial::sgn_G_computation(const ComplexSpectrum& localW, ComplexSpectrum& G)
{
    double re, im;
    for (size_t i = 0 ; i < localW.getNbValues() ; i++)
    {
        re = localW.getTabValReel()[i];
        im = localW.getTabValImag()[i];

        TYComplex conjG = conj(TYComplex(G.getTabValReel()[i], G.getTabValImag()[i]));

        if ((re < 0) && (im > 0)) // imp
        {
            G.getTabValReel()[i] = conjG.real() ;
            G.getTabValImag()[i] = conjG.imag() ;
        }
        else if ((re > 0) && (im < 0)) //ipm
        {
            TYComplex v1 = TYComplex(localW.getTabValReel()[i], localW.getTabValImag()[i]);
            TYComplex v2 = TYComplex(G.getTabValReel()[i], G.getTabValImag()[i]);
            TYComplex tampon = sgnReIm(v1, v2);

            G.getTabValReel()[i] = tampon.real();
            G.getTabValImag()[i] = tampon.imag();               ;
        }
        else if ((re < 0) && (im < 0)) // imm
        {
            TYComplex v1 = TYComplex(localW.getTabValReel()[i], localW.getTabValImag()[i]);
            TYComplex v2 = TYComplex(G.getTabValReel()[i], G.getTabValImag()[i]);
            TYComplex tampon = sgnReIm(v1, v2);

            TYComplex val = conj(tampon);

            G.getTabValReel()[i] = val.real();
            G.getTabValImag()[i] = val.imag();
        }
        else if ((re > 0) && (im > 0)) //ipp
        {
        }
        else
        {
        }
    }
}

void AcousticGroundMaterial::computeQ(double angle, ComplexSpectrum &Rp, ComplexSpectrum &Fw, ComplexSpectrum &Q)
{
    TYComplex Rp_value, Fw_value ;

    for (unsigned int i = 0 ; i < Fw.getNbValues(); i++)
    {
        Rp_value = TYComplex(Rp.getTabValReel()[i], Rp.getTabValImag()[i]);
        Fw_value = TYComplex(Fw.getTabValReel()[i], Fw.getTabValImag()[i]);

        TYComplex val = Rp_value + (CPLX_UN - Rp_value) * Fw_value;

        Q.getTabValReel()[i] = val.real();
        Q.getTabValImag()[i] = val.imag();
    }
}

TYComplex AcousticGroundMaterial::erfcCas1(const TYComplex& wValue) const
{
    const double pi = M_PI;
    const int nbIterations = 5;  // Valeur normale = 5
    const double h = 0.8;        // valeur proposee = 0.8;
    const double h2 = h * h;

    double x = fabs(wValue.real());
    double y = fabs(wValue.imag());
    double x2 = x * x;
    double y2 = y * y;
    double modW = x2 + y2;

    double A1 = cos(2.0 * x * y);
    double B1 = sin(2.0 * x * y);
    double C1 = exp(-2.0 * pi * y / h) - cos(2.0 * pi * x / h) ;
    double D1 = sin(2.0 * pi * x / h);
    double P2 = 2.0 * exp(-(x2 + 2.0 * pi * y / h - y2)) * (A1 * C1 - B1 * D1) / (C1 * C1 + D1 * D1);
    double Q2 = 2.0 * exp(-(x2 + 2.0 * pi * y / h - y2)) * (A1 * D1 + B1 * C1) / (C1 * C1 + D1 * D1);

    double H0, H, K0, K;

    if (y < (pi / h))
    {
        H0 = h * y / (pi * modW) + P2;
    }
    else if (y == (pi / h))
    {
        H0 = h * y / (pi * modW) + (P2 / 2.0);
    }
    else
    {
        H0 = h * y / (pi * modW);
    }

    if (x < (pi / h))
    {
        K0 = h * x / (pi * modW) - Q2;
    }
    else if (x == (pi / h))
    {
        K0 = h * x / (pi * modW) - (Q2 / 2.0) ;
    }
    else
    {
        K0 = h * x / (pi * modW);
    }

    /* Solution MATLAB */
    H = H0;
    K = K0;
    double n2 = 0.0;
    double coeff_y = 2.0 * y * h / pi;
    double coeff_x = 2.0 * x * h / pi;
    double diviseur = 0.0;
    double expMn2h2 = 0.0;
    for (int n = 1 ; n <= nbIterations; n++)
    {
        n2 = (double)(n * n);
        diviseur = ((y2 - x2 + n2 * h2) * (y2 - x2 + n2 * h2)  + (4.0 * y2 * x2));
        expMn2h2 = exp(-n2 * h2);

        H = H + coeff_y * expMn2h2 * (modW + n2 * h2) / diviseur;
        K = K + coeff_x * expMn2h2 * (modW - n2 * h2) / diviseur;
    }

    return TYComplex(H, K);
}

TYComplex AcousticGroundMaterial::erfcCas2(const TYComplex& wValue) const
{
    TYComplex G ;
    TYComplex w2 = wValue * wValue;

    G = CPLX_J * wValue * ((0.4613135   / (w2 - 0.1901635))   +
                      (0.09999216  / (w2 - 1.7844927))   +
                      (0.002883894 / (w2 - 5.5253437)))  ;

    return G;
}

TYComplex AcousticGroundMaterial::erfcCas3(const TYComplex& wValue) const
{
    TYComplex G ;

    TYComplex w2 = wValue * wValue;

    G = CPLX_J * wValue * ((0.5124242  / (w2 - 0.2752551))  +
                      (0.05176536 / (w2 - 2.724745))) ;

    return G;
}

TYComplex AcousticGroundMaterial::sgnReIm(const TYComplex& W, const TYComplex& G) const
{
    double x = fabs(W.real());
    double y = fabs(W.imag());

    double cos2xy = cos(2 * x * y);
    double sin2xy = sin(2 * x * y);

    return 2.0 * exp(y * y - x * x) * (cos2xy + CPLX_J * sin2xy) - conj(G);
}

// ---------

const double AtmosphericConditions::Z_ref = 200.0;
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

double AtmosphericConditions::compute_c()
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

Spectrum AtmosphericConditions::compute_length_absorption(double length)
{
    return absorption_spectrum.mult(-(length / 2.0)).toGPhy();
}

// ---------

AcousticSource::AcousticSource(
    const Point& position_,
    const Spectrum& spectrum_ )
    : position(position_)
    , spectrum(spectrum_)
    , directivity(new SphericalSourceDirectivity())
{}

AcousticReceptor::AcousticReceptor(
    const Point& position_
) : position(position_) {}

AcousticEvent::AcousticEvent(
    const string& event_type_, const Point& point_
) : event_type(event_type_), point(point_) {}

AcousticProblem::AcousticProblem(
    const string& name_
) : name(name_) {}


SiteElement::SiteElement() {}

SiteAcousticReceptor::SiteAcousticReceptor(
    unsigned int id_
) : id(id_) {}

Site::Site(
    unsigned int id_
) : id(id_) {}


// XXX Copy/paste these methods to the \c AcousticGroundMaterial See
// Tympan/MetierSolver/DataManagerMetier/EltMateriaux/TYSol.h|TYSol.cpp These
// commented lines are here in order to list the methods of TYSol to use in the
// future AcousticGroundMaterial.

// /**
//  * Set/Get de l'impedance.
//  */
// const OSpectreComplex getImpedance() const { return *_pImpedance; }

// /**
// * Set/Get de l'impedance.
// */
// void setImpedance(const OSpectreComplex imp) { _pImpedance = new OSpectreComplex(imp); /*_pImpedance->setParent(this);*/ }

// /**
// * Calule le spectre d'absorption du sol
// * en fonction de l'angle d'incidence de l'onde acoustique.
// *
// * @param angle Angle d'incidence de l'onde acoustique.
// * @param Rr distance du trajet reflechi sur le sole entre le point de reflexion et le point suivant
// * @param pAtmo atmosphere courante du calcul
// */
// OSpectreComplex abso(const double& angle, const double& rR, const TYAtmosphere& Atmo) const;

// /**
// * Calcul de Zc, impedance caracteristique du sol a partir de
// * la resistance a l'ecoulement du sol exprimee en kRayl.
// * d'apres la formulation de DELANY-BAZLEY
// *
// * @return Le spectre de l'impedence caracteristique du sol
// */
// void calculZc() const;

// /**
// * Calcul de l'impedance specifique du sol Zc
// */
// void calculZs(const double phi, const TYAtmosphere& atmos) const;

// /**
// * Calcul de w d'apres : "H. JONASSON : The propagation and screening of wheel/rail noise"
// *
// * @param rR distance du trajet reflechi sur le sol entre la source et le point recepteur
// * @param phi angle d'incidence de l'onde acoustique
// *
// * @return w un OSpectreComplex
//      */
// OSpectreComplex calculW(const double rR, const double phi, const TYAtmosphere& atmos) const;

// public:  //Temporairement
// /**
// * Calcul de la fonction Fw facteur de perte pour l'effet de sol
// *
// * @param w LPTYSpectre
// * @return fw LPTYSpectre representant le facteur de perte
// */
// OSpectreComplex calculFw(const OSpectreComplex& w) const;

// /**
//  * Calcul de K, nombre d'onde dans la surface du sol
//  * d'apres la formulation de DELANY-BAZLEY
//  *
//  * @return Le spectre du nombre d'onde dans la surface du sol
//  */
// void calculNombreDOnde(const TYAtmosphere& atmos) const;
// /**
//  * Calcul du coefficient de reflexion du sol en onde plane
//  *
//  * @param phi Angle d'incidence de l'onde acoustique
//  *
//  * @return rP OSpectreComplex
//  */
// OSpectreComplex calculRp(const double& phi) const;

// /**
//  * Calcul du coefficient de reflexion du sol en ondes spheriques
//  *
//  * @param phi Angle d'incidence de l'onde acoustique
//  * @param rp coefficient de reflexion du sol en ondes planes
//  *
//  * @return Q OSpectreComplex
//  */
// OSpectreComplex calculQ(const double& phi, const OSpectreComplex& rp, const OSpectreComplex& fw) const ;

// /**
//  * \fn calculQ(const double& phi, const OSpectreComplex & fw) const
//  * \brief Calcul du coefficient de reflexion du sol en ondes spheriques
//  * \param phi Angle d'incidence de l'onde acoustique
//  * \param fw spectre
//  * \return Q OSpectreComplex
//  */
// OSpectreComplex calculQ(const double& phi, const OSpectreComplex& fw) const;

// /**
//  * Fonctions utilisees dans le calcul de la fonction Fw
//  *
//  * @param un complexe appartenant au vecteur [w]
//  *
//  * @return un complexe repondant au cas specifie
//  */
// TYComplex erfcCas1(const TYComplex& wValue) const;
// inline TYComplex erfcCas2(const TYComplex& wValue) const;
// inline TYComplex erfcCas3(const TYComplex& wValue) const;

// /**
//  * Fonction utilisee dans le traitement du signe de G
//  *
//  * @param TYComplex W Valeur locale de la fonction W
//  * @param TYComplex G Valeur locale de la fonction G
//  *
//  * @return TYComplex
//  */
// inline TYComplex sgnReIm(const TYComplex& W, const TYComplex& G) const ;



} /* namespace tympan */
