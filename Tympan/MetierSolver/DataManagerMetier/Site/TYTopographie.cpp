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
#include "Tympan/GraphicIHM/DataManagerIHM/TYTopographieWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYTopographieGraphic.h"
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/MetierSolver/CommonTools/OSegment3D.h"

#include "Tympan/Tools/TYProgressManager.h"

#include "Tympan/Tools/OMessageManager.h"


TY_EXTENSION_INST(TYTopographie);
TY_EXT_GRAPHIC_INST(TYTopographie);

#define TR(id) OLocalizator::getString("OMessageManager", (id))

// Declaration de la fonction utilisee par qsort pour le tri des terrains
static int compareSurfaceTerrains(const void* elem1, const void* elem2);

TYTopographie::TYTopographie()
{
    _seuilConfondus = 0.002;
    _name = TYNameManager::get()->generateName(getClassName());

    _pAltimetrie = new TYAltimetrie();
    _pAltimetrie->setParent(this);

    LPTYTerrain pDefTerrain = new TYTerrain();
    pDefTerrain->setParent(this);
    pDefTerrain->setName("Terrain par defaut");
    addTerrain(pDefTerrain);
    _DefTerrainIdx = 0;

    // Taille de topo par defaut (rectangle)
    float sizeX = TAILLETOPOX;
    float sizeY = TAILLETOPOY;

#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "SeuilConfondus"))
    {
        _seuilConfondus = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "SeuilConfondus");
    }
    else
    {
        TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "SeuilConfondus", _seuilConfondus);
    }

    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "DefaultDimX"))
    {
        sizeX = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, "DefaultDimX");
    }
    else
    {
        TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, "DefaultDimX", sizeX);
    }

    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "DefaultDimY"))
    {
        sizeY = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, "DefaultDimY");
    }
    else
    {
        TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, "DefaultDimY", sizeY);
    }

#endif

    TYRectangle emprise;
    emprise.setDimension(sizeX, sizeY);

    setEmprise(emprise.getContour());

    _empriseColor = OColor::BLUE;

    _pSortedTerrains = NULL;
}

TYTopographie::TYTopographie(const TYTopographie& other)
{
    *this = other;
}

TYTopographie::~TYTopographie()
{
    if (_pSortedTerrains)
    {
        delete [] _pSortedTerrains;
        _pSortedTerrains = NULL;
    }
}

TYTopographie& TYTopographie::operator=(const TYTopographie& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        _listPlanEau = other._listPlanEau;
        _listCrsEau = other._listCrsEau;
        _listTerrain = other._listTerrain;
        _listCrbNiv = other._listCrbNiv;
        _DefTerrainIdx = other._DefTerrainIdx;
        _pAltimetrie = other._pAltimetrie;
        _emprise = other._emprise;
        _pSortedTerrains = other._pSortedTerrains;
    }
    return *this;
}

bool TYTopographie::operator==(const TYTopographie& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        if (!(_listPlanEau == other._listPlanEau)) { return false; }
        if (!(_listCrsEau == other._listCrsEau)) { return false; }
        if (!(_listTerrain == other._listTerrain)) { return false; }
        if (!(_listCrbNiv == other._listCrbNiv)) { return false; }
        if (_DefTerrainIdx != other._DefTerrainIdx) { return false; }
        if (_pAltimetrie != other._pAltimetrie) { return false; }
        if (_emprise != other._emprise) { return false; }
        if (_pSortedTerrains != other._pSortedTerrains) { return false; }
    }
    return true;
}

bool TYTopographie::operator!=(const TYTopographie& other) const
{
    return !operator==(other);
}

bool TYTopographie::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    TYTopographie* pOtherTopo = (TYTopographie*) pOther;

    _DefTerrainIdx = pOtherTopo->_DefTerrainIdx;
    _pAltimetrie->deepCopy(pOtherTopo->_pAltimetrie, copyId);

    _listCrbNiv.clear();
    unsigned int i;
    for (i = 0; i < pOtherTopo->_listCrbNiv.size(); i++)
    {
        LPTYCourbeNiveauGeoNode pCrbNivGeoNode = new TYCourbeNiveauGeoNode(NULL, this);
        pCrbNivGeoNode->deepCopy(pOtherTopo->_listCrbNiv[i], copyId);
        _listCrbNiv.push_back(pCrbNivGeoNode);
    }

    _listTerrain.clear();
    for (i = 0; i < pOtherTopo->_listTerrain.size(); i++)
    {
        LPTYTerrainGeoNode pTerrainGeoNode = new TYTerrainGeoNode(NULL, this);
        pTerrainGeoNode->deepCopy(pOtherTopo->_listTerrain[i], copyId);
        _listTerrain.push_back(pTerrainGeoNode);
    }

    _listCrsEau.clear();
    for (i = 0; i < pOtherTopo->_listCrsEau.size(); i++)
    {
        LPTYCoursEauGeoNode pCrsEauGeoNode = new TYCoursEauGeoNode(NULL, this);
        pCrsEauGeoNode->deepCopy(pOtherTopo->_listCrsEau[i], copyId);
        _listCrsEau.push_back(pCrsEauGeoNode);
    }

    _listPlanEau.clear();
    for (i = 0; i < pOtherTopo->_listPlanEau.size(); i++)
    {
        LPTYPlanEauGeoNode pPlanEauGeoNode = new TYPlanEauGeoNode(NULL, this);
        pPlanEauGeoNode->deepCopy(pOtherTopo->_listPlanEau[i], copyId);
        _listPlanEau.push_back(pPlanEauGeoNode);
    }

    resetEmprise();
    for (i = 0; i < pOtherTopo->_emprise.size(); i++)
    {
        addPointEmprise(pOtherTopo->_emprise[i]);
    }

    return true;
}

std::string TYTopographie::toString() const
{
    return "TYTopographie";
}

