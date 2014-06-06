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
#include "Tympan/GraphicIHM/DataManagerIHM/TYRouteDeblaiWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYRouteGraphic.h"
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/Tools/OMessageManager.h"


TY_EXTENSION_INST(TYRouteDeblai);

TYRouteDeblai::TYRouteDeblai()
{
    _name = TYNameManager::get()->generateName(getClassName());
    std::ofstream logs;
    logs.open("infosRoute.txt", ios::out | ios::app);
    logs << "Le nom de la route est " << _name.toStdString() << endl;
    logs.close();

    largeur = 5.0;
}

TYRouteDeblai::TYRouteDeblai(const TYRouteDeblai& other)
{
    *this = other;
}

TYRouteDeblai::~TYRouteDeblai()
{
}

TYRouteDeblai& TYRouteDeblai::operator=(const TYRouteDeblai& other)
{
    if (this != &other)
    {
        TYAcousticLine::operator =(other);
        _vitMoy = other._vitMoy;
        _modeCalcul = other._modeCalcul;
        _pTraficJour = other._pTraficJour;
        _pTraficNuit = other._pTraficNuit;
    }
    return *this;
}

bool TYRouteDeblai::operator==(const TYRouteDeblai& other) const
{
    if (this != &other)
    {
        if (TYAcousticLine::operator !=(other)) { return false; }
        if (_vitMoy != other._vitMoy) { return false; }
        if (_modeCalcul != other._modeCalcul) { return false; }
        if (_pTraficJour != other._pTraficJour) { return false; }
        if (_pTraficNuit != other._pTraficNuit) { return false; }
    }
    return true;
}

bool TYRouteDeblai::operator!=(const TYRouteDeblai& other) const
{
    return !operator==(other);
}

bool TYRouteDeblai::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYAcousticLine::deepCopy(pOther, copyId)) { return false; }

    TYRouteDeblai* pOtherRoute = (TYRouteDeblai*) pOther;

    _vitMoy = pOtherRoute->_vitMoy;
    _modeCalcul = pOtherRoute->_modeCalcul;

    _pTraficJour->deepCopy(pOtherRoute->_pTraficJour, copyId);
    _pTraficNuit->deepCopy(pOtherRoute->_pTraficNuit, copyId);

    return true;
}

std::string TYRouteDeblai::toString() const
{
    return "TYRouteDeblai";
}

DOM_Element TYRouteDeblai::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYAcousticLine::toXML(domElement);

    TYXMLTools::addElementIntValue(domNewElem, "modeCalcul", _modeCalcul);
    TYXMLTools::addElementDoubleValue(domNewElem, "vitMoy", _vitMoy);

    _pTraficJour->toXML(domNewElem);
    _pTraficNuit->toXML(domNewElem);

    return domNewElem;
}

int TYRouteDeblai::fromXML(DOM_Element domElement)
{
    TYAcousticLine::fromXML(domElement);

    bool modeCalculOk = false;
    bool vitMoyOk = false;
    bool traficJourFound = false;
    bool traficNuitFound = false;
    DOM_Element elemCur;
    QDomNodeList childs = domElement.childNodes();
    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();
        TYXMLTools::getElementBoolValue(elemCur, "modeCalcul", _modeCalcul, modeCalculOk);
        TYXMLTools::getElementDoubleValue(elemCur, "vitMoy", _vitMoy, vitMoyOk);

        if (!traficJourFound)
        {
            traficJourFound = _pTraficJour->callFromXMLIfEqual(elemCur);
        }
        else if (!traficNuitFound)
        {
            traficNuitFound = _pTraficNuit->callFromXMLIfEqual(elemCur);
        }
    }

    if (_tabRegimes.size() == 1) // Cas d'un ancien fichier "sans regime"
    {
        addRegime(buildRegime()); // On rajoute un regime
        _tabRegimes[0].setName("Jour");
        _tabRegimes[1].setName("Nuit");
    }

    return 1;
}
/*
void TYRoute::setTraficJour(const LPTYTrafic pTrafic)
{
    _pTraficJour = pTrafic;
    _pTraficJour->setParent(this);
    TYSpectre aTYSpectre=computeSpectre(_pTraficJour);
    _pSrcLineic->setRegime(aTYSpectre, 0); // calcul du spectre associe a ce regime

    this->distriSrcs(); //Distribution des sources sur la TYSourceLineic
}

void TYRoute::setTraficNuit(const LPTYTrafic pTrafic)
{
    _pTraficNuit = pTrafic;
    _pTraficNuit->setParent(this);
    TYSpectre aTYSpectre=computeSpectre(_pTraficNuit);
    _pSrcLineic->setRegime(aTYSpectre, 1); // calcul du spectre associe a ce regime

    this->distriSrcs(); //Distribution des sources sur la TYSourceLineic
}
*/
bool TYRouteDeblai::updateAcoustic()
{
    if (_typeDistribution == TY_PUISSANCE_CALCULEE)
    {
        // Calcul des spectres correspondant aux regimes jours et nuit
        TYSpectre traficJour = computeSpectre(_pTraficJour);
        TYSpectre traficNuit = computeSpectre(_pTraficNuit);

        traficJour.setType(SPECTRE_TYPE_LW);
        traficNuit.setType(SPECTRE_TYPE_LW);

        // Affectation des regimes
        _tabRegimes[0].setSpectre(traficJour);
        _tabRegimes[1].setSpectre(traficNuit);
    }

    // Distribution des sources et affectation de la puissance acoustique
    TYAcousticLine::updateAcoustic();

    return true;
}

