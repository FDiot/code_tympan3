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
 * $Id: TYDXFBDTOPOSpecifications.h,v 1.2 2008/02/13 08:04:58 thomasso Exp $
 *
 */

//Ce fichier definit les valeurs des constantes intervenant dans l'importation des fichiers DXF BD-Topo
#ifndef __TY_DXF_BDTOPO_SPECIFICATIONS__
#define __TY_DXF_BDTOPO_SPECIFICATIONS__

#ifdef TY_USE_DXF

//////////////////////////////////////////////////////////////////////////
//      FICHIERS DXF
//////////////////////////////////////////////////////////////////////////
//Attention!!! Ne pas changer l'ordre de l'enum DXFModelNumber (ou alors changer aussi celui de DXFPrefixFileName)
enum DXFModelNumber
{
    ALT,
    VCR,
    FTE,
    HYD,
    LID,
    BAT,
    VEG,
    ORO
};

const char* DXFPrefixFileName[] =
{
    "ALT",//0=ALT
    "VCR",//1=VCR
    "FTE",//2=FTE
    "HYD",//3=HYD
    "LID",//4=LID
    "BAT",//5=BAT
    "VEG",//6=VEG
    "ORO"//7=ORO
};
//////////////////////////////////////////////////////////////////////////
//      Courbes de niveau
//////////////////////////////////////////////////////////////////////////
typedef struct BDTOPO_CourbesNiveau
{
    int nFile;
    const char* sCalque;
} BDTOPO_CourbesNiveau;

BDTOPO_CourbesNiveau CourbesNiveau[] =
{
    {ALT, "CUVETTE"},
    {ALT, "COURBE",},
    {ORO, "LIG_DES"},
    {0, NULL}
};
//////////////////////////////////////////////////////////////////////////
//      Cours d'eau
//////////////////////////////////////////////////////////////////////////
typedef struct BDTOPO_CoursEau
{
    int nFile;
    const char* sCalque;
    double dLargeur;
} BDTOPO_CoursEau;

BDTOPO_CoursEau CoursEau[] =
{
    {HYD, "T_C_EAU", 2.00},
    {HYD, "T_C_TMP", 2.00},
    {HYD, "AQUEDUC", 1.00},
    {HYD, "CON_FOR", 1.00},
    {HYD, "CASCADE", 7.50},
    {0, NULL, 0.00}
};

//////////////////////////////////////////////////////////////////////////
//      Plans d'eau
//////////////////////////////////////////////////////////////////////////
typedef struct BDTOPO_PlansEau
{
    int nFile;
    char* sCalque;
} BDTOPO_PlansEau;

BDTOPO_PlansEau PlansEau[] =
{
    {HYD, "S_C_EAU"},
    {HYD, "S_C_TMP"},
    {HYD, "SCANALA"},
    {HYD, "BASSIN"},
    {HYD, "RES_EAU"},
    {HYD, "TER_NAT"},
    {HYD, "S_HYDRO"},
    {HYD, "SAB_VAS"},
    {HYD, "GRA_GAL"},
    {HYD, "ROC_HYD"},
    {HYD, "VEG_AQU"},
    {HYD, "MAR_SAL"},
    {HYD, "EAU_MAR"},
    {0, NULL}
};



/////////////////////////////////////////////////////////////////////////
//      Terrains
/////////////////////////////////////////////////////////////////////////

typedef struct BDTOPO_Terrains
{
    int nFile;
    char* sCalque;
    double Resistivite;//TYSol
    double Epaisseur;//TYSol
    double HauteurVegetation;
} BDTOPO_Terrains;



#define RESISTIVITE_GOUDRON     6000
#define RESISTIVITE_BETON       5000
#define RESISTIVITE_GAZON       1000
#define RESISTIVITE_ROCHERS     4000
#define RESISTIVITE_BOIS        2500
#define RESISTIVITE_BROUSSAILLE 3000
#define RESISTIVITE_VERGER      1000
#define RESISTIVITE_VIGNE       1000
#define RESISTIVITE_SABLE       2000
#define RESISTIVITE_TERRAIN_MEUBLE  2000

#define EPAISSEUR_GOUDRON       0.5
#define EPAISSEUR_BETON         0.5
#define EPAISSEUR_GAZON         0.2
#define EPAISSEUR_ROCHERS       0.2
#define EPAISSEUR_BOIS          0.2
#define EPAISSEUR_BROUSSAILLE   0.2
#define EPAISSEUR_VERGER        0.2
#define EPAISSEUR_VIGNE         0.2
#define EPAISSEUR_SABLE         0.3
#define EPAISSEUR_TERRAIN_MEUBLE    0.2

