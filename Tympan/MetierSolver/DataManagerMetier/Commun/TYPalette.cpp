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
#include "Tympan/GraphicIHM/DataManagerIHM/TYPaletteWidget.h"
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerCore/TYXMLManager.h"

#include <limits>
#include <cmath>

#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/math/special_functions/fpclassify.hpp>

#include <QDebug>

#include "Tympan/MetierSolver/DataManagerCore/TYPreferenceManager.h"
#include "Tympan/Tools/OMessageManager.h"


OPROTOINST(TYPalette);
TY_EXTENSION_INST(TYPalette);

const TYPalette::values_type TYPalette::infinity = std::numeric_limits<values_type>::infinity() ;

TYPalette::TYPalette()
{
    _name = TYNameManager::get()->generateName(getClassName());

    resetToDefault();
}

void TYPalette::resetToDefault()
{
    // Valeurs min et max
    double valueMin = 40.0f;
    double valueMax = 75.0f;

    // NB : Legacy LookupTable for preferences backward compatibility
    OLookupTable  lookupTable;
    lookupTable.resize(9);

    // Couleurs
    lookupTable[0] = OColor(76.0f / 255.0f, 230.0f / 255.0f,          0.0f);
    lookupTable[1] = OColor(85.0f / 255.0f,          1.0f,          0.0f);
    lookupTable[2] = OColor(209.0f / 255.0f,          1.0f, 115.0f / 255.0f);
    lookupTable[3] = OColor(1.0f,          1.0f,  81.0f / 255.0f);
    lookupTable[4] = OColor(1.0f,          1.0f,          0.0f);
    lookupTable[5] = OColor(1.0f, 170.0f / 255.0f,          0.0f);
    lookupTable[6] = OColor(1.0f,  85.0f / 255.0f,          0.0f);
    lookupTable[7] = OColor(227.0f / 255.0f,          0.0f,          0.0f);
    lookupTable[8] = OColor(168.0f / 255.0f,          0.0f,          0.0f);
    resetcolorMapFromColors(valueMin, valueMax, lookupTable);
}


bool TYPalette::resetFromPreferences()
{
#if TY_USE_IHM
    QString pref = "PaletteDefault";
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, pref + "Min"))
    {
        OLookupTable  lookupTable;
        double valueMin = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, pref + "Min");
        double valueMax = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, pref + "Max");
        lookupTable.resize(TYPreferenceManager::getInt(TYDIRPREFERENCEMANAGER, pref + "NbColors"));
        for (size_t i = 0; i < lookupTable.size(); ++i)
        {
            OColor color;
            // TODO load values in an analogous way ?
            TYPreferenceManager::getColor(TYDIRPREFERENCEMANAGER, pref + "Color" + QString(uintToStr(i).c_str()), color.r, color.g, color.b);
            lookupTable[i] = color;
        }
        resetcolorMapFromColors(valueMin, valueMax, lookupTable);
        return true;
    }
    else
    {
        return false;
    }
#else
    return false;
#endif
}


void TYPalette::saveToPreferences()
{
#if TY_USE_IHM
    QString pref = "PaletteDefault";
    TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, pref + "Min", getValueMin());
    TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, pref + "Max", getValueMax());
    TYPreferenceManager::setUInt(TYDIRPREFERENCEMANAGER, pref + "NbColors", getNbColors());

    // For each (value : color) in the color map
    values_type value;
    OColor color;
    unsigned i = 0;
    BOOST_FOREACH(boost::tie(value, color), getColorMap())
    {
        // TODO save values in an analogous way ?
        TYPreferenceManager::setColor(TYDIRPREFERENCEMANAGER, pref + "Color" + QString(uintToStr(i).c_str()), color.r, color.g, color.b);
        ++i;
    }
    assert(i == getNbColors());
#endif
}

void TYPalette::resetcolorMapFromColors(
    values_type valueMin, values_type valueMax, const std::vector<OColor>& lookupTable)
{
    const size_t nbColors = lookupTable.size() ;
    const size_t nbValues = lookupTable.size() - 1 ;
    assert(nbColors > 2 && "A color map needs at least three colors");
    const values_type interval = (valueMax - valueMin) / (values_type)(nbValues - 1);

    _colorMap.clear();
    for (size_t i = 0; i < lookupTable.size(); ++i)
    {
        // the key for the last color is + infinity
        // otherwise we compute a regular scale of nbValues-1 intervals
        const values_type value = (i == nbColors - 1) ? infinity : valueMin + interval * i ;
        _colorMap.insert(_colorMap.end(), std::make_pair(value, lookupTable[i]));
    }
}

