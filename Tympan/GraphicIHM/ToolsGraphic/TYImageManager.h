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

#ifndef __TY_IMAGEMANAGER__
#define __TY_IMAGEMANAGER__

/**
 * This warning is generated when compiling in debug mode.
 * Debug symbols cannot be longer than 255 but when using templates
 * it is usual to have debug symbols longer than 255.
 * You may ignore this warning.
 */
#ifdef WIN32
#pragma warning(disable: 4786)
#endif // WIN32

#include "Tympan/Tools/OSmartPtr.h"

#include <map>
#include <string>

class OImage;
class TYImageManager;

// Smart pointer sur TYImageManager.
typedef SmartPtr<TYImageManager> LPTYImageManager;

/**
 * Gestionnaire d'image.
 */
class TYImageManager : public IRefCount
{
public:
    /**
     * Constructeur par default.
     */
    TYImageManager();

    /**
     * Destructeur.
     */
    ~TYImageManager();

    /**
     * Definit cette instance comme singleton.
     * Attention : cette instance doit absolument avoir ete creee
     * sur la heap (operateur new).
     *
     * @return Indique si un singleton etait deja defini ou pas.
     */
    bool setAsSingleton();

    /**
     * Retourne l'instance singleton.
     */
    static LPTYImageManager get();

    /**
     * Charge les images requises.
     * Retourne TRUE si toutes les textures sont chargees, FALSE sinon.
     */
    bool loadImages();

    /**
     * Charge l'image filename
     */
    bool loadImage(const char* filename);

    /**
     * Renvoi l'image filename.
     */
    OImage* getImage(const char* filename);

private:
    std::map<std::string, OImage*> _imageMap;

    // Instance unique.
    static LPTYImageManager _pInstance;
};

#endif // __TY_IMAGEMANAGER__
