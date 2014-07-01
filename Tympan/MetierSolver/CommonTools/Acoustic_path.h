#ifndef __ACOUSTIC_PATH_H_
#define __ACOUSTIC_PATH_H_

#include "Tympan/models/common/3d.h"

enum ACOUSTIC_EVENT_TYPES
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

class acoustic_event
{
    /*!
    * \class acoustic_event
    * \brief This class store data and provide functions to manipulate event in the acoustic context
    * \author Projet_Tympan
    */

public:
    acoustic_event();

    acoustic_event(const OPoint3D& pt);

    acoustic_event(const acoustic_event& ev);

    ~acoustic_event();

    acoustic_event& operator=(const acoustic_event& other);
	
    OPoint3D pos;           		/*!< Position de l'evenement */
    double distNextEvent;   		/*!< Distance between this event and the next one in TYRay's list of events */
    double distEndEvent;    		/*!< Distance between this event and the next event needed for calculating (for ex. reflexion after a diffraction) */
    double distPrevNext;    		/*!< Distance between event-1 and event +1 */
    double angle;           		/*!< Angle d'incidence du rayon  (pour l'angle de tir - plan x,z -)*/
    double angletheta;      		/*!< Angle de tir sur un plan horizontal (x,y) */
    ACOUSTIC_EVENT_TYPES type;      /*!< Type d'evenement */
    int idFace1;            		/*!< identifiant de la face sur laquelle a lieu l'evenement (reflexion & diffraction)*/
    int idFace2;            		/*!< identifiant de la face sur laquelle a lieu l'evenement (diffraction seulement)*/
    acoustic_event* previous;   	/*!< Pointer to the previous event in TYRay's list of events*/
    acoustic_event* next;       	/*!< Pointer to the next event in TYRay's list of events*/
    acoustic_event* endEvent;   	/*!< Pointer to the next event in TYRay's list of events needed for calculating (for ex. reflexion after a diffraction) */
};

typedef std::vector<acoustic_event*> tab_acoustic_events;

class Ray;

struct IGeometryModifier
{
    virtual ~IGeometryModifier() {};

    virtual OPoint3D fonction_h(const OPoint3D&) = 0;
    virtual OPoint3D fonction_h_inverse(const OPoint3D&) = 0;
};

class acoustic_path
{
public:
	acoustic_path();
	virtual ~acoustic_path();
	
	/*!
    * \fn acoustic_path(unsigned int source_idx, unsigned int receptor_idx, std::vector<TYRayEvent*> &_events);
    * \brief Construction d'un chemin acoustique avec un identifiant de source, un identifiant de récepteur et 
    *        une liste de positions d'événements acoustiques (réflexion,  diffraction, etc...)
    */
    acoustic_path(unsigned int source_idx, unsigned int receptor_idx, tab_acoustic_events &_events);

    /*!
    * \fn acoustic_path( const acoustic_path &ray)
    * \brief Constructeur par copie d'un chemin acoustique
    */
    acoustic_path(const acoustic_path& ray);

    /*!
     * \fn void cleanEventsTab();
     * \brief clean tab of events
     */
    virtual void cleanEventsTab();

    /*!
     * \fn acoustic_path& operator=(const acoustic_path& other)
     * \brief equal operator
     */
    virtual acoustic_path& operator=(const acoustic_path& other);

    /*!
     * \fn bool deepCopy(acoustic_path *pOther)
     * \brief Deep copy of a ray mainly the events tab
     */
    virtual bool deepCopy(acoustic_path* pOther);

    /*!
     * \fn void tyRayCorrection(IGeometryModifier& transformer);
     * \brief Curve acoustic_path with respect to meteo influence
     *        This is only for watching curved rays on screen
     */
    virtual void tyRayCorrection(IGeometryModifier& transformer);

    /*!
    * \fn void setSource(unsigned int source_idx_, OPoint3D& globalPosition);
    * \brief Place la source du rayon. 
    */
    virtual void setSource(unsigned int source_idx_, OPoint3D& globalPosition);

