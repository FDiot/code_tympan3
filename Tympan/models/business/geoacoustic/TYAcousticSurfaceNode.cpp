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
#include "Tympan/GraphicIHM/DataManagerIHM/TYAcousticSurfaceNodeWidget.h"
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/models/business/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER


TY_EXTENSION_INST(TYAcousticSurfaceNode);

TYAcousticSurfaceNode::TYAcousticSurfaceNode()
{
    _name = TYNameManager::get()->generateName(getClassName());

    _pBoundingRect = new TYRectangle();
    _pBoundingRect->setParent(this);
}

TYAcousticSurfaceNode::TYAcousticSurfaceNode(const TYAcousticSurfaceNode& other)
{
    *this = other;
}

TYAcousticSurfaceNode::~TYAcousticSurfaceNode()
{
    purge();
}

TYAcousticSurfaceNode& TYAcousticSurfaceNode::operator=(const TYAcousticSurfaceNode& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        TYColorInterface::operator =(other);
        TYAcousticInterface::operator =(other);
        _tabAcousticSurf = other._tabAcousticSurf;
        _pBoundingRect = other._pBoundingRect;
    }
    return *this;
}

bool TYAcousticSurfaceNode::operator==(const TYAcousticSurfaceNode& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        if (TYColorInterface::operator !=(other)) { return false; }
        if (TYAcousticInterface::operator !=(other)) { return false; }
        if (!(_tabAcousticSurf == other._tabAcousticSurf)) { return false; }
        if (!(_pBoundingRect == other._pBoundingRect)) { return false; }
    }
    return true;
}

bool TYAcousticSurfaceNode::operator!=(const TYAcousticSurfaceNode& other) const
{
    return !operator==(other);
}

bool TYAcousticSurfaceNode::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    purge();

    TYAcousticSurfaceNode* pOtherAccSurfNode = (TYAcousticSurfaceNode*) pOther;

    TYColorInterface::deepCopy((TYColorInterface*)pOtherAccSurfNode, copyId);

    TYAcousticInterface::deepCopy(pOtherAccSurfNode, copyId);

    for (int i = 0; i < pOtherAccSurfNode->getNbChild(); i++)
    {
        TYAcousticSurface* pNewChild = (TYAcousticSurface*) pOtherAccSurfNode->getAcousticSurf(i)->clone();
        pNewChild->deepCopy(pOtherAccSurfNode->getAcousticSurf(i), copyId);
        addAcousticSurf(pNewChild, pOtherAccSurfNode->_tabAcousticSurf[i]->getORepere3D());
    }

    _pBoundingRect->deepCopy(pOtherAccSurfNode->_pBoundingRect, copyId);

    return true;
}

std::string TYAcousticSurfaceNode::toString() const
{
    return "TYAcousticSurfaceNode";
}

DOM_Element TYAcousticSurfaceNode::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);
    TYColorInterface::toXML(domNewElem);
    TYAcousticInterface::toXML(domNewElem);

    _pBoundingRect->toXML(domNewElem);

    TYXMLTools::addElementUIntValue(domNewElem, "nbChild", getNbChild());

    for (int i = 0; i < getNbChild(); i++)
    {
        _tabAcousticSurf.at(i)->toXML(domNewElem);
    }

    return domNewElem;
}

int TYAcousticSurfaceNode::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);
    TYColorInterface::fromXML(domElement);
    TYAcousticInterface::fromXML(domElement);

    // Reset
    purge();

    int nbChild = 0, retVal = -1;
    LPTYAcousticSurfaceGeoNode pAccSurfGeoNode = new TYAcousticSurfaceGeoNode(NULL, this);

    bool nbChildOk = false;
    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();
    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();
        _pBoundingRect->callFromXMLIfEqual(elemCur);

        TYXMLTools::getElementIntValue(elemCur, "nbChild", nbChild, nbChildOk);
        if (nbChildOk)
        {
            _tabAcousticSurf.reserve(nbChild);
        }

        if (pAccSurfGeoNode->callFromXMLIfEqual(elemCur, &retVal))
        {
            if (retVal == 1)
            {
                // Ajout de la surface au tableau des surfaces
                pAccSurfGeoNode->setParent(this);
                pAccSurfGeoNode->getElement()->setParent(this);
                _tabAcousticSurf.push_back(pAccSurfGeoNode);

                // Creation d'une surface vierge
                pAccSurfGeoNode = new TYAcousticSurfaceGeoNode(NULL, this);
            }
        }
    }

    return 1;
}

