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

#include "Tympan/models/business/TYSolverInterface.h"

#include "Tympan/models/business/infrastructure/TYSiteNode.h"

#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/Triangulate.h"

#include "TYANIME3DSolver.h"
#include "TYANIME3DFaceSelector.h"

TYANIME3DFaceSelector::TYANIME3DFaceSelector(const TYSiteNode& Site) : _site(Site)
{
}

TYANIME3DFaceSelector::~TYANIME3DFaceSelector()
{

}

bool TYANIME3DFaceSelector::exec(TYStructSurfIntersect *&tabPolygon, size_t& tabPolygonSize)
{
    bool bRet = buildCalcStruct(tabPolygon, tabPolygonSize);
    bRet &= triangulateConcavePolygon(tabPolygon, tabPolygonSize);

    return bRet;
}

bool TYANIME3DFaceSelector::buildCalcStruct(TYStructSurfIntersect *&tabPolygon, size_t& tabPolygonSize)
{
    // On nettoie le tableau des faces
    if (tabPolygon)
    {
        delete [] tabPolygon;
        tabPolygonSize = 0;
    }

    // On recupere les faces
    TYTabAcousticSurfaceGeoNode tabFaces;
    std::vector<bool> estUnIndexDeFaceEcran;
    unsigned int nbFacesInfra = 0;
    std::vector<std::pair<int, int> > indexBatiment;
    std::vector<int> etages;

    _site.getListFacesWithoutFloor(tabFaces, nbFacesInfra, estUnIndexDeFaceEcran, indexBatiment, etages);

    // Reservation de l'espace pour les tableaux
    tabPolygonSize = static_cast<uint32>(tabFaces.size());

    tabPolygon = new TYStructSurfIntersect[tabPolygonSize];

    unsigned int i, j;

//    OMatrix mat;
    TYSol* sol;

    //====================================================
    // 1. Construction du tableau pour les infrastructures
    //====================================================

    // Types de faces d'infrastructure
    TYAcousticPolygon* pPoly = NULL;
    TYAcousticRectangle* pRect = NULL;
    TYAcousticCircle* pCircle = NULL;
    TYAcousticSemiCircle* pSemiCircle = NULL;

    for (i = 0 ; i < nbFacesInfra ; i++)
    {
        // Creation d'une structure de type TYStructSurfIntersect
        tabPolygon[i].pSurfGeoNode = tabFaces[i];

        // Ajout de la matrice inverse
        tabPolygon[i].matInv = tabFaces[i]->getMatrix().getInvert();

        // Ajout d'une face d'ecran
        tabPolygon[i].isEcran = estUnIndexDeFaceEcran[i];

        // Ajout du booleen d'intrastructure et d'ecran
        tabPolygon[i].isInfra = true;

        // Ajout du booleen de l'indice du batiment
        tabPolygon[i].idBuilding = indexBatiment.at(i).second;

        // Ajout de l'indice de face du batiment
        tabPolygon[i].idFace = i; //indexBatiment.at(i).first;

        // Ajout du booleen de l'indice d'etage du batiment
        tabPolygon[i].idEtage = etages.at(i);

        // Ajout du spectre d'attenuation
        tabPolygon[i].spectreAbso = OSpectreComplex(OSpectre(0.0));

        // Recuperation des informations de la suface acoustique
        // si c'est un polygone
        if (pPoly = TYAcousticPolygon::safeDownCast(tabFaces[i]->getElement()))
        {
            tabPolygon[i].tabPoint = pPoly->getContour();
            tabPolygon[i].normal = pPoly->normal();
            tabPolygon[i].spectreAbso = pPoly->getMateriau()->getSpectreAbso();
        }
        // si c'est un rectangle
        else if (pRect = TYAcousticRectangle::safeDownCast(tabFaces[i]->getElement()))
        {
            tabPolygon[i].tabPoint = pRect->getContour();
            tabPolygon[i].normal = pRect->normal();
            tabPolygon[i].spectreAbso = pRect->getMateriau()->getSpectreAbso();
        }
        // si c'est un cercle
        else if (pCircle = TYAcousticCircle::safeDownCast(tabFaces[i]->getElement()))
        {
            tabPolygon[i].tabPoint = pCircle->getContour();
            tabPolygon[i].normal = pCircle->normal();
            tabPolygon[i].spectreAbso = pCircle->getMateriau()->getSpectreAbso();
        }
        // si c'est un 1/2 cercle
        else if (pSemiCircle = TYAcousticSemiCircle::safeDownCast(tabFaces[i]->getElement()))
        {
            tabPolygon[i].tabPoint = pSemiCircle->getContour();
            tabPolygon[i].normal = pSemiCircle->normal();
            tabPolygon[i].spectreAbso = pSemiCircle->getMateriau()->getSpectreAbso();
        }
        else  //Rmq : Spectre d'attenuation pour le sol calcule dans la methode acoustique car depend du rayon incident
        {
            // Verification avec un type de surface non identifie
            continue; // type de face non identifiee
        }
        // Conversion des points dans le repere global
        for (j = 0; j < tabPolygon[i].tabPoint.size(); j++)
        {
            tabPolygon[i].tabPoint[j] =  tabFaces[i]->getMatrix() * tabPolygon[i].tabPoint[j];
        }

        // Ajout de la normale
        tabPolygon[i].normal = tabFaces[i]->getMatrix().multNormal(tabPolygon[i].normal);
        tabPolygon[i].normal.normalize();

        // Ajout de G =0
        tabPolygon[i].G = 0.;

        // A la fin du tableau de pts des contours on rajoute le premier point car ce sont des objets fermes
        if (!tabPolygon[i].tabPoint.empty())
        {
            tabPolygon[i].tabPoint.push_back(tabPolygon[i].tabPoint[0]);
        }
    }

    //===================================================================
    // 2. Construction du tableau pour toutes les faces de la topographie
    //===================================================================

    for (i = nbFacesInfra ; i < tabFaces.size() ; i++)
    {
        // Creation d'une structure de type TYStructSurfIntersect
        tabPolygon[i].pSurfGeoNode = tabFaces[i];

        tabPolygon[i].matInv = OMatrix();

        // pas d'ecran
        tabPolygon[i].isEcran = false;

        // pas d'infrastructures
        tabPolygon[i].isInfra = false;

        // Recuperation du tableau des points formant le contour
        TYAcousticPolygon* pPoly = TYAcousticPolygon::safeDownCast(tabFaces[i]->getElement());
        OPoint3D pt(0., 0., 0.);
        if (pPoly)
        {
            tabPolygon[i].tabPoint = pPoly->getContour();
            for (unsigned int j = 0; j < tabPolygon[i].tabPoint.size(); j++)
            {
                double coords[3];
                tabPolygon[i].tabPoint[j].getCoords(coords);
                pt.set(pt._x +  coords[0], pt._y +  coords[1], pt._z +  coords[2]);
            }

            tabPolygon[i].normal = pPoly->normal();
            tabPolygon[i].normal.normalize();
        }
        pt.set(pt._x / (double)tabPolygon[i].tabPoint.size(), pt._y / (double)tabPolygon[i].tabPoint.size(), pt._z / (double)tabPolygon[i].tabPoint.size());

        sol = _site.getTopographie()->terrainAt(pt)->getSol();

        // indices de batiment, de face et d'etage =-1
        tabPolygon[i].idBuilding = -1;
        tabPolygon[i].idFace = i; //-1;
        tabPolygon[i].idEtage = -1;

        // calcul et ajout de G j'ai pas vu
        tabPolygon[i].G = sol->getG(); //min(pow(300.0 / sol->getResistivite(), 0.57), 1.0);  //Resistivite du sol capable de calculer le G


        if (!tabPolygon[i].tabPoint.empty())
        {
            tabPolygon[i].tabPoint.push_back(tabPolygon[i].tabPoint[0]);    // On rajoute le premier point car ce sont des objets fermes
        }
    }

    return true;
}

