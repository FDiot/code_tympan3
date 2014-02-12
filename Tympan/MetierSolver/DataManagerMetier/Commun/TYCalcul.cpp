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

#ifdef _MSC_VER
#   pragma warning (disable : 4786)
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include <stdlib.h>
#include <string>

#include "Tympan/MetierSolver/DataManagerCore/TYElementCollection.h"

#include "Tympan/Tools/TYProgressManager.h"

#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/Tools/OMessageManager.h"

#if defined(WIN32)
#include <crtdbg.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif

#include "Tympan/MetierSolver/DataManagerCore/TYXMLManager.h"
#include "Tympan/MetierSolver/DataManagerCore/TYPluginManager.h"

OPROTOINST(TYCalcul);

#define TR(id) OLocalizator::getString("OMessageManager", (id))

TYCalcul::TYCalcul(LPTYProjet pParent /*=NULL*/)
{
    _name = TYNameManager::get()->generateName(getClassName());

    _auteur = "Auteur";
    _comment = "Commentaire";
    _dateCreation = "2001-10-01";
    _dateModif = "2001-10-01";
    _numero = 1;

    _pAtmosphere = new TYAtmosphere();
    _pAtmosphere->setParent(this);

    _pResultat = new TYResultat();
    _pResultat->setParent(this);

    _upTodate = true;

    _expansGeo = 0; // 4pi
    _useSol = true;
    _typeCalculSol = 0; // Sol Local
    _condFav = false;
    _useVegetation = false;
    _useAtmosphere = true;
    _useReflexion = false;
    _useEcran = true;
    _bCalculTrajetsHorizontaux = true;

    _useExternal = false;   // Par defaut, on utilise la methode interne
    _refExternal = 0;       // Reference de la methode externe utilisee

    _geomPrecision = 10.0;  // Precicion 10%

    HARM_averaging = 2; // Lissage
    HARM_airAbsorption = 4; // Prise en compte de l'absorption atmospherique
    HARM_turbulenceScattering = 8; // Prise en compte de la diffusion turbulente
    HARM_diffHaddenPierce = 0; // Non utilisation de la Formule de diffraction de Hadde-Pierce
    HARM_randomTerrain = 0; // Non activation des Perturbation aleatoire du profil du terrain

    _useReflexion = false;
    _interference = false;

    _state = TYCalcul::Actif; // A sa creation, le calcul est actif

    _h1 = 10.0;
    _distanceSRMin = 0.3;
    _anechoic = false; // Par defaut : conditions semi-anechoiques
    _freqMinSpectre = 16.0;
    _freqMaxSpectre = 16000;

    _seuilConfondus = TYSEUILCONFONDUS;

    // Le solveur par defaut est le solveur standard de TYMPAN
    _solverId = OGenID("{A98B320C-44C4-47a9-B689-1DD352DAA8B2}");

    _typeSaisieMeteo = 0;

    _nbThread = 4;

#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "TypeSaisieMeteo"))
    {
        _typeSaisieMeteo = TYPreferenceManager::getInt(TYDIRPREFERENCEMANAGER, "TypeSaisieMeteo");
    }
    else
    {
        TYPreferenceManager::setInt(TYDIRPREFERENCEMANAGER, "TypeSaisieMeteo", _typeSaisieMeteo);
    }

    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "UseReflexionCalculDefault"))
    {
        _useReflexion = TYPreferenceManager::getBool(TYDIRPREFERENCEMANAGER, "UseReflexionCalculDefault");
    }
    else
    {
        TYPreferenceManager::setBool(TYDIRPREFERENCEMANAGER, "UseReflexionCalculDefault", _useReflexion);
    }

    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "UseEcranCalculDefault"))
    {
        _useEcran = TYPreferenceManager::getBool(TYDIRPREFERENCEMANAGER, "UseEcranCalculDefault");
    }
    else
    {
        TYPreferenceManager::setBool(TYDIRPREFERENCEMANAGER, "UseEcranCalculDefault", _useEcran);
    }

    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "UseVegetationCalculDefault"))
    {
        _useVegetation = TYPreferenceManager::getBool(TYDIRPREFERENCEMANAGER, "UseVegetationCalculDefault");
    }
    else
    {
        TYPreferenceManager::setBool(TYDIRPREFERENCEMANAGER, "UseVegetationCalculDefault", _useVegetation);
    }

    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "AtmosphereCalculDefault"))
    {
        _useAtmosphere = TYPreferenceManager::getBool(TYDIRPREFERENCEMANAGER, "AtmosphereCalculDefault");
    }
    else
    {
        TYPreferenceManager::setBool(TYDIRPREFERENCEMANAGER, "AtmosphereCalculDefault", _useAtmosphere);
    }


    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "CondFavCalculDefault"))
    {
        _condFav = TYPreferenceManager::getBool(TYDIRPREFERENCEMANAGER, "CondFavCalculDefault");
    }
    else
    {
        TYPreferenceManager::setBool(TYDIRPREFERENCEMANAGER, "CondFavCalculDefault", _condFav);
    }

    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "UseSolCalculDefault"))
    {
        _useSol = TYPreferenceManager::getBool(TYDIRPREFERENCEMANAGER, "UseSolCalculDefault");
    }
    else
    {
        TYPreferenceManager::setBool(TYDIRPREFERENCEMANAGER, "UseSolCalculDefault", _useSol);
    }

    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "InterferenceCalculDefault"))
    {
        _interference = TYPreferenceManager::getBool(TYDIRPREFERENCEMANAGER, "InterferenceCalculDefault");
    }
    else
    {
        TYPreferenceManager::setBool(TYDIRPREFERENCEMANAGER, "InterferenceCalculDefault", _interference);
    }

    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "ParamHCalculDefault"))
    {
        _h1 = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "ParamHCalculDefault");
    }
    else
    {
        TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "ParamHCalculDefault", _h1);
    }

    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "DistanceSRMinCalculDefault"))
    {
        _distanceSRMin = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "DistanceSRMinCalculDefault");
    }
    else
    {
        TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "DistanceSRMinCalculDefault", _distanceSRMin);
    }

    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "ExpansGeo2PiCalculDefault"))
    {
        if (TYPreferenceManager::getBool(TYDIRPREFERENCEMANAGER, "ExpansGeo2PiCalculDefault"))
        {
            _expansGeo = 1;
        }
        else
        {
            _expansGeo = 0;
        }
    }
    else
    {
        if (_expansGeo == 1)
        {
            TYPreferenceManager::setBool(TYDIRPREFERENCEMANAGER, "ExpansGeo2PiCalculDefault", true);
        }
        else
        {
            TYPreferenceManager::setBool(TYDIRPREFERENCEMANAGER, "ExpansGeo2PiCalculDefault", false);
        }
    }

    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "GeomPrecision"))
    {
        _geomPrecision = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "GeomPrecision");
    }
    else
    {
        TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "GeomPrecision", _geomPrecision);
    }

    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "SeuilConfondus"))
    {
        _seuilConfondus = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "SeuilConfondus");
    }
    else
    {
        TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "SeuilConfondus", _seuilConfondus);
    }


    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "NbThread"))
    {
        _nbThread = static_cast<unsigned int>(TYPreferenceManager::getInt(TYDIRPREFERENCEMANAGER, "NbThread"));
    }
    else
    {
        TYPreferenceManager::setUInt(TYDIRPREFERENCEMANAGER, "NbThread", static_cast<unsigned long>(_nbThread));
    }
