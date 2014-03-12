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

#ifndef __TY_ELEMENT_COLLECTION__
#define __TY_ELEMENT_COLLECTION__


#include "TYElement.h"

 // std::vector<LPTYElement> will now be used instead of TYElementCollection.
 // This typedef will make it easier to switch from vector to another data
 // container later on.
typedef std::vector<LPTYElement> LPTYElementArray;

/**
 * Collection d'elements de base des composants et elements metiers.
 */
class TYElementCollection : public IRefCount, public std::vector<LPTYElement>
{
    // Methodes
public:
    /**
     * Constructeur.
     */
    TYElementCollection();

    /**
     * Destructeur.
     */
    virtual ~TYElementCollection();

    /**
     * Vide le tableau.
     */
    void purge();

    /**
     * Ajoute un nouvel element a la collection.
     *
     * @param pElt Un element a ajouter a la collection.
     *
     * @return <code>true</code> si l'element a ete correctement ajoute;
     *         <code>false</code> sinon.
     */
    bool add(LPTYElement pElt);

    /**
     * Retourne le nombre d'elements contenus dans le tableau.
     */
    size_t getCount() { return this->size(); }

    /**
     * Retourne l'element a l'indice demande.
     */
    LPTYElement getAt(int index) { return this->at(index); }
};


typedef SmartPtr<TYElementCollection> LPTYElementCollection;


#endif // __TY_ELEMENT_COLLECTION__
