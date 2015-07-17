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

#ifdef _MSC_VER
#   pragma warning (disable : 4786)
#endif

#include <stdlib.h>
#include <string>

#include "Tympan/core/defines.h"
#include "Tympan/core/logging.h"
#include "Tympan/models/business/TYProgressManager.h"
#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/TYXMLManager.h"
#include "Tympan/models/business/TYRectangularMaillage.h"
#include "Tympan/models/business/DefaultSolverConfig.h"
#include "TYProjet.h"
#include "TYCalcul.h"

#if TY_USE_IHM
  #include "Tympan/gui/widgets/TYCalculWidget.h"
  #include "Tympan/gui/gl/TYCalculGraphic.h"
#endif


TY_EXTENSION_INST(TYCalcul);
TY_EXT_GRAPHIC_INST(TYCalcul);

#define TR(id) OLocalizator::getString("OMessageManager", (id))

#define MAX_SOURCES 1024
#define MAX_RECEPTEURS 131072

TYCalcul::TYCalcul() : 
            _auteur("Auteur"),
            _comment("Commentaire"),
            _dateCreation("2001-10-01"),
            _dateModif("2001-10-01"),
            _numero(1),
            _pResultat(new TYResultat()),
            _upTodate(true),
            _state(TYCalcul::Actif),
            _solverId( OGenID("{A98B320C-44C4-47a9-B689-1DD352DAA8B2}") ),
            solverParams(DEFAULT_SOLVER_CONFIG)
{
    _name = TYNameManager::get()->generateName(getClassName());
    _pResultat->setParent(this);
}

TYCalcul::TYCalcul(const TYCalcul& other)
{
    *this = other;
}

TYCalcul::~TYCalcul()
{
    purge();
}

TYProjet* TYCalcul::getProjet()
{
    return TYProjet::safeDownCast(_pParent);
}

TYCalcul& TYCalcul::operator=(const TYCalcul& other)
{
    _tabRays.clear();
    if (this != &other)
    {
        TYElement::operator =(other);
        _numero = other._numero;
        _auteur = other._auteur;
        _dateCreation = other._dateCreation;
        _dateModif = other._dateModif;
        _comment = other._comment;
        _upTodate = other._upTodate;
        _state = other._state;
        //_maillages = other._maillages;
        _pResultat = other._pResultat;
        _elementSelection = other._elementSelection;
        _emitAcVolNode = other._emitAcVolNode;
        _mapElementRegime = other._mapElementRegime;
        _solverId = other._solverId;
        _tabRays = other._tabRays;
    }

    return *this;
}

bool TYCalcul::operator==(const TYCalcul& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        if (_auteur != other._auteur) { return false; }
        if (_dateCreation != other._dateCreation) { return false; }
        if (_dateModif != other._dateModif) { return false; }
        if (_comment != other._comment) { return false; }
        if (_numero != other._numero) { return false; }
        if (_upTodate != other._upTodate) { return false; }
        if (_state != other._state) { return false; }
        //if (_maillages != other._maillages) { return false; }
        if (_pResultat != other._pResultat) { return false; }
        if (_emitAcVolNode != other._emitAcVolNode) { return false; }
        if (_mapElementRegime != other._mapElementRegime) { return false; }
        if (_elementSelection != other._elementSelection) { return false; }
        if (_mapPointCtrlSpectre != other._mapPointCtrlSpectre) { return false; }
        if (_solverId != other._solverId) { return false; }
        if (_tabRays != other._tabRays) { return false; }
    }
    return true;
}

bool TYCalcul::operator!=(const TYCalcul& other) const
{
    return !operator==(other);
}

bool TYCalcul::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    purge();

    TYCalcul* pOtherCalcul = (TYCalcul*) pOther;

    // copie du parent
    _pParent = pOther->getParent();

    _numero = pOtherCalcul->_numero;
    _auteur = pOtherCalcul->_auteur;
    _dateCreation = pOtherCalcul->_dateCreation;
    _dateModif = pOtherCalcul->_dateModif;
    _comment = pOtherCalcul->_comment;
    _upTodate = pOtherCalcul->_upTodate;
    _state = pOtherCalcul->_state;

    _pResultat->deepCopy(pOtherCalcul->_pResultat, copyId);

    _elementSelection = pOtherCalcul->_elementSelection;
    _emitAcVolNode = pOtherCalcul->_emitAcVolNode;
    _mapElementRegime = pOtherCalcul->_mapElementRegime;

    // Use same points but initialize spectrum
    _mapPointCtrlSpectre = pOtherCalcul->_mapPointCtrlSpectre;
    clearCtrlPointsSpectrums();

    // Use Same NoiseMap but initialize spectrums
    copyNoiseMapSpectrums(pOtherCalcul->_noiseMapsSpectrums);

    _solverId = pOtherCalcul->_solverId;

    for (unsigned int i=0; i<pOtherCalcul->_tabRays.size(); i++)
    {
        LPTYRay aRay = new TYRay();
        aRay->deepCopy(pOtherCalcul->_tabRays.at(i), copyId);
        _tabRays.push_back(aRay);
    }

    return true;
}