#endif
    setUseAtmosphere(_useAtmosphere);

    _pParent = pParent;
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
    if (this != &other)
    {
        TYElement::operator =(other);
        _numero = other._numero;
        _auteur = other._auteur;
        _dateCreation = other._dateCreation;
        _dateModif = other._dateModif;
        _comment = other._comment;
        _expansGeo = other._expansGeo;
        _upTodate = other._upTodate;
        _useSol = other._useSol;
        _condFav = other._condFav;
        _useVegetation = other._useVegetation;
        _useAtmosphere = other._useAtmosphere;
        _useEcran = other._useEcran;
        _useReflexion = other._useReflexion;
        _interference = other._interference;
        _state = other._state;
        _h1 = other._h1;
        _distanceSRMin = other._distanceSRMin;
        _freqMinSpectre = other._freqMinSpectre;
        _freqMaxSpectre = other._freqMaxSpectre;
        _pAtmosphere = other._pAtmosphere;
        _maillages = other._maillages;
        _pResultat = other._pResultat;
        _elementSelection = other._elementSelection;
        _emitAcVolNode = other._emitAcVolNode;
        _mapElementRegime = other._mapElementRegime;
        _anechoic = other._anechoic;
        _solverId = other._solverId;
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
        if (_expansGeo != other._expansGeo) { return false; }
        if (_upTodate != other._upTodate) { return false; }
        if (_useSol != other._useSol) { return false; }
        if (_condFav != other._condFav) { return false; }
        if (_useVegetation != other._useVegetation) { return false; }
        if (_useAtmosphere != other._useAtmosphere) { return false; }
        if (_useEcran != other._useEcran) { return false; }
        if (_useReflexion != other._useReflexion) { return false; }
        if (_interference != other._interference) { return false; }
        if (_state != other._state) { return false; }
        if (_h1 != other._h1) { return false; }
        if (_distanceSRMin != other._distanceSRMin) { return false; }
        if (_freqMinSpectre != other._freqMinSpectre) { return false; }
        if (_freqMaxSpectre != other._freqMaxSpectre) { return false; }
        if (_pAtmosphere != other._pAtmosphere) { return false; }
        if (_maillages != other._maillages) { return false; }
        if (_pResultat != other._pResultat) { return false; }
        if (_emitAcVolNode != other._emitAcVolNode) { return false; }
        if (_mapElementRegime != other._mapElementRegime) { return false; }
        if (_elementSelection != other._elementSelection) { return false; }
        if (_anechoic != other._anechoic) { return false; }
        if (_solverId != other._solverId) { return false; }
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
    _expansGeo = pOtherCalcul->_expansGeo;
    _upTodate = pOtherCalcul->_upTodate;
    _useSol = pOtherCalcul->_useSol;
    _condFav = pOtherCalcul->_condFav;
    _useVegetation = pOtherCalcul->_useVegetation;
    _useAtmosphere = pOtherCalcul->_useAtmosphere;
    _useEcran = pOtherCalcul->_useEcran;
    _useReflexion = pOtherCalcul->_useReflexion;
    _interference = pOtherCalcul->_interference;
    _state = pOtherCalcul->_state;
    _h1 = pOtherCalcul->_h1;
    _distanceSRMin = pOtherCalcul->_distanceSRMin;
    _freqMinSpectre = pOtherCalcul->_freqMinSpectre;
    _freqMaxSpectre = pOtherCalcul->_freqMaxSpectre;
    _anechoic = pOtherCalcul->_anechoic;

    _pAtmosphere->deepCopy(pOtherCalcul->_pAtmosphere, copyId);
    _pResultat->deepCopy(pOtherCalcul->_pResultat, copyId);

    _elementSelection = pOtherCalcul->_elementSelection;
    _emitAcVolNode = pOtherCalcul->_emitAcVolNode;
    _mapElementRegime = pOtherCalcul->_mapElementRegime;
    _solverId = pOtherCalcul->_solverId;

    unsigned int i;

    for (i = 0; i < pOtherCalcul->_maillages.size(); i++)
    {
        // Dupplication du maillage
        LPTYMaillageGeoNode pMaillageGeoNode = new TYMaillageGeoNode(NULL, this);
        pMaillageGeoNode->deepCopy(pOtherCalcul->_maillages[i], copyId);
        // Duplication des etats des points de calcul
        TYMaillage* pMaillage = TYMaillage::safeDownCast(pMaillageGeoNode->getElement());
        pMaillage->duplicatePtCalcState(pOtherCalcul, this);
        // Ajout du maillage
        addMaillage(pMaillageGeoNode);
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
    TYXMLTools::addElementIntValue(domNewElem, "expansGeo", _expansGeo);
    TYXMLTools::addElementIntValue(domNewElem, "useSol", _useSol);
    TYXMLTools::addElementIntValue(domNewElem, "typeCalculSol", _typeCalculSol);
    TYXMLTools::addElementIntValue(domNewElem, "condFav", _condFav);
    TYXMLTools::addElementIntValue(domNewElem, "useVegetation", _useVegetation);
    TYXMLTools::addElementIntValue(domNewElem, "useAtmosphere", _useAtmosphere);
    TYXMLTools::addElementIntValue(domNewElem, "useEcran", _useEcran);
    TYXMLTools::addElementIntValue(domNewElem, "calculTrajetHorizontaux", _bCalculTrajetsHorizontaux);
    TYXMLTools::addElementIntValue(domNewElem, "useReflexion", _useReflexion);
    TYXMLTools::addElementIntValue(domNewElem, "interference", _interference);
    TYXMLTools::addElementDoubleValue(domNewElem, "h1", _h1);
    TYXMLTools::addElementDoubleValue(domNewElem, "distanceSRMin", _distanceSRMin);
    TYXMLTools::addElementDoubleValue(domNewElem, "freqMinSpectre", _freqMinSpectre);
    TYXMLTools::addElementDoubleValue(domNewElem, "freqMaxSpectre", _freqMaxSpectre);

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

    // Atmosphere
    _pAtmosphere->toXML(domNewElem);

    // Maillages
    unsigned int i;
    for (i = 0; i < _maillages.size(); i++)
    {
        _maillages[i]->toXML(domNewElem);
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


    return domNewElem;
}

int TYCalcul::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);

    purge();

    bool getOk[21];
    unsigned int i;
    for (i = 0; i < 21; i++) { getOk[i] = false; }
    int retVal = -1;
    LPTYMaillageGeoNode pMaillageGeoNode = new TYMaillageGeoNode(NULL, this);

    // CLM-NT33: Compatibilti¿½ ancien format XML
    LPTYPointControl pPointControl = new TYPointControl();
    TYProjet* pProjet = getProjet();

    int etat = -1; // Etat du calcul

    std::string strIDSiteCalcul;
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
        TYXMLTools::getElementStringValue(elemCur, "solverId", strSolverId, getOk[20]);
        TYXMLTools::getElementIntValue(elemCur, "etat", etat, getOk[19]);
        TYXMLTools::getElementIntValue(elemCur, "expansGeo", _expansGeo, getOk[5]);
        TYXMLTools::getElementBoolValue(elemCur, "useSol", _useSol, getOk[6]);
        TYXMLTools::getElementIntValue(elemCur, "typeCalculSol", _typeCalculSol, getOk[17]);
        TYXMLTools::getElementBoolValue(elemCur, "condFav", _condFav, getOk[7]);
        TYXMLTools::getElementBoolValue(elemCur, "useVegetation", _useVegetation, getOk[8]);
        TYXMLTools::getElementBoolValue(elemCur, "useAtmosphere", _useAtmosphere, getOk[9]);
        TYXMLTools::getElementBoolValue(elemCur, "useEcran", _useEcran, getOk[10]);
        TYXMLTools::getElementBoolValue(elemCur, "calculTrajetHorizontaux", _bCalculTrajetsHorizontaux, getOk[18]);
        TYXMLTools::getElementBoolValue(elemCur, "useReflexion", _useReflexion, getOk[11]);
        TYXMLTools::getElementBoolValue(elemCur, "interference", _interference, getOk[12]);
        TYXMLTools::getElementDoubleValue(elemCur, "h1", _h1, getOk[13]);
        TYXMLTools::getElementDoubleValue(elemCur, "distanceSRMin", _distanceSRMin, getOk[14]);
        TYXMLTools::getElementFloatValue(elemCur, "freqMinSpectre", _freqMinSpectre, getOk[15]);
        TYXMLTools::getElementFloatValue(elemCur, "freqMaxSpectre", _freqMaxSpectre, getOk[16]);


        if (!getOk[17]) { _typeCalculSol = 0; } // Par defaut, calcul avec sol local

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
        else if (elemCur.nodeName() == "ReflexionAccVolNodes") // Etats des AccVolNodes en reflexion
        {
        }
        else if (elemCur.nodeName() == "EtatAtts") // Etats des attenuateurs
        {
        }

        // Atmosphere
        _pAtmosphere->callFromXMLIfEqual(elemCur);

        // CLM-NT33: Compatibiliti¿½ avec ancien format XML
        // Points de contri¿½le
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

        // Maillages
        if (pMaillageGeoNode->callFromXMLIfEqual(elemCur, &retVal))
        {
            if (retVal == 1)
            {
                addMaillage(pMaillageGeoNode);
                pMaillageGeoNode = new TYMaillageGeoNode(NULL, this);
            }
        }

        // Resultat
        _pResultat->callFromXMLIfEqual(elemCur, &readOk);
    }

    if (getOk[20]) { _solverId.FromString(strSolverId); } // Recuperation de l'Id du solveur

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
            if (pElement->inherits("TYAcousticVolumeNode"))
            {
                regime = TYAcousticVolumeNode::safeDownCast(pElement)->getCurRegime();
                bEmit = TYAcousticVolumeNode::safeDownCast(pElement)->getIsRayonnant();
            }
            else if (pElement->inherits("TYAcousticLine"))
            {
                regime = TYAcousticLine::safeDownCast(pElement)->getCurRegime();
                bEmit = TYAcousticLine::safeDownCast(pElement)->getIsRayonnant();
            }
            else if (pElement->isA("TYUserSourcePonctuelle"))
            {
                regime = TYUserSourcePonctuelle::safeDownCast(pElement)->getCurrentRegime();
                bEmit = true;
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
    remAllMaillage();

    clearSelection();

    _pResultat->purge();
    _mapElementRegime.clear();
    _emitAcVolNode.clear();

    setIsGeometryModified(true);
}