DOM_Element TYTopographie::toXML(DOM_Element& domElement)
{
    unsigned int i = 0;

    DOM_Element domNewElem = TYElement::toXML(domElement);
    DOM_Document domDoc = domElement.ownerDocument();
    DOM_Element empriseNode = domDoc.createElement("Emprise");
    domNewElem.appendChild(empriseNode);
    DOM_Element listCrbNivNode = domDoc.createElement("ListCrbNiv");
    domNewElem.appendChild(listCrbNivNode);
    DOM_Element listTerrainNode = domDoc.createElement("ListTerrain");
    domNewElem.appendChild(listTerrainNode);
    DOM_Element listCrsEauNode = domDoc.createElement("ListCrsEau");
    domNewElem.appendChild(listCrsEauNode);
    DOM_Element listPlanEauNode = domDoc.createElement("ListPlanEau");
    domNewElem.appendChild(listPlanEauNode);

    _pAltimetrie->toXML(domNewElem);

    TYXMLTools::addElementIntValue(domNewElem, "DefaultTerrain", _DefTerrainIdx);

    for (i = 0; i < _emprise.size(); i++)
    {
        _emprise[i].toXML(empriseNode);
    }

    for (i = 0; i < _listCrbNiv.size(); i++)
    {
        // Ajout de la courbe de niveau
        _listCrbNiv[i]->toXML(listCrbNivNode);
    }

    for (i = 0; i < _listTerrain.size(); i++)
    {
        // Ajout du terrain
        _listTerrain[i]->toXML(listTerrainNode);
    }

    for (i = 0; i < _listCrsEau.size(); i++)
    {
        // Ajout du cours d'eau
        _listCrsEau[i]->toXML(listCrsEauNode);
    }

    for (i = 0; i < _listPlanEau.size(); i++)
    {
        // Ajout du plan d'eau
        _listPlanEau[i]->toXML(listPlanEauNode);
    }

    return domNewElem;
}

int TYTopographie::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);

    purge();

    unsigned int i = 0, j = 0;
    TYPoint pt;
    LPTYCourbeNiveauGeoNode pCrbNivGeoNode = new TYCourbeNiveauGeoNode(NULL, this);
    LPTYTerrainGeoNode pTerrainGeoNode = new TYTerrainGeoNode(NULL, this);
    LPTYCoursEauGeoNode pCrsEauGeoNode = new TYCoursEauGeoNode(NULL, this);
    LPTYPlanEauGeoNode pPlanEauGeoNode = new TYPlanEauGeoNode(NULL, this);


    bool bDefTerrainOk = false;
    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();
    unsigned int childcount = childs.length();
    for (i = 0; i < childcount; i++)
    {
        elemCur = childs.item(i).toElement();
        OMessageManager::get()->info("Charge element de topographie %d/%d.", i + 1, childcount);

        _pAltimetrie->callFromXMLIfEqual(elemCur);

        // Compatibilite T3.2
        LPTYTerrain pDefTerrainT32 = new TYTerrain();
        pDefTerrainT32->setName("Terrain par defaut");
        if (pDefTerrainT32->callFromXMLIfEqual(elemCur))
        {
            addTerrain(pDefTerrainT32);
        }

        TYXMLTools::getElementIntValue(elemCur, "DefaultTerrain", _DefTerrainIdx, bDefTerrainOk);

        // RELECTURE EMPRISE
        if (elemCur.nodeName() == "Emprise")
        {
            DOM_Element elemCur2;
            QDomNodeList childs2 = elemCur.childNodes();

            for (j = 0; j < childs2.length(); j++)
            {
                elemCur2 = childs2.item(j).toElement();

                if (pt.callFromXMLIfEqual(elemCur2))
                {
                    _emprise.push_back(pt);
                }
            }
        }

        // RELECTURE COURBES DE NIVEAU
        else if (elemCur.nodeName() == "ListCrbNiv")
        {
            DOM_Element elemCur2;
            QDomNodeList childs2 = elemCur.childNodes();

            for (j = 0; j < childs2.length(); j++)
            {
                elemCur2 = childs2.item(j).toElement();
                if (pCrbNivGeoNode->callFromXMLIfEqual(elemCur2))
                {
                    addCrbNiv(pCrbNivGeoNode);
                    pCrbNivGeoNode = new TYCourbeNiveauGeoNode(NULL, this);
                }
            }
        }

        // RELECTURE TERRAINS
        else if (elemCur.nodeName() == "ListTerrain")
        {
            DOM_Element elemCur2;
            QDomNodeList childs2 = elemCur.childNodes();

            for (j = 0; j < childs2.length(); j++)
            {
                elemCur2 = childs2.item(j).toElement();

                if (pTerrainGeoNode->callFromXMLIfEqual(elemCur2))
                {
                    addTerrain((LPTYTerrainGeoNode) pTerrainGeoNode);
                    pTerrainGeoNode = new TYTerrainGeoNode(NULL, this);
                }
            }
        }

        // RELECTURE COURS D'EAU
        else if (elemCur.nodeName() == "ListCrsEau")
        {
            DOM_Element elemCur2;
            QDomNodeList childs2 = elemCur.childNodes();

            for (j = 0; j < childs2.length(); j++)
            {
                elemCur2 = childs2.item(j).toElement();
                if (pCrsEauGeoNode->callFromXMLIfEqual(elemCur2))
                {
                    addCrsEau(pCrsEauGeoNode);
                    pCrsEauGeoNode = new TYCoursEauGeoNode(NULL, this);
                }
            }
        }

        // RELECTURE PLAN D'EAU
        else if (elemCur.nodeName() == "ListPlanEau")
        {
            DOM_Element elemCur2;
            QDomNodeList childs2 = elemCur.childNodes();

            for (j = 0; j < childs2.length(); j++)
            {
                elemCur2 = childs2.item(j).toElement();
                if (pPlanEauGeoNode->callFromXMLIfEqual(elemCur2))
                {
                    addPlanEau(pPlanEauGeoNode);
                    pPlanEauGeoNode = new TYPlanEauGeoNode(NULL, this);
                }
            }
        }
    }

    if (!bDefTerrainOk) // pas de terrain par defaut trouve, on suppose qu'il s'agit d'un fichier a l'ancien format...
    {
        // Pour tous les terrains
        LPTYTerrain pTerrain = NULL;
        for (i = 0; i < _listTerrain.size(); i++)
        {
            pTerrain = dynamic_cast<TYTerrain*>(_listTerrain[i]->getElement());

            // Si taille de la liste de point = 0
            if (pTerrain && pTerrain->getListPoints().size() == 0)
            {
                // Ce terrain est l'ancien terrain par defaut
                this->setDefTerrain(i);
            }
        }
    }

    return 1;
}


