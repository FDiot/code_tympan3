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

#ifndef _TYRAY_
#define _TYRAY_

#include "Tympan/MetierSolver/DataManagerCore/TYElement.h"

#include <vector>

enum TY_RAY_EVENT
{
    TY_NO_TYPE = 0,
    TYDIFFRACTION = 1,
    TYREFLEXION = 2,
    TYREFLEXIONSOL = 4,
    TYREFRACTION = 8,
    TYSOURCE = 16,
    TYRECEPTEUR = 32,
    TY_ALL_TYPE = TYDIFFRACTION | TYREFLEXION | TYREFLEXIONSOL | TYREFRACTION | TYSOURCE | TYRECEPTEUR
};

class Event;

class TYRayEvent
{
    /*!
    * \class TYRayEvent
    * \brief This class store data and provide functions to manipulate event in the acoustic context
    * \author Projet_Tympan
    */

public:
    TYRayEvent();

    TYRayEvent(const OPoint3D& pt);

    TYRayEvent(const TYRayEvent& ev);

    ~TYRayEvent();

    TYRayEvent& operator=(const TYRayEvent& other);

    static TYRayEvent build_from_RayEvent(const Event *rev);

    OPoint3D pos;           /*!< Position de l'evenement */
    double distNextEvent;   /*!< Distance between this event and the next one in TYRay's list of events */
    double distEndEvent;    /*!< Distance between this event and the next event needed for calculating (for ex. reflexion after a diffraction) */
    double distPrevNext;    /*!< Distance between event-1 and event +1 */
    double angle;           /*!< Angle d'incidence du rayon  (pour l'angle de tir - plan x,z -)*/
    double angletheta;      /*!< Angle de tir sur un plan horizontal (x,y) */
    TY_RAY_EVENT type;      /*!< Type d'evenement */
    int idFace1;            /*!< identifiant de la face sur laquelle a lieu l'evenement (reflexion & diffraction)*/
    int idFace2;            /*!< identifiant de la face sur laquelle a lieu l'evenement (diffraction seulement)*/
    TYRayEvent* previous;   /*!< Pointer to the previous event in TYRay's list of events*/
    TYRayEvent* next;       /*!< Pointer to the next event in TYRay's list of events*/
    TYRayEvent* endEvent;   /*!< Pointer to the next event in TYRay's list of events needed for calculating (for ex. reflexion after a diffraction) */
};

typedef std::vector<TYRayEvent*> TYTabRayEvent;
class TYPointCalcul;
class TYSourcePonctuelle;
class Ray;
class geometry_modifier;

/*!
* \file TYRay.h
* \class TYRay
* \brief Classe decrivant un rayon acoustique gere par un lancer de rayon. Cette classe doit permettre la modelisation de rayons pour l'affichage de Tympan.
* \todo : ajout des listes de diffractions/reflexions
* \author Projet_Tympan
*/
class TYRay: public TYElement
{
    OPROTODECL(TYRay)
    TY_EXT_GRAPHIC_DECL_ONLY(TYRay)

public:

    /*!
    * \fn TYRay()
    * \brief Constructeur par defaut
    */
    TYRay();

    /*!
    * \fn TYRay(TYSourcePonctuelle *_sourceP, TYPointControl *_recepP, TYTabPoint &_polyligne)
    * \brief Construction d'un rayon avec une source, un recepteur et une liste de positions d'i申vi申nements acoustiques (ri申flexion, diffraction, etc...)
    * \warning Hypothese : Le premier point de la polyligne correspond i申 la position de la source en coordonni申es globales et le dernier point de la polyligne
    * correspond a la position du recepteur en coordonni申es globales. Si ces conditions ne sont pas respecti申es, la fonction renvoie un rayon vide equivalent i申 TYRay()
    */
    TYRay(TYSourcePonctuelle* _sourceP, TYPointCalcul* _recepP, std::vector<TYRayEvent*> &_events);

    /*!
    * \fn TYRay( const TYRay &ray)
    * \brief Constructeur par copie d'un rayon
    */
    TYRay(const TYRay& ray);

    // Destructor
    ~TYRay();

    static TYRay build_from_Ray(int sens, Ray* ray);

    /*!
     * \fn void cleanEventsTab();
     * \brief clean tab of events
     */
    void cleanEventsTab();

    /*!
     * \fn TYRay& operator=(const TYRay& other)
     * \brief equal operator
     */
    virtual TYRay& operator=(const TYRay& other);

    /*!
     * \fn bool deepCopy(TYRay *pOther)
     * \brief Deep copy of a ray mainly the events tab
     */
    bool deepCopy(TYRay* pOther);

    /*!
     * \fn void TYRayCorrection(TYRay& tyRay)
     * \brief Curve TYRay with respect to meteo influence
     *        This is only for watching curved rays on screen
     */
    void tyRayCorrection(geometry_modifier& transformer);

