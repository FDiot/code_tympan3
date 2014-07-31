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
#include "Tympan/models/business/TYPluginManager.h"
#include "Tympan/models/business/OLocalizator.h"
#include "TYProjet.h"

#if TY_USE_IHM
  #include "Tympan/gui/widgets/TYProjetWidget.h"
  #include "Tympan/gui/gl/TYProjetGraphic.h"
#endif



TY_EXTENSION_INST(TYProjet);
TY_EXT_GRAPHIC_INST(TYProjet);

#define TR(id) OLocalizator::getString("OMessageManager", (id))

bool TYProjet::gSaveValues = true;

TYProjet::TYProjet() :  _auteur(""),
    _dateCreation("2001-10-01"),
    _dateModif("2001-10-01"),
    _comment(""),
    _pSite(NULL),
    _pCurrentCalcul(NULL),
    _delaunayTolerance(0.0001),
    _maxDistBetweenPoints(200.0),
    _useDefaultGeomVal(true),
    _bStatusSolver(true)
{
    _name = TYNameManager::get()->generateName(getClassName());

    _pSite = new TYSiteNode();
    _pSite->setParent(this);

    _pSite->setProjet(this); // Definit le projet courant

    // On initialise le projet avec au moins un calcul
    setCurrentCalcul(new TYCalcul());

    // Ajout du site au calcul courant
    _pCurrentCalcul->addToSelection(_pSite, false);

    forceUseDefault();
}


TYProjet::TYProjet(const TYProjet& other)
{
    *this = other;
}

TYProjet::~TYProjet()
{
    remAllCalcul();
}

TYProjet& TYProjet::operator=(const TYProjet& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        _auteur = other._auteur;
        _dateCreation = other._dateCreation;
        _dateModif = other._dateModif;
        _comment = other._comment;
        _pSite = other._pSite;
        _pointsControl = other._pointsControl;
        _pCurrentCalcul = other._pCurrentCalcul;
        _listCalcul = other._listCalcul;
        _delaunayTolerance = other._delaunayTolerance;
        _maxDistBetweenPoints = other._maxDistBetweenPoints;
        _useDefaultGeomVal = other._useDefaultGeomVal;
    }

    TYCourbeNiveau::setDefaultDistMax(_maxDistBetweenPoints); // MaJ Courbes de niveau

    return *this;
}

bool TYProjet::operator==(const TYProjet& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        if (_auteur != other._auteur) { return false; }
        if (_dateCreation != other._dateCreation) { return false; }
        if (_dateModif != other._dateModif) { return false; }
        if (_comment != other._comment) { return false; }
        if (_pSite != other._pSite) { return false; }
        if (_pointsControl != other._pointsControl) { return false; }
        if (_pCurrentCalcul != other._pCurrentCalcul) { return false; }
        if (!(_listCalcul == other._listCalcul)) { return false; }
        if (_delaunayTolerance != other._delaunayTolerance) { return false; }
        if (_maxDistBetweenPoints != other._maxDistBetweenPoints) { return false; }
        if (_useDefaultGeomVal != other._useDefaultGeomVal) { return false; }

    }
    return true;
}

bool TYProjet::operator!=(const TYProjet& other) const
{
    return !operator==(other);
}

std::string TYProjet::toString() const
{
    return "TYProjet";
}

DOM_Element TYProjet::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);

    TYXMLTools::addElementStringValue(domNewElem, "auteur", _auteur);
    TYXMLTools::addElementStringValue(domNewElem, "dateCreation", _dateCreation);
    TYXMLTools::addElementStringValue(domNewElem, "dateModif", _dateModif);
    TYXMLTools::addElementDoubleValue(domNewElem, "delaunayTolerence", _delaunayTolerance);
    TYXMLTools::addElementDoubleValue(domNewElem, "distMaxBetweenPoints", _maxDistBetweenPoints);
    TYXMLTools::addElementBoolValue(domNewElem, "useDefGeomVal", _useDefaultGeomVal);

    if (_pCurrentCalcul)
    {
        TYXMLTools::addElementStringValue(domNewElem, "currentCalculID", _pCurrentCalcul->getID().toString());
    }

    // Ajout du site node sur lequel s'effectue le calcul
    DOM_Document domDoc = domElement.ownerDocument();

    // Site
    _pSite->toXML(domNewElem);

    // Points de controle
    DOM_Element pointsControlNode = domDoc.createElement("PointsControl");
    domNewElem.appendChild(pointsControlNode);
    unsigned int i;
    for (i = 0; i < _pointsControl.size(); i++)
    {
        _pointsControl[i]->toXML(pointsControlNode);
    }

    // Calculs
    for (unsigned int i = 0; i < _listCalcul.size(); i++)
    {
        _listCalcul.at(i)->toXML(domNewElem);
    }

    return domNewElem;
}

