/*
 * Copyright (c) 2007 EDF. All Rights Reserved.
 * All Rights Reserved. No part of this software and its documentation may
 * be used, copied, modified, distributed and transmitted, in any form or by
 * any means, without the prior written permission of EDF.
 *
 * EDF-R&D Departement AMA - 1, avenue du general de Gaulle, 92141 CLAMART
 *
 */

/**
 * TYDXFBDTopoImporter.cpp : Definit le point d'entree pour l'import des fichiers DXF au format BD-TOPO.
 */

#ifdef _MSC_VER
#pragma warning(disable : 4786 4275 4503)//az++ : 4503; cf OPreferenceManager.h qui fait la meme chose; concerne les map et les xtree de la stl, utilises par le Preference manager
#endif

#ifdef TY_USE_DXF

#include <qdir.h>

#include <dime/Input.h>
#include <dime/Output.h>
#include <dime/Model.h>
#include <dime/State.h>
#include <dime/sections/EntitiesSection.h>
#include <dime/sections/HeaderSection.h>
#include <dime/entities/Entity.h>
#include "TYDXFBDTOPOImporter.h"
#include "TYDXFBDTOPOSpecifications.h"
#include "Tympan/models/business/infrastructure/TYSiteNode.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TYDXFBDTopoImporter::TYDXFBDTopoImporter(double dPolylineDecimationDistanceMin,
                                         double dMinHauteurBatiments)
    : m_dPolylineDecimationDistanceMin(dPolylineDecimationDistanceMin),
      m_dMinHauteurBatiments(dMinHauteurBatiments)
{
    LoadFloatParam("DistMinBDTopo", m_dPolylineDecimationDistanceMin);
    LoadFloatParam("HautMinBatimentsBDTopo", m_dMinHauteurBatiments);
}

TYDXFBDTopoImporter::~TYDXFBDTopoImporter()
{

}

