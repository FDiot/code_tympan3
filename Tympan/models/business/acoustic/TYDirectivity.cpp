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

#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include<boost/lexical_cast.hpp>
#include<boost/tokenizer.hpp>
#include<boost/algorithm/string/trim.hpp>
#include<boost/foreach.hpp>

#if TY_USE_IHM
#include "Tympan/gui/widgets/TYDirectivityWidget.h"
#endif

#include "TYDirectivity.h"

TY_EXTENSION_INST(TYDirectivity);

TYDirectivity::TYDirectivity() : 
        DirectivityVector( OVector3D(0., 0., 1.) )
{
    _name = TYNameManager::get()->generateName(getClassName());
}

TYDirectivity::TYDirectivity(const TYDirectivity& other)
{
    *this = other;
}

TYDirectivity::~TYDirectivity()
{
}

TYDirectivity& TYDirectivity::operator=(const TYDirectivity& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
    }

    DirectivityVector = other.DirectivityVector;

    return *this;
}

bool TYDirectivity::operator==(const TYDirectivity& other) const
{
    if (this != &other)
    {
        if ( TYElement::operator!=(other) ) { return false; }
        if ( DirectivityVector != other.DirectivityVector ) { return false; } 
    }
    return true;
}

bool TYDirectivity::operator!=(const TYDirectivity& other) const
{
    return !operator==(other);
}

bool TYDirectivity::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    TYDirectivity* pOtherDir = (TYDirectivity*) pOther;

    return true;
}

std::string TYDirectivity::toString() const
{
    return "TYDirectivity";
}

DOM_Element TYDirectivity::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);

    DOM_Document domDoc = domElement.ownerDocument();

    DOM_Element vectorXElem = domDoc.createElement("Vector");
    domNewElem.appendChild(vectorXElem);
    vectorXElem.setAttribute("x", doubleToStrPre(DirectivityVector._x, 3).data());
    vectorXElem.setAttribute("y", doubleToStrPre(DirectivityVector._y, 3).data());
    vectorXElem.setAttribute("z", doubleToStrPre(DirectivityVector._z, 3).data());

    return domNewElem;
}

int TYDirectivity::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);

    DOM_Element elemCur;
    QDomNodeList childs = domElement.childNodes();

    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        if (elemCur.nodeName() == "Vector")
        {
            DirectivityVector._x = TYXMLTools::getElementAttributeToDouble(elemCur, "x");
            DirectivityVector._y = TYXMLTools::getElementAttributeToDouble(elemCur, "y");
            DirectivityVector._z = TYXMLTools::getElementAttributeToDouble(elemCur, "z");
        }
    }

    return 1;
}

// -----------------------
TYUserDefinedDirectivity::TYUserDefinedDirectivity( const TYUserDefinedDirectivity& other ) :
                            TYDirectivity(other)
{
    DirectivityTable = other.DirectivityTable;
}


DOM_Element TYUserDefinedDirectivity::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYDirectivity::toXML(domElement);
    QDomDocument domDoc = domElement.ownerDocument();
    for (unsigned int i = 0; i < DirectivityTable.size(); i++)
    {
        DOM_Element domChildElem = domDoc.createElement("DirectivityValues");
        domNewElem.appendChild(domChildElem);

        domChildElem.setAttribute("theta", doubleToStr(DirectivityTable.at(i).Theta).data());
        domChildElem.setAttribute("phi", doubleToStr(DirectivityTable.at(i).Phi).data());
        DirectivityTable.at(i).Coefficients.toXML(domChildElem);
    }

    return domNewElem;
}

int TYUserDefinedDirectivity::fromXML(DOM_Element domElement)
{
    TYDirectivity::fromXML(domElement);

    // Reset
    DirectivityTable.clear();

    DOM_Element elemCur;
    QDomNodeList childs = domElement.childNodes();

    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        ConstraintDirectivityElement dirElement;

        if (elemCur.nodeName() == "DirectivityValues")
        {
            dirElement.Theta = TYXMLTools::getElementAttributeToDouble(elemCur, "theta");
            dirElement.Phi = TYXMLTools::getElementAttributeToDouble(elemCur, "phi");
            dirElement.Coefficients.fromXML(elemCur);

            DirectivityTable.push_back(dirElement);
        }
    }

    return 1;
}
void TYUserDefinedDirectivity::LoadFromCsv(std::string fileName)
{
    DirectivityTable.clear(); // cleaning table
    std::ifstream input(fileName, std::ios::in);

    using namespace std;
    using namespace boost;

    typedef tokenizer< escaped_list_separator<char> > Tokenizer;
    string line;
    while ( getline(input, line) )
    {
        deque<double> line_array;
        Tokenizer tok(line);
        BOOST_FOREACH(string str_rec, tok)
        {
            trim(str_rec);
            line_array.push_back(boost::lexical_cast<double>(str_rec));
        }

        DirectivityTable.push_back( readValuesFromCSV(line_array) );
    }

    input.close();
}

ConstraintDirectivityElement TYUserDefinedDirectivity::readValuesFromCSV(std::deque<double>& line_array)
{
    ConstraintDirectivityElement element;
    element.Theta = line_array[0];
    element.Phi = line_array[1];

    double *spectrumValues = element.Coefficients.getTabValReel();
    for (unsigned int i=0; i<TY_SPECTRE_DEFAULT_NB_ELMT; i++)
    {
        spectrumValues[i] = line_array[i+2]; // i+2 cause theta & phi are loaded before spectrum
    }

    return element;
}

// -------------------

TYComputedDirectivity::TYComputedDirectivity( const TYComputedDirectivity& other ) :
                        TYDirectivity( other )
{
    Type = other.Type;
    SpecificSize = other.SpecificSize;
}

DOM_Element TYComputedDirectivity::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYDirectivity::toXML(domElement);

    DOM_Document domDoc = domElement.ownerDocument();

    DOM_Element directivity = domDoc.createElement("DirectivityType");
    domNewElem.appendChild(directivity);
    directivity.setAttribute("Type", intToStr(Type).data());

    DOM_Element specificSize = domDoc.createElement("SpecificSize");
    domNewElem.appendChild(specificSize);
    specificSize.setAttribute("Size", doubleToStrPre(SpecificSize, 3).data());

    return domNewElem;
}

int TYComputedDirectivity::fromXML(DOM_Element domElement)
{
    TYDirectivity::fromXML(domElement);

    DOM_Element elemCur;
    QDomNodeList childs = domElement.childNodes();

    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();
        if (elemCur.nodeName() == "DirectivityType")
        {
            Type = TYXMLTools::getElementAttributeToInt(elemCur, "Type");
        }

        if (elemCur.nodeName() == "SpecificSize")
        {
            SpecificSize = TYXMLTools::getElementAttributeToDouble(elemCur, "Size");
        }
    }

    return 1;
}