void TYAcousticSurfaceNode::setIsAcousticModified(bool isModified)
{
    TYElement::setIsAcousticModified(isModified);

    if (_pParent) { _pParent->setIsAcousticModified(isModified); }
}

void TYAcousticSurfaceNode::getChilds(LPTYElementArray& childs, bool recursif /*=true*/)
{
    TYElement::getChilds(childs, recursif);

    for (int i = 0; i < getNbChild(); i++)
    {
        childs.push_back(_tabAcousticSurf[i]);
        childs.push_back(_tabAcousticSurf[i]->getElement());
    }

    if (recursif)
    {
        for (int i = 0; i < getNbChild(); i++)
        {
            _tabAcousticSurf[i]->getChilds(childs, recursif);
        }
    }
}

TYTabSourcePonctuelleGeoNode TYAcousticSurfaceNode::getSrcs() const
{
    TYTabSourcePonctuelleGeoNode tab;

    // Pour chaque child
    for (int i = 0; i < getNbChild(); i++)
    {
        LPTYAcousticSurface pSurface = TYAcousticSurface::safeDownCast(_tabAcousticSurf[i]->getElement());

        if (pSurface->getIsRayonnant())
        {
            // Recupere l'ensemble des srcs du child...
            TYTabSourcePonctuelleGeoNode tabChild = pSurface->getSrcs();

            // Concatenation des matrices
            OMatrix matrix = _tabAcousticSurf[i]->getMatrix();
            for (unsigned int j = 0; j < tabChild.size(); j++)
            {
                tabChild[j]->setMatrix(matrix * tabChild[j]->getMatrix());
            }

            //...et ajoute au tableau a retourner
            tab.insert(tab.end(), tabChild.begin(), tabChild.end());
        }
    }

    return tab;
}

TYSourcePonctuelle TYAcousticSurfaceNode::srcPonctEquiv() const
{
    return TYSourcePonctuelle();
}

void TYAcousticSurfaceNode::setDensiteSrcsH(double densite, bool recursif /*=true*/)
{
    TYAcousticInterface::setDensiteSrcsH(densite, recursif);

    if (recursif)
    {
        for (int i = 0; i < getNbChild(); i++)
        {
            LPTYAcousticSurface pSurface = TYAcousticSurface::safeDownCast(_tabAcousticSurf[i]->getElement());

            assert(pSurface);

            // Densite H de srcs ponct
            pSurface->setDensiteSrcsH(getDensiteSrcsH(), recursif);
        }
    }
}

void TYAcousticSurfaceNode::setDensiteSrcsV(double densite, bool recursif /*=true*/)
{
    TYAcousticInterface::setDensiteSrcsV(densite, recursif);

    if (recursif)
    {
        for (int i = 0; i < getNbChild(); i++)
        {
            LPTYAcousticSurface pSurface = TYAcousticSurface::safeDownCast(_tabAcousticSurf[i]->getElement());

            assert(pSurface);

            // Densite V de srcs ponct
            pSurface->setDensiteSrcsV(getDensiteSrcsV(), recursif);
        }
    }
}

void TYAcousticSurfaceNode::setRegime(TYSpectre& Spectre, int regime /*=-1*/, bool recursif /*=false*/)
{
    if (recursif)
    {
        for (int i = 0; i < getNbChild(); i++)
        {
            LPTYAcousticSurface pSurface = TYAcousticSurface::safeDownCast(_tabAcousticSurf[i]->getElement());


            pSurface->setRegime(Spectre, regime, recursif);
        }
    }

    TYAcousticInterface::setRegime(Spectre, regime, recursif);
}

bool TYAcousticSurfaceNode::remRegime(int regime)
{
    for (int i = 0; i < getNbChild(); i++)
    {
        LPTYAcousticSurface pSurface = TYAcousticSurface::safeDownCast(_tabAcousticSurf[i]->getElement());

        pSurface->remRegime(regime);
    }

    return TYAcousticInterface::remRegime(regime);
}

void TYAcousticSurfaceNode::loadRegime(int regimeNb)
{
    for (int i = 0; i < getNbChild(); i++)
    {
        LPTYAcousticSurface pSurface = TYAcousticSurface::safeDownCast(_tabAcousticSurf[i]->getElement());

        pSurface->loadRegime(regimeNb);
    }

    TYAcousticInterface::loadRegime(regimeNb);
}

