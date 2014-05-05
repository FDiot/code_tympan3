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
#include "Tympan/GraphicIHM/DataManagerIHM/TYEtageWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYEtageGraphic.h"
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "TYEtage.h"

#include "Tympan/MetierSolver/ToolsMetier/OSegment3D.h"

#include "Tympan/Tools/OMessageManager.h"

#include "Tympan/MetierSolver/DataManagerCore/TYPreferenceManager.h"

OPROTOINST(TYEtage);
TY_EXTENSION_INST(TYEtage);
TY_EXT_GRAPHIC_INST(TYEtage);

TYEtage::TYEtage(): _closed(false), _surfAbsorbante(0.0), _typeReverb(0), _volumeLibre(0.0)
{
    _name = TYNameManager::get()->generateName(getClassName());

    _bPourCalculTrajet = false;
#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "UseNewAlgoGeom"))
    {
        _bPourCalculTrajet = TYPreferenceManager::getBool(TYDIRPREFERENCEMANAGER, "UseNewAlgoGeom");
    }
    else
    {
        TYPreferenceManager::setBool(TYDIRPREFERENCEMANAGER, "UseNewAlgoGeom", _bPourCalculTrajet);
    }
#endif

    _pSol = new TYDalle();
    _pSol->setParent(this);

    // Le sol est inactif par defaut
    _pSol->setDensiteSrcs(0.0);
    _pSol->setIsRayonnant(false, false);

    _pParoi = new TYParoi();

    _pPlafond = new TYDalle();
    _pPlafond->setParent(this);

    _absoSabine = OSpectre::getEmptyLinSpectre();

    // On rajoute une entree dans le tableau associant les regimes
    TYMapPtrElementInt mapElmRegime;
    _tabRegimesMachines.push_back(mapElmRegime);

    TYMapPtrElementBool mapElmEtat;
    _tabEtatMachines.push_back(mapElmEtat);

    _volEnglob = volEnglob(); // (re)calcul du volume englobant
}

TYEtage::~TYEtage()
{
}

TYEtage::TYEtage(const TYEtage& other)
{
    *this = other;
}

TYEtage& TYEtage::operator=(const TYEtage& other)
{
    TYAcousticVolume::operator =(other);
    _bPourCalculTrajet = other._bPourCalculTrajet;
    _closed = other._closed;
    _pParoi = other._pParoi;
    _typeReverb = other._typeReverb;
    _tabMur = other._tabMur;
    _pSol = other._pSol;
    _pPlafond = other._pPlafond;
    _tabSources = other._tabSources;
    _tabMachine = other._tabMachine;
    _volEnglob = other._volEnglob;

    calculRayonSphere(_volEnglob);
    calculCentreGravite();

    return *this;
}

bool TYEtage::operator==(const TYEtage& other) const
{
    if (this != &other)
    {
        if (TYAcousticVolume::operator !=(other)) { return false; }
        if (_bPourCalculTrajet != (other._bPourCalculTrajet)) { return false; }
        if (_closed != other._closed) { return false; }
        if (_pParoi != other._pParoi) { return false; }
        if (!(_tabMur == other._tabMur)) { return false; }
        if (_pSol != other._pSol) { return false; }
        if (_pPlafond != other._pPlafond) { return false; }
        if (_tabSources != other._tabSources) { return false; }
        if (_tabMachine != other._tabMachine) { return false; }
        if (_typeReverb != other._typeReverb) { return false; }
    }
    return true;
}

bool TYEtage::operator!=(const TYEtage& other) const
{
    return !operator==(other);
}

bool TYEtage::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYAcousticVolume::deepCopy(pOther, copyId)) { return false; }

    TYEtage* pOtherEtage = (TYEtage*) pOther;

    _bPourCalculTrajet = pOtherEtage->_bPourCalculTrajet;
    _closed = pOtherEtage->_closed;
    _typeReverb = pOtherEtage->_typeReverb;

    _pParoi->deepCopy(pOtherEtage->_pParoi, copyId);

    _pSol->deepCopy(pOtherEtage->_pSol, copyId);
    _pSol->setParent(this);
    _pPlafond->deepCopy(pOtherEtage->_pPlafond, copyId);
    _pPlafond->setParent(this);

    _tabMur.clear();

    unsigned int i, j;
    for (i = 0; i < pOtherEtage->_tabMur.size(); i++)
    {
        LPTYMurGeoNode pMurGeoNode = new TYMurGeoNode(NULL, this);
        pMurGeoNode->deepCopy(pOtherEtage->_tabMur[i], copyId);
        pMurGeoNode->getElement()->setParent(this);
        _tabMur.push_back(pMurGeoNode);
    }

    _tabSources.clear();
    for (i = 0 ; i < pOtherEtage->_tabSources.size(); i++)
    {
        LPTYSourcePonctuelleGeoNode pSourceGeoNode = new TYSourcePonctuelleGeoNode(new TYSourcePonctuelle);
        pSourceGeoNode->deepCopy(pOtherEtage->_tabSources[i], copyId);
        _tabSources.push_back(pSourceGeoNode);
    }

    _tabMachine.clear();
    for (i = 0; i < pOtherEtage->_tabMachine.size(); i++)
    {
        LPTYMachineGeoNode pMachineGeoNode = new TYMachineGeoNode(new TYMachine);
        pMachineGeoNode->deepCopy(pOtherEtage->_tabMachine[i], copyId);
        _tabMachine.push_back(pMachineGeoNode);
    }

    // Recopie des etats des machines
    // Il n'est pas possible de recuperer les tableaux d'etats des machines
    // On en reconstruit un avec toutes les machines/sources en regimes 0
    // et non rayonnant pour tous les regimes

    _tabRegimesMachines.clear();
    for (i = 0 ; (int)i < getNbRegimes() ; i++)
    {
        TYMapPtrElementInt mapSources;
        for (j = 0 ; j < _tabSources.size() ; j++)
        {
            TYElement* pElem = _tabSources[j]->getElement();
            mapSources[pElem] = 0;
        }
        for (j = 0 ; j < _tabMachine.size() ; j++)
        {
            TYElement* pElem = _tabMachine[j]->getElement();
            mapSources[pElem] = 0;
        }
        _tabRegimesMachines.push_back(mapSources);
    }

    _tabEtatMachines.clear();
    for (i = 0 ; (int)i < getNbRegimes() ; i++)
    {
        TYMapPtrElementBool mapEtats;
        for (j = 0 ; j < _tabSources.size() ; j++)
        {
            TYElement* pElem = _tabSources[j]->getElement();
            mapEtats[pElem] = false;
        }
        for (j = 0 ; j < _tabMachine.size() ; j++)
        {
            TYElement* pElem = _tabMachine[j]->getElement();
            mapEtats[pElem] = false;
        }
        _tabEtatMachines.push_back(mapEtats);
    }

    _volEnglob = volEnglob();
    calculRayonSphere(_volEnglob);
    calculCentreGravite();

    return true;
}

std::string TYEtage::toString() const
{
    return "TYEtage";
}

DOM_Element TYEtage::toXML(DOM_Element& domElement)
{
    unsigned int i = 0;

    DOM_Element domNewElem = TYAcousticVolume::toXML(domElement);

    TYXMLTools::addElementIntValue(domNewElem, "typeReverberation", _typeReverb);
    TYXMLTools::addElementIntValue(domNewElem, "closed", _closed);

    // Sauvegarde de la paroi par defaut du bi¿½timent
    _pParoi->toXML(domNewElem);

    // Sauvegarde des murs
    DOM_Document domDoc = domElement.ownerDocument();
    DOM_Element listMurNode = domDoc.createElement("ListMur");
    domNewElem.appendChild(listMurNode);
    for (i = 0; i < _tabMur.size(); i++)
    {
        // Ajout du mur
        _tabMur[i]->toXML(listMurNode);
    }

    _pSol->toXML(domNewElem);
    _pPlafond->toXML(domNewElem);

    // Sauvegarde des machines
    DOM_Element listMachineNode = domDoc.createElement("ListMachine");
    domNewElem.appendChild(listMachineNode);
    for (i = 0; i < _tabMachine.size(); i++)
    {
        // Ajout de la machine si valide (i.e. contient des volumes)
        LPTYAcousticVolumeNode pNode = TYAcousticVolumeNode::safeDownCast(_tabMachine[i]->getElement());

        if (pNode && pNode->getNbChild() > 0) { _tabMachine[i]->toXML(listMachineNode); }
    }

    DOM_Element listSourceNode = domDoc.createElement("ListSource");
    domNewElem.appendChild(listSourceNode);
    for (i = 0 ; i < _tabSources.size() ; i++)
    {
        // Ajout de la source
        _tabSources[i]->toXML(listSourceNode);
    }

    // Sauvegarde de l'etat des machines
    DOM_Element listEtatNode = domDoc.createElement("TabEtatElement");
    domNewElem.appendChild(listEtatNode);

    unsigned int test = static_cast<unsigned int>(_tabRegimes.size());
    // Pour tous les regimes
    for (i = 0 ; i < _tabRegimes.size() ; i++)
    {
        if (_tabEtatMachines.size() == 0) { break; }

        DOM_Element tmpNode = domDoc.createElement("Regime");
        listEtatNode.appendChild(tmpNode);

        tmpNode.setAttribute("num", intToStr(i).data());

        TYMapPtrElementBool::iterator iter;
        for (iter = _tabEtatMachines[i].begin(); iter != _tabEtatMachines[i].end(); iter++)
        {
            DOM_Element itemNode = domDoc.createElement("Element");
            tmpNode.appendChild(itemNode);
            itemNode.setAttribute("accVolNodeId", (*iter).first->getID().toString());
            itemNode.setAttribute("state", intToStr((*iter).second).c_str());
        }
    }

    // Sauvegarde des regimes des machines
    DOM_Element listRegimeNode = domDoc.createElement("TabRegimeElement");
    domNewElem.appendChild(listRegimeNode);

    // Pour tous les regimes
    for (i = 0 ; i < _tabRegimes.size() ; i++)
    {
        if (_tabRegimesMachines.size() == 0) { break; }

        DOM_Element tmpNode = domDoc.createElement("Regime");
        listRegimeNode.appendChild(tmpNode);

        tmpNode.setAttribute("num", intToStr(i).data());

        TYMapPtrElementInt::iterator iter;
        for (iter = _tabRegimesMachines[i].begin(); iter != _tabRegimesMachines[i].end(); iter++)
        {
            DOM_Element itemNode = domDoc.createElement("Element");
            tmpNode.appendChild(itemNode);
            itemNode.setAttribute("accVolNodeId", (*iter).first->getID().toString());
            itemNode.setAttribute("regime", intToStr((*iter).second).c_str());
        }
    }


    return domNewElem;
}

