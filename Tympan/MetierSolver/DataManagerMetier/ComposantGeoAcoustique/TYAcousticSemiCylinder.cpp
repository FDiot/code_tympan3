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
#include "Tympan/GraphicIHM/DataManagerIHM/TYAcousticSemiCylinderWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYAcousticSemiCylinderGraphic.h"
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/core/logging.h"
#include "Tympan/Tools/TYProgressManager.h"


TY_EXTENSION_INST(TYAcousticSemiCylinder);
TY_EXT_GRAPHIC_INST(TYAcousticSemiCylinder);

TYAcousticSemiCylinder::TYAcousticSemiCylinder()
{
    _name = TYNameManager::get()->generateName(getClassName());

    // Couleur par default
    float r = 255.0f, g = 67.0f, b = 133.0f;

#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "TYAcousticSemiCylinderGraphicColorR"))
    {
        TYPreferenceManager::getColor(TYDIRPREFERENCEMANAGER, "TYAcousticSemiCylinderGraphicColor", r, g, b);
    }
    else
    {
        TYPreferenceManager::setColor(TYDIRPREFERENCEMANAGER, "TYAcousticSemiCylinderGraphicColor", r, g, b);
    }
#endif // TY_USE_IHM

    OColor color;
    color.r = r / 255;
    color.g = g / 255;
    color.b = b / 255;

    setColor(color);

    _pEnveloppe = new TYAcousticSurface();
    _pEnveloppe->setParent(this);

    _pSemiCircTop = new TYAcousticSemiCircle();
    *_pSemiCircTop->getShape() = TYRectangle(TYPoint(0, 1, 0.5), TYPoint(1, 1, 0.5), TYPoint(1, -1, 0.5), TYPoint(0, -1, 0.5));
    _pSemiCircTop->setParent(this);

    _pSemiCircBottom = new TYAcousticSemiCircle();
    *_pSemiCircBottom->getShape() = TYRectangle(TYPoint(0, -1, -0.5), TYPoint(1, -1, -0.5), TYPoint(1, 1, -0.5), TYPoint(0, 1, -0.5));
    _pSemiCircBottom->setParent(this);

    // Surface laterale
    _pAccRect = new TYAcousticRectangle();
    _pAccRect->setParent(this);
    _pAccRect->setColor(color);
    updateRect();

    _volEnglob = volEnglob();
    calculRayonSphere(_volEnglob);
    calculCentreGravite();
    updateSurfEnveloppe();

}

TYAcousticSemiCylinder::TYAcousticSemiCylinder(const TYAcousticSemiCylinder& other)
{
    *this = other;
}

TYAcousticSemiCylinder::~TYAcousticSemiCylinder()
{
}

TYAcousticSemiCylinder& TYAcousticSemiCylinder::operator=(const TYAcousticSemiCylinder& other)
{
    if (this != &other)
    {
        TYAcousticVolume::operator =(other);
        _pEnveloppe = other._pEnveloppe;
        _pAccRect = other._pAccRect;
        _pSemiCircTop = other._pSemiCircTop;
        _pSemiCircBottom = other._pSemiCircBottom;
    }

    _volEnglob = volEnglob();
    calculRayonSphere(_volEnglob);
    calculCentreGravite();
    updateSurfEnveloppe();

    return *this;
}

bool TYAcousticSemiCylinder::operator==(const TYAcousticSemiCylinder& other) const
{
    if (this != &other)
    {
        if (TYAcousticVolume::operator !=(other)) { return false; }
        if (_pEnveloppe != other._pEnveloppe) { return false; }
        if (_pAccRect != other._pAccRect) { return false; }
        if (_pSemiCircTop != other._pSemiCircTop) { return false; }
        if (_pSemiCircBottom != other._pSemiCircBottom) { return false; }
    }
    return true;
}

bool TYAcousticSemiCylinder::operator!=(const TYAcousticSemiCylinder& other) const
{
    return !operator==(other);
}

bool TYAcousticSemiCylinder::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYAcousticVolume::deepCopy(pOther, copyId)) { return false; }

    TYAcousticSemiCylinder* pOtherSemiCyl = (TYAcousticSemiCylinder*) pOther;

    _pEnveloppe->deepCopy(pOtherSemiCyl->_pEnveloppe, copyId);
    _pAccRect->deepCopy(pOtherSemiCyl->_pAccRect, copyId);
    _pSemiCircTop->deepCopy(pOtherSemiCyl->_pSemiCircTop, copyId);
    _pSemiCircBottom->deepCopy(pOtherSemiCyl->_pSemiCircBottom, copyId);

    _volEnglob = volEnglob();
    calculRayonSphere(_volEnglob);
    calculCentreGravite();
    updateSurfEnveloppe();

    return true;
}

