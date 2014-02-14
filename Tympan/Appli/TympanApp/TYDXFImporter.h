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
 * $Id: TYDXFImporter.h,v 1.2 2008/02/13 08:04:58 thomasso Exp $
 *
 */

#ifndef __TYDXF_IMPORTER__
#define __TYDXF_IMPORTER__

#include "Tympan/MetierSolver/DataManagerCore/TYDefines.h"

#ifdef TY_USE_DXF

class dimeModel;
///Collection de TYTabPoint:
typedef std::vector<TYTabPoint> TYTabTabPoint;

//! Store a kinf of 'boundary' object with a min/max.
struct DXFLimites
{
    double x;
    double y;
};

/**
 * Classe singleton offrant des methodes de lecture de donnees DXF
 */
class TYDXFImporter
{
public:
    /**
     * Constructeur par defaut.
    */
    TYDXFImporter() {};
    /**
     * Destructeur.
     */
    virtual ~TYDXFImporter() {};
    /**
     * Point d'entree de l'import des fichiers DXF de BD-TOPO:
     */
    virtual bool import(const char* FullBDDPath, LPTYSiteNode* pSiteIn) = 0;
    //Surcouche Dime :
    /**
     * Remplit un model Dime avec un fichier DXF
     */
    bool    ReadDXF(dimeModel* model, const QString& DXFFileName);
protected:
    /**
     * Renvoie le rectangle dans le plan XY enlobant la scene DXF 'model'
     */
    bool    GetCurrentLimites(const dimeModel* model, DXFLimites& CurrentMin, DXFLimites& CurrentMax);
    /**
     * Mise a jour des variables limites avec celles du nouveau 'model'
     */
    bool    RefreshMinMaxModels(const dimeModel* model, DXFLimites& LimiteMin, DXFLimites& LimiteMax);
    /**
     * Renvoie la liste des polylines translatees de Offset se trouvant dans le calque LayerName appartenant a la scene DXF 'model'
     */
    int     Get3DPolylinesInLayer(const dimeModel* model, const char* LayerName, TYTabTabPoint* pts, DXFLimites Offset);
    /**
     * Renvoie la liste des points translatees de Offset se trouvant dans le calque LayerName appartenant a la scene DXF 'model'
     */
    int     Get3DPointsInLayer(const dimeModel* model, const char* LayerName, TYTabPoint* pts, DXFLimites Offset);


    //Base pour les importers de geometrie (pas forcement DXF) :
    /**
     * Construit un batiment a partir d'une liste de points, et l'ajoute au site
     */
    LPTYBatiment    BuildConstruction(TYTabPoint* pts, double dHauteur, bool bBatiment, bool bForceFermetureContour, LPTYSiteNode pSite);
    /**
     * Construit un silo a partir d'une liste de points, et l'ajoute au site
     */
    LPTYBatiment    BuildSilo(TYTabPoint* pts, double dHauteur, LPTYSiteNode pSite);
    /**
     * Construit une machine, et l'ajoute au site
     */
    LPTYMachine     BuildMachine(TYTabPoint* pts, double dHauteur, LPTYSiteNode pSite);
    /**
     * Supprime des points dans une polyligne. Critere de decimation : distance minimale entre 2 points consecutifs
     */
    void            DecimatePolyline(double dPolylineDecimationDistanceMin, TYTabPoint* ptsIn, TYTabPoint* ptsOut);
    /**
     * Renvoie les limites 2D d'un tableau de points
     */
    bool            Get2DBoundingBox(TYTabPoint* pts, double& minX, double& maxX, double& minY, double& maxY);

    /**
     * Nommage des objets metiers avec un n° d'instance
     */
    void    SetName(LPTYElement pTYElement, const char* sName, int nInstanceNumber);

    /**
     * Recherche d'un float dans les preferrences
     */
    void    LoadFloatParam(const char* ParamName, double& dParam);

    //Pour le debug:
    void    Display(const char* format, ...);
};

#else

/**
 * Classe singleton offrant des methodes de lecture de donnees DXF
 */
class TYDXFImporter
{
public:
    /**
     * Constructeur par defaut.
    */
    TYDXFImporter() {};
    /**
     * Destructeur.
     */
    virtual ~TYDXFImporter() {};
};

#endif

#endif // __TYDXF_IMPORTER__
