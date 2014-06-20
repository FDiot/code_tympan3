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
#include "Tympan/MetierSolver/CommonTools/OPoint3D.h"
#include "Tympan/MetierSolver/CommonTools/Acoustic_path.h"

/*!
* \file TYRay.h
* \class TYRay
* \brief Classe decrivant un rayon acoustique gere par un lancer de rayon. Cette classe doit permettre la modelisation de rayons pour l'affichage de Tympan.
* \todo : ajout des listes de diffractions/reflexions
* \author Projet_Tympan
*/

class TYSourcePonctuelle;
class TYPointCalcul;

class TYRay: public TYElement, public acoustic_path
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
    * \fn TYRay( const TYRay &ray)
    * \brief Constructeur par copie d'un rayon
    */
    TYRay(const TYRay& ray);

    // Destructor
    ~TYRay();

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
    * \fn void setRecepteur(TYPointCalcul *)
    * \brief Place le recepteur du rayon. Le dernier point de la polyligne est mis a jour
    */
    void setRecepteur(TYPointCalcul* recep) { _recepteur = recep; }


    /*!
    * \fn TYPointControl* getRecepteur()
    * \brief Recuperation du recepteur du rayon
    */
    TYPointCalcul* getRecepteur() {return _recepteur;}


protected:
	int _identifiant;				/*!< Identifiant du rayon */
    TYSourcePonctuelle* _source;	/*!< Pointeur vers la source du rayon */
    TYPointCalcul* _recepteur;		/*!< Pointeur vers le recepteur du rayon */
    OPoint3D _posSourceGlobal;
    OPoint3D _posReceptGlobal;
};
//Smart Pointer sur TYRay
typedef SmartPtr<TYRay> LPTYRay;

typedef std::vector<LPTYRay> TYTabRay;

#endif
