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



#include "OImageBmp.h"
#include <cassert>
#include <fstream>
#include <cstring>

OImageBmp::OImageBmp() : OImage()
{

}

OImageBmp::~OImageBmp()
{

}

bool OImageBmp::load(const std::string& filename)
{
    // Open file to read
    std::ifstream ifs;
    ifs.open(filename.c_str(), std::ios::in | std::ios::binary);
    if (!ifs)
    {
        return false;
    }

    // Read width and height
    ifs.seekg(18, std::ios::beg);
    ifs.read((char*)&_sizeX, sizeof(unsigned int));
    ifs.read((char*)&_sizeY, sizeof(unsigned int));

    // OImage size in BGR and RGBA
    unsigned int size3 = _sizeX * _sizeY * 3;
    unsigned int size4 = _sizeX * _sizeY * 4;

    // Check planes
    unsigned short planes;
    ifs.read((char*)&planes, sizeof(unsigned short));
    if (planes != 1)
    {
        return false;
    }

    // Check bits per pixel
    ifs.read((char*)&_depth, sizeof(unsigned int));
    if (_depth != 24)
    {
        return false;
    }

    // Allocate memory
    _data = new unsigned char[size4];
    unsigned char* tempData = new unsigned char[size3];

    // Read BGR image
    ifs.seekg(22, std::ios::cur);
    ifs.read((char*)tempData, size3);

    // Convert BGR to RGBA
    unsigned int c1, c2;
    for (c1 = 0, c2 = 0 ; c1 < size4 ; c1 += 4, c2 += 3)
    {
        _data[c1]   = tempData[c2 + 2];
        _data[c1 + 1] = tempData[c2 + 1];
        _data[c1 + 2] = tempData[c2];
        _data[c1 + 3] = 255;
    }

    // Free temporary memory
    delete [] tempData;

    _depth = 32;

    ifs.close();

    return true;
}

void OImageBmp::flipHorizontal()
{
    const unsigned int stride = _depth;
    const unsigned int w = _sizeX;
    const unsigned int h = _sizeY;
    const unsigned int m = h / 2;
    unsigned char* temp = new unsigned char[stride];

    for (unsigned int y = 0 ; y < m ; ++y)
    {
        for (unsigned int x = 0 ; x < w ; ++x)
        {
            unsigned int offset = (y * w + x) * stride;
            unsigned int offsetOpp = ((h - y - 1) * w + x) * stride;

            std::memcpy(temp, _data + offset, stride);
            std::memcpy(_data + offset, _data + offsetOpp, stride);
            std::memcpy(_data + offsetOpp, temp, stride);
        }
    }
    delete [] temp;
}

void OImageBmp::flipVertical()
{
    const unsigned int stride = _depth;
    const unsigned int w = _sizeX;
    const unsigned int h = _sizeY;
    const unsigned int m = w / 2;
    unsigned char* temp = new unsigned char[stride];

    for (unsigned int y = 0 ; y < h ; ++y)
    {
        for (unsigned int x = 0 ; x < m ; ++x)
        {
            unsigned int offset = (y * w + x) * stride;
            unsigned int offsetOpp = (y * w + (w - x - 1)) * stride;

            std::memcpy(temp, _data + offset, stride);
            std::memcpy(_data + offset, _data + offsetOpp, stride);
            std::memcpy(_data + offsetOpp, temp, stride);
        }
    }
    delete [] temp;
}