bool TYDXFBDTopoImporter::import(const char* FullBDDPath, LPTYSiteNode* pSiteIn)
{

    //////////////////////////////////////////////////////////////////////
    // Variables de test
    //////////////////////////////////////////////////////////////////////
    //const bool bTestConstructionBDTOPO = false;
    //const bool bTestAltimetrieBDTOPO = false;

    const int nMaxCourbeNiveau = 2000000;
    const int nMaxRoutes = 2000000;
    const int nMaxCoursEau = 2000000;
    const int nMaxReseauTransportNRJ = 2000000;
    const int nMaxPlanEau = 2000000;
    const int nMaxTerrain = 2000000;
    const int nMaxBatiment = 2000000;


    //Import de tous les fichiers DXF dans des objets dime
    const int nDXFFileCount = sizeof(DXFPrefixFileName) / sizeof(const char*);//8;
    dimeModel model[nDXFFileCount];
    //->on calcule les limites geometriques des objets compris dans tous les dxf;
    //cela permet de faire une translation pour centrer la scene au mileu de l'ecran
    //(au lieu de la mettre a 300 km: on se moque des coordonnees de l'origine BD-TOPO)
    DXFLimites LimiteMin, LimiteMax;
    LimiteMin.x = LimiteMin.y = 0;//LimiteMin[2] = 0;
    LimiteMax.x = LimiteMax.y = 0;//LimiteMax[2] = 0;
    for (int i = 0; i < nDXFFileCount; ++i)
    {
        //1. Build DXF file name
        QString DXFFileName = GetDXFFileNameFromBDTopoPrefixAndDirectory(DXFPrefixFileName[i], FullBDDPath);

        //2. Read the DXF file in the model
        if (!ReadDXF(&(model[i]), DXFFileName))
        {
            // Display("Impossible de lire le fichier %s", (char*)DXFFileName.data()); // XXX Old line. TODO remove
            Display("Impossible de lire le fichier %s", DXFFileName.data());
            return false;
        }
        else
        {
            //Maj des limites (min/max globaux)
            RefreshMinMaxModels(&(model[i]), LimiteMin, LimiteMax);
        }
    }
    Display("Limites Min pour les calques : %lf %lf", LimiteMin.x, LimiteMin.y);//, LimiteMin[2]);
    Display("Limites Max pour les calques : %lf %lf", LimiteMax.x, LimiteMax.y);//, LimiteMax[2]);
    //->Calcul du vecteur de translation
    DXFLimites Offset;
    Offset.x = (LimiteMax.x + LimiteMin.x) / 2;
    Offset.y = (LimiteMax.y + LimiteMin.y) / 2;
    //Offset[2] = 0;//LimiteMax[2] - LimiteMin[2];//on ne translate pas l'altitude
    Display("=>Translation de : %lf %lf", Offset.x, Offset.y);//, Offset[2]);

    //Creation d'un projet et/ou d'un site
    LPTYSiteNode pSite = new TYSiteNode;
    (*pSiteIn) = pSite;
    LPTYTopographie pTYTopographie = pSite->getTopographie();


    //////////////////////////////////////////////////////////////////////////
    //
    //          T O P O G R A P H I E
    //
    //
    //////////////////////////////////////////////////////////////////////////

    //Les elements de topographie permettent de calculer l'altimetrie:
    //courbes de niveau, cours d'eau, plan d'eau, terrain

    //////////////////////////////////////////////////////////////////////////
    //      Courbes de niveau
    //////////////////////////////////////////////////////////////////////////
    importIsoline(nMaxCourbeNiveau, model, Offset, pTYTopographie);

    //////////////////////////////////////////////////////////////////////////
    //      Cours d'eau
    //////////////////////////////////////////////////////////////////////////
    importWatercourse(nMaxCoursEau, model, Offset, pTYTopographie);

    //////////////////////////////////////////////////////////////////////////
    //      Plans d'eau
    //////////////////////////////////////////////////////////////////////////
    importWaterplan(nMaxPlanEau, model, Offset, pTYTopographie);

    //////////////////////////////////////////////////////////////////////////
    //      Terrains
    //////////////////////////////////////////////////////////////////////////
    importField(nMaxTerrain, model, Offset, pTYTopographie);

    // Notify the altimetry is not uptodate
    pSite->setAltimetryUpToDate(false);

    //////////////////////////////////////////////////////////////////////////
    //
    //          I N F R A S T R U C T U R E
    //
    //
    //////////////////////////////////////////////////////////////////////////

    // Debut de definition du calcul associe au projet
    //LPTYCalcul pCalcul = new TYCalcul();
    // Affectation du site et du calcul au projet au projet
    //(*pProjet)->setSite(pSite); //Attribution du site au projet
    //(*pProjet)->addCalcul(pCalcul); // Ajout du calcul dans le tableau des calculs du projet

    //////////////////////////////////////////////////////////////////////////
    //      Routes
    //////////////////////////////////////////////////////////////////////////

    importRoad(nMaxRoutes, model, Offset, pTYTopographie, pSite);


    //////////////////////////////////////////////////////////////////////////
    //      Reseau de Transport
    //////////////////////////////////////////////////////////////////////////

    importTransmissionNetwork(nMaxReseauTransportNRJ, model, Offset,
                              pTYTopographie, pSite);

    //////////////////////////////////////////////////////////////////////////
    //      Constructions
    //////////////////////////////////////////////////////////////////////////

    importBuilding(nMaxBatiment, model, Offset, pTYTopographie, pSite);

    return true;
}


