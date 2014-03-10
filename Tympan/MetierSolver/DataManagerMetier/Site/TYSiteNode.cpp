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

#include <cstdlib>
#include <cassert>

#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerIHM/TYSiteNodeWidget.h"
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/MetierSolver/DataManagerCore/TYPreferenceManager.h"
#include "Tympan/MetierSolver/DataManagerCore/TYXMLManager.h"
#include "Tympan/Tools/TYProgressManager.h"
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/Tools/OMessageManager.h"

#include <QDir>

#if defined(WIN32)
#include <crtdbg.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif

OPROTOINST(TYSiteNode);
TY_EXTENSION_INST(TYSiteNode);

#define TR(id) OLocalizator::getString("OMessageManager", (id))

/*static*/ const std::string& TYSiteNode::getTopoFilePath()
{
    if (_topoFilePath)
    {
        return *_topoFilePath;
    }
    else
    {
        _topoFilePath = new std::string(".");
        return *_topoFilePath;
    }
}

/*static*/ void TYSiteNode::setTopoFilePath(const std::string& path)
{
    if (_topoFilePath)
    {
        *_topoFilePath = path;
    }
    else
    {
        _topoFilePath = new std::string(path);
    }
}

std::string* TYSiteNode::_topoFilePath = NULL;

TYSiteNode::TYSiteNode() :  _pProjet(NULL),
    _bEmpriseAsCrbNiv(false),
    _isTopoFileModified(false),
    _echelle(1.0f),
    _useTopoFile(0),
    _topoFileName(""),
    _topoFileExtension(""),
    _nbFaceInfra(0),
    _altiEmprise(0.0),
    _root(false),
    _SIGType(TYMPAN),
    _SIG_X(0.0),
    _SIG_Y(0.0)
{
    _name = TYNameManager::get()->generateName(getClassName());

#ifdef _WIN32
    // CLM-NT35: Pb en debug avec string cast
    std::string id = getStringID().toAscii();
    _topoFile = getTopoFilePath() + "/image_" + id.data();
#else
    _topoFile = getTopoFilePath() + "/image_" + getStringID().toAscii().data();
#endif

    _pTopographie = new TYTopographie();
    _pTopographie->setParent(this);
    _pInfrastructure = new TYInfrastructure();
    _pInfrastructure->setParent(this);
}

TYSiteNode::TYSiteNode(const TYSiteNode& other)
{
    *this = other;
}

TYSiteNode::~TYSiteNode()
{
    _listSiteNode.clear();
}

TYSiteNode& TYSiteNode::operator=(const TYSiteNode& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        _echelle = other._echelle;
        _bEmpriseAsCrbNiv = other._bEmpriseAsCrbNiv;
        _altiEmprise = other._altiEmprise;
        _pTopographie = other._pTopographie;
        _topoFile = other._topoFile;
        _topoFileName = other._topoFileName;
        _topoFileExtension = other._topoFileExtension;
        _useTopoFile = other._useTopoFile;
        _isTopoFileModified = other._isTopoFileModified;
        _pInfrastructure = other._pInfrastructure;
        _orientation = other._orientation;
        _position = other._position;
        _root = other._root;
        _listSiteNode = other._listSiteNode;
        _SIGType = other._SIGType;
        _SIG_X = other._SIG_X;
        _SIG_Y = other._SIG_Y;
    }
    return *this;
}

bool TYSiteNode::operator==(const TYSiteNode& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        if (_echelle != other._echelle) { return false; }
        if (_bEmpriseAsCrbNiv != other._bEmpriseAsCrbNiv) { return false; }
        if (_altiEmprise != other._altiEmprise) { return false; }
        if (_pTopographie != other._pTopographie) { return false; }
        if (_topoFile != other._topoFile) { return false; }
        if (_topoFileName != other._topoFileName) { return false; }
        if (_topoFileExtension != other._topoFileExtension) { return false; }
        if (_useTopoFile != other._useTopoFile) { return false; }
        if (_isTopoFileModified != other._isTopoFileModified) { return false; }
        if (_pInfrastructure != other._pInfrastructure) { return false; }
        if (_orientation != other._orientation) { return false; }
        if (_position != other._position) { return false; }
        if (_root != other._root) { return false; }
        if (!(_listSiteNode == other._listSiteNode)) { return false; }
        if (_SIGType != other._SIGType) { return false; }
        if (_SIG_X != other._SIG_X) { return false; }
        if (_SIG_Y != other._SIG_Y) { return false; }

    }
    return true;
}

bool TYSiteNode::operator!=(const TYSiteNode& other) const
{
    return !operator==(other);
}

bool TYSiteNode::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    TYSiteNode* pOtherSite = (TYSiteNode*) pOther;

    _echelle = pOtherSite->_echelle;
    _bEmpriseAsCrbNiv = pOtherSite->_bEmpriseAsCrbNiv;
    _altiEmprise = pOtherSite->_altiEmprise;
    _topoFile = pOtherSite->_topoFile;
    _topoFileName = pOtherSite->_topoFileName;
    _topoFileExtension = pOtherSite->_topoFileExtension;
    _useTopoFile = pOtherSite->_useTopoFile;
    _isTopoFileModified = pOtherSite->_isTopoFileModified;
    _pTopographie->deepCopy(pOtherSite->_pTopographie, copyId);
    _pInfrastructure->deepCopy(pOtherSite->_pInfrastructure, copyId);
    _orientation.deepCopy(&pOtherSite->_orientation, copyId);
    _position.deepCopy(&pOtherSite->_position, copyId);
    _root = pOtherSite->_root;
    _SIGType = pOtherSite->_SIGType;
    _SIG_X = pOtherSite->_SIG_X;
    _SIG_Y = pOtherSite->_SIG_Y;

    _listSiteNode.clear();
    for (unsigned int i = 0; i < pOtherSite->_listSiteNode.size(); i++)
    {
        LPTYSiteNodeGeoNode pSiteNodeGeoNode = new TYSiteNodeGeoNode(NULL, this);
        pSiteNodeGeoNode->deepCopy(pOtherSite->_listSiteNode[i], copyId);
        _listSiteNode.push_back(pSiteNodeGeoNode);
    }

    return true;
}

std::string TYSiteNode::toString() const
{
    return "TYSiteNode";
}

DOM_Element TYSiteNode::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);

    TYXMLTools::addElementFloatValue(domNewElem, "echelle", _echelle);
    TYXMLTools::addElementBoolValue(domNewElem, "useEmpriseAsCrbNiv", _bEmpriseAsCrbNiv);
    TYXMLTools::addElementBoolValue(domNewElem, "useTopoFile", _useTopoFile);

    if (TYXMLManager::getSavedFileName() == QString(""))
    {
        TYXMLTools::addElementStringValue(domNewElem, "topoFile", _topoFileName.c_str());
    }
    else //si non, on ecrit le chemin relatif
    {
        QString xmlFile = TYXMLManager::getSavedFileName().replace('\\', '/');
        QDir xmlFileDir = QDir(xmlFile.left(xmlFile.lastIndexOf('/')));
        if (xmlFileDir.exists())
        {
            TYXMLTools::addElementStringValue(domNewElem, "topoFile", xmlFileDir.relativeFilePath(QString(_topoFileName.c_str())).toAscii().constData());
        }
        else
        {
            TYXMLTools::addElementStringValue(domNewElem, "topoFile", _topoFileName.c_str());
        }
    }

    TYXMLTools::addElementDoubleValue(domNewElem, "altiEmprise", _altiEmprise);

    _orientation.toXML(domNewElem);
    _position.toXML(domNewElem);

    _pTopographie->toXML(domNewElem);
    _pInfrastructure->toXML(domNewElem);

    TYXMLTools::addElementIntValue(domNewElem, "root", _root);
    TYXMLTools::addElementIntValue(domNewElem, "repere", _SIGType);
    TYXMLTools::addElementDoubleValue(domNewElem, "SIG_X", _SIG_X);
    TYXMLTools::addElementDoubleValue(domNewElem, "SIG_Y", _SIG_Y);

    for (unsigned int i = 0; i < _listSiteNode.size(); i++)
    {
        _listSiteNode[i]->toXML(domNewElem);
    }

    return domNewElem;
}