    /*!
    * \fn void setSource(TYSourcePonctuelle *_source OPoint3D &globalPosition)
    * \brief Place la source du rayon. Le premier point de la polyligne est mis a jour
    */
    void setSource(TYSourcePonctuelle* _source, OPoint3D& globalPosition);

    /*!
    * \fn void setSource(TYSourcePonctuelle *_source)
    * \brief Place la source du rayon.
    */
    void setSource(TYSourcePonctuelle* source) { _source = source; }


    /*!
    * \fn TYSourcePonctuelle* getSource()
    * \brief Recuperation de la source du rayon
    */
    TYSourcePonctuelle* getSource() {return _source;}

    /*!
    * \fn void setRecepteur(TYPointControl *_recep OPoint3D &globalPosition)
    * \brief Place le recepteur du rayon. Le dernier point de la polyligne est mis a jour
    */
    void setRecepteur(TYPointCalcul* _recep, OPoint3D& globalPosition);

    /*!
    * \fn void setRecepteur(TYPointCalcul *)
    * \brief Place le recepteur du rayon. Le dernier point de la polyligne est mis a jour
    */
    void setRecepteur(TYPointCalcul* recep) { _recepteur = recep; }


    /*!
    * \fn TYPointControl* getRecepteur()
    * \brief Recuperation du recepteur du rayon
    */
    TYPointCalcul* getRecepteur() {return _recepteur;}

    /*!
    * \fn void setIdentifiant(int _id)
    * \brief Set l'identifiant du rayon.
    */
    void setIdentifiant(int id) {_identifiant = id;}

    /*!
    * \fn int getIdentifiant()
    * \brief Renvoie l'indentifiant du rayon.
    */
    int getIdentifiant() const {return _identifiant;}

    /*!
    * \fn void addEvent(Event &_event)
    * \brief Ajoute un evenement a la liste des evenements du rayon
    */
    void addEvent(TYRayEvent* TYEvent) {_events.push_back(TYEvent);}

    /*!
    * \fn std::vector<Event>& getEvents()
    * \brief Renvoie la liste des i申vi申nements rencontri申s par le rayon
    */
    TYTabRayEvent& getEvents() {return _events;}
    const TYTabRayEvent& getEvents() const { return _events; }

    /*!
    * \fn void setTabPoint(TYTabPoint &_tabPoint)
    * \brief Place le tableau de point correspondant au parcours du rayon.
    * \warning Le premier TYRayEvent doit toujours correspondre a la source et sa position globale et le dernier point doit toujours correspondre a un recepteur et sa position globale du
    */
    void setEvents(TYTabRayEvent& tabEvents) { _events = tabEvents; }

    /*!
     * \fn vector<unsigned int> getIndexOfEvents(const int& eventType)
     * \brief return a tab of indexes of events of the same type in a ray
     *        you can merge two types of events (ex. TYREFLEXION | TYRECEPTEUR)
              vector returned count all events of the two types
     */
    std::vector<int> getIndexOfEvents(const int& eventType) const;

    /*!
     * \fn unsigned int getNbEventType(const TY_RAY_EVENT& eventType)
     * \brief return the number of events of a type
     */
    unsigned int getNbEventType(const TY_RAY_EVENT& eventType) { return getIndexOfEvents(eventType).size(); }


    /*!
     * \fn void copyEvents(TY_RAY_EVENT eventType);
     * \brief copy only events matching eventType to _events tab
     */
    void copyEvents(const TYRay* tyRay, TY_RAY_EVENT eventType);

    /*!
     * \fn void setNextDistance(const TY_RAY_EVENT& eventType);
     * \brief Compute distance between events of the type "eventType"
     *        and set distNextEvent to each event matching enventType
     */
    void setNextDistance(TY_RAY_EVENT eventType);

    /*!
     * \fn void setEndDistance(const TY_RAY_EVENT& endEventType);
     * \brief Compute distance between each events and next event
     *        of type "eventType"and set distEndEvent to each event
     */
    void setEndDistance(const TY_RAY_EVENT& endEventType);

    /*!
     * \fn void setAngles(TY_RAY_EVENT eventType)
     * \brief Compute angles of incoming ray segment at event point
     */
    void setAngles(TY_RAY_EVENT eventType);


    /*!
    * \fn double getNbrReflexions()
    * \brief Renvoie le nombre de reflexions qui ont lieu le long de ce rayon
    */
    int getNbrReflexions();

    /*!
    * \fn double getNbrDiffractions()
    * \brief Renvoie le nombre de diffractions qui ont lieu le long de ce rayon
    */
    int getNbrDiffractions();

    /*!
    * \fn double getLength()
    * \brief Return total length of the ray taking account of all events
    */
    double getLength();

