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

#ifndef SPECTRE_H
#define SPECTRE_H

#include <vector>

class Spectre
{

public:
    Spectre() { sizeSpectre = 0; }
    Spectre(const Spectre& other)
    {
        sizeSpectre = other.sizeSpectre;
        for (int i = 0; i < sizeSpectre; i++)
        {
            freqs.push_back(freqs.at(i));
        }
    }

    virtual ~Spectre() { }

    int getSizeSpectre() { return sizeSpectre; }

    void getFrequencies(std::vector<int> &r)
    {
        for (int i = 0; i < sizeSpectre; i++)
        {
            r.push_back(freqs.at(i).first);
        }
    }

    void getFrequencies(int* r)
    {
        for (int i = 0; i < sizeSpectre; i++)
        {
            r[i] = freqs.at(i).first;
        }
    }

    int getFrequencie(int i)
    {
        if (i < 0 || i >= sizeSpectre)
        {
            return -1;
        }
        return freqs.at(i).first;
    }

    void getPowers(std::vector<decimal> &r)
    {
        for (int i = 0; i < sizeSpectre; i++)
        {
            r.push_back(freqs.at(i).second);
        }
    }

    void getPowers(decimal* r)
    {
        for (int i = 0; i < sizeSpectre; i++)
        {
            r[i] = freqs.at(i).second;
        }
    }

    decimal getPower(int i)
    {
        if (i < 0 || i >= sizeSpectre)
        {
            return -200.;
        }
        return freqs.at(i).second;
    }

    void addFrequencie(int freq, decimal power)
    {
        freqs.push_back(std::pair<int, decimal>(freq, power));
    }

protected:
    int sizeSpectre;
    std::vector< std::pair<int, decimal> > freqs;
};

#endif