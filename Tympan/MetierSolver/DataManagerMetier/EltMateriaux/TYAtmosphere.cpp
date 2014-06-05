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

#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerIHM/TYAtmosphereWidget.h"
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/Tools/OMessageManager.h"


TY_EXTENSION_INST(TYAtmosphere);

const double TYAtmosphere::_impedanceRef(400.0);


TYAtmosphere::TYAtmosphere()
{
    _name = TYNameManager::get()->generateName(getClassName());

    _pression = TY_EDF_PR;
    _temperature = 20.0;
    _hygrometrie = 70.0;
    _vitVent = 0.0;
    _dirVent = 0.0;
    _h_molaire = 1.0;
    _vitSon = 340.0;
    _impedanceSpecifique = 400.0;

    _A = 0.0; // linear sound speed gradient
    _B = 0.0; // logarithmic sound speed gradient
    _C = 5.0E-6; // turbulence strength
    _D = 0.0; // displacement height

    /// Classe meteo
    _stabilityClass = 0;

#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "PressionAtmoDefault"))
    {
        _pression = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "PressionAtmoDefault");
    }
    else
    {
        TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "PressionAtmoDefault", _pression);
    }

    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "TempAtmoDefault"))
    {
        _temperature = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "TempAtmoDefault");
    }
    else
    {
        TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "TempAtmoDefault", _temperature);
    }

    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "HygroAtmoDefault"))
    {
        _hygrometrie = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "HygroAtmoDefault");
    }
    else
    {
        TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "HygroAtmoDefault", _hygrometrie);
    }

    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "VitVentAtmoDefault"))
    {
        _vitVent = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "VitVentAtmoDefault");
    }
    else
    {
        TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "VitVentAtmoDefault", _vitVent);
    }

    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "DirVentAtmoDefault"))
    {
        _dirVent = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "DirVentAtmoDefault");
    }
    else
    {
        TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "DirVentAtmoDefault", _dirVent);
    }

    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "LinearSpeedGrad"))
    {
        _A = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "LinearSpeedGrad");
    }
    else
    {
        TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "LinearSpeedGrad", _A);
    }

    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "LogSpeedGrad"))
    {
        _B = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "LogSpeedGrad");
    }
    else
    {
        TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "LogSpeedGrad", _B);
    }

    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "TurbulenceStrength"))
    {
        _C = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "TurbulenceStrength");
    }
    else
    {
        TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "TurbulenceStrength", _C);
    }

    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "DisplacementHeight"))
    {
        _D = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "DisplacementHeight");
    }
    else
    {
        TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "DisplacementHeight", _D);
    }

    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "MeteoClass"))
    {
        _stabilityClass = TYPreferenceManager::getInt(TYDIRPREFERENCEMANAGER, "MeteoClass");
    }
    else
    {
        TYPreferenceManager::setInt(TYDIRPREFERENCEMANAGER, "MeteoClass", _stabilityClass);
    }

#endif

    _spectreAtt.setParent(this);
    _spectreAtt.setType(SPECTRE_TYPE_ATT);
    _spectreAtt.setIsReadOnly(true);

    _spectreKAcoust = OSpectre::getEmptyLinSpectre();
    _spectreKAcoust.setType(SPECTRE_TYPE_AUTRE);

    calculProprietes(); // Calcul des proprietes de l'atmosphere
}

TYAtmosphere::TYAtmosphere(const TYAtmosphere& other)
{
    *this = other;
}

TYAtmosphere::~TYAtmosphere()
{
}

TYAtmosphere& TYAtmosphere::operator=(const TYAtmosphere& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        _pression = other._pression;
        _temperature = other._temperature;
        _hygrometrie = other._hygrometrie;
        _h_molaire = other._h_molaire;
        _vitVent = other._vitVent;
        _dirVent = other._dirVent;
        _vitSon = other._vitSon;
        _impedanceSpecifique = other._impedanceSpecifique;
        _spectreAtt = other._spectreAtt;
        _spectreKAcoust = other._spectreKAcoust;
        _A = other._A;
        _B = other._B;
        _C = other._C;
        _D = other._D;
        _stabilityClass = other._stabilityClass;
    }

    return *this;
}