std::string TYCalcul::toString() const
{
    return "TYCalcul";
}

DOM_Element TYCalcul::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);

    if (_upTodate)
    {
        domNewElem.setAttribute("isUpTodate", "1");
    }
    else
    {
        domNewElem.setAttribute("isUpTodate", "0");
    }

    TYXMLTools::addElementIntValue(domNewElem, "numero", _numero);
    TYXMLTools::addElementStringValue(domNewElem, "auteur", _auteur);
    TYXMLTools::addElementStringValue(domNewElem, "dateCreation", _dateCreation);
    TYXMLTools::addElementStringValue(domNewElem, "dateModif", _dateModif);
    TYXMLTools::addElementStringValue(domNewElem, "comment", _comment);
    TYXMLTools::addElementStringValue(domNewElem, "solverId", _solverId.toString());
    TYXMLTools::addElementIntValue(domNewElem, "etat", _state);  // 16/08/2005 Possibilite de bloquer un calcul
    TYXMLTools::addElementStringValue(domNewElem, "solverParams", solverParams);

    // Ajout du site node sur lequel s'effectue le calcul
    DOM_Document domDoc = domElement.ownerDocument();

    // Selection
    TYListID::iterator ite;
    DOM_Element listIDNode = domDoc.createElement("ListID");
    domNewElem.appendChild(listIDNode);

    for (ite = _elementSelection.begin(); ite != _elementSelection.end(); ++ite)
    {
        DOM_Element tmpNode = domDoc.createElement("Element");
        listIDNode.appendChild(tmpNode);

        tmpNode.setAttribute("id", (*ite).toString());
    }

    // Etats des AccVolNodes en emission
    TYMapPtrElementBool::iterator iter3;
    DOM_Element emissionAccVolNodesNode = domDoc.createElement("EmissionAccVolNodes");
    domNewElem.appendChild(emissionAccVolNodesNode);

    for (iter3 = _emitAcVolNode.begin(); iter3 != _emitAcVolNode.end(); iter3++)
    {
        DOM_Element tmpNode = domDoc.createElement("EmissionAccVolNode");
        emissionAccVolNodesNode.appendChild(tmpNode);

        tmpNode.setAttribute("accVolNodeId", (*iter3).first->getID().toString());
        tmpNode.setAttribute("state", QString(intToStr((*iter3).second).c_str()));
    }

    // Regimes associes a chaque source
    TYMapPtrElementInt::iterator iter4;
    DOM_Element regimeAccElementNodesNode = domDoc.createElement("RegimeAccNode");
    domNewElem.appendChild(regimeAccElementNodesNode);

    for (iter4 = _mapElementRegime.begin(); iter4 != _mapElementRegime.end(); iter4++)
    {
        DOM_Element tmpNode = domDoc.createElement("RegimeAccNode");
        regimeAccElementNodesNode.appendChild(tmpNode);

        tmpNode.setAttribute("accVolNodeId", (*iter4).first->getID().toString());
        tmpNode.setAttribute("state", QString(intToStr((*iter4).second).c_str()));
    }

    // Points de controle
    DOM_Element resuRecepteursNode = domDoc.createElement("ResuCtrlPnts");
    domNewElem.appendChild(resuRecepteursNode);
    TYMapIdSpectre::iterator itRec;
    for (itRec=_mapPointCtrlSpectre.begin(); itRec!=_mapPointCtrlSpectre.end(); itRec++)
    {
        DOM_Element tmpNode = domDoc.createElement("Recepteur");
        resuRecepteursNode.appendChild(tmpNode);
        tmpNode.setAttribute("receptor_id", (*itRec).first.toString());
        (*itRec).second->toXML(tmpNode); 
    }

    // Noise maps
    DOM_Element resuNoiseMapNode = domDoc.createElement("ResuNoiseMaps");
    domNewElem.appendChild(resuNoiseMapNode);
    TYMapIdTabSpectre::iterator itNM;
    for (itNM=_noiseMapsSpectrums.begin(); itNM!=_noiseMapsSpectrums.end(); itNM++)
    {
        DOM_Element tmpNode = domDoc.createElement("NoiseMap");
        resuNoiseMapNode.appendChild(tmpNode);
        tmpNode.setAttribute("noise_map_id", (*itNM).first.toString());
        for (unsigned int i=0; i<(*itNM).second.size(); i++)
        {
            (*itNM).second.at(i)->toXML(tmpNode);
        }
    }

    // Resultat
    if (TYProjet::gSaveValues)
    {
        _pResultat->toXML(domNewElem);
    }
    else
    {
        TYResultat tmp;
        tmp.toXML(domNewElem);
    }

    // Sauvegarde de rayons (chemins acoustiques)
    DOM_Element listRaysNode = domDoc.createElement("ListRayons");
    domNewElem.appendChild(listRaysNode);
    for (size_t i=0; i<_tabRays.size(); i++)
    {
        _tabRays.at(i)->toXML(listRaysNode);
    }

    return domNewElem;
}

