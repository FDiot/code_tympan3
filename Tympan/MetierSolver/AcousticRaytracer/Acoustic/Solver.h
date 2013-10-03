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
 
#ifndef SOLVER_H
#define SOLVER_H

#include "Tympan/MetierSolver/AcousticRaytracer/Ray/Ray.h"
#include <vector>


#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/Scene.h"
#include "Recepteur.h"
#include "Tympan/MetierSolver/AcousticRaytracer/global.h"

/*!
* \file Solver.h
* \class Solver
* \brief La classe Solver donne une interface au developpeur pour introduire facilement une nouvelle methode acoustique utilisant le lancer de rayons.
* Cette classe donne la main a chaque point du lancer de rayons ou la methode acoustique intervient dans le comportement de la simulation. Elle permet
* d'ajouter un eventail d'objets acoustiques pour modeliser certains phenomenes comme les diffractions. Les fonctionnalites assurees par cette classe
* sont les suivante : post-traitement de la scene, traitement des feuilles dans les structures acceleratrices, validation d'une intersection (generation de
* l'evenement approprie), validation d'un rayon et exportation. Voir la documentation des prototypes pour plus d'informations.
* \warning Le developpeur doit bien faire attention a ce que les fonctions utilisees dans la bibliotheques soient coherentes entre elles. Si par
* exemple le developpeur a choisi de post-traiter la scene en ajoutant des aretes de diffraction mais qu'il n'a pas definit de fonction de validation
* pour les aretes, alors chaque intersection avec une arete sera ignoree.
*/

class Solver
{
public:
    /*!
    * \fn Solver()
    * \brief Constructeur de base du Solver.
    */
    Solver()
    {
    }

    /*!
    * \fn Solver(Solver *_solver)
    * \brief Constructeur par copie. L'ensemble des rayons contenus dans le solver de base sont transmis a la copie.
    * \param _solver : Pointeur vers le solver a copier.
    */
    Solver(Solver* _solver)
    {
    }

    /*!
    * \fn initGlobalValues()
    * \brief Initialise les valeures globales pour la simulation. Definie aussi bien les valeurs de la simulation que du solver.
    */
    virtual void initGlobalValues()
    {
        globalMaxProfondeur = 0;            //Nombre d'evenements autorises pour un rayon, globalMaxProfondeur inclu
        globalNbRaysPerSource = 100;        //Nombre de rayons lances par les sources
        globalSizeReceiver = 1;             //Diametre de la sphere representant le recepteur
        globalAccelerator = 1;              //Choix de la structure acceleratrice. 0 : BruteForce, 1 : GridAccelerator, 2 : BVH, 3 : KdTree, other : GridAccelerator
        globalMaxTreeDepth = 0;
        globalUseSol = false;
    }

    /*!
    * \fn virtual bool postTreatmentScene(Scene *scene)
    * \brief Fonction virtuelle post traitant la scene. Cette phase a 2 objectifs : transformer les meta-objets et augmenter la scene. La phase
    * de transformation des meta-objets est assez directe : on parcourt les differentes listes de meta-objets et on en extrait des elements utilisables
    * par le lancer de rayon. Typiquement, cela va consister a transformer un objet Mur en un ensemble de Primitives, extraire des Sources poncutelles
    * des polylignes de sources etc. L'augmentation de la scene consiste en l'ajout d'objets plus propres au calcul acoustique qu'on ne retrouvent pas dans
    * la description physique de la scene. Un exemple est les aretes de diffractions qui sont modelisees par un cylindre et doivent etre generes.
    * \param scene : Scene a post-traiter.
    * \param sources : Ensemble des sources pouvant etre modifiees
    * \param recepteurs : Ensemble des recepteurs pouvant etre modifiees
    * \return Renvoie vrai si l'ensemble des operations se sont bien deroulees.
    */
    virtual bool postTreatmentScene(Scene* scene, std::vector<Source>& sources, std::vector<Recepteur>& recepteurs);

    /*!
    * \fn virtual double leafTreatment(vector<struct Isect> &primitives)
    * \brief Fonction de traitement des feuilles des structures acceleratrice. Le comportement par defaut est de garder uniquement la premiere intersection.
    * On peut cependant choisir de garder toutes les intersections s'etant produit avant un type d'intersection. Par exemple, on peut choisir de garder
    * l'ensemble des intersections se produisant avant une intersection avec un triangle qui est considere comme bloquant.
    * \param Ensemble des primitives rencontrees dans la feuille courante.
    * \return Temps maximal autorise par le rayon. Correspond au temps d'intersection avant la premiere primitive bloquante.
    */
    virtual double leafTreatment(vector<Intersection> &primitives);

    /*!
    * \fn virtual double leafTreatment(vector<struct Isect> &primitives)
    * \brief Fonction de validation d'une intersection. Si l'intersection est valide, une evenement est genere et ajouter sur le rayon.
    * \param r : Rayon a traiter.
    * \param p : Primitive a valider pour le rayon.
    * \return Renvoie vrai si la validation a bien pu s'effectuer.
    * \warning Les fonctions de validations doivent etre coherent par rapport au type de primitives contenues dans la scene. Les types de primitives
    * non supportees doivent remonter une erreur si elles ne sont pas gerer dans la simulation.
    */
    virtual bool valideIntersection(Ray* r, Intersection* inter);

    /*!
    * \fn virtual bool valideRayon(Ray *r)
    * \brief Validation d'un rayon. Le developpeur peut, par exemple, choisir de faire un filtrage sur les rayons et de ne valider que les chemins uniques.
    * Si le rayon est valide, il est place dans la liste de rayons valides et sera utilisable pour l'exportation.
    * \param r : Rayon a valider. Le rayon a comme hypothese d'avoir une source et un recepteur.
    * \return Renvoie vrai si le rayon a ete valide.
    */
    virtual bool valideRayon(Ray* r);

    virtual bool invalidRayon(Ray* r);



    /*!
    * \fn vector<Ray*>* getValidRays()
    * \brief Renvoie la liste des rayons valides.
    * \return Pointeur vers le vecteur contenant les rayons valides par le solveur.
    */
    virtual vector<Ray*>* getValidRays() { return &valid_rays;}

    /*!
    * \fn vector<Ray*>* getDebugRays()
    * \brief Renvoie la liste des rayons invalides.
    * \return Pointeur vers le vecteur contenant les rayons invalides par le solveur.
    */
    vector<Ray*>* getDebugRays() { return &debug_rays;}

    virtual void finish();

    virtual void clean();

    /*!
    * \fn bool loadParameters()
    * \brief Charge les parametres du calcul
    * \return booleen indiquant le bon deroulement de l'operation.
    */
    virtual bool loadParameters();

protected:
    vector<Ray*> valid_rays;    /*!< Liste des rayons valides par le solveur */
    vector<Ray*> debug_rays;    /*!< List des rayons invalides par le solveur */
};

#endif // SOLVER_H
