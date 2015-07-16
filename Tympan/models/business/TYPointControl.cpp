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
#include "Tympan/models/business/TYCalcul.h"
#include "Tympan/models/business/TYProjet.h"
#include "Tympan/models/business/TYPreferenceManager.h"
#include "Tympan/models/business/infrastructure/TYSiteNode.h"
#include "TYPointControl.h"

#if TY_USE_IHM
  #include "Tympan/gui/widgets/TYPointControlWidget.h"
  #include "Tympan/gui/gl/TYPointControlGraphic.h"
#endif


TY_EXTENSION_INST(TYPointControl);
TY_EXT_GRAPHIC_INST(TYPointControl);

TYPointControl::TYPointControl() :  _object(0),
    _statusSIG(false)
{
    _name = TYNameManager::get()->generateName(getClassName());

    // Par defaut : Un point de controle est bleu
    float r = 85.0f, g = 0.0f, b = 255.0f;

#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "PointControlGraphicColorR"))
    {
        TYPreferenceManager::getColor(TYDIRPREFERENCEMANAGER, "PointControlGraphicColor", r, g, b);
    }
    else
    {
        TYPreferenceManager::setColor(TYDIRPREFERENCEMANAGER, "PointControlGraphicColor", r, g, b);
    }
#endif
    OColor color;
    color.r = r / 255;
    color.g = g / 255;
    color.b = b / 255;

    setColor(color);

    _hauteur = 2.0;
}

TYPointControl::TYPointControl(const TYPoint& other) :
    TYPointCalcul(other)
{
    _hauteur = 2.0;
    _object = 0;
}

TYPointControl::TYPointControl(const TYPointControl& other)
{
    *this = other;
}

TYPointControl::~TYPointControl()
{
}

TYPointControl& TYPointControl::operator=(const TYPointControl& other)
{
    if (this != &other)
    {
        TYPointCalcul::operator =(other);
        TYColorInterface::operator =(other);
        copyEtats( const_cast<TYPointControl*>(&other) );// Copie les etats
        _hauteur = other._hauteur;
        _object = other._object;
        _statusSIG = other._statusSIG;
    }
    return *this;
}

TYPointControl& TYPointControl::operator=(const TYPoint& other)
{
    if (this != &other)
    {
        TYPointCalcul::operator =(other);
    }
    return *this;
}

bool TYPointControl::operator==(const TYPointControl& other) const
{
    if (this != &other)
    {
        if (TYPointCalcul::operator !=(other)) { return false; }
        if (TYColorInterface::operator !=(other)) { return false; }
        if (_hauteur != other._hauteur) { return false; }
        if (_object != other._object) { return false; }
        if (_statusSIG != other._statusSIG) { return false; }
    }
    return true;
}

bool TYPointControl::operator!=(const TYPointControl& other) const
{
    return !operator==(other);
}

bool TYPointControl::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYPointCalcul::deepCopy(pOther, copyId)) { return false; }

    TYPointControl* pOtherPtControl = (TYPointControl*) pOther;
    copyEtats(pOtherPtControl);

    _hauteur = pOtherPtControl->_hauteur;
    _object = pOtherPtControl->_object;
    _statusSIG = pOtherPtControl->_statusSIG;

    return true;
}

std::string TYPointControl::toString() const
{
    std::string str = "TYPointControl: X=" + doubleToStr(_x) + " Y=" + doubleToStr(_y) + " Z=" + doubleToStr(_z) + " Hauteur=" + doubleToStr(_hauteur);
    return str;
}

DOM_Element TYPointControl::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYPointCalcul::toXML(domElement);
    TYColorInterface::toXML(domNewElem);

    TYXMLTools::addElementStringValue(domNewElem, "hauteur", doubleToStrPre(_hauteur, 3).data());
    TYXMLTools::addElementIntValue(domNewElem, "formeObjet", _object);

    // Ajoute un noeud pour l'etat des points de calcul
    DOM_Document domDoc = domElement.ownerDocument();

    TYMapIdBool::iterator it_b;
    for (it_b = _tabEtats.begin(); it_b != _tabEtats.end(); ++it_b)
    {
        DOM_Element tmpNode = domDoc.createElement("etatCalcul");
        domNewElem.appendChild(tmpNode);

        tmpNode.setAttribute("idCalcul", it_b->first.toString());
        tmpNode.setAttribute("Etat", QString(intToStr(_tabEtats[it_b->first]).c_str()));
    }

    return domNewElem;
}

