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
#include "Tympan/gui/widgets/TYPointCalculWidget.h"
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/models/business/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/core/logging.h"


TY_EXTENSION_INST(TYPointCalcul);

TYPointCalcul::TYPointCalcul() : TYPoint(true)
{
    _name = TYNameManager::get()->generateName(getClassName());

    //  _state = Actif;
    _locked = false;

    _isSpectreUnique = false;

    _dBA = 0.0;
    _dBLin = 0.0;
}

TYPointCalcul::TYPointCalcul(const TYPoint& other) : TYPoint(other, true)
{
    //  _state = Actif;
    _dBA = 0.0;
    _dBLin = 0.0;

    _isSpectreUnique = false;
}

TYPointCalcul::TYPointCalcul(const TYPointCalcul& other) : TYPoint(true)
{
    *this = other;
}

TYPointCalcul::~TYPointCalcul()
{
    // Detruit les spectres calcules
    TYMapIdSpectre::iterator it_s;
    for (it_s = _tabSpectre.begin(); it_s != _tabSpectre.end(); ++it_s)
    {
        delete it_s->second;
    }
}

TYPointCalcul& TYPointCalcul::operator=(TYPointCalcul& other)
{
    if (this != &other)
    {
        TYPoint::operator =(other);

        // Copie les spectres
        copySpectres(&other);

        // Copie les etats
        copyEtats(&other);

        _dBA = other._dBA;
        _dBLin = other._dBLin;

        _isSpectreUnique = other._isSpectreUnique;
    }
    return *this;
}

TYPointCalcul& TYPointCalcul::operator=(const TYPoint& other)
{
    if (this != &other)
    {
        TYPoint::operator =(other);
    }
    return *this;
}

bool TYPointCalcul::operator==(const TYPointCalcul& other) const
{
    if (this != &other)
    {
        if (TYPoint::operator !=(other)) { return false; }
    }
    return true;
}

bool TYPointCalcul::operator!=(const TYPointCalcul& other) const
{
    return !operator==(other);
}

bool TYPointCalcul::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYPoint::deepCopy(pOther, copyId)) { return false; }

    TYPointCalcul* pOtherPtCalcul = (TYPointCalcul*) pOther;

    // Copie les donnees de l'ensemble des spectres existants
    copySpectres(pOtherPtCalcul);
    TYMapIdSpectre::iterator it_s;
    for (it_s = pOtherPtCalcul->_tabSpectre.begin(); it_s != pOtherPtCalcul->_tabSpectre.end(); ++it_s)
    {
        _tabSpectre[it_s->first]->deepCopy((TYElement*) it_s->second, copyId);
    }

    copyEtats(pOtherPtCalcul);

    _dBA = pOtherPtCalcul->_dBA;
    _dBLin = pOtherPtCalcul->_dBLin;

    _isSpectreUnique = pOtherPtCalcul->_isSpectreUnique;

    return true;
}


void TYPointCalcul::copySpectres(TYPointCalcul* pOther)
{
    TYMapIdSpectre::iterator it_s;
    for (it_s = _tabSpectre.begin(); it_s != _tabSpectre.end(); ++it_s)
    {
        delete it_s->second;
    }
    _tabSpectre.clear();
    for (it_s = pOther->_tabSpectre.begin(); it_s != pOther->_tabSpectre.end(); ++it_s)
    {
        TYSpectre* pSpectre = _tabSpectre[it_s->first];
        if (pSpectre == NULL)
        {
            pSpectre = new TYSpectre();
            pSpectre->setParent(this);
        }
        *pSpectre = *(it_s->second);
        _tabSpectre[it_s->first] = pSpectre;
    }
}

void TYPointCalcul::copyEtats(TYPointCalcul* pOther)
{
    _tabEtats.clear(); //On vide la map actuelle

    TYMapIdBool::iterator it_b;
    for (it_b = pOther->_tabEtats.begin(); it_b != pOther->_tabEtats.end(); ++it_b)
    {
        _tabEtats[it_b->first] = it_b->second;
    }
}

void TYPointCalcul::duplicateEtat(const QString& idCalculRef, const QString& idCalculNew)
{
    _tabEtats[idCalculNew] = _tabEtats[idCalculRef];
}

std::string TYPointCalcul::toString() const
{
    std::string str = "TYPointCalcul: X=" + doubleToStr(_x) + " Y=" + doubleToStr(_y) + " Z=" + doubleToStr(_z);
    return str;
}

DOM_Element TYPointCalcul::toXML(DOM_Element& domElement)
{
    // On surcharge completement depuis TYElement et non pas TYPoint
    // car ce dernier ne conserve pas l'id
    DOM_Element domNewElem = TYElement::toXML(domElement);

    domNewElem.setAttribute("x", doubleToStrPre(_x, 3).data());
    domNewElem.setAttribute("y", doubleToStrPre(_y, 3).data());
    domNewElem.setAttribute("z", doubleToStrPre(_z, 3).data());

    // Creation de la liste des etats des points de controle en fonction du calcul
    // Ajoute un noeud pour l'etat des points de calcul
    DOM_Document domDoc = domElement.ownerDocument();

    TYMapIdBool::iterator it_b;
    for (it_b = _tabEtats.begin(); it_b != _tabEtats.end(); ++it_b)
    {
        DOM_Element tmpNode = domDoc.createElement("etatCalcul");
        domNewElem.appendChild(tmpNode);

        tmpNode.setAttribute("idCalcul", it_b->first);
        tmpNode.setAttribute("Etat", QString(intToStr(_tabEtats[it_b->first]).c_str()));
    }

    if (TYProjet::gSaveValues)
    {
        // sauvegarde de l'ensemble des spectres
        TYMapIdSpectre::iterator it_s;
        for (it_s = _tabSpectre.begin(); it_s != _tabSpectre.end(); ++it_s)
        {
            TYSpectre* pSpectre = _tabSpectre[it_s->first];
            if (pSpectre != NULL)
            {
                DOM_Element spectreElem = pSpectre->toXML(domNewElem);
                QString idCalcul = it_s->first;
                spectreElem.setAttribute("idCalcul", idCalcul);
            }
        }
    }

    return domNewElem;
}

