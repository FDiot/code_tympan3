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

#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/Sampler.h"
#include "Spectre.h"
#include <string>

#include "Tympan/MetierSolver/AcousticRaytracer/Base.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/TargetManager.h"

#include <iostream>
#include "Tympan\MetierSolver\AcousticRaytracer\global.h"

//#ifdef USE_QT
//  #include "SourceGraphic.h"
//#endif

class Source : public Base
{

    //#ifdef USE_QT
    //  //WIDGET_DECL(Recepteur)
    //  GRAPHIC_DECL(Source)
    //#endif

public:
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

    virtual ~Source() { }

    std::string getName() { return name; }
    void setName(const std::string& _name) { name = std::string(_name); }

    vec3 getPosition() { return vec3(pos); }
    void setPosition(const vec3 _pos) { pos = vec3(_pos); }

    Spectre getSpectre() { return spectrePuissance; }
    void setSpectre(const Spectre& spectre) { spectrePuissance = Spectre(spectre); }

    void setTargetManager(TargetManager* _targetManager) { targetManager = _targetManager; }

    void setInitialTargetCount(unsigned int nb) { initialTargetCount = nb; targetCount = nb;}
    unsigned int getInitialTargetCount() { return initialTargetCount; }

    int getNbRayLeft() { return nbRayLeft; }
    void setNbRayLeft(int nb) { nbRayLeft = nb; if (nbRayLeft > initialRayCount) { initialRayCount = nbRayLeft; } }

    int getInitialRayCount() { return initialRayCount; }
    void setInitialRayCount(int nb) { initialRayCount = nb; nbRayLeft = nb;}

    Sampler* getSampler() { return sampler; }
    void setSampler(Sampler* _sampler) { sampler = _sampler; }

    bool getDirection(vec3& r)
    {
        //std::cout<<"La source peut encore genere "<<nbRayLeft<<" rayons."<<std::endl;
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

        //std::cout<<"Renvoie d'une nouvelle direction."<<std::endl;
        return true;
    }

    void addDirection(vec3 dir) { directions.push_back(dir); }

    std::vector<vec3>& getPrecomputedDirections() { return directions; }

    unsigned int getId() { return id; }
    void setId(unsigned int _id) { id = _id; }


protected:
    std::string name;
    vec3 pos;
    Spectre spectrePuissance;
    Sampler* sampler;
    int nbRayLeft;
    int initialRayCount;
    int indice; // Knows which ray we are working on.
    unsigned int initialTargetCount;
    unsigned int targetCount;
    std::vector<vec3> directions;
    unsigned int id;
    TargetManager* targetManager;
};
#endif