int TYEtage::fromXML(DOM_Element domElement)
{
    TYAcousticVolume::fromXML(domElement);

    // Reset
    _tabMur.clear();
    _tabSources.clear();
    _tabMachine.clear();
    _tabRegimesMachines.clear();
    _tabEtatMachines.clear();

    bool closedOk = false;
    bool solFound = false;
    bool plafondFound = false;
    bool reverbOk = false;
    bool tabEtatElemFound = false;
    bool tabRegimeElemFound = false;

    LPTYMurGeoNode pMurGeoNode = new TYMurGeoNode(new TYMur, this);
    LPTYSourcePonctuelleGeoNode pSourceGeoNode = new TYSourcePonctuelleGeoNode(new TYSourcePonctuelle);
    LPTYMachineGeoNode pMachineGeoNode = new TYMachineGeoNode(new TYMachine, this);
    int retVal = -1;
    unsigned int i, j;
    DOM_Element elemCur;

    // Cri¿½ation d'une liste de doublons et un flag de di¿½tection de doublon (correction du bug 0006019)
    std::map<TYUUID, TYUUID> mapDoublons;
    // Indicateur d'existence de doublon;
    bool bDoublon = false;


    QDomNodeList childs = domElement.childNodes();
    for (i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();
        TYXMLTools::getElementBoolValue(elemCur, "closed", _closed, closedOk);
        TYXMLTools::getElementIntValue(elemCur, "typeReverberation", _typeReverb, reverbOk);

        _pParoi->callFromXMLIfEqual(elemCur);

        if (elemCur.nodeName() == "ListMur")
        {
            DOM_Element elemCur2;
            QDomNodeList childs2 = elemCur.childNodes();

            for (j = 0; j < childs2.length(); j++)
            {
                elemCur2 = childs2.item(j).toElement();
                if (pMurGeoNode->callFromXMLIfEqual(elemCur2, &retVal))
                {
                    if (retVal == 1)
                    {
                        _tabMur.push_back(pMurGeoNode);
                        pMurGeoNode = new TYMurGeoNode(NULL, this);
                    }
                }
            }
        }

        if (!solFound)
        {
            solFound = _pSol->callFromXMLIfEqual(elemCur);
        }
        else if (!plafondFound)
        {
            plafondFound = _pPlafond->callFromXMLIfEqual(elemCur);
        }

        if (elemCur.nodeName() == "ListMachine")
        {
            DOM_Element elemCur2;
            QDomNodeList childs2 = elemCur.childNodes();

            // Cas des bi¿½timents important des machines, si non perte du lien ri¿½gime bat/ri¿½gime machine
            bool bBak = TYElement::getRegenerateID();
            TYElement::setRegenerateID(false);

            for (j = 0; j < childs2.length(); j++)
            {
                elemCur2 = childs2.item(j).toElement();

                if (pMachineGeoNode->callFromXMLIfEqual(elemCur2, &retVal))
                {
                    if (retVal == 1)
                    {
                        // Vi¿½rification de l'existence eventuelle d'un doublon d'Id
                        TYElement* pElem = pMachineGeoNode->getElement();
                        TYUUID idOld = pElem->getID();
                        if (TYElement::testId(idOld, pElem))
                        {
                            // On change l'Id de l'element
                            pElem->regenerateID();
                            TYUUID idNew = pElem->getID();
                            // Ajout dans la liste des doublons
                            mapDoublons[idOld] = idNew;
                            // Un doublon constati¿½
                            bDoublon = true;
                        }
                        _tabMachine.push_back(pMachineGeoNode);
                        pMachineGeoNode = new TYMachineGeoNode(NULL, this);
                    }
                }
            }

            // Restauration de l'etat anterieur de regeneration de l'ID
            TYElement::setRegenerateID(bBak);
        }

        if (elemCur.nodeName() == "ListSource")
        {
            DOM_Element elemCur2;
            QDomNodeList childs2 = elemCur.childNodes();

            // Cas des bi¿½timents important des machines, si non perte du lien ri¿½gime bat/ri¿½gime source
            bool bBak = TYElement::getRegenerateID();
            TYElement::setRegenerateID(false);

            for (j = 0; j < childs2.length(); j++)
            {
                elemCur2 = childs2.item(j).toElement();

                if (pSourceGeoNode->callFromXMLIfEqual(elemCur2, &retVal))
                {
                    if (retVal == 1)
                    {
                        // Si fichier 3.1 besoin de transformer TYSourcePonctuelle en TYUserSourcePonctuelle
                        if (pSourceGeoNode->getElement()->isA("TYSourcePonctuelle"))
                        {
                            TYSourcePonctuelle* pSP = TYSourcePonctuelle::safeDownCast(pSourceGeoNode->getElement());
                            TYUserSourcePonctuelle* pUSP = new TYUserSourcePonctuelle(pSP);
                            pSourceGeoNode->setElement((TYElement*) pUSP);
                        }

                        // Vi¿½rification de l'existence eventuelle d'un doublon d'Id
                        TYElement* pElem = pSourceGeoNode->getElement();
                        TYUUID idOld = pElem->getID();
                        if (TYElement::testId(idOld, pElem))
                        {
                            // On change l'Id de l'element
                            pElem->regenerateID();
                            TYUUID idNew = pElem->getID();
                            // Ajout dans la liste des doublons
                            mapDoublons[idOld] = idNew;
                            // Un doublon constati¿½
                            bDoublon = true;
                        }

                        pSourceGeoNode->getElement()->setParent(this);
                        _tabSources.push_back(pSourceGeoNode);
                        pSourceGeoNode = new TYUserSourcePonctuelleGeoNode(NULL, this);
                    }
                }
            }

            // Restauration de l'etat anterieur de regeneration de l'ID
            TYElement::setRegenerateID(bBak);
        }

        if (elemCur.nodeName() == "TabEtatElement")
        {
            tabEtatElemFound = true;
            DOM_Element elemCur3;
            QDomNodeList childs3 = elemCur.childNodes();
            // Boucle sur les regimes (de l'etage)
            for (j = 0; j < childs3.length(); j++)
            {
                elemCur3 = childs3.item(j).toElement();
                int regimeEtage = 0;
                bool regimeMachineOk = false;

                if (elemCur3.nodeName() == "Regime")
                {
                    TYMapPtrElementBool mapEtatRegime;

                    // On relit le numero de regime
                    TYXMLTools::getElementIntValue(elemCur3, "num", regimeEtage, regimeMachineOk);

                    DOM_Element elemCur4;

                    QDomNodeList childs4 = elemCur3.childNodes();

                    // On boucle sur tous les elements
                    for (unsigned int k = 0 ; k < childs4.length(); k++)
                    {
                        //                        bool idOk = false;
                        //                        bool elemOk = false;
                        QString id, oldId;
                        bool etat = false;
                        elemCur4 = childs4.item(k).toElement();

                        if (elemCur4.nodeName() == "Element")
                        {
                            id = TYXMLTools::getElementAttributeToString(elemCur4, "accVolNodeId");
                            oldId = id;
                            etat = TYXMLTools::getElementAttributeToInt(elemCur4, "state");
                        }

                        // Recuperation d'un pointeur sur l'element
                        if (bDoublon) // Substitution en cas de doublon di¿½tecti¿½
                        {
                            TYUUID id2 = TYElement::fromString(id);
                            id2 = mapDoublons[id2];
                            id = TYElement::toString(id2);
                        }

                        TYElement* pElement = TYElement::getInstance(id); // On essaye avec l'id

                        if (!pElement) { pElement = TYElement::getInstance(oldId); } // Si ca marche pas on essaye l'ancien


                        // Ajout de l'element dans la table des etats
                        if (pElement)
                        {
                            mapEtatRegime[pElement] = etat;
                        }
                    }

                    // On rajoute le map des etats dans le tableau de l'etage
                    _tabEtatMachines.push_back(mapEtatRegime);
                }
            }
        }

        if (elemCur.nodeName() == "TabRegimeElement")
        {
            tabRegimeElemFound = true;

            DOM_Element elemCur3;

            QDomNodeList childs3 = elemCur.childNodes();

            // Boucle sur les regimes (de l'etage)
            for (j = 0; j < childs3.length(); j++)
            {
                elemCur3 = childs3.item(j).toElement();
                int regimeEtage = 0;
                bool regimeMachineOk = false;
                if (elemCur3.nodeName() == "Regime")
                {
                    TYMapPtrElementInt mapRegimeMachine;

                    // On relit le numero de regime
                    TYXMLTools::getElementIntValue(elemCur3, "num", regimeEtage, regimeMachineOk);

                    DOM_Element elemCur4;

                    QDomNodeList childs4 = elemCur3.childNodes();
                    // On boucle sur tous les elements
                    for (unsigned int k = 0 ; k < childs4.length(); k++)
                    {
                        elemCur4 = childs4.item(k).toElement();
                        //                        bool idOk = false;
                        //                        bool elemOk = false;
                        QString id, oldId;
                        int regimeMachine = 0;

                        if (elemCur4.nodeName() == "Element")
                        {
                            id = TYXMLTools::getElementAttributeToString(elemCur4, "accVolNodeId");
                            oldId = id;
                            regimeMachine = TYXMLTools::getElementAttributeToInt(elemCur4, "regime");
                        }

                        // Recuperation d'un pointeur sur l'element
                        if (bDoublon) // Substitution en cas de doublon di¿½tecti¿½
                        {
                            TYUUID id2 = TYElement::fromString(id);
                            id2 = mapDoublons[id2];
                            id = TYElement::toString(id2);
                        }

                        TYElement* pElement = TYElement::getInstance(id);

                        if (!pElement) { pElement = TYElement::getInstance(oldId); } // Si ca marche pas on essaye l'ancien

                        // Ajout de l'element dans la table des etats
                        if (pElement)
                        {
                            mapRegimeMachine[pElement] = regimeMachine;
                        }
                    }

                    // On rajoute le map des etats dans le tableau de l'etage
                    _tabRegimesMachines.push_back(mapRegimeMachine);
                }
            }
        }

    }

    if (!(tabEtatElemFound && tabRegimeElemFound)) // Cas ancien format de fichier
    {
        // On commence par vider les deux tableaux au cas ou un seul des deux aurait ete trouve
        _tabRegimesMachines.clear();
        _tabEtatMachines.clear();

        // Creation d'un map des etats et d'un map des regimes des machines
        TYMapPtrElementBool mapEtatRegime;
        TYMapPtrElementInt mapRegimeMachine;
        _tabRegimesMachines.push_back(mapRegimeMachine);
        _tabEtatMachines.push_back(mapEtatRegime);

        // Les sources (et machines) seront ri¿½affecti¿½es automtiquement par la fonction verifTabRegimeSources()
    }

    updateSolPlafond();
    _volEnglob = volEnglob();
    calculRayonSphere(_volEnglob);
    calculCentreGravite();

    if (_forceNormales) { inverseNormales(); }

    verifTabRegimeSources();

    return 1;
}

void TYEtage::verifTabRegimeSources()
{
    bool bFound = false;

    for (unsigned int i = 0; i < getNbRegimes() ; i++)
    {
        TYMapPtrElementBool& mapEtatRegime = _tabEtatMachines[i];
        TYMapPtrElementInt& mapRegimeMachine = _tabRegimesMachines[i];

        for (unsigned int j = 0; j < _tabSources.size(); j++) // Pour toutes les sources
        {
            bFound = false;
            TYElement* pElement = _tabSources[j]->getElement();
            if (!pElement->inherits("TYSource")) { continue; }

            std::map<TYElement*, bool>::iterator itEtat;

            for (itEtat = mapEtatRegime.begin(); itEtat != mapEtatRegime.end(); itEtat++)
            {
                if (itEtat->first == pElement)
                {
                    bFound = true;
                    break;
                }
            }

            if (!bFound)
            {
                mapEtatRegime[pElement] = false;
                mapRegimeMachine[pElement] = 0;
            }
        }

        for (unsigned int j = 0; j < _tabMachine.size(); j++) // Pour toutes les machines
        {
            bFound = false;
            TYElement* pElement = _tabMachine[j]->getElement();
            if (!pElement->isA("TYMachine")) { continue; }

            std::map<TYElement*, bool>::iterator itEtat;

            for (itEtat = mapEtatRegime.begin(); itEtat != mapEtatRegime.end(); itEtat++)
            {
                if (itEtat->first == pElement)
                {
                    bFound = true;
                    break;
                }
            }

            if (!bFound)
            {
                mapEtatRegime[pElement] = false;
                mapRegimeMachine[pElement] = 0;
            }
        }
    }
}


