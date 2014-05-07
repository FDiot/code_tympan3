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

/**
 * \file TYPickingTable.h
 * \brief Gestion de la table de correspondance indice/element pour le picking (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_PICKING_TABLE__
#define __TY_PICKING_TABLE__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Tympan/MetierSolver/DataManagerCore/TYElement.h"

///Collection de TYElement*.
typedef std::vector<TYElement*> TYTabPtrElement;


/**
 * \class TYPickingTable
 * \brief classe graphique pour la gestion de la table de correspondanceindice/element pour le picking
 */
class TYPickingTable
{

    // Methodes
public:

    /**
     * Constructeur.
     */
    TYPickingTable();

    /**
     * Destructeur.
     */
    virtual ~TYPickingTable();

    /**
     * Recherche un element particulier a partir de son indice.
     *
     * @param index L'indice de l'element a rechercher.
     *
     * @return L'Element ayant l'indice correspondant, sinon NULL.
     */
    static TYElement* getElement(int index);

    /**
     * Retourne l'ensemble des elements enregistrees.
     *
     * @return L'ensemble des elements enregistrees.
     */
    static TYTabPtrElement getElements() { return _elements; }

    /**
     * Vide le tableau de l'enregistrement des elements.
     */
    static void purgeElements();

    /**
     * Ajoute une Element au tableau d'enregistrement des elements.
     *
     * @param pElt L'element a ajouter.
     */
    static void addElement(TYElement* pElt);

    /**
     * Get de l'indice courant.
     */
    static int getIndex() {return _index;}


private:
    ///Collection de toutes les Elements de type TYPickingTable et derivees.
    static TYTabPtrElement _elements;

    ///indice courant
    static int _index;

};




#endif // __TY_PICKING_TABLE__
