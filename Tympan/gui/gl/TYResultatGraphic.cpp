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
 * \file TYResultatGraphic.cpp
 * \brief Representation graphique des resultats
 *
 *
 */




#include "Tympan/models/business/TYResultat.h"


#include "TYResultatGraphic.h"

TYResultatGraphic::TYResultatGraphic(TYResultat* pElement) :
    TYElementGraphic(pElement)
{
}

TYResultatGraphic::~TYResultatGraphic()
{
}

void TYResultatGraphic::update(bool force /*=false*/)
{
    TYElementGraphic::update(force);
}

void TYResultatGraphic::display(GLenum mode /*= GL_RENDER*/)
{
}