void TYPalette::clear()
{
    _colorMap.clear();
    _colorMap.insert(_colorMap.end(), std::make_pair(0.0f, OColor::BLACK));
    _colorMap.insert(_colorMap.end(), std::make_pair(infinity, OColor::WHITE));
}

//XXX Not needed
TYPalette::TYPalette(const TYPalette& other)
{
    *this = other;
}

//XXX Not needed
TYPalette::~TYPalette()
{

}

TYPalette& TYPalette::operator=(const TYPalette& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        _colorMap = other._colorMap;
    }
    return *this;
}

bool TYPalette::operator==(const TYPalette& other) const
{
    if (this == &other)
    {
        return true;
    }
    // This might be not the best way to compare two TYPalette
    if (TYElement::operator !=(other)) { return false; }
    if (_colorMap != other._colorMap) { return false; }

    return true;
}

bool TYPalette::deepCopy(const TYElement* pOther, bool copyId /*= true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }
    TYPalette* pOtherPalette = (TYPalette*) pOther;
    _colorMap = pOtherPalette->_colorMap;
    return true;
}

std::string TYPalette::toString() const
{
    return "TYPalette";
}

bool TYPalette::insertColor(values_type value, const OColor& color)
{

    std::pair<TYPalette::color_map_iter, bool> status =
        _colorMap.insert(std::make_pair(value, color)) ;
    if (!status.second) // Insertion was not possible : value already exists
    {
        status.first->second = color;    // The existing value is associated to the new color
    }
    return status.second;
}

bool TYPalette::moveValue(values_type old_value, values_type new_value)
{
    TYPalette::color_map_iter where = _colorMap.lower_bound(old_value);   // XXX Use find of lower bound ?
    assert(where != _colorMap.end() && "This should never happen as the last value is always supposed to be +inf");
    values_type actual_value =  where->first;
    TYPalette::color_map_iter after(where);
    if (boost::math::isfinite(actual_value)) // is not greater than the upper bound
    {
        ++after;
    }
    else // We are moving the upper bound from a greater old_value
    {
        --where;
    }

    // The new value must lie betwee the (otherwise unchanged) values around old value.
    if (new_value >= after->first)
    {
        return false;
    }
    if (where != _colorMap.begin())
    {
        TYPalette::color_map_iter before(where); --before;
        if (new_value <= before->first)
        {
            return false;
        }
    }
    // Otherwise, we do update the map
    OColor color = where->second;
    _colorMap.erase(where);
    TYPalette::color_map_iter it =  _colorMap.insert(after, std::make_pair(new_value, color)) ;
    return true;
}

bool TYPalette::removeValue(values_type value)
{
    TYPalette::color_map_iter where = _colorMap.find(value);
    if (where == _colorMap.end()) // value was not en exact bound
    {
        return false;
    }
    values_type actual_value =  where->first;
    if (boost::math::isfinite(actual_value) && getNbValues() > 1) // We can not remove the infinite bound nor the last finite one
    {
        _colorMap.erase(where);
        return true;
    }
    else
    {
        return false;
    }
}

DOM_Element TYPalette::toXML(DOM_Element& domElement) /* const */
{
    // Keep basic stuff for pseudo compat / testing XXX
    DOM_Element domNewElem = save_legacy_XML(domElement); // TYElement::toXML(domElement);

    // Creates a new node to save the color map
    DOM_Document domDoc = domElement.ownerDocument();
    DOM_Element colorMapElem = domDoc.createElement("colorMap");

    // For each (value : color) in the color map
    values_type value;
    OColor color;
    unsigned i = 0;
    BOOST_FOREACH(boost::tie(value, color), getColorMap())
    {
        DOM_Element colorMapEntry = domDoc.createElement("colorMapEntry");
        // NB infinity (lest value) will be rendered as "inf"
        colorMapEntry.setAttribute("value", value);
        unsigned colorcode = color.getAsRGBA();
        colorMapEntry.setAttribute("color", colorcode);
        ++i;
        colorMapElem.appendChild(colorMapEntry);
    }
    assert(i == getNbColors());

    domNewElem.appendChild(colorMapElem);
    return domNewElem;
}

DOM_Element TYPalette::save_legacy_XML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);

    // Valeurs min et max
    TYXMLTools::addElementFloatValue(domNewElem, "valueMin", getValueMin());
    TYXMLTools::addElementFloatValue(domNewElem, "valueMax", getValueMax());

    // Creation d'un nouveau noeud pour l'ensemble des couleurs
    DOM_Document domDoc = domElement.ownerDocument();
    DOM_Element lookupTableElem = domDoc.createElement("lookupTable");
    domNewElem.appendChild(lookupTableElem);

    // Ajout du nombre de couleur
    unsigned int nbColors = static_cast<uint32>(_colorMap.size());
    lookupTableElem.setAttribute("nbColors", nbColors);

    // Formatage des donnees
    std::ostringstream oss;
    values_type value;
    OColor color;
    unsigned i = 0;
    BOOST_FOREACH(boost::tie(value, color), getColorMap())
    {
        oss << color.getAsRGBA() << " ";
    }

    DOMString tab(oss.str().c_str());

    // Ajout des donnees
    QDomText data = domDoc.createTextNode(tab);
    lookupTableElem.appendChild(data);

    return domNewElem;
}

