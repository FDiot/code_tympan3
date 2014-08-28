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
 * \file TYElementListItem.cpp
 * \brief Frame pour les messages de retour
 */

#include <qpixmap.h>

#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/TYPreferenceManager.h"
#include "Tympan/models/business/TYProjet.h"
#include "Tympan/models/business/TYPluginManager.h"
#include "Tympan/gui/widgets/TYWidget.h"
#include "Tympan/gui/app/TYApplication.h"
#include "Tympan/gui/app/TYMainWindow.h"
#include "Tympan/gui/app/TYSiteFrame.h"
#include "TYElementListItem.h"


#define TR(id) OLocalizator::getString("TYElementListItem", (id))
#define IMG(id) OLocalizator::getPicture("TYElementListItem", (id))


TYElementListItem::TYElementListItem(QTreeWidget* parent, LPTYElement pElement, LPTYCalcul pCalcul, const QStringList& labels /*=QStringList()*/, bool checkable) :
    QTreeWidgetItem(parent)
{
    initItem(pElement, pCalcul, labels, checkable);
}

TYElementListItem::TYElementListItem(QTreeWidgetItem* parent, LPTYElement pElement, LPTYCalcul pCalcul, const QStringList& labels /*=QStringList()*/, bool checkable) :
    QTreeWidgetItem(parent)
{
    initItem(pElement, pCalcul, labels, checkable);
}

TYElementListItem::TYElementListItem(QTreeWidget* parent, const QStringList& labels, LPTYCalcul pCalcul, bool checkable) :
    QTreeWidgetItem(parent, labels)
{
    initItem(NULL, pCalcul, labels, checkable);
}

TYElementListItem::TYElementListItem(QTreeWidgetItem* parent, const QStringList& labels, LPTYCalcul pCalcul, bool checkable) :
    QTreeWidgetItem(parent, labels)
{
    initItem(NULL, pCalcul, labels, checkable);
}

TYElementListItem::~TYElementListItem()
{
}

void TYElementListItem::initItem(LPTYElement pElement, LPTYCalcul pCalcul, const QStringList& labels /*=QStringList()*/, bool checkable)
{
    _pElement = pElement;
    _pCurrentCalcul = pCalcul;
    _checkable = checkable;
    _elementItem = _pElement ? true : false;

    if (labels.isEmpty())
    {
        //      setText(0, TYWidget::getDisplayName(_pElement)); // Commente en  VERSION 3.6.2
    }
    else
    {
        setText(0, labels[0]);
    }

    updateContent();
}

void TYElementListItem::updateContent()
{
    if (_pElement)
    {
        setText(0, _pElement->getName());// VERSION 3.6.2
        //      setText(1, _pElement->getName());

        bool bInCurrentCalcul = false;

        if (_pElement->isA("TYCalcul"))
        {
            // Nom du solveur utilisé par le calcul
            OGenID currentId = TYCalcul::safeDownCast(_pElement)->getSolverId();

            setText(1, TYPluginManager::get()->getInfo("name", currentId));
        }
        else if (dynamic_cast<TYPointCalcul*>(_pElement._pObj) != nullptr)
        {
            TYPointControl* pPoint = TYPointControl::safeDownCast(_pElement);
            TYCalcul* pCalcul = static_cast<TYProjet*>(pPoint->getParent())->getCurrentCalcul();
            bInCurrentCalcul = pPoint->getEtat(pCalcul);
            if (bInCurrentCalcul)
            {
                pCalcul->addPtCtrlToResult(pPoint);
            }
            else
            {
                pCalcul->remPtCtrlFromResult(pPoint);
            }
        }
        else
        {
            if (_pCurrentCalcul)
            {
                bInCurrentCalcul = _pCurrentCalcul->isInSelection(_pElement);
            }
            else
            {
                bInCurrentCalcul = _pElement->isInCurrentCalcul();
            }
        }

        if (_checkable)
        {
            QTreeWidgetItem::setCheckState(0, bInCurrentCalcul ? Qt::Checked : Qt::Unchecked);
        }

        //       Mise a jour des items des parents
        QTreeWidgetItem* pParentItem = this->parent();
        bInCurrentCalcul = false;
        while (pParentItem)
        {
            TYElementListItem* pCheckItem = (TYElementListItem*) pParentItem;
            TYElement* pChkElt = pCheckItem->getElement();
            if (pChkElt && pCheckItem->isCheckable())
            {
                if (_pCurrentCalcul)
                {
                    bInCurrentCalcul = _pCurrentCalcul->isInSelection(pChkElt);
                }
                else
                {
                    bInCurrentCalcul = pChkElt->isInCurrentCalcul();
                }

                pCheckItem->QTreeWidgetItem::setCheckState(0, bInCurrentCalcul ? Qt::Checked : Qt::Unchecked);
            }

            pParentItem = pParentItem->parent();
        }

        // Mise à jour des enfant aussi ...
        updateChilds();
    }
}