int TYCalcul::fromXML(DOM_Element domElement)
{
    purge(); // Cleaning before loading

    TYElement::fromXML(domElement);

    bool getOk[8];
    unsigned int i;
    for (i = 0; i < 8; i++) { getOk[i] = false; }
    int retVal = -1;
    LPTYMaillageGeoNode pMaillageGeoNode = new TYMaillageGeoNode(NULL, this);
    LPTYRay aRay = new TYRay();
    _tabRays.clear();

    // CLM-NT33: Compatibiltité ancien format XML
    LPTYPointControl pPointControl = new TYPointControl();
    TYProjet* pProjet = getProjet();

    int etat = -1; // Etat du calcul

    TYListID tempElementSelection;
    QString strSolverId;

    int readOk = 0; // Indicateur de bonne relecture des resultats

    _upTodate = (TYXMLTools::getElementAttributeToInt(domElement, "isUpTodate") == 1);

    DOM_Element elemCur;
    QDomNodeList childs = domElement.childNodes();
    for (i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();
        TYXMLTools::getElementIntValue(elemCur, "numero", _numero, getOk[0]);
        TYXMLTools::getElementStringValue(elemCur, "auteur", _auteur, getOk[1]);
        TYXMLTools::getElementStringValue(elemCur, "dateCreation", _dateCreation, getOk[2]);
        TYXMLTools::getElementStringValue(elemCur, "dateModif", _dateModif, getOk[3]);
        TYXMLTools::getElementStringValue(elemCur, "comment", _comment, getOk[4]);
        TYXMLTools::getElementStringValue(elemCur, "solverId", strSolverId, getOk[5]);
        TYXMLTools::getElementIntValue(elemCur, "etat", etat, getOk[6]);
        TYXMLTools::getElementStringValue(elemCur, "solverParams", solverParams, getOk[7]);

        // Selection
        if (elemCur.nodeName() == "ListID")
        {
            DOM_Element elemCur2;
            QDomNodeList childs2 = elemCur.childNodes();

            for (unsigned int j = 0; j < childs2.length(); j++)
            {
                elemCur2 = childs2.item(j).toElement();
                if (elemCur2.nodeName() == "Element")
                {
                    QString id = TYXMLTools::getElementAttributeToString(elemCur2, "id");
                    tempElementSelection.push_back(id);
                }
            }
        }
        else if (elemCur.nodeName() == "EtatSources") // Etats des sources
        {
            DOM_Element elemCur2;
            QDomNodeList childs2 = elemCur.childNodes();

            for (unsigned int j = 0; j < childs2.length(); j++)
            {
                elemCur2 = childs2.item(j).toElement();

                if (elemCur2.nodeName() == "EtatSource")
                {
                    QString srcId = TYXMLTools::getElementAttributeToString(elemCur2, "srcId");
                    bool state = TYXMLTools::getElementAttributeToInt(elemCur2, "state");

                    TYElement* pSrc = TYElement::getInstance(srcId);

                    if (pSrc)
                    {
                        _emitAcVolNode.insert(TYMapPtrElementBool::value_type(pSrc, state));
                    }
                }
            }
        }
        else if (elemCur.nodeName() == "EmissionAccVolNodes") // Etats des AccVolNodes en emission
        {
            DOM_Element elemCur2;
            QDomNodeList childs2 = elemCur.childNodes();

            for (unsigned int j = 0; j < childs2.length(); j++)
            {
                elemCur2 = childs2.item(j).toElement();

                if (elemCur2.nodeName() == "EmissionAccVolNode")
                {
                    QString accVolNodeId = TYXMLTools::getElementAttributeToString(elemCur2, "accVolNodeId");
                    bool state = TYXMLTools::getElementAttributeToInt(elemCur2, "state");

                    TYElement* pAccVolNode = TYElement::getInstance(accVolNodeId);

                    if (pAccVolNode)
                    {
                        _emitAcVolNode.insert(TYMapPtrElementBool::value_type(pAccVolNode, state));
                    }
                }
            }
        }
        else if (elemCur.nodeName() == "RegimeAccNode") // Regime associe a chaque source
        {
            DOM_Element elemCur2;
            QDomNodeList childs2 = elemCur.childNodes();

            for (unsigned int j = 0; j < childs2.length(); j++)
            {
                elemCur2 = childs2.item(j).toElement();

                if (elemCur2.nodeName() == "RegimeAccNode")
                {
                    QString accVolNodeId = TYXMLTools::getElementAttributeToString(elemCur2, "accVolNodeId");
                    int state = TYXMLTools::getElementAttributeToInt(elemCur2, "state");

                    TYElement* pAccVolNode = TYElement::getInstance(accVolNodeId);

                    if (pAccVolNode)
                    {
                        _mapElementRegime.insert(TYMapPtrElementInt::value_type(pAccVolNode, state));
                    }
                }
            }
        }
        else if (elemCur.nodeName() == "ResuCtrlPnts")
        {
            DOM_Element elemCur2;
            QDomNodeList childs2 = elemCur.childNodes();
            LPTYSpectre spectrum = new TYSpectre();

            for (unsigned int j = 0; j < childs2.length(); j++)
            {
                elemCur2 = childs2.item(j).toElement();
                if (elemCur2.nodeName() == "Recepteur")
                {
                    QString strReceptor_id = TYXMLTools::getElementAttributeToString(elemCur2, "receptor_id");
                    TYUUID receptor_id;
                    receptor_id.FromString(strReceptor_id);
                    
                    // Get the spectrum
                    DOM_Element elemCur3;
                    QDomNodeList childs3 = elemCur2.childNodes();
                    for (unsigned int k = 0; k < childs3.length(); k++)
                    {
                        elemCur3 = childs3.item(k).toElement();
                        if ( spectrum->callFromXMLIfEqual(elemCur3, &retVal) )
                        {
                            if (retVal == 1)
                            {
                                _mapPointCtrlSpectre[receptor_id] = new TYSpectre(*spectrum);
                            }
                        }
                    }
                }
            }
        }
        else if (elemCur.nodeName() == "ResuNoiseMaps")
        {
            DOM_Element elemCur2;
            QDomNodeList childs2 = elemCur.childNodes();

            LPTYSpectre spectrum = new TYSpectre();
            std::vector<LPTYSpectre> tabSpectre;

            for (unsigned int j = 0; j < childs2.length(); j++)
            {
                elemCur2 = childs2.item(j).toElement();

                if (elemCur2.nodeName() == "NoiseMap")
                {
                    tabSpectre.clear();

                    QString strnoise_map_id = TYXMLTools::getElementAttributeToString(elemCur2, "noise_map_id");
                    TYUUID noise_map_id;
                    noise_map_id.FromString(strnoise_map_id);

                    DOM_Element elemCur3;
                    QDomNodeList childs3 = elemCur2.childNodes();
                    for (unsigned int k=0; k<childs3.length(); k++)
                    {
                        elemCur3 = childs3.item(k).toElement();
                        if ( spectrum->callFromXMLIfEqual(elemCur3, &retVal) )
                        {
                            if (retVal == 1)
                            {
                                tabSpectre.push_back( new TYSpectre(*spectrum) );
                            }
                        }
                    }

                    _noiseMapsSpectrums[noise_map_id] = tabSpectre;
                }
            }
        }
        else if (elemCur.nodeName() == "ListRayons")
        {
            DOM_Element elemCur2;
            QDomNodeList childs2 = elemCur.childNodes();

            for (size_t j = 0; j < childs2.length(); j++)
            {
                elemCur2 = childs2.item(j).toElement();
                if (aRay->callFromXMLIfEqual(elemCur2, &retVal))
                {
                    if (retVal == 1)
                    {
                        _tabRays.push_back(aRay);
                        aRay = new TYRay();
                    }
                }
            }
        }

        // CLM-NT33: Compatibilitité avec ancien format XML
        // Points de controle
        if (elemCur.nodeName() == "PointsControl")
        {
            DOM_Element elemCur2;
            QDomNodeList childs2 = elemCur.childNodes();

            for (unsigned int j = 0; j < childs2.length(); j++)
            {
                elemCur2 = childs2.item(j).toElement();

                // ajoute l'attribut id du calcul au spectre
                QDomNodeList childs3 = elemCur2.childNodes();
                for (unsigned int k = 0; k < childs3.length(); k++)
                {
                    DOM_Element elemCur3 = childs3.item(k).toElement();
                    if (elemCur3.nodeName() == "Spectre")
                    {
                        elemCur3.setAttribute("idCalcul", getID().toString());
                    }
                }

                if (pPointControl->callFromXMLIfEqual(elemCur2))
                {
                    pProjet->addPointControl(pPointControl);
                    pPointControl = new TYPointControl();
                }
            }
        }

        //// Compatibility with previous noise map management style
        if (pMaillageGeoNode->callFromXMLIfEqual(elemCur, &retVal))
        {
            if (retVal == 1)
            {
                pProjet->addMaillage(pMaillageGeoNode);
                dynamic_cast<TYMaillage*>( pMaillageGeoNode->getElement() )->setEtat(getID(), true);
                pMaillageGeoNode = new TYMaillageGeoNode(NULL, this);
            }
        }

        // Resultat
        _pResultat->callFromXMLIfEqual(elemCur, &readOk);
    }

    if (getOk[5]) { _solverId.FromString(strSolverId); } // Recuperation de l'Id du solveur

    // On supprime les IDs de la selection des elements non presents dans le site
    TYListID::iterator next = tempElementSelection.begin();

    while (next != tempElementSelection.end())
    {
        if (!TYElement::getInstance((*next)))
        {
            next = tempElementSelection.erase(next);
        }
        else
        {
            next++;
        }
    }

    // On ajoute ceux qui restent dans le calcul
    for (next = tempElementSelection.begin() ; next != tempElementSelection.end() ; next++)
    {
        _elementSelection.push_back((*next));
    }

    // Si le tableau associatif element/regime est vide (fichier 3.1), on le cree,
    // ainsi que le tableau des etats de rayonnement
    if (_mapElementRegime.size() == 0)
    {
        TYListID::iterator iter;
        int regime = 0;
        bool bEmit = false;
        for (iter = _elementSelection.begin(); iter != _elementSelection.end() ; iter++)
        {
            TYElement* pElement = TYElement::getInstance(*iter);
            TYAcousticVolumeNode* pVolNode = dynamic_cast<TYAcousticVolumeNode*>(pElement);
            if (pVolNode != nullptr)
            {
                regime = pVolNode->getCurRegime();
                bEmit = pVolNode->getIsRayonnant();
            }
            else
            {
                TYAcousticLine* pLine = dynamic_cast<TYAcousticLine*>(pElement);
                if(pLine != nullptr)
                {
                    regime = pLine->getCurRegime();
                    bEmit = pLine->getIsRayonnant();
                }
                else if (pElement->isA("TYUserSourcePonctuelle"))
                {
                    regime = TYUserSourcePonctuelle::safeDownCast(pElement)->getCurrentRegime();
                    bEmit = true;
                }
            }
            _mapElementRegime[pElement] = regime;
            _emitAcVolNode[pElement] = bEmit;

            regime = 0;
            bEmit = false;
        }

    }

    if (etat != -1) { setState(etat); } // Si un etat a ete relu (fichier format TYMPAN 3.3) mise a jour de l'etat
    if (readOk == -1)
    {
        _pResultat->purge();
        return readOk;
    }
    return 1;
}

