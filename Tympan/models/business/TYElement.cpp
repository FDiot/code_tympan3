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


#include <string.h>
#include <iostream>

#include "Tympan/core/idgen.h"
#include "Tympan/core/chrono.h"
#include "Tympan/core/exceptions.h"
#include "Tympan/core/logging.h"
#include "TYElement.h"



// Declaration des membres statiques.
std::unordered_map<std::string, OPrototype::IOProtoFactory::ptr_type> OPrototype::_factory_map;

OPrototype::OPrototype()
{
}

OPrototype::~OPrototype()
{
}

/* static */ void OPrototype::add_factory(const char * classname,
        IOProtoFactory::ptr_type factory)
{
    _factory_map[std::string(classname)] = move(factory);
}

/*static*/ OPrototype* OPrototype::findAndClone(const char* className)
{
    auto it = _factory_map.find(className);

    if (it == _factory_map.end())
    {
        std::string err_msg ("Asked to clone class ");
        err_msg.append (className);
        err_msg.append (" which isn't registered in OPrototype.");
        throw tympan::invalid_data(err_msg) << tympan_source_loc
            << tympan::oproto_classname_errinfo(className);
    }
    else
    {
        return _factory_map[className]->make().release();
    }
}

/*static*/ int OPrototype::findPrototype(const char* className)
{
    if (className == 0) { return -1; }
    // Return 1 if the class className exists, -1 otherwise
    return (_factory_map.count(className) > 0 ? 1 : -1);
}

bool OPrototype::isA(const char* className) const
{
    // Test le nom du type
    return (!strcmp(className, this->getClassName()));
}

/*static*/ OPrototype* OPrototype::safeDownCast(OPrototype* pObject)
{
    return (OPrototype*) pObject;
}


// -------------------------------------------------------------------------
// classe TYElement
// -------------------------------------------------------------------------


/** Le log des instances ne doit absolument pas etre active
 * au lancement de l'application, le log des instances statiques
 * provoque un crash. */


bool TYElement::_logInstances = false;
// TYListPtrElement* TYElement::_instances = NULL;
TYElementContainer* TYElement::_instances = NULL;

bool TYElement::_toSave = false;
bool TYElement::_bRegenerateID = false;


/*static*/ uint64 TYElement::ty_created_counter = 0;
/*static*/ uint64 TYElement::ty_destroyed_counter = 0;
/*static*/ uint64 TYElement::ty_regen_id_counter = 0;

TYElementContainer& TYElement::getInstances()
{
    if (_instances == NULL)
    {
        _instances = new TYElementContainer();
    }

    return *_instances;
}

TYElement::TYElement() : _bPutInInstanceList(true), _pParent(NULL), _copyCount(0)
{
    // regenerateID(); //XXX regenerate it on need only : let it null initialy

    // On dit que l'element est present dans le Calcul courant dans
    // le cas ou justement il n'y a pas de notion de calcul courant.
    _inCurrentCalcul = true;

    // Objet graphique
    _pGraphicObject = NULL;

    // Flags modifies
    _isAcousticModified = true;
    _isGeometryModified = true;

    if (_bPutInInstanceList)
    {
        addInstance();
    }
    ty_created_counter++;
}

TYElement::TYElement(TYElement* pParent, bool PutInInstanceList)
    : _bPutInInstanceList(PutInInstanceList), _pParent(pParent), _copyCount(0)
{
    // regenerateID(); //XXX regenerate it on need only : let it null initialy

    // On dit que l'element est present dans le Calcul courant dans
    // le cas ou justement il n'y a pas de notion de calcul courant.
    _inCurrentCalcul = true;

    // Objet graphique
    _pGraphicObject = NULL;

    // Flags modifies
    _isAcousticModified = true;
    _isGeometryModified = true;

    if (_bPutInInstanceList)
    {
        addInstance();
    }
    ty_created_counter++;
}

TYElement::TYElement(const TYElement& other, bool PutInInstanceList/*= true*/)
    : _bPutInInstanceList(PutInInstanceList)
{
    *this = other;

    if (_bPutInInstanceList)
    {
        addInstance();
    }
}

TYElement::~TYElement()
{
    if (_bPutInInstanceList)
    {
        remInstance();
    }
#if TY_USE_IHM
    if (_pGraphicObject)
    {
        _pGraphicObject->setElement(NULL);
        _pGraphicObject = NULL;
    }
#endif

    _pParent = NULL;
    ty_destroyed_counter++;
}

TYElement* TYElement::getInstance(TYUUID uuid)
{
    TYElementContainer::const_iterator elt_it = getInstances().find(uuid);
    if (elt_it != getInstances().end())
    {
        return elt_it->second;
    }

    OMessageManager::get()->error("instance not found: %s",
                                  uuid.toString().toUtf8().data());

    return NULL;
}

void TYElement::purgeInstances()
{
    getInstances().clear();
}

const TYUUID& TYElement::getID() const
{
    if (hasNullID())
    {
        _uuid = newID();
    }

    return _uuid;
}