void TYElementListItem::remove()
{
    if (childCount() == 0)
    {
        if (parent() != NULL)
        {
            return;
        }
    }
    else
    {
        TYElementListItem* childItem;

        while (childCount() > 0)
        {
            childItem = (TYElementListItem*) child(0);
            childItem->remove();
            delete childItem;
        }

        // Appel recursif
        remove();
    }
}

void TYElementListItem::setOn(bool state, bool UpdateModelers)
{
    if (_pElement && _pCurrentCalcul && _checkable)
    {
        // Si le calcul courant est bloque, on empeche le changement d'etat
        //if (_pCurrentCalcul->getState()==TYCalcul::Locked) // DTn : Desactive 20110915
        //{
        //  updateContent(); // On met a jour a tout hasard ...
        //  return;
        //}

        // On traite a part le cas des points de controle
        if (dynamic_cast<TYPointCalcul*>(_pElement._pObj) != nullptr)
        {
            TYPointControl* pPoint = static_cast<TYPointControl*>(_pElement.getRealPointer());
            if (pPoint->getEtat(_pCurrentCalcul) != state)
            {
                if (getTYApp()->getCalculManager()->askForResetResultat())
                {
                    pPoint->setEtat(state, _pCurrentCalcul);
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

                    getTYMainWnd()->updateModelers(false, false, true);
                }

            }
        }
        else if (_pCurrentCalcul->isInSelection(_pElement) != state)
        {
            if (getTYApp()->getCalculManager()->askForResetResultat())
            {
                if (state)
                {
                    _pElement->setIsAcousticModified(true);
                    _pElement->getParent()->setIsAcousticModified(true);
                    _pCurrentCalcul->addToSelection(_pElement, true);


                }
                else
                {
                    _pCurrentCalcul->remToSelection(_pElement, true);
                }

                if (_pElement->isA("TYSiteNode"))
                {
                    TYSiteNode* pSite = dynamic_cast<TYSiteNode*>(_pElement.getRealPointer());
                    getTYMainWnd()->getSiteFrame()->altimetryNeedsUpdate();
                    LPTYCalcul pCalc = NULL;
                    if (getTYApp()->getCurProjet() && pSite)
                    {
                        pCalc = getTYApp()->getCurProjet()->getCurrentCalcul();
                        if (pCalc) { pCalc->getCalculElements(pSite); }
                    }
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

                    getTYMainWnd()->updateModelers(false, false, true);
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

void TYElementListItem::setCheckState(int column, Qt::CheckState state)
{
    if (_checkable)
    {
        setOn(state == Qt::Checked, true);

        QTreeWidgetItem::setCheckState(column, state);
    }
}

void TYElementListItem::updateChilds()
{
    int  nbchilds = this->childCount();
    for (int i = 0; i < nbchilds; i++)
    {
        TYElementListItem* childItem = (TYElementListItem*) child(i);
        TYElement* pChkElt = childItem->getElement();
        bool bInCurrentCalcul = false;
        if (pChkElt && childItem->isCheckable())
        {
            if (_pCurrentCalcul)
            {
                bInCurrentCalcul = _pCurrentCalcul->isInSelection(pChkElt);
            }
            else
            {
                bInCurrentCalcul = pChkElt->isInCurrentCalcul();
            }

            childItem->QTreeWidgetItem::setCheckState(0, bInCurrentCalcul ? Qt::Checked : Qt::Unchecked);
        }

        childItem->updateChilds();
    }
}