void TYCalcul::purge()
{
    //remAllMaillage();
    _tabRays.clear();

    _elementSelection.clear();

    _pResultat->purge();
    _mapElementRegime.clear();
    _emitAcVolNode.clear();
    
    // Cleaning control point / spectrum association
    _mapPointCtrlSpectre.clear();

    // Cleaning noise map / spectrums association
    _noiseMapsSpectrums.clear();

    setIsGeometryModified(true);
}

void TYCalcul::clearResult()
{
    TYTabLPPointControl::iterator ite;
    clearCtrlPointsSpectrums();
    clearNoiseMapsSpectrums();

    _pResultat->purge();
	_tabRays.clear();

    setIsGeometryModified(true);
}

LPTYSiteNode TYCalcul::getSite()
{
    TYProjet* pProjet = getProjet();
    if (pProjet)
    {
        return pProjet->getSite();
    }

    return NULL;
}


void TYCalcul::setSite(LPTYSiteNode pSite)
{
    _pSiteCalcul = pSite;
}

bool TYCalcul::addToSelection(TYUUID id)
{
    bool res = false;

    if (!isInSelection(id))
    {
        _elementSelection.push_back(id);
        res = true;
    }

    return res;
}

void TYCalcul::addToSelection(TYElement* pElt, bool recursif /*=true*/)
{
    if (!pElt) { return ; }

    TYUUID id = pElt->getID();

    if (addToSelection(id))
    {
        bool etat = true;
        if (pElt->isA("TYInfrastructure"))
        {
            // Si un objet est ajoute son parent l'est forcemment
            addToSelection(pElt->getParent(), false);
        }
        else if (dynamic_cast<TYSiteNode*>(pElt) != nullptr)
        {
            // Si un objet est ajoute son parent l'est forcemment
            addToSelection(pElt->getParent(), false);

            // Si c'est un site on n'ajoute pas systématiquement tous les enfants
            recursif = false;
        }
        else if (dynamic_cast<TYAcousticVolumeNode*>(pElt) != nullptr)
        {
            TYAcousticVolumeNode* pVolNode = dynamic_cast<TYAcousticVolumeNode*>(pElt);
            etat = pVolNode->getIsRayonnant();
            _emitAcVolNode[pElt] = etat;
            _mapElementRegime[pElt] = 0;

            // Si un objet est ajoute son parent l'est forcemment
            addToSelection(pElt->getParent(), false);

        }
        else if (dynamic_cast<TYAcousticLine*>(pElt) != nullptr)
        {
            TYAcousticLine* pLine = dynamic_cast<TYAcousticLine*>(pElt);
            etat = pLine->getIsRayonnant();
            _emitAcVolNode[pElt] = etat;
            _mapElementRegime[pElt] = 0;

            // Si un objet est ajoute son parent l'est forcemment
            addToSelection(pElt->getParent(), false);
        }
        else if (dynamic_cast<TYUserSourcePonctuelle*>(pElt) != nullptr)
        {
            TYUserSourcePonctuelle* pSource = dynamic_cast<TYUserSourcePonctuelle*>(pElt);
            if (pSource != nullptr) { etat = pSource->getIsRayonnant(); }
            _emitAcVolNode[pElt] = etat;
            _mapElementRegime[pElt] = 0;

            // Si un objet est ajoute son parent l'est forcemment
            addToSelection(pElt->getParent(), false);
        }

        // Informe l'element qu'il est dans le calcul courant
        pElt->setInCurrentCalcul(true, false);

        // Si recursif on ajoute les enfants
        if (recursif)
        {
            LPTYElementArray childs;
            pElt->getChilds(childs, false);

            for (int i = 0; i < childs.size(); i++)
            {
                addToSelection(childs[i], recursif);
            }
        }
    }
}

