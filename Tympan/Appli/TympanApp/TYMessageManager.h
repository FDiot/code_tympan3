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
 * \file TYMessageManager.h
 * \brief utilitaire pour la gestion des messages dans Tympan (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */


#ifndef __TY_MESSAGE_MANAGER__
#define __TY_MESSAGE_MANAGER__


#include "Tympan/Tools/OMessageManager.h"


/**
 * \class TYMessageManager
 * \brief Classe utilitaire pour la gestion des messages dans Tympan.
 *
 * Cette classe est derivee a OMessageManager afin de surcharger
 * format() pour gerer la localisation (ou internationalisation)
 * et output() pour rediriger les messages vers l'IHM.
 *
 */
class TYMessageManager : public OMessageManager
{
    // Methods
public:
    /**
     * Constructeur.
     */
    TYMessageManager();
    /**
     * Destructeur.
     */
    virtual  ~TYMessageManager();

    virtual void format(int level, const char* message, ...);
    virtual void output(const char* message, int level);
};

#endif // __TY_MESSAGE_MANAGER__
