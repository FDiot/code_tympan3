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
 * \file TYProjetGraphic.h
 * \brief Representation graphique d'un projet (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_PROJET_GRAPHIC__
#define __TY_PROJET_GRAPHIC__

#if TY_USE_IHM
//#include <qsignal.h>
#endif // TY_USE_IHM

#include "TYElementGraphic.h"
class TYProjet;


/**
 * \class TYProjetGraphic
 * \brief Classe graphique pour un projet
 */
class TYProjetGraphic : public TYElementGraphic
{
    TY_DECL_METIER_GRAPHIC(TYProjet)

#if TY_USE_IHM
    //QSignal* _pUpdateSignal;
#endif // TY_USE_IHM
    // Methodes
public:
    TYProjetGraphic(TYProjet* pElement);
    virtual ~TYProjetGraphic();

    virtual void update(bool force = false);
    virtual void display(GLenum mode = GL_RENDER);
    virtual void getChilds(TYListPtrTYElementGraphic& childs, bool recursif = true);

#if TY_USE_IHM
    /**
     * Connecte au signal de notification de mise a jour.
     */
    virtual void connectUpdateSignal(QObject* pReceiver, const char* member); //revoir
    /**
     * Deconnecte au signal de notification de mise a jour.
     */
    virtual void disconnectUpdateSignal(QObject* pReceiver, const char* member); //revoir
#endif
};


#endif // __TY_PROJET_GRAPHIC__
