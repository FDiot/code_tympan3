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

// Frequence de travail minimale
float OSpectre::_fMin = 16;

// Frequence de travail maximale
float OSpectre::_fMax = 16000;

double OSpectre::_defaultValue = TY_SPECTRE_DEFAULT_VALUE;

// Tableau des frequences en Hz centrales normalisees en tiers d'octave.
const float OSpectre::_freqNorm[] = {   /*  0 */    16.0,    20.0,    25.0,
                                                    /*  3 */    31.5,    40.0,    50.0,
                                                    /*  6 */    63.0,    80.0,   100.0,
                                                    /*  9 */   125.0,   160.0,   200.0,
                                                    /* 12 */   250.0,   315.0,   400.0,
                                                    /* 15 */   500.0,   630.0,   800.0,
                                                    /* 18 */  1000.0,  1250.0,  1600.0,
                                                    /* 21 */  2000.0,  2500.0,  3150.0,
                                                    /* 24 */  4000.0,  5000.0,  6300.0,
                                                    /* 27 */  8000.0, 10000.0, 12500.0,
                                                    /* 30 */ 16000.0
                                    };


std::map<float, int> OSpectre::_mapFreqIndice = setMapFreqIndice();


OSpectre::OSpectre() : _valid(true), _type(SPECTRE_TYPE_ATT), _etat(SPECTRE_ETAT_DB), _form(SPECTRE_FORM_TIERS)
{
    for (unsigned int i = 0 ; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        _module[i] = _defaultValue;
    }
}

OSpectre::OSpectre(double defaultValue) : _valid(true), _type(SPECTRE_TYPE_ATT), _etat(SPECTRE_ETAT_DB), _form(SPECTRE_FORM_TIERS)
{
    for (unsigned int i = 0 ; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        _module[i] = defaultValue;
    }
}

OSpectre::OSpectre(const double* valeurs, const short& nbVal, const short& decalage)
{
    unsigned int i;

    // D'abord on initialise les valeurs
    for (i = 0 ; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        _module[i] = 0.0;
    }

    unsigned int maxInd = (unsigned int)(nbVal + decalage) < TY_SPECTRE_DEFAULT_NB_ELMT ? nbVal : TY_SPECTRE_DEFAULT_NB_ELMT;
    for (i = decalage ; i < maxInd; i++)
    {
        _module[i] = valeurs[i - decalage];
    }

    _valid = true;
    _etat  = SPECTRE_ETAT_DB;
    _type  = SPECTRE_TYPE_ATT;
    _form  = SPECTRE_FORM_TIERS;
}


OSpectre::OSpectre(const OSpectre& other)
{
    *this = other;
}

OSpectre::~OSpectre()
{
}

OSpectre& OSpectre::operator=(const OSpectre& other)
{
    if (this != &other)
    {
        _valid = other._valid;
        _type = other._type;
        _etat = other._etat;
        _form = other._form;

        for (unsigned int i = 0 ; i < TY_SPECTRE_DEFAULT_NB_ELMT ; i++)
        {
            _module[i] = other._module[i];
        }
    }

    return *this;
}

bool OSpectre::operator==(const OSpectre& other) const
{
    if (this != &other)
    {
        if (_valid != other._valid) { return false; }
        if (_type != other._type) { return false; }
        if (_etat != other._etat) { return false; }
        if (_form != other._form) { return false; }

        for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
        {
            if (_module[i] != other._module[i]) { return false; }
        }
    }

    return true;
}

bool OSpectre::operator!=(const OSpectre& other) const
{
    return !operator==(other);
}

OSpectre OSpectre::operator * (const OSpectre& spectre) const
{
    OSpectre s;
    s._etat = _etat; s._type = _type; // Recopie de l'empreinte du spectre

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        s._module[i] = this->_module[i] * spectre._module[i];
    }

    return s;
}

OSpectre OSpectre::operator * (const double& coefficient) const
{
    OSpectre s;
    s._etat = _etat; s._type = _type; // Recopie de l'empreinte du spectre

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        s._module[i] = this->_module[i] * coefficient;
    }

    return s;
}

OSpectre OSpectre::operator + (const double& valeur) const
{
    OSpectre s;
    s._etat = _etat; s._type = _type; // Recopie de l'empreinte du spectre

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        s._module[i] = this->_module[i] + valeur;
    }

    return s;
}

OSpectre OSpectre::operator + (const OSpectre& spectre) const
{
    OSpectre s;

    s._etat = _etat; s._type = _type; // Recopie de l'empreinte du spectre

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        s._module[i] = this->_module[i] + spectre._module[i];
    }

    return s;
}

