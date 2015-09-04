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

#if TY_USE_IHM
#include "Tympan/gui/widgets/TYInfrastructureWidget.h"
#include "Tympan/gui/gl/TYInfrastructureGraphic.h"
#endif


#include "Tympan/core/logging.h"
#include "Tympan/models/business/TYProgressManager.h"
#include "Tympan/models/business/TYProjet.h"
#include "Tympan/models/business/TYCalcul.h"
#include "Tympan/models/business/infrastructure/TYSiteNode.h"
#include "TYInfrastructure.h"

TY_EXTENSION_INST(TYInfrastructure);
TY_EXT_GRAPHIC_INST(TYInfrastructure);

TYInfrastructure::TYInfrastructure()
{
    _name = TYNameManager::get()->generateName(getClassName());
}

TYInfrastructure::TYInfrastructure(const TYInfrastructure& other)
{
    *this = other;
}

TYInfrastructure::~TYInfrastructure()
{
    //  purge();
}

TYInfrastructure& TYInfrastructure::operator=(const TYInfrastructure& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        _listBatiment = other._listBatiment;
        _listMachine = other._listMachine;
        _listResTrans = other._listResTrans;
        _listRoute = other._listRoute;
        _listSrc = other._listSrc;
    }
    return *this;
}

bool TYInfrastructure::operator==(const TYInfrastructure& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        if (!(_listBatiment == other._listBatiment)) { return false; }
        if (!(_listMachine == other._listMachine)) { return false; }
        if (!(_listResTrans == other._listResTrans)) { return false; }
        if (!(_listRoute == other._listRoute)) { return false; }
        if (!(_listSrc == other._listSrc)) { return false; }
    }
    return true;
}

bool TYInfrastructure::operator!=(const TYInfrastructure& other) const
{
    return !operator==(other);
}

bool TYInfrastructure::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    TYInfrastructure* pOtherInfra = (TYInfrastructure*) pOther;

    unsigned int i;

    _listRoute.clear();
    for (i = 0; i < pOtherInfra->_listRoute.size(); i++)
    {
        LPTYRouteGeoNode pRouteGeoNode = new TYRouteGeoNode(NULL, this);
        pRouteGeoNode->deepCopy(pOtherInfra->_listRoute[i], copyId);
        pRouteGeoNode->getElement()->setParent(this);
        pRouteGeoNode->setParent(this);
        addRoute(pRouteGeoNode);
    }

    _listResTrans.clear();
    for (i = 0; i < pOtherInfra->_listResTrans.size(); i++)
    {
        LPTYReseauTransportGeoNode pResTransGeoNode = new TYReseauTransportGeoNode(NULL, this);
        pResTransGeoNode->deepCopy(pOtherInfra->_listResTrans[i], copyId);
        pResTransGeoNode->getElement()->setParent(this);
        pResTransGeoNode->setParent(this);
        addResTrans(pResTransGeoNode);
    }

    _listBatiment.clear();
    for (i = 0; i < pOtherInfra->_listBatiment.size(); i++)
    {
        LPTYBatimentGeoNode pBatGeoNode = new TYBatimentGeoNode(NULL, this);
        pBatGeoNode->deepCopy(pOtherInfra->_listBatiment[i], copyId);
        pBatGeoNode->getElement()->setParent(this);
        pBatGeoNode->setParent(this);
        addBatiment(pBatGeoNode);
    }

    _listMachine.clear();
    for (i = 0; i < pOtherInfra->_listMachine.size(); i++)
    {
        LPTYMachineGeoNode pMachineGeoNode = new TYMachineGeoNode(NULL, this);
        pMachineGeoNode->deepCopy(pOtherInfra->_listMachine[i], copyId);
        pMachineGeoNode->getElement()->setParent(this);
        pMachineGeoNode->setParent(this);
        addMachine(pMachineGeoNode);
    }

    _listSrc.clear();
    for (i = 0; i < pOtherInfra->_listSrc.size(); i++)
    {
        LPTYSourcePonctuelleGeoNode pSrcGeoNode = new TYSourcePonctuelleGeoNode(NULL, this);
        pSrcGeoNode->deepCopy(pOtherInfra->_listSrc[i], copyId);
        pSrcGeoNode->getElement()->setParent(this);
        pSrcGeoNode->setParent(this);
        addSrc(pSrcGeoNode);
    }

    return true;
}

std::string TYInfrastructure::toString() const
{
    return "TYInfrastructure";
}

