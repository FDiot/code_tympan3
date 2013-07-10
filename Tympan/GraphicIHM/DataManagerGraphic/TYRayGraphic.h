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

#ifndef __TY_RAY_GRAPHIC__
#define __TY_RAY_GRAPHIC__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "TYElementGraphic.h"

class TYRay;
class TYRayEvent;

/**
 * Classe graphique pour representer un rayon
 * utilise par le calcul.
 *
 * @author Projet_Tympan
 */
class TYRayGraphic : public TYElementGraphic
{
    TY_DECL_METIER_GRAPHIC(TYRay)

    // Methodes
public:
    TYRayGraphic(TYRay* pElement);
    virtual ~TYRayGraphic();

    virtual void update(bool force = false);
    virtual void display(GLenum mode = GL_RENDER);
    virtual void computeBoundingBox();


    // Membres
public:
    ///Indique si toutes les instances sont visibles.
    static bool _gVisible;

protected:
    void getRayEventColor(TYRayEvent& e, float& r, float& g, float& b);

    int _lastRayEventType;                  /*!< Type du dernier evenement du rayon affiche */
    TYPoint _lastRayEventPos;               /*!< Position du dernier evenement du rayon affiche */
    int _repeatRayEventCount;               /*!< Compteur de repetition d'un meme type d'evenement */
};


///Smart Pointer sur TYRayGraphic.
typedef SmartPtr<TYRayGraphic> LPTYRayGraphic;


#endif // __TY_RAY_GRAPHIC__