bool TYAtmosphere::operator==(const TYAtmosphere& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        if (_pression != other._pression) { return false; }
        if (_temperature != other._temperature) { return false; }
        if (_hygrometrie != other._hygrometrie) { return false; }
        if (_vitVent != other._vitVent) { return false; }
        if (_dirVent != other._dirVent) { return false; }
        if (_spectreAtt != other._spectreAtt) { return false; }
        if (_A != other._A) { return false; }
        if (_B != other._B) { return false; }
        if (_C != other._C) { return false; }
        if (_D != other._D) { return false; }
        if (_stabilityClass != other._stabilityClass) { return false; }
    }
    return true;
}

bool TYAtmosphere::operator!=(const TYAtmosphere& other) const
{
    return !operator==(other);
}

bool TYAtmosphere::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    TYAtmosphere* pOtherAtm = (TYAtmosphere*) pOther;

    _pression = pOtherAtm->_pression;
    _temperature = pOtherAtm->_temperature;
    _hygrometrie = pOtherAtm->_hygrometrie;
    _h_molaire = pOtherAtm->_h_molaire;
    _vitVent = pOtherAtm->_vitVent;
    _dirVent = pOtherAtm->_dirVent;
    _vitSon = pOtherAtm->_vitSon;
    _impedanceSpecifique = pOtherAtm->_impedanceSpecifique;
    _A = pOtherAtm->_A;
    _B = pOtherAtm->_B;
    _C = pOtherAtm->_C;
    _D = pOtherAtm->_D;
    _stabilityClass = pOtherAtm->_stabilityClass;

    _spectreAtt.deepCopy(new TYSpectre(pOtherAtm->_spectreAtt), copyId);

    calculProprietes();

    return true;
}

std::string TYAtmosphere::toString() const
{
    return "TYAtmosphere";
}

DOM_Element TYAtmosphere::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);

    TYXMLTools::addElementDoubleValue(domNewElem, "pression", _pression);
    TYXMLTools::addElementDoubleValue(domNewElem, "temperature", _temperature);
    TYXMLTools::addElementDoubleValue(domNewElem, "hygrometrie", _hygrometrie);
    TYXMLTools::addElementDoubleValue(domNewElem, "vitVent", _vitVent);
    TYXMLTools::addElementDoubleValue(domNewElem, "dirVent", _dirVent);

    TYXMLTools::addElementDoubleValue(domNewElem, "A", _A);
    TYXMLTools::addElementDoubleValue(domNewElem, "B", _B);
    TYXMLTools::addElementDoubleValue(domNewElem, "C", _C);
    TYXMLTools::addElementDoubleValue(domNewElem, "D", _D);

    TYXMLTools::addElementIntValue(domNewElem, "stability_class", _stabilityClass);

    TYSpectre::setXMLPrecision(5);
    _spectreAtt.toXML(domNewElem);
    TYSpectre::setXMLPrecision();

    return domNewElem;
}

int TYAtmosphere::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);
    unsigned int i;

    bool getOk[10];
    for (i = 0; i < 10; i++) { getOk[i] = false; }
    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();
    for (i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();
        TYXMLTools::getElementDoubleValue(elemCur, "pression", _pression, getOk[0]);
        TYXMLTools::getElementDoubleValue(elemCur, "temperature", _temperature, getOk[1]);
        TYXMLTools::getElementDoubleValue(elemCur, "hygrometrie", _hygrometrie, getOk[2]);
        TYXMLTools::getElementDoubleValue(elemCur, "vitVent", _vitVent, getOk[3]);
        TYXMLTools::getElementDoubleValue(elemCur, "dirVent", _dirVent, getOk[4]);

        TYXMLTools::getElementDoubleValue(elemCur, "A", _A, getOk[5]);
        TYXMLTools::getElementDoubleValue(elemCur, "B", _B, getOk[6]);
        TYXMLTools::getElementDoubleValue(elemCur, "C", _C, getOk[7]);
        TYXMLTools::getElementDoubleValue(elemCur, "D", _D, getOk[8]);

        TYXMLTools::getElementIntValue(elemCur, "stability_class", _stabilityClass, getOk[9]);

        _spectreAtt.callFromXMLIfEqual(elemCur);
    }

    calculProprietes();
    return 1;
}

OSpectre TYAtmosphere::getAtt(const TYSegment& seg) const
{
    double longueur = seg.longueur();

    return getAtt(longueur);
}

