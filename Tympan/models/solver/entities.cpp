/**
 * \file entities.cpp
 *
 *  \date  nov. 6 2012
 *  \author Anthony Truchet <anthony.truchet@logilab.fr>
 *  \author Damien Garaud <damien.garaud@logilab.fr>
 */

#include "entities.hpp"
#include "Tympan/models/common/atmospheric_conditions.h"

namespace tympan
{
AcousticMaterialBase::AcousticMaterialBase(
    const string& name_
) : name(name_) {}

// ---------
AtmosphericConditions *AcousticGroundMaterial::atmosphere = new AtmosphericConditions(101325., 20., 70.);

AcousticBuildingMaterial::AcousticBuildingMaterial(
    const string& name_,
    const ComplexSpectrum& spectrum_
) : AcousticMaterialBase(name_), spectrum(spectrum_) {}

ComplexSpectrum AcousticBuildingMaterial::asEyring() const
{
    ComplexSpectrum eyring(spectrum);

    // 1-e^(-alphaS)

    return eyring.exp(-1.0).mult(-1.0).sum(1.0);
}

// ---------
AcousticGroundMaterial::AcousticGroundMaterial(
    const string& name_, double resistivity_ ) :
    AcousticMaterialBase(name_),  resistivity(resistivity_), thickness(1.0)
{
    init();
}

void AcousticGroundMaterial::init()
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

