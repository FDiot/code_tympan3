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

/**
 * TYDXFImporter.cpp : Definit le point d'entree pour l'import des fichiers DXF
 * au format BD-TOPO.
 */

#ifdef _MSC_VER
#pragma warning(disable : 4786 4275 4503)//az++ : 4503; cf OPreferenceManager.h qui fait la meme chose; concerne les map et les xtree de la stl, utilises par le Preference manager
#endif

#ifdef TY_USE_DXF

#define LIMITES_MAX "$LIMMAX"
#define LIMITES_MIN "$LIMMIN"

#include <dime/Input.h>
#include <dime/Output.h>
#include <dime/Model.h>
#include <dime/State.h>
#include <dime/sections/EntitiesSection.h>
#include <dime/sections/HeaderSection.h>
#include <dime/entities/Entity.h>

#include "TYDXFImporter.h"

#include "Tympan/models/business/infrastructure/TYSiteNode.h"
#include "Tympan/core/logging.h"
#include "Tympan/models/business/TYPreferenceManager.h"


void TYDXFImporter::LoadFloatParam(const char* ParamName, double& dParam)
{
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, ParamName))
    {
        dParam = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, ParamName);
    }
}

bool TYDXFImporter::ReadDXF(dimeModel* model, const QString& DXFFileName)
{
    if (!QFile::exists(DXFFileName))
    {
        return false;
    }
    const QByteArray ba = DXFFileName.toLocal8Bit();
    const char* c_filename = ba.data();
    dimeInput in;
    if (!in.setFile(c_filename))
    {
        Display("Error opening file for reading: %s", c_filename);
        return false;
    }
    if (!model->read(&in))
    {
        Display("DXF read error in line: %d", in.getFilePosition());
        return false;
    }
    return true;
}

bool TYDXFImporter::GetCurrentLimites(const dimeModel* model, DXFLimites& CurrentMin, DXFLimites& CurrentMax)
{
    dimeHeaderSection* pdimeHeaderSection = (dimeHeaderSection*)model->findSection("HEADER");
    int groupcodes[3];
    dimeParam   max[3];
    dimeParam   min[3];
    int nRes;
    nRes = pdimeHeaderSection->getVariable(LIMITES_MAX,  groupcodes, max, 2);
    if (nRes != 2) { return false; }
    nRes = pdimeHeaderSection->getVariable(LIMITES_MIN,  groupcodes, min, 2);
    if (nRes != 2) { return false; }

    CurrentMin.x = min[0].double_data;
    CurrentMin.y = min[1].double_data;
    CurrentMax.x = max[0].double_data;
    CurrentMax.y = max[1].double_data;
    return true;
}


int TYDXFImporter::Get3DPolylinesInLayer(const dimeModel* model, const char* LayerName, TYTabTabPoint* pts, DXFLimites Offset)
{
    dimeEntitiesSection* pdimeEntitiesSection = (dimeEntitiesSection*)model->findSection("ENTITIES");
    if (NULL == pdimeEntitiesSection)
    {
        return 0;
    }

    int nNbPolylines = 0;
    for (int i = 0; i < pdimeEntitiesSection->getNumEntities(); i++)
    {
        dimeEntity*  pdimeEntity = pdimeEntitiesSection->getEntity(i);
        if (0 == strcmp(pdimeEntity->getEntityName(), "POLYLINE"))
        {
            dimeLayer* pLayer = (dimeLayer*)pdimeEntity->getLayer();
            if (0 == strcmp(pLayer->getLayerName(), LayerName))
            {
                dimeArray< dimeVec3f > verts;
                dimeArray< int > indices;
                dimeVec3f extrusionDir;
                dxfdouble thickness;
                dimeEntity::GeometryType TheGeometryType = pdimeEntity->extractGeometry(verts, indices, extrusionDir, thickness);

                TYTabPoint CurrentPolyline;
                for (int j = 0; j < verts.count(); j++)
                {
                    verts[j].x -= Offset.x;
                    verts[j].y -= Offset.y;
                    TYPoint CurrentPoint(verts[j].x, verts[j].y, verts[j].z);
                    CurrentPolyline.push_back(CurrentPoint);
                }
                (*pts).push_back(CurrentPolyline);
                nNbPolylines++;
            }
        }
    }
    return nNbPolylines;
}