void TYCalcul::clearResult()
{
    TYTabLPPointControl::iterator ite;
    for (ite = getProjet()->getPointsControl().begin(); ite != getProjet()->getPointsControl().end(); ite++)
    {
        (*ite)->purge(this);
#if TY_USE_IHM
        (*ite)->updateGraphic();
#endif
    }

    TYTabMaillageGeoNode::iterator ite2;
    for (ite2 = _maillages.begin(); ite2 != _maillages.end(); ite2++)
    {
        TYMaillage::safeDownCast((*ite2)->getElement())->clearResult();
#if TY_USE_IHM
        TYMaillage::safeDownCast((*ite2)->getElement())->updateGraphic();
#endif

    }

    updateAltiRecepteurs();

    _pResultat->purge();

    _mapElementRegime.clear();
    _emitAcVolNode.clear();

    setIsGeometryModified(true);
}

void TYCalcul::setFreqMinFreq(float freq)
{
    _freqMinSpectre = freq;
    TYSpectre::setFMin(freq);
}

void TYCalcul::setFreqMaxFreq(float freq)
{
    _freqMaxSpectre = freq;
    TYSpectre::setFMax(freq);
}

void TYCalcul::setElementSelection(TYListID selection)
{
    _elementSelection = selection;
    setIsGeometryModified(true);
}

