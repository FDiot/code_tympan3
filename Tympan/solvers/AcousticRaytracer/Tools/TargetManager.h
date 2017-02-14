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

#ifndef TARGETMANAGER_H
#define TARGETMANAGER_H

#include "Tympan/models/common/mathlib.h"
#include <vector>
#include <set>

/**
 * \brief Class to manage targets
 */
class TargetManager
{

public:
	/// Constructor
    TargetManager() { }
    /// Register a new target
    bool registerTarget(const vec3 newTarget);
    /// Register a vector of targets
    bool registerTargets(std::vector<vec3>& newTargets);
    /// Delete the uniqueTargets array
    void finish();
    /// Get the targets in a distance range from a center
    unsigned int getTargetsAround(const vec3 center, unsigned int nbTargets, decimal distance, std::vector<vec3>& result);
    /// Return the targets
    std::vector<vec3>& getTargets() { return targets; }

protected:
    std::vector<vec3> targets;		//!< Vector of targets
    std::set<vec3> uniqueTargets;	//!< Set of single targets
};

#endif
