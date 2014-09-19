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


#include "Tympan/core/logging.h"
#include "Tympan/models/common/3d.h"

/////////////////////////////////////////////////////////////////////////////
// Implementation

#include "TYGeometryNode.h"

TYMapPtrGeoNode* TYGeometryNode::_geoNodeMap = NULL;
TYListPtrGeoNode* TYGeometryNode::_geoNodeDoublonsList = NULL;


// acces map like a singleton to avoid bad order in static instantiation
TYMapPtrGeoNode* TYGeometryNode::GetGeoNodeMap()
{
    if (_geoNodeMap)
    {
        return  _geoNodeMap;
    }
    else { return  _geoNodeMap = new TYMapPtrGeoNode(); }
}

TYListPtrGeoNode* TYGeometryNode::GetGeoNodeDoublonsList()
{
    if (_geoNodeDoublonsList)
    {
        return  _geoNodeDoublonsList;
    }
    else { return  _geoNodeDoublonsList = new TYListPtrGeoNode(); }
}

void TYGeometryNode::addToTheMap()
{
    if (NULL == (TYElement*)getElement())
    {
        return;    //ne devrait pas arriver
    }
    //La place est-elle deja prise ?
    TYGeometryNode* pOldGeometryNode = GetGeoNode(getElement());
    if (pOldGeometryNode)
    {
        //oui => memoriser l'ancien geoNode:
        GetGeoNodeDoublonsList()->push_back(pOldGeometryNode);
    }
    (*GetGeoNodeMap())[getElement()] = this;
}

void TYGeometryNode::removeFromTheMap()
{
    if (NULL == (TYElement*)getElement())
    {
        return;    //ne devrait pas arriver
    }
    //1. supprimer toute reference a this dans la map:
    //La place est-elle deja prise ?
    TYGeometryNode* pOldGeometryNode = GetGeoNode(getElement());
    if (pOldGeometryNode)
    {
        //oui => l'enlever de la map:
        GetGeoNodeMap()->erase(getElement());
    }
    //Y a-t-il parmi la liste de doublon un geoNode pointant sur cet element ?
    //Si c'est le cas, on l'ajoute a la map:
    TYListPtrGeoNode::iterator ite;
    for (ite = GetGeoNodeDoublonsList()->begin(); ite != GetGeoNodeDoublonsList()->end(); ite++)
    {
        TYGeometryNode* pCurrentTYGeometryNode = TYGeometryNode::safeDownCast((*ite));
        if (pCurrentTYGeometryNode->getElement() == getElement())
        {
            //Oui
            //on l'enleve de la liste:
            GetGeoNodeDoublonsList()->erase(ite);
            //on l'ajoute a la map:
            pCurrentTYGeometryNode->addToTheMap();
            break;
        }
    }
}

TYGeometryNode::TYGeometryNode()
{
    _name = TYNameManager::get()->generateName(getClassName());

    _hauteur = 0.0; //dt++
    _pElement = NULL;//az++
}

TYGeometryNode::TYGeometryNode(TYElement* pElt , TYElement* pParent /*=NULL*/) :
    TYElement(pParent)
{
    _pElement = pElt;
    addToTheMap();
    _hauteur = 0.0;

    if (pParent && _pElement)
    {
        // On assigne le meme parent a l'element si celui-ci n'est pas null
        _pElement->setParent(pParent);
    }
}

TYGeometryNode::TYGeometryNode(LPTYElement pElt, TYElement* pParent /*=NULL*/) :
    TYElement(pParent)
{
    _pElement = pElt;
    addToTheMap();
    _hauteur = 0.0;

    if (pParent && _pElement)
    {
        // On assigne le meme parent a l'element si celui-ci n'est pas null
        _pElement->setParent(pParent);
    }
}

TYGeometryNode::TYGeometryNode(const TYRepere& repere, TYElement* pElt)
{
    _repere = repere;
    _hauteur = 0.0;

    _pElement = pElt;
    addToTheMap();
}

TYGeometryNode::TYGeometryNode(const TYRepere& repere, LPTYElement pElt)
{
    _repere = repere;
    _hauteur = 0.0;

    _pElement = pElt;
    addToTheMap();
}