#define HAUTEUR_GAZON           0.2
#define HAUTEUR_BOIS            10.0
#define HAUTEUR_BROUSSAILLE     1.0
#define HAUTEUR_VERGER          2.0
#define HAUTEUR_VIGNE           1.0


BDTOPO_Terrains Terrains[] =
{
    {VCR, "S_PONTF", RESISTIVITE_GOUDRON,       EPAISSEUR_GOUDRON, -1.0},//-1 signifie ici "terrain sans vegetation" (= vegetation non active)
    {VCR, "S_PONTM", RESISTIVITE_GOUDRON,       EPAISSEUR_GOUDRON, -1.0},
    {VCR, "CARF_AM", RESISTIVITE_GOUDRON,       EPAISSEUR_GOUDRON, -1.0},
    {VCR, "PARC_ST", RESISTIVITE_GOUDRON,       EPAISSEUR_GOUDRON, -1.0},
    {VCR, "PEAGE",  RESISTIVITE_GOUDRON,        EPAISSEUR_GOUDRON, -1.0},
    {VCR, "AIRE",   RESISTIVITE_GOUDRON,        EPAISSEUR_GOUDRON, -1.0},
    {BAT, "PIS_AER", RESISTIVITE_GOUDRON,       EPAISSEUR_GOUDRON, -1.0},
    {BAT, "DAL_PRO", RESISTIVITE_BETON,         EPAISSEUR_BETON,   -1.0},
    {BAT, "TER_FOO", RESISTIVITE_GAZON,         EPAISSEUR_GAZON, HAUTEUR_GAZON},
    {BAT, "TER_TEN", RESISTIVITE_GOUDRON,       EPAISSEUR_GOUDRON, -1.0},
    {BAT, "CIMETIE", RESISTIVITE_ROCHERS,       EPAISSEUR_ROCHERS, -1.0},
    {VEG, "BOIS",   RESISTIVITE_BOIS,           EPAISSEUR_BOIS, HAUTEUR_BOIS},
    {VEG, "BROUSSA", RESISTIVITE_BROUSSAILLE,    EPAISSEUR_BROUSSAILLE, HAUTEUR_BROUSSAILLE},
    {VEG, "VERGER", RESISTIVITE_VERGER,         EPAISSEUR_VERGER, HAUTEUR_VERGER},
    {VEG, "VIGNE",  RESISTIVITE_VIGNE,          EPAISSEUR_VIGNE, HAUTEUR_VIGNE},
    {VEG, "MANGROV", RESISTIVITE_BOIS,           EPAISSEUR_BOIS, HAUTEUR_BOIS},
    {VEG, "CAN_SUC", RESISTIVITE_VIGNE,          EPAISSEUR_VIGNE, HAUTEUR_VIGNE},
    {VEG, "BANANE", RESISTIVITE_BOIS,           EPAISSEUR_BOIS, HAUTEUR_BOIS},
    {ORO, "GLACIER", RESISTIVITE_BOIS,           EPAISSEUR_BOIS, HAUTEUR_BOIS},
    {ORO, "MORAINE", RESISTIVITE_BOIS,           EPAISSEUR_BOIS, HAUTEUR_BOIS},
    {ORO, "ROC_ORO", RESISTIVITE_ROCHERS,       EPAISSEUR_ROCHERS, -1.0},
    {ORO, "EBOULIS", RESISTIVITE_ROCHERS,       EPAISSEUR_ROCHERS, -1.0},
    {ORO, "SAB_SEC", RESISTIVITE_SABLE,         EPAISSEUR_SABLE, -1.0},
    {ORO, "FON_CUV", RESISTIVITE_SABLE,         EPAISSEUR_SABLE, -1.0},/*materiau ?*/
    {ORO, "RAVINES", RESISTIVITE_TERRAIN_MEUBLE, EPAISSEUR_TERRAIN_MEUBLE, -1.0},
    {ORO, "BANANE", RESISTIVITE_BOIS,           EPAISSEUR_BOIS, HAUTEUR_BOIS},
    {0, NULL, 0, 0, 0}
};

//////////////////////////////////////////////////////////////////////////
//      Routes
//////////////////////////////////////////////////////////////////////////
typedef struct BDTOPO_Routes
{
    int nFile;
    char* sCalque;
    double dLargeur;
} BDTOPO_Routes;