int TYPalette::load_legacy_XML(const DOM_Element& domElement)
{
    DOM_Element elemCur;
    QDomNodeList children = domElement.elementsByTagName("lookupTable");
    values_type valueMin, valueMax;
    bool fMinOk = false, fMaxOk = false;
    // NB : Legacy LookupTable for preferences backward compatibility
    OLookupTable  lookupTable;

    int status = 0;
    if (children.length() == 0)
    {
        // no colorMap entry, attempt loading according to legacy format
        status = load_legacy_XML(domElement);
        if (!status)
            OMessageManager::get()->error("No lookupTable element while reading the legacy 'Palette' named %s (id %s).",
                                          this->_name.toLocal8Bit().data(), this->getID().toString().toLocal8Bit().data());
        return status;
    }
    if (children.length() > 1)
    {
        OMessageManager::get()->error("While reading the 'Palette' named %s (id %s) : only one lookupTable element is expected.",
                                      this->_name.toLocal8Bit().data(), this->getID().toString().toLocal8Bit().data());
        return 0;
    }
    assert(children.length() == 1);
    QDomElement lookupTableElement = children.at(0).toElement();
    QString elemName = lookupTableElement.nodeName();
    assert(elemName == "lookupTable");

    fMinOk = TYXMLTools::getElementFloatValue(domElement.firstChildElement("valueMin"), "valueMin", valueMin);
    fMaxOk = TYXMLTools::getElementFloatValue(domElement.firstChildElement("valueMax"), "valueMax", valueMax);
    // On recupere le nombre de couleur
    size_t nbColors = TYXMLTools::getElementAttributeToInt(lookupTableElement, "nbColors");

    // On resize la lookupTable
    lookupTable.resize(nbColors);

    // On recupere les couleurs
    QString colors;
    TYXMLTools::getElementStringValue(lookupTableElement, "lookupTable", colors);
    std::istringstream iss(colors.toAscii().data());

    // On extrait les couleurs de la string
    unsigned int rgba;
    for (unsigned int i = 0; i < nbColors; ++i)
    {
        iss >> rgba;
        lookupTable[i].setAsRGBA(rgba);
    }
    resetcolorMapFromColors(valueMin, valueMax, lookupTable);
    return 1;
}

