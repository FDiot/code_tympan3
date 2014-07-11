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
#include "Tympan/GraphicIHM/DataManagerIHM/TYAcousticVolumeNodeWidget.h"
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/models/business/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

TY_EXTENSION_INST(TYAcousticVolumeNode);

TYAcousticVolumeNode::TYAcousticVolumeNode()
{
    _name = TYNameManager::get()->generateName(getClassName());

    _regimeChangeAble = true;
}

TYAcousticVolumeNode::TYAcousticVolumeNode(const TYAcousticVolumeNode& other)
{
    *this = other;
}

TYAcousticVolumeNode::~TYAcousticVolumeNode()
{
    purge();
}

TYAcousticVolumeNode& TYAcousticVolumeNode::operator=(const TYAcousticVolumeNode& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        TYColorInterface::operator =(other);
        TYAcousticInterface::operator =(other);
        _tabAcousticVol = other._tabAcousticVol;
    }
    return *this;
}

bool TYAcousticVolumeNode::operator==(const TYAcousticVolumeNode& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        if (TYColorInterface::operator !=(other)) { return false; }
        if (TYAcousticInterface::operator !=(other)) { return false; }
        if (!(_tabAcousticVol == other._tabAcousticVol)) { return false; }
    }
    return true;
}

bool TYAcousticVolumeNode::operator!=(const TYAcousticVolumeNode& other) const
{
    return !operator==(other);
}

bool TYAcousticVolumeNode::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    purge();

    TYAcousticVolumeNode* pOtherAccVolNode = (TYAcousticVolumeNode*) pOther;

    TYColorInterface::deepCopy((TYColorInterface*) pOther, copyId);

    TYAcousticInterface::deepCopy(pOtherAccVolNode, copyId);

    for (int i = 0; i < pOtherAccVolNode->getNbChild(); i++)
    {
        TYAcousticVolume* pNewChild = (TYAcousticVolume*) pOtherAccVolNode->getAcousticVol(i)->clone();
        pNewChild->deepCopy(pOtherAccVolNode->getAcousticVol(i), copyId);
        addAcousticVol(pNewChild, pOtherAccVolNode->_tabAcousticVol[i]->getORepere3D(), false);
    }

    return true;
}

std::string TYAcousticVolumeNode::toString() const
{
    return "TYAcousticVolumeNode";
}

DOM_Element TYAcousticVolumeNode::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);
    TYColorInterface::toXML(domNewElem);
    TYAcousticInterface::toXML(domNewElem);

    // On commence par supprimer toutes les sources ponctuelles sauf celles des ventils et des cheminees
    remAllSrcs();

    TYXMLTools::addElementUIntValue(domNewElem, "nbChild", getNbChild());

    for (int i = 0; i < getNbChild(); i++)
    {
        _tabAcousticVol.at(i)->toXML(domNewElem);
    }

    return domNewElem;
}

int TYAcousticVolumeNode::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);
    TYColorInterface::fromXML(domElement);
    TYAcousticInterface::fromXML(domElement);

    // Reset
    purge();

    int retVal = -1;
    LPTYAcousticVolumeGeoNode pAccVolGeoNode = new TYAcousticVolumeGeoNode(NULL, this);

    DOM_Element elemCur;
    QDomNodeList childs = domElement.childNodes();
    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        if (pAccVolGeoNode->callFromXMLIfEqual(elemCur, &retVal))
        {
            if (retVal == 1)
            {
                // Ajout au tableau des volumes
                pAccVolGeoNode->setParent(this);
                pAccVolGeoNode->getElement()->setParent(this);
                _tabAcousticVol.push_back(pAccVolGeoNode);
                // Cration d'un nouveau volume vierge
                pAccVolGeoNode = new TYAcousticVolumeGeoNode(NULL, this);
            }
        }
    }

    // On force le regime des sous elements
    setCurRegime(_curRegime);

    // Corrige le nombre de regimes des enfants (correction bug)
    correctNbRegimes();

    return 1;
}