OSpectre OSpectre::operator - (const OSpectre& spectre) const
{
    OSpectre s;

    s._etat = _etat; s._type = _type; // Recopie de l'empreinte du spectre

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        s._module[i] = this->_module[i] - spectre._module[i];
    }

    return s;
}

void OSpectre::setDefaultValue(const double& valeur)
{
    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        _module[i] = valeur;
    }
}

void OSpectre::setValue(const float& freq, const double& reel/*=0.0*/)
{
    int indice = _mapFreqIndice[freq];

    _module[indice] = reel;
}

double OSpectre::getValueReal(float freq, bool* pValid)
{
    int indice = _mapFreqIndice[freq];

    return _module[indice];
}

void OSpectre::getRangeValueReal(double* valeurs, const short& nbVal, const short& decalage)
{
    for (short i = 0; i < nbVal; i++)
    {
        valeurs[i] = _module[i + decalage];
    }
}

double OSpectre::valGlobDBLin() const
{
    OSpectre s(this->toDB());

    double valeurGlob = 0.0;
    for (unsigned int i = 0 ; i < TY_SPECTRE_DEFAULT_NB_ELMT ; i++)
    {
        valeurGlob = valeurGlob + pow(10, (s._module[i] / 10));
    }

    valeurGlob = 10 * log10(valeurGlob);

    return valeurGlob;
}

double OSpectre::valGlobDBA() const
{
    OSpectre s(this->toDB().sum(OSpectre::pondA()));

    return s.valGlobDBLin();
}

OSpectre OSpectre::toDB() const
{
    OSpectre s;

    // Si spectre deja en dB on se contente de recopier les valeurs
    if (_etat == SPECTRE_ETAT_DB)
    {
        s = *this;
        return s;
    }

    unsigned int i;
    s._type = _type; // Recopie du type

    double coef = 1.0;
    switch (_type)
    {
        case SPECTRE_TYPE_LP:
            coef = 4.0e-10;
            break;
        case SPECTRE_TYPE_LW :
            coef = 1.0e-12;
            break   ;
        case SPECTRE_TYPE_ATT:
        default :
            coef = 1.0;
            break;
    }

    double dBVal = 0.0;
    for (i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        dBVal = 10.0 * log10(this->_module[i] / coef);
        s._module[i] = dBVal    ;
    }

    s._etat = SPECTRE_ETAT_DB;   // Etat explicite dB

    return s;
}

OSpectre OSpectre::toGPhy() const
{
    OSpectre s;

    if (_etat == SPECTRE_ETAT_LIN) // Si spectre deja en LIN on se contente de recopier les valeurs
    {
        s = *this;

        return s;
    }

    unsigned int i;
    double coef = 1.0;

    s._type = _type; // Recopie du type

    switch (_type)
    {
        case SPECTRE_TYPE_LP:
            coef = 4.0e-10;
            break;
        case SPECTRE_TYPE_LW :
            coef = 1.0e-12;
            break   ;
        case SPECTRE_TYPE_ATT:
        default :
            coef = 1.0;
            break;
    }

    double powVal = 0.0;
    for (i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        powVal = pow(10.0, this->_module[i] / 10.0) * coef;
        s._module[i] = powVal;
    }

    s._etat = SPECTRE_ETAT_LIN;   // Etat explicite Grandeur physique

    return s;
}

OSpectre OSpectre::sum(const OSpectre& spectre) const
{
    OSpectre s;

    s._etat = _etat; s._type = _type; // Recopie de l'empreinte du spectre

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        s._module[i] = this->_module[i] + spectre._module[i];
    }

    return s;
}

OSpectre OSpectre::sum(const double& valeur) const
{
    OSpectre s;

    // Recopie de l'empreinte du spectre
    s._etat = _etat; s._type = _type;

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        s._module[i] = this->_module[i] + valeur;
    }

    return s;
}

OSpectre OSpectre::sumdB(const OSpectre& spectre) const
{
    OSpectre tempoS1(this->toGPhy());
    OSpectre tempoS2(spectre.toGPhy());

    OSpectre s(tempoS1.sum(tempoS2));

    // Recopie de l'empreinte du spectre
    s._type = _type;

    // Le spectre est reconverti en dB
    return s.toDB();
}

OSpectre OSpectre::subst(const OSpectre& spectre) const
{
    OSpectre s;

    // Recopie de l'empreinte du spectre
    s._etat = _etat; s._type = _type;

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        s._module[i] = this->_module[i] - spectre._module[i];
    }

    return s;
}

