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
 * \file TYMachineGraphic.h
 * \brief Representation graphique d'une machine (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_MACHINE_GRAPHIC__
#define __TY_MACHINE_GRAPHIC__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "TYAcousticVolumeNodeGraphic.h"
class TYMachine;


/**
 * \class TYMachineGraphic
 * \brief classe graphique pour une machine
 */
class TYMachineGraphic : public TYAcousticVolumeNodeGraphic
{
    TY_DECL_METIER_GRAPHIC(TYMachine)

    // Methodes
public:
    TYMachineGraphic(TYMachine* pElement);
    virtual ~TYMachineGraphic() { }
};


#endif // __TY_MACHINE_GRAPHIC__
