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
 
#include "TargetManager.h"

bool TargetManager::registerTarget(const vec3 newTarget)
{

    std::pair<std::set<vec3>::iterator, bool> ret;
    ret = uniqueTargets.insert(newTarget);
    if (ret.second)
    {
        targets.push_back(newTarget);
        return true;
    }

    return false;
}

bool TargetManager::registerTargets(std::vector<vec3>& newTargets)
{
    bool result = true;
    for (unsigned int i = 0; i < newTargets.size(); i++)
    {
        result &= registerTarget(newTargets.at(i));
    }

    return result;
}
void TargetManager::finish()
{
    uniqueTargets.clear();
}

unsigned int TargetManager::getTargetsAround(const vec3 center, unsigned int nbTargets, decimal distance, std::vector<vec3>& result)
{
    std::vector<unsigned int> globalTargets;
    for (unsigned int i = 0; i < targets.size(); i++)
    {
        if (center.distance(targets.at(i)) < distance)
        {
            globalTargets.push_back(i);
        }
    }

    if (globalTargets.size() < nbTargets)
    {
        for (unsigned int i = 0; i < globalTargets.size(); i++)
        {
            result.push_back(targets.at(globalTargets.at(i)));
        }

        return globalTargets.size();
    }
    else
    {
        for (unsigned int i = 0; i < nbTargets; i++)
        {
            unsigned int choice = rand() % globalTargets.size();
            result.push_back(targets.at(globalTargets.at(choice)));
        }

        return nbTargets;
    }
}
