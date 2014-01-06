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

#ifndef __DOM_SAVE__
#define __DOM_SAVE__

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif

#include "QT2DOM.h"

/**
 * \file DOMSave.h
 * \class DOMSave
 * \brief Classe utilitaire XML pour la sauvegarde d'une arborescence DOM
 * \author Projet_Tympan
 *
 */
class DOMSave
{
    // Methodes
public:
    /**
     * \fn static int saveNodeToFile(DOM_Node & nodeToSave, const char * fileName);
     * \brief La methode principale pour la sauvegarde d'un noeud DOM dans un fichier XML.
     *
     * \param nodeToSave Le noeud DOM a partir lequel doit s'effecture la sauvegarde recursive.
     * \param fileName Le nom du fichier dans lequel se fera l'ecriture.
     *
     * \return L'etat de la reussite.
     */
    static int saveNodeToFile(DOM_Node& nodeToSave, const char* fileName);

    /**
     * \fn static QString saveNodeToString(DOM_Node & nodeToSave);
     * \brief Methode pour la sauvegarde d'un noeud DOM dans une string XML.
     *
     * \param nodeToSave Le noeud DOM a partir lequel doit s'effecture la sauvegarde recursive.
     *
     * \return La string XML.
     */
    static QString saveNodeToString(DOM_Node& nodeToSave);

    /**
     * \fn static DOMString getIndent(int indentNb);
     * \brief Methode utilitaire pour la gestion de l'indentation.
     *
     * \param indentNb Le niveau d'indentation.
     *
     * \return Une DOMString comportant autant de tabulation que de niveau d'indentation.
     */
    static DOMString getIndent(int indentNb);

protected:
    /**
     * Constructeur.
     */
    DOMSave() {};

    /**
     * Destructeur.
     */
    ~DOMSave() {};
};


#endif // __DOM_SAVE__