void TYAcousticSurfaceNode::setCurRegime(int regime)
{
    for (unsigned int i = 0; i < _tabAcousticSurf.size(); i++)
    {
        LPTYAcousticSurface pSurface = TYAcousticSurface::safeDownCast(_tabAcousticSurf[i]->getElement());

        pSurface->setCurRegime(regime);
    }

    TYAcousticInterface::setCurRegime(regime);

    setIsAcousticModified(true);
}
/*
int TYAcousticSurfaceNode::addRegime(const TYRegime& regime)
{
    for (int i = 0; i < getNbChild(); i++)
    {
        LPTYAcousticSurface pSurface = TYAcousticSurface::safeDownCast(_tabAcousticSurf[i]->getElement());

        pSurface->addRegime();
    }

    return TYAcousticInterface::addRegime(regime);
}
*/
int TYAcousticSurfaceNode::addRegime()
{
    for (int i = 0; i < getNbChild(); i++)
    {
        LPTYAcousticSurface pSurface = TYAcousticSurface::safeDownCast(_tabAcousticSurf[i]->getElement());

        pSurface->addRegime();
    }

    return TYAcousticInterface::addRegime();
}

void TYAcousticSurfaceNode::correctNbRegimes()
{
    size_t nbRegimes = _tabRegimes.size();
    LPTYElementArray childs;
    getChilds(childs, false);

    LPTYGeometryNode pNode = NULL;
    LPTYAcousticSurface pSurf = NULL;
    for (size_t i = 0; i < childs.size(); i++)
    {
        pNode = TYGeometryNode::safeDownCast(childs[i]);
        if (pNode == NULL) { continue; }
        pSurf = TYAcousticSurface::safeDownCast(pNode->getElement());
        if (pSurf && (pSurf->getNbRegimes() > nbRegimes))
        {
            pSurf->setNbRegimes(static_cast<int>(nbRegimes));
        }
    }
}

void TYAcousticSurfaceNode::propagateAtt(LPTYAttenuateur pAtt)
{
    LPTYAcousticSurface pSurface = NULL;
    for (int i = 0; i < getNbChild(); i++)
    {
        pSurface = TYAcousticSurface::safeDownCast(_tabAcousticSurf[i]->getElement());

        if (pSurface) { pSurface->propagateAtt(pAtt); }
    }

    TYAcousticInterface::propagateAtt(pAtt);
}

void TYAcousticSurfaceNode::setUseAtt(bool state)
{
    for (int i = 0; i < getNbChild(); i++)
    {
        LPTYAcousticSurface pSurface = TYAcousticSurface::safeDownCast(_tabAcousticSurf[i]->getElement());

        if (pSurface->getIsSub() == false) { pSurface->setUseAtt(state); }
    }

    TYAcousticInterface::setUseAtt(state);
}

void TYAcousticSurfaceNode::distriSrcs()
{
    for (int i = 0; i < getNbChild(); i++)
    {
        LPTYAcousticSurface pSurface = TYAcousticSurface::safeDownCast(_tabAcousticSurf[i]->getElement());

        assert(pSurface);

        if (pSurface->getIsRayonnant()) { pSurface->distriSrcs(); }
    }
}

bool TYAcousticSurfaceNode::setSrcsLw()
{
    bool ret = false;

    for (unsigned int i = 0; i < _tabAcousticSurf.size() ; i++)
    {
        LPTYAcousticSurface pSurface = TYAcousticSurface::safeDownCast(_tabAcousticSurf[i]->getElement());

        if (pSurface->getIsRayonnant())
        {
            ret = pSurface->setSrcsLw();
        }
    }

    return ret;
}

