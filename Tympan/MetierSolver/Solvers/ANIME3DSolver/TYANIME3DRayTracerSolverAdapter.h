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

#ifndef TYANIME3D_RAYTRACER_SOLVER_ADAPTER_H
#define TYANIME3D_RAYTRACER_SOLVER_ADAPTER_H

#include "Tympan/MetierSolver/AcousticRaytracer/Tools/SelectorManager.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Acoustic/Solver.h"

/**
* \class TYANIME3DRayTracerSetUp
* \brief Classe implementant l'interface pour piloter le lancer de rayons pour le solver ANIME3D. (cree a partir de la classe SimpleRaySolver)
*/

class TYANIME3DRayTracerSolverAdapter : public Solver
{

public:
    /*!
    * \fn virtual bool postTreatmentScene(Scene *scene, std::vector<Source>& sources, std::vector<Recepteur>& recepteurs)
    * \brief Fonction virtuelle post traitant la scene. Cette phase a 2 objectifs : transformer les meta-objets et augmenter la scene. La phase
    * de transformation des meta-objets est assez directe : on parcourt les differentes listes de meta-objets et on en extrait des elements utilisables
    * par le lancer de rayon.
    * Typiquement, cela va consister a transformer un objet Mur en un ensemble de Primitives, extraire des Sources poncutelles des polylignes de sources etc.
    * L'augmentation de la scene consiste en l'ajout d'objets plus propres au calcul acoustique qu'on ne retrouve pas dans la description physique de la scene.
    * Un exemple est les aretes de diffractions qui sont modelisees par un cylindre et doivent etre generees.
    * \param scene : Scene a post-traiter.
    * \return Renvoie vrai si l'ensemble des operations se sont bien deroulees.
    */
    virtual bool postTreatmentScene(Scene* scene, std::vector<Source>& sources, std::vector<Recepteur>& recepteurs);

    /*!
    * \fn virtual double leafTreatment(vector<struct Isect> &primitives)
    * \brief Fonction de traitement des feuilles des structures acceleratrices. Le comportement par defaut est de garder uniquement la premiere intersection.
    * On peut cependant choisir de garder toutes les intersections s'etant produites avant un type d'intersection.
    * Par exemple, on peut choisir de garder l'ensemble des intersections se produisant avant une intersection avec un triangle qui est considere comme bloquant.
    * \param Ensemble des primitives rencontrees dans la feuille courante.
    * \return Temps maximal autorise par le rayon. Correspond au temps d'intersection avant la premiere primitive bloquante.
    */
    //virtual double leafTreatment(KdTree *kdtree, BVH* bvh, Ray *r, vector<struct Isect> &primitives);

    /*!
    * \fn virtual bool valideIntersection(Ray *r, Intersection* inter)
    * \brief Fonction de validation d'une intersection. Si l'intersection est valide, une evenement est genere et ajoute sur le rayon.
    * \param r : Rayon a traiter.
    * \param inter : Intersection a valider
    * \return Renvoie vrai si la validation a bien pu s'effectuer.
    * \warning Les fonctions de validations doivent etre coherentes par rapport au type de primitives contenues dans la scene. Les types de primitives
    * non supportees doivent remonter une erreur si elles ne sont pas gerees dans la simulation.
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

    /*!
    * \fn virtual bool invalidRayon(Ray *r)
    * \brief Rangement des rayons non valides.
    * Les rayons non valides sont places dans le tableau debug_ray pour permettre la visualisation de tous les rayons
    * \param r : Rayon a place parmi les rayons invalides.
    * \return Renvoie vrai le rayon a ete invalide.
    */
    virtual bool invalidRayon(Ray* r);

    /*!
    * \fn vector<Ray*>* getValidRays()
    * \brief Renvoie la liste des rayons valides.
    * \return Pointeur vers le vecteur contenant les rayons valides par le solveur.
    */
    //vector<Ray*>* getValidRays(){ return &valid_rays;}


    virtual void finish();

    //virtual void clean();

    bool _useFresnelArea;


protected:
    SelectorManager<Ray> selectorManagerIntersection;
    SelectorManager<Ray> selectorManagerValidation;

#ifdef _ALLOW_TARGETING_
protected :
    TargetManager targetManager;
#endif //_ALLOW_TARGETING_
};

#endif //TYANIME3D_RAYTRACER_SOLVER_ADAPTER_H