void TYEtage::exportCSV(std::ofstream& ofs)
{
    // Export du nom de l'objet
    ofs << _name.toStdString() << '\n';

    // Export des donnees acoustiques
    TYAcousticInterface::exportCSV(ofs);

    for (unsigned int i = 0; i < _tabMur.size(); i++)
    {
        TYMur* pMur = TYMur::safeDownCast(_tabMur[i]->getElement());
        pMur->exportCSV(ofs);
    }

    // On oublie pas les sols et plafond
    _pSol->exportCSV(ofs);
    _pPlafond->exportCSV(ofs);

    ofs << '\n';
}

void TYEtage::getChilds(LPTYElementArray& childs, bool recursif /*=true*/)
{
    TYAcousticVolume::getChilds(childs, recursif);

    unsigned int i;

    for (i = 0; i < _tabMur.size(); i++)
    {
        childs.push_back(_tabMur[i]->getElement());
    }

    childs.push_back(_pSol);
    childs.push_back(_pPlafond);

    if (recursif)
    {
        for (i = 0; i < _tabMur.size(); i++)
        {
            _tabMur[i]->getChilds(childs, recursif);
        }

        _pSol->getChilds(childs, recursif);
        _pPlafond->getChilds(childs, recursif);

        for (i = 0; i < _tabMachine.size(); i++)
        {
            _tabMachine[i]->getChilds(childs, recursif);
        }

        for (i = 0 ; i < _tabSources.size() ; i++)
        {
            _tabSources[i]->getChilds(childs, recursif);
        }
    }
}

bool TYEtage::addMur(LPTYMur pMur)
{
    assert(pMur);

    pMur->setParent(this);
    pMur->setParoi(_pParoi);

    _tabMur.push_back(new TYMurGeoNode((LPTYElement)pMur));

    updateSolPlafond();
    _volEnglob = volEnglob();
    calculRayonSphere(_volEnglob);
    calculCentreGravite();

    return true;
}

bool TYEtage::remMur(const LPTYMur pMur)
{
    assert(pMur);
    bool ret = false;
    TYTabMurGeoNode::iterator ite;

    for (ite = _tabMur.begin(); ite != _tabMur.end(); ite++)
    {
        if (TYMur::safeDownCast((*ite)->getElement()) == pMur)
        {
            _tabMur.erase(ite);
            ret = true;
            break;
        }
    }

    updateSolPlafond();
    _volEnglob = volEnglob();
    calculRayonSphere(_volEnglob);
    calculCentreGravite();

    return ret;
}

bool TYEtage::remMur(QString idMur)
{
    bool ret = false;
    TYTabMurGeoNode::iterator ite;

    for (ite = _tabMur.begin(); ite != _tabMur.end(); ite++)
    {
        if ((*ite)->getElement()->getID().toString() == idMur)
        {
            _tabMur.erase(ite);
            ret = true;
            break;
        }
    }

    updateSolPlafond();
    _volEnglob = volEnglob();
    calculRayonSphere(_volEnglob);
    calculCentreGravite();

    return ret;
}

void TYEtage::remMurs()
{
    _tabMur.clear();
    updateSolPlafond();
    _volEnglob = volEnglob();
    calculRayonSphere(_volEnglob);
    calculCentreGravite();
}

bool TYEtage::setMurs(const TYTabPoint& tabPts, double hauteur /*=2.0*/, bool close /*=true*/)
{
    TYPoint pt0, pt1, pt2, pt3;
    size_t count = tabPts.size();
    TYRepere repMur;

    if ((count == 0) || (hauteur <= 0.0))
    {
        return false;
    }

    // Reset
    _tabMur.clear();

    // S'il y a moins de 2 murs, l'etage ne peut pas etre ferme
    if (tabPts.size() > 1)
    {
        // On conserve l'etat
        _closed = close;
    }
    else
    {
        _closed = false;
    }

    // On teste le 1er point et le dernier point sont confondus avec une tolerance
    if ((OCoord3D) tabPts[0] == (OCoord3D) tabPts[count - 1])
    {
        // On ne reprend pas le dernier point
        count -= 1;
    }
    else if (!_closed)
    {
        // On s'arrete avant de fermer les murs
        count -= 1;
    }

    // Une face pour chaque couple de points qui se suivent
    for (int i = 0; i < count; i++)
    {
        pt1 = pt2 = tabPts[i];
        pt0 = pt3 = tabPts[(i + 1) % tabPts.size()];

        // Vecteur pour la "longueur" de la face
        OVector3D vec01(pt0, pt1);

        // Le mur pour cette face
        LPTYMur pMur = new TYMur();

        pMur->setParent(this);
        // Dimension du mur
        pMur->setSize(vec01.norme(), hauteur);

        // Position du mur
        repMur._origin = OVector3D(pt0) + (vec01 * 0.5);
        repMur._origin._z = hauteur / 2.0;

        // Orientation du mur
        vec01.normalize();
        repMur._vecI = vec01;
        repMur._vecJ = OVector3D(0.0, 0.0, 1.0);
        repMur._vecK = vec01.cross(repMur._vecJ);

        LPTYMurGeoNode pMurGeoNode = new TYMurGeoNode();
        pMurGeoNode->setRepere(repMur);
        pMurGeoNode->setElement((LPTYElement) pMur);
        _tabMur.push_back(pMurGeoNode);
    }

    updateSolPlafond();
    _volEnglob = volEnglob();
    calculRayonSphere(_volEnglob);
    calculCentreGravite();

    return true;
}

TYTabPoint TYEtage::getContour() const
{
    TYTabPoint res;
    size_t nbPts = _tabMur.size();
    size_t i;

    if (!nbPts) { return res; }

    res.reserve(nbPts);

    for (i = 0; i < nbPts; i++)
    {
        // On recupere le point
        TYPoint pt = TYMur::safeDownCast(_tabMur[i]->getElement())->getBoundingRect()->_pts[2];

        // On passe dans le repere de l'etage
        pt = _tabMur[i]->getMatrix() * pt;

        // Ajout du point
        res.push_back(pt);
    }

    // Si l'etage n'est pas ferme (ecran), on rajoute le dernier
    // point manuellement
    if (!_closed)
    {
        // On recupere le point
        TYPoint pt = TYMur::safeDownCast(_tabMur[i - 1]->getElement())->getBoundingRect()->_pts[3];

        // On passe dans le repere de l'etage
        pt = _tabMur[i - 1]->getMatrix() * pt;

        // Ajout du point
        res.push_back(pt);
    }

    return res;
}

void TYEtage::setHauteur(double hauteur)
{
    size_t nbPts = _tabMur.size();

    if (nbPts <= 0)
    {
        return;
    }

    for (size_t i = 0; i < nbPts; i++)
    {
        // On applique la nouvelle hauteur
        TYMur::safeDownCast(_tabMur[i]->getElement())->setSizeY(hauteur);

        // On positionne le centre du mur a la 1/2 hauteur
        TYRepere rep = _tabMur[i]->getRepere();
        rep._origin._z = hauteur / 2.0;
        _tabMur[i]->setRepere(rep);
    }

    updateSolPlafond();
    _volEnglob = volEnglob();
    calculRayonSphere(_volEnglob);
    calculCentreGravite();
}

double TYEtage::getHauteur() const
{
    double res = 0.0;

    if (_tabMur.size() > 0)
    {
        res = TYMur::safeDownCast(_tabMur[0]->getElement())->getSizeY();
    }

    return res;
}

bool TYEtage::addMachine(LPTYMachineGeoNode pMachineGeoNode)
{
    assert(pMachineGeoNode);

    TYMachine* pMachine = TYMachine::safeDownCast(pMachineGeoNode->getElement());

    assert(pMachine);

    pMachineGeoNode->setParent(this);
    pMachine->setParent(this);

    _tabMachine.push_back(pMachineGeoNode);

    // Pour tous les regimes de l'etage
    TYElement* pElement = (TYElement*) pMachine;
    for (unsigned int i = 0 ; i < _tabRegimes.size() ; i++)
    {
        // On rajoute la machine dans le tableau associatif du regime
        _tabRegimesMachines[i][pElement] = 0;

        // On met la machine active pour ce regime
        _tabEtatMachines[i][pElement] = true;
    }

    setIsGeometryModified(true);

    return true;
}

bool TYEtage::addMachine(LPTYMachine pMachine, const TYRepere& pos)
{
    return addMachine(new TYMachineGeoNode(pos, (LPTYElement)pMachine));
}

bool TYEtage::addMachine(LPTYMachine pMachine)
{
    return addMachine(new TYMachineGeoNode((LPTYElement)pMachine));
}

bool TYEtage::remMachine(const LPTYMachineGeoNode pMachineGeoNode)
{
    assert(pMachineGeoNode);
    bool ret = false;
    TYTabMachineGeoNode::iterator ite;

    for (ite = _tabMachine.begin(); ite != _tabMachine.end(); ite++)
    {
        if ((*ite) == pMachineGeoNode)
        {
            _tabMachine.erase(ite);
            ret = true;
            break;
        }
    }

    // Pour tous les regimes de l'etage
    TYElement* pElement = pMachineGeoNode->getElement();
    TYMapPtrElementInt::iterator iter;
    TYMapPtrElementBool::iterator itb;
    for (unsigned int i = 0 ; i < _tabRegimes.size() ; i++)
    {
        iter = _tabRegimesMachines[i].find(pElement);
        // On supprime la machine dans le tableau associatif du regime
        _tabRegimesMachines[i].erase(iter);

        itb = _tabEtatMachines[i].find(pElement);
        _tabEtatMachines[i].erase(itb);
    }


    setIsGeometryModified(true);

    return ret;
}

bool TYEtage::remMachine(const LPTYMachine pMachine)
{
    assert(pMachine);
    bool ret = false;
    TYTabMachineGeoNode::iterator ite;

    for (ite = _tabMachine.begin(); ite != _tabMachine.end(); ite++)
    {
        if (TYMachine::safeDownCast((*ite)->getElement()) == pMachine)
        {
            _tabMachine.erase(ite);
            ret = true;
            break;
        }
    }

    // Pour tous les regimes de l'etage
    TYElement* pElement = (TYElement*) pMachine;
    TYMapPtrElementInt::iterator iter;
    TYMapPtrElementBool::iterator itb;
    for (unsigned int i = 0 ; i < _tabRegimes.size() ; i++)
    {
        iter = _tabRegimesMachines[i].find(pElement);
        // On supprime la machine dans le tableau associatif du regime
        _tabRegimesMachines[i].erase(iter);

        itb = _tabEtatMachines[i].find(pElement);
        _tabEtatMachines[i].erase(itb);
    }


    setIsGeometryModified(true);

    return ret;
}

bool TYEtage::remMachine(QString idMachine)
{
    bool ret = false;
    TYTabMachineGeoNode::iterator ite;

    for (ite = _tabMachine.begin(); ite != _tabMachine.end(); ite++)
    {
        if ((*ite)->getElement()->getID().toString() == idMachine)
        {
            _tabMachine.erase(ite);
            ret = true;
            break;
        }
    }

    // Pour tous les regimes de l'etage
    TYElement* pElement = (*ite)->getElement();
    TYMapPtrElementInt::iterator iter;
    TYMapPtrElementBool::iterator itb;
    for (unsigned int i = 0 ; i < _tabRegimes.size() ; i++)
    {
        iter = _tabRegimesMachines[i].find(pElement);
        // On supprime la machine dans le tableau associatif du regime
        _tabRegimesMachines[i].erase(iter);

        itb = _tabEtatMachines[i].find(pElement);
        _tabEtatMachines[i].erase(itb);
    }
    setIsGeometryModified(true);

    return ret;
}

