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

/*
 *
 */

#ifndef __TY_POINTCALCUL__
#define __TY_POINTCALCUL__


#include "Tympan/models/business/geometry/TYPoint.h"
#include "Tympan/models/business/geometry/TYGeometryNode.h"
#include "Tympan/models/business/acoustic/TYSpectre.h"

/**
 * \file TYPointCalcul.h
 * \class TYPointCalcul
 * \brief  Classe de definition d'un point de calcul.C'est une classe derivee a TYPoint avec en plus un spectrepour contenir le resultat du calcul.
 */
class TYPointCalcul: public TYPoint
{
    OPROTODECL(TYPointCalcul)
    TY_EXTENSION_DECL_ONLY(TYPointCalcul)

    // Methodes
public:
    /**
     * \fn TYPointCalcul()
     * \brief Constructeur par defaut
     *   Constructeur par defaut de la classe TYPointCalcul.
     */
    TYPointCalcul();

    /**
     * \fn TYPointCalcul(const TYPoint& other)
     * \brief Constructeur a partir d'un TYPoint.
     *  Constructeur a partir d'un TYPoin de la classe TYPointCalcul.
     */
    TYPointCalcul(const TYPoint& other);

    /**
     * \fn TYPointCalcul(const TYPointCalcul& other
     * \brief Constructeur par copie.
     *  Constructeur par copie de la classe TYPointCalcul.
     */
    TYPointCalcul(const TYPointCalcul& other);

    /**
     * \fn virtual ~TYPointCalcul(
     * \brief Destructeur
     *   Destructeur de la classe TYPointCalcul.
     */
    virtual ~TYPointCalcul();

    ///Operateur =.
    TYPointCalcul& operator=(TYPointCalcul& other);

    ///Operateur =.
    TYPointCalcul& operator=(const TYPoint& other);

    ///Operateur ==.
    bool operator==(const TYPointCalcul& other) const;

    ///Operateur !=.
    bool operator!=(const TYPointCalcul& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    /// Copie du map calcul-spectre
    //void copySpectres(TYPointCalcul* pOther);



    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);

    virtual int fromXML(DOM_Element domElement);

    /**
     * \fn void setSpectre(const TYSpectre & spectre, TYCalcul* pCalcul = NULL)
     * \brief Set du spectre resultat d'un calcul donne.
     */
    void setSpectre(const LPTYSpectre spectre) { _spectre = spectre; }

    /**
     * \fn  TYSpectre* getSpectre(TYCalcul* pCalcul = NULL, const bool& addToCalcul = false)
     * \brief Get du spectre resultat d'un calcul donne.
     */
    virtual LPTYSpectre getSpectre() { return _spectre; };

    virtual bool etat() { return _etat; }
    virtual bool etat(const TYUUID& id_calc) { return _etat; }
    virtual bool etat(const TYCalcul* pCalc) { return _etat; }
    virtual void setEtat(const bool& etat) { _etat = etat; };

    /**
     * \fn double getValA()
     *     const double getValA()
     * \brief Get de la valeur globale A au point
     * \return _dBA
     */
    double getValA() { return _spectre->valGlobDBA(); }

    /**
     * \fn const double getValA() const
     * \brief Get de la valeur globale A au point
     * \return _dBA
     */
    const double getValA() const { return _spectre->valGlobDBA(); }

    /**
     * \fn double getValLin()
     * \brief Get de la valeur globale Lin au point
     * \return _dBLin
     */
    double getValLin() { return _spectre->valGlobDBLin(); }

    /**
     * \fn const double getValLin() const
     * \brief Get de la valeur globale Lin au point
     * \return _dBLin
     */
    const double getValLin() const { return _spectre->valGlobDBLin(); }

   /**
     * \fn  TYPoint getCoordSIG()
     * \brief Passage en coordonnees SIG
     */
    TYPoint getCoordSIG();

    /**
     * \fn enum PointCalculState
     * \brief Les differents etats possible pour un point de calcul.
     * \param Actif      Le point est utilise pour les calculs.
           Inactif   Le point n'est pas utilise pour les calculs.
     */

    enum PointCalculState
    {
        Actif,
        Inactif,
    };

    // Membres
protected:
    LPTYSpectre _spectre;

private:
    bool _etat;
};


///Noeud geometrique de type TYPointCalcul.
typedef TYGeometryNode TYPointCalculGeoNode;
///Smart Pointer sur TYPointCalculGeoNode.
typedef SmartPtr<TYPointCalculGeoNode> LPTYPointCalculGeoNode;
///Collection de noeuds geometriques de type TYPointCalcul.
typedef std::vector<LPTYPointCalculGeoNode> TYTabPointCalculGeoNode;


#endif // __TY_POINTCALCUL__