DOM_Element TYInfrastructure::toXML(DOM_Element& domElement)
{
    unsigned int i = 0;

    DOM_Element domNewElem = TYElement::toXML(domElement);

    DOM_Document domDoc = domElement.ownerDocument();
    DOM_Element listRouteNode = domDoc.createElement("ListRoute");
    domNewElem.appendChild(listRouteNode);
    DOM_Element listResTransNode = domDoc.createElement("ListResTrans");
    domNewElem.appendChild(listResTransNode);
    DOM_Element listBatimentNode = domDoc.createElement("ListBatiment");
    domNewElem.appendChild(listBatimentNode);
    DOM_Element listMachineNode = domDoc.createElement("ListMachine");
    domNewElem.appendChild(listMachineNode);
    DOM_Element listSrcNode = domDoc.createElement("ListSource");
    domNewElem.appendChild(listSrcNode);

    for (i = 0; i < _listRoute.size(); i++)
    {
        // Ajout de la route
        _listRoute[i]->toXML(listRouteNode);
    }

    for (i = 0; i < _listResTrans.size(); i++)
    {
        // Ajout du reseau de transport
        _listResTrans[i]->toXML(listResTransNode);
    }

    for (i = 0; i < _listBatiment.size(); i++)
    {
        // Ajout du bi¿½timent si valide (i.e. contient des etages)
        LPTYAcousticVolumeNode pNode = TYAcousticVolumeNode::safeDownCast(_listBatiment[i]->getElement());

        if (pNode && pNode->getNbChild() > 0) { _listBatiment[i]->toXML(listBatimentNode); }
    }

    for (i = 0; i < _listMachine.size(); i++)
    {
        // Ajout de la machine si valide (i.e. contient des volumes)
        LPTYAcousticVolumeNode pNode = TYAcousticVolumeNode::safeDownCast(_listMachine[i]->getElement());

        if (pNode && pNode->getNbChild() > 0) { _listMachine[i]->toXML(listMachineNode); }
    }

    for (i = 0; i < _listSrc.size(); i++)
    {
        // Ajout de la source
        _listSrc[i]->toXML(listSrcNode);
    }

    return domNewElem;
}

int TYInfrastructure::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);

    // Reset
    clean();

    LPTYRouteGeoNode pRouteGeoNode = new TYRouteGeoNode(NULL, this);
    LPTYReseauTransportGeoNode pResTransGeoNode = new TYReseauTransportGeoNode(NULL, this);
    LPTYBatimentGeoNode pBatGeoNode = new TYBatimentGeoNode(NULL, this);
    LPTYMachineGeoNode pMachineGeoNode = new TYMachineGeoNode(NULL, this);
    LPTYUserSourcePonctuelleGeoNode pSrcGeoNode = new TYUserSourcePonctuelleGeoNode(NULL, this);

    unsigned int i = 0, j = 0;
    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();
    unsigned int childcount = childs.length();
    for (i = 0; i < childcount; i++)
    {
        elemCur = childs.item(i).toElement();

        OMessageManager::get()->info("Charge element d'infrastructure %d/%d.", i + 1, childcount);

        if (elemCur.nodeName() == "ListRoute")
        {
            QDomNodeList childs2 = elemCur.childNodes();
            for (j = 0; j < childs2.length(); j++)
            {
                DOM_Element elemCur2 = childs2.item(j).toElement();
                if (pRouteGeoNode->callFromXMLIfEqual(elemCur2))
                {
                    addRoute(pRouteGeoNode);
                    pRouteGeoNode = new TYRouteGeoNode(NULL, this);
                }
            }
        }
        else if (elemCur.nodeName() == "ListResTrans")
        {
            QDomNodeList childs2 = elemCur.childNodes();

            for (j = 0; j < childs2.length(); j++)
            {
                DOM_Element elemCur2 = childs2.item(j).toElement();
                if (pResTransGeoNode->callFromXMLIfEqual(elemCur2))
                {
                    addResTrans(pResTransGeoNode);
                    pResTransGeoNode = new TYReseauTransportGeoNode(NULL, this);
                }
            }
        }
        else if (elemCur.nodeName() == "ListBatiment")
        {
            QDomNodeList childs2 = elemCur.childNodes();
            unsigned int childcount = childs2.length();
            for (j = 0; j < childcount; j++)
            {
                DOM_Element elemCur2 = childs2.item(j).toElement();
                OMessageManager::get()->info("Charge batiment %d/%d.", j + 1, childcount);
                if (pBatGeoNode->callFromXMLIfEqual(elemCur2))
                {
                    addBatiment(pBatGeoNode);
                    pBatGeoNode = new TYBatimentGeoNode(NULL, this);
                }
            }
        }
        else if (elemCur.nodeName() == "ListMachine")
        {
            QDomNodeList childs2 = elemCur.childNodes();
            unsigned int childcount = childs2.length();
            for (j = 0; j < childcount; j++)
            {
                DOM_Element elemCur2 = childs2.item(j).toElement();

                OMessageManager::get()->info("Charge machine %d/%d.", j + 1, childcount);
                if (pMachineGeoNode->callFromXMLIfEqual(elemCur2))
                {
                    addMachine(pMachineGeoNode);
                    pMachineGeoNode = new TYMachineGeoNode(NULL, this);
                }
            }
        }
        else if (elemCur.nodeName() == "ListSource")
        {
            QDomNodeList childs2 = elemCur.childNodes();

            for (j = 0; j < childs2.length(); j++)
            {
                DOM_Element elemCur2 = childs2.item(j).toElement();

                if (pSrcGeoNode->callFromXMLIfEqual(elemCur2))
                {
                    if (pSrcGeoNode && pSrcGeoNode->getElement()->isA("TYSourcePonctuelle"))
                    {
                        // Si ancien fichier, conversion en "TYUserSourcePnctuelle"
                        LPTYSourcePonctuelle pSrc = TYSourcePonctuelle::safeDownCast(pSrcGeoNode->getElement());
                        LPTYUserSourcePonctuelle pUSrc = new TYUserSourcePonctuelle(pSrc);
                        pSrcGeoNode->setElement((TYElement*) pUSrc);
                    }
                    addSrc(pSrcGeoNode);
                    pSrcGeoNode = new TYUserSourcePonctuelleGeoNode(NULL, this);
                }
            }
        }
    }

    return 1;
}

