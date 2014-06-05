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
#include "Tympan/GraphicIHM/DataManagerIHM/TYAcousticCylinderWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYAcousticCylinderGraphic.h"
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/Tools/OMessageManager.h"
#include "Tympan/Tools/TYProgressManager.h"

TY_EXTENSION_INST(TYAcousticCylinder);
TY_EXT_GRAPHIC_INST(TYAcousticCylinder);

TYAcousticCylinder::TYAcousticCylinder()
{
    _name = TYNameManager::get()->generateName(getClassName());

    // Couleur par default
    float r = 255.0f, g = 67.0f, b = 133.0f;

#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "TYAcousticCylinderGraphicColorR"))
    {
        TYPreferenceManager::getColor(TYDIRPREFERENCEMANAGER, "TYAcousticCylinderGraphicColor", r, g, b);
    }
    else
    {
        TYPreferenceManager::setColor(TYDIRPREFERENCEMANAGER, "TYAcousticCylinderGraphicColor", r, g, b);
    }

#endif // TY_USE_IHM

    OColor color;
    color.r = r / 255;
    color.g = g / 255;
    color.b = b / 255;

    setColor(color);

    _pEnveloppe = new TYAcousticSurface();
    _pEnveloppe->setParent(this);

    _pCircTop = new TYAcousticCircle();
    *_pCircTop->getShape() = TYRectangle(TYPoint(-1, 1, 0.5), TYPoint(1, 1, 0.5), TYPoint(1, -1, 0.5), TYPoint(-1, -1, 0.5));
    _pCircTop->setParent(this);

    _pCircBottom = new TYAcousticCircle();
    *_pCircBottom->getShape() = TYRectangle(TYPoint(-1, -1, -0.5), TYPoint(1, -1, -0.5), TYPoint(1, 1, -0.5), TYPoint(-1, 1, -0.5));
    _pCircBottom->setParent(this);

    _volEnglob = volEnglob();
    calculRayonSphere(_volEnglob);
    calculCentreGravite();
    updateSurfEnveloppe();

}

TYAcousticCylinder::TYAcousticCylinder(const TYAcousticCylinder& other)
{
    *this = other;
}

TYAcousticCylinder::~TYAcousticCylinder()
{
}

TYAcousticCylinder& TYAcousticCylinder::operator=(const TYAcousticCylinder& other)
{
    if (this != &other)
    {
        TYAcousticVolume::operator =(other);
        _pEnveloppe = other._pEnveloppe;
        _pCircTop = other._pCircTop;
        _pCircBottom = other._pCircBottom;
    }

    _volEnglob = volEnglob();
    calculRayonSphere(_volEnglob);
    calculCentreGravite();
    updateSurfEnveloppe();

    return *this;
}

bool TYAcousticCylinder::operator==(const TYAcousticCylinder& other) const
{
    if (this != &other)
    {
        if (TYAcousticVolume::operator !=(other)) { return false; }
        if (_pEnveloppe != other._pEnveloppe) { return false; }
        if (_pCircTop != other._pCircTop) { return false; }
        if (_pCircBottom != other._pCircBottom) { return false; }
    }
    return true;
}

bool TYAcousticCylinder::operator!=(const TYAcousticCylinder& other) const
{
    return !operator==(other);
}

bool TYAcousticCylinder::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYAcousticVolume::deepCopy(pOther, copyId)) { return false; }

    TYAcousticCylinder* pOtherCyl = (TYAcousticCylinder*) pOther;

    _pEnveloppe->deepCopy(pOtherCyl->_pEnveloppe, copyId);
    _pCircTop->deepCopy(pOtherCyl->_pCircTop, copyId);
    _pCircBottom->deepCopy(pOtherCyl->_pCircBottom, copyId);

    _volEnglob = volEnglob();
    calculRayonSphere(_volEnglob);
    calculCentreGravite();
    updateSurfEnveloppe();

    return true;
}

std::string TYAcousticCylinder::toString() const
{
    return "TYAcousticCylinder";
}

DOM_Element TYAcousticCylinder::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYAcousticVolume::toXML(domElement);

    _pEnveloppe->toXML(domNewElem);
    _pCircTop->toXML(domNewElem);
    _pCircBottom->toXML(domNewElem);

    return domNewElem;
}

