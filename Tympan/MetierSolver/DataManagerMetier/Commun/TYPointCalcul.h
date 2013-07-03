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
 *
 *
 *
 */

#ifndef __TY_POINTCALCUL__
#define __TY_POINTCALCUL__


#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYPoint.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYGeometryNode.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSpectre.h"

#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerIHM/TYPointCalculWidget.h"
#endif


/**
 * \file TYPointCalcul.h
 * \class TYPointCalcul
 * \brief  Classe de definition d'un point de calcul.C'est une classe derivee a TYPoint avec en plus un spectrepour contenir le resultat du calcul.
 * \author Projet_Tympan
 * \version v 1.1
 * \date 2008/01/21
 *
 *
 * @author Projet_Tympan
 */
class TYPointCalcul: public TYPoint
{
    OPROTOSUPERDECL(TYPointCalcul, TYPoint)
    TY_EXTENSION_DECL(TYPointCalcul)

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
    void copySpectres(TYPointCalcul* pOther);

    /// Copie du map calcul-etat
    void copyEtats(TYPointCalcul* pOther);

    /*!
     * \brief Duplique l'etat defini pour un calcul pour un autre calcul
     * \param idCalculRef : Identifiant unique du calcul referent
     * \param idCalculNew : Identifiant unique du calcul a recopier
     */
    void duplicateEtat(const QString& idCalculRef, const QString& idCalculNew);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);

    virtual int fromXML(DOM_Element domElement);

    /**
     * \fn void setSpectre(const TYSpectre & spectre, TYCalcul* pCalcul = NULL)
     * \brief Set du spectre resultat d'un calcul donne.
     */
    void setSpectre(const TYSpectre& spectre, TYCalcul* pCalcul = NULL);

    /**
     * \fn  TYSpectre* getSpectre(TYCalcul* pCalcul = NULL, const bool& addToCalcul = false)
     * \brief Get du spectre resultat d'un calcul donne.
     */
    TYSpectre* getSpectre(TYCalcul* pCalcul = NULL, const bool& addToCalcul = false);

    /**
     * \fn double getValA()
     *     const double getValA()
     * \brief Get de la valeur globale A au point
     * \return _dBA
     */
    double getValA() { return _dBA; }

    /**
     * \fn const double getValA() const
     * \brief Get de la valeur globale A au point
     * \return _dBA
     */
    const double getValA() const { return _dBA; }

    /**
     * \fn double getValLin()
     * \brief Get de la valeur globale Lin au point
     * \return _dBLin
     */
    double getValLin() { return _dBLin; }

    /**
     * \fn const double getValLin() const
     * \brief Get de la valeur globale Lin au point
     * \return _dBLin
     */
    const double getValLin() const { return _dBLin; }

    /**
     * Set/Get de l'etat de ce point de calcul.
     */
    void setEtat(const bool& etat, TYCalcul* pCalcul = NULL);
    bool getEtat(TYCalcul* pCalcul = NULL, const bool& addToCalcul = false);
    //  void setState(int state) { _state = state; setIsGeometryModified(true); }

    /**
     * Set/Get si le point de calcul n'est associe qu'a un seul spectre (maillage)
     */
    void setIsSpectreUnique(bool isSpectreUnique) { _isSpectreUnique = isSpectreUnique; }
    bool getIsSpectreUnique() { return _isSpectreUnique; }

    /**
     * Set/Get de l'etat de ce point de calcul.
     */
    //  int getState() { return _state; }

    /*
     * \fn  bool isLocked()
     *      const bool isLocked()
     *      void setLocked(const bool& locked)
     * \brief Set/Get de l'etat de blocage du point de calcul.
     * \return _locked
     */
    bool isLocked() { return _locked; }
    const bool isLocked() const { return _locked; }
    void setLocked(const bool& locked) { _locked = locked; /*_pSpectre->setIsReadOnly(_locked);*/}

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
    ///Les spectres resultat des calculs en ce point.
    TYMapIdSpectre _tabSpectre;

    ///Indique que le point de calcul n'a qu'un seul spectre (cas des maillages)
    bool _isSpectreUnique;

    ///L'etat du point pour un calcul donne
    TYMapIdBool _tabEtats;

    ///Indique sur l'etat de ce point de calcul.
    //  int _state;

    /// Permet de bloquer le spectre si le calcul est verrouille
    bool _locked;

    /// Valeur en dBA et en dBLin au point de calcul
    double _dBA;
    double _dBLin;
};


///Noeud geometrique de type TYPointCalcul.
typedef TYGeometryNode TYPointCalculGeoNode;
///Smart Pointer sur TYPointCalculGeoNode.
typedef SmartPtr<TYPointCalculGeoNode> LPTYPointCalculGeoNode;
///Collection de noeuds geometriques de type TYPointCalcul.
typedef std::vector<LPTYPointCalculGeoNode> TYTabPointCalculGeoNode;


#endif // __TY_POINTCALCUL__