bool TYAcousticSurfaceNode::updateAcoustic(const bool& force) //force=false
{
    unsigned int j;
    bool ret = true;

    // Si la surface n'est pas rayonnante, inutile de tenter de distribuer
    // la puissance, mais ce n'est pas une raison pour bloquer le calcul
    if (!_isRayonnant) { return true ; }

    // 1/ Recuperer la surface totale
    double surfTotale = activeSurface();

    TYSpectre LWc = TYSpectre::getEmptyLinSpectre(); // Spectre cumule de l'ensembles des elements
    TYSpectre LWg = getCurrentSpectre().toGPhy(); // Spectre global du regime courant
    TYSpectre LWv; //Spectre de puissance de la face courante

    LWc.setType(SPECTRE_TYPE_LW);

    // On recupere le tableau des sous faces
    for (j = 0; j < _tabAcousticSurf.size(); j++)
    {
        LPTYAcousticSurface pSurf = TYAcousticSurface::safeDownCast(_tabAcousticSurf[j]->getElement());

        if (pSurf->getIsRayonnant()) // Si elle est rayonnante
        {
            LWv = pSurf->setGlobalLW(LWg, surfTotale);
            LWc = LWc.sum(LWv);
        }
        else
        {
            TYSpectre aTYSpectre;
            pSurf->setRegime(aTYSpectre);
        }
    }

    // 3 cas
    if (ret && (_typeDistribution == TY_PUISSANCE_IMPOSEE))
    {
        // Il faut s'assurer que la somme des puissances des volumes egale la puissance de la machine
        double residu = ABS(10 * log10(LWg.sigma()) - 10 * log10(LWc.sigma()));

        // Si l'ecart est inferieur a 1 dB on accepte
        ret = residu < 1 ? true : false;
    }
    else if (ret && (_typeDistribution == TY_PUISSANCE_CALCULEE))
    {
        TYSpectre temp =  LWc.toDB();
        setRegime(temp) ; // Pas de probleme le spectre peut etre remplace
    }
    else
    {
        ret = false; // Probleme, la somme des puissances des faces n'est pas correcte
    }

    return ret;
}

double TYAcousticSurfaceNode::surface() const
{
    return _pBoundingRect->surface();
}



OVector3D TYAcousticSurfaceNode::normal() const
{
    return _pBoundingRect->normal();
}

OPlan TYAcousticSurfaceNode::plan() const
{
    return _pBoundingRect->plan();
}

TYTabPoint TYAcousticSurfaceNode::getContour(int n /*=-1*/) const
{
    return _pBoundingRect->getContour(n);
}

TYTabPoint3D TYAcousticSurfaceNode::getOContour(int n /*=-1*/) const
{
    return _pBoundingRect->getOContour(n);
}

int TYAcousticSurfaceNode::intersects(const TYSurfaceInterface* pSurf, OSegment3D& seg) const
{
    return _pBoundingRect->intersects(pSurf, seg);
}

int TYAcousticSurfaceNode::intersects(const OSegment3D& seg, OPoint3D& pt) const
{
    return _pBoundingRect->intersects(seg, pt);
}

int TYAcousticSurfaceNode::intersects(const OPoint3D& pt) const
{
    return _pBoundingRect->intersects(pt);
}

void TYAcousticSurfaceNode::purge()
{
    _tabAcousticSurf.clear();
    setIsGeometryModified(true);
}

bool TYAcousticSurfaceNode::addAcousticSurf(LPTYAcousticSurfaceGeoNode pAccSurfGeoNode)
{

    assert(pAccSurfGeoNode);

    LPTYAcousticSurface pSurface = TYAcousticSurface::safeDownCast(pAccSurfGeoNode->getElement());

    assert(pSurface);

    // On lui passe la meme couleur
    //pSurface->setColor(_color);

    int i;

    // On construit autant de regime pour cette nouvelle surface que (this) en a
    // NB elle a au moins un regime par construction c'est pourquoi on par de 1
    for (i = 1 ; i < getNbRegimes() ; i++)
    {
        pSurface->addRegime();
    }

    // On affecte les memes noms aux differents regimes
    bool status = true;
    for (i = 0; i < getNbRegimes(); i++)
    {
        QString nomReg = getRegimeNb(i, status).getRegimeName();
        pSurface->setCurRegime(i);
        pSurface->setRegimeName(nomReg);

        if (!pSurface->getIsSub())
        {
            pSurface->getRegimeNb(i, status)._isRayonnant = getRegimeNb(i, status)._isRayonnant; // Etat de l'element
            pSurface->getRegimeNb(i, status)._useAtt = getRegimeNb(i, status)._useAtt;          // Utilisation d'un attenuateur
            pSurface->getRegimeNb(i, status)._typeDistribution = TY_PUISSANCE_CALCULEE; // Puissance imposee ou calculee;
            pSurface->getRegimeNb(i, status)._pAtt = getRegimeNb(i, status)._pAtt;              // Attenuateur eventuellement associe au regime
        }
    }


    // On met la surface au meme regime que son parent (CAD this)
    pSurface->setCurRegime(getCurRegime());


    // Synchronise le numero suivant
    pSurface->setNextRegimeNb(getNextRegimeNb());


    pAccSurfGeoNode->setParent(this);
    pSurface->setParent(this);

    //Mise a jour des densite de sources pour la nouvelle surface
    pSurface->setDensiteSrcsH(_densiteSrcsH);
    pSurface->setDensiteSrcsV(_densiteSrcsV);

    // Les rejetons rayonnent comme les parents
    // Sauf pour les faces marquees "sub"
    if (pSurface->getIsSub() == false) { pSurface->setIsRayonnant(_isRayonnant, true); }

    _tabAcousticSurf.push_back(pAccSurfGeoNode);

    setIsGeometryModified(true);

    return true;
}
void TYAcousticSurfaceNode::setNextRegimeNb(const int& next)
{
    _nextRegime = next;

    for (unsigned int i = 0 ; i < _tabAcousticSurf.size() ; i++)
    {
        LPTYAcousticSurface pSurf = TYAcousticSurface::safeDownCast(_tabAcousticSurf[i]->getElement());

        pSurf->setNextRegimeNb(next);
    }
}