void TYInfrastructure::getChilds(LPTYElementArray& childs, bool recursif /*=true*/)
{
    unsigned int i;

    TYElement::getChilds(childs, recursif);

    for (i = 0; i < _listRoute.size(); i++)
    {
        childs.push_back(_listRoute[i]);
        childs.push_back(_listRoute[i]->getElement());
    }

    for (i = 0; i < _listResTrans.size(); i++)
    {
        childs.push_back(_listResTrans[i]);
        childs.push_back(_listResTrans[i]->getElement());
    }

    for (i = 0; i < _listBatiment.size(); i++)
    {
        childs.push_back(_listBatiment[i]);
        childs.push_back(_listBatiment[i]->getElement());
    }

    for (i = 0; i < _listMachine.size(); i++)
    {
        childs.push_back(_listMachine[i]);
        childs.push_back(_listMachine[i]->getElement());
    }

    for (i = 0; i < _listSrc.size(); i++)
    {
        childs.push_back(_listSrc[i]);
        childs.push_back(_listSrc[i]->getElement());
    }

    if (recursif)
    {
        for (i = 0; i < _listRoute.size(); i++)
        {
            _listRoute[i]->getChilds(childs, recursif);
        }

        for (i = 0; i < _listResTrans.size(); i++)
        {
            _listResTrans[i]->getChilds(childs, recursif);
        }

        for (i = 0; i < _listBatiment.size(); i++)
        {
            _listBatiment[i]->getChilds(childs, recursif);
        }

        for (i = 0; i < _listMachine.size(); i++)
        {
            _listMachine[i]->getChilds(childs, recursif);
        }

        for (i = 0; i < _listSrc.size(); i++)
        {
            _listSrc[i]->getChilds(childs, recursif);
        }
    }
}

void TYInfrastructure::updateCurrentCalcul(TYListID& listID, bool recursif)//=true
{
    if (recursif) // On parcours les enfants si besoin est...
    {
        // Collecte des childs
        LPTYElementArray childs;
        getChilds(childs, false);
        for (int i = 0; i < childs.size(); i++)
        {
            childs[i]->updateCurrentCalcul(listID, recursif);
        }
    }

    TYElement::updateCurrentCalcul(listID, false);
    // l'infrastructure est toujours presente!
    //  setInCurrentCalcul ( true, false );
}

void TYInfrastructure::reparent()
{
    unsigned int i;
    for (i = 0; i < _listRoute.size(); i++)
    {
        _listRoute[i]->setParent(this);
        _listRoute[i]->getElement()->setParent(this);
    }

    for (i = 0; i < _listResTrans.size(); i++)
    {
        _listResTrans[i]->setParent(this);
        _listResTrans[i]->getElement()->setParent(this);
    }

    for (i = 0; i < _listBatiment.size(); i++)
    {
        _listBatiment[i]->setParent(this);
        _listBatiment[i]->getElement()->setParent(this);
    }

    for (i = 0; i < _listMachine.size(); i++)
    {
        _listMachine[i]->setParent(this);
        _listMachine[i]->getElement()->setParent(this);
    }

    for (i = 0; i < _listSrc.size(); i++)
    {
        _listSrc[i]->setParent(this);
        _listSrc[i]->getElement()->setParent(this);
    }
}

TYProjet* TYInfrastructure::getProjet()
{
    if (_pParent)
    {
        TYSiteNode* pSite = TYSiteNode::safeDownCast((TYElement*) _pParent);
        if (pSite) { return pSite->getProjet(); }
    }
    return NULL;
}

void TYInfrastructure::purge()
{
    remAllBatiment();
    remAllMachine();
    remAllResTrans();
    remAllRoute();
    remAllSrc();

    setIsGeometryModified(true);
}

void TYInfrastructure::concatInfra(const TYInfrastructure* infra)
{
    unsigned int i;

    for (i = 0; i < infra->_listRoute.size(); ++i)
    {
        addRoute(infra->_listRoute[i]);
    }

    for (i = 0; i < infra->_listResTrans.size(); ++i)
    {
        addResTrans(infra->_listResTrans[i]);
    }

    for (i = 0; i < infra->_listBatiment.size(); ++i)
    {
        addBatiment(infra->_listBatiment[i]);
    }

    for (i = 0; i < infra->_listMachine.size(); ++i)
    {
        addMachine(infra->_listMachine[i]);
    }

    for (i = 0; i < infra->_listSrc.size(); ++i)
    {
        addSrc(infra->_listSrc[i]);
    }
}

bool TYInfrastructure::addRoute(LPTYRouteGeoNode pRouteGeoNode)
{
    assert(pRouteGeoNode);
    assert(pRouteGeoNode->getElement());

    pRouteGeoNode->setParent(this);
    pRouteGeoNode->getElement()->setParent(this);

    _listRoute.push_back(pRouteGeoNode);

    // Ajoute dans le calcul courant
    addToCalcul(pRouteGeoNode);

    setIsGeometryModified(true);

    return true;
}

