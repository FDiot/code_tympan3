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

/**
 * \file TYPickingTable.cpp
 * \brief Gestion de la table de correspondance indice/element pour le picking
 *
 *
 */








#include "TYPickingTable.h"

TYTabPtrElement TYPickingTable::_elements;
int TYPickingTable::_index;

TYPickingTable::TYPickingTable()
{
    _index = -1;
}

TYPickingTable::~TYPickingTable()
{
    _elements.clear();
}

void TYPickingTable::addElement(TYElement* pElt)
{
    _elements.push_back(pElt);
    _index++;
}


TYElement* TYPickingTable::getElement(int index)
{
    TYElement* pElt = NULL;

    if (index < (int)_elements.size())
    {
        pElt = _elements[index];
    }

    return pElt;
}

void TYPickingTable::purgeElements()
{
    _elements.clear();
    _index = -1;
}