int TYSiteNode::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);

    bool echelleOk = false;
    QString topoFile;
    bool topoFileOk = false;
    bool empriseAsCrbNivOk = false;
    bool altiEmpriseOk = false;
    bool useTopoFileOk = false;
    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();
    unsigned int childcount = childs.length();
    for (unsigned int i = 0; i < childcount; i++)
    {
        elemCur = childs.item(i).toElement();
        OMessageManager::get()->info("Charge element de site %d/%d.", i + 1, childcount);

        TYXMLTools::getElementFloatValue(elemCur, "echelle", _echelle, echelleOk);
        TYXMLTools::getElementBoolValue(elemCur, "useEmpriseAsCrbNiv", _bEmpriseAsCrbNiv, empriseAsCrbNivOk);
        TYXMLTools::getElementBoolValue(elemCur, "useTopoFile", _useTopoFile, useTopoFileOk);
        TYXMLTools::getElementStringValue(elemCur, "topoFile", topoFile, topoFileOk);
        TYXMLTools::getElementDoubleValue(elemCur, "altiEmprise", _altiEmprise, altiEmpriseOk);

        _orientation.callFromXMLIfEqual(elemCur);
        _position.callFromXMLIfEqual(elemCur);

        _pTopographie->callFromXMLIfEqual(elemCur);
        _pInfrastructure->callFromXMLIfEqual(elemCur);
    }


    purge(); // On vide le tableau des sous-sites

    bool rootOk = false;
    bool repereOk = false;
    bool SIG_XOk = false;
    bool SIG_YOk = false;
    int SIGType = 0;

    LPTYSiteNodeGeoNode pSiteNodeGeoNode = new TYSiteNodeGeoNode(NULL, this);
    //  DOM_Element elemCur;

    //  QDomNodeList childs = domElement.childNodes();
    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();
        TYXMLTools::getElementBoolValue(elemCur, "root", _root, rootOk);
        TYXMLTools::getElementIntValue(elemCur, "repere", SIGType, repereOk);
        TYXMLTools::getElementDoubleValue(elemCur, "SIG_X", _SIG_X, SIG_XOk);
        TYXMLTools::getElementDoubleValue(elemCur, "SIG_Y", _SIG_Y, SIG_YOk);

        if (pSiteNodeGeoNode->callFromXMLIfEqual(elemCur))
        {
            addSiteNode(pSiteNodeGeoNode);
            pSiteNodeGeoNode = new TYSiteNodeGeoNode(NULL, this);
        }
    }

    _SIGType = (systemSIG)SIGType;

    if (_useTopoFile && topoFileOk)
    {
        _topoFileName = topoFile.toStdString();
        setTopoFileName(_topoFileName);
        loadTopoFile(_topoFileName);
    }

    return 1;
}

void TYSiteNode::getChilds(LPTYElementArray& childs, bool recursif /*=true*/)
{
    TYElement::getChilds(childs, recursif);

    childs.push_back(_pTopographie);
    childs.push_back(_pInfrastructure);

    if (recursif)
    {
        _pTopographie->getChilds(childs, recursif);
        _pInfrastructure->getChilds(childs, recursif);
    }

    for (unsigned int i = 0; i < _listSiteNode.size(); i++)
    {
        childs.push_back(_listSiteNode[i]);
        childs.push_back(_listSiteNode[i]->getElement());
    }

    if (recursif)
    {
        for (unsigned int i = 0; i < _listSiteNode.size(); i++)
        {
            _listSiteNode[i]->getChilds(childs, recursif);
        }
    }
}

void TYSiteNode::setIsGeometryModified(bool isModified)
{
    TYElement::setIsGeometryModified(isModified);

    if (!_root && _pParent) { _pParent->setIsGeometryModified(isModified); }
}

bool TYSiteNode::addToCalcul()
{
    bool res = _pInfrastructure->addToCalcul(); // Ajoute les elements du site lui-meme dans le calcul
    res = res && (_pInfrastructure->addToCalcul());

    if (res && _listSiteNode.size())
    {
        for (unsigned int i = 0; i < _listSiteNode.size(); i++)
        {
            res = res && (TYSiteNode::safeDownCast(_listSiteNode[i]->getElement()))->addToCalcul();
        }
    }

    return res;
}

bool TYSiteNode::remFromCalcul()
{
    bool res = _pInfrastructure->remFromCalcul();
    res = res && _pInfrastructure->remFromCalcul();;

    if (res && _listSiteNode.size())
    {
        for (unsigned int i = 0; i < _listSiteNode.size(); i++)
        {
            res = res && (TYSiteNode::safeDownCast(_listSiteNode[i]->getElement()))->remFromCalcul();
        }
    }

    return res;
}

void TYSiteNode::updateCurrentCalcul(TYListID& listID, bool recursif)//=true
{
    TYListID::iterator ite;

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
}

void TYSiteNode::setProjet(TYProjet* pProjet)
{
    _pProjet = pProjet;
    for (unsigned int i = 0; i < _listSiteNode.size(); i++)
    {
        TYSiteNode::safeDownCast(_listSiteNode[i]->getElement())->setProjet(pProjet);
    }
}

void TYSiteNode::reparent()
{
    _pTopographie->reparent();
    _pInfrastructure->reparent();

    for (unsigned int i = 0; i < _listSiteNode.size(); i++)
    {
        TYSiteNode::safeDownCast(_listSiteNode[i]->getElement())->reparent();
    }
}

void TYSiteNode::loadTopoFile(std::string fileName)
{
    _topoFileName = fileName;
    loadTopoFile();
}

void TYSiteNode::loadTopoFile()
{
    FILE* streamSrc = NULL;
    FILE* streamDest = NULL;
    unsigned char c;

    // Ouverture du fichier source
    if ((streamSrc = fopen(_topoFileName.c_str(), "rb")) == NULL)
    {
        return;
    }

    // Creation d'un copie
    if ((streamDest = fopen(_topoFile.c_str(), "w+b")) == NULL)
    {
        return;
    }

    // Lecture et copie
    while (!feof(streamSrc))
    {
        fread(&c, sizeof(unsigned char), 1, streamSrc);
        fwrite(&c, sizeof(unsigned char), 1, streamDest);
    }

    // Fermeture des fichiers
    fclose(streamSrc);
    fclose(streamDest);

    // Mise a jour du flag.
    _isTopoFileModified = true;

    // On conserve le nom de l'image
    size_t slashAt = _topoFileName.find_last_of("\\");
    if (slashAt == std::string::npos)
    {
        slashAt = _topoFileName.find_last_of("/");
    }


    // On conserve l'extension de l'image = son type
    size_t pointAt = _topoFileName.find_last_of(".");

    if (pointAt = -1)
    {
        _topoFileExtension = "";
    }
    else
    {
        _topoFileExtension = _topoFileName.substr(pointAt);
    }

    setIsGeometryModified(true);
}