void TYTopographie::setIsGeometryModified(bool isModified)
{
    TYElement::setIsGeometryModified(isModified);

    if (_pParent) { _pParent->setIsGeometryModified(isModified); }
}

void TYTopographie::reparent()
{
    _pAltimetrie->setParent(this);
    unsigned int i;
    for (i = 0; i < _listCrbNiv.size(); i++)
    {
        _listCrbNiv[i]->setParent(this);
        _listCrbNiv[i]->getElement()->setParent(this);
    }

    for (i = 0; i < _listTerrain.size(); i++)
    {
        _listTerrain[i]->setParent(this);
        _listTerrain[i]->getElement()->setParent(this);
    }

    for (i = 0; i < _listCrsEau.size(); i++)
    {
        _listCrsEau[i]->setParent(this);
        _listCrsEau[i]->getElement()->setParent(this);
    }

    for (i = 0; i < _listPlanEau.size(); i++)
    {
        _listPlanEau[i]->setParent(this);
        _listPlanEau[i]->getElement()->setParent(this);
    }
}

void TYTopographie::updateCurrentCalcul(TYListID& listID, bool recursif)//=true
{
    TYElement::updateCurrentCalcul(listID, false);
}

void TYTopographie::purge()
{
    resetEmprise();
    remAllCrbNiv();
    remAllCrsEau();
    remAllTerrain();
    remAllPlanEau();

    if (_pSortedTerrains)
    {
        delete [] _pSortedTerrains;
        _pSortedTerrains = NULL;
    }

    setIsGeometryModified(true);
}

void TYTopographie::concatTopo(const TYTopographie* pTopo, bool emprise /*= true*/)
{
    unsigned int i;

    for (i = 0; i < pTopo->_listCrbNiv.size(); ++i)
    {
        _listCrbNiv.push_back(pTopo->_listCrbNiv[i]);
    }

    for (i = 0; i < pTopo->_listTerrain.size(); ++i)
    {
        _listTerrain.push_back(pTopo->_listTerrain[i]);
    }

    for (i = 0; i < pTopo->_listCrsEau.size(); ++i)
    {
        _listCrsEau.push_back(pTopo->_listCrsEau[i]);
    }

    for (i = 0; i < pTopo->_listPlanEau.size(); ++i)
    {
        _listPlanEau.push_back(pTopo->_listPlanEau[i]);
    }

    if (emprise)
    {
        resetEmprise();
        for (i = 0; i < pTopo->_emprise.size(); ++i)
        {
            addPointEmprise(pTopo->_emprise[i]);
        }
    }
}

bool TYTopographie::addPlanEau(LPTYPlanEauGeoNode pPlanEauGeoNode)
{
    assert(pPlanEauGeoNode);

    TYPlanEau* pPlanEau = dynamic_cast<TYPlanEau*>(pPlanEauGeoNode->getElement());

    assert(pPlanEau);

    pPlanEauGeoNode->setParent(this);
    pPlanEau->setParent(this);

    _listPlanEau.push_back(pPlanEauGeoNode);

    setIsGeometryModified(true);

    // L'alti n'est plus a jour non plus...
    _pAltimetrie->setIsGeometryModified(true);

    return true;
}

bool TYTopographie::addPlanEau(LPTYPlanEau pPlanEau)
{
    return addPlanEau(new TYPlanEauGeoNode((LPTYElement)pPlanEau));
}

bool TYTopographie::remPlanEau(const LPTYPlanEauGeoNode pPlanEauGeoNode)
{
    assert(pPlanEauGeoNode);
    bool ret = false;
    TYTabPlanEauGeoNode::iterator ite;

    for (ite = _listPlanEau.begin(); ite != _listPlanEau.end(); ite++)
    {
        if ((*ite) == pPlanEauGeoNode)
        {
            //#if TY_USE_IHM
            //          (*ite)->remFromAllRenderer();
            //#endif
            _listPlanEau.erase(ite);
            ret = true;
            break;
        }
    }

    setIsGeometryModified(true);

    // L'alti n'est plus a jour non plus...
    _pAltimetrie->setIsGeometryModified(true);

    return ret;
}

bool TYTopographie::remPlanEau(const LPTYPlanEau pPlanEau)
{
    assert(pPlanEau);
    bool ret = false;
    TYTabPlanEauGeoNode::iterator ite;

    for (ite = _listPlanEau.begin(); ite != _listPlanEau.end(); ite++)
    {
        if (dynamic_cast<TYPlanEau*>((*ite)->getElement()) == pPlanEau)
        {
            //#if TY_USE_IHM
            //          (*ite)->remFromAllRenderer();
            //#endif
            _listPlanEau.erase(ite);
            ret = true;
            break;
        }
    }

    setIsGeometryModified(true);

    // L'alti n'est plus a jour non plus...
    _pAltimetrie->setIsGeometryModified(true);

    return ret;
}

bool TYTopographie::remPlanEau(QString idPlanEau)
{
    bool ret = false;
    TYTabPlanEauGeoNode::iterator ite;

    for (ite = _listPlanEau.begin(); ite != _listPlanEau.end(); ite++)
    {
        if ((*ite)->getElement()->getID().toString() == idPlanEau)
        {
            //#if TY_USE_IHM
            //          (*ite)->remFromAllRenderer();
            //#endif
            _listPlanEau.erase(ite);
            ret = true;
            break;
        }
    }

    setIsGeometryModified(true);

    // L'alti n'est plus a jour non plus...
    _pAltimetrie->setIsGeometryModified(true);

    return ret;
}

void TYTopographie::remAllPlanEau()
{
    _listPlanEau.clear();

    setIsGeometryModified(true);

    // L'alti n'est plus a jour non plus...
    _pAltimetrie->setIsGeometryModified(true);
}

LPTYPlanEauGeoNode TYTopographie::findPlanEau(const LPTYPlanEau pPlanEau)
{
    assert(pPlanEau);
    TYTabPlanEauGeoNode::iterator ite;

    for (ite = _listPlanEau.begin(); ite != _listPlanEau.end(); ite++)
    {
        if (dynamic_cast<TYPlanEau*>((*ite)->getElement()) == pPlanEau)
        {
            return (*ite);
        }
    }

    return NULL;
}

