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




#include "TYXMLTools.h"

#include "Tympan/core/logging.h"

#if defined(WIN32)
#include <crtdbg.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif


/*static*/ void TYXMLTools::addElementStringValue(DOM_Element& parentElem, DOMString nodeName, DOMString nodeValue)
{
    // Recupere le noeud Document
    QDomDocument domDoc = parentElem.ownerDocument();

    // Cree un nouvel element XML
    QDomElement childElem = domDoc.createElement(nodeName);
    // Ajoute le nouvel element au parent
    parentElem.appendChild(childElem);

    // Cree une nouvelle valeur Text
    QDomText dataVal = domDoc.createTextNode(nodeValue);
    // Associe la valeur au nouvel element
    childElem.appendChild(dataVal);
}

#if TY_ARCH_TYPE == TY_ARCHITECTURE_64
/*static*/ void TYXMLTools::addElementUIntValue(DOM_Element& parentElem, DOMString nodeName, unsigned int nodeValue)
{
    addElementStringValue(parentElem, nodeName, uintToStr(nodeValue).data());
}

/*static*/ void TYXMLTools::addElementUIntValue(DOM_Element& parentElem, DOMString nodeName, size_t nodeValue)
{
    addElementStringValue(parentElem, nodeName, uintToStr(nodeValue).data());
}
#if TY_COMPILER == TY_COMPILER_MSVC
/*static*/ void TYXMLTools::addElementUIntValue(DOM_Element& parentElem, DOMString nodeName, unsigned long nodeValue)
{
    addElementStringValue(parentElem, nodeName, uintToStr(nodeValue).data());
}
#endif
#else
/*static*/ void TYXMLTools::addElementUIntValue(DOM_Element& parentElem, DOMString nodeName, size_t nodeValue)
{
    addElementStringValue(parentElem, nodeName, uintToStr(nodeValue).data());
}

/*static*/ void TYXMLTools::addElementUIntValue(DOM_Element& parentElem, DOMString nodeName, unsigned long nodeValue)
{
    addElementStringValue(parentElem, nodeName, uintToStr(nodeValue).data());
}
#endif

/*static*/ void TYXMLTools::addElementIntValue(DOM_Element& parentElem, DOMString nodeName, int nodeValue)
{
    addElementStringValue(parentElem, nodeName, intToStr(nodeValue).data());
}

/*static*/ void TYXMLTools::addElementBoolValue(DOM_Element& parentElem, DOMString nodeName, bool nodeValue)
{
    addElementStringValue(parentElem, nodeName, intToStr(nodeValue).data());
}

/*static*/ void TYXMLTools::addElementFloatValue(DOM_Element& parentElem, DOMString nodeName, float nodeValue)
{
    addElementStringValue(parentElem, nodeName, floatToStr(nodeValue).data());
}

/*static*/ void TYXMLTools::addElementDoubleValue(DOM_Element& parentElem, DOMString nodeName, double nodeValue)
{
    addElementStringValue(parentElem, nodeName, doubleToStr(nodeValue).data());
}

/*static*/ bool TYXMLTools::getElementStringValue(DOM_Element parentElem, DOMString nodeName, QString& nodeValue)
{
    bool bRet = false;
    if (parentElem.nodeName() == nodeName)
    {
        QDomNode child = parentElem.firstChild();

        if (!child.isNull())
        {
            nodeValue = child.nodeValue();
            bRet = true;
        }
    }
    return bRet;
}

/*static*/ void TYXMLTools::getElementStringValue(DOM_Element parentElem, DOMString nodeName, QString& nodeValue, bool& ok)
{
    if (ok) { return; }
    if (parentElem.nodeName() == nodeName)
    {
        DOM_Node child = parentElem.firstChild();

        if (!child.isNull())
        {
            nodeValue = child.nodeValue();
            ok = true;
        }
    }
}