void TYCalcul::clearSelection()
{
    _elementSelection.clear();
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
        else if (pElt->inherits("TYSiteNode"))
        {
            // Si un objet est ajoute son parent l'est forcemment
            addToSelection(pElt->getParent(), false);

            // Si c'est un site on ajoute pas systi¿½matiquement tous les enfants
            recursif = false;
        }
        else if (pElt->inherits("TYAcousticVolumeNode"))
        {
            TYAcousticVolumeNode* pVolNode = TYAcousticVolumeNode::safeDownCast(pElt);
            if (pVolNode) { etat = pVolNode->getIsRayonnant(); }
            _emitAcVolNode[pElt] = etat;
            _mapElementRegime[pElt] = 0;

            // Si un objet est ajoute son parent l'est forcemment
            addToSelection(pElt->getParent(), false);

        }
        else if (pElt->inherits("TYAcousticLine"))
        {
            TYAcousticLine* pLine = TYAcousticLine::safeDownCast(pElt);
            if (pLine) { etat = pLine->getIsRayonnant(); }
            _emitAcVolNode[pElt] = etat;
            _mapElementRegime[pElt] = 0;

            // Si un objet est ajoute son parent l'est forcemment
            addToSelection(pElt->getParent(), false);
        }
        else if (pElt->inherits("TYSourcePonctuelle"))
        {
            TYUserSourcePonctuelle* pSource = TYUserSourcePonctuelle::safeDownCast(pElt);
            if (pSource) { etat = pSource->getIsRayonnant(); }
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
            TYElementCollection childs;
            pElt->getChilds(childs, false);

            for (int i = 0; i < childs.getCount(); i++)
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

        //  On di¿½sactive ses enfants
        TYElementCollection childs;
        pElt->getChilds(childs, false);

        for (int i = 0; i < childs.getCount(); i++)
        {
            remToSelection(childs[i], recursif);
        }

        pElt->setIsGeometryModified(true);

        ret = true;
    }

    // Si c'est un siteNode, on fait une mise i¿½ jour de l'altimi¿½trie
    if (pElt->inherits("TYSiteNode"))
    {
        // Dans ce cas, on remonte au site racine et on fait l'update de tout
        TYSiteNode* pSite = TYSiteNode::safeDownCast(pElt);
        pSite->getProjet()->getSite()->update(true);
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

bool TYCalcul::addMaillage(LPTYMaillageGeoNode pMaillageGeoNode)
{
    assert(pMaillageGeoNode);
    assert(pMaillageGeoNode->getElement());

    pMaillageGeoNode->setParent(this);
    pMaillageGeoNode->getElement()->setParent(this);

    _maillages.push_back(pMaillageGeoNode);

    setIsGeometryModified(true);

    return true;
}

bool TYCalcul::addMaillage(LPTYMaillage pMaillage)
{
    return addMaillage(new TYMaillageGeoNode((LPTYElement)pMaillage));
}

bool TYCalcul::remMaillage(const LPTYMaillage pMaillage)
{
    assert(pMaillage);
    bool ret = false;
    TYTabMaillageGeoNode::iterator ite;

    for (ite = _maillages.begin(); ite != _maillages.end(); ite++)
    {
        if (TYMaillage::safeDownCast((*ite)->getElement()) == pMaillage)
        {
            _maillages.erase(ite);
            ret = true;
            setIsGeometryModified(true);
            break;
        }
    }

    return ret;
}

bool TYCalcul::remMaillage(const LPTYMaillageGeoNode pMaillageGeoNode)
{
    assert(pMaillageGeoNode);
    bool ret = false;
    TYTabMaillageGeoNode::iterator ite;

    for (ite = _maillages.begin(); ite != _maillages.end(); ite++)
    {
        if ((*ite) == pMaillageGeoNode)
        {
            _maillages.erase(ite);
            ret = true;
            setIsGeometryModified(true);
            break;
        }
    }

    return ret;
}

bool TYCalcul::remMaillage(QString idMaillage)
{
    bool ret = false;
    TYTabMaillageGeoNode::iterator ite;

    for (ite = _maillages.begin(); ite != _maillages.end(); ite++)
    {
        if ((*ite)->getElement()->getID().toString() == idMaillage)
        {
            _maillages.erase(ite);
            ret = true;
            setIsGeometryModified(true);
            break;
        }
    }

    return ret;
}

bool TYCalcul::remAllMaillage()
{
    _maillages.clear();
    setIsGeometryModified(true);

    return true;
}

LPTYMaillageGeoNode TYCalcul::findMaillage(const LPTYMaillage pMaillage)
{
    assert(pMaillage);
    TYTabMaillageGeoNode::iterator ite;

    for (ite = _maillages.begin(); ite != _maillages.end(); ite++)
    {
        if (TYMaillage::safeDownCast((*ite)->getElement()) == pMaillage)
        {
            return (*ite);
        }
    }

    return NULL;
}

void TYCalcul::updateGraphicMaillage()
{
    TYMaillage* pMaillage = NULL;

    for (unsigned int i = 0; i < getMaillages().size(); i++)
    {
        pMaillage = getMaillage(i);
#if TY_USE_IHM
        pMaillage->getGraphicObject()->update();
#endif
    }
}

bool TYCalcul::updateAltiMaillage(TYMaillageGeoNode* pMaillageGeoNode)
{
    TYAltimetrie* pAlti = getProjet()->getSite()->getTopographie()->getAltimetrie()._pObj;
    return updateAltiMaillage(pMaillageGeoNode, pAlti);
}

bool TYCalcul::updateAltiMaillage(TYMaillageGeoNode* pMaillageGeoNode, const TYAltimetrie* pAlti)
{
    assert(pMaillageGeoNode);
    assert(pAlti);

    bool modified = true;

    if (pMaillageGeoNode == NULL) { return false; }

    TYPoint pt;
    TYMaillage* pMaillage = TYMaillage::safeDownCast(pMaillageGeoNode->getElement());

    if (pMaillage == NULL) { return false; }

    OMatrix matrix = pMaillageGeoNode->getMatrix();
    OMatrix matrixinv = matrix.getInvert();
    TYTabLPPointCalcul& tabpoint = pMaillage->getPtsCalcul();

    bool cancel = false;
    bool bNoPbAlti = true; // Permet de tester si tous les points sont altimétrisés correctement.

    if (pMaillage->getComputeAlti()) // Cas des maillages rectangulaires et lineaires horizontaux
    {

#if TY_USE_IHM
        TYProgressManager::setMessage("Calcul des altitudes des points de maillage");
        TYProgressManager::set(static_cast<uint32>(tabpoint.size()));
#endif // TY_USE_IHM

        for (unsigned int i = 0; i < tabpoint.size(); i++)
        {

#if TY_USE_IHM
            TYProgressManager::step(cancel);
            if (cancel) { break; }
#endif // TY_USE_IHM

            pt = *tabpoint[i];
            // Passage au repere du site
            pt = matrix * pt;
            pt._z = 0;

            // Recherche de l'altitude
            bNoPbAlti &= pAlti->updateAltitude(pt);

            // Retour au repere d'origine
            pt = matrixinv * pt;

            // Ajout de l'offset en Z
            pt._z += pMaillage->getHauteur();

            // Application du calcul
            tabpoint[i]->_x = pt._x;
            tabpoint[i]->_y = pt._y;
            tabpoint[i]->_z = pt._z;
        }

#if TY_USE_IHM
        TYProgressManager::stepToEnd();
#endif // TY_USE_IHM

    }
    else // Cas des maillages verticaux
    {
        // Init
        pt = pMaillageGeoNode->getRepere()._origin;
        pt._z = 0.0;

        // Recherche de l'altitude
        bNoPbAlti &= pAlti->updateAltitude(pt);

        // Ajout de l'offset en Z
        pt._z += pMaillage->getHauteur();

        if (pMaillage->isA("TYRectangularMaillage"))
        {
            // Ajout d'un offset de la demi-hauteur (maillages verticaux)
            pt._z += ((TYRectangularMaillage*) pMaillage)->getRectangle()->getSizeY() / 2.0;
        }

        ORepere3D repere = pMaillageGeoNode->getRepere();
        // On assigne la nouvelle altitude
        repere._origin._z = pt._z;
        pMaillageGeoNode->setRepere(repere);

        modified = true;
    }

    if (!bNoPbAlti) // Certains point pas altimetrises
    {
        OMessageManager::get()->info(TR("msg_pbalti"));
    }


    // Done
    if (modified)
    {
#if TY_USE_IHM
        pMaillage->updateGraphicTree();
#endif
        setIsGeometryModified(true);
        _pParent->setIsGeometryModified(true);
    }

    return modified;
}

bool TYCalcul::updateAltiRecepteurs()
{
    TYAltimetrie* pAlti = getProjet()->getSite()->getTopographie()->getAltimetrie();
    return updateAltiRecepteurs(pAlti);
}

bool TYCalcul::updateAltiRecepteurs(const TYAltimetrie* pAlti)
{
    assert(pAlti);
    bool modified = false;

    // Mise a jour de l'altitude pour les points des maillages
    unsigned int i;
    for (i = 0; i < getMaillages().size(); i++)
    {
        modified = updateAltiMaillage(getMaillages()[i], pAlti);
    }

    // Done
    if (modified)
    {
        setIsGeometryModified(true);
        _pParent->setIsGeometryModified(true);
    }

    return modified;
}
//*/
void TYCalcul::selectActivePoint(const LPTYSiteNode pSite)
{
    int i, j;


    // Recuperation des volumes
    TYInfrastructure* pInfra = pSite->getInfrastructure();
    TYTabAcousticVolumeNodeGeoNode tabVolNodeGeoNode;

    // On commence par recuperer la liste des machines et des bi¿½timents
    // Bi¿½timents
    for (j = 0; j < static_cast<int>(pInfra->getListBatiment().size()); j++)
    {
        // Si ce bi¿½timent est actif pour le calcul
        if (TYBatiment::safeDownCast(pInfra->getBatiment(j)->getElement())->isInCurrentCalcul())
        {
            tabVolNodeGeoNode.push_back((LPTYAcousticVolumeNodeGeoNode&) pInfra->getListBatiment()[j]);
        }
    }

    // Machines
    for (j = 0; j < static_cast<int>(pInfra->getListMachine().size()); j++)
    {
        // Si cette machine est active pour le calcul
        if (TYMachine::safeDownCast(pInfra->getMachine(j)->getElement())->isInCurrentCalcul())
        {
            tabVolNodeGeoNode.push_back((LPTYAcousticVolumeNodeGeoNode&) pInfra->getListMachine()[j]);
        }
    }

    // Ensuite, on verifie si des points du maillage ne sont pas a l'interieur
    vector <OPoint3D> tabPts;

    for (i = 0; i < static_cast<int>(getMaillages().size()); i++)
    {
        TYMaillage* pMaillage = getMaillage(i);
        TYMaillageGeoNode* pMaillageGeoNode = getMaillages()[i];

        // Matrice pour la position de ce maillage
        OMatrix matrixMaillage = pMaillageGeoNode->getMatrix();
        // Nb de points de calcul dans ce maillage
        size_t nbPtsCalcul = pMaillage->getPtsCalcul().size();

        tabPts.reserve(nbPtsCalcul); // Reservation du nombre de points pour eviter les mouvements memoire

        for (j = 0; (int)j < nbPtsCalcul; j++)
        {
            pMaillage->getPtsCalcul()[j]->setEtat(true, this);
            tabPts.push_back(matrixMaillage * (*(pMaillage->getPtsCalcul()[j])));
        }

        // Detection des points a l'interieur de volumes
        OPoint3D pt;
        for (j = 0; j < tabVolNodeGeoNode.size(); j++)
        {
            OMatrix matrixVolNode = tabVolNodeGeoNode[j]->getMatrix();
            matrixVolNode.invert();

            LPTYAcousticVolumeNode pVolumeNode = TYAcousticVolumeNode::safeDownCast(tabVolNodeGeoNode[j]->getElement());
            for (int k = 0; k < nbPtsCalcul; k++)
            {
                // Passage au repere du volume
                pt = matrixVolNode * tabPts[k];

                // Si le point se trouve a l'interieur de ce volume
                if (pVolumeNode->isInside(pt))
                {
                    // Desactivation de ce point de calcul
                    pMaillage->getPtsCalcul()[k]->setEtat(false, this);
                }
            }

        }

        // Le maillage et son GeoNode sont maintenant up to date
        pMaillage->setIsGeometryModified(false);
        pMaillageGeoNode->setIsGeometryModified(false);

        // Vidage du tableau de points
        tabPts.clear();
    }

    tabVolNodeGeoNode.clear();
}

void TYCalcul::buildValidTrajects(const TYTabSourcePonctuelleGeoNode& sources, TYTabPointCalculGeoNode& recepteurs)
{
    unsigned int i;
    // Suppression des points trop proches des sources
    TYTabPointCalculGeoNode::iterator ite;

    _tabTrajets.reserve(sources.size()*recepteurs.size());
    TYTrajet trajet;
    double distance = 0.0;

    TYSourcePonctuelle* pSource = NULL;
    TYPointCalcul* pRecepteur = NULL;

    for (i = 0 ; i < sources.size(); i++)
    {
        pSource = TYSourcePonctuelle::safeDownCast(sources[i]->getElement());

        // Position de la source dans le repere du site
        OPoint3D source = sources[i]->getMatrix() * (*(pSource->getPos()));

        for (ite = recepteurs.begin(); ite != recepteurs.end(); ite++)
        {
            pRecepteur = TYPointCalcul::safeDownCast((*ite)->getElement());

            // Position du recepteur dans le repere du site
            OPoint3D recepteur = (*ite)->getMatrix() * (*pRecepteur);
            distance = recepteur.distFrom(source);

            if (distance < _distanceSRMin)
            {
                // Inactivation du recepteur
                pRecepteur->setEtat(false, this);
            }
            else
            {
                // On remplit la liste des trajets
                trajet.setSourcePonctuelle(sources[i]);
                trajet.setPointCalcul((*ite));
                trajet.setPtSetPtR(source, recepteur);
                trajet.setDistance(distance);
                _tabTrajets.push_back(trajet);
            }
        }
    }
}

void TYCalcul::getAllRecepteurs(TYTabPointCalculGeoNode& tabRecepteur)
{
    unsigned int i, j;
    tabRecepteur.clear();
    TYPointCalculGeoNode* pPointCalculNode = NULL;

    // D'abord les points isoles
    TYTabLPPointControl tabPointsControl = getProjet()->getPointsControl();
    for (i = 0; i < tabPointsControl.size(); i++)
    {
        if (tabPointsControl[i]->getEtat(this)) // On ne prend que les points actifs
        {
            pPointCalculNode = new TYPointCalculGeoNode((LPTYElement&) tabPointsControl[i]);
            if (!pPointCalculNode) { continue; }
            tabRecepteur.push_back(pPointCalculNode);
        }
    }

    // Ensuite les points des maillages
    for (i = 0 ; i < _maillages.size(); i++)
    {
        if (getMaillage(i)->getState() != TYMaillage::Actif) { continue; }

        TYTabLPPointCalcul tabPointCalcul = getMaillage(i)->getPtsCalcul();
        OMatrix matrix = _maillages[i]->getMatrix();

        // Changement de repere
        for (j = 0; j < tabPointCalcul.size(); j++)
        {
            if (tabPointCalcul[j]->getEtat(this))// On ne prend que les points actifs
            {
                pPointCalculNode = new TYPointCalculGeoNode((LPTYElement)tabPointCalcul[j], matrix);
                if (!pPointCalculNode) { continue; }
                tabRecepteur.push_back(pPointCalculNode);
            }
        }
    }

}

void TYCalcul::getAllSources(TYMapElementTabSources& mapElementSources, TYTabSourcePonctuelleGeoNode& tabSources)
{
    TYMapElementTabSources::iterator it;

    for (it = mapElementSources.begin(); it != mapElementSources.end(); it++)
    {
        TYTabSourcePonctuelleGeoNode tabSrc = (*it).second;

        for (unsigned int i = 0 ; i < tabSrc.size() ; i++)
        {
            if (!tabSrc[i]) { continue; } // Au cas ou une source ne serait pas definie
            tabSources.push_back(tabSrc[i]);
        }
    }
}

bool TYCalcul::isCalculPossible(const int& nbSources, const int& nbRecepteurs, const LPTYSiteNode pMergeSite)
{
    OMessageManager::get()->info("Nombre de sources : %d", nbSources);
    OMessageManager::get()->info("Nombre de recepteurs : %d", nbRecepteurs);
    OMessageManager::get()->info("Nombre de trajets potentiels : %d", nbSources * nbRecepteurs);

    if (nbSources == 0)
    {
        OMessageManager::get()->info("Pas de source");

        // On indique quels elements (s'il y en a), n'ont pas pu  etre mis a jour
        vector<LPTYElement> tabElemNOk = pMergeSite->getInfrastructure()->getTabElemNOk();
        if (tabElemNOk.size() != 0)
        {
            OMessageManager::get()->info("\n Les elements suivants ont echoue dans leur mise a jour :");
            for (unsigned int i = 0 ; i < tabElemNOk.size() ; i++)
            {
                OMessageManager::get()->info(tabElemNOk[i]->getName().toAscii().data());
            }
            OMessageManager::get()->info("\n\n");
        }

        return false;
    }
    else if (nbSources > MAX_SOURCES)
    {
        OMessageManager::get()->info("Le nombre de sources est trop grand. Le maximum est de %d.", MAX_SOURCES);
    }

    if (nbRecepteurs == 0)
    {
        OMessageManager::get()->info("Pas de recepteur");
        return false;
    }
    else if (nbRecepteurs > MAX_RECEPTEURS)
    {
        OMessageManager::get()->info("Le nombre de recepteurs est trop grand. Le maximum est de %d.", MAX_RECEPTEURS);
        return false;
    }

    return true;
}

bool TYCalcul::go()
{
    TYProjet* pProjet = getProjet();

    if (!pProjet)
    {
        return false;
    }

    if (_state == TYCalcul::Locked)
    {
        OMessageManager::get()->info("+++ UN RESULTAT MESURE NE PEUX FAIRE L'OBJET D'UN CALCUL +++");
        return true; // Si le calcul est bloque, il ne peut etre execute
    }

    bool ret = true;
    bool cancel = false;
    unsigned int i = 0;

    // Reset des resultats precedents
    _pResultat->purge();

    // Nettoyage du tableau des trajets
    _tabTrajets.clear();

    // Nettoyage du tableau de rayon
    _tabRays.clear();
    _tabEdges.clear();

    // Regroupement des sites et mise a jour de l'altimetrie et des sols
    // Creation de la liste des recepteurs
    OMessageManager::get()->info("Mise a jour du Site");

    // Fusion des sites
    LPTYSiteNode pMergeSite = pProjet->getSite()->merge();
    pMergeSite->update(true);
    pMergeSite->getTopographie()->sortTerrainsBySurface();
    pMergeSite->updateAcoustique(true);

    // Actualisation de l'altimetrie des recepteurs (securite)
    pProjet->updateAltiRecepteurs(pMergeSite->getTopographie()->getAltimetrie());

    pMergeSite->setAtmosphere(getAtmosphere());

    TYNameManager::get()->enable(false);

    // #define EXPORT_MERGED_SITE
#ifdef EXPORT_MERGED_SITE

    QString docName = "Tympan";
    QString version = "3.7";

    TYXMLManager xmlManager;

    xmlManager.createDoc(docName, version);
    xmlManager.addElement(pMergeSite);

    if (xmlManager.save("merged.xml") == 0)
    {
        bool bRet = true;
    }
    else
    {
        bool bRet = false;
    }

#endif

    OMessageManager::get()->info("Recuperation de la liste des sources");
    TYMapElementTabSources& mapElementSources = _pResultat->getMapEmetteurSrcs();
    pMergeSite->getInfrastructure()->getAllSrcs(this, mapElementSources);

    TYTabSourcePonctuelleGeoNode sources; // Creation d'une collection des sources
    getAllSources(mapElementSources, sources);

    OMessageManager::get()->info("Selection des points de reception actifs");

    selectActivePoint(pMergeSite); // Supprime les points a l'interieur des volumes
    TYTabPointCalculGeoNode recepteurs; // Creation d'une collection de recepteurs
    getAllRecepteurs(recepteurs);

    OMessageManager::get()->info("Selection des trajets actifs");
    buildValidTrajects(sources, recepteurs); // Supprime les points trop proches d'une sources

    if (isCalculPossible(static_cast<int>(sources.size()), static_cast<int>(recepteurs.size()), pMergeSite))
    {
        OMessageManager::get()->info("Calcul en cours...");

        pluginInfos* pInfos = new pluginInfos();
        TYPluginManager::get()->getInfos(pInfos, getSolverId());
        OMessageManager::get()->info("***************************************************************");
        OMessageManager::get()->info("                          APPEL DE LA DLL");
        OMessageManager::get()->info("");
        OMessageManager::get()->info("Nom         : %s", pInfos->_name.toAscii().data());
        OMessageManager::get()->info("Version     : %s", pInfos->_version.toAscii().data());
        OMessageManager::get()->info("Description : %s", pInfos->_description.toAscii().data());
        OMessageManager::get()->info("Auteur      : %s", pInfos->_author.toAscii().data());
        OMessageManager::get()->info("");
        OMessageManager::get()->info("***************************************************************");
        delete pInfos;
        pInfos = NULL;

        TYSolverInterface* pSolver = TYPluginManager::get()->getSolver(getSolverId());

        ret = pSolver->solve(*pMergeSite, *this);

        pSolver->purge();

        // Cumul de la pression aux differents points de calcul
        if (ret) // Si l'etape precedente s'est mal passee, inutile de continuer
        {
            // Puisque le calcul est OK on va construire la matrice resultat
            _pResultat->buildSources(sources);
            _pResultat->buildRecepteurs(recepteurs);
            _pResultat->buildMatrix();

            ret = _pResultat->cumulSpectres(_tabTrajets);

            // Suppression des points de maillage de la matrice
            for (unsigned int i = 0; i < recepteurs.size(); i++)
            {
                if (recepteurs[i]->getElement()->getParent()->inherits("TYMaillage"))
                {
                    _pResultat->remSpectres(static_cast<TYPointCalcul*>(recepteurs[i]->getElement()));
                }
            }
        }
    }
    else
    {
        ret = false;
    }

    if (ret)
    {
        OMessageManager::get()->info("Finalisation des resultats (post-traitement)...");
        // Condensation des sources sur les emetteurs
        _pResultat->condensate();

        updateGraphicMaillage();
    }
    else
    {
        _pResultat->purge();
    }

    // Le calcul a proprement parler est termine
    // Il est necessaire de reattribuer les parents des elements du site merges
    getProjet()->getSite()->reparent();

    pMergeSite = NULL;

    sources.clear();
    recepteurs.clear();
    _tabTrajets.clear();

    TYNameManager::get()->enable(true);

    setIsAcousticModified(true); // Les donni¿½es acoustiques ont i¿½ti¿½ actualisi¿½es

    // (Re) init default number of thread (sorry this is not clean)
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "NbThread"))
    {
        _nbThread = static_cast<unsigned int>(TYPreferenceManager::getInt(TYDIRPREFERENCEMANAGER, "NbThread"));
      }
    else
      {
        TYPreferenceManager::setUInt(TYDIRPREFERENCEMANAGER, "NbThread", static_cast<unsigned long>(_nbThread));
    }

    return ret;
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

void TYCalcul::setUseAtmosphere(const bool use /*=true*/)
{
    _useAtmosphere = use;

    if (!_useAtmosphere)
    {
        TYSpectre attenuationNulle;

        attenuationNulle.setType(SPECTRE_TYPE_ATT); // Spectre d'attenuation
        attenuationNulle.setDefaultValue(1e-20);// Spectre d'attenuation nulle

        _pAtmosphere->setSpectreAtt(attenuationNulle);

        HARM_airAbsorption = 0;
    }
    else
    {
        _pAtmosphere->calculProprietes();
        HARM_airAbsorption = 4;
    }
}

void TYCalcul::setAtmosphere(const LPTYAtmosphere pAtmo)
{
    _pAtmosphere = pAtmo;
    _pAtmosphere->setParent(this);

    if (!_useAtmosphere) // Si flag "calcul sans attenuation atmospherique" alors attenuation nulle
    {
        TYSpectre attenuationNulle;

        attenuationNulle.setType(SPECTRE_TYPE_ATT); // Spectre d'attenuation
        attenuationNulle.setDefaultValue(1e-20);// Spectre d'attenuation nulle

        _pAtmosphere->setSpectreAtt(attenuationNulle);
    }
}


void TYCalcul::setState(int state)
{
    _state = state;

    unsigned int i;

    // Changement d'etat des maillages
    for (i = 0; i < _maillages.size(); i++)
    {
        LPTYMaillage pMail = TYMaillage::safeDownCast(_maillages[i]->getElement());

        if (_state == TYCalcul::Locked)
        {
            pMail->setLocked(true);;
        }
        else
        {
            pMail->setLocked(false);
        }
    }
}

void TYCalcul::addPtCtrlToResult(TYPointControl* pPoint)
{
    _pResultat->addRecepteur(pPoint);
}

void TYCalcul::remPtCtrlFromResult(TYPointControl* pPoint)
{
    _pResultat->remRecepteur(pPoint);
}
