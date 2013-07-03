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
 * \file TYPolygonGraphic.h
 * \brief Representation graphique d'un polygone (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 */

#ifndef __TY_POLYGON_GRAPHIC__
#define __TY_POLYGON_GRAPHIC__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TYElementGraphic.h"

/**
 * \class TYPolygonGraphic
 * \brief classe graphique pour un polygone
 */
class TYPolygonGraphic : public TYElementGraphic
{
    TY_DECL_METIER_GRAPHIC(TYPolygon)

    // Methodes
public:
    TYPolygonGraphic(TYPolygon* pElement);
    virtual ~TYPolygonGraphic();

    virtual void display(GLenum mode = GL_RENDER);
    virtual void update(bool force = false);

    void setAltimetrieColor(double color0[3], double color1[3], double color2[3]);
    virtual void computeBoundingBox();
    void setTextureBg(int semiXBg, int semiYBg, TYPoint bgImagePosition, OVector3D bgImageOrientation) { _textureBg = true; _semiXBg = semiXBg; _semiYBg = semiYBg; _bgImagePosition = bgImagePosition; _bgImageOrientation = bgImageOrientation;}
    void unsetTextureBg() { _textureBg = false; }

    // Membres
protected:

    ///Indique si ce polygone fait partie de l'altimetrie.
    bool _altimetrie;
    ///Pour les polygones de l'altimetrie chaque sommet a une couleur differente.
    double _color0[3];
    double _color1[3];
    double _color2[3];
    //Pour gerer l'affichage correcte des polygones concaves:
    GLUtesselator*  _gLUtesselator;
    bool _textureBg;
    TYPoint _bgImagePosition;
    int _semiXBg;
    int _semiYBg;
    OVector3D _bgImageOrientation;
};


///Smart Pointer sur TYPolygonGraphic.
typedef SmartPtr<TYPolygonGraphic> LPTYPolygonGraphic;


#endif // __TY_POLYGON_GRAPHIC__
