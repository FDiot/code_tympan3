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

OSpectreComplex::OSpectreComplex(const double &defModule, const double &defPhase) : OSpectre(defModule)
{
    for (unsigned int i = 0 ; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        _phase[i] = defPhase;
    }
}

OSpectreComplex::OSpectreComplex(const OSpectreComplex& other)
{
    *this = other;
}

OSpectreComplex::OSpectreComplex(const OSpectre& other)
{
    OSpectre::operator = (other);

    for (unsigned int i = 0 ; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        _phase[i] = 0.0;
    }
}

OSpectreComplex::OSpectreComplex(const OSpectre& spectre1, const OSpectre& spectre2)
{
    for (unsigned int i = 0 ; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        _module[i] = (spectre1.getTabValReel())[i];
        _phase[i]  = (spectre2.getTabValReel())[i];
    }
}

OSpectreComplex::~OSpectreComplex()
{
}

OSpectreComplex& OSpectreComplex::operator=(const OSpectreComplex& other)
{
    if (this != &other)
    {
        OSpectre::operator = (other);

        for (unsigned int i = 0 ; i < TY_SPECTRE_DEFAULT_NB_ELMT ; i++)
        {
            _phase[i] = other._phase[i];
        }
    }
    return *this;
}

bool OSpectreComplex::operator==(const OSpectreComplex& other) const
{
    if (this != &other)
    {
        if ((OSpectre::operator != (other))) { return false; }

        for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
            if (_phase[i] != other._phase[i]) { return false; }
    }
    return true;
}

bool OSpectreComplex::operator!=(const OSpectreComplex& other) const
{
    return !operator==(other);
}

OSpectreComplex OSpectreComplex::operator + (const OSpectreComplex& spectre) const
{
    OSpectreComplex s;

    // Recopie de l'empreinte du spectre
    s._etat = _etat;
    s._type = _type;

	//WIP // A complex number is defined as z = a + ib
	double a1 = 0.0; // This is the original one
	double b1 = 0.0;
	double a2 = 0.0; // This is the one we add
	double b2 = 0.0;
	double a3 = 0.0; // This is the returned complex number i.e. the result
	double b3 = 0.0;
	double cos3 = 0.0;
	//

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        // WIP
		a1 = _module[i]*std::cos(_phase[i]);
		b1 = _module[i]*std::sin(_phase[i]);
		
		a2 = spectre._module[i]*std::cos(spectre._phase[i]);
		b2 = spectre._module[i]*std::sin(spectre._phase[i]);

		a3 = a1 + a2;
		b3 = b1 + b2;

		s._module[i] = std::sqrt( a3*a3 + b3*b3);
		cos3 = a3 / s._module[i];
		s._phase[i] = std::acos(cos3);
		//
		//s._module[i] = _module[i] + spectre._module[i];
  //      s._phase[i] = _phase[i] + spectre._phase[i];
    }
    return s;
}

OSpectreComplex OSpectreComplex::operator * (const OSpectreComplex& spectre) const
{
	// Produit de deux complexes en module/phase
	//  = produit des modules et somme des phases
    OSpectreComplex s;

    // Recopie de l'empreinte du spectre
    s._etat = _etat;
    s._type = _type;

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        s._module[i] = _module[i] * spectre._module[i];
        s._phase[i] = _phase[i] + spectre._phase[i];
    }
    return s;
}

OSpectreComplex OSpectreComplex::operator * (const OSpectre& spectre) const
{
	// Produit de deux complexes en module/phase
	//  = produit des modules et somme des phases
    OSpectreComplex s;

    // Recopie de l'empreinte du spectre
    s._etat = _etat;
    s._type = _type;

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
		s._module[i] = _module[i] * (spectre.getTabValReel())[i];
        s._phase[i] = _phase[i];
    }
    return s;	
}

OSpectreComplex OSpectreComplex::operator * (const double& coefficient) const
{
    OSpectreComplex s;

    // Recopie de l'empreinte du spectre
    s._etat = _etat; s._type = _type;

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        s._module[i] = _module[i] * coefficient;
        s._phase[i] = _phase[i] * coefficient;
    }

    return s;
}

