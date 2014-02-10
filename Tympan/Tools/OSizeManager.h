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

#ifndef __O_SIZEMANAGER__
#define __O_SIZEMANAGER__

#include "OSmartPtr.h"

// Smart pointer sur OSizeManager.
class OSizeManager;
typedef SmartPtr<OSizeManager> LPOSizeManager;

/**
 * Classe de type Singleton permettant d'avoir un acces
 * a la taille d'une fenetre.
 * Cette classe permet de contourner le probleme du manque d'acces
 * aux donnees de la fenetre notamment pour OpenGL.
 * Il serait bien que les donnees relatives a la taille soit, par la suite,
 * plus accessible.
 *
 * @author Projet_Tympan
 *
 */
class OSizeManager : public IRefCount
{
public:
    /**
     * Destructeur.
     */
    ~OSizeManager();

    /**
     * Retourne l'instance singleton.
     */
    static LPOSizeManager get();

    /**
     * Fixe la largeur de la fenetre
     */
    inline void setWidth(unsigned int width) { _width = width; }

    /**
     * Fixe la hauteur de la fenetre
     */
    inline void setHeight(unsigned int height) { _height = height; }

    /**
     * Retourne la largeur de la fenetre
     */
    inline unsigned int getWidth() const { return _width; }

    /**
     * Retourne la hauteur de la fenetre
     */
    inline unsigned int getHeight() const { return _height; }

private:
    // Instance unique.
    static LPOSizeManager _pInstance;

    /**
     * Constructeur par default.
     */
    OSizeManager();

    // Copy prohibited
    OSizeManager(const OSizeManager&);
    OSizeManager& operator=(const OSizeManager&);

    // Largeur
    unsigned int _width;

    // Hauteur
    unsigned int _height;
};


#endif // __O_SIZEMANAGER__