std::string TYAcousticSemiCylinder::toString() const
{
    return "TYAcousticSemiCylinder";
}

DOM_Element TYAcousticSemiCylinder::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYAcousticVolume::toXML(domElement);

    _pEnveloppe->toXML(domNewElem);
    _pSemiCircTop->toXML(domNewElem);
    _pSemiCircBottom->toXML(domNewElem);
    _pAccRect->toXML(domNewElem);

    return domNewElem;
}

int TYAcousticSemiCylinder::fromXML(DOM_Element domElement)
{
    TYAcousticVolume::fromXML(domElement);

    bool semiCircTopFound = false;
    bool semiCircBottomFound = false;
    bool enveloppeFound = false;

    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();
    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        if (!enveloppeFound)
        {
            enveloppeFound = _pEnveloppe->callFromXMLIfEqual(elemCur);
        }

        if (!semiCircTopFound)
        {
            semiCircTopFound = _pSemiCircTop->callFromXMLIfEqual(elemCur);
        }
        else if (!semiCircBottomFound)
        {
            semiCircBottomFound = _pSemiCircBottom->callFromXMLIfEqual(elemCur);
        }

        _pAccRect->callFromXMLIfEqual(elemCur);
    }

    if (!enveloppeFound) // Lecture d'un fichier TYMPAN 3.2
    {
        _pEnveloppe->setDensiteSrcsH(_densiteSrcsH);
        _pEnveloppe->setDensiteSrcsV(_densiteSrcsV);

        // On ajoute autant de regime a l'enveloppe qu'il y a de regime a la machine.
        // On prend la machine comme reference car il y a un probleme sur le modele chinon
        // un cylindre possede 11 regimes alors que la machine n'en possede que 2.
        LPTYAcousticVolumeNode pMachine = TYAcousticVolumeNode::safeDownCast(this->getParent());
        size_t nbRegMachine = pMachine->getNbRegimes();

        size_t j;
        for (j = 0; j < nbRegMachine - 1; j++) // (-1) car l'enveloppe possede deja au moins un regime
        {
            _pEnveloppe->addRegime();
        }

        // On "construit" les regimes
        bool statusFrom = true;
        bool statusTo   = true;
        for (j = 0; j < _pEnveloppe->getNbRegimes(); j++)
        {
            TYRegime& regimeFrom = this->getRegimeNb(static_cast<int>(j), statusFrom);
            TYRegime& regimeTo = _pEnveloppe->getRegimeNb(static_cast<int>(j), statusTo);

            if (!statusFrom || !statusTo) { break; }

            regimeTo.deepCopy(&regimeFrom, false);
        }

        _pEnveloppe->setCurRegime(this->getCurRegime());
        _pEnveloppe->loadRegime(-1); // On met l'etat de l'enveloppe par rapport au regime courant
    }

    _volEnglob = volEnglob();
    calculRayonSphere(_volEnglob);
    calculCentreGravite();
    updateSurfEnveloppe();

    return 1;
}

void TYAcousticSemiCylinder::getChilds(LPTYElementArray& childs, bool recursif /*=true*/)
{
    childs.push_back(_pSemiCircTop);
    childs.push_back(_pSemiCircBottom);
    childs.push_back(_pAccRect);
    childs.push_back(_pEnveloppe);

    if (recursif)
    {
        _pSemiCircTop->getChilds(childs, recursif);
        _pSemiCircBottom->getChilds(childs, recursif);
        _pAccRect->getChilds(childs, recursif);
        _pEnveloppe->getChilds(childs, recursif);
    }

    TYAcousticVolume::getChilds(childs, recursif);
}

void TYAcousticSemiCylinder::setRegime(TYSpectre& Spectre, int regime /*=-1*/, bool recursif /*=false*/)
{
    if (recursif)
    {
        _pEnveloppe->setRegime(Spectre, regime, recursif);
        _pSemiCircTop->setRegime(Spectre, regime, recursif);
        _pSemiCircBottom->setRegime(Spectre, regime, recursif);
        _pAccRect->setRegime(Spectre, regime, recursif);
    }

    TYAcousticInterface::setRegime(Spectre, regime, recursif);
}

