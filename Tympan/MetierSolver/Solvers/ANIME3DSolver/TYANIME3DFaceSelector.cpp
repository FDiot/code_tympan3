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

#include "Tympan/MetierSolver/DataManagerCore/TYSolverInterface.h"

#include "Tympan/MetierSolver/DataManagerMetier/Site/TYSiteNode.h"

#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/Triangulate.h"
#include "Tympan/MetierSolver/SolverDataModel/acoustic_problem_model.hpp"

#include "TYANIME3DSolver.h"
#include "TYANIME3DFaceSelector.h"

TYANIME3DFaceSelector::TYANIME3DFaceSelector(const TYSiteNode& Site, const tympan::AcousticProblemModel& aproblem_) : _site(Site), aproblem(aproblem_)
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
    }

    return true;
}

//bool TYANIME3DFaceSelector::triangulateConcavePolygon(TYStructSurfIntersect* tabPolygon, size_t tabPolygonSize)
//{
//    if (tabPolygon == NULL || tabPolygonSize <= 0)
//    {
//        return false;
//    }
//
//    for (unsigned int i = 0; i < tabPolygonSize; i++)
//    {
//        //On triangule toutes les faces qui ne sont pas des triangles
//        //(On ignore le dernier point qui est le premier)
//        if (((int)tabPolygon[i].tabPoint.size()) - 1 > 3)
//        {
//            double normalCoord[3];
//            tabPolygon[i].normal.normalize();
//            tabPolygon[i].normal.getCoords(normalCoord);
//
//            OMatrix P;
//            OMatrix invP;
//
//            //Matrice de passage pour que l'ensemble des points soient 2D (z = 0)
//            double matrix[4][4];
//            double origineCoord[3];
//            tabPolygon[i].tabPoint[0].getCoords(origineCoord);
//
//            if (normalCoord[2] > 0.99 || normalCoord[2] < -0.99) //Face horizontale, on a pas besoin de faire de rotation, juste la translation.
//            {
//                matrix[0][0] = 1; matrix[0][1] = 0; matrix[0][2] = 0;   matrix[0][3] = origineCoord[0];
//                matrix[1][0] = 0; matrix[1][1] = 1; matrix[1][2] = 0;   matrix[1][3] = origineCoord[1];
//                matrix[2][0] = 0; matrix[2][1] = 0; matrix[2][2] = 1;   matrix[2][3] = origineCoord[2];
//                matrix[3][0] = 0; matrix[3][1] = 0; matrix[3][2] = 0;   matrix[3][3] = 1;
//
//            }
//            else //Matrice de passage pour une face non horizontale
//            {
//                vec3 X = vec3(normalCoord[1], -normalCoord[0], 0);
//                vec3 Y = vec3(-normalCoord[0] * normalCoord[2], -normalCoord[2] * normalCoord[1], normalCoord[0] * normalCoord[0] + normalCoord[1] * normalCoord[1]);
//                vec3 Z = vec3(normalCoord[0], normalCoord[1], normalCoord[2]);
//                X.normalize();
//                Y.normalize();
//                Z.normalize();
//
//                matrix[0][0] = X.x; matrix[0][1] = Y.x; matrix[0][2] = Z.x; matrix[0][3] = origineCoord[0];
//                matrix[1][0] = X.y; matrix[1][1] = Y.y; matrix[1][2] = Z.y; matrix[1][3] = origineCoord[1];
//                matrix[2][0] = X.z; matrix[2][1] = Y.z; matrix[2][2] = Z.z; matrix[2][3] = origineCoord[2];
//                matrix[3][0] = 0;   matrix[3][1] = 0;   matrix[3][2] = 0;   matrix[3][3] = 1;
//            }
//
//            //Definition des matrices de passage
//            P = OMatrix(matrix);
//            invP = P.getInvert();
//
//            //Mise en place des coordonnees 2D pour la triangulation
//            Vector2dVector input, output;
//            for (unsigned int j = 0; j < tabPolygon[i].tabPoint.size() - 1; j++)
//            {
//                OPoint3D newCoord = invP * tabPolygon[i].tabPoint[j];
//
//                input.push_back(vec2(newCoord._x, newCoord._y));
//            }
//            // Lancement de la procedure de triangulation
//            Triangulate::Process(input, output);
//
//
//            //Fabrication des OTriangles
//            for (unsigned int j = 0; j < output.size(); j++)
//            {
//                OTriangle t = OTriangle(j, j + 1, j + 2);
//                j += 2;
//                tabPolygon[i].triangles.append(t);
//            }
//
//            //Fabrication des coordonnees vers le repere reel.
//            for (unsigned int j = 0; j < output.size(); j++)
//            {
//                //Conversion vers le repere reel
//                OPoint3D coordLocal = OCoord3D(output.at(j).x, output.at(j).y, 0.f);
//                OPoint3D coordGlobal = P * coordLocal;
//                double coords[3];
//                coordGlobal.getCoords(coords);
//                tabPolygon[i].realVertex.append(OPoint3D(coords[0], coords[1], coords[2]));
//            }
//        }
//    }
//    return true;
//}
