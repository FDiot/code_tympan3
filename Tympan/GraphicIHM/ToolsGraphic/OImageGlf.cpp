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



#include "OImageGlf.h"
#include <cassert>
#include <fstream>

OImageGlf::OImageGlf() : OImage(), _start(0), _end(0)
{

}

OImageGlf::~OImageGlf()
{

}

bool OImageGlf::load(const std::string& filename)
{
    unsigned int i;

    // Ouvre le fichier a lire
    std::ifstream ifs;
    ifs.open(filename.c_str(), std::ios::in | std::ios::binary);
    if (!ifs)
    {
        return false;
    }

    // Largeur et hauteur de la texture
    ifs.seekg(4, std::ios::beg);
    ifs.read((char*)&_sizeX, sizeof(int));
    ifs.read((char*)&_sizeY, sizeof(int));

    // Nombre de caractere.
    ifs.read((char*)&_start, sizeof(int));
    ifs.read((char*)&_end, sizeof(int));
    unsigned int nbChar = _end - _start + 1;

    // Caracteres.
    ifs.seekg(4, std::ios::cur);
    for (i = 0; i < nbChar; ++i)
    {
        OGLFontChar c;
        ifs.read((char*)&c._dx, sizeof(float));
        ifs.read((char*)&c._dy, sizeof(float));
        ifs.read((char*)&c._tx0, sizeof(float));
        ifs.read((char*)&c._ty0, sizeof(float));
        ifs.read((char*)&c._tx1, sizeof(float));
        ifs.read((char*)&c._ty1, sizeof(float));

        _char.push_back(c);
    }

    // Donnees
    unsigned int size = _sizeX * _sizeY * 2;
    _data = new unsigned char[size];
    ifs.read((char*)_data, size);

    // Profondeur
    _depth = 16;

    ifs.close();

    return true;
}

const OImageGlf::OGLFontChar& OImageGlf::getChar(char c) const
{
    return _char[(unsigned int)c - _start];
}

bool OImageGlf::isCharValid(char c) const
{
    return ((unsigned int)c >= _start && (unsigned int)c <= _end);
}