bool TYInfrastructure::addRoute(LPTYRoute pRoute)
{
    return addRoute(new TYRouteGeoNode((LPTYElement)pRoute));
}

bool TYInfrastructure::remRoute(const LPTYRouteGeoNode pRouteGeoNode)
{
    assert(pRouteGeoNode);

    bool ret = false;
    TYTabRouteGeoNode::iterator ite;

    TYProjet* pProjet = getProjet();

    for (ite = _listRoute.begin(); ite != _listRoute.end(); ite++)
    {
        if ((*ite) == pRouteGeoNode)
        {

            if (pProjet) { pProjet->remElmtFromCalculs((*ite)->getElement()); } // Supprime l'element des calculs

            _listRoute.erase(ite);
            ret = true;
            break;
        }
    }


    setIsGeometryModified(true);

    return ret;
}

bool TYInfrastructure::remRoute(const LPTYRoute pRoute)
{
    assert(pRoute);
    bool ret = false;
    TYTabRouteGeoNode::iterator ite;

    for (ite = _listRoute.begin(); ite != _listRoute.end(); ite++)
    {
        if (TYRoute::safeDownCast((*ite)->getElement()) == pRoute)
        {
            ret = remRoute(*ite);
            break;
        }
    }

    return ret;
}

bool TYInfrastructure::remRoute(QString idRoute)
{
    bool ret = false;
    TYTabRouteGeoNode::iterator ite;

    for (ite = _listRoute.begin(); ite != _listRoute.end(); ite++)
    {
        if ((*ite)->getElement()->getID().toString() == idRoute)
        {
            ret = remRoute(*ite);
            break;
        }
    }

    return ret;
}

void TYInfrastructure::remAllRoute()
{
    TYProjet* pProjet = getProjet();

    if (pProjet) { pProjet->remTabElmtFromCalculs(_listRoute); }

    _listRoute.clear();
    setIsGeometryModified(true);
}

LPTYRouteGeoNode TYInfrastructure::findRoute(const LPTYRoute pRoute)
{
    assert(pRoute);
    TYTabRouteGeoNode::iterator ite;

    for (ite = _listRoute.begin(); ite != _listRoute.end(); ite++)
    {
        if (TYRoute::safeDownCast((*ite)->getElement()) == pRoute)
        {
            return (*ite);
        }
    }

    return NULL;
}

bool TYInfrastructure::addResTrans(LPTYReseauTransportGeoNode pResTransGeoNode)
{
    assert(pResTransGeoNode);

    LPTYReseauTransport pReseau = TYReseauTransport::safeDownCast(pResTransGeoNode->getElement());

    assert(pReseau);

    pResTransGeoNode->setParent(this);
    pReseau->setParent(this);

    _listResTrans.push_back(pResTransGeoNode);

    // Ajoute au calcul courant
    addToCalcul(pResTransGeoNode);

    setIsGeometryModified(true);

    return true;
}

bool TYInfrastructure::addResTrans(LPTYReseauTransport pResTrans)
{
    return addResTrans(new TYReseauTransportGeoNode((LPTYElement)pResTrans));
}

bool TYInfrastructure::remResTrans(const LPTYReseauTransportGeoNode pResTransGeoNode)
{
    assert(pResTransGeoNode);
    bool ret = false;
    TYTabReseauTransportGeoNode::iterator ite;
    TYProjet* pProjet = getProjet();

    for (ite = _listResTrans.begin(); ite != _listResTrans.end(); ite++)
    {
        if ((*ite) == pResTransGeoNode)
        {
            if (pProjet) { pProjet->remElmtFromCalculs((*ite)->getElement()); } // Supprime l'element des calculs

            _listResTrans.erase(ite);
            ret = true;
            break;
        }
    }

    setIsGeometryModified(true);

    return ret;
}

bool TYInfrastructure::remResTrans(const LPTYReseauTransport pResTrans)
{
    assert(pResTrans);
    bool ret = false;
    TYTabReseauTransportGeoNode::iterator ite;

    for (ite = _listResTrans.begin(); ite != _listResTrans.end(); ite++)
    {
        if (TYReseauTransport::safeDownCast((*ite)->getElement()) == pResTrans)
        {
            ret = remResTrans(*ite);
            break;
        }
    }

    return ret;
}

bool TYInfrastructure::remResTrans(QString idResTrans)
{
    bool ret = false;
    TYTabReseauTransportGeoNode::iterator ite;

    for (ite = _listResTrans.begin(); ite != _listResTrans.end(); ite++)
    {
        if (TYReseauTransport::safeDownCast((*ite)->getElement())->getID().toString() == idResTrans)
        {
            ret = remResTrans(*ite);
            break;
        }
    }

    return ret;
}

void TYInfrastructure::remAllResTrans()
{
    TYProjet* pProjet = getProjet();
    if (pProjet) { pProjet->remTabElmtFromCalculs(_listResTrans); }


    _listResTrans.clear();
    setIsGeometryModified(true);
}

