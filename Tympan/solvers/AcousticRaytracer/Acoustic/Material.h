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

#include "Tympan/solvers/AcousticRaytracer/Base.h"

class Material : public Base
{
public:
	/// Default constructor
    Material() : Base() { name = "unknown material"; isNatural = true;};
    /// Constructor by giving a name to the material
    Material(std::string _name) : Base() { name = _name; isNatural = true;}
    /// Copy constructor
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
    unsigned int id; 	//!< Identification set by the MaterialManager
    bool isNatural;		//!< Flag to define a natural material

    //#ifdef USE_QT
    //  unsigned int r;
    //  unsigned int g;
    //  unsigned int b;
    //#endif
};

/**
 * \brief A manager class for Material
 */
class MaterialManager
{

public:
	/// Constructor
    MaterialManager() : counterKey(0) { };
    /// Destructor
    ~MaterialManager();
    /// Register a new material into the list
    unsigned int registerNewMaterial(Material* m);
    /// Return a material from the list by its index
    Material* requestMaterial(unsigned int key);
    /// Print the materials list
    void print();

protected:
    std::map<unsigned int, Material*> materials;	//!< Pointer list to materials
    unsigned int counterKey;						//!< Counter of materials into the list
};

#endif