int TYAcousticSemiCylinder::addRegime()
{
    _pEnveloppe->addRegime();
    _pSemiCircTop->addRegime();
    _pSemiCircBottom->addRegime();
    _pAccRect->addRegime();

    return TYAcousticInterface::addRegime();
}

bool TYAcousticSemiCylinder::remRegime(int regime)
{
    _pEnveloppe->remRegime(regime);
    _pSemiCircTop->remRegime(regime);
    _pSemiCircBottom->remRegime(regime);
    _pAccRect->remRegime(regime);

    return TYAcousticInterface::remRegime(regime);
}

void TYAcousticSemiCylinder::loadRegime(int regimeNb)
{
    _pEnveloppe->loadRegime(regimeNb);
    _pSemiCircTop->loadRegime(regimeNb);
    _pSemiCircBottom->loadRegime(regimeNb);
    _pAccRect->loadRegime(regimeNb);

    TYAcousticInterface::loadRegime(regimeNb);
}

void TYAcousticSemiCylinder::setCurRegime(int regime)
{
    _pEnveloppe->setCurRegime(regime);
    _pSemiCircTop->setCurRegime(regime);
    _pSemiCircBottom->setCurRegime(regime);
    _pAccRect->setCurRegime(regime);

    TYAcousticInterface::setCurRegime(regime);

    setIsAcousticModified(true);
}

void TYAcousticSemiCylinder::propagateAtt(LPTYAttenuateur pAtt)
{
    _pEnveloppe->propagateAtt(pAtt);
    _pSemiCircTop->propagateAtt(pAtt);
    _pSemiCircBottom->propagateAtt(pAtt);
    _pAccRect->propagateAtt(pAtt);

    TYAcousticInterface::propagateAtt(pAtt);
}

TYTabSourcePonctuelleGeoNode TYAcousticSemiCylinder::getSrcs() const
{
    TYTabSourcePonctuelleGeoNode tab, tabTmp;

    tabTmp = _pSemiCircTop->getSrcs();
    tab.insert(tab.end(), tabTmp.begin(), tabTmp.end());

    tabTmp = _pSemiCircBottom->getSrcs();
    tab.insert(tab.end(), tabTmp.begin(), tabTmp.end());

    tabTmp = _pAccRect->getSrcs();
    tab.insert(tab.end(), tabTmp.begin(), tabTmp.end());

    tabTmp = _pEnveloppe->getSrcs();
    tab.insert(tab.end(), tabTmp.begin(), tabTmp.end());

    return tab;
}

TYSourcePonctuelle TYAcousticSemiCylinder::srcPonctEquiv() const
{
    return TYSourcePonctuelle();
}

void TYAcousticSemiCylinder::setDensiteSrcsH(double densite, bool recursif /*=true*/)
{
    TYAcousticVolume::setDensiteSrcsH(densite, recursif);

    if (recursif)
    {
        // Demi cercle bottom
        _pSemiCircBottom->setDensiteSrcsH(getDensiteSrcsH(), recursif);

        // Demi cercle top
        _pSemiCircTop->setDensiteSrcsH(getDensiteSrcsH(), recursif);

        // Face laterale
        _pAccRect->setDensiteSrcsH(getDensiteSrcsH(), recursif);

        // Enveloppe (src surf)
        _pEnveloppe->setDensiteSrcsH(getDensiteSrcsH());
    }
}

void TYAcousticSemiCylinder::setDensiteSrcsV(double densite, bool recursif /*=true*/)
{
    TYAcousticVolume::setDensiteSrcsV(densite, recursif);

    if (recursif)
    {
        // Demi cercle bottom
        _pSemiCircBottom->setDensiteSrcsV(getDensiteSrcsV(), recursif);

        // Demi cercle top
        _pSemiCircTop->setDensiteSrcsV(getDensiteSrcsV(), recursif);

        // Face laterale
        _pAccRect->setDensiteSrcsV(getDensiteSrcsV(), recursif);

        // Enveloppe (src surf)
        _pEnveloppe->setDensiteSrcsV(getDensiteSrcsV());
    }
}

void TYAcousticSemiCylinder::remAllSrcs()
{
    _pEnveloppe->remAllSrcs();
    _pSemiCircBottom->remAllSrcs();
    _pSemiCircTop->remAllSrcs();
    _pAccRect->remAllSrcs();
}