void TYDXFBDTopoImporter::importIsoline(int nMaxCourbeNiveau, const dimeModel model[],
                                        const DXFLimites& Offset,
                                        LPTYTopographie pTYTopographie)
{
    bool bRes;
    int nNbItem = 0;
    int nNbPointTotalCrbNivo = 0;
    TYTabTabPoint pts;
    //Pour chaque nom de calque suceptible de contenir des Courbes de Niveau (CUVETTE,...):
    for (int k = 0; CourbesNiveau[k].sCalque != NULL; k++)
    {
        //Pour chaque polyligne trouvee, creer une courbe de niveau Tympan
        int nNbPolylines = Get3DPolylinesInLayer(&(model[CourbesNiveau[k].nFile]),
                                                 CourbesNiveau[k].sCalque, &pts, Offset);
        for (int i = 0; i < nNbPolylines && nNbItem < nMaxCourbeNiveau; ++i)
        {
            TYTabPoint ptsOut;
            DecimatePolyline(m_dPolylineDecimationDistanceMin, &(pts[i]), &ptsOut);
            if (ptsOut.size() >= 3)
            {
                LPTYCourbeNiveau pCrbNiv = new TYCourbeNiveau();
                nNbItem++;
                int l = 0;
                for (l; l < (int)ptsOut.size(); ++l)
                {
                    pCrbNiv->addPoint(ptsOut.at(l));
                }
                nNbPointTotalCrbNivo += l;
                if (ptsOut.size())
                {
                    pCrbNiv->setAltitude(ptsOut.at(0)._z);    //sinon, les courbes apparaissent toutes en z=0
                }
                SetName(pCrbNiv, CourbesNiveau[k].sCalque, i + 1);
                bRes = pTYTopographie->addCrbNiv(pCrbNiv);//Err
                if (!bRes)
                {
                    Display("Impossible d'ajouter les courbes de niveau %s",
                            pCrbNiv->getName().toUtf8().data());
                    break;
                }
            }
        }
        pts.clear();
    }
    Display("%d Courbes de niveau importees, soit %d points",
            nNbItem, nNbPointTotalCrbNivo);
}


void TYDXFBDTopoImporter::importWatercourse(int nMaxCoursEau, const dimeModel model[],
                                            const DXFLimites& Offset,
                                            LPTYTopographie pTYTopographie)
{
    TYTabTabPoint pts;
    int nNbItem = 0;
    bool bRes;

    //Pour chaque nom de calque suceptible de contenir des Cours d'eau (NAT_1V,...):
    for (int k = 0; CoursEau[k].sCalque != NULL; ++k)
    {
        //Pour chaque polyligne trouvee, creer une Cours d'eau Tympan
        int nNbPolylines = Get3DPolylinesInLayer(&(model[CoursEau[k].nFile]), CoursEau[k].sCalque, &pts, Offset);
        for (int i = 0; i < nNbPolylines && nNbItem < nMaxCoursEau; ++i)
        {
            TYTabPoint ptsOut;
            DecimatePolyline(m_dPolylineDecimationDistanceMin, &(pts[i]), &ptsOut);
            if (ptsOut.size() >= 3)
            {
                LPTYCoursEau pTYCoursEau = new TYCoursEau;
                nNbItem++;
                pTYCoursEau->setLargeur(CoursEau[k].dLargeur);
                pTYCoursEau->setTabPoint(ptsOut);
                SetName(pTYCoursEau, CoursEau[k].sCalque, i + 1);
                bRes = pTYTopographie->addCrsEau(pTYCoursEau);//Err
                if (!bRes) //Err
                {
                    Display("Impossible d'ajouter le cours d'eau %s",
                            pTYCoursEau->getName().toUtf8().data());
                    break;
                }
            }
        }
        pts.clear();
    }
    Display("%d Cours d'eau importes",  nNbItem);
}

