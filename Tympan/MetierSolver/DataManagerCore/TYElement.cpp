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
 *
 */



#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHCore.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/Tools/OGenID.h"
#include "Tympan/Tools/OChrono.h"

#if defined(WIN32)
#include <crtdbg.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif

#include "Tympan/Tools/OMessageManager.h"


// -------------------------------------------------------------------------
// classe TYElement
// -------------------------------------------------------------------------

OPROTOINST(TYElement)

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

// void CompteInstance(TYListPtrElement& instances)
// {
//     const int nbTypeObjet = 1024;
//     int nCurrentNbTypeObjet = 0;
//     std::string InstancesName[nbTypeObjet];
//     long instanceCount[nbTypeObjet];
//     int i;
//     //RAZ
//     for (i = 0; i < nbTypeObjet; i++)
//     {
//         instanceCount[i] = 0;
//     }
//     //Comptage des objets
//     TYElement* pTYElement = NULL;
//     TYListPtrElement::iterator ite;
//     int nCurrentPos = 0;
//     for (ite = instances.begin(); ite != instances.end(); ite++)
//     {
//         pTYElement = (*ite);
//         bool bValidPtr = false ; // INIT to false Projet_Tympan to avoid warnings on g++
//         if (pTYElement)
//         {
//             bValidPtr = pTYElement->getRefCount() >= 0 && pTYElement->getRefCount() < 5000;    //(NULL != OPrototype::safeDownCast(pTYElement));
//         }
//         if (bValidPtr && pTYElement)
//         {
//             //Est-ce que le nom de cet objet est enregistre ?
//             bool bClasseConnue = false;
//             for (i = 0; i < nCurrentNbTypeObjet; i++)
//             {
//                 if (pTYElement->getClassName() && InstancesName[i] == pTYElement->getClassName())
//                 {
//                     bClasseConnue = true;
//                     instanceCount[i]++;//et un objet en plus
//                     break;//oui
//                 }
//             }
//             //Non enregistre:
//             if (!bClasseConnue && pTYElement->getClassName())
//             {
//                 InstancesName[nCurrentNbTypeObjet] = pTYElement->getClassName();
//                 instanceCount[nCurrentNbTypeObjet]++;//et un objet en plus
//                 nCurrentNbTypeObjet++;
//             }

//         }
//         nCurrentPos++;

//     }
//     //Decompte total:
// #ifdef _DEBUG
//     for (i = 0; i < nCurrentNbTypeObjet; i++)
//     {
//         char Debug[2048];
//         sprintf(Debug, "%s : %d\n", InstancesName[i].c_str(), instanceCount[i]);
//         OMsg::msgDebug(Debug);
//         //printf(Debug);
//     }
//     OMsg::msgDebug("\n");
// #endif
//
// }

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
    // bool bCompteInstance = false;
    // if (bCompteInstance)
    // {
    //     CompteInstance(getInstances());
    // }

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
        bool success = getInstances().insert(std::make_pair(_uuid, this)).second;
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
        bool success = getInstances().insert(std::make_pair(uuid, this)).second;
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
    if (!pOther) { return false; }

    if (!pOther->inherits(getClassName())) { return false; }

    _name = pOther->_name;
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
    //  TYElementCollection childs;
    //  getChilds(childs, false);
    //
    //  // Appel recursif
    //  for (int i = 0; i < childs.getCount(); i++)
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
    TYElementCollection childs;
    getChilds(childs, false);

    if (childs.getCount() > 0)
    {
        // Appel recursif
        bool onechildpresent = false;
        for (int i = 0; i < childs.getCount(); i++)
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
        TYElementCollection childs;
        getChilds(childs, false);
        for (unsigned int i = 0; i < childs.getCount(); i++)
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


/*static*/ TYElementCollection TYElement::findTypeCollectionAndCallFromXML(DOM_Element parentElem, const char* type)
{
    TYElementCollection eltCollection;
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

        // Auto construction
        pElt = (TYElement*) TYElement::findAndClone((char*)str.toAscii().data());

        if (pElt && pElt->inherits(type))
        {
            // Parsing XML
            pElt->fromXML(elemCur);

            // Ajout
            eltCollection.add(pElt);
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
