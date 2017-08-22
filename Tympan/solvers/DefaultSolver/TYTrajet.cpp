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

#include "TYTrajet.h"


TYTrajet::TYTrajet(tympan::AcousticSource& asrc_, tympan::AcousticReceptor& arcpt_) :
    asrc(asrc_),
    arcpt(arcpt_),
    _distance(0.0)
{
    _ptS = asrc.position;
    _ptR = arcpt.position;
    _distance = _ptS.distFrom(_ptR);
	//build_tab_rays();
}


TYTrajet::TYTrajet(const TYTrajet& other) : asrc(other.asrc), arcpt(other.arcpt)
{
    *this = other;
}

TYTrajet::~TYTrajet()
{
    reset();
}

void TYTrajet::reset()
{
    _chemins.clear();
    _cheminsDirect.clear();
}


TYTrajet& TYTrajet::operator=(const TYTrajet& other)
{
    if (this != &other)
    {
        _chemins = other._chemins;
        _ptS = other._ptS;
        _ptR = other._ptR;
        _distance = other._distance;
        _sLP = other._sLP;
        asrc = other.asrc;
        arcpt = other.arcpt;
        asrc_idx = other.asrc_idx;
        arcpt_idx = other.arcpt_idx;
    }
    return *this;
}

bool TYTrajet::operator==(const TYTrajet& other) const
{
    if (this != &other)
    {
        if (_chemins != other._chemins) { return false; }
        if (_ptS != other._ptS) { return false; }
        if (_ptR != other._ptR) { return false; }
        if (_distance != other._distance) { return false; }
        if (_sLP != other._sLP) { return false; };
        //if (asrc != other.asrc) { return false; };
        //if (arcpt != other.arcpt) ;
        if (asrc_idx != other.asrc_idx) { return false; };
        if (arcpt_idx != other.arcpt_idx) { return false; };
    }

    return true;
}

bool TYTrajet::operator!=(const TYTrajet& other) const
{
    return !operator==(other);
}

void TYTrajet::addChemin(const TYChemin& chemin)
{
    _chemins.push_back(chemin);
}

void TYTrajet::addCheminDirect(const TYChemin& chemin)
{
    _cheminsDirect.push_back(chemin);
}

OSpectre TYTrajet::getPNoOp()
{
    return _chemins[0].getAttenuation();
}

OSpectre TYTrajet::getPEnergetique(const AtmosphericConditions& atmos)
{
    OSpectre s = OSpectre::getEmptyLinSpectre();
    OSpectreComplex sTemp;
    int firstReflex = -1;
    int indiceDebutEffetEcran = 0;
    unsigned int i;

    // On calcule l'attenuation sur le trajet direct (sauf chemins reflechis).
    for (i = 0 ; i < this->_chemins.size() ; i++)
    {
        // Si un ecran est present, on ne traite pas les reflexions (dans un premier temp ...)
        if ((_chemins[0].getType() == CHEMIN_ECRAN) && (_chemins[i].getType() == CHEMIN_REFLEX))
        {
            firstReflex = i;
            break;
        }
        sTemp = _chemins[i].getAttenuation();
        s     = s.sum(sTemp.mult(sTemp)); // somme des carres des modules
    }

    // Dans le cas d'un ecran, on compare l'attenuation obtenue a celle du trajet direct
    // pour eviter les effets d'amplification (plus de bruit avec l'ecran que sans ecran ...)
    if (_chemins[0].getType() == CHEMIN_ECRAN)
    {
        OSpectre attDirect = OSpectre::getEmptyLinSpectre();

        for (i = 0; i < _cheminsDirect.size() ; i++)
        {
            sTemp = _cheminsDirect[i].getAttenuation();
            attDirect = attDirect.sum(sTemp.mult(sTemp));
        }

        // On regarde l'attenuation globale obtenue pour chaque frequence,
        // on la compare a celle obtenue sur le trajet sans ecran,
        // si elle est superieure a 1 alors on prend la valeur obtenue pour le trajet sans ecran
        for (i = 0 ; i < s.getNbValues() ; i++)
        {
            if (s.getTabValReel()[i] < attDirect.getTabValReel()[i])
            {
                indiceDebutEffetEcran = i; // On prend note de l'indice
                break; // Si l'ecran commence a attenuer plus que le trajet direct, il faut sortir de la boucle
            }
        } //*/

        if (firstReflex != -1) // S'il y a une reflexion sur un ecran
        {
            // On rajoute la contribution des chemins reflechis
            // 1. Aux chemins normaux et aux chemins directs
            for (i = firstReflex ; i < _chemins.size() ; i++)
            {
                sTemp = _chemins[i].getAttenuation().mult(_chemins[i].getAttenuation());
                s = s.sum(sTemp);
                attDirect = attDirect.sum(sTemp);
            }

            // On remplace la contribution du trajet direct pour toutes les frequences ou cela est necessaire
            for (i = 0; i < indiceDebutEffetEcran; i++)
            {
                s.getTabValReel()[i] = attDirect.getTabValReel()[i];
            }
        }

    }
    build_tab_rays();
    _chemins.clear();  // On efface le tableau des chemins pour (essayer de) gagner de la place en memoire
    _cheminsDirect.clear();
    return s;
}