void TYAcousticSemiCylinder::distriSrcs()
{
    if (_isRayonnant == false)
    {
        // Pas de calcul
        return;
    }

    // Demi cercle bottom
    if (_pSemiCircBottom->getIsRayonnant()) { _pSemiCircBottom->distriSrcs(); }

    // Demi cercle top
    if (_pSemiCircTop->getIsRayonnant()) { _pSemiCircTop->distriSrcs(); }

    // Face laterale
    if (_pAccRect->getIsRayonnant()) { _pAccRect->distriSrcs(); }

    // Enveloppe (src surf)
    if (!_pEnveloppe->getIsRayonnant() || (_pEnveloppe->getDensiteSrcsH() == 0) || (_pEnveloppe->getDensiteSrcsV() == 0))
    {
        return;
    }

    // On travaille directement sur la source surfacique de l'enveloppe
    LPTYSourceSurfacic pSrcSurf = _pEnveloppe->getSrcSurf();

    // Distance angulaire (sur la circonference de chaque cercle
    // concentrique)

    const double distAngle = pSrcSurf->getDensiteSrcsH();
    int nbSrcs = ROUND(M_PI * getDiameter() / (2 * distAngle)) ; // (pi * r /Dist
    nbSrcs = (nbSrcs == 0) ? 1 : nbSrcs;  // Minimum une source

    const double anglePortion = M_PI / (nbSrcs + 1);    // Distance angulaire entre les sources
    double distHauteur = pSrcSurf->getDensiteSrcsV();

    const double hauteurCyl = getHauteur();

    int nbCercle = (int)(hauteurCyl / distHauteur);
    if (nbCercle == 0)
    {
        nbCercle = 1;
        distHauteur = hauteurCyl / 2;
    }
    // Si la hauteur residuelle est inferieure a 1/2 distance entre les cercles, on retire un cercle
    const double HResiduel = (hauteurCyl - (distHauteur * nbCercle)) / 2.0;
    if ((nbCercle > 1) && ((HResiduel < (distHauteur / 2.0)))) { nbCercle--; }

    // Position du premier cercle de sources en partant du bas
    double base = (hauteurCyl - (nbCercle - 1) * distHauteur) / 2.0;

    OPoint3D pt;
    double angle = 0.0;
    double hauteur = base;
    double rayon = (getDiameter() / 2.0) + _offsetSources; // Decalage des sources pour le lancer de rayons
    const double demiPi = M_PI / 2.0;
    const double hauteurOffset = _pSemiCircBottom->getCenter()._z;

    if (anglePortion > 0.0)
    {
        // Parcours sur la hauteur
        for (int i = 0 ; i < nbCercle ; i++)
        {
            // Init angle
            angle = anglePortion - demiPi;

            // Pour chaque portion d'angle
            for (int j = 0 ; j < nbSrcs ; j++)
            {
                // Point correspondant
                pt._x = cos(angle) * rayon;
                pt._y = sin(angle) * rayon;
                pt._z = hauteurOffset + hauteur;

                // Creation d'une source ponctuelle
                TYSourcePonctuelle* pSrc = new TYSourcePonctuelle();

                // Definition de sa position
                *pSrc->getPos() = pt;

                // Definition des sources comme sources a directivite calculee
                pSrc->setTypeRaynt(CALCULATED);

                // Ajout de la src ponct a la src surf
                pSrcSurf->addSrc(pSrc);

                // Increment de la portion (sens horaire)
                angle += anglePortion;
            }

            // Increment de la hauteur
            hauteur += distHauteur;
        }
    }

    //  _pSrcSurf->setParent(this); // DT-- 20060505 Desactive pour le moment// Permet de corriger un bug (perte de parent...) constate
}

bool TYAcousticSemiCylinder::setSrcsLw()
{
    if (_pEnveloppe->getIsRayonnant()) { _pEnveloppe->setSrcsLw(); }
    if (_pSemiCircBottom->getIsRayonnant()) { _pSemiCircBottom->setSrcsLw(); }
    if (_pSemiCircTop->getIsRayonnant()) { _pSemiCircTop->setSrcsLw(); }
    if (_pAccRect->getIsRayonnant()) { _pAccRect->setSrcsLw(); }

    return true;
}

double TYAcousticSemiCylinder::volume() const
{
    // (Pi.Ri申.h) / 2
    double rayon = getDiameter() / 2.0;
    return ((M_PI * rayon * rayon * getHauteur()) / 2.0);
}