TYGeometryNode::TYGeometryNode(TYElement* pElt, const OMatrix& matrix)
    : _repere(matrix)
{
    _hauteur = 0.0;

    _pElement = pElt;
    addToTheMap();
}

TYGeometryNode::TYGeometryNode(LPTYElement pElt, const OMatrix& matrix)
    : _repere(matrix)
{
    _hauteur = 0.0;

    _pElement = pElt;
    addToTheMap();
}

TYGeometryNode::TYGeometryNode(const TYGeometryNode& other)
{
    _pElement = NULL;
    *this = other;
}

TYGeometryNode::~TYGeometryNode()
{
    if (_pElement) //ce test a ete rajoute surtout pour eviter de faire des recherches dans les map & list en fin d'application, car les map & list en static peuvent etre detruites avant le dernier GeoNode !
    {
        //Comme le pointeur this ne sera plus valide, le supprimer de la liste des doublons:
        GetGeoNodeDoublonsList()->remove(this);
        //Enlever toute reference  dans le map:
        removeFromTheMap();
    }
}


DOM_Element TYGeometryNode::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);

    // On sauvegarde la hauteur en premier
    TYXMLTools::addElementStringValue(domNewElem, "hauteur", doubleToStrPre(_hauteur, 3).data());
    // Puis, on sauvegarde le repere
    _repere.toXML(domNewElem);

    if (_pElement)
    {
        _pElement->toXML(domNewElem);
    }

    return domNewElem;
}

int TYGeometryNode::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);

    int res = -1;
    bool eltFound = false;
    bool hauteurOk = false;
    double hauteurLue = 0;
    DOM_Element elemCur;
    DOM_Node nodeTmp;
    QString str;

    QDomNodeList childs = domElement.childNodes();

    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        // La hauteur doit etre le premier element
        TYXMLTools::getElementDoubleValue(elemCur, "hauteur", hauteurLue, hauteurOk);
        if (hauteurOk)
        {
            _hauteur = hauteurLue;
        }

        // On cherche le repere
        if (_repere.callFromXMLIfEqual(elemCur))
        {
            // Le prochain child (node et pas '#text')
            // doit etre le noeud de l'element
            nodeTmp = elemCur.nextSibling();

            // problem here with count
            // nodeTmp = nodeTmp.nextSibling();

            // Au cas oi nbChild soit faux (trop grand)
            if (nodeTmp.isNull())
            {
                break;
            }

            // Ajout du prefixe TY
            str = "TY";
            str += nodeTmp.nodeName();

            // Auto construction a partir du type trouve
            //_pElement = (TYElement *) TYElement::findAndClone((char *)str.data());//az--
            setElement((TYElement*) TYElement::findAndClone((char*)str.toAscii().data()));  //az++

            // Si la classe a ete trouve (elle doit heriter de TYElement)
            if (_pElement)
            {
                // Le parent de l'element n'est pas le GeoNode mais son parent
                // (uniquement si le champs parent du GeoNode est renseigne)
                if (getParent())
                {
                    _pElement->setParent(getParent());
                }

                // Parsing
                _pElement->fromXML(*((DOM_Element*) &nodeTmp));

                // L'element a ete trouve et traite
                eltFound = true;
            }
        }
    }

    if (eltFound)
    {
        res = 1;
    }

    return res;
}

void TYGeometryNode::getChilds(LPTYElementArray& childs, bool recursif /*=true*/)
{
    TYElement::getChilds(childs, recursif);

    if (_pElement)
    {
        _pElement->getChilds(childs, recursif);
    }
}

void TYGeometryNode::setElement(LPTYElement pElt)
{
    setElement((TYElement*)pElt);
}

void TYGeometryNode::setElement(TYElement* pElt)
{
    removeFromTheMap();
    _pElement = pElt;
    addToTheMap();

    setIsGeometryModified(true);
}


void TYGeometryNode::delElement()
{
    if (_pElement)
    {
        GetGeoNodeMap()->erase(_pElement);
        delete _pElement;
        _pElement = NULL;
    }

    setIsGeometryModified(true);
}


TYGeometryNode& TYGeometryNode::operator=(const TYGeometryNode& other)
{
    if (this != &other)
    {
        TYElement::operator=(other);
        //_pElement = other._pElement;//az--
        setElement(other._pElement);//az++
        _repere = other._repere;
        _hauteur = other._hauteur;
    }
    return *this;
}