    /*!
    * \fn void setSource(unsigned int source_idx_)
    * \brief Place la source du rayon.
    */
    virtual void setSource(unsigned int source_idx_) { source_idx = source_idx_; }


    /*!
    * \fn TYSourcePonctuelle* getSource()
    * \brief Recuperation de la source du rayon
    */
    virtual unsigned int getSource_idx() {return source_idx;}

    /*!
    * \fn void setRecepteur(unsigned  int receptor_idx_, OPoint3D& globalPosition)
    * \brief Place le recepteur du rayon. Le dernier point de la polyligne est mis a jour
    */
    virtual void setRecepteur(unsigned  int receptor_idx_, OPoint3D& globalPosition);

    /*!
    * \fn void setRecepteur(unsigned  int receptor_idx_)
    * \brief Place le recepteur du rayon. 
    */
    virtual void setRecepteur(unsigned  int receptor_idx_) { receptor_idx = receptor_idx_; }


    /*!
    * \fn unsigned int getRecepteur_idx()
    * \brief Recuperation du recepteur du rayon
    */
    virtual unsigned int getRecepteur_idx() {return receptor_idx;}

    /*!
    * \fn void setIdentifiant(unsigned int _id)
    * \brief Set l'identifiant du rayon.
    */
    virtual void setIdentifiant(unsigned int id) {_identifiant = id;}

    /*!
    * \fn int getIdentifiant()
    * \brief Renvoie l'indentifiant du rayon.
    */
    virtual unsigned int getIdentifiant() const {return _identifiant;}

    /*!
    * \fn void addEvent(Event &_event)
    * \brief Ajoute un evenement a la liste des evenements du rayon
    */
    virtual void addEvent(acoustic_event* TYEvent) {_events.push_back(TYEvent);}

    /*!
    * \fn std::vector<Event>& getEvents()
    * \brief Renvoie la liste des i¿½vi¿½nements rencontri¿½s par le rayon
    */
    virtual tab_acoustic_events& getEvents() {return _events;}
    virtual const tab_acoustic_events& getEvents() const { return _events; }

    /*!
    * \fn void setTabPoint(TYTabPoint &_tabPoint)
    * \brief Place le tableau de point correspondant au parcours du rayon.
    * \warning Le premier acoustic_event doit toujours correspondre a la source 
	* \warning et sa position globale et le dernier point doit toujours correspondre 
	* \warning a un recepteur et sa position globale du
    */
    virtual void setEvents(tab_acoustic_events& tabEvents) { _events = tabEvents; }

    /*!
     * \fn vector<unsigned int> getIndexOfEvents(const int& eventType)
     * \brief return a tab of indexes of events of the same type in a ray
     *        you can merge two types of events (ex. TYREFLEXION | TYRECEPTEUR)
              vector returned count all events of the two types
     */
    virtual std::vector<int> getIndexOfEvents(const int& eventType) const;

    /*!
     * \fn unsigned int getNbEventType(const ACOUSTIC_EVENT_TYPES& eventType)
     * \brief return the number of events of a type
     */
    virtual unsigned int getNbEventType(const ACOUSTIC_EVENT_TYPES& eventType) { return getIndexOfEvents(eventType).size(); }


    /*!
     * \fn void copyEvents(ACOUSTIC_EVENT_TYPES eventType);
     * \brief copy only events matching eventType to _events tab
     */
    virtual void copyEvents(const acoustic_path* tyRay, ACOUSTIC_EVENT_TYPES eventType);

    /*!
     * \fn void setNextDistance(const ACOUSTIC_EVENT_TYPES& eventType);
     * \brief Compute distance between events of the type "eventType"
     *        and set distNextEvent to each event matching enventType
     */
    virtual void setNextDistance(ACOUSTIC_EVENT_TYPES eventType);

    /*!
     * \fn void setAngles(ACOUSTIC_EVENT_TYPES eventType)
     * \brief Compute angles of incoming ray segment at event point
     */
    virtual void setAngles(ACOUSTIC_EVENT_TYPES eventType);