void TYDXFBDTopoImporter::importWaterplan(int nMaxPlanEau, const dimeModel model[],
                                          const DXFLimites& Offset,
                                          LPTYTopographie pTYTopographie)
{
    TYTabTabPoint pts;
    int nNbItem = 0;
    bool bRes;

    //Pour chaque nom de calque suceptible de contenir des PlansEau (S_C_EAU,...):
    for (int k = 0; PlansEau[k].sCalque != NULL; ++k)
    {
        //Pour chaque polyligne trouvee, creer un PlansEau Tympan
        int nNbPolylines = Get3DPolylinesInLayer(&(model[PlansEau[k].nFile]),
                                                 PlansEau[k].sCalque, &pts, Offset);
        for (int i = 0; i < nNbPolylines && nNbItem < nMaxPlanEau; ++i)
        {
            TYTabPoint ptsOut;
            DecimatePolyline(m_dPolylineDecimationDistanceMin, &(pts[i]), &ptsOut);
            if (ptsOut.size() >= 3)
            {
                LPTYPlanEau pTYPlanEau = new TYPlanEau;
                nNbItem++;
                pTYPlanEau->setListPoints(ptsOut);
                if (ptsOut.size())
                {
                    pTYPlanEau->setAltitude(ptsOut.at(0)._z);    //pour l'altimetrie
                }
                SetName(pTYPlanEau, PlansEau[k].sCalque, i + 1);
                bRes = pTYTopographie->addPlanEau(pTYPlanEau);
                if (!bRes) //Err
                {
                    Display("Impossible d'ajouter le plan d'eau %s",
                            pTYPlanEau->getName().toUtf8().data());
                    break;
                }
            }
        }
        pts.clear();
    }
    Display("%d Plans d'eau importes",  nNbItem);
}

void TYDXFBDTopoImporter::importField(int nMaxTerrain, const dimeModel model[],
                                      const DXFLimites& Offset,
                                      LPTYTopographie pTYTopographie)
{
    TYTabTabPoint pts;
    int nNbItem = 0;
    bool bRes;

    //Pour chaque model suceptible de contenir des Terrains:
    //Pour chaque nom de calque suceptible de contenir des Terrains (S_C_EAU,...):
    for (int k = 0; Terrains[k].sCalque != NULL; ++k)
    {
        //Pour chaque polyligne trouvee, creer un Terrains Tympan
        int nNbPolylines = Get3DPolylinesInLayer(&(model[Terrains[k].nFile]),
                                                 Terrains[k].sCalque, &pts, Offset);
        for (int i = 0; i < nNbPolylines && nNbItem < nMaxTerrain; ++i)
        {
            TYTabPoint ptsOut;
            DecimatePolyline(m_dPolylineDecimationDistanceMin, &(pts[i]), &ptsOut);
            if (ptsOut.size() >= 3)
            {
                // Definition du terrain par defaut
                //Ajout du dernier point = le premier
                ptsOut.push_back(ptsOut.at(0));
                LPTYTerrain pTerrain = new TYTerrain;
                nNbItem++;
                pTerrain->setListPoints(ptsOut);
                //Sol associe au terrain
                pTerrain->getSol()->setResistivite(Terrains[k].Resistivite); // Equivalent a terre battue; 60000 kRayls
                pTerrain->getSol()->setEpaisseur(Terrains[k].Epaisseur);
                if (Terrains[k].HauteurVegetation > 0)
                {
                    TYVegetation* current_vegetation =
                        pTerrain->getSol()->useVegetation(true);
                    current_vegetation->setHauteur(Terrains[k].HauteurVegetation);
                }
                SetName(pTerrain, Terrains[k].sCalque, i + 1);
                bRes = pTYTopographie->addTerrain(pTerrain);
                //pTopo->setDefTerrain(pTerrain);
                if (!bRes) //Err
                {
                    Display("Impossible d'ajouter le terrain %s",   pTerrain->getName().toUtf8().data());
                    break;
                }
            }
        }
        pts.clear();
    }
    Display("%d Terrains importes",     nNbItem);
}

void TYDXFBDTopoImporter::importRoad(int nMaxRoutes, const dimeModel model[],
                                     const DXFLimites& Offset,
                                     LPTYTopographie pTYTopographie,
                                     LPTYSiteNode pSite)
{
    TYTabTabPoint pts;
    int nNbItem = 0;
    bool bRes;

    //Pour chaque nom de calque suceptible de contenir des Routes (NAT_1V,...):
    for (int k = 0; Routes[k].sCalque != NULL; ++k)
    {
        //Pour chaque polyligne trouvee, creer une route Tympan
        int nNbPolylines = Get3DPolylinesInLayer(&(model[Routes[k].nFile]),
                                                 Routes[k].sCalque, &pts, Offset);
        for (int i = 0; i < nNbPolylines && nNbItem < nMaxRoutes; ++i)
        {
            TYTabPoint ptsOut;
            DecimatePolyline(m_dPolylineDecimationDistanceMin, &(pts[i]), &ptsOut);
            if (ptsOut.size() >= 3)
            {
                LPTYRoute pTYRoute = new TYRoute;
                nNbItem++;
                pTYRoute->setLargeur(Routes[k].dLargeur);
                pTYRoute->setTabPoint(ptsOut);
                SetName(pTYRoute, Routes[k].sCalque, i + 1);
                bRes = pSite->getInfrastructure()->addRoute(pTYRoute);//Err
                if (!bRes) //Err
                {
                    Display("Impossible d'ajouter la route %s",
                            pTYRoute->getName().toUtf8().data());
                    break;
                }
                //pCalcul->addToSelection(pTYRoute);
            }
        }
        pts.clear();
    }
    Display("%d Routes importees", nNbItem);
}