/*virtual*/ bool TYSiteNode::updateAltimetrie(const bool& force) // force = false
{
    ostringstream msg;
    OMessageManager& logger =  *OMessageManager::get();
    try
    {
        do_updateAltimetrie(force);
        return true;
    }
    catch (const tympan::AltimetryBuilder::NonComparablePolygons& exc)
    {
        logger.error("Invalid ground material polygons prevented to update the altimetry");
        // TODO move here reporting code from AltimetryBuilder
        msg << boost::diagnostic_information(exc);
        logger.debug(msg.str().c_str());
        return false;
    }
    catch (const tympan::invalid_data& exc)
    {
        msg << boost::diagnostic_information(exc);
        logger.error("Invalid data prevented to update the altimetry (set log level to debug for diagnosic)");
        logger.debug(msg.str().c_str());
        return false;
    }
    catch (const tympan::exception& exc)
    {
        msg << boost::diagnostic_information(exc);
        logger.error("An error prevented to update the altimetry (set log level to debug for diagnosic)");
        logger.debug(msg.str().c_str());
        return false;
    }
}

/*virtual*/ void TYSiteNode::do_updateAltimetrie(const bool& force) // force = false
{
    if (!getIsGeometryModified() && !force) { return; } //L'altimetrie est a jour

    TYNameManager::get()->enable(false);

    OMessageManager::get()->info("Mise a jour altimetrie...");

    /* This is the place where plug the AltimetryBuilder into :
     *  1. pass the topographical elements to the AltimetryBuilder
     *  2. get from it a table of points and triangles vertices
     *  3. put that into the accelerating structure
     *
     */

    // _pTopographie->getAltimetrie()->compute(collectPointsForAltimetrie(), getDelaunay());

    std::deque<OPoint3D> points;
    std::deque<OTriangle> triangles;
    _pTopographie->computeAltimetricTriangulation(points, triangles, getUseEmpriseAsCrbNiv());
    _pTopographie->getAltimetrie()->plugBackTriangulation(points, triangles);


    setIsGeometryModified(false);  // L'altimetrie est a jour

    OMessageManager::get()->info("Mise a jour altimetrie terminee.");

    TYNameManager::get()->enable(true);
}