LPTYReseauTransportGeoNode TYInfrastructure::findResTrans(const LPTYReseauTransport pResTrans)
{
    assert(pResTrans);
    TYTabReseauTransportGeoNode::iterator ite;

    for (ite = _listResTrans.begin(); ite != _listResTrans.end(); ite++)
    {
        if (TYReseauTransport::safeDownCast((*ite)->getElement()) == pResTrans)
        {
            return (*ite);
        }
    }

    return NULL;
}

bool TYInfrastructure::addMachine(LPTYMachineGeoNode pMachineGeoNode)
{
    assert(pMachineGeoNode);

    LPTYMachine pMachine = TYMachine::safeDownCast(pMachineGeoNode->getElement());

    assert(pMachine);

    pMachineGeoNode->setParent(this);
    pMachine->setParent(this);

    _listMachine.push_back(pMachineGeoNode);

    // Ajoute au calcul courant
    addToCalcul(pMachineGeoNode);

    setIsGeometryModified(true);

    return true;
}

bool TYInfrastructure::addMachine(LPTYMachine pMachine, const TYRepere& pos)
{
    return addMachine(new TYMachineGeoNode(pos, (LPTYElement)pMachine));
}

bool TYInfrastructure::remMachine(const LPTYMachineGeoNode pMachineGeoNode)
{
    assert(pMachineGeoNode);
    bool ret = false;
    TYTabMachineGeoNode::iterator ite;
    TYProjet* pProjet = getProjet();

    for (ite = _listMachine.begin(); ite != _listMachine.end(); ite++)
    {
        if ((*ite) == pMachineGeoNode)
        {
            if (pProjet) { pProjet->remElmtFromCalculs((*ite)->getElement()); } // Supprime l'element des calculs

            _listMachine.erase(ite);
            ret = true;
            break;
        }
    }

    setIsGeometryModified(true);

    return ret;
}

bool TYInfrastructure::remMachine(const LPTYMachine pMachine)
{
    assert(pMachine);
    bool ret = false;
    TYTabMachineGeoNode::iterator ite;

    for (ite = _listMachine.begin(); ite != _listMachine.end(); ite++)
    {
        if (TYMachine::safeDownCast((*ite)->getElement()) == pMachine)
        {
            ret = remMachine(*ite);
            break;
        }
    }

    return ret;
}

bool TYInfrastructure::remMachine(QString idMachine)
{
    bool ret = false;
    TYTabMachineGeoNode::iterator ite;

    for (ite = _listMachine.begin(); ite != _listMachine.end(); ite++)
    {
        if (TYMachine::safeDownCast((*ite)->getElement())->getID().toString() == idMachine)
        {
            ret = remMachine(*ite);
            break;
        }
    }

    return ret;
}

void TYInfrastructure::remAllMachine()
{
    TYProjet* pProjet = getProjet();
    if (pProjet) { pProjet->remTabElmtFromCalculs(_listMachine); }

    _listMachine.clear();
    setIsGeometryModified(true);
}

LPTYMachineGeoNode TYInfrastructure::findMachine(const LPTYMachine pMachine)
{
    assert(pMachine);
    TYTabMachineGeoNode::iterator ite;

    for (ite = _listMachine.begin(); ite != _listMachine.end(); ite++)
    {
        if (TYMachine::safeDownCast((*ite)->getElement()) == pMachine)
        {
            return (*ite);
        }
    }

    return NULL;
}

bool TYInfrastructure::addBatiment(LPTYBatimentGeoNode pBatimentGeoNode)
{
    assert(pBatimentGeoNode);

    LPTYBatiment pBatiment = TYBatiment::safeDownCast(pBatimentGeoNode->getElement());

    assert(pBatiment);

    pBatimentGeoNode->setParent(this);
    pBatiment->setParent(this);

    _listBatiment.push_back(pBatimentGeoNode);

    // Ajoute au calcul courant s'il existe
    addToCalcul(pBatimentGeoNode);

    setIsGeometryModified(true);

    return true;
}

bool TYInfrastructure::addBatiment(LPTYBatiment pBatiment, const TYRepere& pos)
{
    return addBatiment(new TYBatimentGeoNode(pos, (LPTYElement)pBatiment));
}

bool TYInfrastructure::remBatiment(const LPTYBatimentGeoNode pBatimentGeoNode)
{
    assert(pBatimentGeoNode);

    bool ret = false;
    TYTabBatimentGeoNode::iterator ite;
    TYProjet* pProjet = getProjet();

    for (ite = _listBatiment.begin(); ite != _listBatiment.end(); ite++)
    {
        if ((*ite) == pBatimentGeoNode)
        {
            if (pProjet) { pProjet->remElmtFromCalculs((*ite)->getElement()); } // Supprime l'element des calculs

            _listBatiment.erase(ite);
            ret = true;
            break;
        }
    }

    setIsGeometryModified(true);

    return ret;
}

bool TYInfrastructure::remBatiment(const LPTYBatiment pBatiment)
{
    bool ret = false;
    TYTabBatimentGeoNode::iterator ite;

    for (ite = _listBatiment.begin(); ite != _listBatiment.end(); ite++)
    {
        if (TYBatiment::safeDownCast((*ite)->getElement()) == pBatiment)
        {
            ret = remBatiment(*ite);
            break;
        }
    }

    return ret;
}