bool TYTopographie::addCrsEau(LPTYCoursEauGeoNode pCoursEauGeoNode)
{
    assert(pCoursEauGeoNode);

    LPTYCoursEau pCoursEau = dynamic_cast<TYCoursEau*>(pCoursEauGeoNode->getElement());

    assert(pCoursEau);

    pCoursEauGeoNode->setParent(this);
    pCoursEau->setParent(this);

    _listCrsEau.push_back(pCoursEauGeoNode);

    setIsGeometryModified(true);

    return true;
}

bool TYTopographie::addCrsEau(LPTYCoursEau pCoursEau)
{
    return addCrsEau(new TYCoursEauGeoNode((LPTYElement)pCoursEau));
}

bool TYTopographie::remCrsEau(const LPTYCoursEauGeoNode pCoursEauGeoNode)
{
    assert(pCoursEauGeoNode);
    bool ret = false;
    TYTabCoursEauGeoNode::iterator ite;

    for (ite = _listCrsEau.begin(); ite != _listCrsEau.end(); ite++)
    {
        if ((*ite) == pCoursEauGeoNode)
        {
            //#if TY_USE_IHM
            //          (*ite)->remFromAllRenderer();
            //#endif
            _listCrsEau.erase(ite);
            ret = true;
            break;
        }
    }

    setIsGeometryModified(true);

    return ret;
}

bool TYTopographie::remCrsEau(const LPTYCoursEau pCoursEau)
{
    assert(pCoursEau);
    bool ret = false;
    TYTabCoursEauGeoNode::iterator ite;

    for (ite = _listCrsEau.begin(); ite != _listCrsEau.end(); ite++)
    {
        if (dynamic_cast<TYCoursEau*>((*ite)->getElement()) == pCoursEau)
        {
            //#if TY_USE_IHM
            //          (*ite)->remFromAllRenderer();
            //#endif
            _listCrsEau.erase(ite);
            ret = true;
            break;
        }
    }

    setIsGeometryModified(true);

    return ret;
}

bool TYTopographie::remCrsEau(QString idCrsEau)
{
    bool ret = false;
    TYTabCoursEauGeoNode::iterator ite;

    for (ite = _listCrsEau.begin(); ite != _listCrsEau.end(); ite++)
    {
        if (dynamic_cast<TYCoursEau*>((*ite)->getElement())->getID().toString() == idCrsEau)
        {
            //#if TY_USE_IHM
            //          (*ite)->remFromAllRenderer();
            //#endif
            _listCrsEau.erase(ite);
            ret = true;
            break;
        }
    }

    setIsGeometryModified(true);

    return ret;
}

void TYTopographie::remAllCrsEau()
{
    _listCrsEau.clear();
    setIsGeometryModified(true);
}

LPTYCoursEauGeoNode TYTopographie::findCrsEau(const LPTYCoursEau pCrsEau)
{
    assert(pCrsEau);
    TYTabCoursEauGeoNode::iterator ite;

    for (ite = _listCrsEau.begin(); ite != _listCrsEau.end(); ite++)
    {
        if (dynamic_cast<TYCoursEau*>((*ite)->getElement()) == pCrsEau)
        {
            return (*ite);
        }
    }

    return NULL;
}

bool TYTopographie::addTerrain(LPTYTerrainGeoNode pTerGeoNode)
{
    assert(pTerGeoNode);

    TYTerrain* pTerrain = dynamic_cast<TYTerrain*>(pTerGeoNode->getElement());

    assert(pTerrain);

    pTerGeoNode->setParent(this);
    pTerrain->setParent(this);

    _listTerrain.push_back(pTerGeoNode);

    setIsGeometryModified(true);

    return true;
}

bool TYTopographie::addTerrain(LPTYTerrain pTer)
{
    return addTerrain(new TYTerrainGeoNode((LPTYElement)pTer));
}

bool TYTopographie::remTerrain(const LPTYTerrainGeoNode pTerGeoNode)
{
    assert(pTerGeoNode);
    // On efface pas le terrain par defaut
    if (pTerGeoNode == _listTerrain[_DefTerrainIdx]) { return false; }

    bool ret = false;
    unsigned int terrainNbr = 0;

    TYTabTerrainGeoNode::iterator ite;

    for (ite = _listTerrain.begin(); ite != _listTerrain.end(); ite++)
    {
        if ((*ite) == pTerGeoNode)
        {

            //#if TY_USE_IHM
            //          (*ite)->remFromAllRenderer();
            //#endif
            _listTerrain.erase(ite);

            if (terrainNbr < _DefTerrainIdx) { _DefTerrainIdx--; } // Dans ce cas, on decremente l'indice du terrain par defaut

            ret = true;
            break;
        }

        terrainNbr++;
    }

    setIsGeometryModified(true);

    return ret;
}

bool TYTopographie::remTerrain(const LPTYTerrain pTer)
{
    assert(pTer);
    bool ret = false;
    unsigned int terrainNbr = 0;
    TYTabTerrainGeoNode::iterator ite;

    for (ite = _listTerrain.begin(); ite != _listTerrain.end(); ite++)
    {
        if (dynamic_cast<TYTerrain*>((*ite)->getElement()) == pTer)
        {
            if ((*ite) == _listTerrain[_DefTerrainIdx]) { return false; }

            //#if TY_USE_IHM
            //          (*ite)->remFromAllRenderer();
            //#endif
            _listTerrain.erase(ite);

            if (terrainNbr < _DefTerrainIdx) { _DefTerrainIdx--; } // Dans ce cas, on decremente l'indice du terrain par defaut

            ret = true;
            break;
        }

        terrainNbr++;
    }

    setIsGeometryModified(true);

    return ret;
}

bool TYTopographie::remTerrain(QString idTerrain)
{
    bool ret = false;
    unsigned int terrainNbr = 0;
    TYTabTerrainGeoNode::iterator ite;

    for (ite = _listTerrain.begin(); ite != _listTerrain.end(); ite++)
    {
        if ((*ite)->getElement()->getID().toString() == idTerrain)
        {
            //#if TY_USE_IHM
            //          (*ite)->remFromAllRenderer();
            //#endif
            _listTerrain.erase(ite);

            if (terrainNbr < _DefTerrainIdx) { _DefTerrainIdx--; } // Dans ce cas, on decremente l'indice du terrain par defaut

            ret = true;
            break;
        }

        terrainNbr++;
    }

    setIsGeometryModified(true);

    return ret;
}