void TYRouteDeblai::distriSrcs()
{
    TYAcousticLine::distriSrcs();

    TYTabLPSourcePonctuelle tabSrcs = _pSrcLineic->getSrcs();

    LPTYPoint pPos = NULL;

    // Ajout d'un offset a la hauteur des sources
    for (unsigned int i = 0; i < tabSrcs.size(); i++)
    {
        pPos = tabSrcs[i]->getPos();
        pPos->_z += _offSet;
    }

}

bool TYRouteDeblai::computeConstraint()
{
    purge();

    //Construction de la geometrie de la route. Code extrait de TYEcran
    OVector3D pt0, pt1, pt2;
    OVector3D s0, s1; // sommets pour les faces (current)
    OVector3D ls0, ls1; // last computed values
    unsigned int firstIndex; //Index du premier point du polygone courant
    size_t count = _tabPoint.size();
    TYRepere repMur;

    if ((count < 2) || (largeur <= 0.0))
    {
        return false;
    }

    OVector3D zvec(0, 0, 1);
    OVector3D vecN; //vecteur normal au segment courant

    // Cree la premiere face de cote
    pt0 = _tabPoint[0];
    pt1 = _tabPoint[1];

    // Vecteur pour la "longueur" de la face
    OVector3D vec01(pt0, pt1);
    vec01.normalize();
    // compute normal to the vector vec01
    vecN = vec01.cross(zvec);
    vecN.normalize();
    s0 = pt0 + (largeur / 2.0f) * vecN;
    s1 = pt0 - (largeur / 2.0f) * vecN;
    //s2 = s0 + hauteur * zvec;
    //s3 = s1 + hauteur * zvec;

    //pFace = newFace(s0,s1,s3,s2); // remarque: l'ordre des sommets est important car il definit le sens de la normale!!!
    //if (pFace != NULL) faces.push_back(pFace);

    ls0 = s0;
    ls1 = s1;
    //ls2 = s2;
    //ls3 = s3;

    //Ajout de la base de la route comme contrainte
    firstIndex = pointsAltimetrie.size();
    pointsAltimetrie.push_back(OPoint3D(s0._x, s0._y, s0._z));
    pointsAltimetrie.push_back(OPoint3D(s1._x, s1._y, s1._z));
    contraintes.push_back(std::pair<unsigned int, unsigned int>(firstIndex, firstIndex + 1));



    // On construit les faces de la tubulure
    OPoint3D a1, b1, c1, d1; // (a1,b1) et (c1,d1) segment // au 1er segment
    OPoint3D a2, b2, c2, d2; // (a2,b2) et (c2,d2) segment // au segment suivant
    OPoint3D ptI1, ptI2;
    for (size_t i = 1; i < count - 1; i++)
    {
        pt0 = _tabPoint[i - 1];
        pt1 = _tabPoint[i];
        pt2 = _tabPoint[i + 1];

        // Vecteur pour la "longueur" de la face
        OVector3D vec01(pt0, pt1); // 1er segment
        vec01.normalize();
        OVector3D vec02(pt1, pt2); // segment suivant
        vec02.normalize();

        vecN = vec01.cross(zvec); // compute normal to the vector vec01
        vecN.normalize();

        a1 = pt0 + (largeur / 2.0f) * vecN;
        b1 = pt1 + (largeur / 2.0f) * vecN;
        c1 = pt0 - (largeur / 2.0f) * vecN;
        d1 = pt1 - (largeur / 2.0f) * vecN;

        vecN = vec02.cross(zvec); // compute normal to the vector vec01
        vecN.normalize();

        a2 = pt1 + (largeur / 2.0f) * vecN;
        b2 = pt2 + (largeur / 2.0f) * vecN;
        c2 = pt1 - (largeur / 2.0f) * vecN;
        d2 = pt2 - (largeur / 2.0f) * vecN;

        OGeometrie::intersDroitesPoints(a1, b1, a2, b2, ptI1);
        OGeometrie::intersDroitesPoints(c1, d1, c2, d2, ptI2);

        s0 = ptI1;
        s1 = ptI2;
        //s2 = s0 + hauteur * zvec;
        //s3 = s1 + hauteur * zvec;

        // bottom
        TYTabPoint pFace;
        pFace.push_back(ls0);
        pFace.push_back(s0);
        pFace.push_back(s1);
        pFace.push_back(ls1);
        enveloppes.push_back(pFace);

        firstIndex = (unsigned int)((int)(pointsAltimetrie.size()) - 2); //Indice de ls0
        pointsAltimetrie.push_back(OPoint3D(s0._x, s0._y, s0._z));
        pointsAltimetrie.push_back(OPoint3D(s1._x, s1._y, s1._z));
        contraintes.push_back(std::pair<unsigned int, unsigned int>(firstIndex, firstIndex + 2)); //segment (ls0,s0)
        contraintes.push_back(std::pair<unsigned int, unsigned int>(firstIndex + 2, firstIndex + 3)); //segment (s0,s1)
        contraintes.push_back(std::pair<unsigned int, unsigned int>(firstIndex + 3, firstIndex + 1)); //segment (s1,ls1)


        // keep previous vertice
        ls0 = s0;
        ls1 = s1;
    }

    // derniere portion de tubulure
    pt0 = _tabPoint[count - 2];
    pt1 = _tabPoint[count - 1];

    // Vecteur pour la "longueur" de la face
    OVector3D vec02(pt0, pt1);
    vec02.normalize();
    // compute normal to the vector vec02
    vecN = vec02.cross(zvec);
    vecN.normalize();
    s0 = pt1 + (largeur / 2.0f) * vecN;
    s1 = pt1 - (largeur / 2.0f) * vecN;

    // bottom
    TYTabPoint pFace;
    pFace.push_back(ls0);
    pFace.push_back(s0);
    pFace.push_back(s1);
    pFace.push_back(ls1);
    enveloppes.push_back(pFace);

    firstIndex = (unsigned int)((int)(pointsAltimetrie.size()) - 2); //Indice de ls0
    pointsAltimetrie.push_back(OPoint3D(s0._x, s0._y, s0._z));
    pointsAltimetrie.push_back(OPoint3D(s1._x, s1._y, s1._z));
    contraintes.push_back(std::pair<unsigned int, unsigned int>(firstIndex, firstIndex + 2)); //segment (ls0,s0)
    contraintes.push_back(std::pair<unsigned int, unsigned int>(firstIndex + 2, firstIndex + 3)); //segment (s0,s1)
    contraintes.push_back(std::pair<unsigned int, unsigned int>(firstIndex + 3, firstIndex + 1)); //segment (s1,ls1)

    //Logs de construction de la route :
    std::ofstream logs;
    logs.open("contraintes.txt", ios::out | ios::trunc);

    logs << "/////////////////////////////////////" << endl;
    logs << "Description de contrainte d'une route" << endl;
    logs << "/////////////////////////////////////" << endl;
    logs << endl;
    logs << "La route a donnee " << enveloppes.size() << " enveloppes." << endl;
    for (unsigned int i = 0; i < enveloppes.size(); i++)
    {
        logs << "Description de l'enveloppe " << i << ":" << endl;
        for (unsigned int j = 0; j < enveloppes.at(i).size(); j++)
        {
            double coords[3];
            enveloppes.at(i).at(j).getCoords(coords);
            logs << "Point " << j << " (" << coords[0] << "," << coords[1] << "," << coords[2] << ")" << endl;
        }
    }
    logs << "Le route a donnee " << pointsAltimetrie.size() << " points d'emprise." << endl;
    for (unsigned int i = 0; i < pointsAltimetrie.size(); i++)
    {
        logs << "Point " << i << " (" << pointsAltimetrie.at(i)._x << "," << pointsAltimetrie.at(i)._y << "," << pointsAltimetrie.at(i)._z << ")" << endl;
    }
    logs << endl;
    logs.close();

    return true;


}
