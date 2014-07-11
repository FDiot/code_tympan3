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
 * \file TYAction.h
 * \brief Definit une action, necessaire pour la gestion de l'undo (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */


#ifndef __TY_ACTION__
#define __TY_ACTION__

class QString;

/**
 * .
 * \class TYAction
 * \brief Definit une action, necessaire pour la gestion de l'undo
 */
class TYAction
{
public:
    /**
     * Constructeur.
     */
    TYAction(const QString& actionName);
    /**
     * Destructeur.
     */
    virtual ~TYAction();


    /**
     * Annule la derniere action.
     */
    virtual void undo() = 0;

    /**
     * Effectue a nouveau la derniere action annulee.
     */
    virtual void redo() = 0;


    /**
     * Retourne le nom de cette action.
     */
    QString getName() const;


private:
    ///Le nom de cette action.
    QString _actionName;
};


#endif // __TY_ACTION__