int TYProjet::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);

    _pCurrentCalcul = NULL;

    remAllCalcul();

    bool auteurOk = false;
    bool dateCreationOk = false;
    bool dateModifOk = false;
    bool commentOk = false;
    bool currentCalculIDOk = false;
    bool delaunayOk = false;
    bool dMaxOk = false;
    bool useDefaultOk = false;

    QString currentCalculID;
    LPTYCalcul pCalcul = new TYCalcul();
    int readOk = 0; // Indicateur de bonne relecture du calcul
    int i;

    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();
    int childcount = childs.length();
    for (i = 0; i < childcount; i++)
    {
        elemCur = childs.item(i).toElement();
        OMessageManager::get()->info("Charge element du projet %d/%d.", i + 1, childcount);

        TYXMLTools::getElementStringValue(elemCur, "auteur", _auteur, auteurOk);
        TYXMLTools::getElementStringValue(elemCur, "dateCreation", _dateCreation, dateCreationOk);
        TYXMLTools::getElementStringValue(elemCur, "dateModif", _dateModif, dateModifOk);
        TYXMLTools::getElementStringValue(elemCur, "comment", _comment, commentOk);
        TYXMLTools::getElementStringValue(elemCur, "currentCalculID", currentCalculID, currentCalculIDOk);

        TYXMLTools::getElementDoubleValue(elemCur, "delaunayTolerence", _delaunayTolerance, delaunayOk);
        TYXMLTools::getElementDoubleValue(elemCur, "distMaxBetweenPoints", _maxDistBetweenPoints, dMaxOk);
        TYXMLTools::getElementBoolValue(elemCur, "useDefGeomVal", _useDefaultGeomVal, useDefaultOk);

        if (_pSite->callFromXMLIfEqual(elemCur)) { _pSite->setChildsNotInCurrentCalcul(); }

        // Points de controle
        LPTYPointControl pPointControl = new TYPointControl();
        if (elemCur.nodeName() == "PointsControl")
        {
            DOM_Element elemCur2;
            QDomNodeList childs2 = elemCur.childNodes();

            for (unsigned int j = 0; j < childs2.length(); j++)
            {
                elemCur2 = childs2.item(j).toElement();
                if (pPointControl->callFromXMLIfEqual(elemCur2))
                {
                    addPointControl(pPointControl);
                    pPointControl = new TYPointControl();
                }
            }
        }

        //Mise a jour des parametre pour la triangulation du site
        verifGeometricParam();

        // Calculs
        pCalcul->setParent(this);
        if (pCalcul->callFromXMLIfEqual(elemCur, &readOk))
        {
            addCalcul(pCalcul);
            pCalcul = new TYCalcul();
        }
    }


    // Identification du calcul courant
    bool bCurrentCalculFound = false;
    for (i = 0; i < (int)_listCalcul.size(); i++)
    {
        if (_listCalcul[i]->getID().toString() == currentCalculID)
        {
            setCurrentCalcul(_listCalcul[i]);
            bCurrentCalculFound = true;
            break;
        }
    }

    if (!bCurrentCalculFound && (_listCalcul.size() > 0)) { setCurrentCalcul(_listCalcul[0]); }

    if (readOk == -1) { return readOk; }
    return 1;
}

bool TYProjet::addPointControl(LPTYPointControl pPointControl)
{
    assert(pPointControl);

    pPointControl->setParent(this);
    if (getCurrentCalcul()) // Uniquement pour la creation depuis un modeleur
    {
        pPointControl->setEtat(true, getCurrentCalcul()); // Le point de controle est actif dans le calcul courant
    }
    _pointsControl.push_back(pPointControl);

    setIsGeometryModified(true);

    return true;
}

bool TYProjet::remPointControl(LPTYPointControl pPointControl)
{
    bool ret = false;
    assert(pPointControl);
    TYTabLPPointControl::iterator ite;

    for (ite = _pointsControl.begin(); ite != _pointsControl.end(); ite++)
    {
        if ((*ite) == pPointControl)
        {
            _pointsControl.erase(ite);
            ret = true;
            setIsGeometryModified(true);
            break;
        }
    }

    return ret;
}

LPTYPointControl TYProjet::duplicatePointControl(const LPTYPointControl& pPoint)
{
    assert(pPoint);

    // Duplication du point
    LPTYPointControl pPointCopy = new TYPointControl(*pPoint);

    // Ajout a la liste des points du projet
    addPointControl(pPointCopy);

    return pPointCopy;
}

