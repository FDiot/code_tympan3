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
 
#include "Material.h"

MaterialManager::~MaterialManager()
{
    for (std::map<unsigned int, Material*>::iterator it = materials.begin(); it != materials.end(); it++)
    {
        delete it->second;
    }
}

unsigned int MaterialManager::registerNewMaterial(Material* m)
{

    //Donne un nouveau nom si celui ci n'est pas renseigne
    if (m->getName().empty())
    {
        std::ostringstream num;
        num << counterKey;
        m->getName() = "Material " + num.str();
    }

    m->id = counterKey;

    materials.insert(std::pair<unsigned int, Material*>(counterKey, m));

    unsigned int result = counterKey;
    counterKey++;

    return result;
}

void MaterialManager::print()
{
    //std::cout << "Description de la table des materiaux." << std::endl;
    unsigned int compteur = 0;
    for (std::map<unsigned int, Material*>::iterator it = materials.begin(); it != materials.end(); it++)
    {
        //std::cout << "Materiau " << compteur << " : " << it->second->getName() << std::endl;
        //#ifdef USE_QT
        //      std::cout<<"Couleur : ("<<it->second->r<<","<<it->second->g<<","<<it->second->b<<")"<<std::endl;
        //#endif
        compteur++;
    }

}

Material* MaterialManager::requestMaterial(unsigned int key)
{
    std::map<unsigned int, Material*>::iterator it = materials.find(key);
    if (it != materials.end())
    {
        return it->second;
    }
    else
    {
        return NULL;
    }
}