bool TYGeometryNode::operator==(const TYGeometryNode& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        if (_pElement != other._pElement) { return false; }
        if (_repere != other._repere) { return false; }
        if (_hauteur != other._hauteur) { return false; }
    }
    return true;
}

bool TYGeometryNode::operator!=(const TYGeometryNode& other) const
{
    return !operator==(other);
}

bool TYGeometryNode::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    TYGeometryNode* pOtherGeoNode = NULL;
    if (pOther) { pOtherGeoNode = (TYGeometryNode*) pOther; }
    if (!pOtherGeoNode || !pOtherGeoNode->getElement()) { return false; }

    // Avant de faire la deep copy sur l'element il faut s'assurer
    // qu'ils sont de meme type
    setElement((TYElement*) pOtherGeoNode->getElement()->clone());

    // Deep copy de l'element
    if (!_pElement->deepCopy(pOtherGeoNode->_pElement, copyId)) { return false; }

    // Deep copy du repere
    if (!_repere.deepCopy(&pOtherGeoNode->_repere, copyId)) { return false; }

    _hauteur = pOtherGeoNode->_hauteur;

    setIsGeometryModified(true);
    setIsAcousticModified(true);

    return true;
}

// XXX There seems to be excessive complexity around updateMatrix and updateRepere :
// setMAtrix->setPrivateMatrix->updateRepere->_repere.set(_matrix)
    // FIXME Why this double copy of matrices while it looks like
    // _repere.getMatChangeRep(_matrix) would be simpler and more efficient ?
TYGeometryNode* TYGeometryNode::GetGeoNode(TYElement* pElement)
{
    if (GetGeoNodeMap()->find(pElement) != GetGeoNodeMap()->end())
    {
        return ((*GetGeoNodeMap())[pElement]);
    }
    return NULL;
}

TYGeometryNode* TYGeometryNode::GetGeoNodeParent() const
{
    //1. Element pointe par le GeoNode:
    TYElement* pElement = getElement();
    //2. on s'interresse aux parents du cet element
    TYElement* pCurrentParent = pElement->getParent();
    while (pCurrentParent)
    {
        //3. le parent est peut-etre deja un TYGeometryNode ?
        TYGeometryNode* pPotentialGeoNode = TYGeometryNode::safeDownCast(pCurrentParent);
        if (pPotentialGeoNode)
        {
            return pPotentialGeoNode;
        }
        //4. le parent dispose d'un GeoNode
        pPotentialGeoNode = GetGeoNode(pCurrentParent);
        if (pPotentialGeoNode)
        {
            return pPotentialGeoNode;
        }
        //5. examinons le parent du parent...
        pCurrentParent = pCurrentParent->getParent();
    }
    return NULL;
}

void TYGeometryNode::GetGeoNodeParentList(TYListPtrGeoNode& GetGeoNodeParents)
{
    TYGeometryNode* pCurrrentGeoNodeParent = GetGeoNodeParent();
    while (pCurrrentGeoNodeParent)
    {
        GetGeoNodeParents.push_back(pCurrrentGeoNodeParent);
        pCurrrentGeoNodeParent = pCurrrentGeoNodeParent->GetGeoNodeParent();
    }
}

OMatrix TYGeometryNode::localToGlobal() const
{
    TYGeometryNode* pParent = GetGeoNodeParent();
    OMatrix matrix = _repere.asMatrix();

    while (pParent != NULL)
    {
        matrix = pParent->getORepere3D().asMatrix() * matrix;
        pParent = pParent->GetGeoNodeParent();
    }

    return matrix;
}

OMatrix TYGeometryNode::globalToLocal() const
{
    return localToGlobal().getInvert();
}

#if TY_USE_IHM
LPTYElementGraphic TYGeometryNode::getGraphicObject()
{
    if (!_pGraphicObject)
    {
        _pGraphicObject = new TYGeometryNodeGraphic((TYGeometryNode*) this);
    }
    return _pGraphicObject;
}
#endif // TY_USE_IHM

void TYGeometryNode::setMatrix(const OMatrix& matrix)
{
    _repere.set(matrix);
    setIsGeometryModified(true);
}