int TYPointControl::fromXML(DOM_Element domElement)
{
    TYPointCalcul::fromXML(domElement);
    TYColorInterface::fromXML(domElement);

    bool hauteurOk = false;
    bool formeObjetOk = false;
    bool bOldDatas = false;
    TYUUID idCalcul;
    std::map<TYUUID, LPTYSpectre> *compatibilityVector = new std::map<TYUUID, LPTYSpectre>();

    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();
    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        TYXMLTools::getElementDoubleValue(elemCur, "hauteur", _hauteur, hauteurOk);
        TYXMLTools::getElementIntValue(elemCur, "formeObjet", _object, formeObjetOk);
        
        if (elemCur.nodeName() == "etatCalcul")
        {
            QString strIdCalcul = TYXMLTools::getElementAttributeToString(elemCur, "idCalcul");
            idCalcul.FromString(strIdCalcul);
            bool bEtat = TYXMLTools::getElementAttributeToInt(elemCur, "Etat");
            _tabEtats[idCalcul] = bEtat;
        }
        else if (elemCur.nodeName() == "Spectre")
        {
            bOldDatas = true;
            LPTYSpectre pSpectre = new TYSpectre();
            pSpectre->callFromXMLIfEqual(elemCur);

            // recupere le calcul associe au spectre
            QString strId = TYXMLTools::getElementAttributeToString(elemCur, "idCalcul");
            idCalcul.FromString( strId );
            compatibilityVector->operator[](idCalcul) = pSpectre;

            _dBA = pSpectre->valGlobDBA();
            _dBLin = pSpectre->valGlobDBLin();
        }
    }

    if (bOldDatas == true)
    {
        // Cleaning compatibility data
        std::map<TYUUID, LPTYSpectre>::iterator it;
        for (it=compatibilityVector->begin(); it!=compatibilityVector->end(); )
        {
            if ( _tabEtats[(*it).first] == false)
            {
                it = compatibilityVector->erase( it );
            }
            else
            {
                it++;
            }
        }

        setAllUses( (void*)compatibilityVector );
    }
    else
    {
        delete compatibilityVector;
    }

    return 1;
}


//void TYPointControl::purge(TYCalcul* pCalcul)
//{
//    if (pCalcul == NULL)
//    {
//        TYMapIdSpectre::iterator it_s;
//        for (it_s = _tabSpectre.begin(); it_s != _tabSpectre.end(); ++it_s)
//        {
//            delete it_s->second;
//        }
//        _tabSpectre.clear();
//    }
//    else
//    {
//        _tabSpectre[pCalcul->getID().toString()] = new TYSpectre();
//        _tabSpectre[pCalcul->getID().toString()]->setType(SPECTRE_TYPE_LP);
//    }
//}

void TYPointControl::toSIG()
{
    TYProjet* pProjet = TYProjet::safeDownCast(getParent());
    TYSiteNode* pSite = NULL;
    if (pProjet)
    {
        pSite = pProjet->getSite();
    }

    if (pSite && !_statusSIG)
    {
        _x += pSite->getSIG_X();
        _y += pSite->getSIG_Y();
        _statusSIG = true;
    }
}

void TYPointControl::fromSIG()
{
    TYProjet* pProjet = TYProjet::safeDownCast(getParent());
    TYSiteNode* pSite = NULL;
    if (pProjet)
    {
        pSite = pProjet->getSite();
    }


    if (pSite && _statusSIG)
    {
        _x -= pSite->getSIG_X();
        _y -= pSite->getSIG_Y();
        _statusSIG = false;
    }
}

int TYPointControl::getSIGType()
{
    TYCalcul* pCalcul = TYCalcul::safeDownCast(getParent());
    LPTYSiteNode pSite = NULL;
    if (pCalcul) { pSite = pCalcul->getSite(); }

    if (pSite) { return pSite->getSIGType(); }

    return 0;
}

void TYPointControl::setEtat(const TYUUID& id_calc, bool etat)
{
    _tabEtats[id_calc] = etat;
}

bool TYPointControl::etat()
{
    TYUUID id_calc = dynamic_cast<TYProjet*>(getParent())->getCurrentCalcul()->getID();

    return etat(id_calc);
}

bool TYPointControl::etat(const TYUUID& id_calc)
{
    // Constrol point knows the calcul
    TYMapIdBool::iterator it = _tabEtats.find(id_calc);
    if ( it != _tabEtats.end() )
    {
        return (*it).second;
    }
    else
    {
        _tabEtats[id_calc] = false;
    }

    return false;
}

bool TYPointControl::etat(const TYCalcul* pCalc)
{
    assert(pCalc != nullptr);
    return etat( pCalc->getID() );
}

void TYPointControl::copyEtats(TYPointControl* pOther)
{
    _tabEtats.clear(); //On vide la map actuelle

    TYMapIdBool::iterator it_b;
    for (it_b = pOther->_tabEtats.begin(); it_b != pOther->_tabEtats.end(); ++it_b)
    {
        _tabEtats[it_b->first] = it_b->second;
    }
}

void TYPointControl::duplicateEtat(const TYUUID& idCalculRef, const TYUUID& idCalculNew)
{
    _tabEtats[idCalculNew] = _tabEtats[idCalculRef];
}

LPTYSpectre TYPointControl::getSpectre()
{
    return dynamic_cast<TYProjet*>(getParent())->getCurrentCalcul()->getSpectre(this);
}