OSpectre TYTrajet::getPInterference(const AtmosphericConditions& atmos)
{
    unsigned int i, j;
    int firstReflex = -1;
    int indiceDebutEffetEcran = 0;
    bool ecranFound = false;

    if (_chemins.size()) { ecranFound = (_chemins[0].getType() == CHEMIN_ECRAN); }

    // On recupere prealablement l'ensemble des attenuations et les longueurs des chemins
    std::vector<OSpectreComplex> tabSpectreAtt;
    std::vector<OSpectreComplex> tabSpectreAttDirect;
    std::vector<double> tabLongueur;
    std::vector<double> tabLongueurDirect;

    for (i = 0 ; i < _chemins.size(); i++)
    {
        tabSpectreAtt.push_back(_chemins[i].getAttenuation());
        tabLongueur.push_back(_chemins[i].getLongueur());
    }

    // On calcule la somme des carres des modules et la somme des produits croises

    OSpectre sCarreModule = OSpectre::getEmptyLinSpectre();
    OSpectre sProduitCroise = OSpectre::getEmptyLinSpectre();
    OSpectre sTemp;

    for (i = 0 ; i < _chemins.size(); i++)
    {

        // Si un ecran est present, on ne traite pas les reflexions (dans un premier temp ...)
        if (ecranFound && (_chemins[i].getType() == CHEMIN_REFLEX))
        {
            firstReflex = i;
            break;
        }
        // on fait la somme du carre des modules
        sCarreModule = sCarreModule.sum(tabSpectreAtt[i].mult(tabSpectreAtt[i]));

        // on calcule les produits croises avec les autres chemins
        for (j = i + 1; j < _chemins.size(); j++)
        {
            // On procedera aux produits croise avec les chemins reflechis plus loin ...
            if (ecranFound && (_chemins[j].getType() == CHEMIN_REFLEX)) { continue ; }

            sTemp = tabSpectreAtt[i].mult(tabSpectreAtt[j].mult(2.0));
            sTemp = sTemp.mult(correctTiers(tabSpectreAtt[i], tabSpectreAtt[j], atmos, tabLongueur[i], tabLongueur[j]));
            sProduitCroise = sProduitCroise.sum(sTemp);
        }
    }

    OSpectre s = sCarreModule.sum(sProduitCroise); //.abs() ;

    // Dans le cas d'un ecran, on compare l'attenuation obtenue a celle du trajet direct
    // pour eviter les effets d'amplification (plus de bruit avec l'ecran que sans ecran ...)

    if (ecranFound) // On comparera au carre des modules des trajets directs
    {
        // On calcule l'attenuation sur le trajet direct
        OSpectre sCarreModuleDirect = OSpectre::getEmptyLinSpectre();
        OSpectre sProduitCroiseDirect = OSpectre::getEmptyLinSpectre();

        for (i = 0 ; i < _cheminsDirect.size(); i++)
        {
            tabSpectreAttDirect.push_back(_cheminsDirect[i].getAttenuation());
            tabLongueurDirect.push_back(_cheminsDirect[i].getLongueur());
        }

        for (i = 0 ; i < _cheminsDirect.size(); i++)
        {
            // on fait la somme du carre des modules
            sCarreModuleDirect = sCarreModuleDirect.sum(tabSpectreAttDirect[i].mult(tabSpectreAttDirect[i]));

            // on calcule les produits croises avec les autres chemins
            for (j = i + 1; j < _cheminsDirect.size(); j++)
            {
                sTemp = tabSpectreAttDirect[i].mult(tabSpectreAttDirect[j].mult(2.0));
                sTemp = sTemp.mult(correctTiers(tabSpectreAttDirect[i], tabSpectreAttDirect[j], atmos, tabLongueurDirect[i], tabLongueurDirect[j]));
                sProduitCroiseDirect = sProduitCroiseDirect.sum(sTemp);
            }
        }

        OSpectre attDirect = sCarreModuleDirect.sum(sProduitCroiseDirect); //.abs() ;

        // On compare l'attenuation sur le trajet "normal" en energetique a
        // l'attenuation sur le trajet direct en energetique.
        // Si elle est superieure, alors on prend l'attenuation sur le trajet direct

        for (j = 0 ; j < s.getNbValues() ; j++)
        {
            if (s.getTabValReel()[j] < attDirect.getTabValReel()[j])
            {
                indiceDebutEffetEcran = j; // On prend note de l'indice
                break; // Si l'ecran commence a attenuer plus que le trajet direct, il faut sortir de la boucle
            }
        }

        if (firstReflex != -1) // S'il y a une reflexion sur un ecran
        {
            // On rajoute la contribution des chemins reflechis:
            // 1. aux chemins "normaux"
            for (i = firstReflex; i < _chemins.size() ; i++)
            {
                sCarreModule = sCarreModule.sum(tabSpectreAtt[i].mult(tabSpectreAtt[i]));

                // on calcule les produits croises avec les autres chemins
                for (j = 0 ; j < _chemins.size(); j++)
                {
                    if (j == i) { continue; } // pas avec lui meme

                    sTemp = tabSpectreAtt[i].mult(tabSpectreAtt[j].mult(2.0));
                    sTemp = sTemp.mult(correctTiers(tabSpectreAtt[i], tabSpectreAtt[j], atmos, tabLongueur[i], tabLongueur[j]));
                    sProduitCroise = sProduitCroise.sum(sTemp);
                }

            }

            s = sCarreModule.sum(sProduitCroise);

            //      // 2. au chemins "direct"
            for (i = firstReflex; i < _chemins.size() ; i++)
            {
                sCarreModuleDirect = sCarreModuleDirect.sum(tabSpectreAtt[i].mult(tabSpectreAtt[i]));

                // Produit croise avec les chemins directs
                for (j = 0; j < _cheminsDirect.size() ; j++)
                {
                    sTemp = tabSpectreAtt[i].mult(tabSpectreAttDirect[j].mult(2.0));
                    sTemp = sTemp.mult(correctTiers(tabSpectreAtt[i], tabSpectreAttDirect[j], atmos, tabLongueur[i], tabLongueurDirect[j]));
                    sProduitCroiseDirect = sProduitCroiseDirect.sum(sTemp);
                }
                // Produit croise avec les autres chemins reflechis
                for (j = i + 1; j < _chemins.size(); j++)
                {
                    sTemp = tabSpectreAtt[i].mult(tabSpectreAtt[j].mult(2.0));
                    sTemp = sTemp.mult(correctTiers(tabSpectreAtt[i], tabSpectreAtt[j], atmos, tabLongueur[i], tabLongueur[j]));
                    sProduitCroiseDirect = sProduitCroiseDirect.sum(sTemp);
                }

            }

            attDirect = sCarreModuleDirect.sum(sProduitCroiseDirect); //.abs();
        }

        // On remplace la contribution du trajet direct pour toutes les frequences ou cela est necessaire
        for (i = 0; i < indiceDebutEffetEcran; i++)
        {
            s.getTabValReel()[i] = attDirect.getTabValReel()[i];
        }
    }
    build_tab_rays();
    _chemins.clear();  // On efface le tableau des chemins pour (essayer de) gagner de la place en memoire
    _cheminsDirect.clear();

    return s;
}

