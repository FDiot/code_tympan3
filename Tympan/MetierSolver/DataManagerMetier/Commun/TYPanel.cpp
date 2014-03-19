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


#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

OPROTOINST(TYPanel);

TYPanel::TYPanel()
{
    _name = TYNameManager::get()->generateName(getClassName());

    _firstPos = "";
    _secondPos = "";
    _value = "";
}

TYPanel::TYPanel(const TYPanel& other)
{
    *this = other;
}

TYPanel::~TYPanel()
{

}

TYPanel& TYPanel::operator=(const TYPanel& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        _firstPos = other._firstPos;
        _secondPos = other._secondPos;
        _value = other._value;
    }
    return *this;
}

bool TYPanel::operator==(const TYPanel& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        if (_firstPos != other._firstPos) { return false; }
        if (_secondPos != other._secondPos) { return false; }
        if (_value != other._value) { return false; }
    }
    return true;
}

bool TYPanel::operator!=(const TYPanel& other) const
{
    return !operator==(other);
}

bool TYPanel::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    TYPanel* pOtherPanel = (TYPanel*) pOther;

    _firstPos = pOtherPanel->_firstPos;
    _secondPos = pOtherPanel->_secondPos;
    _value = pOtherPanel->_value;

    return true;
}

std::string TYPanel::toString() const
{
    return "TYPanel";
}
