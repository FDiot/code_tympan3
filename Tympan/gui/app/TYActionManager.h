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
 * \file TYActionManager.h
 * \brief Gere les undos (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 *
 */


#ifndef __TY_ACTION_MANAGER__
#define __TY_ACTION_MANAGER__


#include <qobject.h>
#include <qlist.h>
#include "TYAction.h"

/**
 * \class TYActionManager
 * \brief Gere les undos
 */
class TYActionManager : public QObject
{
    Q_OBJECT

public:
    /**
     * \fn TYActionManager(int size)
     * \brief Constructeur.
     */
    TYActionManager(int size);
    /**
     * \fn virtual ~TYActionManager()
     * \brief Destructeur.
     */
    virtual ~TYActionManager();

    /**
     * \fn void setModified(bool modified)
     * \brief Set/Get de l'etat du manager.
     */
    void setModified(bool modified);
    /**
     * \fn bool getModified()
     * \brief Set/Get de l'etat du manager.
     */
    bool getModified() const { return _modified; }

    /**
     * \fn void addAction(TYAction * pAction)
     * \brief Ajoute une nouvelle action a l'historique.
     */
    void addAction(TYAction* pAction);

    /**
     * \fn void emitUndoRedo()
     * \brief Envoie un signal informant sur l'etat de ce ActionManager.
     */
    void emitUndoRedo();

    /**
     * \fn int getHistorySize()
     * \brief Retourne la taille de l'historique.
     */
    int getHistorySize() { return _size; }


public slots:
    /**
     * \fn void undo()
     * \brief Annule la derniere action.
     */
    void undo();

    /**
     * \fn void redo()
     * \brief Effectue a nouveau la derniere action annulee.
     */
    void redo();

    /**
     * \fn void setHistorySize(int size)
     * \brief Definit la taille de l'historique.
     */
    void setHistorySize(int size);

    /**
     * \fn void reset()
     * \brief Reinitialise l'historique.
     */
    void reset();


signals:
    /**
     * \fn void undoRedoChanged(bool undoAvailable, bool redoAvailable,
              const QString &undoCmd, const QString &redoCmd)
     * \brief Indique l'etat de ce ActionManager.
     */
    void undoRedoChanged(bool undoAvailable, bool redoAvailable,
                         const QString& undoCmd, const QString& redoCmd);
    /**
     * \fn void modificationChanged(bool modified)
     * \brief Indique si un undo ou un redo a ete fait.
     */
    void modificationChanged(bool modified);


protected:
    ///L'historique des actions.
    QList<TYAction*> _history;

    ///Index de l'action courante.
    int _current;

    ///Index de l'action a laquelle une sauvegarde a ete effectuee.
    int _savedAt;

    ///Nombre maximum d'actions pour l'historique.
    int _size;

    ///Indique l'etat du manager.
    bool _modified;
};

#endif // __TY_ACTION_MANAGER__