    /*!
    * \fn int getnbEvents()
    * \brief Renvoie le nombre d'evenements compris dans le rayon
    */
    int getnbEvents() {return _events.size();}

    /*!
    * \fn void setPosSourceGlobal(const OPoint3D& pos)
    * \brief Set de la position de la source dans le repi申re global
    */
    void setPosSourceGlobal(const OPoint3D& pos) { _posSourceGlobal = pos; }

    /*!
    * \fn OPoint3D& getPosSourceGlobal()
    * \brief Get de la position de la source dans le repere global
    */
    OPoint3D& getPosSourceGlobal() { return _posSourceGlobal; }

    /*!
    * \fn void setPosReceptGlobal(const OPoint3D& pos)
    * \brief Set de la position du recepteur dans le repere global
    */
    void setPosReceptGlobal(const OPoint3D& pos) { _posReceptGlobal = pos; }

    /*!
    * \fn OPoint3D& getPosReceptGlobal()
    * \brief Get de la position du recepteur dans le repere global
    */
    OPoint3D& getPosReceptGlobal() { return _posReceptGlobal; }

    /*!
    * \fn void overSampleTYRay(TYRay* tyRay);
    * \brief Rajoute des evenements aux rayons de type TYRay
    */
    void overSample(const double& dMin);

    /*!
    * \fn void sampleAndCorrection()
    * \brief Computes angle and length correction
    * \ by calling the three previous functions
    * \ Creates two matrix which have corrected lengths and angles
    */
    void sampleAndCorrection(geometry_modifier& transformer);

    /*!
     * \fn void endLenghtCompute(TYRay *tyRay)
     * \brief compute the length between an event and the next pertinent event
     *        (i.e. betwween a diffraction and the next reflection or receptor)
     */
    void endLenghtCompute(geometry_modifier& transformer);

    /*!
     * \fn void angleCompute(TYRay *tyRay)
     * \brief compute the angle between incident ray and the face
     */
    void angleCompute(geometry_modifier& transformer);

    /*!
     * \fn void nextLenghtCompute(TYRay *tyRay)
     * \brief compute the length between an event and the next event
     */
    void nextLenghtCompute(geometry_modifier& transformer);

    /*!
     * \fn void prevNextLengthCompute(TYRay *tyRay)
     * \brief Computes the length between event-1 and event+1
     */
    void prevNextLengthCompute(geometry_modifier& transformer);

    /*!
     * \fn void nextLenghtCompute(TYRay *tyRay)
     * \brief compute the length between an event and the next event
     */
    void eventPosCompute(geometry_modifier& transformer);

    /*!
    * \fn void lengthCorrection()
    * \brief Computes length correction on path
    * \ Works on over-sampled TYRays
    * \ Creates a vector which has all corrected path lengths
    */
    double lengthCorrection(TYRayEvent* ev1, const TYRayEvent* ev2, geometry_modifier& transformer);

    /*!
    * \fn void angleCorrection()
    * \brief Computes angle correction on path
    *        ev1 -> previous event
    *        ev2 -> event to wich compute anngle
    *        ev3 -> next event
    */
    double angleCorrection(const TYRayEvent* ev1, TYRayEvent* ev2, const TYRayEvent* ev3, geometry_modifier& transformer);

	static void set_sampler_step(double sampler_step_) { sampler_step = sampler_step_; }

private :
    /*!
     * \fn void buildListEvent(const int& sens, const Ray ray, std::list<TYEvent>& listEvent)
     * \brief construit une liste d'evenement TYEvent a partir d'un rayon
     */
    void build_event_list_from_Ray(int sens, Ray* ray);

    /*!
     * \fn void build_links_between_events();
     * \brief TYRayEvent has to know is direct neighbour (before and after him)
     * \brief It olso has to know his the next REFLEXION event or (if not exist) the RECEPTOR
     */
    void build_links_between_events();

    /*!
     * \fn void compute_shot_angle();
     * \brief Compute shot angle from source
     */
    void compute_shot_angle();

protected:
	static double sampler_step;		/*!< max size of step between events after spatial sampling*/
	int _identifiant;				/*!< Identifiant du rayon */
    TYSourcePonctuelle* _source;	/*!< Pointeur vers la source du rayon */
    TYPointCalcul* _recepteur;		/*!< Pointeur vers le recepteur du rayon */
    OPoint3D _posSourceGlobal;		
    OPoint3D _posReceptGlobal;
    
    TYTabRayEvent _events;			/*!< Vecteurs d'evenements contenant la liste des evenements du rayon associe et leurs positions.*/
};
//Smart Pointer sur TYRay
typedef SmartPtr<TYRay> LPTYRay;

typedef std::vector<LPTYRay> TYTabRay;

#endif