// TODO : Split the huge method based on the type of infrastructure
// See https://extranet.logilab.fr/ticket/1508248
void TYSiteNode::updateAltiInfra(const bool& force) // force = false
{
    TYNameManager::get()->enable(false);

    TYAltimetrie* pAlti = _pTopographie->getAltimetrie();
    bool modified = false;
    OPoint3D pt;
    register unsigned int i, j;

    size_t totalSteps = _pInfrastructure->getListRoute().size() +
                        _pInfrastructure->getListResTrans().size() +
                        _pInfrastructure->getListBatiment().size() +
                        _pInfrastructure->getListMachine().size() +
                        _pInfrastructure->getSrcs().size() +
                        _pTopographie->getListCrsEau().size() +
                        _pTopographie->getListTerrain().size();

#if TY_USE_IHM
    TYProgressManager::setMessage("Mise a jour de l'altimetrie des infrastructures");
    TYProgressManager::set(static_cast<int>(totalSteps));
#endif // TY_USE_IHM

    bool cancel = false;
    bool bNoPbAlti = true;

    // Mise a jour de l'altitude pour les points des routes
    for (j = 0; j < _pInfrastructure->getListRoute().size() && !cancel; j++)
    {
#if TY_USE_IHM
        TYProgressManager::step(cancel);
        if (cancel) { break; }
#endif // TY_USE_IHM

        // La route
        LPTYRouteGeoNode pGeoNode = _pInfrastructure->getListRoute()[j];
        TYRoute* pRoute = _pInfrastructure->getRoute(j);

        bNoPbAlti &= pRoute->updateAltitudes(*pAlti, pGeoNode);
        modified = true; // As long as there is a road, it will be updated anyways.
    }

    // Mise a jour de l'altitude pour les points des reseaux transport
    for (j = 0; j < _pInfrastructure->getListResTrans().size() && !cancel; j++)
    {
        TYProgressManager::step(cancel);
        if (cancel) { break; }

        TYReseauTransport* pResTrans = _pInfrastructure->getResTrans(j);

        // Hauteur au sol du reseau de transport
        double hauteur = pResTrans->getHauteurMoyenne();

        // Matrice pour la position de cette element
        OMatrix matrix = _pInfrastructure->getListResTrans()[j]->getMatrix();
        OMatrix matrixinv = matrix.getInvert();

        for (i = 0; i < pResTrans->getTabPoint().size(); i++)
        {
            // Passage au repere du site
            pt = matrix * pResTrans->getTabPoint()[i];

            // Init
            pt._z = 0.0;

            // Recherche de l'altitude
            bNoPbAlti &= pAlti->updateAltitude(pt);

            // Ajout de la hauteur du reseau de transport
            pt._z += hauteur;

            // Retour au repere d'origine
            pResTrans->getTabPoint()[i] = matrixinv * pt;

            modified = true;
        }

        pResTrans->setIsGeometryModified(false);
    }

    // Mise a jour de l'altitude pour les batiments
    for (j = 0; j < _pInfrastructure->getListBatiment().size() && !cancel; j++)
    {
#if TY_USE_IHM
        TYProgressManager::step(cancel);
        if (cancel) { break; }
#endif // TY_USE_IHM

        TYBatimentGeoNode* pBatGeoNode = _pInfrastructure->getListBatiment()[j];
        TYBatiment* pBat = TYBatiment::safeDownCast(pBatGeoNode->getElement());

        double hauteurBat = 0.0;
        if (pBat)
        {
            hauteurBat = pBat->volEnglob()._sizeZ;
        }

        // Recuperation de l'origine de l'element
        pt = pBatGeoNode->getRepere()._origin;

        // Hauteur par rapport au sol
        double hauteur = pBatGeoNode->getHauteur();

        // Init
        pt._z = 0.0;

        // Recherche de l'altitude
        bNoPbAlti &= pAlti->updateAltitude(pt);

        ORepere3D repere = pBatGeoNode->getRepere();

        // On repositionne la nouvelle origine
        repere._origin._z = pt._z + hauteur;

        // Mise a jour de la matrice du GeoNode
        pBatGeoNode->setRepere(repere);

        pBat->setIsGeometryModified(false);
        pBat = NULL;
        modified = true;
    }

    // Mise a jour de l'altitude pour les machines
    for (j = 0; j < _pInfrastructure->getListMachine().size() && !cancel; j++)
    {
#if TY_USE_IHM
        TYProgressManager::step(cancel);
        if (cancel) { break; }
#endif // TY_USE_IHM

        TYMachineGeoNode* pMachineGeoNode = _pInfrastructure->getListMachine()[j];
        TYMachine* pMachine = TYMachine::safeDownCast(pMachineGeoNode->getElement());

        double hauteurMachine = 0.0;
        if (pMachine)
        {
            hauteurMachine = pMachine->volEnglob()._sizeZ;
        }

        // Recuperation de l'origine de l'element
        pt = pMachineGeoNode->getRepere()._origin;

        // Hauteur par rapport au sol
        double hauteur = pMachineGeoNode->getHauteur();

        // Init
        pt._z = 0.0;

        // Recherche de l'altitude
        bNoPbAlti &= pAlti->updateAltitude(pt);

        ORepere3D repere = pMachineGeoNode->getRepere();

        // On repositionne la nouvelle origine
        repere._origin._z = pt._z + hauteur;

        pMachineGeoNode->setRepere(repere);

        pMachine->setIsGeometryModified(false);
        pMachine = NULL;
        modified = true;
    }

    // Mise a jour de l'altitude pour les sources utilisateur
    for (j = 0; j < _pInfrastructure->getSrcs().size() && !cancel; j++)
    {
#if TY_USE_IHM
        TYProgressManager::step(cancel);
        if (cancel) { break; }
#endif // TY_USE_IHM

        // La source
        LPTYUserSourcePonctuelle pSrc = TYUserSourcePonctuelle::safeDownCast(_pInfrastructure->getSrc(j)->getElement());

        // Matrice pour la position de cette element
        OMatrix matrix = _pInfrastructure->getSrcs()[j]->getMatrix();
        OMatrix matrixinv = matrix.getInvert();

        // Passage au repere du site
        pt = matrix * *pSrc->getPos();

        // Init
        pt._z = 0.0;

        // Recherche de l'altitude
        bNoPbAlti &= pAlti->updateAltitude(pt);

        // Ajout de la hauteur
        pt._z += pSrc->getHauteur();

        // Retour au repere d'origine
        pt = matrixinv * pt;
        pSrc->getPos()->_z = pt._z;

        // On va modifier la route (l'altitude seulement)
        pSrc->setIsGeometryModified(false);

        modified = true;
    }

    // Mise a jour de l'altitude pour les points des cours d'eau
    for (j = 0; j < _pTopographie->getListCrsEau().size() && !cancel; j++)
    {
#if TY_USE_IHM
        TYProgressManager::step(cancel);
        if (cancel) { break; }
#endif // TY_USE_IHM

        TYCoursEau* pCrsEau = _pTopographie->getCrsEau(j);

        // Matrice pour la position de cette element
        OMatrix matrix = _pTopographie->getListCrsEau()[j]->getMatrix();
        OMatrix matrixinv = matrix.getInvert();

        for (i = 0; i < pCrsEau->getTabPoint().size(); i++)
        {
            // Passage au repere du site
            pt = matrix * pCrsEau->getTabPoint()[i];

            // Init
            pt._z = 0.0;

            // Recherche de l'altitude
            bNoPbAlti &= pAlti->updateAltitude(pt);

            // Retour au repere d'origine
            pCrsEau->getTabPoint()[i] = matrixinv * pt;

            modified = true;
        }

        pCrsEau->setIsGeometryModified(false);
    }

    // Mise a jour de l'altitude pour les points des terrains
    for (j = 0; j < _pTopographie->getListTerrain().size() && !cancel; j++)
    {
#if TY_USE_IHM
        TYProgressManager::step(cancel);
        if (cancel) { break; }
#endif // TY_USE_IHM

        TYTerrain* pTerrain = _pTopographie->getTerrain(j);

        // Matrice pour la position de cette element
        OMatrix matrix = _pTopographie->getListTerrain()[j]->getMatrix();
        OMatrix matrixinv = matrix.getInvert();

        for (i = 0; i < pTerrain->getListPoints().size(); i++)
        {
            // Passage au repere du site
            pt = matrix * pTerrain->getListPoints()[i];

            // Init
            pt._z = 0.0;

            // Recherche de l'altitude
            bNoPbAlti &= pAlti->updateAltitude(pt);

            // Retour au repere d'origine
            pTerrain->getListPoints()[i] = matrixinv * pt;

            modified = true;
        }

        pTerrain->setIsGeometryModified(false);
    }

#if TY_USE_IHM
    TYProgressManager::stepToEnd();
#endif // TY_USE_IHM

    // Warning if an object is not correctly altimetrized
    if (!bNoPbAlti) { OMessageManager::get()->info(TR("msg_pbalti")); }

    if (modified)
    {
        _pInfrastructure->setIsGeometryModified(false);
        _pTopographie->setIsGeometryModified(false);
        setIsGeometryModified(false);
    }

    OMessageManager::get()->info("Mise a jour altimetrie des infrastructures terminee.");

    TYNameManager::get()->enable(true);
}

void TYSiteNode::updateAcoustique(const bool& force)
{
    if (_pProjet)
    {
        TYCalcul* pCalcul = _pProjet->getCurrentCalcul()._pObj;
        assert(pCalcul);
        _pInfrastructure->updateAcoustic(pCalcul, force);
    }

    for (unsigned short i = 0; i < _listSiteNode.size(); i++)
    {
        TYSiteNode* pSite = TYSiteNode::safeDownCast(_listSiteNode[i]->getElement());

        if (pSite && pSite->isInCurrentCalcul()) { pSite->updateAcoustique(force); }
    }
}

// XXX The process is being rewritten and this function removed
/*
TYTabPoint TYSiteNode::collectPointsForAltimetrie() const
{
    TYTabPoint points;
    LPTYPolygon pPolyEmprise = new TYPolygon();
    LPTYTopographie pTopo = NULL;
    unsigned int i, j;



    // Recurence
    for (i = 0; i < _listSiteNode.size(); i++)
    {
        TYSiteNode* pSite = TYSiteNode::safeDownCast(_listSiteNode[i]->getElement());

        // Si le site n'est pas dans le calcul courant, on ne tient pas compte de sa topographie, ni celle de ses sous-sites
        if (pSite->isInCurrentCalcul())
        {
            // Collecte des points
            TYTabPoint tabChild = pSite->collectPointsForAltimetrie();

            // Changement de repere
            OMatrix matrix = _listSiteNode[i]->getMatrix();
            for (j = 0; j < tabChild.size(); j++)
            {
                // et ajout
                points.push_back(matrix * tabChild[j]);
            }
        }
    }

    // Les points d'alti de ce site
    TYTabPoint ptsOfThisSite = getTopographie()->collectPointsForAltimetrie(getUseEmpriseAsCrbNiv());

    // Collecte des sites childs
    TYTabSiteNodeGeoNode sites = collectSites(false);

    // Remove the points of the outter site which lie within the inner site.
    // This (buggy) feature is to be disabled awaiting better specification.

    for (i = 0; i < ptsOfThisSite.size(); i++)
    {
        bool add = true;
        TYPoint pt(ptsOfThisSite[i]);

        for (j = 0; j < sites.size(); j++)
        {
            OPoint3D ptTest(pt);

            // Site child
            pTopo = TYSiteNode::safeDownCast(sites[j]->getElement())->getTopographie();
            pPolyEmprise->setPoints(pTopo->getEmprise());

            // On passe dans le repere de ce site
            OMatrix matrix(sites[j]->getMatrix().getInvert());
            ptTest = matrix * ptTest;

            // La polyline (polygon) definissant l'emprise est a z=0
            // on ramene le point a z=0 pour effectuer le test
            ptTest._z = 0.0;

            // On teste si le point est dans l'emprise du site child
            if (pPolyEmprise->intersects(ptTest) == INTERS_OUI)
            {
                // Dans ce cas on ne garde pas ce point
                add = false;
                // Pas besoin de continuer avec les autres sites
                break;
            }
        }

        if (add)
        {
            // Ajout
            points.push_back(pt);
        }
    }

    return points;
}
*/