void TYTopographie::remAllTerrain()
{
    _listTerrain.clear();
    setIsGeometryModified(true);
}

LPTYTerrainGeoNode TYTopographie::findTerrain(const LPTYTerrain pTerrain)
{
    assert(pTerrain);
    TYTabTerrainGeoNode::iterator ite;

    for (ite = _listTerrain.begin(); ite != _listTerrain.end(); ite++)
    {
        if (dynamic_cast<TYTerrain*>((*ite)->getElement()) == pTerrain)
        {
            return (*ite);
        }
    }

    return NULL;
}

bool TYTopographie::addCrbNiv(LPTYCourbeNiveauGeoNode pCrbNivGeoNode)
{
    assert(pCrbNivGeoNode);

    TYCourbeNiveau* pCourbeNiv = dynamic_cast<TYCourbeNiveau*>(pCrbNivGeoNode->getElement());

    assert(pCourbeNiv);

    pCrbNivGeoNode->setParent(this);
    pCourbeNiv->setParent(this);

    _listCrbNiv.push_back(pCrbNivGeoNode);

    setIsGeometryModified(true);

    // L'alti n'est plus a jour non plus...
    _pAltimetrie->setIsGeometryModified(true);

    return true;
}

bool TYTopographie::addCrbNiv(LPTYCourbeNiveau pCrbNiv)
{
    return addCrbNiv(new TYCourbeNiveauGeoNode((LPTYElement)pCrbNiv));
}

bool TYTopographie::remCrbNiv(const LPTYCourbeNiveauGeoNode pCrbNivGeoNode)
{
    assert(pCrbNivGeoNode);
    bool ret = false;
    TYTabCourbeNiveauGeoNode::iterator ite;

    for (ite = _listCrbNiv.begin(); ite != _listCrbNiv.end(); ite++)
    {
        if ((*ite) == pCrbNivGeoNode)
        {
            //#if TY_USE_IHM
            //          (*ite)->remFromAllRenderer();
            //#endif
            _listCrbNiv.erase(ite);
            ret = true;
            break;
        }
    }

    setIsGeometryModified(true);

    // L'alti n'est plus a jour non plus...
    _pAltimetrie->setIsGeometryModified(true);

    return ret;
}

bool TYTopographie::remCrbNiv(const LPTYCourbeNiveau pCrbNiv)
{
    assert(pCrbNiv);
    bool ret = false;
    TYTabCourbeNiveauGeoNode::iterator ite;

    for (ite = _listCrbNiv.begin(); ite != _listCrbNiv.end(); ite++)
    {
        if (dynamic_cast<TYCourbeNiveau*>((*ite)->getElement()) == pCrbNiv.getRealPointer())
        {
            //#if TY_USE_IHM
            //          (*ite)->remFromAllRenderer();
            //#endif
            _listCrbNiv.erase(ite);
            ret = true;
            break;
        }
    }

    setIsGeometryModified(true);

    // L'alti n'est plus a jour non plus...
    _pAltimetrie->setIsGeometryModified(true);

    return ret;
}

bool TYTopographie::remCrbNiv(QString idCrbNiv)
{
    bool ret = false;
    TYTabCourbeNiveauGeoNode::iterator ite;

    for (ite = _listCrbNiv.begin(); ite != _listCrbNiv.end(); ite++)
    {
        if ((*ite)->getElement()->getID().toString() == idCrbNiv)
        {
            //#if TY_USE_IHM
            //          (*ite)->remFromAllRenderer();
            //#endif
            _listCrbNiv.erase(ite);
            ret = true;
            break;
        }
    }

    setIsGeometryModified(true);

    // L'alti n'est plus a jour non plus...
    _pAltimetrie->setIsGeometryModified(true);

    return ret;
}

void TYTopographie::remAllCrbNiv()
{
    _listCrbNiv.clear();

    setIsGeometryModified(true);

    // L'alti n'est plus a jour non plus...
    _pAltimetrie->setIsGeometryModified(true);
}

LPTYCourbeNiveauGeoNode TYTopographie::findCrbNiv(const LPTYCourbeNiveau pCrbNiv)
{
    assert(pCrbNiv);
    TYTabCourbeNiveauGeoNode::iterator ite;

    for (ite = _listCrbNiv.begin(); ite != _listCrbNiv.end(); ite++)
    {
        if (dynamic_cast<TYCourbeNiveau*>((*ite)->getElement()) == pCrbNiv.getRealPointer())
        {
            return (*ite);
        }
    }

    return NULL;
}

void TYTopographie::computeAltimetricTriangulation(
    std::deque<OPoint3D>& points,
    std::deque<OTriangle>& triangles,
    bool use_emprise_as_level_curve)
{

    // we instanciate an AltimetryBuilder
    p_alti_builder = std::move(tympan::make_altimetry_builder());

    // We ask it to process this topography
    p_alti_builder->process(*this, use_emprise_as_level_curve);
    p_alti_builder->insertMaterialPolygonsInTriangulation();
    if (p_alti_builder->number_of_faces() == 0)
    {
        return;
    }

    // We do update the materials
    p_alti_builder->indexFacesMaterial();
    // There should be a actual default ground material in the app
    // But actually the default material in TYTerrain happens to be
    // a new default constructed TYSol. So we do the same.
    // TODO cf https://extranet.logilab.fr/ticket/1481980
    LPTYSol def_mat(new TYSol());
    p_alti_builder->labelFaces(def_mat);

    //Fill points and triangle
    p_alti_builder->exportMesh(points, triangles);

    std::cerr << "Mise a jour altimetrie (Triangulation):  "
              << number_of_vertices() << " points et "
              << number_of_faces() << " triangles." << std::endl;

    setIsGeometryModified(true); // For compatibility (XXX to be checked)
}
//
//void TYTopographie::computeAltimetrie()
//{
//  double distanceMax = 200.0;
//
//#if TY_USE_IHM
//  if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "DistMinPtCrbNiv"))
//  {
//      distanceMax = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "DistMinPtCrbNiv");
//  }
//  else
//  {
//      TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "DistMinPtCrbNiv", distanceMax);
//  }
//#endif
//
//  // Calcul du parametre de Delaunay
//  double delaunay;
//  OSegment3D diagTopo;
//
//  delaunay = 5.0/getTopoSize(diagTopo); // 5 pour eviter les tout petits triangles
//
//  // Construction de l'altimetrie
//  _pAltimetrie->compute(collectPointsForAltimetrie(distanceMax), delaunay);
//}