void TYEtage::remAllMachine()
{
    unsigned int i, j;

    TYMapPtrElementInt::iterator iter;
    for (i = 0 ; i < _tabRegimesMachines.size(); i++)
    {
        for (j = 0 ; j < _tabMachine.size(); j++)
        {
            TYElement* pElement = _tabMachine[j]->getElement();
            iter = _tabRegimesMachines[i].find(pElement);
            _tabRegimesMachines[i].erase(iter);
        }
    }

    TYMapPtrElementBool::iterator itb;
    for (i = 0 ; i < _tabEtatMachines.size(); i++)
    {
        for (j = 0 ; j < _tabMachine.size(); j++)
        {
            TYElement* pElement = _tabMachine[j]->getElement();
            itb = _tabEtatMachines[i].find(pElement);
            _tabEtatMachines[i].erase(itb);
        }
    }

    _tabMachine.clear();
    setIsGeometryModified(true);
}

LPTYMachineGeoNode TYEtage::findMachine(const LPTYMachine pMachine)
{
    assert(pMachine);
    TYTabMachineGeoNode::iterator ite;

    for (ite = _tabMachine.begin(); ite != _tabMachine.end(); ite++)
    {
        if (TYMachine::safeDownCast((*ite)->getElement()) == pMachine)
        {
            return (*ite);
        }
    }

    return NULL;
}

bool TYEtage::addSource(LPTYUserSourcePonctuelleGeoNode pSourceGeoNode)
{
    assert(pSourceGeoNode);
    assert(pSourceGeoNode->getElement());

    if (this->isA("TYAcousticCylinder")) { return false; }

    pSourceGeoNode->setParent(this);
    pSourceGeoNode->getElement()->setParent(this);

    _tabSources.push_back(pSourceGeoNode);

    // Pour tous les regimes de l'etage
    TYElement* pElement = pSourceGeoNode->getElement();
    for (unsigned int i = 0 ; i < _tabRegimes.size() ; i++)
    {
        // On rajoute la machine dans le tableau associatif du regime
        _tabRegimesMachines[i][pElement] = 0;
        // On met la machine active pour ce regime
        _tabEtatMachines[i][pElement] = true;
    }
    setIsGeometryModified(true);

    return true;
}

bool TYEtage::addSource(LPTYUserSourcePonctuelle pSource, const TYRepere& pos)
{
    return addSource(new TYUserSourcePonctuelleGeoNode(pos, (LPTYElement)pSource));
}

bool TYEtage::addSource(LPTYUserSourcePonctuelle pSource)
{
    return addSource(new TYUserSourcePonctuelleGeoNode((LPTYElement)pSource));
}

bool TYEtage::remSource(const LPTYUserSourcePonctuelleGeoNode pSourceGeoNode)
{
    assert(pSourceGeoNode);

    bool ret = false;
    TYTabUserSourcePonctuelleGeoNode::iterator ite;

    for (ite = _tabSources.begin(); ite != _tabSources.end(); ite++)
    {
        if ((*ite) == pSourceGeoNode)
        {
            _tabSources.erase(ite);
            ret = true;
            break;
        }
    }

    // Pour tous les regimes de l'etage
    TYElement* pElement = (*ite)->getElement();
    TYMapPtrElementInt::iterator iter;
    TYMapPtrElementBool::iterator itb;
    for (unsigned int i = 0 ; i < _tabRegimes.size() ; i++)
    {
        iter = _tabRegimesMachines[i].find(pElement);
        // On supprime la machine dans le tableau associatif du regime
        _tabRegimesMachines[i].erase(iter);

        itb = _tabEtatMachines[i].find(pElement);
        _tabEtatMachines[i].erase(itb);
    }
    setIsGeometryModified(true);

    return ret;
}

bool TYEtage::remSource(const LPTYUserSourcePonctuelle pSource)
{
    assert(pSource);
    bool ret = false;
    TYTabUserSourcePonctuelleGeoNode::iterator ite;

    for (ite = _tabSources.begin(); ite != _tabSources.end(); ite++)
    {
        if (TYUserSourcePonctuelle::safeDownCast((*ite)->getElement()) == pSource)
        {
            _tabSources.erase(ite);
            ret = true;
            break;
        }
    }

    // Pour tous les regimes de l'etage
    TYElement* pElement = (TYElement*) pSource;
    TYMapPtrElementInt::iterator iter;
    TYMapPtrElementBool::iterator itb;
    for (unsigned int i = 0 ; i < _tabRegimes.size() ; i++)
    {
        iter = _tabRegimesMachines[i].find(pElement);
        // On supprime la machine dans le tableau associatif du regime
        _tabRegimesMachines[i].erase(iter);

        itb = _tabEtatMachines[i].find(pElement);
        _tabEtatMachines[i].erase(itb);
    }

    setIsGeometryModified(true);

    return ret;
}

bool TYEtage::remSource(QString idSource)
{
    bool ret = false;
    TYTabUserSourcePonctuelleGeoNode::iterator ite;

    for (ite = _tabSources.begin(); ite != _tabSources.end(); ite++)
    {
        if (TYUserSourcePonctuelle::safeDownCast((*ite)->getElement())->getID().toString() == idSource)
        {
            _tabSources.erase(ite);
            ret = true;
            break;
        }
    }

    TYElement* pElement = (*ite)->getElement();
    TYMapPtrElementInt::iterator iter;
    TYMapPtrElementBool::iterator itb;
    for (unsigned int i = 0 ; i < _tabRegimes.size() ; i++)
    {
        iter = _tabRegimesMachines[i].find(pElement);
        // On supprime la machine dans le tableau associatif du regime
        _tabRegimesMachines[i].erase(iter);

        itb = _tabEtatMachines[i].find(pElement);
        _tabEtatMachines[i].erase(itb);
    }

    setIsGeometryModified(true);

    return ret;
}

void TYEtage::remAllSources()
{
    unsigned int i, j;

    TYMapPtrElementInt::iterator iter;
    for (i = 0 ; i < _tabRegimesMachines.size(); i++)
    {
        for (j = 0 ; j < _tabSources.size(); j++)
        {
            TYElement* pElement = _tabSources[j]->getElement();
            iter = _tabRegimesMachines[i].find(pElement);
            _tabRegimesMachines[i].erase(iter);
        }
    }

    TYMapPtrElementBool::iterator itb;
    for (i = 0 ; i < _tabEtatMachines.size(); i++)
    {
        for (j = 0 ; j < _tabSources.size(); j++)
        {
            TYElement* pElement = _tabSources[j]->getElement();
            itb = _tabEtatMachines[i].find(pElement);
            _tabEtatMachines[i].erase(itb);
        }
    }

    _tabSources.clear();

    setIsGeometryModified(true);
}

LPTYUserSourcePonctuelleGeoNode TYEtage::findSource(const LPTYUserSourcePonctuelle pSource)
{
    assert(pSource);
    TYTabUserSourcePonctuelleGeoNode::iterator ite;

    for (ite = _tabSources.begin(); ite != _tabSources.end(); ite++)
    {
        if (TYUserSourcePonctuelle::safeDownCast((*ite)->getElement()) == pSource)
        {
            return (*ite);
        }
    }

    return NULL;
}

void TYEtage::setRegime(TYSpectre& Spectre, int regime /*=-1*/, bool recursif /*=true*/)
{
    if (recursif)
    {
        for (unsigned int i = 0; i < _tabMur.size(); i++)
        {
            TYMur::safeDownCast(_tabMur[i]->getElement())->setRegime(Spectre, regime, recursif);
        }
    }

    TYAcousticVolume::setRegime(Spectre, regime, recursif);
}

bool TYEtage::remRegime(int regime)
{
    for (unsigned int i = 0; i < _tabMur.size(); i++)
    {
        TYMur::safeDownCast(_tabMur[i]->getElement())->remRegime(regime);
    }

    _pSol->remRegime(regime);
    _pPlafond->remRegime(regime);

    // On supprime le tableau associatif regime/"regime machine"
    _tabRegimesMachines.erase(_tabRegimesMachines.begin() + regime);
    _tabEtatMachines.erase(_tabEtatMachines.begin() + regime);

    return TYAcousticVolume::remRegime(regime);
}

void TYEtage::setCurRegime(int regime)
{
    if (regime >= (int) _tabRegimesMachines.size()) { return; }
    if (regime < 0) { regime = (int)_tabRegimes.size() - 1; }

    for (unsigned int i = 0; i < _tabMur.size(); i++)
    {
        TYMur::safeDownCast(_tabMur[i]->getElement())->setCurRegime(regime);
    }

    _pSol->setCurRegime(regime);
    _pPlafond->setCurRegime(regime);


    // On met les machines au regime qui correspond
    TYMapPtrElementInt::iterator iter;
    for (iter = _tabRegimesMachines[regime].begin(); iter != _tabRegimesMachines[regime].end() ; iter++)
    {
        TYElement* pElement = (*iter).first;
        if (pElement->isA("TYMachine"))
        {
            LPTYMachine pMachine = TYMachine::safeDownCast(pElement);
            assert(pMachine);
            pMachine->setCurRegime((*iter).second);
        }
        else
        {
            LPTYUserSourcePonctuelle pSource = TYUserSourcePonctuelle::safeDownCast(pElement);
            assert(pSource);
            pSource->setCurrentRegime((*iter).second);
        }
    }

    // On met les machines dans l'etat (rayonnant/non rayonnant) qui correspond
    TYMapPtrElementBool::iterator iter2;
    for (iter2 = _tabEtatMachines[regime].begin(); iter2 != _tabEtatMachines[regime].end(); iter2++)
    {
        TYElement* pElement = (*iter2).first;
        if (pElement->isA("TYMachine"))
        {
            LPTYMachine pMachine = TYMachine::safeDownCast(pElement);
            assert(pMachine);
            bool etat = (*iter2).second;
            pMachine->setIsRayonnant(etat);
        }
        else
        {
            LPTYUserSourcePonctuelle pSource = TYUserSourcePonctuelle::safeDownCast(pElement);
            assert(pSource);
            pSource->setIsRayonnant((*iter2).second);
        }
    }

    TYAcousticVolume::setCurRegime(regime);

    setIsAcousticModified(true);
}

int TYEtage::addRegime(TYRegime regime)
{
    int value = TYAcousticInterface::addRegime(regime);

    TYMapPtrElementInt mapElmRegime = _tabRegimesMachines[0];
    _tabRegimesMachines.push_back(mapElmRegime);

    TYMapPtrElementBool mapElmBool = _tabEtatMachines[0];
    _tabEtatMachines.push_back(mapElmBool);

    // Creation du nouveau regime pour tous les murs ainsi que le sol et le plafond
    for (unsigned int i = 0; i < _tabMur.size(); i++)
    {
        TYMur::safeDownCast(_tabMur[i]->getElement())->addRegime();
    }

    _pSol->addRegime();
    _pPlafond->addRegime();

    return value;
}

int TYEtage::addRegime()
{
    int value = TYAcousticInterface::addRegime();

    TYMapPtrElementInt mapElmRegime = _tabRegimesMachines[0];
    _tabRegimesMachines.push_back(mapElmRegime);

    TYMapPtrElementBool mapElmBool = _tabEtatMachines[0];
    _tabEtatMachines.push_back(mapElmBool);

    // Creation du nouveau regime pour tous les murs ainsi que le sol et le plafond
    for (unsigned int i = 0; i < _tabMur.size(); i++)
    {
        TYMur::safeDownCast(_tabMur[i]->getElement())->addRegime();
    }

    _pSol->addRegime();
    _pPlafond->addRegime();

    return value;
}