void TYAcousticSurfaceNode::setRegimeName(const QString& name)
{
    TYAcousticInterface::setRegimeName(name);

    for (unsigned int i = 0 ; i < _tabAcousticSurf.size() ; i++)
    {
        LPTYAcousticSurface pSurf = TYAcousticSurface::safeDownCast(_tabAcousticSurf[i]->getElement());

        pSurf->setRegimeName(name);
    }
}

bool TYAcousticSurfaceNode::addAcousticSurf(LPTYAcousticSurface pAccSurf, const TYRepere& repere)
{
    return addAcousticSurf(new TYAcousticSurfaceGeoNode(repere, (LPTYElement) pAccSurf));
}

bool TYAcousticSurfaceNode::addAcousticSurf(LPTYAcousticSurface pAccSurf)
{
    return addAcousticSurf(new TYAcousticSurfaceGeoNode((LPTYElement)pAccSurf));
}

bool TYAcousticSurfaceNode::remAcousticSurf(const LPTYAcousticSurface pAccSurf)
{
    assert(pAccSurf);
    bool ret = false;
    TYTabAcousticSurfaceGeoNode::iterator ite;

    for (ite = _tabAcousticSurf.begin(); ite != _tabAcousticSurf.end(); ite++)
    {
        if (TYAcousticSurface::safeDownCast((*ite)->getElement()) == pAccSurf)
        {
            _tabAcousticSurf.erase(ite);
            ret = true;
            break;
        }
    }

    setIsGeometryModified(true);

    return ret;
}

bool TYAcousticSurfaceNode::remAcousticSurf(const LPTYAcousticSurfaceGeoNode pAccSurfGeoNode)
{
    assert(pAccSurfGeoNode);
    bool ret = false;
    TYTabAcousticSurfaceGeoNode::iterator ite;

    for (ite = _tabAcousticSurf.begin(); ite != _tabAcousticSurf.end(); ite++)
    {
        if ((*ite) == pAccSurfGeoNode)
        {
            _tabAcousticSurf.erase(ite);
            ret = true;
            break;
        }
    }

    setIsGeometryModified(true);

    return ret;
}

bool TYAcousticSurfaceNode::remAcousticSurf(QString idAccSurf)
{
    bool ret = false;
    TYTabAcousticSurfaceGeoNode::iterator ite;

    for (ite = _tabAcousticSurf.begin(); ite != _tabAcousticSurf.end(); ite++)
    {
        if (TYAcousticSurface::safeDownCast((*ite)->getElement())->getID().toString() == idAccSurf)
        {
            _tabAcousticSurf.erase(ite);
            ret = true;
            break;
        }
    }

    setIsGeometryModified(true);

    return ret;
}

LPTYAcousticSurface TYAcousticSurfaceNode::getAcousticSurf(int index)
{
    if ((index >= 0) && (index < (int)_tabAcousticSurf.size()))
    {
        return TYAcousticSurface::safeDownCast(_tabAcousticSurf.at(index)->getElement());
    }
    else { return NULL; }
}