void TYElement::setID(TYUUID id)
{
    // If the _uuid is not NULL, we remove it from the map.
    bool was_registered = false;
    if (!hasNullID())
    {
        // size_t remove_count = getInstances().erase(_uuid);
        TYElementContainer::iterator it = getInstances().find(_uuid);
        if (it != getInstances().end())
        {
            getInstances().erase(it);
            was_registered = true;
        }
    }

    _uuid = id;

    // If the element was registered, we update the map (new insertion).
    if (was_registered)
    {
        getInstances().insert(std::make_pair(_uuid, this));
    }
}

void TYElement::setID(const QString& str_id)
{
    TYUUID new_uuid(str_id);
    setID(new_uuid);
}


bool TYElement::testId(const TYUUID& id, const TYElement* pElem)
{
    TYElement* pEl = getInstance(id);
    if (pEl != pElem) { return true; }
    return false;
}

void TYElement::addInstance()
{
    if (_logInstances)
    {
        const TYUUID& uuid = getID(); // Could force the generation of the UUID.
        getInstances().insert(std::make_pair(uuid, this));
    }
}


void TYElement::remInstance()
{
    if (!hasNullID())
    {
        getInstances().erase(getID());
    }
}


void TYElement::regenerateID()
{
    setID(newID());
}

void TYElement::setIsAcousticModified(bool isModified)
{
    _isAcousticModified = isModified;
}

void TYElement::setIsGeometryModified(bool isModified)
{
    _isGeometryModified = isModified;

#if TY_USE_IHM
    if (_pGraphicObject && isModified)
    {
        _pGraphicObject->setModified(_isGeometryModified);
    }
#endif
}

TYElement& TYElement::operator = (const TYElement& other)
{
    if (this != &other)
    {
        setID(other.getID());
        _name = other._name;
        _pParent = other._pParent;
        _inCurrentCalcul = other._inCurrentCalcul;
        _copyCount = other._copyCount;

        setIsGeometryModified(true);
        setIsAcousticModified(true);
    }
    return *this;
}

bool TYElement::operator == (const TYElement& other) const
{
    if (this != &other)
    {
        if (getID() != other.getID()) { return false; }
        if (_name != other._name) { return false; }
        if (_pParent != other._pParent) { return false; }
    }
    return true;
}

bool TYElement::operator != (const TYElement& other) const
{
    return !operator==(other);
}

bool TYElement::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!pOther) { return false; }     // XXX assert(pOther);

    // For now we don't look at the hierarchy. Let's suppose we are expecting
    // to copy an element of the same class as this
    if (strcmp(pOther->getClassName(), getClassName()) != 0) { return false; }

    _name = pOther->_name;
    _pParent = pOther->_pParent;
    _inCurrentCalcul = pOther->_inCurrentCalcul;
    _copyCount = pOther->_copyCount;

    _copyCount++;

    QString copyTag = QString("Copie (%1) de :").arg(_copyCount);

    if (copyId)
    {
        setID(pOther->getID());
        copyTag = "";
        _copyCount > 0 ? _copyCount-- : 0 ;
    }

    TYElement* pOther2 = const_cast<TYElement*>(pOther);
    pOther2->setCopyCount(_copyCount);

    if (_copyCount > 1)
    {
        QStringList strLst = _name.split(':');
        if (strLst.size() > 1)
        {
            _name = strLst[1];
        }
    }

    _name = copyTag + _name;

    _pGraphicObject = NULL;

    setIsGeometryModified(true);
    setIsAcousticModified(true);

    return true;
}

DOM_Element TYElement::toXML(DOM_Element& domElement) /* const */
{
    QDomDocument domDoc = domElement.ownerDocument();
    QDomElement domNewElem = domDoc.createElement(getMetierName().data());

    domNewElem.setAttribute("id", getID().toString());
    domNewElem.setAttribute("name", _name);

    domElement.appendChild(domNewElem);

    return domNewElem;
}

int TYElement::fromXML(DOM_Element domElement)
{
    QString tmpString = TYXMLTools::getElementAttributeToString(domElement, "id");
    if (tmpString.size() == 0)
    {
        // regenerateID(); //XXX
    }
    else
    {
        setID(tmpString);
    }

    QString name = TYXMLTools::getElementAttributeToString(domElement, "name");
    if (!name.isEmpty())
    {
        _name = name;
    }

    if (_bRegenerateID) { regenerateID(); }

    setIsGeometryModified(true);
    setIsAcousticModified(true);

    return 1;
}

void TYElement::setInCurrentCalcul(bool state, bool recurschild /*= true*/,  bool recursparent /*= true*/)
{
    // Set state for this object
    _inCurrentCalcul = state;
    setIsGeometryModified(true);

    //// update childs status...
    //if (recurschild) {
    //  // Collecte des childs
    //  LPTYElementArray childs;
    //  getChilds(childs, false);
    //
    //  // Appel recursif
    //  for (int i = 0; i < childs.size(); i++)
    //  {
    //      childs[i]->setInCurrentCalcul(state, recurschild, false);
    //  }
    //}


    // update parent status (uniquement en cas d'activation)
    if (recursparent && state)
    {
        TYElement* pParent = getParent();
        if (pParent) { pParent->setInCurrentCalcul(state, false, recursparent); }
    }
}

