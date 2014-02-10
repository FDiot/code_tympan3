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
 * \file TYResultatTreeDialog.cpp
 * \brief  Boite de dialogue pour la representation par une arborescence des elements contribuant au resultat d'un point de calcul
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHIHM.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include <qmap.h>
#include <qpushbutton.h>
//#include <qtreeWidget.h>

#include "Tympan/MetierSolver/DataManagerCore/TYElementCollection.h"

#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/MetierSolver/DataManagerCore/TYPreferenceManager.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QShowEvent>
#include <QCloseEvent>
#include <QTreeWidget>

#define TR(id) OLocalizator::getString("TYResultatTreeDialog", (id))


TYResultatTreeDialog::TYResultatTreeDialog(QWidget* parent, const char* name, Qt::WFlags f):
    QDialog(parent,  f), _pPtCalcul(NULL)
{
    setObjectName(name);
    setWindowTitle(TR("id_caption"));
    resize(400, 300);

    QGridLayout* pLayout = new QGridLayout();
    setLayout(pLayout);

    _pListView = new QTreeWidget();
    QStringList stringList;
    stringList.append(TR("id_column_element_type"));
    stringList.append(TR("id_column_element_name"));
    stringList.append(TR("id_column_puissance_dba"));
    stringList.append(TR("id_column_puissance_dblin"));
    _pListView->setColumnCount(4);
    _pListView->setHeaderLabels(stringList);
    _pListView->setSelectionMode(QTreeWidget::NoSelection);
    _pListView->setRootIsDecorated(TRUE);
    _pListView->sortItems(2, Qt::DescendingOrder); // Tri decroissant sur les valeurs en dB(A)
    pLayout->addWidget(_pListView, 0, 0);

    _pCloseButton = new QPushButton(TR("id_close_button"), this);
    pLayout->addWidget(_pCloseButton, 1, 0);
    QObject::connect(_pCloseButton, SIGNAL(clicked()), this, SLOT(close()));
}

TYResultatTreeDialog::~TYResultatTreeDialog()
{
}

void TYResultatTreeDialog::set(LPTYResultat pResultat, LPTYPointCalcul pPtCalcul)
{
    Q_ASSERT(pResultat);
    Q_ASSERT(pPtCalcul);

    _pResultat = pResultat;
    _pPtCalcul = pPtCalcul;

    updateContent();
}

void TYResultatTreeDialog::updateContent()
{
    if (!_pPtCalcul)
    {
        _pListView->clear();
        return;
    }


    // On travaille d'abord sur une arborescence temporaire
    QMap <TYElement*, childTreeItem*> mapElementChilds;

    int nbSources = static_cast<int>(_pResultat->getInitialNbOfSources());
    int indexRecepteur = static_cast<int>(_pResultat->getIndexRecepteur(_pPtCalcul));
    TYElement* pParent = NULL;
    TYElement* pChild  = NULL;

    // Pour chaque source (couple)
    for (int i = 0; i < nbSources; i++)
    {
        LPTYElement pSrc = _pResultat->getElementSource(i);

        pParent = pSrc->getParent();
        pChild  = NULL;

        if (!pParent) { return; } // Securite

        OSpectre spectre = _pResultat->getElementSpectre(indexRecepteur, i);

        // La source ponctuelle utilisateur necessite une entree meme si elle n'a pas de descendant
        if (pSrc->isA("TYUserSourcePonctuelle"))
        {
            // On cree l'entree manquante
            childTreeItem* pChildItem = new childTreeItem();
            pChildItem->spectre = spectre.toDB();
            pChildItem->pChildsList[pChild] = pChild;
            mapElementChilds.insert(pSrc, pChildItem);
        }

        while (pParent)
        {
            // On teste si le parent est present dans le tab de parents
            if (mapElementChilds.contains(pParent))
            {
                // On ajoute le spectre au spectre du parent
                mapElementChilds[pParent]->spectre = mapElementChilds[pParent]->spectre.sumdB(spectre.toDB());
                // On ajoute "l'enfant" a la liste des enfants
                mapElementChilds[pParent]->pChildsList[pChild] = pChild;
            }
            else
            {
                // On cree l'entree manquante
                childTreeItem* pChildItem = new childTreeItem();
                pChildItem->spectre = spectre.toDB();
                pChildItem->pChildsList[pChild] = pChild;
                mapElementChilds.insert(pParent, pChildItem);
            }

            pChild = pParent;
            pParent = pParent->getParent();
        }
    }

    // On parcours ensuite la map pour construire l'arboresecnce dans le bon sens (inch allah)

    QMap<TYElement*, QTreeWidgetItem*> mapParentItem;
    QStringList stringList;
    stringList.append(TYElementWidget::getDisplayName(_pPtCalcul));
    stringList.append(_pPtCalcul->getName());
    QTreeWidgetItem* pRootItem =  new QTreeWidgetItem(_pListView, stringList);
    //  pRootItem->setExpanded(true);
    QTreeWidgetItem* pCurItem = NULL;
    QMap<TYElement*, childTreeItem*>::iterator it;

    for (it = mapElementChilds.begin(); it != mapElementChilds.end(); it++)
    {
        TYElement* pParent = it.key();
        pCurItem = pRootItem;

        if (pParent->isA("TYUserSourcePonctuelle"))
        {
            // Spectre de l'element racine
            OSpectre spectre = it.value()->spectre;
            QStringList stringList;
            stringList.append(TYElementWidget::getDisplayName(pParent));
            stringList.append(pParent->getName());
            pCurItem = new QTreeWidgetItem(pCurItem, stringList);
            pCurItem->setText(2, QString().setNum(spectre.valGlobDBA(), 'f', 2));
            pCurItem->setText(3, QString().setNum(spectre.valGlobDBLin(), 'f', 2));

            // Ajout a la collection de parents
            mapParentItem.insert(pParent, pCurItem);
        }
        else if (pParent->inherits("TYAcousticLine"))
        {
            // Spectre de l'element racine
            OSpectre spectre = it.value()->spectre;
            QStringList stringList;
            stringList.append(TYElementWidget::getDisplayName(pParent));
            stringList.append(pParent->getName());
            pCurItem = new QTreeWidgetItem(pCurItem, stringList);
            pCurItem->setText(2, QString().setNum(spectre.valGlobDBA(), 'f', 2));
            pCurItem->setText(3, QString().setNum(spectre.valGlobDBLin(), 'f', 2));

            // Ajout a la collection de parents
            mapParentItem.insert(pParent, pCurItem);
        }
        else if (pParent->inherits("TYAcousticVolumeNode"))
        {
            QMap<TYElement*, childTreeItem*>::iterator it2  = mapElementChilds.find(pParent);
            followChilds(mapElementChilds, it2, mapParentItem, pCurItem);
        }

        pCurItem = pRootItem;
    }

    // On detruit l'arborescence utilisee pour la construction
    QMap<TYElement*, childTreeItem*>::iterator itdel;

    for (itdel = mapElementChilds.begin(); itdel != mapElementChilds.end(); itdel++)
    {
        TYElement* pParent = itdel.key();

        delete itdel.value();
        itdel.value() = NULL;
    }

}


