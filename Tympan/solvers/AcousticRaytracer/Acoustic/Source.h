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

#ifndef SOURCE_H
#define SOURCE_H

#include <string>
#include <iostream>

#include "Tympan/solvers/AcousticRaytracer/Geometry/Sampler.h"
#include "Tympan/solvers/AcousticRaytracer/Base.h"
#ifdef _ALLOW_TARGETING_
  #include "Tympan/solvers/AcousticRaytracer/Tools/TargetManager.h"
#endif //_ALLOW_TARGETING_
#include "Spectre.h"

/**
 * \brief Acoustic source class
 */
class Source : public Base
{
public:
#ifdef _ALLOW_TARGETING_
    Source(std::string _name = "unknow source") :  sampler(NULL), Base(), targetManager(NULL) { name = _name;}
    Source(const Source& other) : Base(other)
    {
        name = std::string(other.name);
        pos = vec3(other.pos);
        spectrePuissance = Spectre(other.spectrePuissance);
        if (other.sampler) { sampler = other.sampler->Clone(); }
        else { sampler = NULL; }
        nbRayLeft = other.nbRayLeft;
        initialRayCount = other.initialRayCount;
        targetManager = other.targetManager;
        for (unsigned int i = 0; i < other.directions.size(); i++)
        {
            directions.push_back(other.directions.at(i));
        }

    }
#else
    /// Base constructor
    Source(std::string _name = "unknow source") :  sampler(NULL), Base() { name = _name;}
    /// Copy constructor
    Source(const Source& other) : Base(other)
    {
        name = std::string(other.name);
        pos = vec3(other.pos);
        spectrePuissance = Spectre(other.spectrePuissance);
        if (other.sampler) { sampler = other.sampler->Clone(); }
        else { sampler = NULL; }
        nbRayLeft = other.nbRayLeft;
        initialRayCount = other.initialRayCount;
        id = other.id;
        indice =other.indice;
    }
#endif
    /// Destructor
    virtual ~Source() { }

    /// Get the name of the Source
    std::string getName() { return name; }
    /// Set the name of the Source
    void setName(const std::string& _name) { name = std::string(_name); }
    /// Get the position of the Source
    vec3 getPosition() { return vec3(pos); }
    /// Set the position of the Source
    void setPosition(const vec3 _pos) { pos = vec3(_pos); }
    /// Get the spectrum
    Spectre getSpectre() { return spectrePuissance; }
    /// Set the spectrum
    void setSpectre(const Spectre& spectre) { spectrePuissance = Spectre(spectre); }

    /// Get the number of remaining rays to generate
    int getNbRayLeft() { return nbRayLeft; }
    /// Set the number of remaining rays to generate
    void setNbRayLeft(int nb) { nbRayLeft = nb; if (nbRayLeft > initialRayCount) { initialRayCount = nbRayLeft; } }
    /// Get the initial rays counter
    int getInitialRayCount() { return initialRayCount; }
    /// Set the initial rays counter
    void setInitialRayCount(int nb) { initialRayCount = nb; nbRayLeft = nb;}
    /// Get the Sampler of this Source
    Sampler* getSampler() { return sampler; }
    /// Set the Sampler for this Source
    void setSampler(Sampler* _sampler) { sampler = _sampler; }
    /// Get the Source id
	unsigned int getId() { return id; }
	/// Set the Source id
    void setId(unsigned int _id) { id = _id; }
    /// Get a direction (return false if all rays has been launched)
    bool getDirection(vec3& r)
    {
#ifdef _ALLOW_TARGETING_
        if (nbRayLeft <= 0)
        {
            return false;
        }
        nbRayLeft--;
        if (!directions.empty())
        {
            r = vec3(directions.back());
            directions.pop_back();
        }
        //else if(targetCount != 0){
        //  targetCount--;
        //  std::vector<vec3>& targets = targetManager->getTargets();
        //  unsigned int indexTarget = rand() % targets.size();
        //  r = vec3(targets.at(indexTarget) - pos);
        //  r.normalize();
        //}
        else
        {
            r = vec3(sampler->getSample());
        }
#else
        if (nbRayLeft <= 0)
        {
            return false;
        }

        nbRayLeft--;
		
		r = vec3(sampler->getSample());
#endif // _ALLOW_TARGETING_

        //std::cout<<"Renvoie d'une nouvelle direction."<<std::endl;
        return true;
    }

protected:
    std::string name;	//!< Name
    vec3 pos;			//!< Position
    Spectre spectrePuissance;	//!< Spectrum
    Sampler* sampler;	//!< A pointer to a Sampler
    int nbRayLeft;		//!< Number of remaining rays to generate
    int initialRayCount;	//!< Initial rays counter
    unsigned int id;	//!< Source identification
    int indice; 		//!< Index to know which ray we are working on.

#ifdef _ALLOW_TARGETING_
public:
    void setTargetManager(TargetManager* _targetManager) { targetManager = _targetManager; }

    void setInitialTargetCount(unsigned int nb) { initialTargetCount = nb; targetCount = nb;}
    unsigned int getInitialTargetCount() { return initialTargetCount; }
    void addDirection(vec3 dir) { directions.push_back(dir); }

    std::vector<vec3>& getPrecomputedDirections() { return directions; }

protected:
	unsigned int initialTargetCount;
    unsigned int targetCount;
    std::vector<vec3> directions;
    TargetManager* targetManager;
#endif //_ALLOW_TARGETING_
};
#endif