void TYDXFBDTopoImporter::importTransmissionNetwork(int nMaxReseauTransportNRJ,
                                                    const dimeModel model[],
                                                    const DXFLimites& Offset,
                                                    LPTYTopographie pTYTopographie,
                                                    LPTYSiteNode pSite)
{
    TYTabTabPoint pts;
    int nNbItem = 0;
    bool bRes;

    //Pour chaque nom de calque suceptible de contenir des Reseau de Transport d'NRJ (LE_M150,...):
    for (int k = 0; ReseauTransportNRJ[k].sCalque != NULL; ++k)
    {
        //Pour chaque polyligne trouvee, creer un Reseau de Transport d'NRJ Tympan
        int nNbPolylines = Get3DPolylinesInLayer(&(model[ReseauTransportNRJ[k].nFile]),
                                                 ReseauTransportNRJ[k].sCalque, &pts, Offset);
        for (int i = 0; i < nNbPolylines && nNbItem < nMaxReseauTransportNRJ; ++i)
        {
            LPTYReseauTransport pTYReseauTransport = new TYReseauTransport;
            nNbItem++;
            pTYReseauTransport->setLargeur(ReseauTransportNRJ[k].dLargeur);
            if (ReseauTransportNRJ[k].dTension > 0) //Temp : revoir la tension par defaut des basses lignes
            {
                pTYReseauTransport->setTension(ReseauTransportNRJ[k].dTension);
            }
            pTYReseauTransport->setTabPoint(pts[i]);
            double dHauteurMoyenne = pSite->getAltimetry()->HauteurMoyenne(pts[i]);
            if (dHauteurMoyenne == 0)
            {
                dHauteurMoyenne = ReseauTransportNRJ[k].dHauteurMoyenneSiIncalculable;
            }
            pTYReseauTransport->setHauteurMoyenne(dHauteurMoyenne);
            SetName(pTYReseauTransport, ReseauTransportNRJ[k].sCalque, i + 1);
            bRes = pSite->getInfrastructure()->addResTrans(pTYReseauTransport);//Err
            if (!bRes)
            {
                Display("Impossible d'ajouter le réseaux de transport %s",
                        pTYReseauTransport->getName().toUtf8().data());
                break;
            }
            //pCalcul->addToSelection(pTYReseauTransport);
        }
        pts.clear();
    }
    Display("%d Reseau de Transport importes",  nNbItem);
}


void TYDXFBDTopoImporter::importBuilding(int nMaxBatiment, const dimeModel model[],
                                         const DXFLimites& Offset,
                                         LPTYTopographie pTYTopographie,
                                         LPTYSiteNode pSite)
{
    TYTabTabPoint pts;
    int nNbItem = 0;

    //Pour chaque nom de calque suceptible de contenir des Batiment (S_C_EAU,...):
    for (int k = 0; Batiment[k].sCalque != NULL; ++k)
    {
        //Pour chaque polyligne trouvee, creer un Batiment Tympan
        int nNbPolylines = Get3DPolylinesInLayer(&(model[Batiment[k].nFile]),
                                                 Batiment[k].sCalque, &pts, Offset);
        for (int i = 0; i < nNbPolylines && nNbItem < nMaxBatiment; ++i)
        {
            LPTYBatiment  pBat = BuildConstruction(&(pts[i]), m_dMinHauteurBatiments,
                                                   (ETAGE == Batiment[k].eType),
                                                   (ECRAN_OUVERT != Batiment[k].eType),
                                                   pSite);
            if (pBat)
            {
                Display("Construction du batiment %s#%d a partir de la Polyligne %d/%d : hauteur %lf",
                        Batiment[k].sCalque, nNbItem, i, nNbPolylines,
                        m_dMinHauteurBatiments);
                //Nommage du batiment:
                SetName(pBat, Batiment[k].sCalque, i + 1);
                //pCalcul->addToSelection(pBat);
                nNbItem++;
            }
        }
        pts.clear();
    }

    Display("%d Constructions importees",   nNbItem);
}

