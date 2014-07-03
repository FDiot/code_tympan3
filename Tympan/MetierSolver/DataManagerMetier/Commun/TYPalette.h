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
 *
 *
 */

#ifndef __TY_PALETTE__
#define __TY_PALETTE__

#include <map>

#include "Tympan/Tools/OColor.h"
#include "Tympan/Tools/TYColorManager.h"
#include "Tympan/MetierSolver/DataManagerCore/TYElement.h"

/**
 * \file TYPalette.h
 * \class TYPalette
 * \brief Classe de definition d'une palette.
 * \version v 1.0
 * \author Projet_Tympan
 * \date 2008/04/17
 *
 */
class TYPalette : public TYElement
{
    OPROTODECL(TYPalette)
    TY_EXTENSION_DECL_ONLY(TYPalette)
    TY_EXT_GRAPHIC_DECL_ONLY(TYPalette)

    // Methodes
public:

    /// The real number type used to store values (typically double or float)
    typedef float values_type;

    typedef std::map<values_type, OColor> color_map_type;
    typedef color_map_type::iterator color_map_iter;
    typedef color_map_type::const_iterator color_map_const_iter;
    typedef color_map_type::value_type color_map_item;
    /**
     * \brief A TYPalette represents a mapping from values to colors
     */
    TYPalette();

    /**
     * \brief Constructeur par copie.
     *  Constructeur par copie de la classe TYPalette
     */
    TYPalette(const TYPalette& other);

    /**
     * \brief Destructeur.
     *  Destructeur de la classe TYPalette
     */
    virtual ~TYPalette();


    TYPalette& operator=(const TYPalette& other);


    bool operator==(const TYPalette& other) const;

    bool operator!=(const TYPalette& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement) /* const */;

    virtual int fromXML(DOM_Element domElement);

    void clear();

    static const values_type infinity;

    /**
     * \brief Get de la valeur min
     * \return _valueMin
     */
    values_type getValueMin() const { assert(getNbColors() >= 2); return _colorMap.begin()->first; }

    /**
     * \brief Get de la valeur max
     * \return _valueMax
     */
    values_type getValueMax() const { assert(getNbColors() >= 2); return (++_colorMap.rbegin())->first; }

    /**
     * \brief Getter for the number of colors
     * \return the number of colors (i.e. number of values plus one)
     */
    size_t getNbColors() const { return _colorMap.size(); }

    /**
     * \brief Getter for the number of values
     * \return the number of values (i.e. number of colors minus one)
     */
    size_t getNbValues() const { return _colorMap.size() - 1 ; }

    /**
     * \brief Get de la couleur pour la borne inf
     * \return _colorMap.front()
     */
    const OColor& getInfColor() const { assert(getNbColors() >= 2); return _colorMap.begin()->second; }

    /**
     * \brief Set de la couleur pour la borne inf
     */
    void setInfColor(const OColor& color) { assert(getNbColors() >= 2); _colorMap.begin()->second = color; }

    /**
     * \brief Get/Set de la couleur pour la borne sup
     * \return _colorMap.back()
     */
    const OColor& getSupColor() const { assert(_colorMap.size() >= 2); return _colorMap.rbegin()->second; }

    /**
     * \brief Set de la couleur pour la borne sup
     */
    void setSupColor(const OColor& color) { assert(_colorMap.size() >= 2); return _colorMap.rbegin()->second = color; }

    /**
     * \brief Insert a new color or update an existing one
     *
     * \param value the upper-bound to insert or update
     * \param color values up to \a value will be associated to
     * \return true when a new value has been inserted, false when the color for an existing one has been updated
     */
    bool insertColor(values_type value, const OColor& color);

    /**
     * \brief Moves an existing bound,
     *
     * The new value must lie within the interval occupied by the old one.
     * You can move the upper bound by giving a greater value as well.
     *
     * \return whether the move was successful (otherwise the palette is not modified)
     */
    bool moveValue(values_type old_value, values_type new_value);

    /**
     * \brief Removes an existing bound
     *
     * \return whether the removal was successful (otherwise the palette is not modified)
     */
    bool removeValue(values_type value);


    /**
     * \brief Reset the colormap as a linear scale with the given colors between minValue and maxValue
     */
    void resetcolorMapFromColors(values_type valueMin, values_type valueMax, const std::vector<OColor>& lookupTable);

    /**
     * \brief Reset the colormap to a hard-coded default
     */
    void resetToDefault();

    /**
     * \brief Reset the colormap from the preferences XXX
     */
    bool resetFromPreferences();

    /**
     * \brief Saves the colormap to the preferences XXX
     */
    void saveToPreferences();

    /**
     * \brief Get de la lookup table
     * \return _colorMap
     */
    // OLookupTable& getLookupTable() { return _colorMap; } // XXX update callers
    const color_map_type& getColorMap() const { return _colorMap; } // Should be replace be an smart range

    /**
     * \brief Set de la lookup table
     */
    // void setColors(const OLookupTable& lookupTable) { _colorMap = lookupTable; } // XXX update callers

    /**
     * \brief Set de la couleur pour un pourcentage
     */
    //void setColorFromPercent(values_type percent, const OColor& color); //XXX merge into setColorFromValue

    /**
     * \brief Get de la couleur pour une valeur
     */
    const OColor& getColorFromValue(values_type value) const;

    /**
     * \brief Get de la couleur par index
     */
    const OColor& getColorFromIndex(unsigned i) const;

    /**
     * \brief Get de la valeur par index
     */
    TYPalette::values_type getValueFromIndex(unsigned i) const;


    /**
     * \brief Set de la couleur pour une valeur
     */
    //void setColorFromValue(values_type value, const OColor& color); // Unused : to be deleted ?

    /**
     * \brief Denormalize a value, ie convert a percentage to a value between min and max
     */
    values_type denormalize(values_type percent) const
    { return getValueMin() + (getValueMax() - getValueMin()) * percent ;}

    /**
     * \brief Normalize a value, ie convert a value between min and max to a percentage
     */
    values_type normalize(values_type value) const;

    /**
     * \brief Create a new TYPalette from an XML file Holding only one Palette
     * \return a Smart Pointer to the Palette newly created or NULL in case of failure
     */
    static LPTYPalette loadPaletteFromFile(const QString& qFileName);

    /**
     * \brief Save the Palette into a file with the name given
     * \return if the operation was succesful
     */
    bool savePaletteToFile(const QString& qFileName);

    /**
     * \brief Make a linear palette from a min/max couple
     */
    void makeLinearPalette();

protected:
    color_map_type _colorMap;

    /**
     * \brief implement loading legacy (pre v4.6) XML format for TYPalettes
     */
    int load_legacy_XML(const DOM_Element& domElement);

    /**
     * \brief implement loading legacy (pre v4.6) XML format for TYPalettes
     */
    DOM_Element save_legacy_XML(DOM_Element& domElement);
};

#endif // __TY_PALETTE__