bool TYProjet::remAllPointControl()
{
    _pointsControl.clear();
    setIsGeometryModified(true);

    return true;
}


bool TYProjet::addCalcul(LPTYCalcul pCalcul)
{
    assert(pCalcul);

    pCalcul->setParent(this);
    _listCalcul.push_back(pCalcul);

    //// Si le calcul ajoute est le 1er on le marque comme calcul courant
    //if (_listCalcul.size() == 1) {
    //  setCurrentCalcul(pCalcul);
    //}

    return true;
}

bool TYProjet::remCalcul(const LPTYCalcul pCalcul)
{
    assert(pCalcul);
    bool ret = false;

    // On ne peut detruire le calcul courant
    if (pCalcul == getCurrentCalcul()) { return false; }

    TYTabLPCalcul::iterator ite;

    for (ite = _listCalcul.begin(); ite != _listCalcul.end(); ite++)
    {
        if ((*ite) == pCalcul)
        {
            _listCalcul.erase(ite);
            ret = true;
            break;
        }
    }

    return ret;
}

bool TYProjet::remCalcul(QString idCalcul)
{
    bool ret = false;
    TYTabLPCalcul::iterator ite;

    for (ite = _listCalcul.begin(); ite != _listCalcul.end(); ite++)
    {
        if ((*ite)->getID().toString() == idCalcul)
        {
            return remCalcul((*ite));
        }
    }

    return ret;
}

void TYProjet::remAllCalcul()
{
    _listCalcul.clear();
}

void TYProjet::setCurrentCalcul(LPTYCalcul pCurCalcul)
{
    assert(pCurCalcul);

    // On test si le calcul est present dans la liste
    bool present = false;

    // Statut solveur ok par defaut
    _bStatusSolver = true;

    for (unsigned int i = 0; i < _listCalcul.size(); i++)
    {
        if (_listCalcul[i] == pCurCalcul)
        {
            // Il est present
            present = true;
            break;
        }
    }

    // Non present ?
    if (!present)
    {
        // On l'ajoute
        addCalcul(pCurCalcul);
    }

    // Calcul courant
    _pCurrentCalcul = pCurCalcul;

    ;
    // Test si le solveur du calcul est bien disponible
    if (TYPluginManager::get()->exist(_pCurrentCalcul->getSolverId()))
    {
        _bStatusSolver = false;
    }

    if (_pSite)
    {
        // Le site root associe au projet est obligatoirement actif dans le calcul courant
        _pCurrentCalcul->setSite(_pSite);
        _pSite->setInCurrentCalcul(true, false);
        TYListID aTYListID = _pCurrentCalcul->getElementSelection();
        _pSite->updateCurrentCalcul(aTYListID, true);
    }

#if TY_USE_IHM
    _pCurrentCalcul->updateGraphicTree();
#endif
    this->setIsGeometryModified(true);

    _pSite->update(true);
    _pSite->updateAcoustique(true);
}

bool TYProjet::updateAltiPointControle(TYPointControl* pPtControl, const TYAltimetrie* pAlti)
{
    assert(pPtControl);
    assert(pAlti);

    bool modified = true;

    // On va modifier le point de contrele (l'altitude seulement)
    // (Utile pour l'update du GraphicObject du maillage)
    pPtControl->setIsGeometryModified(true);

    // Init
    OPoint3D pt;

    pt._x = pPtControl->_x;
    pt._y = pPtControl->_y;
    pt._z = 0.0;

    // Recherche de l'altitude
    modified = pAlti->updateAltitude(pt);

    // Ajout de l'offset en Z
    pt._z += pPtControl->getHauteur();

    pPtControl->_z = pt._z;

    // Le point est maintenant up to date
    pPtControl->setIsGeometryModified(false);

    return modified;
}

bool TYProjet::updateAltiRecepteurs()
{
    TYAltimetrie* pAlti = getSite()->getTopographie()->getAltimetrie()._pObj;
    return updateAltiRecepteurs(pAlti);
}

bool TYProjet::updateAltiRecepteurs(const TYAltimetrie* pAlti)
{
    assert(pAlti);
    bool modified = false;


    // Mise a jour de l'altitude pour les points de controle
    unsigned int i;
    for (i = 0; i < _pointsControl.size(); i++)
    {
        // XXX See ticket https://extranet.logilab.fr/ticket/1484180
        // The coordinates of the problematic point need to be properly reported
        // or idealy an exception should be thrown.
        modified &= updateAltiPointControle(getPointControl(i), pAlti);
    }

    // XXX See ticket https://extranet.logilab.fr/ticket/1484180:
    // Why handling of PointControle and Recepteur are so different ?

    // Mise a jour de l'altitude pour les recepteurs du calcul
    getCurrentCalcul()->updateAltiRecepteurs(pAlti);

    // Done
    if (!modified)
    {
        OMessageManager::get()->info(TR("msg_pbalti"));
    }
    else
    {
        setIsGeometryModified(true);
    }

    return modified;
}