double TYSiteNode::getDelaunay()
{
    if (_pProjet) { return _pProjet->getDelaunayTolerence(); }

    double delaunay(0.0001);
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

    delaunay = delaunay <= 0.0 ? 0.0001 : delaunay;
    return delaunay > 0.05 ? 0.05 : delaunay;
}

//az++ (revoir les faces des ecrans; il vaudrait mieux en ajouter proprement):
// tableau d'index des faces ecrans
// According to ticket https://extranet.logilab.fr/ticket/1459658 the notion
// of ecran is obsolete
// TODO remove cleanly related stuff
vector<bool> EstUnIndexDeFaceEcran;

void TYSiteNode::getListFacesWithoutFloor(const bool useEcran, TYTabAcousticSurfaceGeoNode& tabFaces, unsigned int& nbFaceInfra, std::vector<bool>& EstUnIndexDeFaceEcran, std::vector<std::pair<int, int> >& indices, std::vector<int>& etages) const
{
    assert(useEcran && "The useEcran option is obsolete and should always be true before being removed.");

    std::ofstream file;
    file.open("logsChargement.txt", ios::out | ios::trunc);
    file << "Chargement de la liste des faces." << endl;

    EstUnIndexDeFaceEcran.clear();

    unsigned int j, i;
    int compteurFace = 0;
    int compteurInfra = 0;
    TYTabAcousticSurfaceGeoNode tabTmp;

    tabFaces.clear();

    // Si le calcul prend en compte les ecrans
    if (useEcran)
    {
        // Batiments
        for (i = 0; i < _pInfrastructure->getListBatiment().size(); i++)
        {
            file << "Chargement du batiment " << i << endl;
            // Si ce batiment est actif pour le calcul
            LPTYBatiment pBatiment = TYBatiment::safeDownCast(_pInfrastructure->getBatiment(i)->getElement());

            if (pBatiment && pBatiment->isInCurrentCalcul())
            {
                tabTmp.clear();

                // Matrice de changement de repere pour ce batiment
                OMatrix matrix = _pInfrastructure->getListBatiment()[i]->getMatrix();

                for (j = 0; j < pBatiment->getTabAcousticVol().size(); j++)
                {
                    //Attempt to cast volume to a TYEtage
                    LPTYEtage pEtage = TYEtage::safeDownCast(pBatiment->getAcousticVol(j));
                    OMatrix matriceEtage = pBatiment->getTabAcousticVol().at(j)->getMatrix();
                    if (pEtage)
                    {
                        //Récupération des faces des murs
                        for (unsigned int k = 0; k < pEtage->getTabMur().size(); k++)
                        {
                            TYMur* mur = TYMur::safeDownCast(pEtage->getTabMur().at(k)->getElement());
                            OMatrix matriceMur = pEtage->getTabMur().at(k)->getMatrix();
                            if (mur)
                            {
                                file << "Récupération d'un mur rectangulaire." << endl;
                                TYAcousticRectangle* pRect = TYAcousticRectangle::safeDownCast(mur->getTabAcousticSurf().at(0)->getElement());
                                if (pRect)
                                {
                                    //Conversion de la face du mur en AcousticSurfaceGeoNode
                                    file << "Récupération d'un rectangle." << endl;
                                    file << "Ajout de la face " << compteurFace  << ", etage " << j << ", batiment " << i << endl;
                                    LPTYAcousticSurfaceGeoNode newNode = LPTYAcousticSurfaceGeoNode(new TYAcousticSurfaceGeoNode(pRect, matriceEtage * matriceMur));
                                    tabTmp.push_back(newNode);
                                    indices.push_back(std::pair<int, int>(compteurFace++, compteurInfra));
                                    etages.push_back(j);

                                }
                            }
                        }

                        //Recovery of the upper floor only
                        TYAcousticPolygon* poly = TYAcousticPolygon::safeDownCast(pEtage->getPlafond());
                        LPTYAcousticSurfaceGeoNode newNode = LPTYAcousticSurfaceGeoNode(new TYAcousticSurfaceGeoNode(poly, matriceEtage));
                        tabTmp.push_back(newNode);
                        indices.push_back(std::pair<int, int>(compteurFace++, (int)i));
                        etages.push_back(j);

                    }
                    else
                    {
                        // try to cast as a screen (TYEcran)
                        LPTYEcran pEcran = TYEcran::safeDownCast(pBatiment->getAcousticVol(j));

                        if (pEcran)
                        {
                            TYTabAcousticSurfaceGeoNode tabTmp2;
                            tabTmp2 = pEcran->acousticFaces();
                            for (unsigned k = 0; k < tabTmp2.size(); k++)
                            {
                                tabTmp2[k]->setMatrix(matriceEtage * tabTmp2[k]->getMatrix());
                                tabTmp.push_back(tabTmp2[k]);
                                indices.push_back(std::pair<int, int>(compteurFace++, compteurInfra));
                                etages.push_back(j);
                            }
                        }
                    }
                }

                LPTYEtage pEtage = TYEtage::safeDownCast(pBatiment->getAcousticVol(0));
                bool bEtageEcran = false;
                if (pEtage)
                {
                    bEtageEcran = !pEtage->getClosed();
                }
                if (bEtageEcran)
                {
                    pEtage->setacousticFacesPourCalcul(true);
                }

                // L'ensemble des faces de ce batiment
                //tabTmp = TYBatiment::safeDownCast(_pInfrastructure->getBatiment(i)->getElement())->acousticFaces();

                bool bEcran = false; // element de type TYEcran
                // Next 3 lines commented, may be invalid (a building can't be a floor)
                //if (_pInfrastructure->getBatiment(i)->getElement()->isA("TYEcran"))
                //{
                //    bEcran = true;
                //}

                // Pour chacune de ces faces
                for (j = 0; j < tabTmp.size(); j++)
                {
                    // On concatene les matrices
                    tabTmp[j]->setMatrix(matrix * tabTmp[j]->getMatrix());

                    // Ajout de la face
                    tabFaces.push_back(tabTmp[j]);
                    EstUnIndexDeFaceEcran.push_back(bEtageEcran || bEcran);
                }
                if (bEtageEcran)
                {
                    pEtage->setacousticFacesPourCalcul(false);
                }
            }

            compteurInfra++;
        }

        // Machines
        for (i = 0; i < _pInfrastructure->getListMachine().size(); i++)
        {
            // Si cette machine est active pour le calcul
            LPTYMachine pMachine = TYMachine::safeDownCast(_pInfrastructure->getMachine(i)->getElement());
            if (pMachine && pMachine->isInCurrentCalcul())
            {
                tabTmp.clear();

                // Matrice de changement de repere pour cette machine
                OMatrix matrix = _pInfrastructure->getListMachine()[i]->getMatrix();

                // L'ensemble des faces de cette machine
                tabTmp = TYMachine::safeDownCast(_pInfrastructure->getMachine(i)->getElement())->acousticFaces();

                // Pour chacune de ces faces
                for (j = 0; j < tabTmp.size(); j++)
                {
                    // On concatene les matrices
                    tabTmp[j]->setMatrix(matrix * tabTmp[j]->getMatrix());

                    // Ajout de la face
                    tabFaces.push_back(tabTmp[j]);
                    indices.push_back(std::pair<int, int>(compteurFace++, compteurInfra));
                    EstUnIndexDeFaceEcran.push_back(false);
                    etages.push_back(0);
                }
            }
            compteurInfra++;
        }
    }

    nbFaceInfra = static_cast<uint32>(tabFaces.size()); // Determination du nombre de faces de l'infrastructure;

    // Les faces de la topographie (altimetrie) sont transformee en faces acoustiques
    // avec des proprietes acoustiques nulles
    //EstUnIndexDeFaceEcran n'est pas a affecter, car les faces d'infrastructures sont separees de celles de l'alti,
    //donc sachant ou commence les faces d'alti, le test "est un ecran" n'a pas de sens pour ces dernieres


    // WIP here : the materials {c/sh}ould be stored in the TYAcousticPolygon
    //            and thus be stored or exracted from the Altimetry ?
    //            or is this data pulling from the solver to be replaced by data
    //            pushing from the site to the model
    TYTabLPPolygon& listFacesAlti = _pTopographie->getAltimetrie()->getListFaces();
    unsigned int nbFacesAlti = static_cast<uint32>(listFacesAlti.size());

    for (i = 0; i < nbFacesAlti; i++)
    {
        LPTYAcousticPolygon pAccPolygon = new TYAcousticPolygon();
        pAccPolygon->setParent(_pTopographie->getAltimetrie());

        // Geomtrie
        *pAccPolygon->getPolygon() = *listFacesAlti.at(i);

        // Ajout
        LPTYAcousticSurfaceGeoNode pNode = new TYAcousticSurfaceGeoNode((LPTYElement) pAccPolygon);
        tabFaces.push_back(pNode);
        indices.push_back(std::pair<int, int>(compteurFace++, -1));
        etages.push_back(-1);
    }

    file.close();

}

