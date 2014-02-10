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
 */


#include "TYImageManager.h"
#include "Tympan/Tools/OLocalizator.h"

#include "OImageBmp.h"
#include "OImageGlf.h"

#include <sstream>

// Initialise a NULL.
LPTYImageManager TYImageManager::_pInstance = 0;

TYImageManager::TYImageManager()
{

}

TYImageManager::~TYImageManager()
{
    for (std::map<std::string, OImage*>::iterator it = _imageMap.begin(); it != _imageMap.end(); ++it)
    {
        delete it->second;
    }
    _imageMap.clear();
}

bool TYImageManager::setAsSingleton()
{
    bool ret = _pInstance ? true : false;
    _pInstance = this;
    return ret;
}

LPTYImageManager TYImageManager::get()
{
    if (!_pInstance)
    {
        _pInstance = new TYImageManager();
    }

    return _pInstance;
}

bool TYImageManager::loadImages()
{
    // Chargement des images pour les terrains.
    for (unsigned int i = 0 ; i < 10 ; ++i)
    {
        std::ostringstream oss;
        oss << "id_terrain_" << i;

        if (!loadImage(OLocalizator::getPicture("TYTerrainGraphic", oss.str().c_str()).toAscii().data()))
        {
            return false;
        }
    }

    // Chargement de la police de caracteres pour les barres de legende.
    if (!loadImage(OLocalizator::getPicture("TYPaletteGraphic", "id_font").toAscii().data()))
    {
        return false;
    }

    return true;
}

bool TYImageManager::loadImage(const char* filename)
{
    // On verifie si l'image existe deja
    std::map<std::string, OImage*>::iterator it;
    it = _imageMap.find(filename);
    if (it != _imageMap.end())
    {
        return true;
    }

    // On recupere l'extension
    std::string str(filename);
    size_t pos = str.rfind('.');
    if (pos == std::string::npos)
    {
        return false;
    }
    std::string ext(str, pos + 1, str.size() - pos);

    // On cree une nouvelle image
    OImage* im;
    if (ext == "bmp")
    {
        im = new OImageBmp();
    }
    else if (ext == "glf")
    {
        im = new OImageGlf();
    }
    else
    {
        return false;
    }

    // On charge l'image
    if (!im->load(filename))
    {
        delete im;
        return false;
    }

    // On l'ajoute a la map
    _imageMap[filename] = im;

    return true;
}

OImage* TYImageManager::getImage(const char* filename)
{
    // On verifie si l'image existe deja
    std::map<std::string, OImage*>::iterator it;
    it = _imageMap.find(filename);
    if (it != _imageMap.end())
    {
        return it->second;
    }

    // On recupere l'extension
    std::string str(filename);
    size_t pos = str.rfind('.');
    if (pos == std::string::npos)
    {
        return false;
    }
    std::string ext(str, pos + 1, str.size() - pos);

    // On cree une nouvelle image
    OImage* im;
    if (ext == "bmp")
    {
        im = new OImageBmp();
    }
    else if (ext == "glf")
    {
        im = new OImageGlf();
    }

    // On charge l'image
    if (!im->load(filename))
    {
        delete im;
        return 0;
    }

    // Add it to the map
    _imageMap[filename] = im;
    return im;
}