OSpectre OSpectre::subst(const double& valeur) const
{
    OSpectre s;

    // Recopie de l'empreinte du spectre
    s._etat = _etat; s._type = _type;

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        s._module[i] = this->_module[i] - valeur;
    }

    return s;
}

OSpectre OSpectre::substdB(const OSpectre& spectre) const
{
    OSpectre s;

    OSpectre tempoS1(this->toGPhy());
    OSpectre tempoS2(spectre.toGPhy());

    // Calcul de la somme energetique
    double tempVal;

    for (unsigned int i = 0; i < s.getNbValues(); i++)
    {
        tempVal = tempoS1._module[i] - tempoS2._module[i];

        if (tempVal <= 0)
        {
            s._module[i] = 1e-20;
            s._valid = false ;
            continue;
        }

        s._module[i] = tempVal;
    }

    s._type = _type;
    s._etat = SPECTRE_ETAT_LIN;

    //  s = s.toDB();

    return s.toDB();
}

OSpectre OSpectre::mult(const OSpectre& spectre) const
{
    OSpectre s;

    // Recopie de l'empreinte du spectre
    s._etat = _etat; s._type = _type;

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        s._module[i] = this->_module[i] * spectre._module[i];
    }

    return s;
}

OSpectre OSpectre::mult(const double& coefficient) const
{
    OSpectre s;

    // Recopie de l'empreinte du spectre
    s._etat = _etat; s._type = _type;

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        s._module[i] = this->_module[i] * coefficient;
    }

    return s;
}

OSpectre OSpectre::div(const OSpectre& spectre) const
{
    OSpectre s;

    // Recopie de l'empreinte du spectre
    s._etat = _etat; s._type = _type;

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        if (spectre._module[i] == 0.0)
        {
            s._module[i] = 1E20;
            s._valid = false ;
            break;
        }

        s._module[i] = _module[i] / spectre._module[i];
    }

    return s;
}

OSpectre OSpectre::div(const double& coefficient) const
{
    OSpectre s;

    // Recopie de l'empreinte du spectre
    s._etat = _etat;
    s._type = _type;

    if (coefficient == 0.0)  // Division par zero
    {
        s._valid = false;
    }
    else
    {
        for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
        {
            s._module[i] = this->_module[i] / coefficient;
        }
    }

    return s;
}

OSpectre OSpectre::invMult(const double& coefficient) const
{
    OSpectre s;

    // Recopie de l'empreinte du spectre
    s._etat = _etat;
    s._type = _type;

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        if (_module[i] == 0.0)
        {
            s._module[i] = 1E20;
            s._valid = false ;
            break;
        }

        s._module[i] = coefficient / _module[i];

    }

    return s;
}

OSpectre OSpectre::inv() const
{
    OSpectre s;

    // Recopie de l'empreinte du spectre
    s._etat = _etat; s._type = _type;

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        if (_module[i] == 0.0)
        {
            s._module[i] = 1E20;
            s._valid = false ;
            break;
        }

        s._module[i] = 1.0 / _module[i];
    }

    return s;
}

OSpectre OSpectre::power(const double& puissance) const
{
    OSpectre s;

    // Recopie de l'empreinte du spectre
    s._etat = _etat; s._type = _type;

    for (unsigned int i = 0 ; i < TY_SPECTRE_DEFAULT_NB_ELMT ; i++)
    {
        s._module[i] = pow(this->_module[i], puissance);
    }

    return s;
}

OSpectre OSpectre::log(const double& base) const
{
    OSpectre s;

    // Recopie de l'empreinte du spectre
    s._etat = _etat; s._type = _type;

    double logBase = ::log(base);

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        if (_module[i] <= 0.0)
        {
            s._module[i] = 1E20;
            s._valid = false ;
            break;
        }

        s._module[i] = ::log(_module[i]) / logBase;
    }

    return s;
}

OSpectre OSpectre::tenPow() const
{
    OSpectre  s;

    // Recopie de l'empreinte du spectre
    s._etat = _etat; s._type = _type;

    for (unsigned int i = 0 ; i < TY_SPECTRE_DEFAULT_NB_ELMT ; i++)
    {
        s._module[i] = pow(10.0, this->_module[i]);
    }

    return s;
}