int TYDXFImporter::Get3DPointsInLayer(const dimeModel* model, const char* LayerName, TYTabPoint* pts, DXFLimites Offset)
{
    dimeEntitiesSection* pdimeEntitiesSection = (dimeEntitiesSection*)model->findSection("ENTITIES");
    if (NULL == pdimeEntitiesSection)
    {
        return 0;
    }
    int nNbPoints = 0;
    for (int i = 0; i < pdimeEntitiesSection->getNumEntities(); i++)
    {
        dimeEntity*  pdimeEntity = pdimeEntitiesSection->getEntity(i);
        if (0 == strcmp(pdimeEntity->getEntityName(), "POINT"))
        {
            dimeLayer* pLayer = (dimeLayer*)pdimeEntity->getLayer();
            if (0 == strcmp(pLayer->getLayerName(), LayerName))
            {
                dimeArray< dimeVec3f > verts;
                dimeArray< int > indices;
                dimeVec3f extrusionDir;
                dxfdouble thickness;
                dimeEntity::GeometryType TheGeometryType = pdimeEntity->extractGeometry(verts, indices, extrusionDir, thickness);

                int j = 0;
                verts[j].x -= 0;//test Offset.x;
                verts[j].y -= 0;//test Offset.y;
                TYPoint CurrentPoint(verts[j].x, verts[j].y, verts[j].z);
                (*pts).push_back(CurrentPoint);
                nNbPoints++;
            }
        }
    }

    return nNbPoints;
}

bool TYDXFImporter::RefreshMinMaxModels(const dimeModel* model, DXFLimites& LimiteMin, DXFLimites& LimiteMax)
{
    DXFLimites CurrentMin, CurrentMax;
    if (GetCurrentLimites(model, CurrentMin, CurrentMax))
    {
        if (LimiteMin.x == LimiteMax.x && LimiteMin.y == LimiteMax.y)
        {
            LimiteMin = CurrentMin;
            LimiteMax = CurrentMax;
        }
    }
    else
    {
        return false;
    }
    //Refresh Max
    if (LimiteMin.x > CurrentMin.x)
    {
        LimiteMin.x = CurrentMin.x;
    }
    if (LimiteMin.y > CurrentMin.y)
    {
        LimiteMin.y = CurrentMin.y;
    }
    //Refresh Max
    if (LimiteMax.x < CurrentMax.x)
    {
        LimiteMax.x = CurrentMax.x;
    }
    if (LimiteMax.y < CurrentMax.y)
    {
        LimiteMax.y = CurrentMax.y;
    }

    return true;
}

//////////////////////////////////////////////////////////////////////
// Methodes geometriques
//////////////////////////////////////////////////////////////////////
void TYDXFImporter::DecimatePolyline(double dPolylineDecimationDistanceMin, TYTabPoint* ptsIn, TYTabPoint* ptsOut)
{
    //En sortie, on doit avoir au moins un segment
    TYTabPoint::iterator it = ptsIn->begin();
    TYPoint* LastPoint = &(*it);
    ++it;
    int i = 1;
    double dCurrentTotalDistance = 0;
    while (it != ptsIn->end())
    {
        double dCurrentDistance = (*it).distFrom(*LastPoint);
        dCurrentTotalDistance += dCurrentDistance;
        if (dCurrentTotalDistance > dPolylineDecimationDistanceMin)
        {
            //RAZ distance totale car depassement de seuil de distance:
            dCurrentTotalDistance = 0;
            //Insert du point courant dans le tableau de sortie
            (*ptsOut).push_back((*it));
            //Memorisation du dernier point valide:
            LastPoint = &(*it);
        }
        ++it;
        i++;
    }
}

bool TYDXFImporter::Get2DBoundingBox(TYTabPoint* pts, double& minX, double& maxX, double& minY, double& maxY)
{
    minX = maxX = minY = maxY = 0;
    if (!pts->size())
    {
        return false;
    }

    minX = maxX = (*pts)[0]._x;
    minY = maxY = (*pts)[0]._y;

    for (unsigned int i = 1; i < pts->size(); i++)
    {
        if (maxX < (*pts)[i]._x)
        {
            maxX = (*pts)[i]._x;
        }
        if (maxY < (*pts)[i]._y)
        {
            maxY = (*pts)[i]._y;
        }
        if (minX > (*pts)[i]._x)
        {
            minX = (*pts)[i]._x;
        }
        if (minY > (*pts)[i]._y)
        {
            minY = (*pts)[i]._y;
        }
    }
    return true;
}

LPTYMachine TYDXFImporter::BuildMachine(TYTabPoint* pts, double dHauteur, LPTYSiteNode pSite)
{
    double minX, maxX, minY, maxY;
    bool bGet2DBoundingBox = Get2DBoundingBox(pts, minX, maxX, minY, maxY);
    if (!bGet2DBoundingBox)
    {
        return NULL;
    }
    // Construction d'une machine
    LPTYMachine pMachine = new TYMachine();

    // Construction des volumes acoustiques de la machine
    LPTYAcousticBox pBox = new TYAcousticBox();

    pBox->setDimension(maxX - minX, maxY - minY, dHauteur);

    // Localisation de la machine
    TYRepere repereMachine;
    repereMachine._origin._x = (maxX + minX) / 2;
    repereMachine._origin._y = (maxY + minY) / 2;
    repereMachine._origin._z = dHauteur / 2;
    TYRepere rep;
    rep._origin._z = dHauteur / 2;
    pMachine->addAcousticVol(pBox, rep);

    pSite->getInfrastructure()->addMachine(pMachine, repereMachine); // ajoute la machine a l'infrastructure
    return pMachine;
}

