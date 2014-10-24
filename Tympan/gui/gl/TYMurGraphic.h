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
 * \file TYMurGraphic.h
 * \brief Representation graphique d'un mur (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_MUR_GRAPHIC__
#define __TY_MUR_GRAPHIC__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "TYAcousticRectangleNodeGraphic.h"
class TYMur;


/**
 * \class TYMurGraphic
 * \brief classe graphique pour un mur
 */
class TYMurGraphic : public TYAcousticRectangleNodeGraphic
{
    TY_DECL_METIER_GRAPHIC(TYMur)

    // Methodes
public:
    TYMurGraphic(TYMur* pElement);
    virtual ~TYMurGraphic() { }
};


#endif // __TY_MUR_GRAPHIC__
