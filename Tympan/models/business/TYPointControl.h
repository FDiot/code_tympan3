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

#ifndef __TY_POINTCONTROL__
#define __TY_POINTCONTROL__

#include "Tympan/models/business/geometry/TYGeometryNode.h"
#include "Tympan/models/business/TYColorInterface.h"
#include "Tympan/models/business/TYPointCalcul.h"


/**
 * \file TYPointControl.h
 * \class TYPointControl
 * \brief Classe de definition d'un point de controle.Le point de controle est un point de calcul avec une hauteur par rapport a l'altitude.
 * \date 2008/01/21
 * \version v 1.1
 * \author Projet_Tympan
 */
class TYPointControl: public TYPointCalcul, public TYColorInterface
{
    OPROTODECL(TYPointControl)
    TY_EXTENSION_DECL_ONLY(TYPointControl)
    TY_EXT_GRAPHIC_DECL_ONLY(TYPointControl)

    // Methodes
public:
    /**
     * \fn TYPointControl()
     * \brief Constructeur par defaut.
     * Constructeur par defaut de la classe TYPointControl
     */
    TYPointControl();

    /**
     * \fn TYPointControl(const TYPoint& other)
     * \brief Constructeur a partir d'un TYPoint.
     * Le constructeur a partir d'un TYPoint de la classe TYPointControl .
     */
    TYPointControl(const TYPoint& other);

    /**
     * \fn TYPointControl(const TYPointControl& other
     * \brief Constructeur par copie.
     * Le constructeur par copie de la classe TYPointControl.
     */
    TYPointControl(const TYPointControl& other);

    /**
     * \fn virtual ~TYPointControl()
     * \brief Destructeur
     *  Destructeur de la classe TYPointControl  .
     */
    virtual ~TYPointControl();

    ///Operateur =.
    TYPointControl& operator=(const TYPointControl& other);
    ///Operateur =.
    TYPointControl& operator=(const TYPoint& other);
    ///Operateur ==.
    bool operator==(const TYPointControl& other) const;
    ///Operateur !=.
    bool operator!=(const TYPointControl& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    /**
     * \fn  int getObject()
     * \brief Set/Get de l'objet a afficher.
     * \return _object
     */
    int getObject() const { return _object; }

    /**
     * \fn void setObject(int object)
     * \brief Set/Get de l'objet a afficher.
     */
    void setObject(int object) { _object = object; }

    /**
     * \fn void setHauteur(double hauteur)
     * \brief Set de la hauteur de ce point par rapport au sol (a l'altimetrie en fait).
     */
    void setHauteur(double hauteur) { _hauteur = hauteur; setIsGeometryModified(true); }

    /**
     * \fn double getHauteur()
     * \brief Get de la hauteur de ce point par rapport au sol (a l'altimetrie en fait).
     * \return _hauteur
     */
    double getHauteur() const { return _hauteur; }

    ///**
    // * \fn void purge()
    // * \brief Mise a zero du resultat
    // */
    //void purge(TYCalcul* pCalcul);



    /**
     * \param objectCube, objectPyramid, objectSphere, objectStar
     * Les differents types d'objet graphique que l'on peut afficher.
     */
    enum
    {
        objectCube,
        objectPyramid,
        objectSphere,
        objectStar
    };

    /**
     * \fn void toSIG()
     *     void fromSIG()
     * \brief Conversion dans le repere du Systeme d'Information Geographique
     */
    void toSIG();
    void fromSIG();

    /**
     * \fn void setStatusSIG(const bool& statusSIG)
     *     bool getStatusSIG()
     *     const bool getStatusSIG()
     * \brief Get/Set du statut des coordonnees par rapport au SIG
     * \return _statusSIG
     */
    void setStatusSIG(const bool& statusSIG) { _statusSIG = statusSIG; }
    bool getStatusSIG() { return _statusSIG; }
    const bool getStatusSIG() const { return _statusSIG; }

    /**
     * \fn int getSIGType())
     * \brief Retourne le type de SIG
     */
    int getSIGType();

    virtual LPTYSpectre getSpectre();

    /**
     * Set/Get de l'etat de ce point de calcul.
     */
    virtual void setEtat(const TYUUID& id_calc, bool etat);
    virtual bool etat();
    virtual bool etat(const TYUUID& id_calc);
    virtual bool etat(const TYCalcul* pCalc);

    /// Copie du map calcul-etat
    void copyEtats(TYPointControl* pOther);

    /*!
     * \brief Duplique l'etat defini pour un calcul pour un autre calcul
     * \param idCalculRef : Identifiant unique du calcul referent
     * \param idCalculNew : Identifiant unique du calcul a recopier
     */
    void duplicateEtat(const TYUUID& idCalculRef, const TYUUID& idCalculNew);

    void* getCompatibilityVector();

// Membres
protected:
    ///L'etat du point pour un calcul donne
    TYMapIdBool _tabEtats;

    ///shape of the point in GUI
    int _object;

    ///La hauteur du point par rapport a l'altitude.
    double _hauteur;

    /// Etat des coordonnees par rapport au SIG (position dans le repere SIG ou pas)
    bool _statusSIG;
};


///Smart Pointer sur TYPointControl.
typedef SmartPtr<TYPointControl> LPTYPointControl;
///Collection de Smart Pointer sur TYPointControl.
typedef std::vector<LPTYPointControl> TYTabLPPointControl;

///Noeud geometrique de type TYPointControl.
typedef TYGeometryNode TYPointControlGeoNode;
///Smart Pointer sur TYPointControlGeoNode.
typedef SmartPtr<TYPointControlGeoNode> LPTYPointControlGeoNode;
///Collection de noeuds geometriques de type TYPointControl.
typedef std::vector<LPTYPointControlGeoNode> TYTabLPPointControlGeoNode;


#endif // __TY_POINTCONTROL__

