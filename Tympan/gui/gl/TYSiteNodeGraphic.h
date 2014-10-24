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
 * \file TYSiteNodeGraphic.h
 * \brief Representation graphique d'un ensemble de sites (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_SITE_NODE_GRAPHIC__
#define __TY_SITE_NODE_GRAPHIC__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#if TY_USE_IHM
#ifdef Bool
#undef Bool
#endif
//#include <qsignal.h>
#endif // TY_USE_IHM

#include "TYElementGraphic.h"

class TYSiteNode;


/**
 * \class TYSiteNodeGraphic
 * \brief clase graphique pour un ensemble de sites
 */
class TYSiteNodeGraphic : public TYElementGraphic
{
    TY_DECL_METIER_GRAPHIC(TYSiteNode)

#if TY_USE_IHM
    //QSignal* _pUpdateSignal;
#endif // TY_USE_IHM
    // Methodes
public:
    TYSiteNodeGraphic(TYSiteNode* pElement);
    virtual ~TYSiteNodeGraphic();

    virtual void update(bool force = false);
    virtual void display(GLenum mode = GL_RENDER);
    virtual void getChilds(TYListPtrTYElementGraphic& childs, bool recursif = true);
public:
#if TY_USE_IHM
    /**
     * \fn virtual void connectUpdateSignal(QObject * pReceiver, const char* member)
     * \brief Connecte au signal de notification de mise a jour.
     */
    virtual void connectUpdateSignal(QObject* pReceiver, const char* member);

    /**
     * \fn virtual void disconnectUpdateSignal(QObject * pReceiver, const char* member)
     * \bief Deconnecte au signal de notification de mise a jour.
     */
    virtual void disconnectUpdateSignal(QObject* pReceiver, const char* member);
#endif

protected:
    GLuint _texture;
    unsigned char* _imgDataPtr;
    int _imgXSize;
    int _imgYSize;
    QString _sOldTopoFile;
    int _imgBytesPerPixel;
};


#endif // __TY_SITE_NODE_GRAPHIC__