bool TYANIME3DFaceSelector::triangulateConcavePolygon(TYStructSurfIntersect* tabPolygon, size_t tabPolygonSize)
{
    if (tabPolygon == NULL || tabPolygonSize <= 0)
    {
        return false;
    }

    for (unsigned int i = 0; i < tabPolygonSize; i++)
    {
        //On triangule toutes les faces qui ne sont pas des triangles
        //(On ignore le dernier point qui est le premier)
        if (((int)tabPolygon[i].tabPoint.size()) - 1 > 3)
        {
            double normalCoord[3];
            tabPolygon[i].normal.normalize();
            tabPolygon[i].normal.getCoords(normalCoord);

            OMatrix P;
            OMatrix invP;

            //Matrice de passage pour que l'ensemble des points soient 2D (z = 0)
            double matrix[4][4];
            double origineCoord[3];
            tabPolygon[i].tabPoint[0].getCoords(origineCoord);

            if (normalCoord[2] > 0.99 || normalCoord[2] < -0.99) //Face horizontale, on a pas besoin de faire de rotation, juste la translation.
            {
                matrix[0][0] = 1; matrix[0][1] = 0; matrix[0][2] = 0;   matrix[0][3] = origineCoord[0];
                matrix[1][0] = 0; matrix[1][1] = 1; matrix[1][2] = 0;   matrix[1][3] = origineCoord[1];
                matrix[2][0] = 0; matrix[2][1] = 0; matrix[2][2] = 1;   matrix[2][3] = origineCoord[2];
                matrix[3][0] = 0; matrix[3][1] = 0; matrix[3][2] = 0;   matrix[3][3] = 1;

            }
            else //Matrice de passage pour une face non horizontale
            {
                vec3 X = vec3(normalCoord[1], -normalCoord[0], 0);
                vec3 Y = vec3(-normalCoord[0] * normalCoord[2], -normalCoord[2] * normalCoord[1], normalCoord[0] * normalCoord[0] + normalCoord[1] * normalCoord[1]);
                vec3 Z = vec3(normalCoord[0], normalCoord[1], normalCoord[2]);
                X.normalize();
                Y.normalize();
                Z.normalize();

                matrix[0][0] = X.x; matrix[0][1] = Y.x; matrix[0][2] = Z.x; matrix[0][3] = origineCoord[0];
                matrix[1][0] = X.y; matrix[1][1] = Y.y; matrix[1][2] = Z.y; matrix[1][3] = origineCoord[1];
                matrix[2][0] = X.z; matrix[2][1] = Y.z; matrix[2][2] = Z.z; matrix[2][3] = origineCoord[2];
                matrix[3][0] = 0;   matrix[3][1] = 0;   matrix[3][2] = 0;   matrix[3][3] = 1;
            }

            //Definition des matrices de passage
            P = OMatrix(matrix);
            invP = P.getInvert();

            //Mise en place des coordonnees 2D pour la triangulation
            Vector2dVector input, output;
            for (unsigned int j = 0; j < tabPolygon[i].tabPoint.size() - 1; j++)
            {
//                double coord[3];
                //OCoord3D newCoord = invP * tabPolygon[i].tabPoint[j];
                OPoint3D newCoord = invP * tabPolygon[i].tabPoint[j];
//                newCoord.getCoords(coord);

                input.push_back(vec2(newCoord._x, newCoord._y));
            }
            // Lancement de la procedure de triangulation
            Triangulate::Process(input, output);


            //Fabrication des OTriangles
            for (unsigned int j = 0; j < output.size(); j++)
            {
                OTriangle t = OTriangle(j, j + 1, j + 2);
                j += 2;
                tabPolygon[i].triangles.append(t);
            }

            //Fabrication des coordonnees vers le repere reel.
            for (unsigned int j = 0; j < output.size(); j++)
            {
                //Conversion vers le repere reel
                OPoint3D coordLocal = OCoord3D(output.at(j).x, output.at(j).y, 0.f);
                OPoint3D coordGlobal = P * coordLocal;
                double coords[3];
                coordGlobal.getCoords(coords);
                tabPolygon[i].realVertex.append(OPoint3D(coords[0], coords[1], coords[2]));
            }
        }
    }
    return true;
}
