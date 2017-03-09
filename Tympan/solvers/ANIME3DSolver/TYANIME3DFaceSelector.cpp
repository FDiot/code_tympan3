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

#include "Tympan/models/solver/acoustic_problem_model.hpp"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Triangulate.h"
#include "TYANIME3DSolver.h"
#include "TYANIME3DFaceSelector.h"

TYANIME3DFaceSelector::TYANIME3DFaceSelector(const tympan::AcousticProblemModel& aproblem_) : aproblem(aproblem_)
{
}

TYANIME3DFaceSelector::~TYANIME3DFaceSelector()
{

}

bool TYANIME3DFaceSelector::exec(TYStructSurfIntersect *&tabPolygon, size_t& tabPolygonSize)
{
    bool bRet = buildCalcStruct(tabPolygon, tabPolygonSize);
    //bRet &= triangulateConcavePolygon(tabPolygon, tabPolygonSize);

    return bRet;
}

bool TYANIME3DFaceSelector::buildCalcStruct(TYStructSurfIntersect *&tabPolygon, size_t& tabPolygonSize)
{
     bool cancel = false;

    const tympan::nodes_pool_t& nodes = aproblem.nodes(); 
    const tympan::triangle_pool_t& triangles = aproblem.triangles();

    tabPolygonSize = triangles.size();
    tabPolygon = new TYStructSurfIntersect[triangles.size()];

    for (unsigned int i=0; i<triangles.size(); i++)
    {
        OPoint3D pts[3];
        pts[0] = nodes[triangles[i].n[0]];
        pts[1] = nodes[triangles[i].n[1]];
        pts[2] = nodes[triangles[i].n[2]];

        OVector3D normal;
        OGeometrie::computeNormal(pts, 3, normal);

        tabPolygon[i].tabPoint.push_back(pts[0]);
        tabPolygon[i].tabPoint.push_back(pts[1]);
        tabPolygon[i].tabPoint.push_back(pts[2]);
        tabPolygon[i].normal = normal;

        tabPolygon[i].material = triangles[i].made_of.get();
    }

    return true;
}