double TYAcousticSemiCylinder::surface() const
{
    // Somme des surf
    return ((2 * M_PI * (getDiameter() / 2.0) * getHauteur()) / 2.0) +   // Aire laterale / 2
           _pSemiCircBottom->surface() +
           _pSemiCircTop->surface() +
           _pAccRect->surface();
}

double TYAcousticSemiCylinder::activeSurface() const
{
    double res = 0.0;

    if (_pEnveloppe->getIsRayonnant()) { res += _pEnveloppe->surface(); }
    if (_pSemiCircBottom->getIsRayonnant()) { res = res + _pSemiCircBottom->surface(); }
    if (_pSemiCircTop->getIsRayonnant()) { res = res + _pSemiCircTop->surface(); }
    if (_pAccRect->getIsRayonnant()) { res = res + _pAccRect->surface(); }

    return res;
}


double TYAcousticSemiCylinder::activeSurface()
{
    double res = 0.0;

    if (_pSemiCircBottom->getIsRayonnant()) { res += _pSemiCircBottom->surface(); }
    if (_pSemiCircTop->getIsRayonnant()) { res += _pSemiCircTop->surface(); }
    if (_pAccRect->getIsRayonnant()) { res += _pAccRect->surface(); }
    if (_pEnveloppe->getIsRayonnant()) { res += _pEnveloppe->surface(); }

    return res;
}

TYTabVector TYAcousticSemiCylinder::normals() const
{
    TYTabVector tab;

    tab.push_back(_pSemiCircBottom->getShape()->normal());
    tab.push_back(_pSemiCircTop->getShape()->normal());

    TYTabRectangle tabRect = getEnveloppe();
    for (unsigned int i = 0; i < tabRect.size(); i++)
    {
        tab.push_back(tabRect[i].normal());
    }

    return tab;
}

TYTabPoint TYAcousticSemiCylinder::sommets() const
{
    TYTabPoint tab;

    TYTabRectangle tabRect = getEnveloppe();
    tab.reserve(tabRect.size() * 4);

    // Chaque rectangle
    for (unsigned int i = 0; i < tabRect.size(); i++)
    {
        // Chaque point du rectangle
        for (int j = 0; j < 4; j++)
        {
            // Ajout du sommet au tableau
            tab.push_back(tabRect[i]._pts[j]);
        }
    }

    return tab;
}

TYTabLPPolygon TYAcousticSemiCylinder::faces() const
{
    TYTabLPPolygon tab;

    TYTabRectangle enveloppe = getEnveloppe();
    for (unsigned int i = 0; i < enveloppe.size(); i++)
    {
        tab.push_back(enveloppe[i].toPolygon());
    }

    tab.push_back(_pSemiCircBottom->getBoundingRect()->toPolygon());
    tab.push_back(_pSemiCircTop->getBoundingRect()->toPolygon());

    return tab;
}

TYBox TYAcousticSemiCylinder::volEnglob() const
{
    return TYVolumeInterface::volEnglob();
}

TYPoint TYAcousticSemiCylinder::centreGravite() const
{
    return getCenter();
}

int TYAcousticSemiCylinder::intersects(const OSegment3D& seg, TYTabPoint& ptList) const
{
    return TYVolumeInterface::intersects(seg, ptList);
}

int TYAcousticSemiCylinder::isInside(const TYPoint& pt) const
{
    int res = INTERS_NULLE;

    // Test de la position du point par rapport au vol englob
    if (_volEnglob.isInside(pt) == INTERS_NULLE)
    {
        return res;
    }

    // Le point doit verifier Xi申+Yi申 < Ri申 pour Z a l'interieur
    // du cylindre (Le cylindre ne doit pas etre un ci申ne...)
    // Le test avec l'equation du cercle complet est correcte ici
    // car on a deja teste si le point appartenait a la bonne moitie
    // du cylindre avec le test de la boi申te englobante.
    // La verification de l'appartenance au cone infini est aussi
    // suffisante car on a deja teste si le point est compris
    // entre les plan haut et bas du cylindre (boi申te englobante)

    // Obtention de la matrice de changement de repere
    ORepere3D repere = _pSemiCircBottom->getShape()->getORepere3D();
    repere._origin = getCenter();

    // On passe dans le repere du cylindre pour travailler en 2D
    OPoint3D ptBis = repere.asMatrix() * pt;

    // Test Xi申 + Yi申 <= Ri申
    double rayon = getDiameter() / 2.0;
    if ((pt._x * ptBis._x) + (pt._y * ptBis._y) <= (rayon * rayon))
    {
        // Le point est a l'interieur du cylindre
        res = INTERS_OUI;
    }

    return res;
}