    /*!
    * \fn double getLength()
    * \brief Return total length of the ray taking account of all events
    */
    virtual double getLength();

    /*!
    * \fn int getnbEvents()
    * \brief Renvoie le nombre d'evenements compris dans le rayon
    */
    virtual int getnbEvents() {return _events.size();}

    /*!
    * \fn void setPosSourceGlobal(const OPoint3D& pos)
    * \brief Set de la position de la source dans le repi¿½re global
    */
    virtual void setPosSourceGlobal(const OPoint3D& pos) { _posSourceGlobal = pos; }

    /*!
    * \fn OPoint3D& getPosSourceGlobal()
    * \brief Get de la position de la source dans le repere global
    */
    virtual OPoint3D& getPosSourceGlobal() { return _posSourceGlobal; }

    /*!
    * \fn void setPosReceptGlobal(const OPoint3D& pos)
    * \brief Set de la position du recepteur dans le repere global
    */
    virtual void setPosReceptGlobal(const OPoint3D& pos) { _posReceptGlobal = pos; }

    /*!
    * \fn OPoint3D& getPosReceptGlobal()
    * \brief Get de la position du recepteur dans le repere global
    */
    virtual OPoint3D& getPosReceptGlobal() { return _posReceptGlobal; }

    /*!
    * \fn void overSampleTYRay(TYRay* tyRay);
    * \brief Rajoute des evenements aux rayons de type TYRay
    */
    virtual void overSample(const double& dMin);

    /*!
    * \fn void sampleAndCorrection()
    * \brief Computes angle and length correction
    * \ by calling the three previous functions
    * \ Creates two matrix which have corrected lengths and angles
    */
    virtual void sampleAndCorrection(IGeometryModifier& transformer);

    /*!
     * \fn void endLenghtCompute(TYRay *tyRay)
     * \brief compute the length between an event and the next pertinent event
     *        (i.e. betwween a diffraction and the next reflection or receptor)
     */
    virtual void endLenghtCompute(IGeometryModifier& transformer);

    /*!
     * \fn void angleCompute(TYRay *tyRay)
     * \brief compute the angle between incident ray and the face
     */
    virtual void angleCompute(IGeometryModifier& transformer);

    /*!
     * \fn void nextLenghtCompute(TYRay *tyRay)
     * \brief compute the length between an event and the next event
     */
    virtual void nextLenghtCompute(IGeometryModifier& transformer);

    /*!
     * \fn void prevNextLengthCompute(TYRay *tyRay)
     * \brief Computes the length between event-1 and event+1
     */
    virtual void prevNextLengthCompute(IGeometryModifier& transformer);

    /*!
     * \fn void nextLenghtCompute(TYRay *tyRay)
     * \brief compute the length between an event and the next event
     */
    virtual void eventPosCompute(IGeometryModifier& transformer);

    /*!
    * \fn void lengthCorrection()
    * \brief Computes length correction on path
    * \ Works on over-sampled TYRays
    * \ Creates a vector which has all corrected path lengths
    */
    virtual double lengthCorrection(acoustic_event* ev1, const acoustic_event* ev2, IGeometryModifier& transformer);

    /*!
    * \fn void angleCorrection()
    * \brief Computes angle correction on path
    *        ev1 -> previous event
    *        ev2 -> event to wich compute anngle
    *        ev3 -> next event
    */
    virtual double angleCorrection(const acoustic_event* ev1, acoustic_event* ev2, const acoustic_event* ev3, IGeometryModifier& transformer);

	static void set_sampler_step(double sampler_step_) { sampler_step = sampler_step_; }

private :


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
	unsigned int _identifiant;		/*!< Identifiant du rayon */
    unsigned int source_idx;
    unsigned int receptor_idx;
    OPoint3D _posSourceGlobal;
    OPoint3D _posReceptGlobal;

    tab_acoustic_events _events;			/*!< Vecteurs d'evenements contenant la liste des evenements du rayon associe et leurs positions.*/
};	

typedef std::vector<acoustic_path*> tab_acoustic_path;

#endif //__ACOUSTIC_PATH_H_