LPTYBatiment TYDXFImporter::BuildSilo(TYTabPoint* pts, double dHauteur, LPTYSiteNode pSite)
{
    //Trouver le diametre du silo
    float diameter = 15;//revoir
    // Nouveau cylindre
    LPTYAcousticCylinderGeoNode pCylGeoNode = new TYAcousticCylinderGeoNode(new TYAcousticCylinder);
    LPTYAcousticCylinder pTYAcousticCylinder = TYAcousticCylinder::safeDownCast(pCylGeoNode->getElement());
    pTYAcousticCylinder->setDiameter(diameter);
    pTYAcousticCylinder->setHauteur(dHauteur);

    // Position: trouver le centre de la polyligne
    TYPoint org;
    org = (*pts)[0];//revoir


    TYRepere rep;

    rep._origin = org;

    LPTYBatiment pBat = new TYBatiment();

    pSite->getInfrastructure()->addBatiment(pBat, rep);//Err
    //Ajout de l'etage au bâtiment
    TYRepere addAcousticVolRep;
    addAcousticVolRep._origin._z = dHauteur / 2;
    pBat->addAcousticVol(pTYAcousticCylinder, addAcousticVolRep);

    return pBat;
}

LPTYBatiment TYDXFImporter::BuildConstruction(TYTabPoint* pts, double dHauteur, bool bBatiment, bool bForceFermetureContour, LPTYSiteNode pSite)
{

#ifdef SPECIFICITE_BDD_PAMIER
    //Les batiments ont un point visuellement abherant (le dernier) : de visu dans le modeler 3D, ressemble a un point du toit qui se trouve au sol
    //et a l'exterieur du contour du toit; pour eviter cette visu "gruyere", on supprime le dernier point:
    pts->pop_back();//Test Pamiers
#endif // SPECIFICITE_BDD_PAMIER

    LPTYTopographie pTYTopographie = pSite->getTopographie();
    if (bForceFermetureContour)
    {
        //Ajouter un point pour refermer le contour (dernier = premier) si celui-ci n'est deja pas present:
        // On teste le 1er point et le dernier point sont confondus avec une tolerance
        if (!((OCoord3D)(*pts)[0] == (OCoord3D)(*pts)[pts->size() - 1]))
        {
            // On reprend le dernier point:
            pts->push_back((*pts)[0]);
        }
    }

    //On garde la polyligne courante
    LPTYEtage pEtage = new TYEtage();
    // L'etage, dans son repere, est en z=0
    int nbPts = (*pts).size();
    int j;
    for (j = 0; j < nbPts; j++)
    {
        (*pts).at(j)._z = 0.0;
    }

    // On calcul le centre de gravite de l'etage pour l'utiliser comme repere dans le bâtiment
    OVector3D centre(0.0, 0.0, 0.0);


    // On effectue la moyenne en (X,Y) des points au sol
    for (j = 0; j < nbPts; j++)
    {
        centre._x += (*pts).at(j)._x;
        centre._y += (*pts).at(j)._y;
    }

    // Moyenne
    centre._x /= nbPts;
    centre._y /= nbPts;

    // On translate les points saisies en tenant compte du centre
    for (j = 0; j < nbPts; j++)
    {
        (*pts).at(j) = OVector3D((*pts).at(j)) - centre;
    }

    // Hauteur au sol : nulle (=>on ne peut pas faire d'etage au dessus d'un autre)
    centre._z = 0;
    OPoint3D centre2(centre._x, centre._y, 0.0);
    pSite->getAltimetry()->altitude(centre2);
    centre._z = centre2._z;

    // Repere de l'etage dans le bâtiment
    TYRepere rep;
    rep._origin = centre;

    // Construction des murs
    pEtage->setMurs((*pts), dHauteur, bBatiment);
    LPTYBatiment pBat = new TYBatiment();
    pSite->getInfrastructure()->addBatiment(pBat, rep);//Err
    //Ajout de l'etage au bâtiment
    pBat->addAcousticVol((LPTYAcousticVolume&) pEtage, TYRepere());


    return pBat;
}
//////////////////////////////////////////////////////////////////////
// Procedures utilitaires
//////////////////////////////////////////////////////////////////////
//Redirection de l'affichage des messages
void TYDXFImporter::Display(const char* format, ...)
{
    char        MessageString[2048];
    va_list ArgList;
    va_start(ArgList, format);       //Initialize variable arguments.
    vsprintf(MessageString, format, ArgList);
    unsigned long str_len = strlen(MessageString) + 1;
    if (str_len < 2048)
    {
        OMessageManager::get()->info(MessageString);
    }
    va_end(ArgList);                // Reset variable arguments.
}



//Nommage des objets metiers avec un n° d'instance (a mettre dans TYElement ?):
void TYDXFImporter::SetName(LPTYElement pTYElement, const char* sName, int nInstanceNumber)
{
    static char sInstanceName[1024];
    sprintf(sInstanceName, "%s#%d", sName, nInstanceNumber);
    pTYElement->setName( std::string(sName) );
}

#endif
