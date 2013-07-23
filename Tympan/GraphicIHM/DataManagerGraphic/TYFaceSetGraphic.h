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
 * \file TYFaceSetGraphic.h
 * \brief Representation d'un ensemble de faces (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_FACESET_GRAPHIC__
#define __TY_FACESET_GRAPHIC__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "TYElementGraphic.h"



/**
 * \class TYFaceSetGraphic
 * \brief classe graphique pour un ensemble de faces
 */
class TYFaceSetGraphic : public TYElementGraphic
{
    TY_DECL_METIER_GRAPHIC(TYFaceSet)

    // Methodes
public:
    TYFaceSetGraphic(TYFaceSet* pElement);
    virtual ~TYFaceSetGraphic();

    virtual void display(GLenum mode = GL_RENDER);
    virtual void update(bool force = false);

    virtual void computeBoundingBox();

    // Membres
protected:
};


///Smart Pointer sur TYFaceSetGraphic.
typedef SmartPtr<TYFaceSetGraphic> LPTYFaceSetGraphic;


#endif // __TY_FACESET_GRAPHIC__
