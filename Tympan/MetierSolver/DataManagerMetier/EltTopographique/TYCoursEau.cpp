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
#include "Tympan/GraphicIHM/DataManagerIHM/TYCoursEauWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYCoursEauGraphic.h"
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/core/logging.h"


TY_EXTENSION_INST(TYCoursEau);
TY_EXT_GRAPHIC_INST(TYCoursEau);

TYCoursEau::TYCoursEau()
{
    _name = TYNameManager::get()->generateName(getClassName());

    // Couleur par default
    float r = 33.0f, g = 147.0f, b = 217.0f;

#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "TYCoursEauGraphicColorR"))
    {
        TYPreferenceManager::getColor(TYDIRPREFERENCEMANAGER, "TYCoursEauGraphicColor", r, g, b);
    }
    else
    {
        TYPreferenceManager::setColor(TYDIRPREFERENCEMANAGER, "TYCoursEauGraphicColor", r, g, b);
    }
#endif
    OColor color;
    color.r = r / 255;
    color.g = g / 255;
    color.b = b / 255;

    setColor(color);
}

TYCoursEau::TYCoursEau(const TYCoursEau& other)
{
    *this = other;
}

TYCoursEau::~TYCoursEau()
{
}

TYCoursEau& TYCoursEau::operator=(const TYCoursEau& other)
{
    if (this != &other)
    {
        TYAcousticLine::operator =(other);
    }
    return *this;
}

bool TYCoursEau::operator==(const TYCoursEau& other) const
{
    if (this != &other)
    {
        if (TYAcousticLine::operator !=(other)) { return false; }
    }
    return true;
}

bool TYCoursEau::operator!=(const TYCoursEau& other) const
{
    return !operator==(other);
}

std::string TYCoursEau::toString() const
{
    return "TYCoursEau";
}

DOM_Element TYCoursEau::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYAcousticLine::toXML(domElement);
    return domNewElem;
}

int TYCoursEau::fromXML(DOM_Element domElement)
{
    TYAcousticLine::fromXML(domElement);

    return 1;
}