    AtmosphericConditions *CommonFaceDirectivity::atmosphere = new AtmosphericConditions(101325., 20., 70.);

// ---------
/*static*/ const double VolumeFaceDirectivity::_tabRA[] = {  1.0,
                                                          2.0,
                                                          3.0,
                                                          4.0,
                                                          5.0,
                                                          10.0,
                                                          20.0,
                                                          30.0,
                                                          40.0,
                                                          50.0,
                                                          100.0,
                                                          200.0,
                                                          300.0,
                                                       };
/*static*/ const double VolumeFaceDirectivity::_tabCor[] = {0.608,
                                                         0.817,
                                                         0.879,
                                                         0.909,
                                                         0.928,
                                                         0.964,
                                                         0.982,
                                                         0.988,
                                                         0.991,
                                                         0.993,
                                                         0.996,
                                                         0.998,
                                                         1.000
                                                        };
double VolumeFaceDirectivity::calculC(double distance)
{
    double X = distance / support_size;

    if (X < 1.0) { return _tabCor[0]; }
    if (X > 300.0) { return 1.0; }

    int i = 1;
    while (_tabRA[i] < X && i < 12) { i++; } // Recherche de l'indice le plus proche

    // Calcul de la valeur de c par interpolation lineaire
    return (_tabCor[i] - _tabCor[i - 1]) / (_tabRA[i] - _tabRA[i - 1]) * (X - _tabRA[i - 1]) + _tabCor[i - 1];
}

Spectrum VolumeFaceDirectivity::lwAdjustment(Vector direction, double distance)
{
    Spectrum directivity_spectrum;
    double q, ka;

    // Angle angle between support normal and (source to receptor) vector
    double cosPhi = cos( support_normal.angle( direction ) );

    double C = calculC(distance);  //Coefficient de directivite

    for (unsigned int i = 0 ; i < directivity_spectrum.getNbValues() ; i++)
    {
        ka = atmosphere->get_k().getTabValReel()[i] * support_size ;

        q = (1 + (ka / (ka + 0.4)) * cosPhi) * C;

        directivity_spectrum.getTabValReel()[i] = q;
    }

    return directivity_spectrum;
}
//
// ----------------------------------------
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
const double ChimneyFaceDirectivity::_tabQ[NB_KA][NB_THETA] =
{
    {
        /*ka1*/  3.9213E-02, 3.8299E-02, 3.5626E-02, 3.1399E-02, 2.5933E-02, 1.9633E-02, 1.2955E-02,
        6.3631E-03, 2.9321E-04, -4.8880E-03, -8.9124E-03, -1.1631E-02, -1.3023E-02, -1.3192E-02,
        -1.2351E-02, -1.0793E-02, -8.8587E-03, -6.8993E-03, -5.2361E-03, -4.1269E-03, -3.7380E-03
    },
    {
        /*ka2*/  1.5295E-01, 1.4940E-01, 1.3903E-01, 1.2260E-01, 1.0132E-01, 7.6753E-02, 5.0654E-02,
        2.4837E-02, 1.0112E-03, -1.9370E-02, -3.5229E-02, -4.5960E-02, -5.1463E-02, -5.2134E-02,
        -4.8805E-02, -4.2637E-02, -3.4980E-02, -2.7223E-02, -2.0640E-02, -1.6250E-02, -1.4711E-02
    },
    {
        /*ka3*/  3.3341E-01, 3.2573E-01, 3.0323E-01, 2.6755E-01, 2.2126E-01, 1.6765E-01, 1.1054E-01,
        5.3866E-02, 1.4017E-03, -4.3595E-02, -7.8677E-02, -1.0242E-01, -1.1455E-01, -1.1590E-01,
        -1.0831E-01, -9.4383E-02, -7.7136E-02, -5.9688E-02, -4.4893E-02, -3.5032E-02, -3.1576E-02
    },
    {
        /*ka4*/  5.7143E-01, 5.5835E-01, 5.2003E-01, 4.5914E-01, 3.7992E-01, 2.8790E-01, 1.8948E-01,
        9.1440E-02, 3.3045E-04, -7.8073E-02, -1.3934E-01, -1.8081E-01, -2.0183E-01, -2.0382E-01,
        -1.8997E-01, -1.6488E-01, -1.3394E-01, -1.0271E-01, -7.6267E-02, -5.8657E-02, -5.2487E-02
    },
    {
        /*ka5*/  8.5759E-01, 8.3811E-01, 7.8096E-01, 6.8997E-01, 5.7125E-01, 4.3285E-01, 2.8419E-01,
        1.3542E-01, -3.4683E-03, -1.2348E-01, -2.1752E-01, -2.8120E-01, -3.1323E-01, -3.1559E-01,
        -2.9322E-01, -2.5329E-01, -2.0429E-01, -1.5494E-01, -1.1321E-01, -8.5459E-02, -7.5741E-02
    },
    {
        /*ka6*/  1.1831E+00, 1.1564E+00, 1.0781E+00, 9.5307E-01, 7.8950E-01, 5.9808E-01, 3.9154E-01,
        1.8377E-01, -1.1197E-02, -1.8049E-01, -3.1367E-01, -4.0394E-01, -4.4902E-01, -4.5143E-01,
        -4.1811E-01, -3.5943E-01, -2.8771E-01, -2.1567E-01, -1.5487E-01, -1.1446E-01, -1.0033E-01
    },
    {
        /*ka7*/  1.5403E+00, 1.5058E+00, 1.4044E+00, 1.2424E+00, 1.0296E+00, 7.7974E-01, 5.0881E-01,
        2.3480E-01, -2.3835E-02, -2.4970E-01, -4.2827E-01, -5.4964E-01, -6.0999E-01, -6.1221E-01,
        -5.6552E-01, -4.8406E-01, -3.8483E-01, -2.8537E-01, -2.0156E-01, -1.4594E-01, -1.2649E-01
    },
    {
        /*ka8*/  1.9232E+00, 1.8805E+00, 1.7546E+00, 1.5530E+00, 1.2876E+00, 9.7474E-01, 6.3391E-01,
        2.8725E-01, -4.2025E-02, -3.3151E-01, -5.6184E-01, -7.1921E-01, -7.9754E-01, -7.9976E-01,
        -7.3751E-01, -6.2935E-01, -4.9783E-01, -3.6618E-01, -2.5541E-01, -1.8197E-01, -1.5631E-01
    },
    {
        /*ka9*/  2.3273E+00, 2.2758E+00, 2.1243E+00, 1.8812E+00, 1.5605E+00, 1.1808E+00, 7.6540E-01,
        3.4042E-01, -6.6025E-02, -4.2610E-01, -7.1493E-01, -9.1390E-01, -1.0138E+00, -1.0170E+00,
        -9.3769E-01, -7.9931E-01, -6.3089E-01, -4.6239E-01, -3.2076E-01, -2.2695E-01, -1.9418E-01
    },
    {
        /*ka10*/ 2.7492E+00, 2.6887E+00, 2.5106E+00, 2.2244E+00, 1.8459E+00, 1.3965E+00, 9.0236E-01,
        3.9399E-01, -9.5749E-02, -5.3340E-01, -8.8806E-01, -1.1353E+00, -1.2618E+00, -1.2684E+00,
        -1.1715E+00, -1.0001E+00, -7.9063E-01, -5.8088E-01, -4.0461E-01, -2.8794E-01, -2.4721E-01
    },
    {
        /*ka11*/ 3.1894E+00, 3.1197E+00, 2.9141E+00, 2.5834E+00, 2.1451E+00, 1.6229E+00, 1.0464E+00,
        4.4972E-01, -1.2959E-01, -6.5256E-01, -1.0819E+00, -1.3865E+00, -1.5474E+00, -1.5627E+00,
        -1.4504E+00, -1.2450E+00, -9.9164E-01, -7.3698E-01, -5.2275E-01, -3.8095E-01, -3.3145E-01
    },
    {
        /*ka12*/ 3.6467E+00, 3.5674E+00, 3.3337E+00, 2.9572E+00, 2.4572E+00, 1.8599E+00, 1.1978E+00,
        5.0838E-01, -1.6650E-01, -7.8278E-01, -1.2969E+00, -1.6703E+00, -1.8769E+00, -1.9100E+00,
        -1.7875E+00, -1.5497E+00, -1.2509E+00, -9.4848E-01, -6.9332E-01, -5.2427E-01, -4.6524E-01
    },
    {
        /*ka13*/ 4.0966E+00, 4.0077E+00, 3.7455E+00, 3.3227E+00, 2.7605E+00, 2.0874E+00, 1.3388E+00,
        5.5573E-01, -2.1615E-01, -9.2821E-01, -1.5310E+00, -1.9786E+00, -2.2366E+00, -2.2918E+00,
        -2.1605E+00, -1.8882E+00, -1.5398E+00, -1.1845E+00, -8.8398E-01, -6.8475E-01, -6.1517E-01
    },
    {
        /*ka14*/ 4.5369E+00, 4.4382E+00, 4.1471E+00, 3.6774E+00, 3.0522E+00, 2.3026E+00, 1.4670E+00,
        5.8981E-01, -2.7973E-01, -1.0889E+00, -1.7833E+00, -2.3100E+00, -2.6256E+00, -2.7083E+00,
        -2.5709E+00, -2.2632E+00, -1.8614E+00, -1.4484E+00, -1.0982E+00, -8.6591E-01, -7.8478E-01
    },
    {
        /*ka15*/ 4.9652E+00, 4.8566E+00, 4.5361E+00, 4.0188E+00, 3.3298E+00, 2.5030E+00, 1.5798E+00,
        6.0833E-01, -3.5883E-01, -1.2655E+00, -2.0530E+00, -2.6626E+00, -3.0418E+00, -3.1586E+00,
        -3.0192E+00, -2.6764E+00, -2.2180E+00, -1.7428E+00, -1.3386E+00, -1.0703E+00, -9.7666E-01
    },
    {
        /*ka16*/ 5.3802E+00, 5.2615E+00, 4.9111E+00, 4.3454E+00, 3.5916E+00, 2.6865E+00, 1.6752E+00,
        6.0927E-01, -4.5493E-01, -1.4582E+00, -2.3388E+00, -3.0336E+00, -3.4819E+00, -3.6400E+00,
        -3.5043E+00, -3.1278E+00, -2.6105E+00, -2.0689E+00, -1.6065E+00, -1.2994E+00, -1.1921E+00
    },
    {
        /*ka17*/ 5.7809E+00, 5.6519E+00, 5.2709E+00, 4.6558E+00, 3.8360E+00, 2.8514E+00, 1.7510E+00,
        5.9048E-01, -5.6989E-01, -1.6679E+00, -2.6400E+00, -3.4203E+00, -3.9418E+00, -4.1484E+00,
        -4.0235E+00, -3.6166E+00, -3.0391E+00, -2.4271E+00, -1.9025E+00, -1.5536E+00, -1.4318E+00
    },
    {
        /*ka18*/ 6.1672E+00, 6.0276E+00, 5.6153E+00, 4.9496E+00, 4.0623E+00, 2.9966E+00, 1.8057E+00,
        5.5010E-01, -7.0558E-01, -1.8960E+00, -2.9562E+00, -3.8199E+00, -4.4165E+00, -4.6786E+00,
        -4.5731E+00, -4.1408E+00, -3.5029E+00, -2.8170E+00, -2.2262E+00, -1.8328E+00, -1.6954E+00
    },
    {
        /*ka19*/ 6.5395E+00, 6.3890E+00, 5.9446E+00, 5.2269E+00, 4.2703E+00, 3.1214E+00, 1.8382E+00,
        4.8645E-01, -8.6398E-01, -2.1439E+00, -3.2877E+00, -4.2304E+00, -4.9013E+00, -5.2244E+00,
        -5.1475E+00, -4.6969E+00, -4.0000E+00, -3.2376E+00, -2.5767E+00, -2.1360E+00, -1.9820E+00
    },
    {
        /*ka20*/ 6.8987E+00, 6.7370E+00, 6.2595E+00, 5.4883E+00, 4.4602E+00, 3.2257E+00, 1.8476E+00,
        3.9797E-01, -1.0472E+00, -2.4141E+00, -3.6356E+00, -4.6504E+00, -5.3917E+00, -5.7788E+00,
        -5.7401E+00, -5.2805E+00, -4.5278E+00, -3.6868E+00, -2.9524E+00, -2.4616E+00, -2.2901E+00
    },
    {
        /*ka21*/ 7.2462E+00, 7.0730E+00, 6.5614E+00, 5.7349E+00, 4.6328E+00, 3.3096E+00, 1.8335E+00,
        2.8322E-01, -1.2576E+00, -2.7091E+00, -4.0024E+00, -5.0802E+00, -5.8841E+00, -6.3348E+00,
        -6.3433E+00, -5.8859E+00, -5.0828E+00, -4.1626E+00, -3.3514E+00, -2.8078E+00, -2.6178E+00
    },
    {
        /*ka22*/ 7.5836E+00, 7.3985E+00, 6.8516E+00, 5.9679E+00, 4.7890E+00, 3.3735E+00, 1.7953E+00,
        1.4071E-01, -1.4979E+00, -3.0326E+00, -4.3913E+00, -5.5215E+00, -6.3766E+00, -6.8863E+00,
        -6.9488E+00, -6.5063E+00, -5.6609E+00, -4.6621E+00, -3.7715E+00, -3.1726E+00, -2.9630E+00
    },
    {
        /*ka23*/ 7.9125E+00, 7.7152E+00, 7.1317E+00, 6.1886E+00, 4.9299E+00, 3.4179E+00, 1.7326E+00,
        -3.1215E-02, -1.7709E+00, -3.3886E+00, -4.8069E+00, -5.9777E+00, -6.8694E+00, -7.4281E+00,
        -7.5480E+00, -7.1343E+00, -6.2574E+00, -5.1825E+00, -4.2103E+00, -3.5537E+00, -3.3237E+00
    },
    {
        /*ka24*/ 8.2345E+00, 8.0244E+00, 7.4032E+00, 6.3983E+00, 5.0563E+00, 3.4431E+00, 1.6449E+00,
        -2.3448E-01, -2.0804E+00, -3.7824E+00, -5.2552E+00, -6.4540E+00, -7.3646E+00, -7.9573E+00,
        -8.1330E+00, -7.7614E+00, -6.8670E+00, -5.7206E+00, -4.6657E+00, -3.9491E+00, -3.6977E+00
    },
    {
        /*ka25*/ 8.5510E+00, 8.3278E+00, 7.6673E+00, 6.5982E+00, 5.1690E+00, 3.4493E+00, 1.5312E+00,
        -4.7147E-01, -2.4305E+00, -4.2200E+00, -5.7434E+00, -6.9574E+00, -7.8668E+00, -8.4733E+00,
        -8.6970E+00, -8.3789E+00, -7.4837E+00, -6.2733E+00, -5.1355E+00, -4.3570E+00, -4.0835E+00
    },
    {
        /*ka26*/ 8.8632E+00, 8.6262E+00, 7.9252E+00, 6.7892E+00, 5.2687E+00, 3.4364E+00, 1.3906E+00,
        -7.4501E-01, -2.8265E+00, -4.7089E+00, -6.2803E+00, -7.4966E+00, -8.3829E+00, -8.9783E+00,
        -9.2352E+00, -8.9780E+00, -8.1010E+00, -6.8372E+00, -5.6177E+00, -4.7758E+00, -4.4793E+00
    },
    {
        /*ka27*/ 9.1717E+00, 8.9207E+00, 8.1775E+00, 6.9719E+00, 5.3555E+00, 3.4041E+00, 1.2212E+00,
        -1.0587E+00, -3.2745E+00, -5.2580E+00, -6.8767E+00, -8.0824E+00, -8.9219E+00, -9.4773E+00,
        -9.7454E+00, -9.5504E+00, -8.7116E+00, -7.4086E+00, -6.1105E+00, -5.2042E+00, -4.8840E+00
    },
    {
        /*ka28*/ 9.4773E+00, 9.2117E+00, 8.4247E+00, 7.1465E+00, 5.4296E+00, 3.3517E+00, 1.0212E+00,
        -1.4170E+00, -3.7824E+00, -5.8785E+00, -7.5458E+00, -8.7278E+00, -9.4948E+00, -9.9776E+00,
        -1.0228E+01, -1.0089E+01, -9.3081E+00, -7.9839E+00, -6.6123E+00, -5.6413E+00, -5.2969E+00
    },
    {
        /*ka29*/ 9.7800E+00, 9.4993E+00, 8.6669E+00, 7.3131E+00, 5.4905E+00, 3.2779E+00, 7.8776E-01,
        -1.8253E+00, -4.3595E+00, -6.5844E+00, -8.3040E+00, -9.4487E+00, -1.0115E+01, -1.0489E+01,
        -1.0688E+01, -1.0589E+01, -9.8825E+00, -8.5589E+00, -7.1217E+00, -6.0864E+00, -5.7173E+00
    },
    {
        /*ka30*/ 1.0080E+01, 9.7834E+00, 8.9039E+00, 7.4713E+00, 5.5375E+00, 3.1815E+00, 5.1771E-01,
        -2.2903E+00, -5.0181E+00, -7.3937E+00, -9.1727E+00, -1.0265E+01, -1.0798E+01, -1.1024E+01,
        -1.1130E+01, -1.1049E+01, -1.0427E+01, -9.1291E+00, -7.6373E+00, -6.5392E+00, -6.1452E+00
    },
    {
        /*ka31*/ 1.0376E+01, 1.0064E+01, 9.1354E+00, 7.6206E+00, 5.5698E+00, 3.0605E+00, 2.0709E-01,
        -2.8202E+00, -5.7735E+00, -8.3303E+00, -1.0180E+01, -1.1202E+01, -1.1563E+01, -1.1594E+01,
        -1.1565E+01, -1.1468E+01, -1.0935E+01, -9.6893E+00, -8.1576E+00, -6.9995E+00, -6.5807E+00
    },
    {
        /*ka32*/ 1.0668E+01, 1.0339E+01, 9.3604E+00, 7.7600E+00, 5.5861E+00, 2.9127E+00, -1.4879E-01,
        -3.4253E+00, -6.6463E+00, -9.4274E+00, -1.1367E+01, -1.2295E+01, -1.2433E+01, -1.2217E+01,
        -1.2003E+01, -1.1852E+01, -1.1400E+01, -1.0234E+01, -8.6812E+00, -7.4676E+00, -7.0243E+00
    },
    {
        /*ka33*/ 1.0956E+01, 1.0609E+01, 9.5782E+00, 7.8886E+00, 5.5849E+00, 2.7355E+00, -5.5553E-01,
        -4.1185E+00, -7.6641E+00, -1.0733E+01, -1.2791E+01, -1.3593E+01, -1.3439E+01, -1.2909E+01,
        -1.2457E+01, -1.2206E+01, -1.1820E+01, -1.0758E+01, -9.2063E+00, -7.9436E+00, -7.4766E+00
    },
    {
        /*ka34*/ 1.1237E+01, 1.0873E+01, 9.7877E+00, 8.0051E+00, 5.5648E+00, 2.5261E+00, -1.0198E+00,
        -4.9166E+00, -8.8666E+00, -1.2322E+01, -1.4549E+01, -1.5171E+01, -1.4623E+01, -1.3692E+01,
        -1.2939E+01, -1.2540E+01, -1.2193E+01, -1.1254E+01, -9.7309E+00, -8.4279E+00, -7.9385E+00
    },
    {
        /*ka35*/ 1.1512E+01, 1.1129E+01, 9.9878E+00, 8.1084E+00, 5.5239E+00, 2.2812E+00, -1.5494E+00,
        -5.8416E+00, -1.0313E+01, -1.4320E+01, -1.6807E+01, -1.7156E+01, -1.6046E+01, -1.4592E+01,
        -1.3464E+01, -1.2864E+01, -1.2521E+01, -1.1718E+01, -1.0253E+01, -8.9208E+00, -8.4107E+00
    },
    {
        /*ka36*/ 1.1778E+01, 1.1376E+01, 1.0177E+01, 8.1972E+00, 5.4605E+00, 1.9974E+00, -2.1540E+00,
        -6.9238E+00, -1.2098E+01, -1.6967E+01, -1.9918E+01, -1.9789E+01, -1.7805E+01, -1.5641E+01,
        -1.4046E+01, -1.3190E+01, -1.2810E+01, -1.2145E+01, -1.0769E+01, -9.4225E+00, -8.8943E+00
    },
    {
        /*ka37*/ 1.2035E+01, 1.1614E+01, 1.0355E+01, 8.2703E+00, 5.3729E+00, 1.6709E+00, -2.8454E+00,
        -8.2070E+00, -1.4393E+01, -2.0830E+01, -2.4847E+01, -2.3638E+01, -2.0073E+01, -1.6886E+01,
        -1.4703E+01, -1.3528E+01, -1.3066E+01, -1.2530E+01, -1.1275E+01, -9.9329E+00, -9.3900E+00
    },
    {
        /*ka38*/ 1.2282E+01, 1.1841E+01, 1.0521E+01, 8.3267E+00, 5.2594E+00, 1.2974E+00, -3.6384E+00,
        -9.7578E+00, -1.7557E+01, -2.7951E+01, -3.7266E+01, -3.0744E+01, -2.3208E+01, -1.8396E+01,
        -1.5453E+01, -1.3891E+01, -1.3297E+01, -1.2873E+01, -1.1769E+01, -1.0452E+01, -9.8987E+00
    }
};

Spectrum ChimneyFaceDirectivity::lwAdjustment(Vector direction, double distance)
{
    Spectrum directivity_spectrum;

    Spectrum spectre_ka = atmosphere->get_k().mult(support_size);

    double theta = direction.angle(support_normal); // Angle du segment par rapport a la normale au support de la source

    // angle compris entre 0 et pi
    if (theta < -M_PI) { theta = theta + M_2PI; }
    if (theta >  M_PI) { theta = theta - M_2PI; }
    theta = ABS(theta); // On prend la valeur absolue de theta

    int indice_theta = (int)(20 * theta / M_PI); // Indice de l'angle theta dans le tableau
    for (unsigned int i = 0 ; i < spectre_ka.getNbValues(); i++)
    {
        double ka = spectre_ka.getTabValReel()[i];
        ka = ka > 3.8 ? 3.8 : ka ;
        int indice_Ka = (int)(10 * ka);

        directivity_spectrum.getTabValReel()[i] = compute_q(indice_Ka, indice_theta, ka, theta);
    }

    return directivity_spectrum;
}

double ChimneyFaceDirectivity::compute_q(int ka_idx, int theta_idx, double ka, double theta)
{
    /* ===========================================================================================
       Recherche par interpolation lineaire dans le tableau _tabQ
       de la valeur de Q la plus proche
       ===========================================================================================*/
    int indiceKaTab = ka_idx > 0 ? ka_idx - 1 : 0; // Eviter les depassement de tableau
    indiceKaTab = indiceKaTab > (NB_KA - 2) ? NB_KA - 2 : indiceKaTab; // Eviter les depassement de tableau

    double tabQ1_1 = _tabQ[indiceKaTab]    [theta_idx];     //_tabQ[indice_Ka]  [indice_theta]
    double tabQ1_2 = _tabQ[indiceKaTab]    [theta_idx + 1]; //_tabQ[indice_Ka]  [indice_theta+1]
    double tabQ2_1 = _tabQ[indiceKaTab + 1][theta_idx];     //_tabQ[indice_Ka+1][indice_theta]
    double tabQ2_2 = _tabQ[indiceKaTab + 1][theta_idx + 1]; //_tabQ[indice_Ka+1][indice_theta+1]

    double ka1 = ka_idx / 10.0;
    double ka2 = (ka_idx + 1) / 10.0;

    double theta1 = theta_idx * M_PI / 20.0;
    double theta2 = (theta_idx + 1) * M_PI / 20.0;

    double val1 = tabQ1_1 + (theta - theta1) * (tabQ1_2 - tabQ1_1) / (theta2 - theta1);
    double val2 = tabQ2_1 + (theta - theta1) * (tabQ2_2 - tabQ2_1) / (theta2 - theta1);

    double val = val1 + (ka2 - ka) * (val2 - val1) / (ka2 - ka1);


    return pow(10.0, val / 10.0);
}
//
// --------------------------
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

const double BaffledFaceDirectivity::_tabKa[NB_KA] = { 0.05, 0.1, 0.25, 0.5, 1.0, 2.0, 5.0, 10.0, 20.0 };

const double BaffledFaceDirectivity::_tabQ[NB_KA][NB_THETA] =
{
    {
        /*ka=0,05*/ 2.0382E+00, 2.0382E+00, 1.7752E+00, 1.7752E+00, 2.0382E+00, 2.0382E+00, 1.6953E+00,
        1.4101E+00, 1.2860E+00, 1.1729E+00, 8.1142E-01, 6.0152E-01, 4.4591E-01, 3.3056E-01,
        2.4505E-01, 1.8165E-01, 1.3466E-01, 9.9827E-02, 7.4003E-02, 5.4859E-02, 4.0667E-02,
        5.4859E-02, 7.4003E-02, 9.9827E-02, 1.3466E-01, 1.8165E-01, 2.4505E-01, 3.3056E-01,
        4.4591E-01, 6.0152E-01, 8.1142E-01, 1.1729E+00, 1.2860E+00, 1.4101E+00, 1.6953E+00,
        2.0382E+00, 2.0382E+00, 1.7752E+00, 1.7752E+00, 2.0382E+00, 2.0382E+00
    },
    {
        /*ka=0,1*/  1.0215E+00, 1.0215E+00, 1.0215E+00, 1.1201E+00, 1.2860E+00, 1.2860E+00, 1.2860E+00,
        1.4765E+00, 1.6190E+00, 1.6190E+00, 1.6190E+00, 1.1201E+00, 7.7490E-01, 5.3610E-01,
        3.7089E-01, 2.5659E-01, 1.7752E-01, 1.2281E-01, 8.4966E-02, 5.8782E-02, 4.0667E-02,
        5.8782E-02, 8.4966E-02, 1.2281E-01, 1.7752E-01, 2.5659E-01, 3.7089E-01, 5.3610E-01,
        7.7490E-01, 1.1201E+00, 1.6190E+00, 1.6190E+00, 1.6190E+00, 1.4765E+00, 1.2860E+00,
        1.2860E+00, 1.2860E+00, 1.1201E+00, 1.0215E+00, 1.0215E+00, 1.0215E+00
    },
    {
        /*ka=0,25*/ 2.1534E+00, 1.7911E+00, 1.4898E+00, 1.3587E+00, 1.4227E+00, 1.7105E+00, 1.7105E+00,
        1.4898E+00, 1.3587E+00, 1.3587E+00, 1.3587E+00, 9.6189E-01, 6.8096E-01, 4.8209E-01,
        3.4129E-01, 2.4162E-01, 1.7105E-01, 1.2109E-01, 8.5728E-02, 6.0691E-02, 4.2966E-02,
        6.0691E-02, 8.5728E-02, 1.2109E-01, 1.7105E-01, 2.4162E-01, 3.4129E-01, 4.8209E-01,
        6.8096E-01, 9.6189E-01, 1.3587E+00, 1.3587E+00, 1.3587E+00, 1.4898E+00, 1.7105E+00,
        1.7105E+00, 1.4227E+00, 1.3587E+00, 1.4898E+00, 1.7911E+00, 2.1534E+00
    },
    {
        /*ka=0,5*/  1.4385E+00, 1.4385E+00, 1.4385E+00, 1.5773E+00, 1.8109E+00, 1.8109E+00, 2.1772E+00,
        1.9857E+00, 1.5063E+00, 1.0912E+00, 9.0762E-01, 6.7283E-01, 4.9877E-01, 3.6975E-01,
        2.7410E-01, 2.0319E-01, 1.5063E-01, 1.1166E-01, 8.2776E-02, 6.1363E-02, 4.5489E-02,
        6.1363E-02, 8.2776E-02, 1.1166E-01, 1.5063E-01, 2.0319E-01, 2.7410E-01, 3.6975E-01,
        4.9877E-01, 6.7283E-01, 9.0762E-01, 1.0912E+00, 1.5063E+00, 1.9857E+00, 2.1772E+00,
        1.8109E+00, 1.8109E+00, 1.5773E+00, 1.4385E+00, 1.4385E+00, 1.4385E+00
    },
    {
        /*ka=1*/    3.5678E+00, 4.2895E+00, 4.4916E+00, 3.7360E+00, 2.7065E+00, 2.2511E+00, 1.2954E+00,
        8.5586E-01, 7.8055E-01, 8.5586E-01, 7.1187E-01, 5.4001E-01, 4.0964E-01, 3.1074E-01,
        2.3572E-01, 1.7881E-01, 1.3564E-01, 1.0290E-01, 7.8055E-02, 5.9211E-02, 4.4916E-02,
        5.9211E-02, 7.8055E-02, 1.0290E-01, 1.3564E-01, 1.7881E-01, 2.3572E-01, 3.1074E-01,
        4.0964E-01, 5.4001E-01, 7.1187E-01, 8.5586E-01, 7.8055E-01, 8.5586E-01, 1.2954E+00,
        2.2511E+00, 2.7065E+00, 3.7360E+00, 4.4916E+00, 4.2895E+00, 3.5678E+00
    },
    {
        /*ka=2*/    1.1891E+01, 9.8907E+00, 7.1652E+00, 4.5209E+00, 2.6015E+00, 1.4970E+00, 1.2452E+00,
        7.8565E-01, 5.4353E-01, 4.5209E-01, 3.7603E-01, 3.0565E-01, 2.4844E-01, 2.0194E-01,
        1.6414E-01, 1.3342E-01, 1.0845E-01, 8.8151E-02, 7.1652E-02, 5.8241E-02, 4.7340E-02,
        5.8241E-02, 7.1652E-02, 8.8151E-02, 1.0845E-01, 1.3342E-01, 1.6414E-01, 2.0194E-01,
        2.4844E-01, 3.0565E-01, 3.7603E-01, 4.5209E-01, 5.4353E-01, 7.8565E-01, 1.2452E+00,
        1.4970E+00, 2.6015E+00, 4.5209E+00, 7.1652E+00, 9.8907E+00, 1.1891E+01
    },
    {
        /*ka=5*/    4.5941E+00, 4.5941E+00, 4.5941E+00, 4.1899E+00, 3.1784E+00, 1.8290E+00, 1.2653E+00,
        1.0051E+00, 8.3600E-01, 6.9535E-01, 5.7837E-01, 4.4896E-01, 3.4850E-01, 2.7052E-01,
        2.0999E-01, 1.6301E-01, 1.2653E-01, 9.8221E-02, 7.6244E-02, 5.9184E-02, 4.5941E-02,
        5.9184E-02, 7.6244E-02, 9.8221E-02, 1.2653E-01, 1.6301E-01, 2.0999E-01, 2.7052E-01,
        3.4850E-01, 4.4896E-01, 5.7837E-01, 6.9535E-01, 8.3600E-01, 1.0051E+00, 1.2653E+00,
        1.8290E+00, 3.1784E+00, 4.1899E+00, 4.5941E+00, 4.5941E+00, 4.5941E+00
    },
    {
        /*ka=10*/   3.8007E+00, 3.1613E+00, 3.0190E+00, 2.7534E+00, 2.3981E+00, 2.3981E+00, 1.9946E+00,
        1.4450E+00, 1.0961E+00, 8.7070E-01, 6.0237E-01, 4.6759E-01, 3.6297E-01, 2.8175E-01,
        2.1871E-01, 1.6977E-01, 1.3179E-01, 1.0230E-01, 7.9408E-02, 6.1641E-02, 4.7848E-02,
        6.1641E-02, 7.9408E-02, 1.0230E-01, 1.3179E-01, 1.6977E-01, 2.1871E-01, 2.8175E-01,
        3.6297E-01, 4.6759E-01, 6.0237E-01, 8.7070E-01, 1.0961E+00, 1.4450E+00, 1.9946E+00,
        2.3981E+00, 2.3981E+00, 2.7534E+00, 3.0190E+00, 3.1613E+00, 3.8007E+00
    },
    {
        /*ka=20*/   2.3330E+00, 2.3330E+00, 2.3330E+00, 2.3330E+00, 2.3330E+00, 2.3330E+00, 2.3330E+00,
        1.7698E+00, 1.2244E+00, 8.4708E-01, 5.8603E-01, 4.5491E-01, 3.5312E-01, 2.7411E-01,
        2.1278E-01, 1.6517E-01, 1.2821E-01, 9.9523E-02, 7.7254E-02, 5.9968E-02, 4.6550E-02,
        5.9968E-02, 7.7254E-02, 9.9523E-02, 1.2821E-01, 1.6517E-01, 2.1278E-01, 2.7411E-01,
        3.5312E-01, 4.5491E-01, 5.8603E-01, 8.4708E-01, 1.2244E+00, 1.7698E+00, 2.3330E+00,
        2.3330E+00, 2.3330E+00, 2.3330E+00, 2.3330E+00, 2.3330E+00, 2.3330E+00
    }
};

Spectrum BaffledFaceDirectivity::lwAdjustment(Vector direction, double distance)
{
    Spectrum directivity_spectrum;
    Spectrum spectre_Ka = atmosphere->get_k().mult(support_size); // 1/2 longueur de la diagonale de la bouche

    double theta = direction.angle(support_normal); // Angle du segment par rapport a la normale au support de la source

    // Angle compris entre 0 et 2pi;
    if (theta < 0) { theta = theta + M_2PI; }
    if (theta > M_2PI) { theta = theta - M_2PI; }

    int indice_theta = (int)(20.0 * theta / M_PI);

    indice_theta = indice_theta > (NB_THETA - 2) ? NB_THETA - 2 : indice_theta; // Eviter les depassement de tableau

    for (unsigned int i = 0 ; i < directivity_spectrum.getNbValues(); i++)
    {
        double ka = spectre_Ka.getTabValReel()[i];
        ka = ka > 20.0 ? 20.0 : ka ;

        int indice_Ka = find_Ka_idx(ka);

        directivity_spectrum.getTabValReel()[i] = compute_q(indice_Ka, indice_theta, ka, theta);
    }

    return directivity_spectrum;
}

double BaffledFaceDirectivity::compute_q(int indice_Ka, int indice_theta, double ka, double theta)
{
    double tabQ1_1 = _tabQ[indice_Ka]  [indice_theta];
    double tabQ1_2 = _tabQ[indice_Ka]  [indice_theta + 1];
    double tabQ2_1 = _tabQ[indice_Ka + 1][indice_theta];
    double tabQ2_2 = _tabQ[indice_Ka + 1][indice_theta + 1];

    double ka1 = _tabKa[indice_Ka];
    double ka2 = _tabKa[indice_Ka + 1];

    double theta1 =  indice_theta    * M_PI / 20;
    double theta2 = (indice_theta + 1) * M_PI / 20;

    double val1 = tabQ1_1 + (theta - theta1) * (tabQ1_2 - tabQ1_1) / (theta2 - theta1);
    double val2 = tabQ2_1 + (theta - theta1) * (tabQ2_2 - tabQ2_1) / (theta2 - theta1);

    return val1 + (ka - ka1) * (val2 - val1) / (ka2 - ka1);
}

int BaffledFaceDirectivity::find_Ka_idx(double ka)
{
    int indice = 0;

    while ((_tabKa[indice] < ka) && (indice < (NB_KA - 1))) { indice++; }

    return indice > (NB_KA - 2) ? NB_KA - 2 : indice; // Eviter les depassement de tableau
//    return indice > 0 ? indice - 1 : 0;
}
//
// --------------------------
//
AcousticSource::AcousticSource(
    const Point& position_,
    const Spectrum& spectrum_ ,
    SourceDirectivityInterface* directivity_)
    : position(position_)
    , spectrum(spectrum_)
    , directivity(directivity_)
{}

// ---------

AcousticReceptor::AcousticReceptor(
        const Point& position_
        ) : position(position_) {}


Point ComputeAcousticCentroid(source_pool_t &tabSources_)
{
    Point centroid;
    double cumul_x=0., cumul_y=0., cumul_z=0., cumul_level = 0.;
    std::deque<double> tabLevels;

    for(unsigned int i=0; i<tabSources_.size(); i++)
    {
        tabLevels.push_back( 10 * ::log10( tabSources_[i].spectrum.valGlobDBA() ) );
    }

    for(unsigned int i=0; i<tabSources_.size(); i++)
    {
        cumul_x = cumul_x + tabSources_[i].position._x * tabLevels[i];
        cumul_y = cumul_y + tabSources_[i].position._y * tabLevels[i];
        cumul_z = cumul_z + tabSources_[i].position._z * tabLevels[i];
        cumul_level = cumul_level * tabLevels[i];
    }

    centroid._x = cumul_x / cumul_level;
    centroid._y = cumul_y / cumul_level;
    centroid._z = cumul_z / cumul_level;

    return centroid;
}
} /* namespace tympan */