void TYResultatTreeDialog::followChilds(QMap<TYElement*, childTreeItem*>& mapElementChilds,
                                        QMap<TYElement*, childTreeItem*>::iterator it,
                                        QMap<TYElement*, QTreeWidgetItem*>& mapParentItem,
                                        QTreeWidgetItem* pCurItem)
{
    TYElement* pParent = it.key();
    if (pParent->inherits("TYSourceSurfacic")) { return; }

    OSpectre spectre = it.value()->spectre;
    QStringList stringList;
    stringList.append(TYElementWidget::getDisplayName(pParent));
    stringList.append(pParent->getName());
    pCurItem = new QTreeWidgetItem(pCurItem, stringList);
    pCurItem->setText(2, QString().setNum(spectre.valGlobDBA(), 'f', 2));
    pCurItem->setText(3, QString().setNum(spectre.valGlobDBLin(), 'f', 2));

    QTreeWidgetItem* pOldItem = NULL;

    // Ajout a la collection de parents
    std::map <TYElement*, TYElement*>::iterator itmap;

    for (itmap = it.value()->pChildsList.begin(); itmap != it.value()->pChildsList.end(); itmap++)
    {
        QMap<TYElement*, childTreeItem*>::iterator it2 = mapElementChilds.find((*itmap).first);

        followChilds(mapElementChilds, it2, mapParentItem, pCurItem);
    }
}


/*
void TYResultatTreeDialog::updateContent()
{
    if (!_pPtCalcul)
    {
        _pListView->clear();
        return;
    }

    QMap<TYElement*, QListViewItem*> mapParentItem;

    // Le point de calcul est le root
    QListViewItem * pRootItem =  new QListViewItem(_pListView, TYElementWidget::getDisplayName(_pPtCalcul), _pPtCalcul->getName());
    pRootItem->setOpen(true);

    int nbSources = _pResultat->getInitialNbOfSources();
    int indexRecepteur = _pResultat->getIndexRecepteur(_pPtCalcul);

    // Pour chaque source (couple)
    for (int i = 0; i < nbSources; i++)
    {
        LPTYElement pSrc = _pResultat->getElementSource(i);

        TYElement * pParent = NULL;
        QListViewItem * pCurItem = pRootItem;

            pParent = pSrc->getParent();

        double correct ;
        while (pParent)
        {
            // On teste si le parent est present dans le tab de parents
            if (mapParentItem.contains(pParent)) {
                QString tmp;
                // Ajout de la contribution de ce couple a ce parent
                OSpectre spectre = _pResultat->getElementSpectre(indexRecepteur, i);

                // dBA
                tmp = mapParentItem[pParent]->text(2);
                correct = 10 * log10(pow(10, tmp.toDouble()/10) + pow(10, spectre.valGlobDBA()/10));
                tmp = QString().setNum(correct, 'f', 2);
                mapParentItem[pParent]->setText(2, tmp);

                // dBLin
                tmp = mapParentItem[pParent]->text(3);
                correct = 10 * log10(pow(10, tmp.toDouble()/10) + pow(10, spectre.valGlobDBLin()/10));
                tmp = QString().setNum(correct, 'f', 2);
                mapParentItem[pParent]->setText(3, tmp);
            }
            else
            {
                // Le spectre de ce couple
                OSpectre spectre = _pResultat->getElementSpectre(indexRecepteur, i);

                // Ajout d'un item pour ce parent
                pCurItem = new QListViewItem(pCurItem, TYElementWidget::getDisplayName(pParent), pParent->getName());
                pCurItem->setText(2, QString().setNum(spectre.valGlobDBA(), 'f', 2));
                pCurItem->setText(3, QString().setNum(spectre.valGlobDBLin(), 'f', 2));

                // Ajout a la collection de parents
                mapParentItem.insert(pParent, pCurItem);
            }

            // Parent du parent
            pParent = pParent->getParent();
        }
    }

}
*/

void TYResultatTreeDialog::showEvent(QShowEvent* pEvent)
{
    // On charge les settings
    TYPreferenceManager::getGeometry(metaObject()->className(), this);
}

void TYResultatTreeDialog::closeEvent(QCloseEvent* pEvent)
{
    // On sauve les settings
    TYPreferenceManager::setGeometry(metaObject()->className(), this);

    QDialog::closeEvent(pEvent);
}