bool TYCalcul::remToSelection(TYUUID id)
{
    _elementSelection.remove(id);
    setIsGeometryModified(true);
    return true;
}

bool TYCalcul::remToSelection(TYElement* pElt, bool recursif /*=true*/)
{
    assert(pElt);
    bool ret = false;
    TYUUID id = pElt->getID();

    if (isInSelection(id))
    {
        // Reset des resultats precedents (car plus valables)
        _pResultat->purge();

        // On supprime l'element lui meme des differents tableaux associatifs
        _elementSelection.remove(id);
        _emitAcVolNode.erase(pElt);
        _mapElementRegime.erase(pElt);

        // On informe l'element qu'ils ne sont plua dans le calcul
        pElt->setInCurrentCalcul(false, false, false);

        //  On désactive ses enfants
        LPTYElementArray childs;
        pElt->getChilds(childs, false);

        for (int i = 0; i < childs.size(); i++)
        {
            remToSelection(childs[i], recursif);
        }

        pElt->setIsGeometryModified(true);

        ret = true;
    }
    return ret;
}

bool TYCalcul::isInSelection(TYUUID id)
{
    bool present = false;
    TYListID::iterator ite;

    for (ite = _elementSelection.begin(); ite != _elementSelection.end(); ++ite)
    {
        if ((*ite) == id)
        {
            present = true;
            break;
        }
    }

    return present;
}