LPTYAcousticSurfaceGeoNode TYAcousticSurfaceNode::findAcousticSurf(const LPTYAcousticSurface pAccSurf)
{
    assert(pAccSurf);
    TYTabAcousticSurfaceGeoNode::iterator ite;

    for (ite = _tabAcousticSurf.begin(); ite != _tabAcousticSurf.end(); ite++)
    {
        if (TYAcousticSurface::safeDownCast((*ite)->getElement()) == pAccSurf)
        {
            return (*ite);
        }
    }

    return NULL;
}

TYSpectre TYAcousticSurfaceNode::setGlobalLW(const TYSpectre& spectre, const double& surfGlobale, const int& regime/*=-1*/)
{
    TYSpectre LWv;

    // 2 cas
    if (_typeDistribution == TY_PUISSANCE_CALCULEE)
    {
        LWv = spectre.toGPhy().mult(surface() / surfGlobale).toDB(); // Spectre pondere de la surface de la face
        setRegime(LWv, regime, false);
    }

    return getCurrentSpectre().toGPhy();
}

void TYAcousticSurfaceNode::setIsRayonnant(bool rayonnant, bool recursif)
{
    TYAcousticInterface::setIsRayonnant(rayonnant);

    // Les faces de construction  (!= cheminee/ventilation/fenetre) sont automatiquement rayonnantes
    for (unsigned int i = 0 ; i < _tabAcousticSurf.size() ; i++)
    {
        LPTYAcousticSurface pSurf = TYAcousticSurface::safeDownCast(_tabAcousticSurf[i]->getElement());
        assert(pSurf);
        if (pSurf->getIsSub() == false)// Si ce n'est pas une cheminee/ventil/fenetre
        {
            pSurf->setIsRayonnant(rayonnant, recursif);
        }
    }

    // Si une face rayonne son parent rayonne mais si elle ne rayonne plus ca ne veut pas dire que son parent ne rayonne plus
    if (rayonnant)
    {
        TYAcousticVolume* pVol = TYAcousticVolume::safeDownCast(this->getParent());
        assert(pVol);
        pVol->setIsRayonnant(rayonnant, recursif);
    }
}

double TYAcousticSurfaceNode::activeSurface()
{
    double surface = 0.0;

    for (unsigned int i = 0 ; i < _tabAcousticSurf.size() ; i++)
    {
        TYAcousticSurface* pSurf = TYAcousticSurface::safeDownCast(_tabAcousticSurf[i]->getElement());

        if (pSurf->getIsRayonnant() && (pSurf->getDensiteSrcs() != 0.0))
        {
            surface += pSurf->surface();
        }
    }

    return surface;
}

double TYAcousticSurfaceNode::activeSurface() const
{
    double surface = 0.0;

    for (unsigned int j = 0; j < _tabAcousticSurf.size(); j++)
    {
        LPTYAcousticSurface pSurf = TYAcousticSurface::safeDownCast(_tabAcousticSurf[j]->getElement());

        if (pSurf->getIsSub() == false) // Si ce n'est pas une cheminee ou une bouche de ventilation
        {
            if (pSurf->getIsRayonnant() && (pSurf->getDensiteSrcs() != 0.0))
            {
                surface += pSurf->surface();
            }
        }
    }

    return surface;
}

void TYAcousticSurfaceNode::remAllSrcs()
{
    for (unsigned int j = 0; j < _tabAcousticSurf.size(); j++)
    {
        LPTYAcousticSurface pSurf = TYAcousticSurface::safeDownCast(_tabAcousticSurf[j]->getElement());

        if (pSurf->getIsSub() == false) // Si ce n'est pas une cheminee ou une bouche de ventilation
        {
            pSurf->remAllSrcs();
        }
    }
}

/// Export au format csv sur un flux transmis
void TYAcousticSurfaceNode::exportCSV(std::ofstream& ofs)
{
    // Export du nom de l'objet
    ofs << getName().toAscii().data() << '\n';

    // Export du type de l'objet
    ofs << toString() << '\n';
    // Export des donnees acoustiques
    TYAcousticInterface::exportCSV(ofs);

    for (unsigned int j = 0; j < _tabAcousticSurf.size(); j++)
    {
        LPTYAcousticSurface pSurf = TYAcousticSurface::safeDownCast(_tabAcousticSurf[j]->getElement());

        if (pSurf->getIsSub()) // Si c'est une cheminee ou une bouche de ventilation
        {
            pSurf->exportCSV(ofs);
        }
    }

    ofs << '\n';

}