int TYAcousticCylinder::fromXML(DOM_Element domElement)
{
    TYAcousticVolume::fromXML(domElement);

    bool circTopFound = false;
    bool circBottomFound = false;
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

        if (!circTopFound)
        {
            circTopFound = _pCircTop->callFromXMLIfEqual(elemCur);
        }
        else if (!circBottomFound)
        {
            circBottomFound = _pCircBottom->callFromXMLIfEqual(elemCur);
        }
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

void TYAcousticCylinder::getChilds(LPTYElementArray& childs, bool recursif /*=true*/)
{
    TYAcousticVolume::getChilds(childs, recursif);

    childs.push_back(_pCircTop);
    childs.push_back(_pCircBottom);
    childs.push_back(_pEnveloppe);

    if (recursif)
    {
        _pCircTop->getChilds(childs, recursif);
        _pCircBottom->getChilds(childs, recursif);
        _pEnveloppe->getChilds(childs, recursif);
    }

}

void TYAcousticCylinder::setRegime(TYSpectre& Spectre, int regime /*=-1*/, bool recursif /*=false*/)
{
    if (recursif)
    {
        _pEnveloppe->setRegime(Spectre, regime, recursif);
        _pCircTop->setRegime(Spectre, regime, recursif);
        _pCircBottom->setRegime(Spectre, regime, recursif);
    }

    TYAcousticInterface::setRegime(Spectre, regime, recursif);
}

bool TYAcousticCylinder::remRegime(int regime)
{
    _pEnveloppe->remRegime(regime);
    _pCircTop->remRegime(regime);
    _pCircBottom->remRegime(regime);

    return TYAcousticVolume::remRegime(regime);
}

void TYAcousticCylinder::loadRegime(int regimeNb)
{
    _pEnveloppe->loadRegime(regimeNb);
    _pCircTop->loadRegime(regimeNb);
    _pCircBottom->loadRegime(regimeNb);

    TYAcousticVolume::loadRegime(regimeNb);
}

void TYAcousticCylinder::setCurRegime(int regime)
{
    _pEnveloppe->setCurRegime(regime);
    _pCircTop->setCurRegime(regime);
    _pCircBottom->setCurRegime(regime);

    TYAcousticInterface::setCurRegime(regime);

    setIsAcousticModified(true);
}

void TYAcousticCylinder::propagateAtt(LPTYAttenuateur pAtt)
{
    _pEnveloppe->propagateAtt(pAtt);
    _pCircTop->propagateAtt(pAtt);
    _pCircBottom->propagateAtt(pAtt);

    TYAcousticVolume::propagateAtt(pAtt);
}
/*
int TYAcousticCylinder::addRegime(const TYRegime& regime)
{
    _pEnveloppe->addRegime();
    _pCircTop->addRegime();
    _pCircBottom->addRegime();

    return TYAcousticInterface::addRegime(regime);
}
*/
int TYAcousticCylinder::addRegime()
{
    _pEnveloppe->addRegime();
    _pCircTop->addRegime();
    _pCircBottom->addRegime();

    return TYAcousticInterface::addRegime();
}


TYTabSourcePonctuelleGeoNode TYAcousticCylinder::getSrcs() const
{
    TYTabSourcePonctuelleGeoNode tab, tabTmp;

    tabTmp = _pCircBottom->getSrcs();
    tab.insert(tab.end(), tabTmp.begin(), tabTmp.end());

    tabTmp = _pCircTop->getSrcs();
    tab.insert(tab.end(), tabTmp.begin(), tabTmp.end());

    tabTmp = _pEnveloppe->getSrcs();
    tab.insert(tab.end(), tabTmp.begin(), tabTmp.end());

    return tab;
}

TYSourcePonctuelle TYAcousticCylinder::srcPonctEquiv() const
{
    return TYSourcePonctuelle();
}

void TYAcousticCylinder::setDensiteSrcsH(double densite, bool recursif /*= true*/)
{
    TYAcousticVolume::setDensiteSrcsH(densite, recursif);

    if (recursif)
    {
        // Cercle bottom
        _pCircBottom->setDensiteSrcsH(getDensiteSrcsH(), recursif);

        // Cercle top
        _pCircTop->setDensiteSrcsH(getDensiteSrcsH(), recursif);

        // Enveloppe (src surf)
        _pEnveloppe->setDensiteSrcsH(getDensiteSrcsH());
    }
}

void TYAcousticCylinder::setDensiteSrcsV(double densite, bool recursif /*=true*/)
{
    TYAcousticVolume::setDensiteSrcsV(densite, recursif);

    if (recursif)
    {
        // Cercle bottom
        _pCircBottom->setDensiteSrcsV(getDensiteSrcsV(), recursif);

        // Cercle top
        _pCircTop->setDensiteSrcsV(getDensiteSrcsV(), recursif);

        // Enveloppe (src surf)
        _pEnveloppe->setDensiteSrcsV(getDensiteSrcsV());
    }
}

void TYAcousticCylinder::remAllSrcs()
{
    // Suppression des sources existantes
    _pEnveloppe->remAllSrcs();
    _pCircBottom->remAllSrcs();
    _pCircTop->remAllSrcs();
}

void TYAcousticCylinder::distriSrcs()
{
    if (_isRayonnant == false)
    {
        // Pas de calcul
        return;
    }

    // Cercle bottom
    if (_pCircBottom->getIsRayonnant()) { _pCircBottom->distriSrcs(); }

    // Cercle top
    if (_pCircTop->getIsRayonnant()) { _pCircTop->distriSrcs(); }


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
    int nbSrcs = ROUND(M_PI * getDiameter() / distAngle);
    nbSrcs = nbSrcs < 4 ? 4 : nbSrcs;  // Au moins 4 sources sur la circonference (champ rayonne homogene)
    const double anglePortion = M_2PI / nbSrcs;

    // Distance sur la hauteur
    double distHauteur = pSrcSurf->getDensiteSrcsV();

    const double hauteurCyl = getHauteur();

    int nbCercle = (int)(hauteurCyl / distHauteur);
    if (nbCercle == 0)
    {
        nbCercle = 1;
        distHauteur = hauteurCyl / 2.0;
    }

    // Si la hauteur residuelle est inferieure a la demi distance entre les cercles, on retire un cercle
    const double HResiduel = (hauteurCyl - (distHauteur * nbCercle)) / 2.0;
    if ((nbCercle > 1) && (HResiduel < (distHauteur / 2.0))) { nbCercle--; }

    // Position du premier cercle en partant du bas
    double base = (hauteurCyl - (nbCercle - 1) * distHauteur) / 2.0;

    // nombre d'iterations ((M_2PI / distAngle) + (hauteurCyl / distHauteur));

    OCoord3D pt;
    double angle = 0.0;
    double hauteur = base;
    double rayon = (getDiameter() / 2.0) + _offsetSources; // decalage des sources pour le lancer de rayons
    const double hauteurOffset = _pCircBottom->getCenter()._z;

    // Calcul de la portion d'angle pour le cercle
    if (anglePortion > 0.0)
    {
        // Parcours sur la hauteur
        for (int i = 0; i < nbCercle ; i++)
        {
            // Init angle
            angle = 0.0;

            // Pour chaque portion d'angle
            while ((angle < M_2PI))
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

                // Attribution du parent
                //              pSrc->setParent(_pEnveloppe);

                // Increment de la portion (sens horaire)
                angle += anglePortion;
            }

            // Increment de la hauteur
            hauteur += distHauteur;
        }
    }
}

