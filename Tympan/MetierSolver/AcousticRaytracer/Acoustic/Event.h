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
 
#ifndef EVENT_H
#define EVENT_H

//#ifdef USE_QT
//
//#include <QtGui>
//#endif

#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/Sampler.h"
#include <vector>
#include "Tympan/MetierSolver/AcousticRaytracer/Base.h"

enum typeevent
{
    SPECULARREFLEXION = 0,
    DIFFRACTION
};

class Shape;

class Event : public Base
{

    //#ifdef USE_QT
    //  //WIDGET_DECL(Recepteur)
    //  GRAPHIC_DECL(Event)
    //#endif

public:
    Event(const vec3& position = vec3(0.0, 0.0, 0.0), const vec3& incomingDirection = vec3(0.0, 0.0, 0.0), Shape* _shape = NULL):
        Base(), pos(position), from(incomingDirection), nbResponseLeft(0), sampler(NULL), shape(_shape) { name = "unknown event"; }

    Event(const Event& other) : Base(other)
    {
        pos = vec3(other.pos);
        from = vec3(other.pos);
        nbResponseLeft = other.nbResponseLeft;
		type = other.type;
		shape = other.shape;
        if (other.sampler) { sampler = new Sampler(*(other.sampler)); }
        else { sampler = NULL; }
    }

    virtual ~Event()
    {
        if (sampler) { delete sampler; }
    };

    virtual bool isDiffuse() { return false; }

	/*!
	 * \fn const vec3& getPosition() const
	 * \brief Renvoie une référence vers le point d'impact de l'evenement.
	 * \return Adresse du vecteur decrivant le point d'impact
	*/
    const vec3& getPosition() const { return pos; }

	/*!
    * \fn void setPosition(vec3 &_pos)
    * \brief Place un nouveau point d'impact a l'evenement
    * \param pos : Nouveau point d'impact. Le pointeur n'est pas modifie
    */
    void setPosition(const vec3& _pos) { pos = vec3(_pos); }

	/*!
   * \fn vec3* getIncomingDirection()
   * \brief Renvoie un pointeur vers le vecteur incident sur l'arete.
   * \return Adresse du vecteur decrivant le vecteur directeur du rayon incident.
   */
    vec3 getIncomingDirection() { return from; }

	/*!
   * \fn void setIncomingDirection(const vec3& _from)
   * \brief Place un nouveau vecteur d'incidence
   * \param from : Nouveau vecteur d'incidence
   */
    void setIncomingDirection(const vec3& _from) { from = vec3(_from); }

    bool isReponseLeft() { return nbResponseLeft > 0; }
    int getNbResponseLeft() { return nbResponseLeft; }
    void setNbResponseLeft(int _nbResponseLeft) { nbResponseLeft = _nbResponseLeft; }

	/*!
   * \fn Shape* getShape()
   * \brief renvoi la primitive impactee
   * \return la primitive impactee
   */
    Shape* getShape() { return shape; }
	
	/*!
	* \fn void setShape(Shape* _shape)
	* \brief place la primitive impactee 
	*/
    void setShape(Shape* _shape) { shape = _shape; }

	/*!
   * \fn virtual vec3 getReponse()
   * \brief Renvoie un vecteur directeur repondant a l'impact. Correspond au vecteur directeur du rayon en reponse a l'evenement.
   * \return Nouveau vecteur directeur.
   * \warning Doit etre redefini par les classes heritant de Event
   */
    virtual bool getResponse(vec3& r, bool force = false) { return false; }

	/*!
   * \fnvirtual bool isAcceptableResponse(vec3& test)
   * \brief Renvoie vrai si le vecteur direct repondant a l'impact est correct.
   * \return True si le reponse est acceptable
    * \warning Doit etre redefini par les classes heritant de Event
   */
    virtual bool isAcceptableResponse(vec3& test) { return false; }

    virtual bool generateTest(std::vector<vec3>& succededTest, std::vector<vec3>& failTest, unsigned int nbResponses) { return true; }

    virtual bool generateResponse(std::vector<vec3>& responses, unsigned int nbResponses) { return false; }

    virtual bool appendTarget(vec3 target, bool force = false) { return false; }

	/*!
   * \fn virtual int getType()
   * \brief Renvoie le type de l'evenement
   * \return type de l'evenement
   * \warning Doit etre redefini par les classes heritant de Event
   */
    virtual int getType() { return type; }

	/*!
	 * \fn virtual void setType()
	 * \brief Added by DTh to make possible simple test of rays
	 */
	virtual void setType( const typeevent &_type) { type = _type; }

    virtual double getAngle() { return 0.0; }

	/*!
    * \fn const decimal distance(const Event &other) const
    * \brief Return distance from another event
    * \param other : event from which distance must be measured
    */
	const decimal distance(const Event &other) const { return this->pos.distance( other.getPosition() ); }


protected:
    vec3 pos;			/*!< Point d'impact de l'evenement */
    vec3 from;			/*!< Vecteur directeur du rayon incident */
    int nbResponseLeft;
    Sampler* sampler;
    Shape* shape;		/*< La primitive impactee */
    typeevent type;		/*!< Type de l'evenement */
    std::vector<vec3> targets;

};

#endif
