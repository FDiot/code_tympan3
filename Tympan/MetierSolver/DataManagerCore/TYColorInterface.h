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
 */

#ifndef __TYCOLORINTERFACE__
#define __TYCOLORINTERFACE__

#include "Tympan/Tools/OColor.h"
#include "TYXMLTools.h"

/**
 * Classe abstraite permettant d'ajouter une couleur a un objet.
 *
 * @author Projet_Tympan
 *
 */
class TYColorInterface
{
public:
    /**
     * @name Construction et Destruction
     */
    //@{

    /**
     * Constructeur par default.
     */
    TYColorInterface();

    /**
     * Constructeur par copie.
     */
    TYColorInterface(const TYColorInterface& other);

    /**
     * Destructeur.
     */
    virtual ~TYColorInterface();

    //@}


    /**
     * @name Operateurs
     */
    //@{

    /**
     * Operateur de copie.
     */
    TYColorInterface& operator=(const TYColorInterface& other);
    /**
     * Operateur de comparaison.
     */
    bool operator==(const TYColorInterface& other) const;
    /**
     * Operateur de comparaison.
     */
    bool operator!=(const TYColorInterface& other) const;

    /**
     * Operateur de copie.
     */
    void deepCopy(const TYColorInterface* pOther, bool copyId = true);

    //@}



    /**
     * @name Interface XML
     */
    //@{

    /**
     * Construit une arborescence DOM a partir du noeud passe
     * afin de sauvegarder les parametres au format XML.
     *
     * @param domElement Le noeud DOM parent a partir duquel l'arborescence doit etre creee.
     *
     * @return  Le noeud DOM nouvellement cree.
     */
    DOM_Element toXML(DOM_Element& domElement);

    /**
     * Extrait les informations a partir du noeud DOM passe afin de
     * recuperer les parametres de la classe.
     *
     * @param domElement Le noeud DOM a partir duquel les parametres doivent etre recuperes.
     *
     * @return 1 si la recuperation des parametres a reussie.
     */
    int fromXML(DOM_Element domElement);

    //@}



    /**
     * @name Accesseurs
     */
    //@{

    /**
     * Set de la couleur.
     */
    virtual void setColor(const OColor& color);

    /**
     * Get de la couleur.
     */
    OColor getColor() const;

    //@}

    /**
     * Mise a jour des couleurs a partir des preferences
     */
    virtual void updateColor() {}


protected:
    OColor _color;

};

#endif // __TYCOLORINTERFACE__
