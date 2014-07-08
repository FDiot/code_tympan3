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

#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <sstream>
#include <fstream>

extern std::stringstream ss;

class Logger
{

public:
    Logger() { };
    ~Logger() { };

    //static stringstream& getStream(){ return ss; }

    void write(const char* filename)
    {
        std::ofstream fichier(filename, std::ios::out | std::ios::trunc);  //declaration du flux et ouverture du fichier

        if (fichier)
        {
            fichier << ss.str();
            fichier.close();
        }
    }

    //static stringstream ss;

};

#endif