BDTOPO_Routes Routes[] =
{
    {VCR, "ROUNREV", 3.50},
    {VCR, "ROUCONS", 3.50},
    {VCR, "AUT_1V", 3.50},
    {VCR, "NAT_1V", 3.50},
    {VCR, "DEP_1V", 3.50},
    {VCR, "ROU_1V", 3.50},
    {VCR, "PONT_F", 3.50},
    {VCR, "PONT_M", 3.50},
    {VCR, "GUE", 3.50},
    {VCR, "AUT_2V", 7.00},
    {VCR, "NAT_2V", 7.00},
    {VCR, "DEP_2V", 7.00},
    {VCR, "ROU_2V", 7.00},
    {VCR, "AUT_34V", 10.50},
    {VCR, "NAT_3V", 10.50},
    {VCR, "DEP_3V", 10.50},
    {VCR, "ROU_3V", 10.50},
    {VCR, "AUT_P4V", 14.00},
    {VCR, "NAT_4VP", 14.00},
    {VCR, "DEP_4VP", 14.00},
    {VCR, "ROU_4VP", 14.00},
    {FTE, "NB1PALE", 1.0},
    {FTE, "VF_SPEC", 1.0},
    {FTE, "NB2PALE", 2.0},
    {FTE, "NB3PALE", 3.0},
    {FTE, "NB4PALE", 4.0},
    {FTE, "NB1PALN", 1.44},
    {FTE, "NB2PALN", 2.88},
    {FTE, "NB3PALN", 4.32},
    {FTE, "NB4PALN", 5.76},
    {0, NULL, 0.00}
};

//////////////////////////////////////////////////////////////////////////
//      Reseau de Transport
//////////////////////////////////////////////////////////////////////////
typedef struct BDTOPO_ReseauTransportNRJ
{
    int nFile;
    char* sCalque;
    double dLargeur;
    double dTension;
    double dHauteurMoyenneSiIncalculable;//a cause d'un mauvais reglage de l'emprise (entrainant l'incapacite d'obtenir l'altitude d'un point)
} BDTOPO_ReseauTransportNRJ;


BDTOPO_ReseauTransportNRJ ReseauTransportNRJ[] =
{
    {FTE, "LE_M150", 5, -1, 10.0},//tension a -1 signifie que la tension par defaut sera choisit
    {FTE, "LE_P150", 15, 150000.0, 50.0},
    {0, NULL, 0, 0}
};


//////////////////////////////////////////////////////////////////////////
//      Constructions
//////////////////////////////////////////////////////////////////////////
typedef enum BDTOPO_BatimentType { ECRAN_OUVERT, ECRAN_FERME, ETAGE};
typedef struct BDTOPO_Batiment
{
    int nFile;
    const char* sCalque;
    int eType;
} BDTOPO_Batiment;

BDTOPO_Batiment Batiment[] =
{
    {FTE, "PARAPET", ECRAN_OUVERT},
    {FTE, "GARE", ETAGE},
    {FTE, "CENTRAL", ECRAN_FERME},
    {FTE, "TRANSFO", ECRAN_FERME},
    {FTE, "R_MATPR", ETAGE},
    {HYD, "STA_POM", ECRAN_FERME},
    {HYD, "BAR_DUR", ECRAN_OUVERT},
    {HYD, "AVA_BAR", ECRAN_OUVERT},
    {HYD, "BAR_TER", ECRAN_OUVERT},
    {HYD, "CHA_EAU", ETAGE},
    {HYD, "ECLUSE", ECRAN_FERME},
    {LID, "MUR", ECRAN_OUVERT},
    {LID, "MUR_SOU", ECRAN_OUVERT},
    {BAT, "REG_BAT", ETAGE},
    {BAT, "BAT_IND", ETAGE},
    {BAT, "SILO", ETAGE},
    {BAT, "SERRE", ETAGE},
    {BAT, "CON_LEG", ETAGE},
    {BAT, "RUINE", ECRAN_OUVERT},
    {BAT, "SBATREM", ETAGE},
    {BAT, "SCONSPE", ETAGE},
    {BAT, "REFUGE", ETAGE},
    {BAT, "BATREL0", ETAGE},
    {BAT, "BATREL1", ETAGE},
    {BAT, "ARENE", ETAGE},
    {BAT, "MONUMEN", ETAGE},
    {BAT, "SAL_SPO", ETAGE},
    {BAT, "TRIBUNE", ETAGE},
    {BAT, "SVESMIL", ETAGE},
    {BAT, "TRIBUNE", ETAGE},
    {BAT, "ENC_COM", ETAGE},
    {BAT, "ENC_IND", ECRAN_FERME}, /* ETAGE avant le 25/08/03, ECRAN_OUVERT avant le 27/08/03 (=> pas de hauteur ?!)*/
    {BAT, "ENC_PUB", ECRAN_FERME}, /* ETAGE avant le 25/08/03, ECRAN_OUVERT avant le 27/08/03 (=> pas de hauteur ?!)*/
    {0, NULL, 0}
};

#endif //TY_USE_DXF

#endif// __TY_DXF_BDTOPO_SPECIFICATIONS__