OSpectre OSpectre::racine() const
{
    OSpectre s;

    // Recopie de l'empreinte du spectre
    s._etat = _etat; s._type = _type;

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        if (_module[i] < 0.0)
        {
            s._module[i] = 1E20;
            s._valid = false ;
            break;
        }

        s._module[i] = ::sqrt(_module[i]);
    }

    return s;
}

OSpectre OSpectre::exp(const double coef)
{
    OSpectre s;

    // Recopie de l'empreinte du spectre
    s._etat = _etat; s._type = _type;

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        s._module[i] = ::exp(coef * _module[i]);
    }

    return s;
}

OSpectre OSpectre::sin() const
{
    OSpectre s;

    // Recopie de l'empreinte du spectre
    s._etat = _etat; s._type = _type;

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        s._module[i] = ::sin(this->_module[i]);
    }

    return s;
}

OSpectre OSpectre::cos() const
{
    OSpectre s;

    // Recopie de l'empreinte du spectre
    s._etat = _etat; s._type = _type;

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        s._module[i] = ::cos(this->_module[i]);
    }

    return s;
}

OSpectre OSpectre::tan() const
{
    return this->sin().div(this->cos());
}

OSpectre OSpectre::abs() const
{
    OSpectre s;

    // Recopie de l'empreinte du spectre
    s._etat = _etat; s._type = _type;

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        s._module[i] = fabs(_module[i]);
    }

    return s;
}

OSpectre OSpectre::sqrt() const
{
    OSpectre s;

    // Recopie de l'empreinte du spectre
    s._etat = _etat; s._type = _type;

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        s._module[i] = ::sqrt(this->_module[i]);
    }

    return s;
}

double OSpectre::valMax()
{
    double result = 0.0;

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        result = max(this->_module[i], result);
    }

    return result;
}


OSpectre OSpectre::seuillage(const double& min, const double max)
{
    OSpectre s;

    // Recopie de l'empreinte du spectre
    s._etat = _etat; s._type = _type;

    // Si min >= max on retourne le spectre original
    if (min >= max) { return *this; }

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        s._module[i] = _module[i] <= min ? min : _module[i];
        s._module[i] = s._module[i] >= max ? max : s._module[i];
    }

    return s;
}


void OSpectre::toSpectreLeger(TYSpectreLeger& spectre) const
{
    OSpectre s(this->toGPhy());

    for (unsigned int i = 0 ; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        spectre._module[i] = s._module[i];
    }
}

void OSpectre::fromSpectreLeger(const TYSpectreLeger& spectre)
{
    _etat = SPECTRE_ETAT_LIN;

    for (unsigned int i = 0 ; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        _module[i] = spectre._module[i];
    }
}

// Fonctions statiques

std::map<float, int>  OSpectre::setMapFreqIndice()
{
    std::map <float, int> mapFI;
    float value = 0.0;

    for (unsigned int i = 0 ; i < TY_SPECTRE_DEFAULT_NB_ELMT ; i++)
    {
        value = _freqNorm[i];
        mapFI[value] = i;
    }

    return mapFI;
}

OSpectre OSpectre::getEmptyLinSpectre(const double& valInit)
{
    OSpectre s(valInit);
    s._etat = SPECTRE_ETAT_LIN;

    return s;
}

OTabFreq OSpectre::getTabFreqExact()
{
    OTabFreq tabFreqExact;

    for (int i = 0; i < (int)TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        tabFreqExact.push_back(1000.0 * pow(pow(10.0, 0.1), i - 18));
    }

    return tabFreqExact;
}

OSpectre OSpectre::getOSpectreFreqExact()
{
    OSpectre tabFreqExact;

    for (int i = 0; i < (int)TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        tabFreqExact._module[i] = 1000.0 * pow(pow(10.0, 0.1), i - 18);
    }

    return tabFreqExact;
}

OSpectre OSpectre::pondA()
{
    OSpectre s;
    OTabFreq tabFreqExact = getTabFreqExact();

    long   v1 = 12200 * 12200;
    double v2 = 20.6 * 20.6;
    double v3 = 107.7 * 107.7;
    double v4 = 737.9 * 737.9;
    double f2 = 1000.0 * 1000.0;                        // frequence au carre
    double v1000 = (v1 * f2 * f2) / ((f2 + v1) * (f2 + v2) * ::sqrt((f2 + v3) * (f2 + v4))); //reference a 1000 Hz

    double valeur = 0.0;

    for (unsigned int i = 0 ; i < TY_SPECTRE_DEFAULT_NB_ELMT ; i++)
    {
        f2 = tabFreqExact[i] * tabFreqExact[i];

        valeur = (v1 * f2 * f2) / ((f2 + v1) * (f2 + v2) * ::sqrt((f2 + v3) * (f2 + v4)));
        valeur = 20 * log10(valeur / v1000);

        s._module[i] = valeur;
    }

    return s;
}

