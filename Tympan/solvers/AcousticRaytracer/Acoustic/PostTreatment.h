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

#ifndef POSTTREATMENT_H
#define POSTTREATMENT_H

#include "Tympan/solvers/AcousticRaytracer/Geometry/Scene.h"
#include "Recepteur.h"

#ifdef _ALLOW_TARGETING_
#include "Tympan/solvers/AcousticRaytracer/Tools/TargetManager.h"
#endif _ALLOW_TARGETING_

/**
 * \brief Namespace for post-treatment
 */
namespace PostTreatment
{
/// Build the edges list of a scene
bool constructEdge(Scene* scene);

#ifdef _ALLOW_TARGETING_
bool findTargetsForNMPB(Scene* scene, std::vector<Recepteur>& recepteurs, TargetManager& targetManager, decimal density);
void appendDirectionToSources(TargetManager* targets, std::vector<Source>& sources);
#endif _ALLOW_TARGETING_
};

#endif