bool TYAcousticCylinder::setSrcsLw()
{
    if (_pCircBottom->getIsRayonnant()) { _pCircBottom->setSrcsLw(); }
    if (_pCircTop->getIsRayonnant()) { _pCircTop->setSrcsLw(); }
    if (_pEnveloppe->getIsRayonnant()) { _pEnveloppe->setSrcsLw(); }

    return true;
}

double TYAcousticCylinder::volume() const
{
    // Pi.Ri申.h
    double rayon = getDiameter() / 2.0;
    return (M_PI * rayon * rayon * getHauteur());
}

double TYAcousticCylinder::surface() const
{
    // 2.Pi.R.(R+h)
    double rayon = getDiameter() / 2.0;
    return (2 * M_PI * rayon * (rayon + getHauteur()));
}

double TYAcousticCylinder::activeSurface() const
{
    double res = 0.0;

    if (_pCircBottom->getIsRayonnant()) { res += _pCircBottom->surface(); }
    if (_pCircTop->getIsRayonnant()) { res += _pCircTop->surface(); }
    if (_pEnveloppe->getIsRayonnant()) { res += _pEnveloppe->surface(); }

    return res;
}

double TYAcousticCylinder::activeSurface()
{
    double res = 0.0;

    if (_pCircBottom->getIsRayonnant()) { res += _pCircBottom->surface(); }
    if (_pCircTop->getIsRayonnant()) { res += _pCircTop->surface(); }
    if (_pEnveloppe->getIsRayonnant()) { res += _pEnveloppe->surface(); }

    return res;
}