void TYEtage::setNextRegimeNb(const int& next)
{
    _nextRegime = next;

    for (unsigned int i = 0; i < _tabMur.size(); i++)
    {
        TYMur::safeDownCast(_tabMur[i]->getElement())->setNextRegimeNb(next);
    }

    _pSol->setNextRegimeNb(next);
    _pPlafond->setNextRegimeNb(next);
}

void TYEtage::setRegimeName(const QString& name)
{
    bool status = true;
    TYRegime& reg = getRegimeNb(_curRegime, status);
    if (status) { reg.setRegimeName(name); }

    for (unsigned int i = 0; i < _tabMur.size(); i++)
    {
        TYMur::safeDownCast(_tabMur[i]->getElement())->setRegimeName(name);
    }

    _pSol->setRegimeName(name);
    _pPlafond->setRegimeName(name);
}


TYTabSourcePonctuelleGeoNode TYEtage::getSrcs() const
{
    TYTabSourcePonctuelleGeoNode tab;
    TYTabSourcePonctuelleGeoNode tabChild;

    // On recupere seulement les sources des murs
    // L'etage est vue comme un volume acoustique quelconque
    for (unsigned int i = 0; i < _tabMur.size(); i++)
    {
        LPTYMur pMur = TYMur::safeDownCast(_tabMur[i]->getElement());
        // Recupere l'ensemble des srcs du child...
        if (pMur->getIsRayonnant())
        {
            tabChild = pMur->getSrcs();

            // Concatenation des matrices
            OMatrix matrix = _tabMur[i]->getMatrix();
            for (unsigned int j = 0; j < tabChild.size(); j++)
            {
                tabChild[j]->setMatrix(matrix * tabChild[j]->getMatrix());
            }

            //...et ajoute au tableau a retourner
            tab.insert(tab.end(), tabChild.begin(), tabChild.end());
        }
    }

    tabChild = _pSol->getSrcs();
    tab.insert(tab.end(), tabChild.begin(), tabChild.end());

    tabChild = _pPlafond->getSrcs();
    tab.insert(tab.end(), tabChild.begin(), tabChild.end());

    return tab;
}

TYSourcePonctuelle TYEtage::srcPonctEquiv() const
{
    return TYSourcePonctuelle();
}

void TYEtage::setDensiteSrcsH(double densite, bool recursif)
{
    TYAcousticVolume::setDensiteSrcsH(densite);

    if (recursif)
    {
        for (unsigned int i = 0; i < _tabMur.size(); i++)
        {
            TYMur::safeDownCast(_tabMur[i]->getElement())->setDensiteSrcsH(getDensiteSrcsH());
        }
        // Le sol est inactif
        // Plafond
        _pPlafond->setDensiteSrcsH(getDensiteSrcsH());
    }
}

void TYEtage::setDensiteSrcsV(double densite, bool recursif)
{
    TYAcousticVolume::setDensiteSrcsV(densite);

    if (recursif)
    {
        for (unsigned int i = 0; i < _tabMur.size(); i++)
        {
            TYMur::safeDownCast(_tabMur[i]->getElement())->setDensiteSrcsV(getDensiteSrcsV());
        }
        // Le sol est inactif
        // Plafond
        _pPlafond->setDensiteSrcsV(getDensiteSrcsV());
    }
}

void TYEtage::distriSrcs()
{
    unsigned int i;

    // On commence par effacer les sources presentes
    for (i = 0; i < _tabMur.size(); i++)
    {
        TYMur::safeDownCast(_tabMur[i]->getElement())->remAllSrcs();
    }

    _pSol->getSrcSurf()->remAllSrc();
    _pPlafond->getSrcSurf()->remAllSrc();

    // Murs
    for (i = 0; i < _tabMur.size(); i++)
    {
        LPTYMur pMur = TYMur::safeDownCast(_tabMur[i]->getElement());
        if (pMur->getIsRayonnant())
        {
            pMur->distriSrcs();
        }
    }

    // Sol
    if (_pSol->getIsRayonnant())
    {
        _pSol->distriSrcs();
    }

    // Plafond
    if (_pPlafond->getIsRayonnant())
    {
        _pPlafond->distriSrcs();
    }
}

bool TYEtage::setSrcsLw()
{
    bool ret = true;

    TYSpectre LWEtage = TYSpectre::getEmptyLinSpectre();
    LWEtage.setType(SPECTRE_TYPE_LW);

    TYSpectre LWElt = TYSpectre::getEmptyLinSpectre();
    TYSpectre LWMur = TYSpectre::getEmptyLinSpectre();


    // ON TRAITE D'ABORD LES MURS

    // Pour tous les murs
    for (unsigned int i = 0 ; i < _tabMur.size() ; i++)
    {
        // Matrice du mur
        OMatrix matMur = _tabMur[i]->getMatrix();

        // Mur courant
        LPTYMur pCurrentMur = TYMur::safeDownCast(_tabMur[i]->getElement());

        // Si le mur est rayonnant, on calcule sa puissance
        if (pCurrentMur->getIsRayonnant())
        {
            // Spectre de puissance affectee au mur
            double surfMur = pCurrentMur->activeSurface();

            // Si la puissance est calculee, la puissance du mur est mise a 0
            if (pCurrentMur->getTypeDistribution() == TY_PUISSANCE_CALCULEE)
            {
                LWMur = TYSpectre::getEmptyLinSpectre();
            }
            else // La puissance est imposee
            {
                LWMur = pCurrentMur->getCurrentSpectre().toGPhy();
            }

            LWMur.setType(SPECTRE_TYPE_LW);

            // Pour toutes les faces du mur
            TYTabAcousticSurfaceGeoNode tabAcousticSurf = pCurrentMur->getTabAcousticSurf();

            LPTYParoi pMurParoi = TYMur::safeDownCast(_tabMur[i]->getElement())->getParoi();

            // Pour toutes les surfaces constituant le mur
            for (unsigned int j = 0 ; j < tabAcousticSurf.size() ; j++)
            {
                LPTYAcousticSurfaceGeoNode pSurfaceNode = tabAcousticSurf[j];
                OMatrix matSurf = matMur * pSurfaceNode->getMatrix();

                LPTYAcousticSurface pCurrentSurf = TYAcousticSurface::safeDownCast(pSurfaceNode->getElement());
                assert(pCurrentSurf);


                if (pCurrentSurf->getIsSub() == true)
                {
                    if (pCurrentSurf->getTypeDistribution() == TY_PUISSANCE_CALCULEE)
                    {
                        LWElt = getPuissanceRayonnee(pCurrentSurf, matSurf, pMurParoi);
                        LWEtage = LWEtage.sum(LWElt);
                    }
                    else
                    {
                        LWEtage = LWEtage.sum(pCurrentSurf->getCurrentSpectre().toGPhy());
                    }
                }
                else
                {
                    if (pCurrentMur->getTypeDistribution() == TY_PUISSANCE_CALCULEE)
                    {
                        LWElt = getPuissanceRayonnee(pCurrentSurf, matSurf, pMurParoi);
                        LWMur = LWMur.sum(LWElt);
                    }
                    else
                    {
                        double surfElt = pCurrentSurf->surface();
                        LWElt = LWMur.mult(surfElt / surfMur);
                    }
                }

                // CORRECTION ANOMALIE 28 - test du type d'attribution de la puissance (calculee/imposee)
                if (pCurrentSurf->getTypeDistribution() == TY_PUISSANCE_CALCULEE)
                {
                    TYSpectre aSpectre(LWElt.toDB());
                    pCurrentSurf->setRegime(aSpectre, -1, false);
                }

                ret &= pCurrentSurf->setSrcsLw();
            }

            // CORRECTION ANOMALIE 28 - test du type d'attribution de la puissance (calculee/imposee)
            if (pCurrentMur->getTypeDistribution() == TY_PUISSANCE_CALCULEE)
            {
                TYSpectre aSpectre(LWMur.toDB());
                pCurrentMur->setRegime(aSpectre, -1, false);
            }
        }
        else // Le mur n'est pas rayonnant, on lui affecte une puissance nulle
        {
            LWMur.setType(SPECTRE_TYPE_LW);
        }

        LWEtage = LWEtage.sum(LWMur);
    }

    // ON N'OUBLIE PAS LES DALLES (ENFIN LA DALLE SUPERIEURE
    // PARCE QU'EN GENERAL LA DALLE INEFRIEURE EST SOIT POSEE AU SOL SOIT POSEE SUR UN AUTRE ETAGE ...

    if (_closed) // Si c'est un bi¿½timent et non un simple ecran
    {
        OMatrix matDalle;
        if (_pSol->getIsRayonnant())
        {
            if (_pSol->getTypeDistribution() == TY_PUISSANCE_CALCULEE)
            {
                LPTYParoi pSolParoi = _pSol->getParoi();
                LWElt = getPuissanceRayonnee(_pSol, matDalle, pSolParoi);
                TYSpectre aTYSpectre(LWElt.toDB());
                _pSol->setRegime(aTYSpectre, -1, false);
                LWEtage = LWEtage.sum(LWElt);
            }
            else
            {
                LWEtage = LWEtage.sum(_pSol->getCurrentSpectre().toGPhy());
            }

            _pSol->setSrcsLw();
        }

        if (_pPlafond->getIsRayonnant())
        {
            if (_pPlafond->getTypeDistribution() == TY_PUISSANCE_CALCULEE)
            {
                LPTYParoi pPlafondParoi = _pPlafond->getParoi();
                LWElt = getPuissanceRayonnee(_pPlafond, matDalle, pPlafondParoi);
                TYSpectre aTYSpectre(LWElt.toDB());
                _pPlafond->setRegime(aTYSpectre, -1, false);
                LWEtage = LWEtage.sum(LWElt);
            }
            else
            {
                LWEtage = LWEtage.sum(_pPlafond->getCurrentSpectre().toGPhy());
            }

            _pPlafond->setSrcsLw();
        }
    }
    TYSpectre aTYSpectre(LWEtage.toDB());
    this->setRegime(aTYSpectre, -1, false);
    return ret;
}

double TYEtage::volume() const
{
    return _pSol->surface() * getHauteur();
}

double TYEtage::surface() const
{
    double res = 0.0;

    for (unsigned int i = 0; i < _tabMur.size(); i++)
    {
        res += TYMur::safeDownCast(_tabMur[i]->getElement())->surface();
    }

    res += _pSol->surface();
    res += _pPlafond->surface();

    return res;
}

TYTabVector TYEtage::normals() const
{
    TYTabVector tab;

    for (unsigned int i = 0; i < _tabMur.size(); i++)
    {
        OVector3D normale = TYMur::safeDownCast(_tabMur[i]->getElement())->normal();
        normale = _tabMur[i]->getMatrix() * normale;
        tab.push_back(normale);
    }

    tab.push_back(_pSol->normal());
    tab.push_back(_pPlafond->normal());

    return tab;
}

TYTabPoint TYEtage::sommets() const
{
    TYTabPoint tab;
    TYPoint pt;

    for (unsigned int i = 0; i < _tabMur.size(); i++)
    {
        for (int j = 0; j < 4; j++)
        {
            // On recupere le sommet
            pt = TYMur::safeDownCast(_tabMur[i]->getElement())->getBoundingRect()->_pts[j];

            // On passe dans le repere de l'etage
            pt = _tabMur[i]->getMatrix() * pt;

            // Ajout du sommet
            tab.push_back(pt);
        }
    }

    return tab;
}