void TYCalcul::goPostprocessing()
{
    // Create result map (business sources --> micro sources)
    TYMapElementTabSources& mapElementSources = _pResultat->getMapEmetteurSrcs();
    getProjet()->getSite()->getInfrastructure()->getAllSrcs(this, mapElementSources);
    // build sources spectra
    _pResultat->buildMapSourceSpectre();

    // Le calcul a proprement parler est termine
    // Il est necessaire de reattribuer les parents des elements du site merges
    getProjet()->getSite()->reparent();

    TYNameManager::get()->enable(true);

    setIsAcousticModified(true); // Les données acoustiques ont été actualisées
}

void TYCalcul::getCalculElements(LPTYSiteNode pSite)
{
    unsigned int i;
    //  bool ok = true;

    if (getProjet()->getCurrentCalcul() != this)
    {
        return;
    }

    // Pour chaque element, 2 cas :
    //      - il sont dans la liste des elements du calcul mais ne sont pas "inCurrentCalcul"
    //          --> il faut les informer (setInCurrentCalcul(true))
    //      - ils sont "inCurrentCalcul" mais ne sont pas dans la liste
    //          --> il faut les "desactiver" (setInCurrentCalcul(false))

    for (i = 0; i < pSite->getInfrastructure()->getListBatiment().size(); i++)
    {
        LPTYBatiment pBatiment = TYBatiment::safeDownCast(pSite->getInfrastructure()->getListBatiment()[i]->getElement());

        if ((pBatiment->isInCurrentCalcul()) && (_emitAcVolNode.find(pBatiment) == _emitAcVolNode.end()))
        {
            // On l'informe qu'il ne fait pas partie du calcul
            pBatiment->setInCurrentCalcul(false);
        }
        else if ((!pBatiment->isInCurrentCalcul()) && (_emitAcVolNode.find(pBatiment) != _emitAcVolNode.end()))
        {
            // On l'informe qu'il fait partie du calcul
            pBatiment->setInCurrentCalcul(true);
        }
    }

    for (i = 0; i < pSite->getInfrastructure()->getListMachine().size(); i++)
    {
        LPTYMachine pMachine = TYMachine::safeDownCast(pSite->getInfrastructure()->getListMachine()[i]->getElement());

        if ((pMachine->isInCurrentCalcul()) && (_emitAcVolNode.find(pMachine) == _emitAcVolNode.end()))
        {
            // On l'informe qu'il ne fait pas partie du calcul
            pMachine->setInCurrentCalcul(false);
        }
        else if ((!pMachine->isInCurrentCalcul()) && (_emitAcVolNode.find(pMachine) != _emitAcVolNode.end()))
        {
            // On l'informe qu'il fait partie du calcul
            pMachine->setInCurrentCalcul(true);
        }
    }

    for (i = 0; i < pSite->getInfrastructure()->getSrcs().size(); i++)
    {
        LPTYSourcePonctuelle pSrcPonct = TYSourcePonctuelle::safeDownCast(pSite->getInfrastructure()->getSrc(i)->getElement());

        if ((pSrcPonct->isInCurrentCalcul()) && (_emitAcVolNode.find(pSrcPonct) == _emitAcVolNode.end()))
        {
            // On l'informe qu'il ne fait pas partie du calcul
            pSrcPonct->setInCurrentCalcul(false);
        }
        else if ((!pSrcPonct->isInCurrentCalcul()) && (_emitAcVolNode.find(pSrcPonct) != _emitAcVolNode.end()))
        {
            // On l'informe qu'il fait partie du calcul
            pSrcPonct->setInCurrentCalcul(true);
        }
    }

    for (i = 0; i < pSite->getInfrastructure()->getListRoute().size() ; i++)
    {
        LPTYRoute pRoute = pSite->getInfrastructure()->getRoute(i);

        if ((pRoute->isInCurrentCalcul()) && (_emitAcVolNode.find(pRoute) == _emitAcVolNode.end()))
        {
            // On l'informe qu'il ne fait pas partie du calcul
            pRoute->setInCurrentCalcul(false);
        }
        else if ((!pRoute->isInCurrentCalcul()) && (_emitAcVolNode.find(pRoute) != _emitAcVolNode.end()))
        {
            // On l'informe qu'il fait partie du calcul
            pRoute->setInCurrentCalcul(true);
        }
    }

    for (i = 0; i < pSite->getInfrastructure()->getListResTrans().size() ; i++)
    {
        LPTYReseauTransport pResTrans = pSite->getInfrastructure()->getResTrans(i);

        if ((pResTrans->isInCurrentCalcul()) && (_emitAcVolNode.find(pResTrans) == _emitAcVolNode.end()))
        {
            // On l'informe qu'il ne fait pas partie du calcul
            pResTrans->setInCurrentCalcul(false);
        }
        else if ((!pResTrans->isInCurrentCalcul()) && (_emitAcVolNode.find(pResTrans) != _emitAcVolNode.end()))
        {
            // On l'informe qu'il fait partie du calcul
            pResTrans->setInCurrentCalcul(true);
        }
    }
}