TYTabAcousticSurfaceGeoNode TYAcousticSemiCylinder::acousticFaces()
{
    TYTabAcousticSurfaceGeoNode tab;

    // Demi cercle top
    tab.push_back(new TYAcousticSurfaceGeoNode((LPTYElement&) _pSemiCircTop));
    // Demi cercle bottom
    tab.push_back(new TYAcousticSurfaceGeoNode((LPTYElement&) _pSemiCircBottom));
    // Face laterale
    tab.push_back(new TYAcousticSurfaceGeoNode((LPTYElement&) _pAccRect));

    // Enveloppe
    // Resolution pour l'enveloppe
    int resolution = TYDEFAULTRESOLUTIONIONCIRCLE;

#if TY_USE_IHM
    //  static const char prefName[] = "ResolutionCircle";

    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "ResolutionCircle"))
    {
        resolution = TYPreferenceManager::getInt(TYDIRPREFERENCEMANAGER, "ResolutionCircle");
    }
    else
    {
        TYPreferenceManager::setInt(TYDIRPREFERENCEMANAGER, "ResolutionCircle", resolution);
    }

#endif // TY_USE_IHM

    TYTabRectangle enveloppe = getEnveloppe(resolution);
    TYAcousticRectangle* pEnvElt = NULL;

    for (unsigned int i = 0; i < enveloppe.size() - 1; i++)
    {
        // Element de l'enveloppe
        pEnvElt = new TYAcousticRectangle();
        pEnvElt->setParent(this);
        pEnvElt->setInCurrentCalcul(isInCurrentCalcul());

        // Geometrie
        *pEnvElt->getShape() = enveloppe[i];

        // Acoustique
        pEnvElt->setSrcSurf(_pEnveloppe->getSrcSurf());

        // Ajout
        tab.push_back(new TYAcousticSurfaceGeoNode(pEnvElt));
    }

    return tab;
}

bool TYAcousticSemiCylinder::findAcousticSurface(const TYAcousticSurface* pAccSurf, OMatrix* pMatrix /*=0*/)
{
    bool ret = false;

    if (((TYAcousticSurface*) _pSemiCircBottom._pObj == pAccSurf) ||
        ((TYAcousticSurface*) _pSemiCircTop._pObj == pAccSurf) ||
        ((TYAcousticSurface*) _pAccRect._pObj == pAccSurf))
    {
        // Pas de changement de repere pour ces surfaces
        ret = true;
    }

    return ret;
}

double TYAcousticSemiCylinder::getDiameter() const
{
    return _pSemiCircBottom->getDiameter();
}

void TYAcousticSemiCylinder::setDiameter(double diameter)
{
    _pSemiCircBottom->setDiameter(diameter);
    _pSemiCircTop->setDiameter(diameter);

    updateRect();

    _volEnglob = volEnglob();
    if (_pParent) { TYAcousticVolumeNode::safeDownCast(_pParent)->updateVolEnglob(); } // Informe le parent de se mettre a jour

    calculRayonSphere(_volEnglob);
    calculCentreGravite();
    // Recalcul de la surface de l'enveloppe affectee au rectangle associe a l'acousticsurface
    updateSurfEnveloppe();

}

double TYAcousticSemiCylinder::getHauteur() const
{
    return OVector3D(_pSemiCircTop->getCenter(), _pSemiCircBottom->getCenter()).norme();
}

void TYAcousticSemiCylinder::setHauteur(double hauteur)
{
    // On attribue +hauteur/2 au cercle du dessus
    // et -hauteur/2 au cercle du dessous
    // On calcul en fait des facteurs en fonction de la taille actuelle
    // Les vecteurs obtenus font abstraction de la position du cylindre

    // Demi cercle top
    OVector3D vecTop(getCenter(), _pSemiCircTop->getCenter());
    vecTop = vecTop * (((hauteur / 2.0) / vecTop.norme()) - 1.0);

    // Demi cercle bottom
    OVector3D vecBottom(getCenter(), _pSemiCircBottom->getCenter());
    vecBottom = vecBottom * (((hauteur / 2.0) / vecBottom.norme()) - 1.0);

    // Pour chaque point
    for (int i = 0; i < 4; i ++)
    {
        // On applique le vecteur facteur
        _pSemiCircTop->getShape()->_pts[i] = OVector3D(_pSemiCircTop->getShape()->_pts[i]) + vecTop;
        _pSemiCircBottom->getShape()->_pts[i] = OVector3D(_pSemiCircBottom->getShape()->_pts[i]) + vecBottom;
    }

    // Mise a jour de la surface laterale
    updateRect();

    _volEnglob = volEnglob();
    if (_pParent) { TYAcousticVolumeNode::safeDownCast(_pParent)->updateVolEnglob(); } // Informe le parent de se mettre a jour

    calculRayonSphere(_volEnglob);
    calculCentreGravite();

    // Recalcul de la surface de l'enveloppe affectee au rectangle associe a l'acousticsurface
    updateSurfEnveloppe();
}

