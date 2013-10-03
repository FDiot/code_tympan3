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
 * \file TYAltimetrieGraphic.h
 * \brief Representation graphique de l'altimetrie (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_ALTIMETRIE_GRAPHIC__
#define __TY_ALTIMETRIE_GRAPHIC__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Tympan/GraphicIHM/ToolsGraphic/OGLTexture2D.h"
#include "TYElementGraphic.h"
#include "TYPolygonGraphic.h"
#include <QImage>

class TYAltimetrie;

/**
 * \class TYAltimetrieGraphic
 * \brief classe graphique pour une altimetrie
 */
class TYAltimetrieGraphic : public TYElementGraphic
{
    TY_DECL_METIER_GRAPHIC(TYAltimetrie)

    // Methodes
public:
    TYAltimetrieGraphic(TYAltimetrie* pElement);
    virtual ~TYAltimetrieGraphic();

    virtual void update(bool force = false);
    virtual void display(GLenum mode = GL_RENDER);
    virtual void getChilds(TYListPtrTYElementGraphic& childs, bool recursif = true);
    virtual void computeBoundingBox();

    void setBackgroundImage(QString sTopoFileName, int semiX, int semiY, TYPoint ptPosition, OVector3D bgImageOrientation);
    void unsetBackgroundImage();

protected:
    void bindTexture();
    OLookupTable _oColorMap;
    LPOGLTexture2D _pTex;
    bool _isBgImage;
    int _semiX;
    int _semiY;
    OVector3D _bgImageOrientation;
    TYPoint _imagePosition;
    GLuint _theTexture;
    double _angle;
    QString _sOldTopoFileName;
    QString _sTopoFileName;
    unsigned char* _imgDataPtr;
    int _imgXSize;
    int _imgYSize;
    int _imgBytesPerPixel;
};


#endif // __TY_ALTIMETRIE_GRAPHIC__