LPTYCalcul TYProjet::duplicateCalcul(const LPTYCalcul pCalcul)
{
    assert(pCalcul);

    LPTYCalcul pCalculCopy = new TYCalcul();

    // Duplication
    // On dit qu'on copie aussi l'ID pour que les ID des elements
    // composant le calcul soit copiees
    pCalculCopy->deepCopy(pCalcul, false);

    // On incremente aussi le numero du calcul
    pCalculCopy->setNumero(pCalculCopy->getNumero() + 1);

    // Duplication de l'etat des points de controle
    duplicatePtCalcState(pCalcul, pCalculCopy);

    // Ajout
    addCalcul(pCalculCopy);

    return pCalculCopy;
}

void TYProjet::duplicatePtCalcState(const TYCalcul* pCalculRef, TYCalcul* pCalculNew)
{
    QString idCalculRef = pCalculRef->getID().toString();
    QString idCalculNew = pCalculNew->getID().toString();

    for (unsigned int i = 0; i < _pointsControl.size(); i++)
    {
        _pointsControl[i]->duplicateEtat(idCalculRef, idCalculNew);
    }
}

void TYProjet::remElmtFromCalculs(TYElement* pElement)
{
    for (unsigned int i = 0 ; i < _listCalcul.size() ; i++)
    {
        _listCalcul[i]->remToSelection(pElement);
    }
}

void TYProjet::remTabElmtFromCalculs(vector<LPTYGeometryNode> tabGeoNode)
{
    for (unsigned int i = 0 ; i < tabGeoNode.size() ; i++)
    {
        remElmtFromCalculs(tabGeoNode[i]->getElement());
    }
}


void TYProjet::verifGeometricParam()
{

    _delaunayTolerance = _delaunayTolerance <= 0.0 ? 0.0001 : _delaunayTolerance;
    _delaunayTolerance > 0.05 ? 0.05 : _delaunayTolerance;
    _maxDistBetweenPoints = _maxDistBetweenPoints < 1.0 ? 1.0 : _maxDistBetweenPoints;

    TYCourbeNiveau::setDefaultDistMax(_maxDistBetweenPoints); // MaJ Courbes de niveau
}

void TYProjet::forceUseDefault()
{
    _delaunayTolerance = getDefaultDelaunayTolerence();
    _maxDistBetweenPoints = getDefaultDMax();

    //
    //#if TY_USE_IHM
    //  if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "DelaunayTolerance"))
    //  {
    //      _delaunayTolerance = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "DelaunayTolerance");
    //  }
    //  else
    //  {
    //      TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "DelaunayTolerance", _delaunayTolerance);
    //  }
    //
    //  if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "DistMinPtCrbNiv"))
    //  {
    //      _maxDistBetweenPoints = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "DistMinPtCrbNiv");
    //  }
    //  else
    //  {
    //      TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "DistMinPtCrbNiv", _maxDistBetweenPoints);
    //  }
    //#endif

    verifGeometricParam();
}

const double TYProjet::getDefaultDelaunayTolerence() const
{
    double delaunay = 0.0001;

#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "DelaunayTolerance"))
    {
        delaunay = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "DelaunayTolerance");
    }
    else
    {
        TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "DelaunayTolerance", delaunay);
    }
#endif

    return delaunay;
}

const double TYProjet::getDefaultDMax() const
{
    double DMax = 100.0;

#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "DistMinPtCrbNiv"))
    {
        DMax = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "DistMinPtCrbNiv");
    }
    else
    {
        TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "DistMinPtCrbNiv", DMax);
    }
#endif

    return DMax;
}


void TYProjet::setDMax(const double& val)
{
    _maxDistBetweenPoints = val;
    TYCourbeNiveau::setDefaultDistMax(_maxDistBetweenPoints); // MaJ Courbes de niveau
}

void TYProjet::setSite(const LPTYSiteNode pSite)
{
    assert(pSite);
    _pSite = pSite;
    _pSite->setParent(this);
    _pSite->setProjet(this);
    _pSite->setRoot(true); // Ce site est forcemment le site racine
    _pSite->addToCalcul();
    setIsGeometryModified(true);
}
