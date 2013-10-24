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
 * \file TYPickHandler.h
 * \brief Objet pour le picking graphique (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_PICK_HANDLER__
#define __TY_PICK_HANDLER__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class TYElement;


/**
 * \class TYPickHandler
 * \brief classe grapique pour la gestion de l'element picke.
 */
class TYPickHandler
{
    // Methodes
public:
    /**
     * Cette methode est la callback passee en parametre de la methode
     * SetPickMethod() avec pour argument le pointeur de
     * l'element metier associe a l'element graphique.
     *
     * @param pPickedElt Un pointeur sur un element metier (heritant de TYElement).
     */
    static void pickCallback(void* pPickedElt);

    /**
     * Retourne le pointeur sur l'element metier associe au dernier element
     * graphique picke.
     */
    static TYElement* getPickedElt() { return _pPickedElt; }


protected:
    /**
     * Pointeur sur l'element metier associe au dernier element
     * graphique picke.
     */
    static TYElement* _pPickedElt;
};


#endif // __TY_PICK_HANDLER__
