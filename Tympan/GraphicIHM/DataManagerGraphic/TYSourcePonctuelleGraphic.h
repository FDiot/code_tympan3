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
 * \file TYSourcePonctuelleGraphic.h
 * \brief Representation graphique d'une source ponctuelle (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_SOURCEPONCTUELLE_GRAPHIC__
#define __TY_SOURCEPONCTUELLE_GRAPHIC__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class TYSourcePonctuelle;

#include "TYElementGraphic.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYPoint.h"

/**
 * \class  TYSourcePonctuelleGraphic
 * \brief classe graphique pour une source ponctuelle
 */
class TYSourcePonctuelleGraphic : public TYElementGraphic
{
    TY_DECL_METIER_GRAPHIC(TYSourcePonctuelle)

    // Methodes
public:
    TYSourcePonctuelleGraphic(TYSourcePonctuelle* pElement);
    virtual ~TYSourcePonctuelleGraphic();

    /**
     * \fn virtual void update(bool)
     * \brief Mise a jour
     */
    virtual void update(bool force = false);
    virtual void display(GLenum mode = GL_RENDER);

    /**
     * \fn  virtual void computeBoundingBox()
     * \brief calcul de la boite englobante
     * */
    virtual void computeBoundingBox();

    // Membres
protected:
    ///Taille de la source.
    float _size;

private:
    void drawCube(TYPoint* pPt) const;
    void drawPyramid(TYPoint* pPt) const;
    void drawSphere(TYPoint* pPt) const;
    void drawStar(TYPoint* pPt) const;
    void setSize();
};


#endif // __TY_SOURCEPONCTUELLE_GRAPHIC__
