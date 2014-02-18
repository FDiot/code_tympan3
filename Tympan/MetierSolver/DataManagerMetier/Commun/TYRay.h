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
#include "Tympan/MetierSolver/DataManagerCore/TYDefines.h"          //Importation des TYPoint et TYTabPoint
#include "TYPointControl.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSourcePonctuelle.h"

#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerGraphic/TYRayGraphic.h"
#endif

enum TY_RAY_EVENT
{
    TY_NO_TYPE = 0,
    TYDIFFRACTION,
    TYREFLEXION,
    TYREFRACTION,
    TYSOURCE,
    TYRECEPTEUR
};

struct TYRayEvent
{
public:
    TYRayEvent() {}
    TYRayEvent(const TYPoint& pt, const double& ang = 0.0, const double& angtheta = 0.0, const TY_RAY_EVENT& ev = TY_NO_TYPE, const int& idF1 = 0, const int& idF2 = 0);
    TYPoint pos;        /*!< Position de l'evenement */
    double angle;       /*!< Angle d'incidence du rayon  (pour l'angle de tir - plan x,z -)*/
    double angletheta;  /*!< Angle de tir sur un plan horizontal (x,y) */
    TY_RAY_EVENT type;  /*!< Type d'evenement */
    int idFace1;        /*!< identifiant de la face sur laquelle a lieu l'evenement (reflexion & diffraction)*/
    int idFace2;        /*!< identifiant de la face sur laquelle a lieu l'evenement (diffraction seulement)*/
};

typedef std::vector<TYRayEvent> TYTabRayEvent;

/*!
* \file TYRay.h
* \class TYRay
* \brief Classe di申crivant un rayon acoustique gi申ri申 par un lancer de rayon. Cette classe doit permettre la modi申lisation de rayons pour l'affichage de Tympan.
* \todo : ajout des listes de diffractions/reflexions
* \author Projet_Tympan
* \author Projet_Tympan
*/
class TYRay: public TYElement
{
    OPROTOSUPERDECL(TYRay, TYElement)
    TY_EXT_GRAPHIC_DECL(TYRay)

public:

    /*!
    * \fn TYRay()
    * \brief Constructeur par di申faut
    */
    TYRay();

    /*!
    * \fn TYRay(TYSourcePonctuelle *_sourceP, TYPointControl *_recepP, TYTabPoint &_polyligne)
    * \brief Construction d'un rayon avec une source, un recepteur et une liste de positions d'i申vi申nements acoustiques (ri申flexion, diffraction, etc...)
    * \warning Hypothi申se : Le premier point de la polyligne correspond i申 la position de la source en coordonni申es globales et le dernier point de la polyligne
    * correspond i申 la position du recepteur en coordonni申es globales. Si ces conditions ne sont pas respecti申es, la fonction renvoie un rayon vide equivalent i申 TYRay()
    */
    //  TYRay(TYSourcePonctuelle *_sourceP, TYPointControl *_recepP, std::vector<TYRayEvent> &_events);
    TYRay(TYSourcePonctuelle* _sourceP, TYPointCalcul* _recepP, std::vector<TYRayEvent> &_events);

    /*!
    * \fn TYRay( const TYRay &ray)
    * \brief Constructeur par copie d'un rayon
    */
    TYRay(const TYRay& ray);

    /*!
    * \fn void setSource(TYSourcePonctuelle *_source TYPoint &globalPosition)
    * \brief Place la source du rayon. Le premier point de la polyligne est mis i申 jour
    */
    void setSource(TYSourcePonctuelle* _source, TYPoint& globalPosition);
    /*!
    * \fn TYSourcePonctuelle* getSource()
    * \brief Ri申cupi申ration de la source du rayon
    */
    TYSourcePonctuelle* getSource() {return _source;}

    /*!
    * \fn void setRecepteur(TYPointControl *_recep TYPoint &globalPosition)
    * \brief Place le recepteur du rayon. Le dernier point de la polyligne est mis i申 jour
    */
    //  void setRecepteur(TYPointControl *_recep, TYPoint &globalPosition);
    void setRecepteur(TYPointCalcul* _recep, TYPoint& globalPosition);

    /*!
    * \fn TYPointControl* getRecepteur()
    * \brief Ri申cupi申ration du recepteur du rayon
    */
    //  TYPointControl* getRecepteur(){return _recepteur;}
    TYPointCalcul* getRecepteur() {return _recepteur;}

    /*!
    * \fn void setTabPoint(TYTabPoint &_tabPoint)
    * \brief Place le tableau de point correspondant au parcours du rayon.
    * \warning Le premier point doit toujours correspondre i申 la position globale de la source et le dernier point doit toujours correspondre i申 la position globale du recepteur
    */
    //void setTabPoint(TYTabPoint &_tabPoint);

    /*!
    * \fn TYTabPoint& getTabPoint()
    * \brief Renvoie le tableau de points correspondant au parcours du rayon incluant la source et le recepteur
    */
    //TYTabPoint& getTabPoint(){return polyligne;}

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
    * \brief Ajoute un evenement i申 la liste des i申vi申nements du rayon
    */
    void addEvent(TYRayEvent& event) {_events.push_back(event);}

    /*!
    * \fn std::vector<Event>& getEvents()
    * \brief Renvoie la liste des i申vi申nements rencontri申s par le rayon
    */
    TYTabRayEvent& getEvents() {return _events;}

    /*!
    * \fn void setTabPoint(TYTabPoint &_tabPoint)
    * \brief Place le tableau de point correspondant au parcours du rayon.
    * \warning Le premier TYRayEvent doit toujours correspondre i申 la source et sa position globale et le dernier point doit toujours correspondre i申 un ri申cepteur et sa position globale du
    */
    void setTabEvent(TYTabRayEvent& _tabEvent);

    /*!
    * \fn TYTabRayEvent getTabEvent()
    * \brief Renvoie le tableau de points correspondant au parcours du rayon incluant la source et le recepteur
    */
    TYTabRayEvent getTabEvent() {return _events;}

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
    * \fn double getSize()
    * \brief Renvoie la taille du rayon
    */
    double getSize();

    /*!
    * \fn double getDistanceSR()
    * \brief Renvoie la distance source/recepteur du rayon
    */
    double getDistanceSR() const;

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
    * \fn TYPoint& getPosSourceGlobal()
    * \brief Get de la position de la source dans le repi申re global
    */
    TYPoint& getPosSourceGlobal() { return _posSourceGlobal; }

    /*!
    * \fn void setPosReceptGlobal(const OPoint3D& pos)
    * \brief Set de la position du recepteur dans le repi申re global
    */
    void setPosReceptGlobal(const OPoint3D& pos) { _posReceptGlobal = pos; }

    /*!
    * \fn TYPoint& getPosReceptGlobal()
    * \brief Get de la position du recepteur dans le repi申re global
    */
    TYPoint& getPosReceptGlobal() { return _posReceptGlobal; }



protected:
    /*!< Identifiant du rayon */
    int _identifiant;

    /*!< Pointeur vers la source du rayon */
    TYSourcePonctuelle* _source;

    /*!< Pointeur vers le recepteur du rayon */
    //  TYPointControl* _recepteur;
    TYPointCalcul* _recepteur;

    TYPoint _posSourceGlobal;
    TYPoint _posReceptGlobal;

    /*!< Vecteurs d'evenements contenant la liste des evenements du rayon associi申s i申 leurs positions.*/
    TYTabRayEvent _events;
};

///Smart Pointer sur TYRay
typedef SmartPtr<TYRay> LPTYRay;

typedef std::vector<LPTYRay> TYTabRay;

#endif