void TYSiteNode::getListFaces(const bool useEcran, TYTabAcousticSurfaceGeoNode& tabFaces, unsigned int& nbFaceInfra, std::vector<bool>& EstUnIndexDeFaceEcran) const
{
    assert(useEcran && "The useEcran option is obsolete and should always be true before being removed.");
    EstUnIndexDeFaceEcran.clear();

    unsigned int j, i;
    TYTabAcousticSurfaceGeoNode tabTmp;

    tabFaces.clear();

    // Si le calcul prend en compte les ecrans
    if (useEcran)
    {
        // Batiments
        for (i = 0; i < _pInfrastructure->getListBatiment().size(); i++)
        {
            // Si ce batiment est actif pour le calcul
            LPTYBatiment pBatiment = TYBatiment::safeDownCast(_pInfrastructure->getBatiment(i)->getElement());
            if (pBatiment && pBatiment->isInCurrentCalcul())
            {
                tabTmp.clear();

                // Matrice de changement de repere pour ce batiment
                OMatrix matrix = _pInfrastructure->getListBatiment()[i]->getMatrix();

                //                LPTYBatiment pBatiment = TYBatiment::safeDownCast(_pInfrastructure->getBatiment(i)->getElement());
                LPTYEtage pEtage = TYEtage::safeDownCast(pBatiment->getAcousticVol(0));

                // Old Code_TYMPAN version could use a floor as a screen so, that case should be treated
                bool bEtageEcran = false;
                if (pEtage)
                {
                    bEtageEcran = !pEtage->getClosed();
                }
                if (bEtageEcran)
                {
                    pEtage->setacousticFacesPourCalcul(true);
                }

                // L'ensemble des faces de ce batiment
                tabTmp = pBatiment->acousticFaces();

                bool bEcran = false; // element de type TYEcran

                // Pour chacune de ces faces
                for (j = 0; j < tabTmp.size(); j++)
                {
                    // On concatene les matrices
                    tabTmp[j]->setMatrix(matrix * tabTmp[j]->getMatrix());

                    // Ajout de la face
                    tabFaces.push_back(tabTmp[j]);
                    EstUnIndexDeFaceEcran.push_back(bEtageEcran || bEcran);
                }
                if (bEtageEcran)
                {
                    pEtage->setacousticFacesPourCalcul(false);
                }
            }
        }

        // Machines
        for (i = 0; i < _pInfrastructure->getListMachine().size(); i++)
        {
            // Si cette machine est active pour le calcul
            LPTYMachine pMachine = TYMachine::safeDownCast(_pInfrastructure->getMachine(i)->getElement());
            if (pMachine && pMachine->isInCurrentCalcul())
            {
                tabTmp.clear();

                // Matrice de changement de repere pour cette machine
                OMatrix matrix = _pInfrastructure->getListMachine()[i]->getMatrix();

                // L'ensemble des faces de cette machine
                tabTmp = TYMachine::safeDownCast(_pInfrastructure->getMachine(i)->getElement())->acousticFaces();

                // Pour chacune de ces faces
                for (j = 0; j < tabTmp.size(); j++)
                {
                    // On concatene les matrices
                    tabTmp[j]->setMatrix(matrix * tabTmp[j]->getMatrix());

                    // Ajout de la face
                    tabFaces.push_back(tabTmp[j]);
                    EstUnIndexDeFaceEcran.push_back(false);
                }
            }
        }
    }

    nbFaceInfra = static_cast<uint32>(tabFaces.size()); // Determination du nombre de faces de l'infrastructure;

    // Les faces de la topographie (altimetrie) sont transformee en faces acoustiques
    // avec des proprietes acoustiques nulles
    //EstUnIndexDeFaceEcran n'est pas a affecter, car les faces d'infrastructures sont separees de celles de l'alti,
    //donc sachant ou commence les faces d'alti, le test "est un ecran" n'a pas de sens pour ces dernieres

    TYTabLPPolygon& listFacesAlti = _pTopographie->getAltimetrie()->getListFaces();
    unsigned int nbFacesAlti = static_cast<uint32>(listFacesAlti.size());

    for (i = 0; i < nbFacesAlti; i++)
    {
        LPTYAcousticPolygon pAccPolygon = new TYAcousticPolygon();
        pAccPolygon->setParent(_pTopographie->getAltimetrie());

        // Geomtrie
        *pAccPolygon->getPolygon() = *listFacesAlti.at(i);

        // Ajout
        LPTYAcousticSurfaceGeoNode pNode = new TYAcousticSurfaceGeoNode((LPTYElement) pAccPolygon);
        tabFaces.push_back(pNode);
    }

}

void TYSiteNode::setAtmosphere(const LPTYAtmosphere& pAtmosphere)
{
    _pTopographie->updateSol(*pAtmosphere);
}

