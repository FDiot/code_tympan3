/*
 * Copyright (C) <2012> <EDF-R&D> <FRANCE>
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

/*
 *
 */



#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/Tools/OMessageManager.h"


OPROTOINST(TYSol);

TYSol::TYSol():
    _resistivite(20000),
    _epaisseur(1.0),
    _bMiroir(false),
    _vegetActive(false)
{
    _name = TYNameManager::get()->generateName(getClassName());

    _pVegetation = NULL;

#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "ResisSolDefault"))
    {
        _resistivite = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "ResisSolDefault");
    }
    else
    {
        TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "ResisSolDefault", _resistivite);
    }

    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "EpaisSolDefault"))
    {
        _epaisseur = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "EpaisSolDefault");
    }
    else
    {
        TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "EpaisSolDefault", _epaisseur);
    }
#endif

    calculZc();
}

TYSol::TYSol(const TYSol& other)
{
    *this = other;
}

TYSol::~TYSol()
{
    if (_pVegetation)
    {
        delete _pVegetation;
    }

    _pVegetation = NULL;
}

TYSol& TYSol::operator=(const TYSol& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        _resistivite = other._resistivite;
        _epaisseur = other._epaisseur;
        _vegetActive = other._vegetActive;
        _pImpedance = other._pImpedance;
        _pVegetation = other._pVegetation;
        _bMiroir = other._bMiroir;
    }
    return *this;
}

bool TYSol::operator==(const TYSol& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        if (_resistivite != other._resistivite) { return false; }
        if (_epaisseur != other._epaisseur) { return false; }
        if (_pImpedance != other._pImpedance) { return false; }
        if (_pVegetation != other._pVegetation) { return false; }
        if (_vegetActive != other._vegetActive) { return false; }
        if (_bMiroir != other._bMiroir) { return false; }
    }
    return true;
}

bool TYSol::operator!=(const TYSol& other) const
{
    return !operator==(other);
}

bool TYSol::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    TYSol* pOtherSol = (TYSol*) pOther;

    _resistivite = pOtherSol->_resistivite;
    _epaisseur = pOtherSol->_epaisseur;
    _vegetActive = pOtherSol->_vegetActive;
    _bMiroir = pOtherSol->_bMiroir;

    if (_pVegetation)
    {
        _pVegetation->deepCopy(pOtherSol->_pVegetation, copyId);
    }

    calculZc(); // Compute _pImpedance spectrum

    return true;
}

std::string TYSol::toString() const
{
    return "TYSol";
}

DOM_Element TYSol::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);

    TYXMLTools::addElementDoubleValue(domNewElem, "resistivite", _resistivite);
    TYXMLTools::addElementDoubleValue(domNewElem, "epaisseur", _epaisseur);
    TYXMLTools::addElementIntValue(domNewElem, "active", _vegetActive);
    TYXMLTools::addElementBoolValue(domNewElem, "miroir", _bMiroir);

    if (_vegetActive && _pVegetation) { _pVegetation->toXML(domNewElem); }

    return domNewElem;
}

int TYSol::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);

    bool resistiviteOk = false;
    bool epaisseurOk = false;
    bool activeOk = false;
    bool miroirOk = false;
    bool bVegetDone = false;

    if (_pVegetation) { delete _pVegetation; }
    _pVegetation = new TYVegetation();

    DOM_Element elemCur;
    QDomNodeList childs = domElement.childNodes();
    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        TYXMLTools::getElementDoubleValue(elemCur, "resistivite", _resistivite, resistiviteOk);
        TYXMLTools::getElementDoubleValue(elemCur, "epaisseur", _epaisseur, epaisseurOk);
        TYXMLTools::getElementBoolValue(elemCur, "active", _vegetActive, activeOk);
        TYXMLTools::getElementBoolValue(elemCur, "miroir", _bMiroir, miroirOk);

        if (_vegetActive && !bVegetDone)
        {
            bVegetDone = _pVegetation->callFromXMLIfEqual(elemCur);
        }
    }

    if (!bVegetDone && _pVegetation)
    {
        delete _pVegetation;
        _pVegetation = NULL;
    }

    calculZc(); // Recalcule l'impedance caracteristique du sol

    return 1;
}

