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
 * \file TYReseauTransportGraphic.h
 * \brief Representation graphique d'un reseau electrique (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_RESEAU_TRANSPORT_GRAPHIC__
#define __TY_RESEAU_TRANSPORT_GRAPHIC__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "TYAcousticLineGraphic.h"
class TYReseauTransport;


/**
 * \class TYReseauTransportGraphic
 * \brief classe graphique d'un reseau electrique
 */
class TYReseauTransportGraphic : public TYAcousticLineGraphic
{
    TY_DECL_METIER_GRAPHIC(TYReseauTransport)

    // Methodes
public:
    TYReseauTransportGraphic(TYReseauTransport* pElement);
    virtual ~TYReseauTransportGraphic() { }
};


#endif // __TY_RESEAU_TRANSPORT_GRAPHIC__