/* is being replaced by AltymetryBuilder::process
//TYTabPoint TYTopographie::collectPointsForAltimetrie(const double& distanceMax, bool bEmpriseAsCrbNiv) const
TYTabPoint TYTopographie::collectPointsForAltimetrie(bool bEmpriseAsCrbNiv) const
{
    TYTabPoint pts;

    size_t i, j;
    TYTabPoint tabPt;
    TYTabPoint tabPtTemp;
    float altitude(0.0);
    double distMax(0.0);
    TYPoint pt;
    size_t nbCrbs = _listCrbNiv.size();

    if (bEmpriseAsCrbNiv || nbCrbs == 0)
    {
        //      if (_emprise.size())
        //      {
        // On recupere l'altitude du site parent
        TYSiteNode* pSiteNode = dynamic_cast<TYSiteNode*>(const_cast<TYElement*>(this->getParent()));
        assert(pSiteNode);
        altitude = pSiteNode->getAltiEmprise();
        TYCourbeNiveau* pCourbeNiv = new TYCourbeNiveau(_emprise, altitude);
        pCourbeNiv->close();
        distMax = pCourbeNiv->getDistMax();

        //if (pSiteNode)
        //{
        //  z = pSiteNode->getAltiEmprise();
        //}

        // On utilise les points de l'emprise pour en creer une (fermee)
        //tabPt = _emprise;
        //tabPt.push_back(_emprise[0]);

        //// On affecte a tous les points l'altitude z
        //for (i=0; i<tabPt.size(); i++)
        //{
        //  tabPt[i]._z = z;
        //}

        tabPt = pCourbeNiv->getListPoints();

        // On ajoute des points si necessaire pour une meilleure precision
        pts = TYPoint::checkPointsMaxDistance(tabPt, distMax);

        delete pCourbeNiv;
        pCourbeNiv = NULL;
        //      }
    }

    // Extraction des points des courbes de niveau.
    for (i = 0; i < nbCrbs; i++)
    {
        TYCourbeNiveau* pCourbeNiv = dynamic_cast<TYCourbeNiveau*>(_listCrbNiv[i]->getElement());
        // Si la courbe de niveau possede une distMax propre, elle est prise en compte
        distMax = pCourbeNiv->getDistMax();
        altitude = pCourbeNiv->getAltitude();

        // On ajoute des points si necessaire pour une meilleure precision
        tabPt = pCourbeNiv->getListPoints();
        tabPtTemp = TYPoint::checkPointsMaxDistance(tabPt, distMax);

        const OMatrix& pMatrix = _listCrbNiv[i]->getMatrix();

        for (j = 0; j < tabPtTemp.size(); j++)
        {
            // Point
            pt = tabPtTemp[j];

            // Changement de repere
            pt = pMatrix.operator * (pt);

            // Altitude
            pt._z = altitude;

            // Ajout
            pts.push_back(pt);
        }

        tabPt.clear();
        tabPtTemp.clear();
    }


    // Extraction des points des plans d'eau.
    for (i = 0; i < _listPlanEau.size(); i++)
    {
        TYPlanEau* pPlanEau = dynamic_cast<TYPlanEau*>(_listPlanEau[i]->getElement());

        tabPt = pPlanEau->getCrbNiv()->getListPoints();
        altitude = pPlanEau->getAltitude();

        // Si le plan d'eau possede une distMax propre, elle est prise en compte
        double distMax = pPlanEau->getDistMax();
        TYTabPoint tabPtTemp = TYPoint::checkPointsMaxDistance(tabPt, distMax);

        const OMatrix& pMatrix = _listPlanEau[i]->getMatrix();

        for (j = 0; j < tabPtTemp.size(); j++)
        {
            // Point
            pt = tabPtTemp[j];

            // Changement de repere
            pt = pMatrix.operator * (pt);

            // Altitude
            pt._z = altitude;

            // Ajout
            pts.push_back(pt);
        }

        tabPt.clear();
        tabPtTemp.clear();
    }

    return pts;
}
*/


bool TYTopographie::penteMoy(const OSegment3D& seg, OSegment3D& penteMoy) const
{
    /* ========================================================================================================
        07/03/2005 : Le calcul de la pente moyenne par les moindres carres est mis en commentaire et remplace
                     par un segment defini comme la simple projection au sol de la source et du recepteur.

       ======================================================================================================== */

    OPoint3D ptA(seg._ptA), ptB(seg._ptB);


    // Recuperation des altitudes correspondant aux extremites du segment
    ptA._z = ptB._z = 0.0; // Mise a 0 au cas oi�� les points ne soient pas sur l'alti
    bool resu = _pAltimetrie._pObj->updateAltitude(ptA);
    resu &= _pAltimetrie._pObj->updateAltitude(ptB);

    penteMoy = OSegment3D(ptA, ptB);
    return resu;
}

LPTYTerrain TYTopographie::solMoy(const OSegment3D& seg, const TYAtmosphere& atmo)
{
    TYSol* pSol;
    TYSol* pSolRes = new TYSol();
    TYTabStructSegPtrTer tabPtrTerSeg;
    double  poids,
            resMoyenne,
            epaisseurMoyenne,
            sommeResistivite = 0.0,
            sommeEpaisseur = 0.0,
            sommePoids = 0.0;

    // Recuperation du tableau des terrains et segments associes
    tabPtrTerSeg = sols(seg);
    size_t nbSols = tabPtrTerSeg.size();

    if (nbSols == 0) { return dynamic_cast<TYTerrain*>(_listTerrain[_DefTerrainIdx]->getElement()); }

    for (unsigned int  i = 0; i < nbSols; i++)
    {
        pSol = tabPtrTerSeg.at(i)._terrain->getSol();
        poids = tabPtrTerSeg.at(i)._segment.longueur();

        // Sommes ponderees par la taille du terrain
        sommeResistivite += pSol->getResistivite() * poids;
        sommeEpaisseur += pSol->getEpaisseur() * poids;
        sommePoids += poids;
    }

    resMoyenne = sommeResistivite / sommePoids;
    epaisseurMoyenne = sommeEpaisseur / sommePoids;

    pSolRes->setResistivite(resMoyenne);
    pSolRes->setEpaisseur(epaisseurMoyenne);

    pSolRes->calculNombreDOnde(atmo);

    TYTerrain* pTerrain = new TYTerrain();

    pTerrain->setSol(pSolRes);

    return pTerrain;
}

