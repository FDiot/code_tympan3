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
 * \file TYElementCheckListItem.h
 * \brief Frame pour les messages de retour (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_ELEMENT_CHECK_LIST_ITEM__
#define __TY_ELEMENT_CHECK_LIST_ITEM__


#include "TYAppDefines.h"
#include <qtreewidget.h>

#include "Tympan/models/business/TYCalcul.h"


/**
 * \class TYElementCheckListItem
 * \brief Frame pour les messages de retour
 */
class TYElementCheckListItem : public QTreeWidgetItem
{
public:
    /**
     * \fn TYElementCheckListItem(QTreeWidget * parent, LPTYElement pElement, LPTYCalcul pCalcul = NULL, const QStringList& labels = QStringList())
     * \brief Constructeur.
     */
    TYElementCheckListItem(QTreeWidget* parent, LPTYElement pElement, LPTYCalcul pCalcul = NULL, const QStringList& labels = QStringList());
    /**
     * \fn TYElementCheckListItem(QTreeWidgetItem * parent, LPTYElement pElement, LPTYCalcul pCalcul = NULL, const QStringList& labels = QStringList())
     * \brief Constructeur.
     */
    TYElementCheckListItem(QTreeWidgetItem* parent, LPTYElement pElement, LPTYCalcul pCalcul = NULL, const QStringList& labels = QStringList());
    /**
     * \fn TYElementCheckListItem(QTreeWidget * parent, const QStringList& labels, LPTYCalcul pCalcul = NULL)
     * \brief Constructeur.
     */
    TYElementCheckListItem(QTreeWidget* parent, const QStringList& labels, LPTYCalcul pCalcul = NULL);
    /**
     * \fn TYElementCheckListItem(QTreeWidgetItem * parent, const QStringList& labels, LPTYCalcul pCalcul = NULL)
     * \brief Constructeur.
     */
    TYElementCheckListItem(QTreeWidgetItem* parent, const QStringList& labels, LPTYCalcul pCalcul = NULL);

    /**
     * \fn virtual ~TYElementCheckListItem()
     * \brief Destructeur.
     */
    virtual ~TYElementCheckListItem();


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
     * \fn virtual void setCheckState(Qt::CheckState)
     * \brief Surcharge de QCheckListItem::setCheckState() pour la gestion de calcul courant
     * de l'element.
     * Appelle setOn(bool state, bool UpdateModelers=true)
     */
    virtual void setCheckState(Qt::CheckState);

    /**
     * \fn void setOn(bool state, bool UpdateModelers)
     * \brief Sert a ne pas mettre a jour les modelers dans le cas de selection multiples
     */
    void setOn(bool state, bool UpdateModelers);

protected:
    void initItem(LPTYElement pElement, LPTYCalcul pCalcul, const QStringList& labels = QStringList());

    virtual void stateChange(bool state)
    {
        bool _st = (checkState(0) == Qt::Checked);
        if (_st != state) { setOn(state, true); }
    }

protected:
    ///Indique si un element est associe a cet item.
    bool _elementItem;

    ///L'element associe a cet item.
    LPTYElement _pElement;

    ///Le Calcul courant.
    LPTYCalcul _pCurrentCalcul;
};

#endif //__TY_ELEMENT_CHECK_LIST_ITEM__
