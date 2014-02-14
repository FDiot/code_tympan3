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
 * \file TYElementListItem.h
 * \brief Frame pour les messages de retour (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_ELEMENT_LIST_ITEM__
#define __TY_ELEMENT_LIST_ITEM__


#include "TYAppDefines.h"
#include <qtreewidget.h>

#include "Tympan/MetierSolver/DataManagerCore/TYElement.h"


/**
 * \class TYElementListItem
 * \brief Frame pour les messages de retour.
 */
class TYElementListItem : public QTreeWidgetItem
{
public:
    /**
     * \fn TYElementListItem(QTreeWidget * parent, LPTYElement pElement, LPTYCalcul pCalcul = NULL, const QStringList& labels = QStringList(), bool checkable=false)
     * \brief Constructeur.
     */
    TYElementListItem(QTreeWidget* parent, LPTYElement pElement, LPTYCalcul pCalcul = NULL, const QStringList& labels = QStringList(), bool checkable = false);
    /**
     * \fn TYElementListItem(QTreeWidgetItem * parent, LPTYElement pElement, LPTYCalcul pCalcul = NULL, const QStringList& labels = QStringList(), bool checkable=false)
     * \brief Constructeur.
     */
    TYElementListItem(QTreeWidgetItem* parent, LPTYElement pElement, LPTYCalcul pCalcul = NULL, const QStringList& labels = QStringList(), bool checkable = false);
    /**
     * \fn TYElementListItem(QTreeWidget * parent, const QStringList& labels, LPTYCalcul pCalcul = NULL, bool checkable=false)
     * \brief Constructeur.
     */
    TYElementListItem(QTreeWidget* parent, const QStringList& labels, LPTYCalcul pCalcul = NULL, bool checkable = false);
    /**
     * \fn TYElementListItem(QTreeWidgetItem * parent, const QStringList& labels, LPTYCalcul pCalcul = NULL, bool checkable=false)
     * \brief Constructeur.
     */
    TYElementListItem(QTreeWidgetItem* parent, const QStringList& labels, LPTYCalcul pCalcul = NULL, bool checkable = false);

    /**
     * \fn virtual ~TYElementListItem()
     * \brief Destructeur.
     */
    virtual ~TYElementListItem();

    /**
     * \fn LPTYElement getElement()
     * \brief Retourne l'element associe a cet item.
     */
    LPTYElement getElement() { return _pElement; }

    /**
     * \fn void updateContent()
     * \brief Mets a jour le contenu des colonnes.
     */
    void updateContent();

    /**
     * \fn bool isElementItem()
     * \brief Indique si un element est associe a cet item.
     */
    bool isElementItem() { return _elementItem; }

    /**
     * \fn void remove()
     * \brief Supprime l'item de la liste.
     */
    void remove();

    /**
     * \fn bool isCheckable()
     * \brief Indique si de type checkbox.
     */
    bool isCheckable() { return _checkable; }

    /**
     * \fn void setOn(bool state, bool UpdateModelers)
     * \brief ajouter a la liste
     */
    void setOn(bool state, bool UpdateModelers);

    /**
     * \fn void setCheckState(int column, Qt::CheckState state)
     * \brief Reimplementation
     */
    void setCheckState(int column, Qt::CheckState state);

private:
    /**
     * \fn void updateChilds()
     * \brief update child status
     */
    void updateChilds();

protected:
    void initItem(LPTYElement pElement, LPTYCalcul pCalcul, const QStringList& labels = QStringList(), bool checkable = false);


protected:
    ///Indique si un element est associe a cet item.
    bool _elementItem;

    ///L'element associe a cet item.
    LPTYElement _pElement;

    ///Le Calcul courant.
    LPTYCalcul _pCurrentCalcul;

    ///element de type checkbox ?
    bool _checkable;
};

#endif //__TY_ELEMENT_LIST_ITEM__