OSpectre TYTrajet::correctTiers(const OSpectreComplex& si, const OSpectreComplex& sj, const AtmosphericConditions& atmos, const double& ri, const double& rj) const
{
    const double dp6 = pow(2, (1.0 / 6.0));
    const double invdp6 = 1.0 / dp6;
    const double dfSur2f = (dp6 - invdp6) / 2.0;  // df/2f
    OSpectre cosTemp;
    OSpectre s;

    OSpectre sTemp = atmos.get_k().mult(ri - rj); // k(ri-rj)

    if (ri == rj)
    {
        s = (si.getPhase().subst(sj.getPhase()).subst(sTemp)).cos(); // cos(EPS_i - EPS_j)
    }
    else
    {

        s = si.getPhase().subst(sj.getPhase()); // thetaI - thetaJ

        double df = sqrt(1 + dfSur2f * dfSur2f);
        cosTemp = sTemp.mult(df); // k(ri-rj) * sqrt(1 + (df/2f)²)
        cosTemp = cosTemp.sum(s);// k(ri-rj) * sqrt(1 + (df/2f)²) + (thetaI - thetaJ)
        cosTemp = cosTemp.subst(sTemp); // k(ri-rj)*sqrt(1 + (df/2f)²) + EPS_i - EPS_j
        cosTemp = cosTemp.cos(); // cos(k(ri-rj)*sqrt(1 + (df/2f)²) + EPS_i - EPS_j)

        sTemp = sTemp.mult(dfSur2f); // k(ri-rj)*df/2f

        s = sTemp.sin();          // sin(k(ri-rj)*df/2f)
        s = s.div(sTemp);           // sin(k(ri-rj)*df/2f) / (k(ri-rj)*df/2f)
        s = s.mult(cosTemp);        // sin(k(ri-rj)*df/2f) / (k(ri-rj)*df/2f) * cos(k(ri-rj)*sqrt(1 + (df/2f)²) + EPS_i - EPS_j)
    }

    return s;
}

void TYTrajet::build_tab_rays()
{
    _tabRays.clear();
    for (size_t i=0; i<_chemins.size(); i++)
    {
        _tabRays.push_back(_chemins[i].get_ray(_ptR));
    }
}

std::vector<acoustic_path*>& TYTrajet::get_tab_rays()
{
	return _tabRays;
}