bool TYCalcul::addPtCtrlToResult(LPTYPointControl pPoint)
{
    // Create point control entry if not done
    TYUUID id_point = pPoint->getID();
    if ( _mapPointCtrlSpectre.find(id_point) == _mapPointCtrlSpectre.end() )
    {
        LPTYSpectre spectre = new TYSpectre();
        _mapPointCtrlSpectre[id_point] = spectre;

        // Set control point on for this calcul
        pPoint->setEtat(getID(), true);

        if ( _pResultat->addRecepteur(pPoint) == true ) { _pResultat->buildMatrix(); }

        return true;
    }

    return false;
}

bool TYCalcul::remPtCtrlFromResult(LPTYPointControl pPoint)
{
    // Remove point control entry
    TYMapIdSpectre::iterator it = _mapPointCtrlSpectre.find( pPoint->getID() );
    if ( it != _mapPointCtrlSpectre.end() ) { _mapPointCtrlSpectre.erase(it); }

    // Set control point off for this calcul
    pPoint->setEtat(getID(), false);

    return _pResultat->remRecepteur(pPoint);
}

void TYCalcul::clearCtrlPointsSpectrums()
{
    TYMapIdSpectre::iterator it;
    for (it=_mapPointCtrlSpectre.begin(); it!=_mapPointCtrlSpectre.end(); it++)
    {
        (*it).second = new TYSpectre();
    }
}

void TYCalcul::clearNoiseMapsSpectrums()
{
    TYMapIdTabSpectre::iterator it;
    for (it=_noiseMapsSpectrums.begin(); it!=_noiseMapsSpectrums.end(); it++)
    {
        for (unsigned int i=0; i<(*it).second.size(); i++)
        {
            (*it).second.at(i)->deepCopy( new TYSpectre() );
        }
    }
}