TYVegetation* TYSol::useVegetation(bool state /*= true*/)
{
    _vegetActive = state;

    if (_vegetActive)
    {
        _pVegetation = new TYVegetation();
        _pVegetation->setParent(this);
    }
    else
    {
        if (_pVegetation) { delete _pVegetation; }
        _pVegetation = NULL;
    }

    return _pVegetation;
}

OSpectreComplex TYSol::abso(const double& angle, const double& rR, const TYAtmosphere& Atmo) const
{
    if (_bMiroir) { return OSpectreComplex(TYComplex(1.0, 0.0)); }

    OSpectreComplex abso ;

    // L'impedance caracteristique (Zc) et le nombre d'onde (K) ont ete calcules avant
    OSpectreComplex Zs = calculZs(angle, Atmo);

    OSpectreComplex rP = calculRp(Zs, angle);
    OSpectreComplex w = calculW(Zs, rR, angle, Atmo);
    OSpectreComplex fW = calculFw(w);

    abso = calculQ(angle, rP, fW);
    abso = abso.toModulePhase(); // Q * e^(i.k.rr)

    abso.setType(SPECTRE_TYPE_ABSO);
    return abso;
}

void TYSol::calculZc()
{
    _pImpedance.setType(SPECTRE_TYPE_AUTRE);

    double sigmaSurF = 0.0;
    double reel, imaginaire;
    TYTabFreq tabFreq = TYSpectre::getTabFreqExact();  // On travaille en frequence reelle


    for (unsigned int i = 0 ; i < _pImpedance.getNbValues() ; i++)
    {
        sigmaSurF = _resistivite / tabFreq[i];

        reel       = 1.0 +  9.08 * pow(sigmaSurF, 0.75);
        imaginaire =       11.9  * pow(sigmaSurF, 0.73);

        _pImpedance.getTabValReel()[i] = reel;
        _pImpedance.getTabValImag()[i] = imaginaire;
    }
}

OSpectreComplex TYSol::calculZs(const double phi, const TYAtmosphere& atmos) const
{
    OSpectreComplex Zs;
    const TYComplex j = TYComplex(0.0, 1.0);
    const TYComplex CPLX_UN  = TYComplex(1.0, 0.0);
    const TYComplex CPLX_MUN = TYComplex(-1.0, 0.0);
    double k;

    TYComplex operand, K, Zc;
    double cosPhi = cos(phi);

    TYComplex cplxVal ;

    for (unsigned int i = 0 ; i < _pImpedance.getNbValues(); i++)
    {
        k = atmos.getKAcoust().getTabValReel()[i];
        K = TYComplex(_pNombreDOnde.getTabValReel()[i], _pNombreDOnde.getTabValImag()[i]);
        Zc = TYComplex(_pImpedance.getTabValReel()[i], _pImpedance.getTabValImag()[i]);

        operand = std::sqrt(CPLX_UN - ((k * cosPhi / K) * (k * cosPhi / K)));

        cplxVal = Zc * cotanh(CPLX_MUN * j * K * _epaisseur * operand) / operand;
        Zs.getTabValReel()[i] = cplxVal.real();
        Zs.getTabValImag()[i] = cplxVal.imag();
    }

    return Zs;
}

void TYSol::calculNombreDOnde(const TYAtmosphere& atmos)
{
    /** =================================================================
        TRACE DES MODIFS

        13/11/04    Sur partie reelle changement de signe de l'exposant
                    reel       =  k * (1 + 10.8 * pow(sigmaSurF, -0.7 ));
                    au lieu de
                    reel       =  k * (1 + 10.8 * pow(sigmaSurF,  0.7 ));
                    precedemment
        13/11/04    Remplacement de sigmaSurF par FSurSigma
                    FSurSigma = tabFreq[i] / _resistivite
                    (precedente formulation SigmaSurF mise en commentaire)
        13/11/04    Sur partie imaginaire changement du signe de l'exposant
                    imaginaire =  k * (    10.3 * pow(FSurSigma, 0.59));
                    au lieu de
                    imaginaire =  k * (    10.3 * pow(FSurSigma, -0.59));

     =====================================================================
        */
    _pNombreDOnde.setType(SPECTRE_TYPE_AUTRE);

    double FSurSigma = 0.0;

    double reel, imaginaire, k;
    TYTabFreq tabFreq = TYSpectre::getTabFreqExact();  // On travaille en frequence reelle

    for (unsigned int i = 0 ; i < _pNombreDOnde.getNbValues() ; i++)
    {
        FSurSigma = tabFreq[i] / _resistivite;

        k = atmos.getKAcoust().getTabValReel()[i];

        reel       =  k * (1 + 10.8 * pow(FSurSigma, -0.7));
        imaginaire =  k * (10.3 * pow(FSurSigma,  0.59));

        _pNombreDOnde.getTabValReel()[i] = reel;
        _pNombreDOnde.getTabValImag()[i] = imaginaire;
    }
}

