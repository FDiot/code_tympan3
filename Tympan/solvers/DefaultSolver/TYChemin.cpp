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

#include "TYChemin.h"
#include "Tympan/models/solver/config.h"


TYChemin::TYChemin() :  _typeChemin(CHEMIN_DIRECT), _longueur(0.0), _distance(0.0), _eq_path(nullptr)
{
    _attenuation = OSpectreComplex::getEmptyLinSpectre();
}

TYChemin::TYChemin(const TYChemin& other)
{
    *this = other;
}

TYChemin::~TYChemin()
{
    if(tympan::SolverConfiguration::get()->Anime3DKeepRays ==  false){
        _eq_path->cleanEventsTab();
        }
}


TYChemin& TYChemin::operator=(const TYChemin& other)
{
    if (this != &other)
    {
        _typeChemin = other._typeChemin;
        _distance = other._distance;
        _longueur = other._longueur;
		_eq_path = other._eq_path;
        _attenuation = other._attenuation;
    }

    return *this;
}

bool TYChemin::operator==(const TYChemin& other) const
{
    if (this != &other)
    {
        if (_typeChemin != other._typeChemin) { return false; }
        if (_distance != other._distance) { return false; }
        if (_longueur != other._longueur) { return false; }
		if (_eq_path != other._eq_path) { return false; };
        if (_attenuation != other._attenuation) { return false; }
    }

    return true;
}

bool TYChemin::operator!=(const TYChemin& other) const
{
    return !operator==(other);
}

void TYChemin::calcAttenuation(const TYTabEtape& tabEtapes, const AtmosphericConditions& atmos)
{
    unsigned int i;

    OSpectre phase = OSpectre::getEmptyLinSpectre(); //Reference de phase

    switch (_typeChemin)
    {
        case CHEMIN_DIRECT: // S*A*e^(i.k.Rd) avec A =attenuation atmosphere et S=directivite de la source
            _attenuation = tabEtapes[0]._Absorption; //directivite de la source (S)
//            _attenuation = _attenuation.mult(atmos.getAtt(_longueur)); // S*A (A = attenuation atmospherique)
            _attenuation = _attenuation.mult(atmos.compute_length_absorption(_longueur)); // S*A (A = attenuation atmospherique)

//            phase = atmos.getKAcoust().mult(_longueur); // = kRd
            phase = atmos.get_k().mult(_longueur); // = kRd
            _attenuation.setPhase(phase); // =e^(i.kRd) //*/

            break;

        case CHEMIN_SOL: //S*A*Q*e^(i.k.Rr)/Rr  //avec Q absorption du sol
            _attenuation = tabEtapes[0]._Absorption; //directivite de la source (S)
            _attenuation = _attenuation.mult(atmos.compute_length_absorption(_longueur));// S*A (A = attenuation atmospherique)

            _attenuation = _attenuation.mult(tabEtapes[1]._Absorption); // S*A*Q
            _attenuation = _attenuation.mult(_distance / _longueur) ; //S*A*Q*Rd / Rr

            phase = atmos.get_k().mult(_longueur); // = kRr
            phase = phase.sum(tabEtapes[1]._Absorption.getPhase()); // kRr + epsilon (epsilon = phase du coeff de reflexion du sol

            _attenuation.setPhase(phase);

            break;

        case CHEMIN_ECRAN: //= S*A*Q/D*e^(i.k.Rd + eps) avec Q=module du coefficient de reflexion du sol et D=attenuation diffraction
            _attenuation = tabEtapes[0]._Absorption; // S = Directivite de la source
            _attenuation = _attenuation.mult(atmos.compute_length_absorption(_longueur)); // S*A (A = attenuation atmospherique)

            phase = atmos.get_k().mult(_longueur); // = kRr

            // On fait le produit des absorptions des etapes a partir de la seconde jusqu'a l'avant derniere
            // la derniere portant l'effet de diffraction
            for (i = 1; i < tabEtapes.size() - 1; i++)
            {
                _attenuation = _attenuation.mult(tabEtapes[i]._Absorption); // S.A.Q
                phase = phase.sum(tabEtapes[i]._Absorption.getPhase()); // kRr + Somme des epsilon i
            }

            _attenuation = _attenuation.div(tabEtapes[tabEtapes.size() - 1]._Attenuation); // S.A.Q/D

            _attenuation.setPhase(phase);
            break;

        case CHEMIN_REFLEX:// S*A*Q*e^(i.k.Rr + eps)/Rr avec Q coefficient de reflexion de la paroi, eps = 0
            _attenuation = tabEtapes[0]._Absorption; // S = Directivite de la source
            _attenuation = _attenuation.mult(atmos.compute_length_absorption(_longueur));// S*A (A = attenuation atmospherique)

            phase = atmos.get_k().mult(_longueur); // = kRr

            // On fait le produit des absorptions des etapes ) partir de la deuxieme
            for (i = 1; i < tabEtapes.size(); i++)
            {
                _attenuation = _attenuation.mult(tabEtapes[i]._Absorption); // Produit des modules
                phase = phase.sum(tabEtapes[i]._Absorption.getPhase()); //Somme des phases
            }

            _attenuation = _attenuation.mult(_distance / _longueur); // <== MODIFIE ON DIVISE PAR Rd² au niveau du solveur (a cause des ecrans)

            _attenuation.setPhase(phase);

            break;

        default:
            break;
    }

	build_eq_path(tabEtapes);
}

void TYChemin::build_eq_path(const TYTabEtape& tabEtapes)
{
    _eq_path = new acoustic_path();

    for (size_t i=0; i<tabEtapes.size(); i++)
    {
        _eq_path->addEvent(tabEtapes[i].asEvent());
    }
}

acoustic_path* TYChemin::get_ray(OPoint3D ptR)
{
    acoustic_event* receptor_event = new acoustic_event();
    receptor_event->pos = ptR;
    receptor_event->type = TYRECEPTEUR;
	_eq_path->addEvent(receptor_event);
	return _eq_path;
}