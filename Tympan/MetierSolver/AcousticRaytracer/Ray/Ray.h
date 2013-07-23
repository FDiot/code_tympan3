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
 
#ifndef RAY_H
#define RAY_H

#include <QSharedPointer>
#include "Tympan/MetierSolver/AcousticRaytracer/Acoustic/Source.h"
//#include "Tympan/MetierSolver/AcousticRaytracer/Acoustic/Recepteur.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Acoustic/Event.h"

#include "Tympan/MetierSolver/AcousticRaytracer/Base.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/mathlib.h"
using namespace std;

//#ifdef USE_QT
//  #include "RayGraphic.h"
//#endif


class Ray : public Base
{

public:
    //#ifdef USE_QT
    //  //WIDGET_DECL(Sphere)
    //  GRAPHIC_DECL(Ray)
    //#endif

    Ray() : Base(), position(), direction(), mint(0), maxt(100000), source(NULL), recepteur(NULL), nbReflexion(0), nbDiffraction(0) { name = "unknow ray"; }
    Ray(const vec3& _position, const vec3& _direction) : Base(), position(_position), direction(_direction), mint(0), maxt(100000), source(NULL), recepteur(NULL), nbReflexion(0), nbDiffraction(0)
    { name = "unknow ray";}

    Ray(const Ray& other) : Base(other)
    {
        position = vec3(other.position);
        direction = vec3(other.direction);
        mint = other.mint;
        maxt = other.maxt;
        source = other.source;
        recepteur = other.recepteur;
        constructId = other.constructId;
        for (unsigned int i = 0; i < other.events.size(); i++)
        {
            events.push_back(other.events.at(i));
        }

        nbDiffraction = other.nbDiffraction;
        nbReflexion = other.nbReflexion;
    }

    Ray(Ray* other)
    {
        position = vec3(other->position);
        direction = vec3(other->direction);
        mint = other->mint;
        maxt = other->maxt;
        source = other->source;
        recepteur = other->recepteur;
        constructId = other->constructId;
        for (unsigned int i = 0; i < other->events.size(); i++)
        {
            events.push_back(other->events.at(i));
        }

        nbDiffraction = other->nbDiffraction;
        nbReflexion = other->nbReflexion;
    }

    virtual ~Ray() { }

    /*!
    * \fn void computeLongueur()
    * \brief Calcul la distance parcourue par le rayon et place le resultat dans longueur
    */
    void computeLongueur();

    /*!
    * \fn double getLongueur()
    * \brief Renvoie la distance parcourue par le rayon
    * \return Distance parcourue par le rayon.
    */
    double getLongueur() {return longueur;}

    /*!
    * \fn double getDiff()
    * \brief Renvoie le nombre de diffractions realisees par le rayon parcourue par le rayon
    * \return Nombre de diffractions realisees par le rayon.
    */
    unsigned int getDiff() { return nbDiffraction; }

    /*!
    * \fn double getReflex()
    * \brief Renvoie le nombre de reflexions realisees par le rayon parcourue par le rayon
    * \return Nombre de reflexions realisees par le rayon.
    */
    unsigned int getReflex() { return nbReflexion; }

    /*!
    * \fn double getEvents()
    * \brief Renvoie le tableau des evenements rencontres par le rayon
    * \return Tableau des evenements rencontres par le rayon.
    */
    std::vector<QSharedPointer<Event> >* getEvents() { return &events; }
    /*!
    * \fn float distanceSourceRecepteur()
    * \brief Calcul la distance entre une source et un recepteur
    * \return Distance entre la source et le recepteur associes au rayon
    */
    float distanceSourceRecepteur();

    vector<unsigned int>getFaceHistory();

    vector<unsigned int> getPrimitiveHistory();

    /*!
    * \fn Source* getSource()
    * \brief Renvoie la source d'un rayon
    * \return Pointeur vers la source du rayon
    */
    Source* getSource() { return source; }

    /*!
    * \fn Recepteur* getRecepteur()
    * \brief Renvoie le recepteur d'un rayon.
    * \return Pointeur vers le recepteur rayon. Renvoie NULL si le rayon n'a pas de recepteur associe
    */
    void* getRecepteur() { return recepteur; }

public:
    vec3 position;                              /*!< Point de depart du rayon */
    vec3 direction;                             /*!< vecteur directeur du rayon a la source */
    vec3 finalPosition;                         /*!< Point d'arrivee du rayon */
    decimal mint;
    decimal maxt;
    Source* source;                             /*!< Pointeur vers la source du rayon */
    void* recepteur;                            /*!< Pointeur vers le recepteur du rayon */
    std::vector<QSharedPointer<Event> > events; /*!< Liste d'evenements subis par le rayon */
    decimal longueur;                           /*!< Distance parcourue par le rayon */
    unsigned long long int constructId;         /*!< Identifiant du rayon */
    unsigned int nbReflexion;                   /*!< Nombre de reflexions subis par le rayon */
    unsigned int nbDiffraction;                 /*!< Nombre de diffractions subis par le rayon */
};


#endif