OSpectreComplex TYSol::calculW(const OSpectreComplex& Zs, const double rR, const double phi, const TYAtmosphere& atmos) const
{
    OSpectreComplex w;

    TYComplex zs, invZs, cplxTempo, wTemp;

    const TYComplex j = TYComplex(0.0, 1.0);
    double k;                       // nombre d'onde acoustique
    double sinPhi = sin(phi);

    for (unsigned int i = 0 ; i < w.getNbValues() ; i++)
    {
        k = atmos.getKAcoust().getTabValReel()[i];
        zs = TYComplex(Zs.getTabValReel()[i], Zs.getTabValImag()[i]);

        invZs = 1.0 / zs;
        cplxTempo = (sinPhi + invZs) * (sinPhi + invZs);

        wTemp = std::sqrt(1.0 / 2.0 * j * k * rR * cplxTempo) ;

        w.getTabValReel()[i] = wTemp.real();
        w.getTabValImag()[i] = wTemp.imag();
    }

    return w;
}

inline TYComplex TYSol::erfcCas1(const TYComplex& wValue) const
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

inline TYComplex TYSol::erfcCas2(const TYComplex& wValue) const
{
    TYComplex G ;
    const TYComplex j = TYComplex(0.0, 1.0);
    TYComplex w2 = wValue * wValue;

    G = j * wValue * ((0.4613135   / (w2 - 0.1901635))   +
                      (0.09999216  / (w2 - 1.7844927))   +
                      (0.002883894 / (w2 - 5.5253437)))  ;

    return G;
}

inline TYComplex TYSol::erfcCas3(const TYComplex& wValue) const
{
    TYComplex G ;
    const TYComplex j = TYComplex(0.0, 1.0);
    TYComplex w2 = wValue * wValue;

    G = j * wValue * ((0.5124242  / (w2 - 0.2752551))  +
                      (0.05176536 / (w2 - 2.724745))) ;

    return G;
}

using namespace :: std;

inline TYComplex TYSol::sgnReIm(const TYComplex& W, const TYComplex& G) const
{
    double x = fabs(W.real());
    double y = fabs(W.imag());
    const TYComplex j = TYComplex(0.0, 1.0);

    double cos2xy = cos(2 * x * y);
    double sin2xy = sin(2 * x * y);

    return 2.0 * exp(y * y - x * x) * (cos2xy + j * sin2xy) - conj(G);
}