TYTabLPPolygon TYEtage::faces() const
{
    TYTabLPPolygon tab;

    for (register unsigned int i = 0; i < _tabMur.size(); i++)
    {
        // Polygon representant la face
        TYPolygon* pPolygon = TYMur::safeDownCast(_tabMur[i]->getElement())->getBoundingRect()->toPolygon();

        // On convertit le polygon dans le repere de  l'etage
        OMatrix matrix = _tabMur[i]->getMatrix();

        pPolygon->transform(matrix);

        // Ajout du polygon
        tab.push_back(pPolygon);
    }

    tab.push_back(new TYPolygon(*_pSol->getPolygon()));
    tab.push_back(new TYPolygon(*_pPlafond->getPolygon()));

    return tab;
}

TYBox TYEtage::volEnglob() const
{
    return TYVolumeInterface::volEnglob();
}

TYPoint TYEtage::centreGravite() const
{
    TYPoint centre(0.0, 0.0, 0.0);

    // Points au sol
    TYTabPoint pts = _pSol->getContour();
    size_t nbPts = pts.size();

    // On effectue la moyenne en (X,Y) des points au sol
    for (size_t i = 0; i < nbPts; i++)
    {
        centre._x += pts[i]._x;
        centre._y += pts[i]._y;
    }

    // Moyenne
    centre._x /= nbPts;
    centre._y /= nbPts;

    // L'altitude est la mi-hauteur de l'etage
    centre._z = getHauteur() / 2.0;

    return centre;
}

int TYEtage::intersects(const OSegment3D& seg, TYTabPoint& ptList) const
{
    return TYVolumeInterface::intersects(seg, ptList);
}

int TYEtage::isInside(const TYPoint& pt) const
{
    int res = INTERS_NULLE;
    OPoint3D ptCopy(pt);

    // Le test avec le volume englobant permet de tester si le pt
    // se trouve entre les plans du sol et du plafond,
    // et d'evite aussi des calculs inutiles...
    if (_volEnglob.isInside(pt) == INTERS_NULLE) { return res; }

    TYTabPoint ptsSol = _pSol->getContour();
    size_t nbPts = ptsSol.size();

    if (_closed && (nbPts >= 3))
    {
        // On pose tout au sol !
        ptCopy._z = 0.0;

        // On se ramene a un plan (z=0), on teste si le pt est a l'interieur
        // du polygone forme par le contour de l'etage
        OPoint3D* pts = new OPoint3D[nbPts];

        for (size_t i = 0; i < nbPts; i++)
        {
            // Creation du polygone
            pts[i] = ptsSol[i];

            // ... tout au sol !
            pts[i]._z = 0.0;
        }

        OPoint3D ptMin, ptMax;

        OGeometrie::boundingBox(pts, static_cast<int>(nbPts), ptMin, ptMax);

        OBox box(ptMin, ptMax);

#if TY_USE_IHM
        res = OGeometrie::pointInPolygonRayCasting(ptCopy, pts, static_cast<int>(nbPts));
#else
        res = OGeometrie::pointInPolygonAngleSum(ptCopy, pts, nbPts);
#endif

        delete [] pts;
    }

    return res;
}

void TYEtage::setacousticFacesPourCalcul(bool bPourCalculTrajet)
{
    _bPourCalculTrajet = bPourCalculTrajet;
}

TYTabAcousticSurfaceGeoNode TYEtage::acousticFaces()
{
    TYTabAcousticSurfaceGeoNode tab;

    // Ajout des murs
    for (unsigned int i = 0; i < _tabMur.size(); i++)
    {
        // Sous-mur (MurElement) pour ce mur et sa matrice
        TYTabAcousticSurfaceGeoNode tabTmp = TYMur::safeDownCast(_tabMur[i]->getElement())->getTabAcousticSurf();
        OMatrix matrixMur = _tabMur[i]->getMatrix();

        //az++:
        unsigned int j = 0;

        // Pour chaque sous-mur
        for (j = 0; j < tabTmp.size(); j++)
        {
            // Le sous-mur
            LPTYAcousticSurface pAccSurf = TYAcousticSurface::safeDownCast(tabTmp[j]->getElement());

            // Si l'etage est un ecran on tient compte de l'epaisseur de la paroi
            if (!_closed && TYMurElement::safeDownCast(pAccSurf))
            {
                // Pour cela on construit 2 nouveaux GeoNodes associes a cette meme face
                // pour materialiser l'epaisseur
                LPTYAcousticSurfaceGeoNode pAccSurfGeoNode1 = new TYAcousticSurfaceGeoNode((LPTYElement)pAccSurf);
                LPTYAcousticSurfaceGeoNode pAccSurfGeoNode2 = new TYAcousticSurfaceGeoNode((LPTYElement)pAccSurf);

                // Recuperation de la demi epaisseur du mur parent
                double epaisseur = TYMur::safeDownCast(_tabMur[i]->getElement())->getParoi()->getEpaisseur() / 2.0;
                if (_bPourCalculTrajet)
                {
                    epaisseur = 0;    //comme ca, on a le meme nombre de point que pour un vrai contour, mais en n'ayant que le squelette de l'ecran
                }

                // Ajout d'un offset en z seulement car les faces (plan) sont definis sur x et y
                TYRepere rep = tabTmp[j]->getRepere();
                rep._origin._z = +epaisseur;
                pAccSurfGeoNode1->setRepere(rep);
                rep._origin._z = -epaisseur;
                pAccSurfGeoNode2->setRepere(rep);

                // Concatenation des matrices
                pAccSurfGeoNode1->setMatrix(matrixMur * pAccSurfGeoNode1->getMatrix());
                pAccSurfGeoNode2->setMatrix(matrixMur * pAccSurfGeoNode2->getMatrix());

                // Ajout des faces
                tab.push_back(pAccSurfGeoNode1);
                tab.push_back(pAccSurfGeoNode2);
            }
            // Sinon on ajoute simplement le sous-mur
            else
            {
                // M = M(mur) . M(sous-mur)
                OMatrix matrix = matrixMur * tabTmp[j]->getMatrix();

                // Ajout
                tab.push_back(new TYAcousticSurfaceGeoNode((LPTYElement)pAccSurf, matrix));
            }
        }
    }

    // Ajout du sol et plafond si cela est necessaire
    if (_closed)
    {
        tab.push_back(new TYAcousticSurfaceGeoNode((LPTYElement&) _pSol));
        tab.push_back(new TYAcousticSurfaceGeoNode((LPTYElement&) _pPlafond));
    }

    return tab;
}

bool TYEtage::findAcousticSurface(const TYAcousticSurface* pAccSurf, OMatrix* pMatrix /*=0*/)
{
    bool ret = false;

    for (unsigned int i = 0; (i < _tabMur.size()) && !ret; i++)
    {
        // Mur elements pour ce mur
        TYTabAcousticSurfaceGeoNode& tabTmp = TYMur::safeDownCast(_tabMur[i]._pObj->getElement())->getTabAcousticSurf();

        for (unsigned int j = 0; j < tabTmp.size(); j++)
        {
            if (TYAcousticSurface::safeDownCast(tabTmp[j]._pObj->getElement()) == pAccSurf)
            {
                if (pMatrix)
                {
                    // Mise a jour de la matrice
                    *pMatrix = *pMatrix * _tabMur[i]._pObj->getMatrix() * tabTmp[j]._pObj->getMatrix();
                }

                // Surf trouvee
                break;
            }
        }
    }

    return ret;
}

void TYEtage::coeffSabine()
{
    OSpectre absoElem;
    _absoSabine.setDefaultValue(0.0); // On reinitialise le spectre de Sabine

    size_t i = 0, j = 0;
    TYTabAcousticSurfaceGeoNode elemMur;        // Elements constituant le mur
    TYTabAcousticVolumeGeoNode  elemMachine ;   // Elements constituants la machine
    double surfElem = 0.0;                      // surface elementaire
    _surfAbsorbante = 0.0;                      // Initialisation surface

    _volumeLibre = this->volume()   ;           // recupere le volume de l'etage

    // Lecture de l'absorption de chacune des faces du volume
    size_t nbFaces = this->getTabMur().size();

    for (i = 0 ; i < nbFaces ; i++)
    {
        elemMur = TYMur::safeDownCast(_tabMur[i]->getElement())->getTabAcousticSurf();

        LPTYParoi pMurParoi = TYMur::safeDownCast(_tabMur[i]->getElement())->getParoi();

        for (j = 0; j < elemMur.size() ; j++)
        {
            // Une sous-face de mur doit etre de type MurElement
            LPTYMurElement pMurElt = TYMurElement::safeDownCast(elemMur[j]->getElement());
            assert(pMurElt);

            // Recuperation des caracteristiques locales du mur
            surfElem = pMurElt->surface();

            // Absorption de la paroi associee a la surface
            if (pMurElt->getIsSub())
            {
                // Si la surface est une fenetre on prend son absorption
                absoElem = pMurElt->getParoi()->getMatFace1()->getSpectreAbso().mult(surfElem);
            }
            else
            {
                // Sinon on prend le materiau composant le mur
                absoElem = pMurParoi->getMatFace1()->getSpectreAbso().mult(surfElem);
            }

            // Cumul des alpha*S elementaire et des surfaces elementaire
            _absoSabine = _absoSabine.sum(absoElem);
            _surfAbsorbante = _surfAbsorbante + surfElem ;
        }
    }

    // Prise en compte des dalles inferieures (sol) et superieures (plafond)

    // Aire occupee au sol par les machines
    double aireTotMachinesSol = 0.0;
    for (i = 0 ; i < _facesMachineSol.size() ; i++)
    {
        aireTotMachinesSol = aireTotMachinesSol + TYAcousticSurface::safeDownCast(_facesMachineSol[i]->getElement())->surface();
    }

    // Surface sol libre = surface totale du sol - surface occupee par les machines
    surfElem = _pSol->surface() - aireTotMachinesSol;
    _surfAbsorbante = _surfAbsorbante + surfElem;

    absoElem = _pSol->getParoi()->getMatFace1()->getSpectreAbso().mult(surfElem);
    _absoSabine = _absoSabine.sum(absoElem);

    surfElem = _pPlafond->surface();
    _surfAbsorbante = _surfAbsorbante + surfElem;

    absoElem = _pPlafond->getParoi()->getMatFace1()->getSpectreAbso().mult(surfElem);
    _absoSabine = _absoSabine.sum(absoElem);


    // Prise en compte de l'absorption de chacune des machines
    size_t nbMachines = this->getTabMachine().size();
    TYTabAcousticSurfaceGeoNode tabPtrSurf;

    for (i = 0 ; i < nbMachines ; i++)
    {
        elemMachine = TYMachine::safeDownCast(this->getTabMachine()[i]->getElement())->getTabAcousticVol();

        for (j = 0 ; j < elemMachine.size() ; j++)
        {
            LPTYAcousticVolume pElemMachine = TYAcousticVolume::safeDownCast(elemMachine[j]->getElement());

            surfElem = pElemMachine->surface();

            // Considere que les volumes de la machines sont constituees d'un materiau homogene
            // Si on souhaite passer a un materiau par face, il faudrait mettre les deux lignes
            // qui suivent dans une boucle traitant chacune des faces.
            absoElem = pElemMachine->getMateriau()->getSpectreAbso().mult(surfElem);

            _absoSabine = _absoSabine.sum(absoElem);
            double volElem = pElemMachine->volume();
            _surfAbsorbante = _surfAbsorbante + surfElem ;
            _volumeLibre = _volumeLibre - volElem;

        }
    }

    // Les faces des machines posees au sol ne jouent pas dans l'absorption des machines
    for (i = 0 ; i < _facesMachineSol.size() ; i++)
    {
        LPTYAcousticSurface pSurface = TYAcousticSurface::safeDownCast(_facesMachineSol[i]->getElement());

        surfElem  = pSurface->surface();
        absoElem = pSurface->getMateriau()->getSpectreAbso().mult(surfElem);

        _surfAbsorbante = _surfAbsorbante - surfElem ;
        _absoSabine = _absoSabine.subst(absoElem);
    }

    _absoSabine = _absoSabine.div(_surfAbsorbante); // On calcul le alpha moyen de la salle
    _absoSabine = _absoSabine.sum(1.0E-6); // Au cas ou le alpha serait nul, on lui rajoute un epsilon
}