OSpectreComplex OSpectreComplex::multi(const double& coefficient) const
{
    OSpectreComplex s;

    // Recopie de l'empreinte du spectre
    s._etat = _etat; s._type = _type;

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        s._module[i] = _module[i] * coefficient;
        s._phase[i] = _phase[i] * coefficient;
    }

    return s;
}
OSpectreComplex OSpectreComplex::operator / (const OSpectreComplex& spectre) const
{
	// Produit de deux complexes en module/phase
	//  = rapport des modules et difference des phases
    OSpectreComplex s;

    // Recopie de l'empreinte du spectre
    s._etat = _etat;
    s._type = _type;

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        s._module[i] = _module[i] / spectre._module[i];
        s._phase[i] = _phase[i] - spectre._phase[i];
    }
    return s;
}

void OSpectreComplex::setValue(const float& freq, const double& reel, const double& imag)
{
    int indice = _mapFreqIndice[freq];

    _module[indice] = reel;

    _phase[indice] = imag;
}

void OSpectreComplex::setValue(const float& freq, const TYComplex& cplx)
{
    setValue(freq, cplx.real(), cplx.imag());
}

double OSpectreComplex::getValueImag(float freq, bool* pValid)
{
    int indice = _mapFreqIndice[freq];

    return _phase[indice];
}

void OSpectreComplex::setPhase(const OSpectre& spectre)
{
    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        _phase[i] = spectre.getTabValReel()[i];
    }
}

void OSpectreComplex::setPhase(const double& valeur)
{
    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        _phase[i] = valeur;
    }
}

OSpectre OSpectreComplex::getPhase() const
{
    OSpectre s = OSpectre::getEmptyLinSpectre();

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        s.getTabValReel()[i] = _phase[i];
    }

    return s;
}

OSpectre OSpectreComplex::getModule() const
{
    OSpectre s = OSpectre::getEmptyLinSpectre();

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        s.getTabValReel()[i] = _module[i];
    }

    return s;
}

OSpectreComplex OSpectreComplex::toModulePhase() const
{
    OSpectreComplex s ;
    double module, phase, reel, imag;

    // Recopie de l'empreinte du spectre
    s._etat = _etat;
    s._type = _type;

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        reel = this->_module[i];
        imag = this->_phase[i];

        module = ::sqrt(reel * reel + imag * imag);

        phase = atan2(imag, reel);

        s._module[i] = module;
        s._phase[i] = phase;
    }
    return s;
}

OSpectreComplex OSpectreComplex::toReelImaginaire() const
{
    OSpectreComplex s;
    double reel, imag, module, phase;

    // Recopie de l'empreinte du spectre
    s._etat = _etat;
    s._type = _type;

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        module = this->_module[i];
        phase = this->_phase[i];

        reel = module * ::cos(phase);
        imag = module * ::sin(phase);

        s._module[i] = reel;
        s._phase[i] = imag;
    }
    return s;
}

OSpectreComplex OSpectreComplex::sumComplex(const OSpectreComplex& spectre) const
{
    OSpectreComplex s;

    // Recopie de l'empreinte du spectre
    s._etat = _etat;
    s._type = _type;

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        s._module[i] = this->_module[i] + spectre._module[i];
        s._phase[i] = this->_phase[i] + spectre._phase[i];
    }
    return s;
}

OSpectreComplex OSpectreComplex::sumComplex(const OSpectre& spectre) const
{
    OSpectreComplex s;

    // Recopie de l'empreinte du spectre
    s._etat = _etat;
    s._type = _type;

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
		s._module[i] = this->_module[i] + spectre.getTabValReel()[i];
    }
    return s;
}

OSpectreComplex OSpectreComplex::getCplxSpectre(const double& valInit)
{
    OSpectreComplex s(OSpectre::getEmptyLinSpectre(valInit));

    for (unsigned int i = 0 ; i < TY_SPECTRE_DEFAULT_NB_ELMT ; i++)
    {
        s._phase[i] = 0.0;
    }

    return s;
}