OSpectre TYAtmosphere::getAtt(const double& distance) const
{
    return _spectreAtt.mult(-(distance / 2.0)).toGPhy(); // == MODIFIE mult(distance)
}

void TYAtmosphere::setPression(double press)
{
    _pression = press;

    // Recalcul de toutes les proprietes associees a ce parametre
    calculProprietes();
}

void TYAtmosphere::setTemperature(double temp)
{
    _temperature = temp;

    // Recalcul de toutes les proprietes associees a ce parametre
    calculProprietes();
}

void TYAtmosphere::setHygrometrie(double hygro)
{
    _hygrometrie = hygro;

    // Recalcul de toutes les proprietes associees a ce parametre
    calculProprietes();
}

void TYAtmosphere::hrToFMol()
{
    double T01 = TY_EDF_0K + 0.01   ;       // Temperature isotherme au point triple
    double tK = _temperature + TY_EDF_0K;   // Temperature en Kelvin
    double pr_rel = _pression / TY_EDF_PR;  // Pression relative
    double prSat_prRef              ;       // Rapport de la pression saturante a la pression atmospherique
    double C                        ;       // Sert a calculer prSat_^prRef

    C = -6.8346 * pow((T01 / tK), 1.261) + 4.6151 ;

    prSat_prRef = pow(10, C)    ;

    _h_molaire = _hygrometrie * prSat_prRef * pr_rel    ;
}

void TYAtmosphere::calculSpectre()
{
    double tampon;
    double f2;                                      // Carre de la frequence de travail
    double frO, frN;                                // Frequences de relaxation de l'oxygene et de l'azote
    double tA_classic, tA_N, tA_O;                  // Termes d'absoption classique, Azote Oxygene
    double tK = _temperature + TY_EDF_0K;           // temperature absolue exprimee en Kelvin
    double pr_rel = _pression / TY_EDF_PR;          // Pression relative
    double t_rel  = tK / TY_EDF_T0;                 // Temperature relative
    TYTabFreq tabFreq = TYSpectre::getTabFreqExact();

    // Ce test est necessaire par precaution
    if (tabFreq.size() < _spectreAtt.getNbValues())
    {
        return;
    }

    frO = pr_rel * (24.0 + (4.04 * 10000.0 * _h_molaire * ((0.02 + _h_molaire) / (0.391 + _h_molaire))));
    frN = pr_rel * pow(t_rel, -0.5) * (9.0 + 280.0 * _h_molaire * exp((-4.170 * (pow(t_rel, (-1.0 / 3.0)) - 1))));

    for (unsigned int i = 0; i < _spectreAtt.getNbValues() ; i++)
    {
        f2 = tabFreq[i] * tabFreq[i];

        tA_classic = 1.84e-11 * (1.0 / pr_rel) * sqrt(t_rel);
        tA_O = 0.01275 * exp(-2239.1 / tK) * (1.0 / (frO + (f2 / frO)));
        tA_N = 0.1068  * exp(-3352.0 / tK) * (1.0 / (frN + (f2 / frN)));

        tampon = (8.686 * f2 * (tA_classic + (pow(t_rel, -2.5) * (tA_O + tA_N))));
        _spectreAtt.getTabValReel()[i] = tampon;
    }

    tabFreq.clear();
}

void TYAtmosphere::calculC()
{
    const double gamma = 1.41;
    const double R = 8.31441;
    const double M = 0.029;
    const double tK = _temperature + TY_EDF_0K;

    _vitSon = sqrt((gamma * R * tK) / M);
}


void TYAtmosphere::calculK()
{
    const double pi = M_PI;
    double k;
    TYTabFreq tabFreq = TYSpectre::getTabFreqExact();

    for (unsigned int i = 0 ; i < tabFreq.size() ; i++)
    {
        k = (2 * pi * tabFreq[i]) / _vitSon ;

        _spectreKAcoust.getTabValReel()[i] = k;
    }
}

void TYAtmosphere::calculImpedance()
{
    const double R = 8.31441;                       // Constante des gazs parfaits
    const double tK = _temperature + TY_EDF_0K;
    const double M = 0.029;                         // Masse molaire de l'air

    _impedanceSpecifique = (M * (_pression / (R * tK))) * _vitSon;
}

void TYAtmosphere::calculProprietes()
{
    hrToFMol();
    calculSpectre();
    calculC();
    calculImpedance();
    calculK();
}




