bool TYInfrastructure::remBatiment(QString idBatiment)
{
    bool ret = false;
    TYTabBatimentGeoNode::iterator ite;

    for (ite = _listBatiment.begin(); ite != _listBatiment.end(); ite++)
    {
        if (TYBatiment::safeDownCast((*ite)->getElement())->getID().toString() == idBatiment)
        {
            ret = remBatiment(*ite);
            break;
        }
    }

    return ret;
}

void TYInfrastructure::remAllBatiment()
{
    TYProjet* pProjet = getProjet();
    if (pProjet) { pProjet->remTabElmtFromCalculs(_listBatiment); }

    _listBatiment.clear();
    setIsGeometryModified(true);
}

LPTYBatimentGeoNode TYInfrastructure::findBatiment(const LPTYBatiment pBatiment)
{
    assert(pBatiment);
    TYTabBatimentGeoNode::iterator ite;

    for (ite = _listBatiment.begin(); ite != _listBatiment.end(); ite++)
    {
        if (TYBatiment::safeDownCast((*ite)->getElement()) == pBatiment)
        {
            return (*ite);
        }
    }

    return NULL;
}

bool TYInfrastructure::addSrc(LPTYUserSourcePonctuelleGeoNode pSrcGeoNode)
{
    assert(pSrcGeoNode);

    LPTYUserSourcePonctuelle pSource = TYUserSourcePonctuelle::safeDownCast(pSrcGeoNode->getElement());

    assert(pSource);

    pSrcGeoNode->setParent(this);
    pSource->setParent(this);

    _listSrc.push_back(pSrcGeoNode);

    // Ajoute au calcul courant
    addToCalcul(pSrcGeoNode);

    setIsGeometryModified(true);

    return true;
}

bool TYInfrastructure::addSrc(LPTYUserSourcePonctuelle pSrc)
{
    return addSrc(new TYUserSourcePonctuelleGeoNode((LPTYElement)pSrc));
}


bool TYInfrastructure::remSrc(const LPTYUserSourcePonctuelleGeoNode pSrcGeoNode)
{
    assert(pSrcGeoNode);
    bool ret = false;
    TYTabUserSourcePonctuelleGeoNode::iterator ite;
    TYProjet* pProjet = getProjet();

    for (ite = _listSrc.begin(); ite != _listSrc.end(); ite++)
    {
        if ((*ite) == pSrcGeoNode)
        {
            if (pProjet) { pProjet->remElmtFromCalculs((*ite)->getElement()); } // Supprime l'element des calculs
            _listSrc.erase(ite);
            ret = true;
            break;
        }
    }

    setIsGeometryModified(true);

    return ret;
}

bool TYInfrastructure::remSrc(const LPTYUserSourcePonctuelle pSrc)
{
    assert(pSrc);
    bool ret = false;
    TYTabUserSourcePonctuelleGeoNode::iterator ite;

    for (ite = _listSrc.begin(); ite != _listSrc.end(); ite++)
    {
        if (TYUserSourcePonctuelle::safeDownCast((*ite)->getElement()) == pSrc)
        {
            ret = remSrc(*ite);
            break;
        }
    }

    return ret;
}

bool TYInfrastructure::remSrc(QString idSrc)
{
    bool ret = false;
    TYTabUserSourcePonctuelleGeoNode::iterator ite;

    for (ite = _listSrc.begin(); ite != _listSrc.end(); ite++)
    {
        if ((*ite)->getID().toString() == idSrc)
        {
            ret = remSrc(*ite);
            break;
        }
    }

    return ret;
}

void TYInfrastructure::remAllSrc()
{
    TYProjet* pProjet = getProjet();
    if (pProjet) { pProjet->remTabElmtFromCalculs(_listSrc); }

    _listSrc.clear();
    setIsGeometryModified(true);
}

LPTYUserSourcePonctuelleGeoNode TYInfrastructure::findSrc(const LPTYUserSourcePonctuelle pSrc)
{
    assert(pSrc);
    TYTabUserSourcePonctuelleGeoNode::iterator ite;

    for (ite = _listSrc.begin(); ite != _listSrc.end(); ite++)
    {
        if (TYUserSourcePonctuelle::safeDownCast((*ite)->getElement()) == pSrc)
        {
            return (*ite);
        }
    }

    return NULL;
}

bool TYInfrastructure::addToCalcul()
{
    // Ajoute au calcul courant s'il existe
    if (!getProjet() || !getProjet()->getCurrentCalcul())
    {
        return false;
    }

    if (_listSrc.size())
    {
        TYTabUserSourcePonctuelleGeoNode::iterator ite;
        for (ite = _listSrc.begin(); ite != _listSrc.end(); ite++)
        {
            getProjet()->getCurrentCalcul()->addToSelection((*ite)->getElement());
        }
    }

    if (_listMachine.size())
    {
        TYTabMachineGeoNode::iterator ite;
        for (ite = _listMachine.begin(); ite != _listMachine.end(); ite++)
        {
            getProjet()->getCurrentCalcul()->addToSelection((*ite)->getElement());
        }
    }

    if (_listBatiment.size())
    {
        TYTabBatimentGeoNode::iterator ite;
        for (ite = _listBatiment.begin(); ite != _listBatiment.end(); ite++)
        {
            getProjet()->getCurrentCalcul()->addToSelection((*ite)->getElement());
        }
    }

    if (_listResTrans.size())
    {
        TYTabReseauTransportGeoNode::iterator ite;
        for (ite = _listResTrans.begin(); ite != _listResTrans.end(); ite++)
        {
            getProjet()->getCurrentCalcul()->addToSelection((*ite)->getElement());
        }
    }

    if (_listRoute.size())
    {
        TYTabRouteGeoNode::iterator ite;
        for (ite = _listRoute.begin(); ite != _listRoute.end(); ite++)
        {
            getProjet()->getCurrentCalcul()->addToSelection((*ite)->getElement());
        }
    }

    return true;
}