void TYAcousticVolumeNode::getChilds(LPTYElementArray& childs, bool recursif /*=true*/)
{
    TYElement::getChilds(childs, recursif);

    for (int i = 0; i < getNbChild(); i++)
    {
        childs.push_back(_tabAcousticVol[i]);
        childs.push_back(_tabAcousticVol[i]->getElement());
    }

    if (recursif)
    {
        for (int i = 0; i < getNbChild(); i++)
        {
            _tabAcousticVol[i]->getChilds(childs, recursif);
        }
    }
}

void TYAcousticVolumeNode::setRegime(TYSpectre& Spectre, int regime /*=-1*/, bool recursif /*=false*/)
{
    if (recursif)
    {
        for (int i = 0; i < getNbChild(); i++)
        {
            LPTYAcousticVolume pVolume = TYAcousticVolume::safeDownCast(_tabAcousticVol[i]->getElement());

            pVolume->setRegime(Spectre, regime, recursif);
        }
    }

    TYAcousticInterface::setRegime(Spectre, regime, recursif);
}

void TYAcousticVolumeNode::setRegimeName(const QString& name)
{
    TYAcousticInterface::setRegimeName(name);

    for (int i = 0; i < getNbChild(); i++)
    {
        LPTYAcousticVolume pVolume = TYAcousticVolume::safeDownCast(_tabAcousticVol[i]->getElement());

        pVolume->setRegimeName(name);
    }
}

bool TYAcousticVolumeNode::remRegime(int regime)
{
    bool ret = true;

    if (TYAcousticInterface::remRegime(regime))
    {
        for (int i = 0; i < getNbChild(); i++)
        {
            LPTYAcousticVolume pVolume = TYAcousticVolume::safeDownCast(_tabAcousticVol[i]->getElement());

            pVolume->remRegime(regime);
        }
    }
    else
    {
        ret = false;
    }

    return ret;
}

void TYAcousticVolumeNode::loadRegime(int regimeNb)
{
    for (int i = 0; i < getNbChild(); i++)
    {
        LPTYAcousticVolume pVolume = TYAcousticVolume::safeDownCast(_tabAcousticVol[i]->getElement());

        pVolume->loadRegime(regimeNb);
    }

    TYAcousticInterface::loadRegime(regimeNb);
}

void TYAcousticVolumeNode::setCurRegime(int regime)
{
    for (unsigned int i = 0; i < _tabAcousticVol.size(); i++)
    {
        LPTYAcousticVolume pVolume = TYAcousticVolume::safeDownCast(_tabAcousticVol[i]->getElement());

        pVolume->setCurRegime(regime);
    }

    TYAcousticInterface::setCurRegime(regime);

    setIsAcousticModified(true);
}

int TYAcousticVolumeNode::addRegime()
{
    for (unsigned int i = 0; i < _tabAcousticVol.size(); i++)
    {
        TYAcousticVolume::safeDownCast(_tabAcousticVol[i]->getElement())->addRegime();
    }

    return TYAcousticInterface::addRegime();
}

void TYAcousticVolumeNode::correctNbRegimes()
{
    size_t nbRegimes = _tabRegimes.size();
    LPTYElementArray childs;
    getChilds(childs, false);
    LPTYAcousticVolume pVolume = NULL;
    LPTYGeometryNode pNode = NULL;
    for (size_t i = 0; i < childs.size(); i++)
    {
        pNode = TYGeometryNode::safeDownCast(childs[i]);
        if (pNode == NULL) { continue; }
        pVolume = TYAcousticVolume::safeDownCast(pNode->getElement());
        if (pVolume)
        {
            if (pVolume->getNbRegimes() > nbRegimes)
            {
                pVolume->setNbRegimes(static_cast<int>(nbRegimes));
            }

            pVolume->correctNbRegimes();
        }
    }
}