void TYSiteNode::setChildsNotInCurrentCalcul()
{
    TYTabSiteNodeGeoNode& tabGeoNode = getListSiteNode();
    TYSiteNode* pSite = NULL;

    for (unsigned int i = 0; i < tabGeoNode.size(); i++)
    {
        pSite = TYSiteNode::safeDownCast(tabGeoNode[i]->getElement());
        pSite->setInCurrentCalcul(false, true, false);
    }
}


TYTabSiteNodeGeoNode TYSiteNode::collectSites(bool include /*=true*/) const
{
    TYTabSiteNodeGeoNode sites;

    if (include)
    {
        sites.push_back(new TYSiteNodeGeoNode((TYSiteNode*) this));
    }

    for (unsigned int i = 0; i < _listSiteNode.size(); i++)
    {
        // On inclut forcement les sites sinon sans interet
        TYSiteNode* pSite = TYSiteNode::safeDownCast(_listSiteNode[i]->getElement());
        if (pSite && pSite->isInCurrentCalcul())   // Uniquement si le sous-site est dans le calcul
        {
            TYTabSiteNodeGeoNode tabChild = pSite->collectSites(true);

            // Concatenation des matrices
            OMatrix matrix = _listSiteNode[i]->getMatrix();
            for (unsigned int j = 0; j < tabChild.size(); j++)
            {
                tabChild[j]->setMatrix(matrix * tabChild[j]->getMatrix());
            }

            //...et ajoute au tableau a retourner
            sites.insert(sites.end(), tabChild.begin(), tabChild.end());
        }
    }

    return sites;
}

bool TYSiteNode::update(TYElement* pElem)
{
    bool ret = false;

    if (pElem->inherits("TYSourcePonctuelle"))
    {
        return true; // Pas de mise à jour nécessaire
    }
    else if (pElem->inherits("TYAcousticLine")) // Cas 1 : un objet de type source linéique
    {
        TYAcousticLine::safeDownCast(pElem)->updateAcoustic(true);
    }
    else // Autres cas, recherche de parent et traitement approprié
    {
        TYElement* pParent = pElem; // On commencera par tester l'objet lui-meme
        do
        {
            if (pParent->inherits("TYAcousticVolumeNode"))
            {
                ret = TYAcousticVolumeNode::safeDownCast(pParent)->updateAcoustic(true);
                break; // On a trouvé, on peut sortir
            }
            else if (pParent->inherits("TYSiteNode"))
            {
                TYSiteNode::safeDownCast(pParent)->update();
                ret = true;
                break; // On a trouvé, on peut sortir
            }

            pParent = pParent->getParent();
        }
        while (pParent);
    }

    return ret;
}


void TYSiteNode::update(const bool& force) // Force = false
{
    // XXX See ticket https://extranet.logilab.fr/ticket/1484188

    // Mise a jour de l'altimetrie du site principal
    updateAltimetrie(force);

    // Altimetrisation des infrastructures du site
    updateAltiInfra(force);

    // Mise a jour de l'acoustique des elements du site
    updateAcoustique(force);

    // Et celle des sites inclus
    for (unsigned short i = 0; i < _listSiteNode.size(); i++)
    {
        TYSiteNode* pSite = TYSiteNode::safeDownCast(_listSiteNode[i]->getElement());

        if (pSite && pSite->isInCurrentCalcul()) { pSite->update(force); }
    }

    // Si le site est dans un projet, on altimétrise les points de controle
    if (_pProjet)
    {
        TYCalcul* pCalcul = _pProjet->getCurrentCalcul()._pObj;
        assert(pCalcul);
        pCalcul->updateAltiRecepteurs();
    }
}

bool TYSiteNode::addSiteNode(LPTYSiteNodeGeoNode pSiteNodeGeoNode)
{
    assert(pSiteNodeGeoNode);

    LPTYSiteNode pSite = TYSiteNode::safeDownCast(pSiteNodeGeoNode->getElement());

    assert(pSite);

    pSiteNodeGeoNode->setParent(this);
    pSite->setParent(this);
    pSite->setProjet(_pProjet); // Informe du projet cadre

    _listSiteNode.push_back(pSiteNodeGeoNode);

#if TY_USE_IHM
    pSite->updateGraphicTree();
#endif
    setIsGeometryModified(true);

    return true;
}



bool TYSiteNode::addSiteNode(LPTYSiteNode pSiteNode)
{
    return addSiteNode(new TYSiteNodeGeoNode((LPTYElement)pSiteNode));
}

bool TYSiteNode::remSiteNode(const LPTYSiteNodeGeoNode pSiteNodeGeoNode)
{
    assert(pSiteNodeGeoNode);
    bool ret = false;
    TYTabSiteNodeGeoNode::iterator ite;

    for (ite = _listSiteNode.begin(); ite != _listSiteNode.end(); ite++)
    {
        if ((*ite) == pSiteNodeGeoNode)
        {
            // Suppression des calcul
            if (_pProjet) { _pProjet->remElmtFromCalculs((*ite)->getElement()); }

            _listSiteNode.erase(ite);
            ret = true;
            break;
        }
    }

    setIsGeometryModified(true);

    return ret;
}

bool TYSiteNode::remSiteNode(const LPTYSiteNode pSiteNode)
{
    assert(pSiteNode);
    bool ret = false;
    TYTabSiteNodeGeoNode::iterator ite;

    for (ite = _listSiteNode.begin(); ite != _listSiteNode.end(); ite++)
    {
        if (TYSiteNode::safeDownCast((*ite)->getElement()) == pSiteNode)
        {
            // Suppression des calcul
            if (_pProjet) { _pProjet->remElmtFromCalculs((*ite)->getElement()); }

            _listSiteNode.erase(ite);
            ret = true;
            break;
        }
    }

    setIsGeometryModified(true);

    return ret;
}

bool TYSiteNode::remSiteNode(QString idSiteNode)
{
    bool ret = false;
    TYTabSiteNodeGeoNode::iterator ite;

    for (ite = _listSiteNode.begin(); ite != _listSiteNode.end(); ite++)
    {
        if (TYSiteNode::safeDownCast((*ite)->getElement())->getID().toString() == idSiteNode)
        {
            // Suppression des calcul
            if (_pProjet) { _pProjet->remElmtFromCalculs((*ite)->getElement()); }

            _listSiteNode.erase(ite);
            ret = true;
            break;
        }
    }

    setIsGeometryModified(true);

    return ret;
}

LPTYSiteNodeGeoNode TYSiteNode::findSiteNode(const LPTYSiteNode pSiteNode)
{
    assert(pSiteNode);
    TYTabSiteNodeGeoNode::iterator ite;

    for (ite = _listSiteNode.begin(); ite != _listSiteNode.end(); ite++)
    {
        if (TYSiteNode::safeDownCast((*ite)->getElement()) == pSiteNode)
        {
            return (*ite);
        }
    }

    return NULL;
}