void TYAcousticSemiCylinder::updateRect()
{
    // Les coordonnees de la face laterale sont calculees a partir de
    // celle des demi-cercles top et bottom
    _pAccRect->getShape()->_pts[0] = _pSemiCircTop->getShape()->_pts[0];
    _pAccRect->getShape()->_pts[1] = _pSemiCircTop->getShape()->_pts[3];
    _pAccRect->getShape()->_pts[2] = _pSemiCircBottom->getShape()->_pts[0];
    _pAccRect->getShape()->_pts[3] = _pSemiCircBottom->getShape()->_pts[3];

    _pAccRect->setIsGeometryModified(true);
    setIsGeometryModified(true);
}

TYTabRectangle TYAcousticSemiCylinder::getEnveloppe(int n /*=-1*/) const
{
    TYTabRectangle tab;

    if (n == -1)
    {
        // Prise en compte de la precision
        n = ROUND(((getDiameter() / 2.0) / TYPRECISIONCIRCLE) / 2.0);

        // On veut au moins tous les Pi/2 (90i申)
        n = ROUND(n / 2 * 2);
    }

    if (n < 3)
    {
        return tab;
    }

    // Obtention des contours des demi-cercles top et bottom
    // (Le nombre de points generes doit etre le meme)
    TYTabPoint tabPtBottom = _pSemiCircBottom->getContour(n);
    TYTabPoint tabPtTop = _pSemiCircTop->getContour(n);

    // n faces + la face laterale
    int nbFaces = n + 1;
    // On genere une face pour chaque couple de points top bottom
    // + une face pour la face laterale (celle-ci sera redondante par
    // rapport a '_rect')
    for (int i = 0; i < nbFaces; i ++)
    {
        TYRectangle rect(tabPtTop[(i + 1) % nbFaces], tabPtTop[i], tabPtBottom[nbFaces - i - 1], tabPtBottom[(nbFaces - 1) - ((i + 1) % nbFaces)]);
        tab.push_back(rect);
    }

    return tab;
}

TYPoint TYAcousticSemiCylinder::getCenter() const
{
    OVector3D vec(_pSemiCircBottom->getCenter(), _pSemiCircTop->getCenter());
    return (OVector3D(_pSemiCircBottom->getCenter()) + (vec * 0.5));
}