int TYPointCalcul::fromXML(DOM_Element domElement)
{
    // On surcharge completement depuis TYElement et non pas TYPoint
    // car ce dernier ne conserve pas l'id
    TYElement::fromXML(domElement);

    _x = TYXMLTools::getElementAttributeToDouble(domElement, "x");
    _y = TYXMLTools::getElementAttributeToDouble(domElement, "y");
    _z = TYXMLTools::getElementAttributeToDouble(domElement, "z");

    DOM_Element elemCur;

    bool bOldEtat = false;
    QString idCalcul = "";
    int state;

    QDomNodeList childs = domElement.childNodes();
    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        if (elemCur.nodeName() == "state")
        {
            bOldEtat = TYXMLTools::getElementIntValue(elemCur, "state", state);

        }
        else if (elemCur.nodeName() == "etatCalcul")
        {
            idCalcul = TYXMLTools::getElementAttributeToString(elemCur, "idCalcul");
            bool bEtat = TYXMLTools::getElementAttributeToInt(elemCur, "Etat");
            _tabEtats[idCalcul] = bEtat;
        }
        else if (elemCur.nodeName() == "Spectre")
        {
            TYSpectre* pSpectre = new TYSpectre();
            pSpectre->setParent(this);
            pSpectre->callFromXMLIfEqual(elemCur);

            // recupere le calcul associe au spectre
            idCalcul = TYXMLTools::getElementAttributeToString(elemCur, "idCalcul");
            _tabSpectre[idCalcul] = pSpectre;

            _dBA = pSpectre->valGlobDBA();
            _dBLin = pSpectre->valGlobDBLin();
        }

        // Cree et remplit le tableau des etats des points de controle si non existant
        if (bOldEtat && (idCalcul != ""))
        {
            _tabEtats[idCalcul] = !(state > 0);
        }
    }

    setIsAcousticModified(true);

    return 1;
}

void TYPointCalcul::setSpectre(const TYSpectre& spectre, TYCalcul* pCalcul/* = NULL*/)
{
    TYSpectre* pSpectre = getSpectre(pCalcul, true);
    *pSpectre = spectre;
    _dBA = pSpectre->valGlobDBA();
    _dBLin = pSpectre->valGlobDBLin();
    setIsAcousticModified(true);
}

TYSpectre* TYPointCalcul::getSpectre(TYCalcul* pCalcul, const bool& addToCalcul)
{
    TYSpectre* pSpectre = NULL;
    QString idCalcul = "";

    if (_tabSpectre.size() == 1 && _isSpectreUnique)
    {
        // renvoit par defaut le spectre du 1er calcul disponible (cas des maillages)
        idCalcul = _tabSpectre.begin()->first;
    }
    else if (pCalcul != NULL)
    {
        idCalcul = pCalcul->getID().toString();
    }

    TYMapIdSpectre::iterator it = _tabSpectre.find(idCalcul);
    if (it == _tabSpectre.end())
    {
        pSpectre = new TYSpectre();
        pSpectre->setValid(false);

        if (addToCalcul)
        {
            pSpectre->setValid(true);
            _tabSpectre[idCalcul] = pSpectre;
        }
    }
    else
    {
        pSpectre = (*it).second;

        if (pSpectre == NULL)
        {
            // 1er acces au spectre --> Creation
            pSpectre = new TYSpectre();
            pSpectre->setValid(false); // Le spectre vide est invalide
        }
    }

    return pSpectre;
}

void TYPointCalcul::setEtat(const bool& etat, TYCalcul* pCalcul)
{
    if (!pCalcul) { return; }

    QString idCalcul = pCalcul->getID().toString();

    _tabEtats[idCalcul] = etat;
}

bool TYPointCalcul::getEtat(TYCalcul* pCalcul)
{
    QString idCalcul = "";

    if (pCalcul == NULL)
    {
        if (_tabSpectre.size() > 0)
        {
            // renvoit par defaut l'etat du point pour le 1er calcul disponible (cas des maillages)
            idCalcul = _tabEtats.begin()->first;
        }
    }
    else
    {
        idCalcul = pCalcul->getID().toString();
    }

    return _tabEtats[idCalcul];
}

TYPoint TYPointCalcul::getCoordSIG()
{
    TYPoint retPoint;
    LPTYMaillage pMail = TYMaillage::safeDownCast(getParent());
    LPTYMaillageGeoNode pMailNode = NULL;
    LPTYCalcul pCalcul = NULL;
    OMatrix matrix;

    // Recuperation de la matrice de changement de repere du maillage
    if (pMail)
    {
        pCalcul = TYCalcul::safeDownCast(pMail->getParent());
        if (pCalcul)
        {
            pMailNode = pCalcul->findMaillage(pMail);
            if (pMailNode) { matrix = pMailNode->getMatrix(); }
        }
    }

    retPoint = matrix * (*this);

    // Passage dans le repere du SIG
    LPTYSiteNode pSite = NULL;
    if (pCalcul) { pSite = pCalcul->getSite(); }
    if (pSite)
    {
        retPoint._x += pSite->getSIG_X();
        retPoint._y += pSite->getSIG_Y();
    }

    return retPoint;
}