LPTYSiteNode TYSiteNode::merge()
{
    LPTYSiteNode pSite = new TYSiteNode();
    pSite->getTopographie()->getListTerrain().clear(); // On vide car un terrain par defaut a ete cree dans le site cible

    unsigned int j;

    // On copie les elements de ce site
    appendSite(this, OMatrix(), pSite);
    pSite->getTopographie()->setDefTerrainIdx(getTopographie()->getDefTerrainIdx());
    pSite->getTopographie()->setEmprise(getTopographie()->getEmprise(), false);
    pSite->getTopographie()->setDefTerrain(getTopographie()->getDefTerrainIdx());

    // Merge des sites enfants
    for (j = 0; j < _listSiteNode.size(); j++)
    {
        // Site enfant courant
        LPTYSiteNodeGeoNode pSiteNodeGeoNode = _listSiteNode[j];

        // Appel recursif
        TYSiteNode* pSiteChild = TYSiteNode::safeDownCast(pSiteNodeGeoNode->getElement());
        assert(pSiteChild);
        bool btest = pSiteChild->isInCurrentCalcul();
        if (pSiteChild && !(pSiteChild->isInCurrentCalcul())) { continue; }

        LPTYSiteNode pSiteTmp = pSiteChild->merge();

        // On copie les elements du site enfant en prenant compte le changement de repere
        appendSite(pSiteTmp, pSiteNodeGeoNode->getMatrix(), pSite);
    }

    pSite->getTopographie()->sortTerrainsBySurface();
    pSite->setProjet(_pProjet);

    return pSite;
}

void TYSiteNode::appendSite(LPTYSiteNode pSiteFrom, const OMatrix& matrix, LPTYSiteNode pSiteTo)
{
    assert(pSiteFrom);
    assert(pSiteTo);
    unsigned int i;
    OMatrix newMatrix;

    // Ajout des elements de topo
    LPTYTopographie pTopoFrom = pSiteFrom->getTopographie();
    LPTYTopographie pTopoTo = pSiteTo->getTopographie();

    // TODO BUG #0008309 : Courbe de niveau pas prise en compte dans l'altimétrie
    if (pSiteFrom->getUseEmpriseAsCrbNiv())
    {
        pTopoTo->addCrbNiv(new TYCourbeNiveau(pTopoFrom->getEmprise(), pSiteFrom->getAltiEmprise()));
    }
    // This scope is here to make p_ground local
    {
        // Add the 'emprise' as a Terrain with its defaultTErrain as Sol attribute.
        TYTerrain* p_ground = new TYTerrain();
        p_ground->setSol(pTopoFrom->getDefTerrain()->getSol());
        p_ground->setListPoints(pTopoFrom->getEmprise());
        pTopoTo->addTerrain(new TYTerrainGeoNode(p_ground, matrix));
    }
    for (i = 0; i < pTopoFrom->getListCrbNiv().size(); i++)
    {
        newMatrix = matrix * pTopoFrom->getListCrbNiv()[i]->getMatrix();
        pTopoTo->addCrbNiv(new TYCourbeNiveauGeoNode(pTopoFrom->getListCrbNiv()[i]->getElement(), newMatrix));
    }

    for (i = 0; i < pTopoFrom->getListTerrain().size(); i++)
    {
        newMatrix = matrix * pTopoFrom->getListTerrain()[i]->getMatrix();
        pTopoTo->addTerrain(new TYTerrainGeoNode(pTopoFrom->getListTerrain()[i]->getElement(), newMatrix));
    }

    for (i = 0; i < pTopoFrom->getListCrsEau().size(); i++)
    {
        newMatrix = matrix * pTopoFrom->getListCrsEau()[i]->getMatrix();
        pTopoTo->addCrsEau(new TYCoursEauGeoNode(pTopoFrom->getListCrsEau()[i]->getElement(), newMatrix));
    }

    for (i = 0; i < pTopoFrom->getListPlanEau().size(); i++)
    {
        newMatrix = matrix * pTopoFrom->getListPlanEau()[i]->getMatrix();
        pTopoTo->addPlanEau(new TYPlanEauGeoNode(pTopoFrom->getListPlanEau()[i]->getElement(), newMatrix));
    }


    // Ajout des elements d'infra
    LPTYInfrastructure pInfraFrom = pSiteFrom->getInfrastructure();
    LPTYInfrastructure pInfraTo = pSiteTo->getInfrastructure();

    for (i = 0; i < pInfraFrom->getListRoute().size(); i++)
    {
        newMatrix = matrix * pInfraFrom->getListRoute()[i]->getMatrix();
        pInfraTo->addRoute(new TYRouteGeoNode(pInfraFrom->getListRoute()[i]->getElement(), newMatrix));
    }

    for (i = 0; i < pInfraFrom->getListResTrans().size(); i++)
    {
        newMatrix = matrix * pInfraFrom->getListResTrans()[i]->getMatrix();
        pInfraTo->addResTrans(new TYReseauTransportGeoNode(pInfraFrom->getListResTrans()[i]->getElement(), newMatrix));
    }

    for (i = 0; i < pInfraFrom->getListBatiment().size(); i++)
    {
        newMatrix = matrix * pInfraFrom->getListBatiment()[i]->getMatrix();
        TYBatimentGeoNode* pBatNode = new TYBatimentGeoNode(pInfraFrom->getListBatiment()[i]->getElement(), newMatrix);
        pBatNode->setHauteur(pInfraFrom->getListBatiment()[i]->getHauteur());
        pInfraTo->addBatiment(pBatNode);
    }

    for (i = 0; i < pInfraFrom->getListMachine().size(); i++)
    {
        newMatrix = matrix * pInfraFrom->getListMachine()[i]->getMatrix();
        TYMachineGeoNode* pMachineNode = new TYMachineGeoNode(pInfraFrom->getListMachine()[i]->getElement(), newMatrix);
        pMachineNode->setHauteur(pInfraFrom->getListMachine()[i]->getHauteur());
        pInfraTo->addMachine(pMachineNode);
    }

    for (i = 0; i < pInfraFrom->getSrcs().size(); i++)
    {
        newMatrix = matrix * pInfraFrom->getSrcs()[i]->getMatrix();
        pInfraTo->addSrc(new TYSourcePonctuelleGeoNode(pInfraFrom->getSrcs()[i]->getElement(), newMatrix));
    }

}

void TYSiteNode::exportCSV(std::ofstream& ofs)
{
    // Export du nom de l'objet
    ofs << getName().toAscii().data() << '\n';

    // Export du type de l'objet
    ofs << toString() << '\n';
    // Export des donnees acoustiques
    LPTYElementArray childs;
    getChilds(childs);

    for (int i = 0; i < childs.size() ; i++)
    {
        TYElement* pElement = childs[i];
        if (pElement->inherits("TYSiteNode"))
        {
            // Export du nom de l'objet
            ofs << pElement->getName().toAscii().data() << '\n';
        }
        else if (pElement->inherits("TYAcousticVolumeNode"))
        {
            LPTYAcousticVolumeNode pVolNode = TYAcousticVolumeNode::safeDownCast(pElement);
            if (pVolNode)
            {
                pVolNode->exportCSV(ofs);
            }
        }
        else if (pElement->inherits("TYAcousticLine"))
        {
            LPTYAcousticLine pAcLine = TYAcousticLine::safeDownCast(pElement);
            if (pAcLine)
            {
                pAcLine->exportCSV(ofs);
            }
        }
        else if (pElement->inherits("TYUserSourcePonctuelle"))
        {
            LPTYUserSourcePonctuelle pSource = TYUserSourcePonctuelle::safeDownCast(pElement);
            if (pSource)
            {
                pSource->exportCSV(ofs);
            }
        }
    }

    ofs << '\n';
}