bool TYAcousticSemiCylinder::updateAcoustic(const bool& force) // force = false
{
    /*
       NB : On travaille sur un regime donne.
    */

    bool ret = true;

    // Si le volume n'est pas rayonnant, inutile de tenter de distribuer
    // la puissance, mais ce n'est pas une raison pour bloquer le calcul
    if (!_isRayonnant) { return true ; }

    // 1/ Recuperer la surface totale
    double surfTotale = this->activeSurface();

    OSpectre LWc = OSpectre::getEmptyLinSpectre(); // Spectre cumule de l'ensemble des elements
    OSpectre LWg = getCurrentSpectre().toGPhy(); // Spectre global du regime courant
    OSpectre LWv; //Spectre de puissance de la face courante

    LWc.setType(SPECTRE_TYPE_LW); // LWc represente une puissance

    if (_pSemiCircTop->getIsRayonnant())
    {
        LWv = _pSemiCircTop->setGlobalLW(LWg, surfTotale);
        LWc = LWc.sum(LWv);
    }
    else
    {
        TYSpectre aTYSpectre;
        _pSemiCircTop->setRegime(aTYSpectre);
    }

    if (_pSemiCircBottom->getIsRayonnant())
    {
        LWv = _pSemiCircBottom->setGlobalLW(LWg, surfTotale);
        LWc = LWc.sum(LWv);
    }
    else
    {
        TYSpectre aTYSpectre;
        _pSemiCircBottom->setRegime(aTYSpectre);
    }

    if (_pAccRect->getIsRayonnant())
    {
        LWv = _pAccRect->setGlobalLW(LWg, surfTotale);
        LWc = LWc.sum(LWv);
    }
    else
    {
        TYSpectre aTYSpectre;
        _pAccRect->setRegime(aTYSpectre);
    }

    if (_pEnveloppe->getIsRayonnant())
    {
        LWv = _pEnveloppe->setGlobalLW(LWg, surfTotale);
        LWc = LWc.sum(LWv);
    }
    else
    {
        TYSpectre aTYSpectre;
        _pEnveloppe->setRegime(aTYSpectre);
    }

    if ((_typeDistribution == TY_PUISSANCE_IMPOSEE))
    {
        // Il faut s'assurer que la somme des puissances des volumes egale la puissance de la machine
        double residu = ABS(10 * log10(LWg.sigma()) - 10 * log10(LWc.sigma()));

        // Si l'ecart est inferieur a 1 dB on accepte
        ret = residu < 1 ? true : false;
    }
    else if (_typeDistribution == TY_PUISSANCE_CALCULEE)
    {

        LWc = OSpectre::getEmptyLinSpectre();
        LWc.setType(SPECTRE_TYPE_LW);

        if (_pSemiCircTop->getIsRayonnant()) { LWc = LWc.sum(_pSemiCircTop->getCurrentSpectre().toGPhy()); }
        if (_pSemiCircBottom->getIsRayonnant()) { LWc = LWc.sum(_pSemiCircBottom->getCurrentSpectre().toGPhy()); }
        if (_pAccRect->getIsRayonnant()) { LWc = LWc.sum(_pAccRect->getCurrentSpectre().toGPhy()); }
        if (_pEnveloppe->getIsRayonnant()) { LWc = LWc.sum(_pEnveloppe->getCurrentSpectre().toGPhy()); }

        TYSpectre temp = LWc.toDB();
        setRegime(temp, -1, false); // Pas de problemes, la puissance peut etre modifiee
        ret = true;
    }

    return ret;
}

TYTabLPAcousticSurface TYAcousticSemiCylinder::getSubFace()
{
    TYTabLPAcousticSurface tabSubFaces;

    // On balaye toutes les faces
    for (unsigned int i = 0; i < 6; i++)
    {
        TYTabAcousticSurfaceGeoNode tabFaces = acousticFaces();

        for (unsigned int j = 0; j < tabFaces.size(); j++)
        {
            LPTYAcousticSurface pSurface = TYAcousticSurface::safeDownCast(tabFaces[j]->getElement());

            if (pSurface->getIsSub())
            {
                tabSubFaces.push_back(pSurface);
            }
        }

    }

    return tabSubFaces;
}

void TYAcousticSemiCylinder::setIsRayonnant(bool rayonnant /*= true*/, bool recursif /*=true*/)
{
    TYAcousticVolume::setIsRayonnant(rayonnant, recursif);
}

void TYAcousticSemiCylinder::setNextRegimeNb(const int& next)
{
    _nextRegime = next;

    _pSemiCircTop->setNextRegimeNb(next);
    _pSemiCircBottom->setNextRegimeNb(next);
    _pAccRect->setNextRegimeNb(next);
    _pEnveloppe->setNextRegimeNb(next);

}

void TYAcousticSemiCylinder::setRegimeName(const QString& name)
{
    TYAcousticInterface::setRegimeName(name);

    _pSemiCircTop->setRegimeName(name);
    _pSemiCircBottom->setRegimeName(name);
    _pAccRect->setRegimeName(name);
    _pEnveloppe->setRegimeName(name);
}

void TYAcousticSemiCylinder::updateSurfEnveloppe()
{
    double sizeRectangle = sqrt(getDiameter() * M_PI * getHauteur() / 2.0); // = pi.r.h
    _pEnveloppe->getBoundingRect()->setDimension(sizeRectangle, sizeRectangle);
}

void TYAcousticSemiCylinder::exportCSV(std::ofstream& ofs)
{
    // Export du nom de l'objet
    ofs << getName().toAscii().data() << '\n';

    // Export du type de l'objet
    ofs << toString() << '\n';

    // Export des donnees acoustiques
    TYAcousticInterface::exportCSV(ofs);

    _pSemiCircTop->exportCSV(ofs);
    _pSemiCircBottom->exportCSV(ofs);
    _pAccRect->exportCSV(ofs);
    _pEnveloppe->exportCSV(ofs);
}
