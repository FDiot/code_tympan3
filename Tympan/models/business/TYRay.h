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

#include "Tympan/models/business/TYElement.h"
#include "Tympan/models/common/3d.h"
#include "Tympan/models/common/acoustic_path.h"

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
     * \fn TYRay(const acoustic_path &ap)
     * \brief Build a TYRay from an acoustic_path
     */
    TYRay(const acoustic_path &ap) : TYElement(), acoustic_path(ap) {}
    
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
    bool deepCopy(TYRay* pOther, bool copyId);

    /**
     * Construit une arborescence DOM a partir du noeud passe
     * afin de sauvegarder les parametres au format XML.
     *
     * @param domElement Le noeud DOM parent a partir duquel l'arborescence doit etre creee.
     *
     * @return  Le noeud DOM nouvellement cree.
     */
    virtual DOM_Element toXML(DOM_Element& domElement) /* const */;

    /**
     * Extrait les informations a partir du noeud DOM passe afin de
     * recuperer les parametres de la classe.
     *
     * @param domElement Le noeud DOM a partir duquel les parametres doivent etre recuperes.
     *
     * @return 1 si la recuperation des parametres a reussie.
     */
    virtual int fromXML(DOM_Element domElement);

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

/*!
 * \fn TYRay(const acoustic_path &ap)
 * \brief Build a TYRay from an acoustic_path (used by cython interface)
*/
SmartPtr<TYRay> build_ray(const acoustic_path& ap);

#endif
