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

/**
 * \brief Spectrum class
 */
class Spectre
{

public:
	/// Default constructor
    Spectre() {freqs = std::vector< std::pair<int, decimal> >();}
    /// Copy constructor
    Spectre(const Spectre& other)
    {
		for (unsigned int i = 0; i < other.freqs.size() ; i++)
        {
            freqs.push_back(freqs.at(i));
        }
    }
    /// Destructor
    virtual ~Spectre() { }

    /// Get the size of the vector storing the spectrum
	unsigned int getSizeSpectre() { return freqs.size(); }

    /// Return into a vector all the frequency values of the spectrum
    void getFrequencies(std::vector<int> &r)
    {
        for (unsigned int i = 0; i < freqs.size(); i++)
        {
            r.push_back(freqs.at(i).first);
        }
    }
    /// Return into an array (sized before) all the frequency values of the spectrum
    void getFrequencies(int* r)
    {
        for (unsigned int i = 0; i < freqs.size(); i++)
        {
            r[i] = freqs.at(i).first;
        }
    }
    /// Get a the ith frequency of the spectrum
    int getFrequencie(unsigned int i)
    {
        if (i < 0 || i >= freqs.size())
        {
            return -1;
        }
        return freqs.at(i).first;
    }
    /// Return into a vector all the power values of the spectrum
    void getPowers(std::vector<decimal> &r)
    {
        for (unsigned int i = 0; i < freqs.size(); i++)
        {
            r.push_back(freqs.at(i).second);
        }
    }
    /// Return into an array (sized before) all the power values of the spectrum
    void getPowers(decimal* r)
    {
        for (unsigned int i = 0; i < freqs.size(); i++)
        {
            r[i] = freqs.at(i).second;
        }
    }
    /// Get the power for a frequency given by its index
    decimal getPower(unsigned int i)
    {
        if (i < 0 || i >= freqs.size())
        {
            return -200.;
        }
        return freqs.at(i).second;
    }
    /// Add a new pair (frequency,power)
    void addFrequencie(int freq, decimal power)
    {
        freqs.push_back(std::pair<int, decimal>(freq, power));
    }

protected:							
    std::vector< std::pair<int, decimal> > freqs;	//!< Vector of pairs (Frequency,Power)
};

#endif