OSpectre OSpectre::pondB()
{
    OSpectre s;
    OTabFreq tabFreqExact = getTabFreqExact();

    long   v1 = 12200 * 12200;
    double v2 = 20.6 * 20.6;
    double v3 = 158.5 * 158.5;
    double f = 1000.0;
    double f2 = f * f;                      // frequence au carre
    double v1000 = (v1 * f2 * f) / ((f2 + v1) * (f2 + v2) * ::sqrt(f2 + v3)); //reference a 1000 Hz

    double valeur = 0.0;

    for (unsigned int i = 0 ; i < TY_SPECTRE_DEFAULT_NB_ELMT ; i++)
    {
        f  = tabFreqExact[i];
        f2 = f * f;

        valeur = (v1 * f2 * f) / ((f2 + v1) * (f2 + v2) * ::sqrt(f2 + v3));
        valeur = 20 * log10(valeur / v1000);

        s._module[i] = valeur;
    }

    return s;
}

OSpectre OSpectre::pondC()
{
    OSpectre s;
    OTabFreq tabFreqExact = getTabFreqExact();

    long   v1 = 12200 * 12200;
    double v2 = 20.6 * 20.6;
    double f2 = 1000.0 * 1000.0;                        // frequence au carre
    double v1000 = (v1 * f2) / ((f2 + v1) * (f2 + v2)); //reference a 1000 Hz

    double valeur = 0.0;

    for (unsigned int i = 0 ; i < TY_SPECTRE_DEFAULT_NB_ELMT ; i++)
    {
        f2 = tabFreqExact[i] * tabFreqExact[i];

        valeur = (v1 * f2) / ((f2 + v1) * (f2 + v2));
        valeur = 20 * log10(valeur / v1000);

        s._module[i] = valeur;
    }

    return s;
}

OSpectre OSpectre::getLambda(const double& c)
{
    OSpectre s;

    // Recopie de l'empreinte du spectre
    s._etat = SPECTRE_ETAT_LIN;
    s._type = SPECTRE_TYPE_AUTRE;

    OTabFreq tabFreqExact = getTabFreqExact();

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        s._module[i] = c / tabFreqExact[i];
    }

    return s;
}

double OSpectre::sigma()
{
    double somme = 0;

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        somme += _module[i];
    }

    return somme;
}

const double OSpectre::sigma() const
{
    double somme = 0;

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        somme += _module[i];
    }

    return somme;
}

bool OSpectre::isTonalite()const
{
    if ((_etat == SPECTRE_ETAT_LIN) || (_form == SPECTRE_FORM_OCT)) { return false; }

    const double seuil = 315;
    double a, b, c, d, e, ab, de, diffG, diffD, freq, tolerence;
    a = _module[0];
    b = _module[1];
    c = _module[2];
    d = _module[3];
    e = _module[4];
    tolerence = 10.0;

    for (unsigned int i = 5; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        ab = 10 * ::log10(((::pow(10, a / 10.0) + ::pow(10, b / 10.0)) / 2.0));
        de = 10 * ::log10(((::pow(10, d / 10.0) + ::pow(10, e / 10.0)) / 2.0));
        diffG = c - ab;
        diffD = c - de;
        freq = _freqNorm[i - 3];
        if (freq > seuil) { tolerence = 5.0; }
        if ((diffG > tolerence) && (diffD > tolerence)) { return true; }
        a = b;
        b = c;
        c = d;
        d = e;
        e = _module[i];
    }

    return false;
}


unsigned int OSpectre::getNbValues()
{
    unsigned int nbFreq = TY_SPECTRE_DEFAULT_NB_ELMT;

    switch (_form)
    {
        case SPECTRE_FORM_OCT:
            nbFreq = 9;
            break;
        default:
            nbFreq = TY_SPECTRE_DEFAULT_NB_ELMT;
            break;
    }

    return nbFreq;
}

const unsigned int OSpectre::getNbValues()const
{
    unsigned int nbFreq = TY_SPECTRE_DEFAULT_NB_ELMT;

    switch (_form)
    {
        case SPECTRE_FORM_OCT:
            nbFreq = 9;
            break;
        default:
            nbFreq = TY_SPECTRE_DEFAULT_NB_ELMT;
            break;
    }

    return nbFreq;
}

