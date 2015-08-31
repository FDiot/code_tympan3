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

#include "Tympan/core/logging.h"
#include "Tympan/models/business/TYProjet.h"
#include "Tympan/models/business/TYCalcul.h"
#include "Tympan/models/business/TYMaillage.h"
#include "TYPointCalcul.h"
#if TY_USE_IHM
  #include "Tympan/gui/widgets/TYPointCalculWidget.h"
#endif


TY_EXTENSION_INST(TYPointCalcul);

TYPointCalcul::TYPointCalcul() : TYPoint(true), _spectre(nullptr)
{
    _name = TYNameManager::get()->generateName(getClassName());
}

TYPointCalcul::TYPointCalcul(const TYPoint& other) : TYPoint(other, true)
{
}

TYPointCalcul::TYPointCalcul(const TYPointCalcul& other) : TYPoint(true)
{
    *this = other;
}

TYPointCalcul::~TYPointCalcul()
{
}

TYPointCalcul& TYPointCalcul::operator=(TYPointCalcul& other)
{
    if (this != &other)
    {
        TYPoint::operator =(other);

        _spectre = new TYSpectre(*other._spectre);
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

    if (pOtherPtCalcul->getSpectre())
    {
        _spectre = new TYSpectre();
        _spectre->deepCopy(pOtherPtCalcul->getSpectre(), false);
    }

    return true;
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
    //DOM_Document domDoc = domElement.ownerDocument();

    //TYMapIdBool::iterator it_b;
    //for (it_b = _tabEtats.begin(); it_b != _tabEtats.end(); ++it_b)
    //{
    //    DOM_Element tmpNode = domDoc.createElement("etatCalcul");
    //    domNewElem.appendChild(tmpNode);

    //    tmpNode.setAttribute("idCalcul", it_b->first);
    //    tmpNode.setAttribute("Etat", QString(intToStr(_tabEtats[it_b->first]).c_str()));
    //}

    //if (TYProjet::gSaveValues)
    //{
    //    // sauvegarde de l'ensemble des spectres
    //    TYMapIdSpectre::iterator it_s;
    //    for (it_s = _tabSpectre.begin(); it_s != _tabSpectre.end(); ++it_s)
    //    {
    //        TYSpectre* pSpectre = _tabSpectre[it_s->first];
    //        if (pSpectre != NULL)
    //        {
    //            DOM_Element spectreElem = pSpectre->toXML(domNewElem);
    //            QString idCalcul = it_s->first;
    //            spectreElem.setAttribute("idCalcul", idCalcul);
    //        }
    //    }
    //}

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
    //QString idCalcul = "";
    int state;

    QDomNodeList childs = domElement.childNodes();
    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        if (elemCur.nodeName() == "state")
        {
            bOldEtat = TYXMLTools::getElementIntValue(elemCur, "state", state);

        }
        //else if (elemCur.nodeName() == "etatCalcul")
        //{
        //    idCalcul = TYXMLTools::getElementAttributeToString(elemCur, "idCalcul");
        //    bool bEtat = TYXMLTools::getElementAttributeToInt(elemCur, "Etat");
        //    _tabEtats[idCalcul] = bEtat;
        //}
        //else if (elemCur.nodeName() == "Spectre")
        //{
        //    TYSpectre* pSpectre = new TYSpectre();
        //    pSpectre->setParent(this);
        //    pSpectre->callFromXMLIfEqual(elemCur);

        //    // recupere le calcul associe au spectre
        //    idCalcul = TYXMLTools::getElementAttributeToString(elemCur, "idCalcul");
        //    _tabSpectre[idCalcul] = pSpectre;

        //    _dBA = pSpectre->valGlobDBA();
        //    _dBLin = pSpectre->valGlobDBLin();
        //}

        // Cree et remplit le tableau des etats des points de controle si non existant
        //if (bOldEtat && (idCalcul != ""))
        //{
        //    _tabEtats[idCalcul] = !(state > 0);
        //}
    }

    setIsAcousticModified(true);

    return 1;
}

//void TYPointCalcul::setSpectre(const TYSpectre& spectre, TYCalcul* pCalcul/* = NULL*/)
//{
//    TYSpectre* pSpectre = getSpectre(pCalcul, true);
//    *pSpectre = spectre;
//    _dBA = pSpectre->valGlobDBA();
//    _dBLin = pSpectre->valGlobDBLin();
//    setIsAcousticModified(true);
//}
//
//TYSpectre* TYPointCalcul::getSpectre(TYCalcul* pCalcul, const bool& addToCalcul)
//{
//    TYSpectre* pSpectre = NULL;
//    QString idCalcul = "";
//
//    if (_tabSpectre.size() == 1 && _isSpectreUnique)
//    {
//        // renvoit par defaut le spectre du 1er calcul disponible (cas des maillages)
//        idCalcul = _tabSpectre.begin()->first;
//    }
//    else if (pCalcul != NULL)
//    {
//        idCalcul = pCalcul->getID().toString();
//    }
//
//    TYMapIdSpectre::iterator it = _tabSpectre.find(idCalcul);
//    if (it == _tabSpectre.end())
//    {
//        pSpectre = new TYSpectre();
//        pSpectre->setValid(false);
//
//        if (addToCalcul)
//        {
//            pSpectre->setValid(true);
//            _tabSpectre[idCalcul] = pSpectre;
//        }
//    }
//    else
//    {
//        pSpectre = (*it).second;
//
//        if (pSpectre == NULL)
//        {
//            // 1er acces au spectre --> Creation
//            pSpectre = new TYSpectre();
//            pSpectre->setValid(false); // Le spectre vide est invalide
//        }
//    }
//
//    return pSpectre;
//}


TYPoint TYPointCalcul::getCoordSIG()
{
    TYPoint retPoint;
    LPTYMaillage pMail = TYMaillage::safeDownCast(getParent());
    LPTYMaillageGeoNode pMailNode = NULL;
    LPTYProjet pProjet = dynamic_cast<TYProjet*>( pMail->getParent() );
    OMatrix matrix;

    // Recuperation de la matrice de changement de repere du maillage
    if (pMail && pProjet)
    {
        pMailNode = pProjet->findMaillage(pMail);
        if (pMailNode) { matrix = pMailNode->getMatrix(); }
    }

    retPoint = matrix * (*this);

    // Passage dans le repere du SIG
    LPTYSiteNode pSite = NULL;
    if (pProjet) { pSite = pProjet->getSite(); }
    if (pSite)
    {
        retPoint._x += pSite->getSIG_X();
        retPoint._y += pSite->getSIG_Y();
    }

    return retPoint;
}