OSpectreComplex TYSol::calculFw(const OSpectreComplex& w) const
{
    OSpectreComplex fw ;
    OSpectreComplex localW = w;
    OSpectreComplex G;

    const double epsilon = 1.0E-20;
    const double pi = M_PI;
    const TYComplex j = TYComplex(0.0, 1.0);

    unsigned int i;
    double valeur = 0.0;
    double tampon = 0.0;

    // Recherche et remplacement des parties reelles et imaginaires < epsilon par epsilon
    for (i = 0; i < localW.getNbValues() ; i++)
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

    // Filtrage des differents cas
    double reelW, imagW, absReelW, absImagW;

    for (i = 0 ; i < localW.getNbValues() ; i++)
    {
        reelW = localW.getTabValReel()[i];
        imagW = localW.getTabValImag()[i];

        absReelW = fabs(reelW);
        absImagW = fabs(imagW);

        if (((absReelW < 3.9) && (absImagW < 3.0)))                                 // Cas 1
        {
            TYComplex val = erfcCas1(TYComplex(reelW, imagW));
            G.getTabValReel()[i] = val.real();
            G.getTabValImag()[i] = val.imag();
        }
        else if (((absReelW >= 3.9) && (absReelW < 6.0)) ||                         // Cas 2
                 ((absImagW >= 3.0) && (absImagW < 6.0)))
        {
            TYComplex val = erfcCas2(TYComplex(absReelW, absImagW));
            G.getTabValReel()[i] = val.real();
            G.getTabValImag()[i] = val.imag();
        }
        else                                                                        // Cas 3
        {
            TYComplex val = erfcCas3(TYComplex(absReelW, absImagW));
            G.getTabValReel()[i] = val.real();
            G.getTabValImag()[i] = val.imag();
        }

    }

    // Operations en fonction du signe des parties reelles et imaginaires de w
    double re, im;
    for (i = 0 ; i < localW.getNbValues() ; i++)
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
    }

    // Calcul de la fonction Fw

    double pi2 = sqrt(pi);

    for (i = 0; i < localW.getNbValues(); i++)
    {
        TYComplex v1 = TYComplex(localW.getTabValReel()[i], localW.getTabValImag()[i]);
        TYComplex v2 = TYComplex(G.getTabValReel()[i], G.getTabValImag()[i]);
        TYComplex val = TYComplex(1.0, 0.0) + j * pi2 * v1 * v2;
        fw.getTabValReel()[i] = val.real();
        fw.getTabValImag()[i] = val.imag();
    }

    return fw;
}


OSpectreComplex TYSol::calculRp(const OSpectreComplex& Zs, const double& phi) const
{
    OSpectreComplex rP;

    const TYComplex CPLX_UN = TYComplex(1.0, 0.0);
    double sinPhi = sin(phi);
    TYComplex Z;

    for (unsigned int i = 0 ; i < _pImpedance.getNbValues() ; i++)
    {
        Z = TYComplex(Zs.getTabValReel()[i], Zs.getTabValImag()[i]) ;

        TYComplex val = (Z * sinPhi - CPLX_UN) / (Z * sinPhi + CPLX_UN);

        rP.getTabValReel()[i] = val.real();
        rP.getTabValImag()[i] = val.imag();
    }

    return rP;
}


OSpectreComplex TYSol::calculQ(const double& phi, const OSpectreComplex& rp, const OSpectreComplex& fw) const
{
    OSpectreComplex Q ;

    const TYComplex CPLX_UN = TYComplex(1.0, 0.0);
    TYComplex RP, FW ;

    for (unsigned int i = 0 ; i < _pImpedance.getNbValues(); i++)
    {
        RP = TYComplex(rp.getTabValReel()[i], rp.getTabValImag()[i]);
        FW = TYComplex(fw.getTabValReel()[i], fw.getTabValImag()[i]);

        TYComplex val = RP + (CPLX_UN - RP) * FW;

        Q.getTabValReel()[i] = val.real();
        Q.getTabValImag()[i] = val.imag();
    }

    return Q ;
}

OSpectreComplex TYSol::calculQ(const double& phi, const OSpectreComplex& fw) const
{
    OSpectreComplex Q ;
    TYAtmosphere atmos; // En attendant ...
    const OSpectreComplex Zs = calculZs(phi, atmos);
    const OSpectreComplex rp = calculRp(Zs, phi);

    const TYComplex CPLX_UN = TYComplex(1.0, 0.0);
    TYComplex RP, FW ;

    for (unsigned int i = 0 ; i < _pImpedance.getNbValues(); i++)
    {
        RP = TYComplex(rp.getTabValReel()[i], rp.getTabValImag()[i]);
        FW = TYComplex(fw.getTabValReel()[i], fw.getTabValImag()[i]);

        TYComplex val = RP + (CPLX_UN - RP) * FW;

        Q.getTabValReel()[i] = val.real();
        Q.getTabValImag()[i] = val.imag();
    }

    return Q ;
}

OSpectreComplex TYSol::calculQ(const double& angle, const double& rR, const TYAtmosphere& Atmo) const
{
    const OSpectreComplex Zs = calculZs(angle, Atmo);
    OSpectreComplex rP = calculRp(Zs, angle);
    OSpectreComplex w  = calculW(Zs, rR, angle, Atmo);
    OSpectreComplex fW = calculFw(w);

    return calculQ(angle, rP, fW).toModulePhase();
}
