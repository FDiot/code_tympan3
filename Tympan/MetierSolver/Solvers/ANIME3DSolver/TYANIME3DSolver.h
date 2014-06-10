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


#ifndef __TY_ANIME3DSOLVER__
#define __TY_ANIME3DSOLVER__

#include "Tympan/MetierSolver/AcousticRaytracer/Tools/Logger.h"
#include "Tympan/MetierSolver/CommonTools/OMatrix.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticSurface.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSourcePonctuelle.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYPointCalcul.h"
#include "Tympan/MetierSolver/CommonTools/Acoustic_path.h"
#include "Tympan/MetierSolver/DataManagerCore/TYSolverInterface.h"


class TYANIME3DAcousticModel;
class TYANIME3DAcousticPathFinder;
class TYANIME3DFaceSelector;

class Lancer;

//Structure permettant de stocker les informations de Tympan dans un format aisement convertible
//Les infos idFace, idBuilding, idEtage, spectreAbsoMat, G ne sont pas obligatoires.
//De meme, il est possible de rajouter des informations lors de la collecte pour repondre a un besoin d'une methode acoustique
//Exemple : le dev peut choisir de conserver la resistivite plutot que le G.
struct TYStructSurfIntersect
{
    TYAcousticSurfaceGeoNode* pSurfGeoNode; //Geonode de la surface
    OMatrix matInv;                         // Matrice inverse pour les faces d'infrastructure
    TYTabPoint tabPoint;                    // Tableau de point utilise pour la preselection
    bool isEcran;                           // Est un ecran
    bool isInfra;                           // Face d'infrastructure
    OVector3D normal;                       //Normale de la surface
    QList<OTriangle> triangles;             //Ensemble des triangles contenus dans la surface apres triangulation
    QList<OPoint3D> realVertex;              //Ensemble des vertex de la surface en coordonn�es globales.
    int idFace;                             //Indice de la face
    int idBuilding;                         //Indice du building dans TYSite
    int idEtage;                            //Indice de l'etage
    OSpectreComplex spectreAbso;            //Spectre d'absorption
    double G;                               //Coefficient d'impedance
};


/*
 * \class TYANIME3DSolver
 * \brief Le solver associe a la methode ANIME3D
 */

class TYANIME3DSolver : public TYSolverInterface
{
public:
    /**
    * \fn TYANIME3DSolver()
    * \brief Constructeur de l'objet ANIME3DSolver
    */
    TYANIME3DSolver();

    /*!
    * \fn ~TYANIME3DSolver()
    * \brief Destructeur de l'objet ANIME3DSolver
    */
    ~TYANIME3DSolver();

    /*!
    * \fn bool solve(const TYSiteNode&, TYCalcul&, const AcousticProblemModel&, AcousticResultModel&)
    * \brief Methode principale de resolution avec la methode ANIME3D
    */
    virtual bool solve(const TYSiteNode& site, TYCalcul& calcul,
            const tympan::AcousticProblemModel& aproblem, tympan::AcousticResultModel& aresult);

    /*!
    * \fn void init(const TYSiteNode&, TYCalcul&)
    * \brief Initialize some data structures used by ANIME3D
    */
    void init(const TYSiteNode& site, TYCalcul& calcul);


    /*!
     * \fn virtual void purge();
     * \brief Clean memory after being used in a calcul
     */
    virtual void purge();

protected:
    /*!
     * \fn void saveAndOverSampleRay(const double& dMin = 0.0)
     * \brief Transforme les ray en TYRay et ajoute des points intermediaires sur les rayons
     */
    void saveAndOverSampleRay(const TYSiteNode& site, TYCalcul& calcul, const unsigned int& sens = 0, const double& dMin = 0.0);

protected:
    tab_acoustic_path _tabRay;

    /// Tableau contenant l'ensemble des infos relatives a la geometrie d'un site et les materiaux associes a chaque face
    TYStructSurfIntersect* _tabPolygon;

    /// Nombre de polygones presents dans _tabPolygon
    size_t _tabPolygonSize;

    Logger logger;
    FILE logs;

    /*!< List of sources used by the solver */
    TYTabSourcePonctuelleGeoNode _tabSources;

    /*!< List of receptors used by the solver */
    TYTabPointCalculGeoNode _tabRecepteurs;
};

#endif // __TY_ANIME3DSOLVER__