TYTerrain* TYTopographie::terrainAt(const OPoint3D& pt)
{
    TYElement* pFound = NULL;
    OPoint3D testPt = pt;
    testPt._z = 0;
    OPoint3D* pts1 = NULL, *pts2 = NULL;

    // On cherche d'abord dans la liste des plans d'eau
    TYPlanEau* pPlanEau = NULL;
    size_t i = 0, j, nbPts;

    while ((i < _listPlanEau.size()) && (pFound == NULL))
    {
        pPlanEau = dynamic_cast<TYPlanEau*>(_listPlanEau.at(i)._pObj->getElement());
        if (!pPlanEau) { i++; continue; }

        const OMatrix& mat = _listPlanEau.at(i)._pObj->getMatrix();

        nbPts = pPlanEau->getListPoints().size();
        if (!nbPts)
        {
            i++;
            continue; // Si terrain non defini par des points on le saute
        }

        pts1 = new OPoint3D[nbPts];

        for (j = 0; j < nbPts; j++)
        {
            pts1[j] = mat * pPlanEau->getListPoints().at(j);
            pts1[j]._z = 0;
        }

#if TY_USE_IHM
        //        if (OGeometrie::pointInPolygonRayCasting(testPt, pts1, static_cast<int>(nbPts) , TYPolygon(pPlanEau->getListPoints()).normal(), box) == 1)
        if (OGeometrie::pointInPolygonRayCasting(testPt, pts1, static_cast<int>(nbPts)) == 1)
#else
        if (OGeometrie::pointInPolygonAngleSum(testPt, pts, nbPts))
#endif
        {
            pFound = pPlanEau;
        }

        delete [] pts1;
        pts1 = NULL;

        i++;
    }

    if (pFound) { return (TYTerrain*) pFound; }

    // On explore ensuite des terrains
    TYTerrain* pTerrain = NULL;
    size_t nbTerrain = _listTerrain.size();
    i = 0;

    while ((i < nbTerrain) && (pFound == NULL))
    {
        pTerrain = dynamic_cast<TYTerrain*>(_pSortedTerrains[i]->getElement());
        if (!pTerrain) { i++; continue; }
        const OMatrix& mat = _pSortedTerrains[i]->getMatrix();

        nbPts = pTerrain->getListPoints().size();
        if (!nbPts)
        {
            i++;
            continue; // Si terrain non defini par des points on le saute
        }

        pts2 = new OPoint3D[nbPts];

        for (j = 0; j < nbPts; j++)
        {
            pts2[j] = mat * pTerrain->getListPoints().at(j);
            pts2[j]._z = 0;
        }

#if TY_USE_IHM
        //        if (OGeometrie::pointInPolygonRayCasting(testPt, pts2, static_cast<int>(nbPts), TYPolygon(pTerrain->getListPoints()).normal(), box) == 1)
        if (OGeometrie::pointInPolygonRayCasting(testPt, pts2, static_cast<int>(nbPts)) == 1)
#else
        if (OGeometrie::pointInPolygonAngleSum(testPt, pts, static_cast<int>(nbPts)))
#endif
        {
            pFound = pTerrain;
        }

        delete [] pts2;
        pts2 = NULL;

        i++;
    }

    // modif DTn 20091215 Le terrain par defaut n'est plus defini par l'emprise du site
    if (!pFound) { pFound = this->getDefTerrain(); }

    return (TYTerrain*) pFound;
}

TYTabStructSegPtrTer TYTopographie::sols(const OSegment3D& seg)
{
    TYTabStructSegPtrTer tabPtrTerSeg;
    TYStructSegPtrTer segTer;
    TYTerrain* pTerrain = NULL;
    double x0, y0, x1, y1, pasX, pasY;
    OPoint3D pt;
    OSegment3D sousSegment(seg._ptA, seg._ptB); // Par defaut, le sous-segment est egal au segment initial

    double longueur = seg.longueur();

    // Si segment de longeur nulle inutile de continuer (retour d'un tableau vide)
    if (longueur == 0.0) { return tabPtrTerSeg; }

    x0 = seg._ptA._x;
    y0 = seg._ptA._y;

    x1 = seg._ptB._x;
    y1 = seg._ptB._y;

    pasX = (x1 - x0) / longueur;
    pasY = (y1 - y0) / longueur;

    int nbStep = (int) longueur + 1; // plus 1 par securite

    pt._x = x0;
    pt._y = y0;
    pt._z = 0;

    double Dx = ABS(x1 - x0);
    double Dy = ABS(y1 - y0);

    for (int i = 0 ; i < nbStep ; i++)
    {
        // Changement de terrain
        if (pTerrain != terrainAt(pt))
        {
            if (pTerrain != NULL)
            {
                // Fermeture du segment precedent et insertion dans la liste
                sousSegment._ptB = pt;
                segTer._segment = sousSegment;
                segTer._terrain = pTerrain;
                tabPtrTerSeg.push_back(segTer);
            }
            else // eviter qu'un terrain non defini soit ajoute a la liste
            {
                continue;
            }

            // Ouverture du nouveau sous-segment
            sousSegment._ptA = pt;

            // Definition du terrain
            pTerrain = terrainAt(pt);

        }
        // Incrementation en X et en Y

        pt._x += pasX;
        pt._y += pasY;

        double distX = ABS(pt._x - x0);
        double distY = ABS(pt._y - y0);
        if ((distX >= Dx) && (distY >= Dy))   // On a depasse les bornes des limites
        {
            sousSegment._ptB = seg._ptB;
            segTer._segment = sousSegment;
            segTer._terrain = pTerrain;
            tabPtrTerSeg.push_back(segTer);
            break;
        }
    }

    return tabPtrTerSeg;
}