bool TYInfrastructure::remFromCalcul()
{
    LPTYProjet pProjet = getProjet();
    assert(pProjet);
    LPTYCalcul pCalcul = pProjet->getCurrentCalcul();

    // Ajoute au calcul courant s'il existe
    if (!pCalcul) { return false; }

    if (_listSrc.size())
    {
        TYTabUserSourcePonctuelleGeoNode::iterator ite;
        for (ite = _listSrc.begin(); ite != _listSrc.end(); ite++)
        {
            pCalcul->remToSelection((*ite)->getElement());
        }
    }

    if (_listMachine.size())
    {
        TYTabMachineGeoNode::iterator ite;
        for (ite = _listMachine.begin(); ite != _listMachine.end(); ite++)
        {
            pCalcul->remToSelection((*ite)->getElement());
        }
    }

    if (_listBatiment.size())
    {
        TYTabBatimentGeoNode::iterator ite;
        for (ite = _listBatiment.begin(); ite != _listBatiment.end(); ite++)
        {
            pCalcul->remToSelection((*ite)->getElement());
        }
    }

    if (_listResTrans.size())
    {
        TYTabReseauTransportGeoNode::iterator ite;
        for (ite = _listResTrans.begin(); ite != _listResTrans.end(); ite++)
        {
            pCalcul->remToSelection((*ite)->getElement());
        }
    }

    if (_listRoute.size())
    {
        TYTabRouteGeoNode::iterator ite;
        for (ite = _listRoute.begin(); ite != _listRoute.end(); ite++)
        {
            pCalcul->remToSelection((*ite)->getElement());
        }
    }

    return true;
}

bool TYInfrastructure::addToCalcul(TYGeometryNode* pNode)
{
    if ((pNode == NULL) || (pNode->getElement() == NULL)) { return false; }

    // Ajoute au calcul courant s'il existe
    if (getProjet() && getProjet()->getCurrentCalcul())
    {
        getProjet()->getCurrentCalcul()->addToSelection(pNode->getElement());
    }

    return true;
}

bool TYInfrastructure::updateAcoustic(const TYCalcul* pCalcul, const bool& force) // force = false
{
    bool ret = true;
    bool tmpRet = true;
    bool cancel = false;
    _tabUpdateNOk.clear();

#if TY_USE_IHM
    TYProgressManager::setMessage("Mise a jour de l'acoustique des infrastructures");
    TYProgressManager::set(static_cast<int>(_listMachine.size()) + static_cast<int>(_listBatiment.size())
                           + static_cast<int>(_listRoute.size()) + static_cast<int>(_listResTrans.size()));
#endif // TY_USE_IHM

    // On recupere les tableaux associatifs decrivant l'etat des elements pour le calcul
    TYMapPtrElementInt mapElementRegime = pCalcul->getMapElementRegime();
    TYMapPtrElementBool mapElementEtat = pCalcul->getEmitAcVolNode();

    TYMapPtrElementInt::iterator itRegime;

    for (itRegime = mapElementRegime.begin(); itRegime != mapElementRegime.end(); itRegime++)
    {
#if TY_USE_IHM
        TYProgressManager::step(cancel);
#endif // TY_USE_IHM
        if (cancel)
        {
            ret = false;
            break;
        }

        TYElement* pElement = (*itRegime).first;
        int regimeNb = (*itRegime).second;
        bool isRayonnant = mapElementEtat[pElement];

        TYAcousticVolumeNode* pVolNode = dynamic_cast<TYAcousticVolumeNode*>(pElement);

        if (pVolNode != nullptr)
        {
            pVolNode->setIsRayonnant(isRayonnant, false);

            if (isRayonnant)
            {
                pVolNode->setCurRegime(regimeNb);
                tmpRet = pVolNode->updateAcoustic(force);

                if (!tmpRet) { _tabUpdateNOk.push_back((TYElement*) pVolNode); }
                ret &= tmpRet;
            }

        }
        else
        {
            TYAcousticLine* pAcLine = dynamic_cast<TYAcousticLine*>(pElement);
            if (pAcLine != nullptr)
            {
                pAcLine->setIsRayonnant(isRayonnant);

                if (isRayonnant)
                {
                    tmpRet = pAcLine->updateAcoustic(force);
                    pAcLine->setCurRegime(regimeNb);

                    if (!tmpRet) { _tabUpdateNOk.push_back((TYElement*) pAcLine); }
                    ret &= tmpRet;
                }
            }
            else if (pElement->isA("TYUserSourcePonctuelle"))
            {
                TYUserSourcePonctuelle* pSP = TYUserSourcePonctuelle::safeDownCast(pElement);
                pSP->setIsRayonnant(isRayonnant);

                if (isRayonnant)
                {
                    pSP->setCurrentRegime(regimeNb);
                }

            }
        }
    }

#if TY_USE_IHM
    TYProgressManager::stepToEnd();
#endif // TY_USE_IHM

    if (ret) { setIsAcousticModified(false); }

    return ret;
}