void TYCalcul::copyNoiseMapSpectrums(TYMapIdTabSpectre& otherNoiseMap)
{
    TYMapIdTabSpectre::iterator it;
    for (it=otherNoiseMap.begin(); it!=otherNoiseMap.end(); it++)
    {
        TYUUID id = (*it).first;
        std::vector<LPTYSpectre> tabSpectres;
        for (unsigned int i=0; i<(*it).second.size(); i++)
        {
            tabSpectres.push_back( new TYSpectre() );
        }

        _noiseMapsSpectrums[id] = tabSpectres;
    }
}

LPTYSpectre  TYCalcul::getSpectre(const TYPointControl* pPoint)
{
    return getSpectre(pPoint->getID());
}

LPTYSpectre TYCalcul::getSpectre(const TYUUID& id_pt)
{
    TYMapIdSpectre::iterator it = _mapPointCtrlSpectre.find(id_pt);

    if (it != _mapPointCtrlSpectre.end()) { return (*it).second; }

    return new TYSpectre();
}

void TYCalcul::setSpectre(TYPointCalcul* pPoint, TYSpectre* pSpectre)
{
    TYMapIdSpectre::iterator it = _mapPointCtrlSpectre.find(pPoint->getID());

    // Add only if the control point is known from the calcul
    if (it != _mapPointCtrlSpectre.end()) 
    { 
        (*it).second = pSpectre;
    }
    else // other case point is owned by a TYMaillage
    {
        if ( dynamic_cast<TYMaillage*>(pPoint->getParent()) )
        {
            TYSpectre *currentSpectre = pPoint->getSpectre().getRealPointer();
            currentSpectre->deepCopy(pSpectre);
        }
    }
}


void TYCalcul::setSpectre(const TYUUID& id_pt, TYSpectre* pSpectre)
{
    assert(true && "NOT IMPLEMENTED");
}

bool TYCalcul::getPtCtrlStatus(const TYUUID& id_pt)
{
    TYMapIdSpectre::iterator it = _mapPointCtrlSpectre.find(id_pt);

    if (it != _mapPointCtrlSpectre.end()) { return true; }

    return false;
}

void TYCalcul::setPtCtrlStatus(const TYUUID& id_pt, bool bStatus)
{
    // Suppression de la map des spectres aux points
    TYMapIdSpectre::iterator it = _mapPointCtrlSpectre.find(id_pt);

    if (it != _mapPointCtrlSpectre.end()) { _mapPointCtrlSpectre.erase(it); }

    // Information du point de controle
    TYElement *pElem = TYElement::getInstance(id_pt);
    if (pElem)
    {
        TYPointControl *pPoint = dynamic_cast<TYPointControl*>(pElem);
        if (pPoint)
        {
            pPoint->setEtat(getID(), bStatus);
        }
    }

}

std::vector<LPTYSpectre> *TYCalcul::getSpectrumDatas(const TYUUID& id)
{
    TYMapIdTabSpectre::iterator it = _noiseMapsSpectrums.find(id);

    if ( it != _noiseMapsSpectrums.end() ) { return &(*it).second; }

    return nullptr;
}

bool TYCalcul::addMaillage(TYMaillage* pMaillage)
{
    TYUUID id = pMaillage->getID();

    // Test if map is already selected for this calcul
    if ( _noiseMapsSpectrums.find(id) != _noiseMapsSpectrums.end() ) { return false; }

    size_t nbPoints = pMaillage->getPtsCalcul().size();
    std::vector<LPTYSpectre> tabSpectres;
    for (unsigned int i=0; i<nbPoints; i++)
    {
        tabSpectres.push_back(new TYSpectre());
    }

    _noiseMapsSpectrums[id] = tabSpectres;

    pMaillage->setEtat(getID(), true);

    return true;
}

bool TYCalcul::remMaillage(TYMaillage* pMaillage)
{    
    TYUUID id = pMaillage->getID();
    TYMapIdTabSpectre::iterator it = _noiseMapsSpectrums.find(id);

    if ( it!=_noiseMapsSpectrums.end() )
    {
        _noiseMapsSpectrums.erase(it);

        pMaillage->setEtat(getID(), false);

        return true;
    }

    return false;
}

void TYCalcul::setNoiseMapSpectrums(const TYMaillage* pMaillage, TYTabLPSpectre& tabSpectrum)
{
    TYUUID id = pMaillage->getID();
    setNoiseMapSpectrums(id, tabSpectrum);
}

void TYCalcul::setNoiseMapSpectrums(const TYUUID& id, TYTabLPSpectre& tabSpectrum)
{
    _noiseMapsSpectrums[id] = tabSpectrum;
}