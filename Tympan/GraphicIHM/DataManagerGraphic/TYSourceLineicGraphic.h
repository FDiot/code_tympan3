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
 * \file TYSourceLineicGraphic.h
 * \brief Representation graphique d'une source lineique (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_SOURCE_LINEIC_GRAPHIC__
#define __TY_SOURCE_LINEIC_GRAPHIC__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "TYElementGraphic.h"
class TYSourceLineic;


/**
 * \class TYSourceLineicGraphic
 * \brief classe graphique pour une source lineique
 */
class TYSourceLineicGraphic : public TYElementGraphic
{
    TY_DECL_METIER_GRAPHIC(TYSourceLineic)

    // Methodes
public:
    TYSourceLineicGraphic(TYSourceLineic* pElement);
    virtual ~TYSourceLineicGraphic() { }

    /**
     * \fn virtual void update(bool)
     * \brief Mise a jour
     */
    virtual void update(bool force = false);

    virtual void display(GLenum mode = GL_RENDER);
    virtual void getChilds(TYListPtrTYElementGraphic& childs, bool recursif = true);
    /**
     * \fn  virtual void computeBoundingBox()
     * \brief calcul de la boite englobante
     * */
    virtual void computeBoundingBox();

public:
    ///Indique si cette instance est visible.
    bool _srcVisible;
    ///Indique si toutes les instances sont visibles.
    static bool _gVisible;
};


#endif // __TY_SOURCE_LINEIC_GRAPHIC__
