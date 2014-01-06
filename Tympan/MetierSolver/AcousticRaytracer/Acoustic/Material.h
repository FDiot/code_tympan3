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
 
#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>
#include <map>
#include <iostream>
#include <sstream>

#include "Tympan/MetierSolver/AcousticRaytracer/Base.h"

class Material : public Base
{
public:
    Material() : Base() { name = "unknown material"; isNatural = true;};
    Material(std::string _name) : Base() { name = _name; isNatural = true;}

    Material(const Material& other)
    {
        name = std::string(other.name);
        isNatural = other.isNatural;
        //#ifdef USE_QT
        //      r = other.r;
        //      g = other.g;
        //      b = other.b;
        //#endif
    }
    virtual ~Material() {}


public:
    unsigned int id; //Set by the manager
    bool isNatural;

    //#ifdef USE_QT
    //  unsigned int r;
    //  unsigned int g;
    //  unsigned int b;
    //#endif
};

class MaterialManager
{

public:
    MaterialManager() : counterKey(0) { };

    ~MaterialManager();

    unsigned int registerNewMaterial(Material* m);
    Material* requestMaterial(unsigned int key);

    void print();

protected:
    std::map<unsigned int, Material*> materials;
    unsigned int counterKey;
};

#endif