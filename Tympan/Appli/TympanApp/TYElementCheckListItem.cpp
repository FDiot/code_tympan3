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
 * \file TYElementCheckListItem.cpp
 * \brief Frame pour les messages de retour
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/GraphicIHM/DataManagerIHM/TYWidget.h"
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerCore/TYPreferenceManager.h"


#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYMachine.h"


/**
 * Identifie la classe TYElementCheckListItem pour la distinguer de sa classe parent QCheckListItem.
 * Cette valeur est retournee par 'virtual int TYElementCheckListItem::rtti()'.
 */
/*static*/ int TYElementCheckListItem::RTTI = 1002;


#define TR(id) OLocalizator::getString("TYElementCheckListItem", (id))


TYElementCheckListItem::TYElementCheckListItem(QTreeWidget* parent, LPTYElement pElement, LPTYCalcul pCalcul, const QStringList& labels /*=QString::null*/) :
    QTreeWidgetItem(parent, labels)
{
    initItem(pElement, pCalcul, labels);
}

TYElementCheckListItem::TYElementCheckListItem(QTreeWidgetItem* parent, LPTYElement pElement, LPTYCalcul pCalcul, const QStringList& labels /*=QString::null*/) :
    QTreeWidgetItem(parent, labels)
{
    initItem(pElement, pCalcul, labels);
}

TYElementCheckListItem::TYElementCheckListItem(QTreeWidget* parent, const QStringList& labels, LPTYCalcul pCalcul) :
    QTreeWidgetItem(parent, labels)
{
    initItem(NULL, pCalcul, labels);
}

TYElementCheckListItem::TYElementCheckListItem(QTreeWidgetItem* parent, const QStringList& labels, LPTYCalcul pCalcul) :
    QTreeWidgetItem(parent, labels)
{
    initItem(NULL, pCalcul, labels);
}

TYElementCheckListItem::~TYElementCheckListItem()
{
}

void TYElementCheckListItem::initItem(LPTYElement pElement, LPTYCalcul pCalcul, const QStringList& labels /*=QString::null*/)
{
    _pElement = pElement;
    _pCurrentCalcul = pCalcul;
    _elementItem = _pElement ? true : false;

    if (labels.isEmpty())
    {
        setText(0, TYWidget::getDisplayName(_pElement));
    }
    else
    {
        setText(0, labels[0]);
    }

    updateContent();
}

void TYElementCheckListItem::updateContent()
{
    if (_pElement)
    {
        // Nom
        setText(1, _pElement->getName());

        // Coche si present dans le calcul courant
        QTreeWidgetItem::setCheckState(0, _pElement->isInCurrentCalcul() ? Qt::Checked : Qt::Unchecked);

        // Mise a jour des items des parents
        QTreeWidgetItem* pParentItem = this->parent();

        while (pParentItem)
        {
            TYElementCheckListItem* pCheckItem = (TYElementCheckListItem*) pParentItem;
            if (pCheckItem->getElement())
            {
                pCheckItem->QTreeWidgetItem::setCheckState(0, pCheckItem->getElement()->isInCurrentCalcul() ? Qt::Checked : Qt::Unchecked);
            }
            pParentItem = pParentItem->parent();
        }
    }
}

void TYElementCheckListItem::remove()
{
    if (childCount() == 0)
    {
        if (parent() != NULL) {}
        //          parent()->takeItem( this );
    }
    else
    {
        TYElementCheckListItem* childItem;

        while (childCount() > 0)
        {
            childItem = (TYElementCheckListItem*) child(0);
            childItem->remove();
        }
        remove();
    }
}


void TYElementCheckListItem::setOn(bool state, bool UpdateModelers)
{
    if (_pElement && _pCurrentCalcul)
    {
        // Si le calcul courant est bloque, on empeche le changement d'etat
        //if (_pCurrentCalcul->getState()==TYCalcul::Locked) // DTn : Desactive 20110915
        //{
        //  updateContent(); // On met a jour a tout hasard ...
        //  return;
        //}

        if (_pElement->isInCurrentCalcul() != state)
        {
            if (getTYApp()->getCalculManager()->askForResetResultat())
            {
                if (state)
                {
                    _pCurrentCalcul->addToSelection(_pElement);
                }
                else
                {
                    _pCurrentCalcul->remToSelection(_pElement);
                }

                if (UpdateModelers)
                {
                    if (_pElement->getParent())
                    {
                        _pElement->getParent()->setIsGeometryModified(true);
                        _pElement->getParent()->updateGraphicTree();
                    }
                    else
                    {
                        _pElement->updateGraphic();
                    }

                    getTYMainWnd()->updateModelers(false, false);
                }
            }
        }
    }
    else
    {
        // Toujours actif quand il n'y a pas de calcul courant
        state = true;
    }

    // Mise a jour de l'item
    updateContent();
}

void TYElementCheckListItem::setCheckState(Qt::CheckState state)
{
    setOn(state == Qt::Checked, true);
}

int TYElementCheckListItem::rtti() const
{
    return RTTI;
}