void TYAcousticVolumeNode::propagateAtt(LPTYAttenuateur pAtt)
{
    LPTYAcousticVolume pVol = NULL;
    for (unsigned int i = 0; i < _tabAcousticVol.size(); i++)
    {
        pVol = TYAcousticVolume::safeDownCast(_tabAcousticVol[i]->getElement());
        if (pVol) { pVol->propagateAtt(pAtt); }
    }

    TYAcousticInterface::propagateAtt(pAtt);
}

TYTabSourcePonctuelleGeoNode TYAcousticVolumeNode::getSrcs() const
{
    TYTabSourcePonctuelleGeoNode tab;

    // Pour chaque child
    for (unsigned int i = 0; i < _tabAcousticVol.size(); i++)
    {
        LPTYAcousticVolume pVolume = TYAcousticVolume::safeDownCast(_tabAcousticVol[i]->getElement());

        if (pVolume->getIsRayonnant())
        {
            // Recupere l'ensemble des srcs du child...
            TYTabSourcePonctuelleGeoNode tabChild = pVolume->getSrcs();

            // Concatenation des matrices
            OMatrix matrix = _tabAcousticVol[i]->getMatrix();
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

TYSourcePonctuelle TYAcousticVolumeNode::srcPonctEquiv() const
{
    return TYSourcePonctuelle();
}

void TYAcousticVolumeNode::setDensiteSrcsH(double densite, bool recursif /*=true*/)
{
    TYAcousticInterface::setDensiteSrcsH(densite, recursif);

    if (recursif)
    {
        for (int i = 0; i < getNbChild(); i++)
        {
            LPTYAcousticVolume pVolume = TYAcousticVolume::safeDownCast(_tabAcousticVol[i]->getElement());

            assert(pVolume);

            // Densite H de srcs ponct
            pVolume->setDensiteSrcsH(getDensiteSrcsH(), recursif);
        }
    }
}

void TYAcousticVolumeNode::setDensiteSrcsV(double densite, bool recursif /*=true*/)
{
    TYAcousticInterface::setDensiteSrcsV(densite, recursif);

    if (recursif)
    {
        for (int i = 0; i < getNbChild(); i++)
        {
            LPTYAcousticVolume pVolume = TYAcousticVolume::safeDownCast(_tabAcousticVol[i]->getElement());

            assert(pVolume);

            // Densite V de srcs ponct
            pVolume->setDensiteSrcsV(getDensiteSrcsV(), recursif);
        }
    }
}

void TYAcousticVolumeNode::distriSrcs()
{
    for (int i = 0; i < getNbChild(); i++)
    {
        LPTYAcousticVolume pVolume = TYAcousticVolume::safeDownCast(_tabAcousticVol[i]->getElement());

        assert(pVolume);

        // Calcul de la distribution des srcs pour chaque volume s'il est rayonnant
        if (pVolume->getIsRayonnant())
        {
            pVolume->distriSrcs();
        }
    }
}

bool TYAcousticVolumeNode::setSrcsLw()
{
    for (int i = 0; i < getNbChild(); i++)
    {
        LPTYAcousticVolume pVolume = TYAcousticVolume::safeDownCast(_tabAcousticVol[i]->getElement());

        assert(pVolume);

        // Affectation de la puissance a chaque volume (si rayonnant)
        if (pVolume->getIsRayonnant())
        {
            pVolume->setSrcsLw();
        }
    }

    return true;
}

void TYAcousticVolumeNode::remAllSrcs()
{
    for (int i = 0; i < getNbChild(); i++)
    {
        LPTYAcousticVolume pVolume = TYAcousticVolume::safeDownCast(_tabAcousticVol[i]->getElement());

        assert(pVolume);

        pVolume->remAllSrcs();
    }
}

bool TYAcousticVolumeNode::updateAcoustic(const bool& force) // force = false
{
    //   NB : On travaille sur un regime donne.

    if (!getIsAcousticModified() && !force) { return true; }   // L'acoustique est i¿½ jour

    int i;
    bool ret = true;

    // Quoiqu'il en soit, on efface toutes les sources
    remAllSrcs();

    // 1/ Recuperer la surface totale
    double surfTotale = activeSurface();

    TYSpectre LWc = TYSpectre::getEmptyLinSpectre(); // Spectre cumule de tous les volumes
    TYSpectre LWg;
    TYSpectre LWv; //Spectre de puissance du volume courant

    // Si la puissance est imposee on part de cette puissance, sinon on repart d'une puissance nulle
    // Pour eviter la redistribution due aux appels successifs a la methode update
    if (this->_typeDistribution == TY_PUISSANCE_IMPOSEE)
    {
        LWg = this->getCurrentSpectre().toGPhy(); // Spectre global du regime courant
    }
    else // TY_PUISSANCE_CALCULEE
    {
        LWg = TYSpectre::getEmptyLinSpectre();
    }

    LWc.setType(SPECTRE_TYPE_LW);

    // la machine(volumeNode transmet son spectre de puissance a ses elements volumes)

    for (i = 0 ; i < getNbChild() ; i++)
    {
        LPTYAcousticVolume volCourant = TYAcousticVolume::safeDownCast(_tabAcousticVol[i]->getElement());

        // Si le volume est rayonnant
        if (volCourant->getIsRayonnant())
        {
            LWv = volCourant->setGlobalLW(LWg, surfTotale);
            LWc = LWc.sum(LWv);
            ret &= volCourant->updateAcoustic();
        }
        else
        {
            TYSpectre aTYSpectre;
            volCourant->setRegime(aTYSpectre) ;
        }
    }


    if (ret && (_typeDistribution == TY_PUISSANCE_CALCULEE))
    {
        // Puissance de la machine = puissance cumulee des volumes qui la composent
        LWc = OSpectre::getEmptyLinSpectre();
        LWc.setType(SPECTRE_TYPE_LW);

        for (i = 0 ; i < getNbChild() ; i++)
        {
            LPTYAcousticVolume volCourant = TYAcousticVolume::safeDownCast(_tabAcousticVol[i]->getElement());
            if (volCourant->getIsRayonnant()) { LWc = LWc.sum(volCourant->getCurrentSpectre().toGPhy()); }
        }
        TYSpectre aTYSpectre(LWc.toDB());
        setRegime(aTYSpectre);
    }
    else if (ret && (_typeDistribution == TY_PUISSANCE_IMPOSEE))
    {
        // Il faut s'assurer que la somme des puissances des volumes egale la puissance de la machine
        double residu = ABS(10 * log10(LWg.sigma()) - 10 * log10(LWc.sigma()));

        // Si l'ecart est inferieur a 1 dB on accepte
        ret = residu < 1 ? true : false;
    }


    if (ret) // Si ca c'est bien passe, on remet des sources et on leur attribue la puissance
    {
        distriSrcs();
        ret &= setSrcsLw();
    }

    if (ret) { setIsAcousticModified(false); }   // L'acoustique est i¿½ jour

    return ret;
}

double TYAcousticVolumeNode::volume() const
{
    double res = 0.0;

    for (int i = 0; i < getNbChild(); i++)
    {
        LPTYAcousticVolume pVolume = TYAcousticVolume::safeDownCast(_tabAcousticVol[i]->getElement());

        res += pVolume->volume();
    }

    return res;
}

double TYAcousticVolumeNode::surface() const
{
    double res = 0.0;

    for (int i = 0; i < getNbChild(); i++)
    {
        LPTYAcousticVolume pVolume = TYAcousticVolume::safeDownCast(_tabAcousticVol[i]->getElement());

        res += pVolume->surface();
    }

    return res;
}

double TYAcousticVolumeNode::activeSurface() const
{
    double res = 0.0;

    for (int i = 0; i < getNbChild(); i++)
    {
        LPTYAcousticVolume pVolume = TYAcousticVolume::safeDownCast(_tabAcousticVol[i]->getElement());

        if (pVolume->getIsRayonnant()) { res += pVolume->activeSurface(); } // Somme la surface si rayonnant !
    }

    return res;

}

double TYAcousticVolumeNode::activeSurface()
{
    double res = 0.0;

    for (int i = 0; i < getNbChild(); i++)
    {
        LPTYAcousticVolume pVolume = TYAcousticVolume::safeDownCast(_tabAcousticVol[i]->getElement());

        if (pVolume->getIsRayonnant()) { res += pVolume->activeSurface(); } // Somme la surface si rayonnant !
    }

    return res;

}

TYTabVector TYAcousticVolumeNode::normals() const
{
    TYTabVector tab;

    for (int i = 0; i < getNbChild(); i++)
    {
        LPTYAcousticVolume pVolume = TYAcousticVolume::safeDownCast(_tabAcousticVol[i]->getElement());
        // On recupere l'ensemble des normales par volume
        TYTabVector tabTmp = pVolume->normals();
        OMatrix matrix = _tabAcousticVol[i]->getMatrix();

        // Pour chacune de ces normales
        for (register unsigned int j = 0; j < tabTmp.size(); j++)
        {
            // On convertit la normale dans le repere du vol node
            tabTmp[j] = matrix * tabTmp[j];
        }

        // Ajout
        tab.insert(tab.end(), tabTmp.begin(), tabTmp.end());
    }

    return tab;
}

TYTabPoint TYAcousticVolumeNode::sommets() const
{
    TYTabPoint tab;

    for (register int i = 0; i < getNbChild(); i++)
    {
        LPTYAcousticVolume pVolume = TYAcousticVolume::safeDownCast(_tabAcousticVol[i]->getElement());
        // On recupere l'ensemble des sommets par volume
        TYTabPoint tabTmp = pVolume->sommets();
        OMatrix matrix = _tabAcousticVol[i]->getMatrix();

        // Pour chacun de ces sommets
        for (register unsigned int j = 0; j < tabTmp.size(); j++)
        {
            // On convertit le point dans le repere du vol node
            tabTmp[j] = matrix * tabTmp[j];
        }

        // Ajout
        tab.insert(tab.end(), tabTmp.begin(), tabTmp.end());
    }

    return tab;
}

TYTabLPPolygon TYAcousticVolumeNode::faces() const
{
    TYTabLPPolygon tab;

    for (register int i = 0; i < getNbChild(); i++)
    {
        LPTYAcousticVolume pVolume = TYAcousticVolume::safeDownCast(_tabAcousticVol[i]->getElement());
        // On recupere l'ensemble des faces par volume
        TYTabLPPolygon tabTmp = pVolume->faces();
        OMatrix matrix = _tabAcousticVol[i]->getMatrix();

        // Pour chacune de ces faces
        for (register unsigned int j = 0; j < tabTmp.size(); j++)
        {
            // Polygon representant la face
            TYPolygon* pPolygon = tabTmp[j];

            // On convertit le polygon dans le repere du vol node
            pPolygon->transform(matrix);
        }

        // Ajout
        tab.insert(tab.end(), tabTmp.begin(), tabTmp.end());
    }

    return tab;
}

TYBox TYAcousticVolumeNode::volEnglob() const
{
    return TYVolumeInterface::volEnglob();
}

TYPoint TYAcousticVolumeNode::centreGravite() const
{
    printf("TYAcousticVolumeNode::centreGravite non implemente.\n");
    return volEnglob().getPosition();
}

int TYAcousticVolumeNode::intersects(const OSegment3D& seg, TYTabPoint& ptList) const
{
    return TYVolumeInterface::intersects(seg, ptList);
}

int TYAcousticVolumeNode::isInside(const TYPoint& pt) const
{
    int res = INTERS_NULLE;
    TYPoint ptTest;

    // Pour eviter des calculs inutiles...
    if (volEnglob().isInside(pt) == INTERS_NULLE) { return res; }

    // On teste si le point se trouve au moins a l'interieur d'un
    // volume composant ce volume node
    for (register int i = 0; (i < getNbChild()) && (res == INTERS_NULLE); i++)
    {
        // On convertit le point dans le repere local au volume
        OMatrix matrix = _tabAcousticVol[i]->getMatrix();
        matrix.invert();
        ptTest = matrix * pt;

        LPTYAcousticVolume pVolume = TYAcousticVolume::safeDownCast(_tabAcousticVol[i]->getElement());

        // Test
        res = pVolume->isInside(ptTest);
    }

    return res;
}

TYTabAcousticSurfaceGeoNode TYAcousticVolumeNode::acousticFaces()
{
    TYTabAcousticSurfaceGeoNode tab;

    for (int i = 0; i < getNbChild(); i++)
    {
        LPTYAcousticVolume pVolume = TYAcousticVolume::safeDownCast(_tabAcousticVol[i]->getElement());
        TYTabAcousticSurfaceGeoNode tabTmp = pVolume->acousticFaces();
        OMatrix matrixVol = _tabAcousticVol[i]->getMatrix();

        for (unsigned int j = 0; j < tabTmp.size(); j++)
        {
            tabTmp[j]->setMatrix(matrixVol * tabTmp[j]->getMatrix());
            tab.push_back(tabTmp[j]);
        }
    }

    return tab;
}

void TYAcousticVolumeNode::purge()
{
    _tabAcousticVol.clear();
    setIsGeometryModified(true);
}

bool TYAcousticVolumeNode::addAcousticVol(LPTYAcousticVolumeGeoNode pAccVolGeoNode, bool recursif/*=true*/)
{
    assert(pAccVolGeoNode);

    LPTYAcousticVolume pVolume = TYAcousticVolume::safeDownCast(pAccVolGeoNode->getElement());

    assert(pVolume);

    pAccVolGeoNode->setParent(this);
    pVolume->setParent(this);

    if (recursif)
    {
        // Affectation de la densite du volume egale a celle du parent
        pVolume->setDensiteSrcsH(_densiteSrcsH);
        pVolume->setDensiteSrcsV(_densiteSrcsV);
    }

    // On construit autant de regime pour cette nouvelle surface que (this) en a
    // NB elle a au moins un regime par construction c'est pourquoi on par de 1
    int i;
    for (i = 1 ; i < getNbRegimes() ; i++)
    {
        pVolume->addRegime();
    }

    // On affecte les memes noms aux differents regimes
    bool status = true;
    for (i = 0; i < getNbRegimes(); i++)
    {
        QString nomReg = getRegimeNb(i, status).getRegimeName();
        pVolume->setCurRegime(i);
        pVolume->setRegimeName(nomReg);
    }


    // On met le volume au meme regime que son parent (CAD this)
    pVolume->setCurRegime(getCurRegime());


    // Synchronise le numero suivant
    pVolume->setNextRegimeNb(getNextRegimeNb());

    _tabAcousticVol.push_back(pAccVolGeoNode);

    setIsAcousticModified(true);
    setIsGeometryModified(true);

    return true;
}

bool TYAcousticVolumeNode::addAcousticVol(LPTYAcousticVolume pAccVol, const TYRepere& repere, bool recursif)
{
    return addAcousticVol(new TYAcousticVolumeGeoNode(repere, (LPTYElement)pAccVol), recursif);
}

bool TYAcousticVolumeNode::addAcousticVol(LPTYAcousticVolume pAccVol, bool recursif)
{
    return addAcousticVol(new TYAcousticVolumeGeoNode((LPTYElement)pAccVol), recursif);
}

bool TYAcousticVolumeNode::remAcousticVol(const LPTYAcousticVolume pAccVol)
{
    assert(pAccVol);
    bool ret = false;
    TYTabAcousticVolumeGeoNode::iterator ite;

    for (ite = _tabAcousticVol.begin(); ite != _tabAcousticVol.end(); ite++)
    {
        if (TYAcousticVolume::safeDownCast((*ite)->getElement()) == pAccVol)
        {
            _tabAcousticVol.erase(ite);
            ret = true;
            break;
        }
    }

    setIsGeometryModified(true);
    setIsAcousticModified(true);

    return ret;
}

bool TYAcousticVolumeNode::remAcousticVol(const LPTYAcousticVolumeGeoNode pAccVolGeoNode)
{
    assert(pAccVolGeoNode);
    bool ret = false;
    TYTabAcousticVolumeGeoNode::iterator ite;

    for (ite = _tabAcousticVol.begin(); ite != _tabAcousticVol.end(); ite++)
    {
        if ((*ite) == pAccVolGeoNode)
        {
            _tabAcousticVol.erase(ite);
            ret = true;
            break;
        }
    }

    setIsGeometryModified(true);
    setIsAcousticModified(true);

    return ret;
}

bool TYAcousticVolumeNode::remAcousticVol(QString idAccVol)
{
    bool ret = false;
    TYTabAcousticVolumeGeoNode::iterator ite;

    for (ite = _tabAcousticVol.begin(); ite != _tabAcousticVol.end(); ite++)
    {
        if (TYAcousticVolume::safeDownCast((*ite)->getElement())->getID().toString() == idAccVol)
        {
            _tabAcousticVol.erase(ite);
            ret = true;
            break;
        }
    }

    setIsGeometryModified(true);
    setIsAcousticModified(true);

    return ret;
}

LPTYAcousticVolume TYAcousticVolumeNode::getAcousticVol(int index)
{
    if ((index >= 0) && (index < (int)_tabAcousticVol.size()))
    {
        return TYAcousticVolume::safeDownCast(_tabAcousticVol.at(index)->getElement());
    }
    else { return NULL; }
}

LPTYAcousticVolumeGeoNode TYAcousticVolumeNode::findAcousticVol(const LPTYAcousticVolume pAccVol)
{
    assert(pAccVol);
    TYTabAcousticVolumeGeoNode::iterator ite;

    for (ite = _tabAcousticVol.begin(); ite != _tabAcousticVol.end(); ite++)
    {
        if (TYAcousticVolume::safeDownCast((*ite)->getElement()) == pAccVol)
        {
            return (*ite);
        }
    }

    return NULL;
}

void TYAcousticVolumeNode::setIsRayonnant(bool rayonnant, bool recursif /*= true */)
{
    TYAcousticInterface::setIsRayonnant(rayonnant);
}


LPTYSpectre TYAcousticVolumeNode::getRealPowerSpectrum()
{
    LPTYElementArray childs;
    getChilds(childs, true);
    TYSpectre sp;
    sp.setType(SPECTRE_TYPE_LW);
    LPTYAcousticSurface pSurf = NULL;
    for (unsigned int i = 0; i < childs.size(); i++)
    {
        pSurf = dynamic_cast<TYAcousticSurface*>(childs[i]._pObj);
        if (pSurf._pObj != nullptr)
        {
            sp = sp.sumdB(*pSurf->getRealPowerSpectrum());
        }
    }

    return new TYSpectre(sp);
}

void TYAcousticVolumeNode::exportCSV(std::ofstream& ofs)
{
    // Mise a jour de l'acoustique
    updateAcoustic();

    // Export du nom de l'objet
    ofs << getName().toAscii().data() << '\n';

    // Export du type de l'objet
    ofs << toString() << '\n';
    // Export des donnees acoustiques
    TYAcousticInterface::exportCSV(ofs);

    TYTabAcousticVolumeGeoNode::iterator ite;

    LPTYAcousticVolume pVol = NULL;

    for (ite = _tabAcousticVol.begin(); ite != _tabAcousticVol.end(); ite++)
    {
        pVol = TYAcousticVolume::safeDownCast((*ite)->getElement());
        if (pVol)
        {
            pVol->exportCSV(ofs);
        }
    }

    ofs << '\n';
}