int TYPalette::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);
    QDomNodeList children = domElement.elementsByTagName("colorMap");
    int status = 0;
    if (children.length() == 0)
    {
        // no colorMap entry, attempt loading according to legacy format
        status = load_legacy_XML(domElement);
        if (!status)
            OMessageManager::get()->error("Invalid legacy format (pre colorMap) while reading the 'Palette' named %s (id %s).",
                                          this->_name.toLocal8Bit().data(), this->getID().toString().toLocal8Bit().data());
        return status;
    }
    if (children.length() > 1)
    {
        OMessageManager::get()->error("Invalid format while reading the 'Palette' named %s (id %s) : only one colorMap element is expected.",
                                      this->_name.toLocal8Bit().data(), this->getID().toString().toLocal8Bit().data());
        return 0;
    }
    assert(children.length() == 1);
    QDomElement colorMapElement = children.at(0).toElement();
    QString elemName = colorMapElement.nodeName();
    assert(elemName == "colorMap");
    children = domElement.elementsByTagName("colorMapEntry"); // XXX colorMapElement

    clear();
    for (unsigned i = 0; i < children.length(); ++i)
    {
        bool ok = false;
        double value;
        OColor color;
        int rgb;
        const QDomElement entryElement = children.item(i).toElement();

        value = TYXMLTools::getElementAttributeToDouble(entryElement, "value", &ok);
        if (!ok)
        {

            OMessageManager::get()->error("Invalid %dth colorMapEntry while reading the 'Palette' named %s (id %s) : expected value field not '%s'",
                                          i, this->_name.toLocal8Bit().data(), this->getID().toString().toLocal8Bit().data(), xml2cstring(entryElement));
            return 0;
        }
        else
        {
            ok = false;
        }
        rgb  = TYXMLTools::getElementAttributeToUnsigned(entryElement, "color", &ok);
        if (!ok)
        {
            QString msg; QTextStream str(&msg);
            entryElement.save(str, 0);
            OMessageManager::get()->error("Invalid %dth colorMapEntry while reading the 'Palette' named %s (id %s) : expected RGB field not '%s'",
                                          i, this->_name.toLocal8Bit().data(), this->getID().toString().toLocal8Bit().data(), msg.toLocal8Bit().data());
            return 0;
        }
        else
        {
            ok = false;
        }
        color.setAsRGBA(static_cast<unsigned>(rgb));
        if (i == 0) // This lower bound needs to be moved, not inserted
        {
            assert(getValueMin() == 0.0 && getInfColor() == OColor::BLACK); // Check cleared state
            ok = moveValue(0, value);
            assert(ok);
            ok = insertColor(value, color);
            assert(!ok); // False status means the value already existed, which is expected here.
        }
        else
        {
            ok = insertColor(value, color);
            if (!ok)
            {
                return 0;
            }
        }
    }
    return 1;
}

TYPalette::values_type TYPalette::normalize(values_type value) const
{
    assert(getNbColors() > 1);
    const double amplitude = getValueMax() - getValueMin();
    assert(amplitude > std::numeric_limits<values_type>::epsilon());
    return (value - getValueMin()) / amplitude ;
}

const OColor& TYPalette::getColorFromPercent(values_type percent) const
{
    // Out of bound values
    if (percent < std::numeric_limits<values_type>::epsilon())
    {
        return getInfColor();
    }
    // Valeur superieur ou egale a 1
    if (percent >= 1.0f)
    {
        return getSupColor();
    }
    return getColorFromValue(denormalize(percent));
}

const OColor& TYPalette::getColorFromValue(values_type value) const
{
    // Look up into the map
    color_map_type::const_iterator it = _colorMap.lower_bound(value);
    assert(it != _colorMap.end() && "This should never happen.");
    return it->second;
}


const OColor& TYPalette::getColorFromIndex(unsigned i) const
{
    assert(i < getNbColors());
    color_map_type::const_iterator it = _colorMap.begin();
    for (; i > 0; i--) // Advance it by i elements
    {
        assert(it != _colorMap.end());
        ++it;
    }
    return it->second;
}

TYPalette::values_type TYPalette::getValueFromIndex(unsigned i) const
{
    assert(i < getNbColors());
    color_map_type::const_iterator it = _colorMap.begin();
    for (; i > 0; i--) // Advance it by i elements
    {
        assert(it != _colorMap.end());
        ++it;
    }
    return it->first;
}

// This function hold code moved from TYPalletteWidget, where it did not belong.
LPTYPalette TYPalette::loadPaletteFromFile(const QString& qFileName)
{
    // Chargement du fichier
    TYXMLManager xmlManager;
    LPTYElementArray elements;

    bool status = xmlManager.load(qFileName, elements);
    if (status)
        if (elements.size() == 1)
        {
            return TYPalette::safeDownCast(elements[0]);
        }
    elements.clear();
    return LPTYPalette(); // Invalid file format
}

// This function hold code moved from TYPalletteWidget, where it did not belong.
bool TYPalette::savePaletteToFile(const QString& qFileName)
{
    TYXMLManager xmlManager;
    xmlManager.createDoc("Tympan", "");
    LPTYPalette pPalette = new TYPalette(*this);
    xmlManager.addElement(pPalette);
    bool status = xmlManager.save(qFileName) == 0;
    return status;

}

void TYPalette::makeLinearPalette()
{
    float hueRange[2], saturationRange[2], valueRange[2];
    getInfColor().getAsHSB(hueRange[0], saturationRange[0], valueRange[0]);
    getSupColor().getAsHSB(hueRange[1], saturationRange[1], valueRange[1]);
    OLookupTable lookupLinear;

    const size_t nbColors = getNbColors();
    TYColorManager::getLinearColorTable(nbColors, hueRange, saturationRange, valueRange, lookupLinear);

    resetcolorMapFromColors(getValueMin(), getValueMax(), lookupLinear);
}