TYTabVector TYAcousticCylinder::normals() const
{
    TYTabVector tab;

    tab.push_back(_pCircBottom->getShape()->normal());
    tab.push_back(_pCircTop->getShape()->normal());

    TYTabRectangle tabRect = getEnveloppe();
    for (unsigned int i = 0; i < tabRect.size(); i++)
    {
        tab.push_back(tabRect[i].normal());
    }

    return tab;
}

TYTabPoint TYAcousticCylinder::sommets() const
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

TYTabLPPolygon TYAcousticCylinder::faces() const
{
    TYTabLPPolygon tab;

    TYTabRectangle enveloppe = getEnveloppe();
    for (unsigned int i = 0; i < enveloppe.size(); i++)
    {
        tab.push_back(enveloppe[i].toPolygon());
    }

    tab.push_back(_pCircBottom->getBoundingRect()->toPolygon());
    tab.push_back(_pCircTop->getBoundingRect()->toPolygon());

    return tab;
}

TYBox TYAcousticCylinder::volEnglob() const
{
    return TYVolumeInterface::volEnglob();
}

TYPoint TYAcousticCylinder::centreGravite() const
{
    return getCenter();
}

int TYAcousticCylinder::intersects(const OSegment3D& seg, TYTabPoint& ptList) const
{
    return TYVolumeInterface::intersects(seg, ptList);
}

int TYAcousticCylinder::isInside(const TYPoint& pt) const
{
    int res = INTERS_NULLE;

    // Test de la position du point par rapport au vol englob
    if (_volEnglob.isInside(pt) == INTERS_NULLE)
    {
        return res;
    }

    // Le point doit verifier Xi申+Yi申 < Ri申 pour Z a l'interieur
    // du cylindre (Le cylindre ne doit pas etre un ci申ne...)
    // La verification de l'appartenance au cone infini est
    // suffisante car on a deja teste si le point est compris
    // entre les plan haut et bas du cylindre (boi申te englobante)

    // Obtention de la matrice de changement de repere
    ORepere3D repere = _pCircBottom->getShape()->getORepere3D();
    repere._origin = getCenter();

    // On passe dans le repere du cylindre pour travailler en 2D
    OPoint3D ptCopy = repere.asMatrix() * pt;

    // Test Xi申 + Yi申 <= Ri申
    double rayon = getDiameter() / 2.0;
    if ((ptCopy._x * ptCopy._x) + (ptCopy._y * ptCopy._y) <= (rayon * rayon))
    {
        // Le point est a l'interieur du cylindre
        res = INTERS_OUI;
    }

    return res;
}

TYTabAcousticSurfaceGeoNode TYAcousticCylinder::acousticFaces()
{
    TYTabAcousticSurfaceGeoNode tab;

    // Circ top
    tab.push_back(new TYAcousticSurfaceGeoNode((LPTYElement&) _pCircTop));
    // Circ bottom
    tab.push_back(new TYAcousticSurfaceGeoNode((LPTYElement&) _pCircBottom));

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

    for (unsigned int i = 0; i < enveloppe.size(); i++)
    {
        // Element de l'enveloppe
        pEnvElt = new TYAcousticRectangle();
        pEnvElt->setParent(this);
        pEnvElt->setInCurrentCalcul(isInCurrentCalcul());

        // Geometrie
        *pEnvElt->getShape() = enveloppe[i];

        // Acoustique
        //      pEnvElt->setSrcSurf(_pEnveloppe->getSrcSurf());

        // Ajout
        tab.push_back(new TYAcousticSurfaceGeoNode(pEnvElt));
    }

    return tab;
}