void TYElement::OnChildInCalculStatusChange()
{
    // Collecte des childs
    LPTYElementArray childs;
    getChilds(childs, false);

    if (childs.size() > 0)
    {
        // Appel recursif
        bool onechildpresent = false;
        for (int i = 0; i < childs.size(); i++)
        {
            onechildpresent = onechildpresent || (childs[i]->isInCurrentCalcul());
        }

        // the parent is in the calcul if at least one of its direct children is in the calcul.
        setInCurrentCalcul(onechildpresent, false);
    }
}

void TYElement::updateCurrentCalcul(TYListID& listID, bool recursif)//=true
{
    bool present = false;
    TYListID::iterator ite;

    // Parcours de la selection du calcul
    for (ite = listID.begin(); ite != listID.end(); ++ite)
    {
        if ((*ite) == getID())
        {
            // Cet element est present dans la liste
            present = true;
            break;
        }
    }

    setInCurrentCalcul(present, false);

    if (recursif)
    {
        // Collecte des childs
        LPTYElementArray childs;
        getChilds(childs, false);
        for (unsigned int i = 0; i < childs.size(); i++)
        {
            childs[i]->updateCurrentCalcul(listID, recursif);
        }
    }
}

#if TY_USE_IHM
void TYElement::drawGraphic(bool draw)
{
    if (draw)
    {
        if (getGraphicObject())
        {
            getGraphicObject()->setVisible(true);
        }

        // Mise a jour
        updateGraphic();
    }
    else
    {
        if (getGraphicObject())
        {
            getGraphicObject()->setVisible(false);
        }
    }
}

void TYElement::updateGraphic()
{
    if (getGraphicObject())
    {
        getGraphicObject()->update();
    }
}

void TYElement::updateGraphicTree()
{
    if (getGraphicObject())
    {
        getGraphicObject()->updateTree();
    }
}

TYEditWidget* TYElement::getEditWidget()
{
    return new TYElementWidget(this);
}

int TYElement::edit(TYEditWidget* pParent /*=NULL*/)
{
    int ret = -1;

    ret = TYWidget::edit(this, pParent);

    return ret;
}

#endif // TY_USE_IHM

bool TYElement::callFromXMLIfEqual(DOM_Element& domElement, int* pRetVal /*=NULL*/)
{
    bool bRet = false;
    int retVal = 0; // INIT to 0 Projet_Tympan to avoid warnings on g++
    if (pRetVal) { retVal = *pRetVal; } // Recuperation de la valeur de pRetVal

    if (domElement.nodeName().compare(QString(getMetierName().data())) == 0)
    {
        retVal = fromXML(domElement);
        bRet = true;
    }

    if (pRetVal) { *pRetVal = retVal; } // Si demandee, propagation de la valeur de retval

    return bRet;
}

std::string TYElement::getMetierName()
{
    return &this->getClassName()[2];
}


/*static*/ LPTYElementArray TYElement::findTypeCollectionAndCallFromXML(DOM_Element parentElem, const char* type)
{
    LPTYElementArray eltCollection;
    LPTYElement pElt = NULL;
    DOM_Element elemCur;

    QDomNodeList childs = parentElem.childNodes();
    QString str;

    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        // Ajout du prefixe TY
        QString nodeName = elemCur.nodeName();
        str = "TY";
        str += nodeName;

        try
        {
            // Auto construction
            pElt = dynamic_cast<TYElement*>(TYElement::findAndClone((char*)str.toAscii().data()));
        }
        catch(tympan::invalid_data& exc) {pElt = nullptr;}

        // For now we don't look at the hierarchy. Let's suppose we are
        // looking for the very type we cloned which is the most likely hypothesis
        if ((pElt != nullptr) && (strcmp(pElt->getClassName(), type) == 0))
        {
            // Parsing XML
            pElt->fromXML(elemCur);

            // Ajout
            eltCollection.push_back(pElt);
        }
    }
    return eltCollection;
}



/*static*/ TYUUID TYElement::newID()
{
    TYUUID newID;
    newID.GenUniqueID(); ++ty_regen_id_counter;
    return newID;
}

/*static*/ TYUUID TYElement::fromString(QString id)
{
    TYUUID newID(id);
    return newID;
}

/*static*/ QString TYElement::toString(TYUUID& uuid)
{
    return uuid.toString();
}

/*static*/ uint64 TYElement::getConstructorCount()
{
    return ty_created_counter;
}
/*static*/ uint64 TYElement::getDestructorCount()
{
    return ty_destroyed_counter;
}

/*static*/ uint64 TYElement::getIdGenerationCount()
{
    return ty_regen_id_counter;
}
