/*
 * Copyright (c) 2007 EDF. All Rights Reserved.
 * All Rights Reserved. No part of this software and its documentation may
 * be used, copied, modified, distributed and transmitted, in any form or by
 * any means, without the prior written permission of EDF.
 *
 * EDF-R&D Departement AMA - 1, avenue du general de Gaulle, 92141 CLAMART
 *
 *
 *
 * $Id: TYDXFBDTOPOImporter.h,v 1.2 2008/02/13 08:04:58 thomasso Exp $
 *
 */

#ifndef __TY_DXF_BDTOPO_IMPORTER__
#define __TY_DXF_BDTOPO_IMPORTER__

#ifdef TY_USE_DXF

#include "TYDXFImporter.h"

/**
 * Classe singleton permettant d'importer des donnees DXF au format BD-TOPO dans des objets metiers Tympan.
 * cf descriptif de livraison "BD-TOPO Vision 1.2", Edition 2 Mars 1999
 *
 * @author Arnaud Zirnheld
 */
class TYDXFBDTopoImporter  :    public TYDXFImporter
{
    double m_dPolylineDecimationDistanceMin;
    double m_dMinHauteurBatiments;
public:
    /**
     * Constructeur par defaut.
     * @param dPolylineDecimationDistanceMin Distance minimale entre deux points, servant a decimer les polylignes importees des fichiers DXF
     * @param dMinHauteurBatiments hauteur en dessous de laquelle on n'importe pas les batiments
    */
    TYDXFBDTopoImporter(double dPolylineDecimationDistanceMin = 25.0,
                        double dMinHauteurBatiments = 10.);
    /**
     * Destructeur.
     */
    virtual ~TYDXFBDTopoImporter();
    /**
     * Point d'entree de l'import des fichiers DXF de BD-TOPO:
     */
    virtual bool import(const char* FullBDDPath, LPTYSiteNode* pSiteIn);
    // Import topographical entities.
    void importIsoline(int nMaxCourbeNiveau, const dimeModel model[],
                       const DXFLimites& Offset,
                       LPTYTopographie pTYTopographie);
    void importWatercourse(int nMaxCoursEau, const dimeModel model[],
                           const DXFLimites& Offset,
                           LPTYTopographie pTYTopographie);
    void importWaterplan(int nMaxPlanEau, const dimeModel model[],
                         const DXFLimites& Offset,
                         LPTYTopographie pTYTopographie);
    void importField(int nMaxTerrain, const dimeModel model[],
                     const DXFLimites& Offset,
                     LPTYTopographie pTYTopographie);
    // Import buildings and other built entities (as roads, etc.).
    void importRoad(int nMaxRoutes, const dimeModel model[],
                    const DXFLimites& Offset,
                    LPTYTopographie pTYTopographie,
                    LPTYSiteNode pSite);
    void importTransmissionNetwork(int nMaxReseauTransportNRJ, const dimeModel model[],
                                   const DXFLimites& Offset,
                                   LPTYTopographie pTYTopographie,
                                   LPTYSiteNode pSite);
    void importBuilding(int nMaxBatiment, const dimeModel model[],
                        const DXFLimites& Offset,
                        LPTYTopographie pTYTopographie,
                        LPTYSiteNode pSite);

private:
    QString GetDXFFileNameFromBDTopoPrefixAndDirectory(const char* BDTopoPrefix, const char* DXFsDirectory);
    //Pour le debug:
    //  bool    TestConstructionBDTOPO(LPTYSiteNode pSite);
    //  bool    TestAltimetrieBDTOPO(LPTYSiteNode pSite);
};

#else

class TYDXFBDTopoImporter
{
public:
    /**
     * Constructeur par defaut.
     * @param dPolylineDecimationDistanceMin Distance minimale entre deux points, servant a decimer les polylignes importees des fichiers DXF
     * @param dMinHauteurBatiments hauteur en dessous de laquelle on n'importe pas les batiments
    */
    TYDXFBDTopoImporter() {};
    /**
     * Destructeur.
     */
    virtual ~TYDXFBDTopoImporter() {};
};

#endif //TY_USE_DXF

#endif // __TY_DXF_BDTOPO_IMPORTER__