bool TYAcousticCylinder::findAcousticSurface(const TYAcousticSurface* pAccSurf, OMatrix* pMatrix /*=0*/)
{
    bool ret = false;

    if (((TYAcousticSurface*) _pCircBottom._pObj == pAccSurf) || ((TYAcousticSurface*) _pCircTop._pObj == pAccSurf))
    {
        // Pas de changement de repere pour ces surfaces
        ret = true;
    }

    return ret;
}

double TYAcousticCylinder::getDiameter() const
{
    return _pCircBottom->getDiameter();
}

void TYAcousticCylinder::setDiameter(double diameter)
{
    _pCircBottom->setDiameter(diameter);
    _pCircTop->setDiameter(diameter);

    _volEnglob = volEnglob();
    if (_pParent) { TYAcousticVolumeNode::safeDownCast(_pParent)->updateVolEnglob(); } // Informe le parent de se mettre a jour

    calculRayonSphere(_volEnglob);

    calculCentreGravite();

    // Recalcul de la surface de l'enveloppe affectee au rectangle associe a l'acousticsurface
    updateSurfEnveloppe();

    setIsGeometryModified(true);
}

double TYAcousticCylinder::getHauteur() const
{
    return OVector3D(_pCircTop->getCenter(), _pCircBottom->getCenter()).norme();
}

void TYAcousticCylinder::setHauteur(double hauteur)
{
    // On attribue +hauteur/2 au cercle du dessus
    // et -hauteur/2 au cercle du dessous
    // On calcul en fait des facteurs en fonction de la taille actuelle
    // Les vecteurs obtenus font abstraction de la position du cylindre

    // Cercle top
    OVector3D vecTop(getCenter(), _pCircTop->getCenter());
    vecTop = vecTop * (((hauteur / 2.0) / vecTop.norme()) - 1.0);

    // Cercle bottom
    OVector3D vecBottom(getCenter(), _pCircBottom->getCenter());
    vecBottom = vecBottom * (((hauteur / 2.0) / vecBottom.norme()) - 1.0);

    // Pour chaque point
    for (int i = 0; i < 4; i ++)
    {
        // On applique le vecteur facteur
        _pCircTop->getShape()->_pts[i] = OVector3D(_pCircTop->getShape()->_pts[i]) + vecTop;
        _pCircBottom->getShape()->_pts[i] = OVector3D(_pCircBottom->getShape()->_pts[i]) + vecBottom;
    }

    _volEnglob = volEnglob();
    if (_pParent) { TYAcousticVolumeNode::safeDownCast(_pParent)->updateVolEnglob(); } // Informe le parent de se mettre a jour

    calculRayonSphere(_volEnglob);
    calculCentreGravite();

    // Recalcul de la surface de l'enveloppe affectee au rectangle associe a l'acousticsurface
    updateSurfEnveloppe();

    setIsGeometryModified(true);
}

TYTabRectangle TYAcousticCylinder::getEnveloppe(int n /*=-1*/) const
{
    TYTabRectangle tab;

    if (n == -1)
    {
        // Prise en compte de la precision
        n = ROUND((getDiameter() / 2.0) / TYPRECISIONCIRCLE);

        // On veut au moins tous les Pi/2 (90i申)
        n = ROUND(n / 4 * 4);
    }

    if (n < 3)
    {
        return tab;
    }

    // Obtention des contours des cercles top et bottom
    // (Le nombre de points generes doit etre le meme)
    TYTabPoint tabPtBottom = _pCircBottom->getContour(n);
    TYTabPoint tabPtTop = _pCircTop->getContour(n);

    // On genere une face pour chaque couple de points top bottom
    for (int i = 0; i < n; i ++)
    {
        TYRectangle rect(tabPtTop[(i + 1) % n], tabPtTop[i], tabPtBottom[(n - i) % n], tabPtBottom[n - i - 1]);
        tab.push_back(rect);
    }

    return tab;
}

TYPoint TYAcousticCylinder::getCenter() const
{
    // Le centre du cylindre est la mi-hauteur entre les centres des 2 cercles
    OVector3D vec(_pCircBottom->getCenter(), _pCircTop->getCenter());
    return (OVector3D(_pCircBottom->getCenter()) + (vec * 0.5));
}