void TYInfrastructure::getAllSrcs(const TYCalcul* pCalcul, TYMapElementTabSources& mapElementSrcs)
{
    unsigned int i, j;

    if (updateAcoustic(pCalcul)) // On recupere les sources uniquement si l'ensemble des element est a jour
    {
        // Insertion de la liste des sources utilisateurs au tableau des sources retourne
        for (i = 0; i < _listSrc.size(); i++)
        {
            LPTYUserSourcePonctuelle pElement = TYUserSourcePonctuelle::safeDownCast(_listSrc[i]->getElement());

            if (pElement->isInCurrentCalcul() && pElement->getIsRayonnant()) // Ajoute a la liste uniquement si utilise par calcul courant
            {
                TYTabSourcePonctuelleGeoNode tab;
                tab.push_back(_listSrc[i]);
                mapElementSrcs[pElement] = tab;
            }

        }

        // Insertion de la liste des sources de machines au tableau retourne
        for (i = 0 ; i < _listMachine.size() ; i++)
        {
            LPTYMachine pElement = TYMachine::safeDownCast(_listMachine[i]->getElement());

            if (pElement->isInCurrentCalcul() && pElement->getIsRayonnant())
            {
                TYTabSourcePonctuelleGeoNode tabTemp = pElement->getSrcs();
                TYTabSourcePonctuelleGeoNode tab;

                // Concatenation des matrices
                OMatrix matrix = _listMachine[i]->getMatrix();
                for (j = 0; j < tabTemp.size(); j++)
                {
                    tabTemp[j]->setMatrix(matrix * tabTemp[j]->getMatrix());
                    tab.push_back(new TYSourcePonctuelleGeoNode(*tabTemp[j]));
                }

                mapElementSrcs[pElement] = tab;
            }
        }

        // Insertion de la liste des sources de bi¿½timents au tableau retourne
        for (i = 0 ; i < _listBatiment.size() ; i++)
        {
            LPTYBatiment pElement = TYBatiment::safeDownCast(_listBatiment[i]->getElement());
            if (pElement->isInCurrentCalcul() && pElement->getIsRayonnant())
            {
                TYTabSourcePonctuelleGeoNode tabTemp = pElement->getSrcs();
                TYTabSourcePonctuelleGeoNode tab;

                // Concatenation des matrices
                OMatrix matrix = _listBatiment[i]->getMatrix();
                for (j = 0; j < tabTemp.size(); j++)
                {
                    tabTemp[j]->setMatrix(matrix * tabTemp[j]->getMatrix());
                    tab.push_back(new TYSourcePonctuelleGeoNode(*tabTemp[j]));
                }

                mapElementSrcs[pElement] = tab;
            }
        }

        // Insertion de la liste des sources de reseau de transport au tableau retourne
        for (i = 0 ; i < _listResTrans.size() ; i++)
        {
            LPTYReseauTransport pElement = TYReseauTransport::safeDownCast(_listResTrans[i]->getElement());
            if (pElement->isInCurrentCalcul() && pElement->getIsRayonnant())
            {
                TYTabSourcePonctuelleGeoNode tabTemp = pElement->getSrcs();
                TYTabSourcePonctuelleGeoNode tab;

                // Changement de repere
                OMatrix matrix = _listResTrans[i]->getMatrix();
                for (j = 0; j < tabTemp.size(); j++)
                {
                    tabTemp[j]->setMatrix(matrix * tabTemp[j]->getMatrix());
                    tab.push_back(new TYSourcePonctuelleGeoNode(*tabTemp[j]));
                }

                mapElementSrcs[pElement] = tab;
            }
        }

        // Insertion de la liste des sources de routes au tableau retourne
        for (i = 0 ; i < _listRoute.size() ; i++)
        {
            LPTYRoute pElement = TYRoute::safeDownCast(_listRoute[i]->getElement());
            if (pElement->isInCurrentCalcul() && pElement->getIsRayonnant())
            {
                TYTabSourcePonctuelleGeoNode tabTemp = pElement->getSrcs();
                TYTabSourcePonctuelleGeoNode tab;

                // Changement de repere
                OMatrix matrix = _listRoute[i]->getMatrix();

                for (j = 0; j < tabTemp.size(); j++)
                {
                    tabTemp[j]->setMatrix(matrix * tabTemp[j]->getMatrix());
                    tab.push_back(new TYSourcePonctuelleGeoNode(*tabTemp[j]));
                }

                mapElementSrcs[pElement] = tab;
            }
        }
    }
}

void TYInfrastructure::clean()
{
    _listRoute.clear();
    _listResTrans.clear();
    _listBatiment.clear();
    _listSrc.clear();

    setIsGeometryModified(true);
}