void TYEtage::calculTempsReverb()
{
    _TR = TYSpectre::getEmptyLinSpectre(); //Represente une grandeur lineaire ... Le temps

    // Prise en compte de l'absorbsion atmospherique

    // S * A
    _TR = _absoSabine.mult(_surfAbsorbante);

    // 0.16*V/(S * A)
    _TR = _TR.invMult(0.16 * _volumeLibre);

    _TR.setType(SPECTRE_TYPE_AUTRE); // unite = secondes
    _TR.setEtat(SPECTRE_ETAT_DB); // Pour eviter une transformation non souhaitee
}

double TYEtage::coeffTransmFrom(const TYSourcePonctuelle* pSrcPonct) const
{
    assert(pSrcPonct);
    TYAcousticSurface* pAccSurf = (TYAcousticSurface*) pSrcPonct->getParent();
    assert(pAccSurf);
    LPTYMateriauConstruction pMat = pAccSurf->getMateriau();
    TYSpectre spectre = pMat->getSpectreTransm();

    return 0;
}

void TYEtage::updateSolPlafond()
{
    int nbPts = static_cast<int>(_tabMur.size());

    TYTabPoint pointsSol;
    TYTabPoint pointsPlafond;

    //for (int i = 0; i < nbPts; i++)
    for (int i = nbPts - 1; i >= 0; --i)
    {
        // Creation du polygone pour le sol
        TYPoint ptSol = TYMur::safeDownCast(_tabMur[i]->getElement())->getBoundingRect()->_pts[3];
        // Passage dans le repere de l'etage
        ptSol = _tabMur[i]->getMatrix() * ptSol;
        // Ajout du point
        pointsSol.push_back(ptSol);

        // Creation du polygone pour le plafond
        TYPoint ptPlafond = TYMur::safeDownCast(_tabMur[i]->getElement())->getBoundingRect()->_pts[0];
        // Passage dans le repere de l'etage
        ptPlafond = _tabMur[i]->getMatrix() * ptPlafond;
        // Ajout du point
        pointsPlafond.push_back(ptPlafond);
    }

    // Inversion des points du sol pour que toutes les normales soient vers l'exti¿½rieur
    TYTabPoint tabPtsTemp = pointsSol;
    pointsSol.clear();
    vector<TYPoint>::reverse_iterator it;
    for (it = tabPtsTemp.rbegin(); it != tabPtsTemp.rend(); it++)
    {
        pointsSol.push_back((*it));
    }

    _pSol->getPolygon()->setPoints(pointsSol);
    _pPlafond->getPolygon()->setPoints(pointsPlafond);

    setIsGeometryModified(true);
}

OSpectre TYEtage::champDirect(const OPoint3D& unPoint)
{
    OSpectre s = OSpectre::getEmptyLinSpectre();
    OSpectre sTemp = OSpectre::getEmptyLinSpectre();
    TYAtmosphere atmos;

    unsigned int i, j;
    double distance;
    OSegment3D SR ; // Segment source recepteur
    TYTabAcousticVolumeGeoNode tabVolumes;
    TYTabSourcePonctuelleGeoNode tabSources;
    OPoint3D posSource;

    // Pour toutes les machines
    for (i = 0 ; i < _tabMachine.size() ; i++)
    {
        LPTYAcousticVolumeNode pVolNode = TYAcousticVolumeNode::safeDownCast(_tabMachine[i]->getElement());

        if (pVolNode && pVolNode->getIsRayonnant()) // Si pas rayonnant y en a pas prendre source
        {
            TYTabSourcePonctuelleGeoNode tabSrcNode = pVolNode->getSrcs();

            for (j = 0; j < tabSrcNode.size(); j++)
            {
                // La source
                LPTYSourcePonctuelle pSrc = TYSourcePonctuelle::safeDownCast(tabSrcNode[j]->getElement());

                // Matrice de changement de repere vers le repere de la source
                OMatrix matInv = tabSrcNode[j]->getMatrix().getInvert();

                // Position du point dans le repere de la source
                OPoint3D posPtInSrcRep = matInv * unPoint;

                // Contribution de la source au point
                SR = OSegment3D(posSource, posPtInSrcRep);
                distance = SR.longueur();

                // Affectation de la directivite a la puissance de la source
                sTemp = pSrc->lwApparenteSrcDest(SR, atmos); // Q
                sTemp = sTemp.mult(pSrc->getSpectre()->toGPhy()); // W
                sTemp = sTemp.mult(atmos.getImpedanceSpecifique() / (4 * M_PI * distance * distance)); // Q.W.rho.C / 4.pi.di¿½

                //Cumul des spectres
                s = s.sum(sTemp);
            }
        }
    }

    // Pour toutes les sources ponctuelles
    for (i = 0 ; i < _tabSources.size() ; i++)
    {
        LPTYUserSourcePonctuelle pSrc = TYUserSourcePonctuelle::safeDownCast(_tabSources[i]->getElement());

        if (pSrc && pSrc->getIsRayonnant())
        {
            posSource = *pSrc->getPos();
            SR = OSegment3D(posSource, unPoint);
            distance = SR.longueur();

            // Affectation de la directivite a la puissance de la source
            sTemp = pSrc->lwApparenteSrcDest(SR, atmos); // Directivite de la source Q
            sTemp = sTemp.mult(pSrc->getSpectre()->toGPhy());  // Puissance W
            sTemp = sTemp.mult(atmos.getImpedanceSpecifique() / (4 * M_PI * distance * distance)); // Q.W.rho.C/(4.pi.di¿½)

            //Cumul des spectres
            s = s.sum(sTemp);
        }
    }

    s.setType(SPECTRE_TYPE_LP);

    return s;
}
void TYEtage::calculChampReverbere()
{
    // On commence par calculer le coefficient d'abosrption moyenne de la salle
    coeffSabine();

    // On calcule le TR
    calculTempsReverb();

    // Puis le champ reverbere
    switch (_typeReverb)
    {
        case 0:
            calculChampRevSabine();
            break;
        case 1:
            calculChampRevSabine(); //champRevKuttruff; // On met tout en sabine pour l'instant
            break;
        default:
            calculChampRevSabine();
            break;
    }
}

void TYEtage::calculChampRevSabine()
{
    unsigned int i, j;
    TYAtmosphere atmos;

    // Effet de salle ((4.Rho.C / (alpha.S))-(4/S) - Tr.C.AbsoATm)
    _reverb = TYSpectre::getEmptyLinSpectre(4.0); // Spectre initialise a la valeur 4

    _reverb = _reverb.div(_absoSabine.mult(_surfAbsorbante)); // 4/(alpha.S)
    _reverb = _reverb.mult(atmos.getImpedanceSpecifique()); // 4.Rho.C/(alpha.S)

    // Champ Reverbere : W*(Effet de salle)
    OSpectre sTemp = OSpectre::getEmptyLinSpectre();

    // Pr = 4W*rho*c/a
    TYTabSourcePonctuelleGeoNode tabAllSources ;

    // On recupere toutes les sources
    // Des machines ...
    for (i = 0; i < _tabMachine.size(); i++)
    {
        LPTYAcousticVolumeNode pVolNode = TYAcousticVolumeNode::safeDownCast(_tabMachine[i]->getElement());
        if (pVolNode && pVolNode->getIsRayonnant())
        {
            // Les sources de cette machine
            TYTabSourcePonctuelleGeoNode tabSrcMachine = TYMachine::safeDownCast(_tabMachine[i]->getElement())->getSrcs();
            tabAllSources.insert(tabAllSources.end(), tabSrcMachine.begin(), tabSrcMachine.end());
        }
    }

    for (i = 0 ; i < _tabSources.size(); i++)
    {
        LPTYUserSourcePonctuelle pSrc = TYUserSourcePonctuelle::safeDownCast(_tabSources[i]->getElement());

        // Si elle est rayonnante, on ajoute la source ponctuelle
        if (pSrc && pSrc->getIsRayonnant())
        {
            tabAllSources.push_back(_tabSources[i]);
        }
    }

    // Pour chacune de ces sources
    for (j = 0; j < tabAllSources.size(); j++)
    {
        // Cumul des spectres de toutes les sources
        sTemp = sTemp.sum(TYSourcePonctuelle::safeDownCast(tabAllSources[j]->getElement())->getSpectre()->toGPhy());
    }

    // Cumul des spectres
    _reverb = _reverb.mult(sTemp); // 4.W.Rho.C / (alpha.S)

    _reverb.setType(SPECTRE_TYPE_LP);
}

void TYEtage::calculChampRevKuttruff()
{
    _reverb = TYSpectre::getEmptyLinSpectre();
    OSpectre sTemp;
    TYAtmosphere atmos;

    OPoint3D unPoint; // DT 20060520 juste pour que ca compile (le point devrait etre passe en parametre

    double h2 = getHauteur() * getHauteur();
    double invh2 = 1.0 / h2;

    // (1-alpha)/alpha
    OSpectre diffAlpha = (_absoSabine.subst(1.0)).mult(-1.0);   // 1-alpha
    OSpectre deltaAlpha = diffAlpha.div(_absoSabine);           // (1-alpha) / alpha
    // diffAlphaGeom = rho*c*(1-alpha) / (pi*hi¿½)
    OSpectre diffAlphaGeom = diffAlpha.mult(atmos.getImpedanceSpecifique() / (M_PI * h2)); //rho*c = 400;


    // b = Log((0.6 + alpha)/alpha)
    OSpectre b = _absoSabine.sum(0.6);          // alpha + 0.6
    b = b.log();                                // log(alpha + 0.6)
    b = b.div(_absoSabine);                     // log(alpha + 0.6) / alpha
    OSpectre b2 = b.mult(b);                // b2 = b*b

    // Preparation du calcul
    OSpectre termeB = OSpectre::getEmptyLinSpectre();
    OSpectre terme1 = OSpectre::getEmptyLinSpectre();

    double distance;

    TYTabAcousticVolumeGeoNode tabVolumes;
    TYTabLPSourcePonctuelle tabSources;
    TYPoint posSource;
    TYSegment SR ; // Segment source recepteur

    unsigned int i ;

    // Pour toutes les machines
    for (i = 0; i < _tabMachine.size(); i++)
    {
        // Changement de repere pour cette machine
        OMatrix matMachine = _tabMachine[i]->getMatrix();

        LPTYAcousticVolumeNode pVolNode = TYAcousticVolumeNode::safeDownCast(_tabMachine[i]->getElement());
        if (pVolNode && pVolNode->getIsRayonnant())
        {
            // Les sources de cette machine
            TYTabSourcePonctuelleGeoNode tabSources = pVolNode->getSrcs();

            // Pour toutes les sources
            for (unsigned int j = 0; j < tabSources.size(); j++)
            {
                LPTYSourcePonctuelle pSource = TYSourcePonctuelle::safeDownCast(tabSources[j]->getElement());

                // Changement de repere total
                OMatrix mat = matMachine * tabSources[j]->getMatrix();

                // Pos de la source dans le repere de l'etage
                posSource = mat * *(pSource->getPos());

                // Contribution de la source au point
                SR = TYSegment(posSource, unPoint);
                distance = SR.longueur();

                // termeB = b / ((bi¿½ + ri¿½/hi¿½)^(3/2)
                termeB = b2.sum(distance * distance * invh2);   // bi¿½ + ri¿½/hi¿½
                termeB = termeB.power(-3.0 / 2.0);              // (bi¿½ + ri¿½/hi¿½)^(-3/2)
                termeB = termeB.mult(b);

                // terme1 = 1 / ((1+r2/h2)^(3/2)
                double powVal = pow((1 + distance * distance * invh2), (-3.0 / 2.0));
                terme1.setDefaultValue(powVal);

                sTemp = deltaAlpha.mult(termeB);
                sTemp = sTemp.sum(terme1);
                sTemp = sTemp.mult(diffAlphaGeom);

                // Energie de la source * facteur de directivite
                sTemp = sTemp.mult(pSource->getSpectre()->toGPhy());

                //Cumul des spectres
                _reverb = _reverb.sum(sTemp);
            }
        }
    }

    // Pour toutes les Sources
    for (i = 0; i < _tabSources.size(); i++)
    {
        LPTYUserSourcePonctuelle pSource = TYUserSourcePonctuelle::safeDownCast(_tabSources[i]->getElement());

        if (pSource && pSource->getIsRayonnant())
        {
            // Changement de repere total
            OMatrix mat =  _tabSources[i]->getMatrix();

            // Pos de la source dans le repere de l'etage
            posSource = mat * *(pSource->getPos());

            // Contribution de la source au point
            SR = TYSegment(posSource, unPoint);
            distance = SR.longueur();

            // termeB = b / ((bi¿½ + ri¿½/hi¿½)^(3/2)
            termeB = b2.sum(distance * distance * invh2);   // bi¿½ + ri¿½/hi¿½
            termeB = termeB.power(-3.0 / 2.0);              // (bi¿½ + ri¿½/hi¿½)^(-3/2)
            termeB = termeB.mult(b);

            // terme1 = 1 / ((1+r2/h2)^(3/2)
            double powVal = pow((1 + distance * distance * invh2), (-3.0 / 2.0));
            terme1.setDefaultValue(powVal);

            sTemp = deltaAlpha.mult(termeB);
            sTemp = sTemp.sum(terme1);
            sTemp = sTemp.mult(diffAlphaGeom);

            // Energie de la source * facteur de directivite
            sTemp = sTemp.mult(pSource->getSpectre()->toGPhy());

            //Cumul des spectres
            _reverb = _reverb.sum(sTemp);
        }
    }

    _reverb.setType(SPECTRE_TYPE_LP);
}


