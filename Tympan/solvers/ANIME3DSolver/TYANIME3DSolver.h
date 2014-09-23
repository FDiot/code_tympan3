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

#include <memory>
#include <QList>

#include "Tympan/core/interfaces.h"
#include "Tympan/models/common/3d.h"
#include "Tympan/models/common/triangle.h"
#include "Tympan/models/common/acoustic_path.h"
#include "Tympan/models/solver/entities.hpp"
#include "Tympan/solvers/AcousticRaytracer/Tools/Logger.h"

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
    OMatrix matInv;                         // Matrice inverse pour les faces d'infrastructure
    TabPoint3D tabPoint;                    // Tableau de point utilise pour la preselection
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
    tympan::AcousticMaterialBase *material;  //triangle material
};


/*
 * \class TYANIME3DSolver
 * \brief Le solver associe a la methode ANIME3D
 */

class TYANIME3DSolver : public SolverInterface
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
    * \fn bool solve(const AcousticProblemModel&, AcousticResultModel&)
    * \brief Methode principale de resolution avec la methode ANIME3D
    */
    virtual bool solve(const tympan::AcousticProblemModel& aproblem,
                       tympan::AcousticResultModel& aresult);

    /*!
    * \fn void init()
    * \brief Initialize some data structures used by ANIME3D
    */
    void init();


    /*!
     * \fn virtual void purge();
     * \brief Clean memory after being used in a calcul
     */
    virtual void purge();

protected:
    tab_acoustic_path _tabRay;

    /// Tableau contenant l'ensemble des infos relatives a la geometrie d'un site et les materiaux associes a chaque face
    TYStructSurfIntersect* _tabPolygon;

    /// Nombre de polygones presents dans _tabPolygon
    size_t _tabPolygonSize;

    //Logger logger;
    //FILE logs;

    /*!< List of sources used by the solver */
    //TYTabSourcePonctuelleGeoNode _tabSources;

    /*!< List of receptors used by the solver */
    //TYTabPointCalculGeoNode _tabRecepteurs;

    tympan::source_pool_t all_sources;
    tympan::receptor_pool_t all_receptors;

private:
   std::unique_ptr<AtmosphericConditions> _pAtmos;
};

#endif // __TY_ANIME3DSOLVER__