void TYTopographie::updateSol(const TYAtmosphere& atmo)
{
    // Traitement des terrains defini sur le site
    LPTYTerrain terrain = NULL;
    LPTYSol sol = NULL;

    for (unsigned int i = 0 ; i < _listTerrain.size() ; i++)
    {
        terrain = dynamic_cast<TYTerrain*>(_listTerrain[i]->getElement());
        if (terrain->isA("TYPlanEau"))
        {
            continue;
        }
        else
        {
            sol = terrain->getSol();
            sol->calculNombreDOnde(atmo);
        }
    }
}

double TYTopographie::getTopoSize(OSegment3D& segDiagonale)
{
    double longueur = 0;

    double minX = +20000;
    double maxX = -20000;
    double minY = +20000;
    double maxY = -20000;

    // On calcule la dimension caracteristique definie par les courbes de niveau
    TYTabCourbeNiveauGeoNode::iterator ite;
    for (ite = _listCrbNiv.begin(); ite != _listCrbNiv.end(); ite++)
    {
        TYCourbeNiveau* pCourbe = dynamic_cast<TYCourbeNiveau*>((*ite)->getElement());
        TYTabPoint tabPoint = pCourbe->getListPoints();

        for (unsigned int i = 0 ; i < tabPoint.size(); i++)
        {
            if (tabPoint[i]._x < minX) { minX = tabPoint[i]._x; }
            if (tabPoint[i]._y < minY) { minY = tabPoint[i]._y; }

            if (tabPoint[i]._x > maxX) { maxX = tabPoint[i]._x; }
            if (tabPoint[i]._y > maxY) { maxY = tabPoint[i]._y; }
        }
    }

    OPoint3D ptMinCourbNiv(minX, minY, 0.0);
    OPoint3D ptMaxCourbNiv(maxX, maxY, 0.0);
    OSegment3D segCrbNiv(ptMinCourbNiv, ptMaxCourbNiv);
    double longueurCrb = segCrbNiv.longueur();

    // On calcule la dimension carateristique definie par l'emprise

    minX = +20000;
    maxX = -20000;
    minY = +20000;
    maxY = -20000;
    for (unsigned int i = 0 ; i < _emprise.size() ; i++)
    {
        if (_emprise[i]._x < minX) { minX = _emprise[i]._x; }
        if (_emprise[i]._y < minY) { minY = _emprise[i]._y; }

        if (_emprise[i]._x > maxX) { maxX = _emprise[i]._x; }
        if (_emprise[i]._y > maxY) { maxY = _emprise[i]._y; }
    }

    OPoint3D ptMinEmprise = OPoint3D(minX, minY, 0.0);
    OPoint3D ptMaxEmprise = OPoint3D(maxX, maxY, 0.0);
    OSegment3D segEmprise(ptMinEmprise, ptMaxEmprise);
    double longueurEmprise = segEmprise.longueur();

    // On compare les deux et on prend le plus grand

    if (longueurCrb > longueurEmprise)
    {
        segDiagonale = segCrbNiv;
        return longueurCrb;
    }
    else
    {
        segDiagonale = segEmprise;
        return longueurEmprise;
    }

    return longueur;

}

void TYTopographie::setEmprise(const TYTabPoint& pts, const bool& defTerrain)
{
    _emprise = pts;

    // On affecte au terrain par defaut le contour de l'emprise
    //  getDefTerrain()->setListPoints(_emprise);

    //  if (defTerrain) setDefTerrain(_DefTerrainIdx);

    setIsGeometryModified(true);
}

void TYTopographie::setDefTerrain(int defTerrainIdx)
{
    if (_listTerrain.size() == 0) { return; }
    //  assert( _listTerrain.size());
    TYTerrain* pTerrain = getDefTerrain();

    LPTYSol pSol = getTerrain(defTerrainIdx)->getSol();
    if ((defTerrainIdx > 0) && (defTerrainIdx < _listTerrain.size()))
    {
        _DefTerrainIdx = defTerrainIdx;
    }
    else
    {
        _DefTerrainIdx = 0;
    }

    // On affecte le sol du terrain choisi par defaut le perimetre defini par l'emprise
    if (pTerrain) { pTerrain->setSol(pSol); }
}

TYTerrain* TYTopographie::getDefTerrain()
{
    assert(_DefTerrainIdx < _listTerrain.size()); // Securite
    return dynamic_cast<TYTerrain*>(_listTerrain[_DefTerrainIdx]._pObj->getElement());
}

void TYTopographie::sortTerrainsBySurface()
{
    // 1. Nettoyage du tableau des terrains si non vide
    if (_pSortedTerrains)
    {
        delete [] _pSortedTerrains;
        _pSortedTerrains = NULL;
    }
    // 2. Generation du tableau des terrains
    size_t nbTerrains = _listTerrain.size();
    _pSortedTerrains = new TYTerrainGeoNode*[nbTerrains];

    for (size_t i = 0; i < nbTerrains; i++)
    {
        _pSortedTerrains[i] = _listTerrain[i]._pObj;
    }

    // 3. Tri du tableau
    qsort(_pSortedTerrains, nbTerrains, sizeof(TYTerrainGeoNode*), compareSurfaceTerrains);
}

int compareSurfaceTerrains(const void* elem1, const void* elem2)
{
    TYTerrainGeoNode* pTerrainNode = *((TYTerrainGeoNode**) elem1);
    TYTerrain* Terrain1 = dynamic_cast<TYTerrain*>(pTerrainNode->getElement());

    pTerrainNode = *((TYTerrainGeoNode**)  elem2);
    TYTerrain* Terrain2 = dynamic_cast<TYTerrain*>(pTerrainNode->getElement());

    // TODO return cmp(Terrain1->surface(), Terrain2->surface()) ?
    double res = Terrain1->surface() - Terrain2->surface();
    int sgn = int(res / fabs(res));
    return (sgn);
}

const tympan::IAltimetryBuilder& TYTopographie::getAltimetryBuilder() const
{ return *p_alti_builder; }

unsigned TYTopographie::number_of_vertices() const
{ return p_alti_builder->number_of_vertices(); }

unsigned TYTopographie::number_of_faces() const
{ return p_alti_builder->number_of_faces(); }

void TYTopographie::exportMesh(std::deque<OPoint3D>& points, std::deque<OTriangle>& triangles, std::deque<LPTYSol>* p_materials)
{
    p_alti_builder->exportMesh(points, triangles, p_materials);
}