bool TYAcousticCylinder::updateAcoustic(const bool& force) // force = false
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

    if (_pCircTop->getIsRayonnant())
    {
        LWv = _pCircTop->setGlobalLW(LWg, surfTotale);
        LWc = LWc.sum(LWv);
    }
    else
    {
        TYSpectre aTYSpectre;
        _pCircTop->setRegime(aTYSpectre);
    }

    if (_pCircBottom->getIsRayonnant())
    {
        LWv = _pCircBottom->setGlobalLW(LWg, surfTotale);
        LWc = LWc.sum(LWv);
    }
    else
    {
        TYSpectre aTYSpectre;
        _pCircBottom->setRegime(aTYSpectre);
    }

    if (_pEnveloppe->getIsRayonnant())
    {
        LWv = _pEnveloppe->setGlobalLW(LWg, surfTotale);
        LWc = LWc.sum(LWv);
    }
    else
    {
        TYSpectre aSpectre;
        _pEnveloppe->setRegime(aSpectre);
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

        if (_pCircTop->getIsRayonnant()) { LWc = LWc.sum(_pCircTop->getCurrentSpectre().toGPhy()); }
        if (_pCircBottom->getIsRayonnant()) { LWc = LWc.sum(_pCircBottom->getCurrentSpectre().toGPhy()); }
        if (_pEnveloppe->getIsRayonnant()) { LWc = LWc.sum(_pEnveloppe->getCurrentSpectre().toGPhy()); }

        TYSpectre temp = LWc.toDB();
        setRegime(temp, -1, false); // Pas de problemes, la puissance peut etre modifiee
        ret = true;
    }

    if (_pParent && _pParent->isA("TYBatiment"))
    {
        // Dans le cas des machine, cette operation est pilotee par TYAcousticVolumeNode
        distriSrcs();
        setSrcsLw();
    }

    return ret;
}

TYTabLPAcousticSurface TYAcousticCylinder::getSubFace()
{
    TYTabLPAcousticSurface tabSubFaces;

    // On balaye toutes les faces
    TYTabAcousticSurfaceGeoNode tabFaces = acousticFaces();
    for (unsigned int j = 0; j < tabFaces.size(); j++)
    {
        if (TYAcousticSurface::safeDownCast(tabFaces[j]->getElement())->getIsSub())
        {
            tabSubFaces.push_back(TYAcousticSurface::safeDownCast(tabFaces[j]->getElement()));
        }
    }

    return tabSubFaces;
}


void TYAcousticCylinder::setIsRayonnant(bool rayonnant /*= true*/, bool recursif /*=true*/)
{
    TYAcousticVolume::setIsRayonnant(rayonnant, recursif);
}


void TYAcousticCylinder::setNextRegimeNb(const int& next)
{
    _nextRegime = next;

    // Surface du dessus
    _pCircTop->setNextRegimeNb(next);
    ///Surface de dessous.
    _pCircBottom->setNextRegimeNb(next);
    // Enveloppe
    _pEnveloppe->setNextRegimeNb(next);


}

void TYAcousticCylinder::setRegimeName(const QString& name)
{
    TYAcousticInterface::setRegimeName(name);

    // Surface du dessus
    _pCircTop->setRegimeName(name);
    ///Surface de dessous.
    _pCircBottom->setRegimeName(name);
    // Enveloppe
    _pEnveloppe->setRegimeName(name);
}

void TYAcousticCylinder::updateSurfEnveloppe()
{
    double sizeRectangle = sqrt(getDiameter() * M_PI * getHauteur());   // = 2.pi.r.h
    _pEnveloppe->getBoundingRect()->setDimension(sizeRectangle, sizeRectangle);
}

void TYAcousticCylinder::exportCSV(std::ofstream& ofs)
{
    // Export du nom de l'objet
    ofs << getName().toAscii().data() << '\n';

    // Export du type de l'objet
    ofs << toString() << '\n';

    // Export des donnees acoustiques
    TYAcousticInterface::exportCSV(ofs);

    _pCircTop->exportCSV(ofs);
    _pCircBottom->exportCSV(ofs);
    _pEnveloppe->exportCSV(ofs);
}