/*static*/ bool TYXMLTools::getElementIntValue(DOM_Element parentElem, DOMString nodeName, int& nodeValue)
{
    QString nodeValueStr;

    bool bRet = getElementStringValue(parentElem, nodeName, nodeValueStr);

    if (bRet)
    {
        nodeValue = nodeValueStr.toInt();
    }

    return bRet;
}

/*static*/ void TYXMLTools::getElementIntValue(DOM_Element parentElem, DOMString nodeName, int& nodeValue, bool& ok)
{
    if (ok) { return; }

    QString nodeValueStr;

    getElementStringValue(parentElem, nodeName, nodeValueStr, ok);

    if (ok)
    {
        nodeValue = nodeValueStr.toInt();
    }
}

/*static*/ bool TYXMLTools::getElementBoolValue(DOM_Element parentElem, DOMString nodeName, bool& nodeValue)
{
    QString nodeValueStr;

    bool bRet = getElementStringValue(parentElem, nodeName, nodeValueStr);

    if (bRet)
    {
        nodeValue = (nodeValueStr.toInt() == 1) ? true : false;
    }

    return bRet;
}

/*static*/ void TYXMLTools::getElementBoolValue(DOM_Element parentElem, DOMString nodeName, bool& nodeValue, bool& ok)
{
    if (ok) { return; }

    QString nodeValueStr;

    getElementStringValue(parentElem, nodeName, nodeValueStr, ok);

    if (ok)
    {
        nodeValue = (nodeValueStr.toInt() == 1) ? true : false;
    }
}

/*static*/ bool TYXMLTools::getElementFloatValue(DOM_Element parentElem, DOMString nodeName, float& nodeValue)
{
    QString nodeValueStr;

    bool bRet = getElementStringValue(parentElem, nodeName, nodeValueStr);

    if (bRet)
    {
        nodeValue = nodeValueStr.toFloat();
    }

    return bRet;
}

/*static*/ void TYXMLTools::getElementFloatValue(DOM_Element parentElem, DOMString nodeName, float& nodeValue, bool& ok)
{
    if (ok) { return; }

    QString nodeValueStr;

    getElementStringValue(parentElem, nodeName, nodeValueStr, ok);

    if (ok)
    {
        nodeValue = nodeValueStr.toFloat();
    }
}

/*static*/ bool TYXMLTools::getElementDoubleValue(DOM_Element parentElem, DOMString nodeName, double& nodeValue)
{
    QString nodeValueStr;

    bool bRet = getElementStringValue(parentElem, nodeName, nodeValueStr);

    if (bRet)
    {
        nodeValue = nodeValueStr.toDouble();
    }

    return bRet;
}

/*static*/ void TYXMLTools::getElementDoubleValue(DOM_Element parentElem, DOMString nodeName, double& nodeValue, bool& ok)
{
    if (ok) { return; }

    QString nodeValueStr;

    getElementStringValue(parentElem, nodeName, nodeValueStr, ok);

    if (ok)
    {
        nodeValue = nodeValueStr.toDouble();
    }
}

/*static*/ QString TYXMLTools::getElementAttributeToString(DOM_Element parentElem, DOMString attName)
{

    QString attValue;
    if (!parentElem.isNull())
    {
        attValue = parentElem.attribute(attName);
    }

    return attValue;

}

/*static*/ double TYXMLTools::getElementAttributeToDouble(DOM_Element parentElem, DOMString attName, bool* ok)
{
    return getElementAttributeToString(parentElem, attName).toDouble(ok);
}

/*static*/ int TYXMLTools::getElementAttributeToInt(DOM_Element parentElem, DOMString attName, bool* ok)
{
    return getElementAttributeToString(parentElem, attName).toInt(ok);
}

/*static*/ unsigned TYXMLTools::getElementAttributeToUnsigned(DOM_Element parentElem, DOMString attName, bool* ok)
{
    return getElementAttributeToString(parentElem, attName).toUInt(ok);
}