void TYEtage::updateParoi()
{
    TYTabMurGeoNode tabMur = getTabMur();

    for (unsigned int i = 0 ; i < tabMur.size() ; i++)
    {
        LPTYMur pMur = TYMur::safeDownCast(tabMur[i]->getElement());
        TYMur::safeDownCast(_tabMur[i]->getElement())->setParoi(_pParoi);
    }

    _pSol->setParoi(_pParoi);
    _pPlafond->setParoi(_pParoi);

}

void TYEtage::findFaceMachineSol()
{
    register unsigned int i = 0;

    _facesMachineSol.clear();

    // On recupere toutes les faces des machines
    TYTabAcousticSurfaceGeoNode accFaces;
    for (i = 0; i < _tabMachine.size(); i++)
    {
        TYTabAcousticSurfaceGeoNode tabTmp = TYMachine::safeDownCast(_tabMachine[i]->getElement())->acousticFaces();
        OMatrix matrixMachine = _tabMachine[i]->getMatrix();

        for (register unsigned int  j = 0; j < tabTmp.size(); j++)
        {
            // Passage dans le repere de cet etage
            OMatrix matrix = matrixMachine * tabTmp[j]->getMatrix();
            tabTmp[j]->setMatrix(matrix);

            accFaces.push_back(tabTmp[j]);
        }
    }

    // On va rechercher les faces posees au sol de cet etage
    OPlan planSol = _pSol->plan();
    double distance = 0.0;

    double seuilConfondus = TYSEUILCONFONDUS;

#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "SeuilConfondus"))
    {
        seuilConfondus = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "SeuilConfondus");
    }
    else
    {
        TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "SeuilConfondus", seuilConfondus);
    }
#endif

    for (i = 0; i < accFaces.size(); i++)
    {
        // La face
        LPTYAcousticSurfaceGeoNode pFaceGeoNode = accFaces[i];

        // Contour de la face
        LPTYAcousticSurface pSurface = TYAcousticSurface::safeDownCast(accFaces[i]->getElement());
        TYTabPoint contour = pSurface->getContour();
        if (contour.size() < 3) { continue; }

        OMatrix matrixFace = accFaces[i]->getMatrix();

        // Un point sur le plan de la face dans le repere etage
        OPoint3D ptPlan = matrixFace * OVector3D(contour[0]);
        // La normale a la face dans le repere etage
        OVector3D vecPlan = OVector3D(matrixFace * OPoint3D(0.0, 0.0, 0.0), matrixFace * pSurface->normal());
        vecPlan.normalize();
        // Le plan de la face dans le repere etage
        OPlan planFace(ptPlan, vecPlan);

        // Distance entre les 2 sols s'ils sont paralleles
        if (planFace.distancePlanParallel(planSol, distance))
        {
            // Si la face est posee au sol (avec une tolerance)
            if (ABS(distance) <= seuilConfondus)
            {
                // On "desactive" cette face de la machine
                pSurface->setDensiteSrcs(0.0);

                // On conserve les faces en contact avec le sol
                _facesMachineSol.push_back(accFaces[i]);
            }
        }
    }

}

bool TYEtage::updateAcoustic(const bool& force) //force=false
{
    register unsigned int i;
    bool ret = true;

    // Si pas rayonnant, on ne bloque pas le calcul
    // (cet etat peut etre voulu, mais on ne fait pas de repartition acoustique)
    if (!_isRayonnant) { return true; }

    // On commence par ajuster la hauteur des sources
    updateZSource();

    // updateAcoustic des machines dans le bi¿½timent s'il y en a
    if ((_tabMachine.size() != 0) || (_tabSources.size() != 0))
    {
        findFaceMachineSol();

        // Machines
        for (i = 0; i < _tabMachine.size(); i++)
        {
            ret &= TYMachine::safeDownCast(_tabMachine[i]->getElement())->updateAcoustic();
        }
    }

    calculChampReverbere();
    distriSrcs();
    ret &= setSrcsLw();

    return ret;
}

void TYEtage::remAllSrcs()
{
    // Suppression des sources sur les murs
    for (unsigned int i = 0 ; i < _tabMur.size() ; i++)
    {
        TYAcousticRectangleNode::safeDownCast(_tabMur[i]->getElement())->remAllSrcs();
    }

    // Suppression des sources sur les dalles
    _pSol->remAllSrcs();
    _pPlafond->remAllSrcs();
}

double TYEtage::activeSurface() const
{
    register unsigned int i;
    double surface = 0.0;

    for (i = 0 ; i < _tabMur.size() ; i++)
    {
        LPTYMur pMur = TYMur::safeDownCast(_tabMur[i]->getElement());

        surface += pMur->activeSurface();
    }

    return surface;
}


TYSpectre TYEtage::setGlobalLW(const TYSpectre& spectre, const double& surfGlobale, const int& regime/*=-1*/)
{
    /* Si l'etage est a "puissance imposee", on la retourne, si l'etage est a "puissance calculee, sa puissance
       depend des sources qu'il contient et a ete calculee prealablement par un updateAcoustic
       (c'est le contraire des machines)*/

    return getCurrentSpectre().toGPhy(); // On retourne le spectre obtenu
}

TYSpectre TYEtage::getPuissanceRayonnee(LPTYAcousticSurface pCurrentSurf, const OMatrix matMur, const LPTYParoi pMurParoi)
{
    TYSpectre spectreEltMur = TYSpectre::getEmptyLinSpectre();
    TYSpectre s = TYSpectre::getEmptyLinSpectre();
    OSpectre spectreAtt;
    OPoint3D posSrc;
    TYAtmosphere atmos;

    // Une sous-face de mur doit etre de type MurElement
    LPTYMurElement pMurElt = TYMurElement::safeDownCast(pCurrentSurf);

    if (pMurElt) // IL faut prendre en compte le cas des dalles
    {
        if (pMurElt->getIsSub())
        {
            // Si la paroi est une fenetre
            spectreAtt = pMurElt->getParoi()->getMatStruct()->getSpectreTransm();
        }
        else
        {
            // Sinon c'est la paroi associee au mur
            spectreAtt = pMurParoi->getMatStruct()->getSpectreTransm();
        }
    }
    else // Cas des dalles
    {
        // Sinon c'est la paroi associee au mur
        spectreAtt = pMurParoi->getMatStruct()->getSpectreTransm();
    }

    TYTabSourcePonctuelleGeoNode tabSources = pCurrentSurf->getSrcs();
    double surf = pCurrentSurf->surface() / tabSources.size();

    // Pour toutes les sources ponctuelles de la face
    for (unsigned int k = 0 ; k < tabSources.size() ; k++)
    {
        LPTYSourcePonctuelle pSource = TYSourcePonctuelle::safeDownCast(tabSources[k]->getElement());

        // Position de la source dans le repere de l'etage
        posSrc = matMur * *(pSource->getPos());

        s = _reverb.div(4.0); // Correction Champ reverbere (pression), intensite reverberee.

        s = s.sum(champDirect(posSrc));

        s = s.mult(surf / atmos.getImpedanceSpecifique()); // W = P² * S / (rho.C)
        //      s = s.sum(_reverb);// Pi¿½ = champDirect + champReverbere

        //      s = s.div(4.0); // DT 20100106 car le cas test paroi transparente de collais pas

        s = s.div((spectreAtt).toGPhy()); // Voir si le spectre doit etre converti en grandeur physique

        spectreEltMur = spectreEltMur.sum(s);
    }

    spectreEltMur.setType(SPECTRE_TYPE_LW);
    return spectreEltMur;
}

void TYEtage::setParoi(const LPTYParoi pParoi)
{
    assert(pParoi);
    assert(_pParoi);

    *_pParoi = *pParoi;
}

void TYEtage::updateZSource()
{
    unsigned int i;
    for (i = 0 ; i < _tabSources.size(); i++)
    {
        LPTYUserSourcePonctuelle pSource = TYUserSourcePonctuelle::safeDownCast(_tabSources[i]->getElement());
        double h = pSource->getHauteur();
        pSource->getPos()->_z = h;
        _tabSources[i]->getRepere()._origin._z = h;
        _tabSources[i]->setIsGeometryModified(true);
    }

    for (i = 0; i < _tabMachine.size(); i++)
    {
        double h = _tabMachine[i]->getHauteur();
        ORepere3D repere = _tabMachine[i]->getRepere();
        repere._origin._z = h;
        _tabMachine[i]->setRepere(repere);
    }

    setIsGeometryModified(true);
}

void TYEtage::propagateAtt(LPTYAttenuateur pAtt)
{
    size_t i;
    size_t nbFaces = this->getTabMur().size();

    LPTYMur pMur = NULL;

    // On propage aux murs
    for (i = 0 ; i < nbFaces ; i++)
    {
        pMur = TYMur::safeDownCast(_tabMur[i]->getElement());

        if (pMur) { pMur->propagateAtt(pAtt); }
    }

    // On propage aux dalles
    _pSol->propagateAtt(pAtt);
    _pPlafond->propagateAtt(pAtt);


    TYAcousticVolume::propagateAtt(pAtt);
}