QString TYDXFBDTopoImporter::GetDXFFileNameFromBDTopoPrefixAndDirectory(const char* BDTopoPrefix, const char* DXFsDirectory)
{
    QString SearchFor;
    SearchFor = BDTopoPrefix;
    SearchFor += "*.dxf";

    QString dirname(DXFsDirectory);
    const QDir dir(dirname);
    QStringList input_list(SearchFor);
    QFileInfoList file_list = dir.entryInfoList(input_list);

    QString res;
    if (file_list.count())
    {
        // Just kept the first element.
        res = file_list.at(0).absoluteFilePath();
    }
    return res;
}

/*
// Ces fonctions ne sont pas appelees

//////////////////////////////////////////////////////////////////////
// Procedures de test
//////////////////////////////////////////////////////////////////////
bool TYDXFBDTopoImporter::TestAltimetrieBDTOPO(LPTYSiteNode pSite)
{
    LPTYTopographie pTYTopographie = pSite->getTopographie();
    //Test de l'altimetrie
    double xcoord[] = {0.0, 1.0,10.0,100.0,1000.0,-1};
    double ycoord[] = {0.0, 1.0,10.0,100.0,1000.0,-1};
    for(int i = 0; xcoord[i] >= 0; i++){
        OPoint3D pt;
        pt._x = xcoord[i];
        pt._y = ycoord[i];
        bool  bOK = pTYTopographie->getAltimetrie()->altitude(pt);
        if(!bOK)
            return false;
    }
    return true;
}

bool TYDXFBDTopoImporter::TestConstructionBDTOPO(LPTYSiteNode pSite)
{
    BDTOPO_Batiment BatimentTest[]=
    {
        {NULL, "TEST_ETAGE", ETAGE},
        {NULL,"TEST_ECRAN_OUVERT", ECRAN_OUVERT},
        {NULL,"TEST_ECRAN_FERME", ECRAN_FERME},
        {NULL,NULL},
    };

    double dHauteur = 50;

    for(int w = 0; BatimentTest[w].sCalque != NULL; w++)
    {
        TYTabPoint pointsTests;
        const double dDemiLargeur = 4 + w*10;
        const double dDemiLongueur = 5 + w*10;
        TYPoint pt1(-dDemiLongueur, dDemiLargeur, 0);
        TYPoint pt2(dDemiLongueur, dDemiLargeur, 0);
        TYPoint pt3(dDemiLongueur, -dDemiLargeur, 0);
        TYPoint pt4(-dDemiLongueur, -dDemiLargeur, 0);

        pointsTests.push_back(pt1);
        pointsTests.push_back(pt2);
        pointsTests.push_back(pt3);
        pointsTests.push_back(pt4);

        LPTYBatiment  pBat = BuildConstruction(&pointsTests, dHauteur, (ETAGE==BatimentTest[w].eType), (ECRAN_OUVERT==BatimentTest[w].eType), pSite);
        if(pBat)
        {
            Display("Test de Construction du batiment %s#%d : hauteur %lf", BatimentTest[w].sCalque, w, dHauteur);
            //Nommage du batiment:
            SetName(pBat, BatimentTest[w].sCalque, w);
        }
        else
        {
            return false;
        }
    }
    return true;
}
*/

#endif
