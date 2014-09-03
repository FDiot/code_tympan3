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
 * \file TYProjetFrame.cpp
 * \brief Frame pour la gestion de projet
 */


#include <qcursor.h>
#include <qtreewidget.h>
#include <qmenubar.h>
#include <qmenu.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include <qdialog.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <QTextStream>
//Added by qt3to4:
#include <QKeyEvent>
#include <QPixmap>
#include <QVBoxLayout>
#include <QBoxLayout>
#include <QHeaderView>
#include <QMdiArea>
#include <QMdiSubWindow>

#include "Tympan/models/business/TYXMLManager.h"
#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/TYPreferenceManager.h"
#include "Tympan/models/business/TYRectangularMaillage.h"
#include "Tympan/gui/tools/NxVec3.h"
#include "Tympan/gui/widgets/TYEtatsWidget.h"
#include "Tympan/gui/app/TYElementListItem.h"
#include "Tympan/gui/app/TYRenderWindowInteractor.h"
#include "Tympan/gui/app/TYModelerFrame.h"
#include "Tympan/gui/app/TYSiteFrame.h"
#include "Tympan/gui/app/TYApplication.h"
#include "Tympan/gui/app/TYMainWindow.h"
#include "TYProjetFrame.h"

#define TR(id) OLocalizator::getString("TYProjetFrame", (id))
#define IMG(id) OLocalizator::getPicture("TYProjetFrame", (id))

using namespace Qt;


TYProjetFrame::TYProjetFrame(QWidget* parent, const char* name, Qt::WFlags f):
    QWidget(parent, f)
{
    setObjectName(name);
    QBoxLayout* pLayout = new QVBoxLayout(this);
    pLayout->setMargin(0);
    setLayout(pLayout);

    // List view
    _pListView = new QTreeWidget(this);
    QStringList stringList;
    stringList.append(TR("id_column_projet"));
    stringList.append(TR("id_column_solver"));
    _pListView->setColumnCount(2);
    _pListView->setHeaderLabels(stringList);
    _pListView->resizeColumnToContents(0);

    pLayout->addWidget(_pListView);

    _pProjet = NULL;
    _pCurrentCalcul = NULL;
    _pCurrentCalculItem = NULL;

    // Signaux
    QObject::connect(_pListView, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
                     this, SLOT(doubleClic(QTreeWidgetItem*, int)));
    QObject::connect(_pListView, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
                     this, SLOT(apply(QTreeWidgetItem*, int)));
}

TYProjetFrame::~TYProjetFrame()
{
    _pListView->disconnect();
}

void TYProjetFrame::setProjet(LPTYProjet pProjet)
{
    if (_pProjet && _pProjet->getGraphicObject())
    {
        _pProjet->getGraphicObject()->disconnectUpdateSignal(this, SLOT(updateList()));
    }

    _pProjet = pProjet;

    if (_pProjet)
    {
        // On signale que le site courant a change
        emit changeSite(_pProjet->getSite());

        if (_pProjet->getGraphicObject())
        {
            _pProjet->getGraphicObject()->connectUpdateSignal(this, SLOT(updateList()));
        }
    }
    else
    {
        // Plus de calcul courant
        _pCurrentCalcul = NULL;

        // On signale le changement de calcul courant
        emit changeCurrentCalcul(_pCurrentCalcul);
    }

    updateList();
}

void TYProjetFrame::updateList()
{
    clear();

    if (!_pProjet) { return; }

    TYElementListItem* pRootItem = addToList(_pProjet);
    _pListView->setItemExpanded(pRootItem, true);
    _pListView->resizeColumnToContents(0);
}

TYElementListItem* TYProjetFrame::addToList(LPTYProjet pElement, TYElementListItem* parent /*=NULL*/)
{
    unsigned int i;
    TYElementListItem* pRootItem  = addEltToList(pElement, parent);

    TYElementListItem* pListPointsControlItem = new TYElementListItem(pRootItem, QStringList(TR("id_list_pointscontrol_item")));
    for (i = 0; i < pElement->getPointsControl().size(); i++)
    {
        new TYElementListItem(pListPointsControlItem, pElement->getPointControl(i), pElement->getCurrentCalcul(), QStringList(), true);
    }

    for (i = 0; i < pElement->getListCalcul().size(); i++)
    {
        addToList(pElement->getListCalcul()[i], pRootItem);
    }

    return pRootItem;
}

TYElementListItem* TYProjetFrame::addToList(LPTYCalcul pElement, TYElementListItem* parent /*=NULL*/)
{
    TYElementListItem* pRootItem = addEltToList(pElement, parent);

    // Icone indiquant si ce calcul est le calcul courant du projet
    if (_pProjet->getCurrentCalcul() == pElement)
    {
        _pCurrentCalcul = pElement;
        pRootItem->setIcon(0, QPixmap(IMG("id_icon_check")));
        emit changeCurrentCalcul(_pCurrentCalcul);

        _pCurrentCalculItem = pRootItem;
    }
    else
    {
        pRootItem->setIcon(0, QPixmap(IMG("id_icon_no_check")));
    }

    setCalculItemState(pRootItem, pElement->getIsUpTodate());

    unsigned int i;
    new TYElementListItem(pRootItem, pElement->getAtmosphere(), NULL);
    new TYElementListItem(pRootItem, NULL, NULL, QStringList(TR("id_Etats_item")));

    TYElementListItem* pListMaillageItem = new TYElementListItem(pRootItem, QStringList(TR("id_list_maillages_item")));
    for (i = 0; i < pElement->getMaillages().size(); i++)
    {
        new TYElementListItem(pListMaillageItem, pElement->getMaillage(i));
    }

    new TYElementListItem(pRootItem, pElement->getResultat());

    _pListView->setItemExpanded(pRootItem, true);

    return pRootItem;
}

void TYProjetFrame::setCalculDone(bool state)
{
    if (state == true)
    {
        _pCurrentCalcul->setIsUpTodate(state);
        if (_pCurrentCalculItem != NULL)
        {
            setCalculItemState(_pCurrentCalculItem, true);
        }
    }
    else
    {
        // Rem: Les autres calculs seront systematiquement recalcule lors qu'ils deviendront actifs
        QTreeWidgetItem* pProjetItem = _pListView->topLevelItem(0);
        for (int i = 0; i < pProjetItem->childCount(); i++)
        {
            TYElementListItem* pCalcItem = static_cast<TYElementListItem*>(pProjetItem->child(i));
            TYCalcul* pCalcul = static_cast<TYCalcul*>((TYElement*)pCalcItem->getElement());
            if (pCalcul != NULL && pCalcul->getIsAcousticModified())
            {
                pCalcul->setIsUpTodate(state);
                setCalculItemState(pCalcItem, false);
            }
        }
    }
}

void TYProjetFrame::setCalculItemState(TYElementListItem* pCalcItem, bool state)
{
    if (pCalcItem != NULL)
    {
        QFont f = pCalcItem->font(0);
        f.setItalic(!state);
        if (state == true)
        {
            pCalcItem->setTextColor(0, QColor::fromRgb(0, 0, 0));
            pCalcItem->setText(0, pCalcItem->text(0).replace(" *", ""));
        }
        else
        {
            pCalcItem->setTextColor(0, QColor::fromRgb(128, 128, 128));
            pCalcItem->setText(0, pCalcItem->text(0).replace(" *", ""));
            pCalcItem->setText(0, pCalcItem->text(0) + " *");
        }
        pCalcItem->setFont(0, f);
    }
}

void TYProjetFrame::contextMenuEvent(QContextMenuEvent* e)
{
    //si on trouve un item associe
    QPoint point = _pListView->mapFrom(this, e->pos());
    QTreeWidgetItem* item = _pListView->itemAt(point.x(), point.y() - _pListView->header()->height());

    if (item != NULL)
    {
        TYElementListItem* eltItem = (TYElementListItem*) item;

        if (eltItem->isElementItem())
        {
            QMenu* pMenu = new QMenu(this);

            QAction* curCalcul = NULL;
            QAction* remCalculRes = NULL;
            QAction* goCalcul = NULL;
            QAction* duplicateCalcul = NULL;
            QAction* showModeler = NULL;
            QAction* delCalcul = NULL;
            QAction* addCalcul = NULL;
            QAction* remPtControl = NULL;
            QAction* duplicatePtControl = NULL;
            QAction* highlight = NULL;  // DTn 20110215
            QAction* remMaillage = NULL;
            QAction* changeSiteRoot = NULL;
            QAction* exportCsv = NULL;
            QAction* actif = NULL, *inactif = NULL , *calc_actif = NULL, *calc_locked = NULL, *mail_actif = NULL, *mail_inactif = NULL;
            QAction* exportInt = NULL;

            TYElementGraphic* pGraphicObject = NULL;

            // En tete de menu l'acces aux proprietes de l'element
            QAction* prop = pMenu->addAction(QIcon(QPixmap(IMG("id_icon_editeelt"))), TR("id_contextmenu_properties"));
            pMenu->addSeparator();

            // Calcul
            if (eltItem->getElement()->isA("TYCalcul"))
            {
                // Suppression des resultats
                remCalculRes = pMenu->addAction(QIcon(QPixmap(IMG("id_icon_bin"))), TR("id_contextmenu_remcalculresult"));
                pMenu->addSeparator();

                // Calcul courant
                if (_pProjet->getCurrentCalcul() != NULL)
                {
                    if (_pProjet->getCurrentCalcul()->getID() != eltItem->getElement()->getID())
                    {
                        curCalcul = pMenu->addAction(TR("id_contextmenu_setascurcalcul"));
                        pMenu->addSeparator();
                    }
                }
                else
                {
                    curCalcul = pMenu->addAction(TR("id_contextmenu_setascurcalcul"));
                    pMenu->addSeparator();
                }

                calc_actif = pMenu->addAction(TR("id_contextmenu_ptactif"));
                if (((TYCalcul*) eltItem->getElement().getRealPointer())->getState() == TYCalcul::Actif)
                {
                    calc_actif->setCheckable(true);
                    calc_actif->setChecked(true);
                }

                calc_locked = pMenu->addAction(TR("id_contextmenu_ptlocked"));
                if (((TYCalcul*) eltItem->getElement().getRealPointer())->getState() == TYCalcul::Locked)
                {
                    calc_locked->setCheckable(true);
                    calc_locked->setChecked(true);
                }

                pMenu->addSeparator();

                // Lancer calcul (si le calcul est actif)
                if (((TYCalcul*) eltItem->getElement().getRealPointer())->getState() == TYCalcul::Actif)
                {
                    goCalcul = pMenu->addAction(QIcon(QPixmap(IMG("id_icon_calcul"))), TR("id_contextmenu_gocalcul"));
                    pMenu->addSeparator();
                }

                // Duplication
                duplicateCalcul = pMenu->addAction(QIcon(QPixmap(IMG("id_icon_duplicate"))), TR("id_contextmenu_duplicatecalcul"));

                pMenu->addSeparator();
                delCalcul = pMenu->addAction(QIcon(QPixmap(IMG("id_icon_del"))), TR("id_contextmenu_delete"));
                pMenu->addSeparator();
                TYElement::setIsSavedOk(true);
            }
            // Projet
            else if (dynamic_cast<TYProjet*>(eltItem->getElement()._pObj) != nullptr)
            {
                // Ajout calcul
                addCalcul = pMenu->addAction(TR("id_contextmenu_addcalcul"));
                pMenu->addSeparator();

                // Suppression des resultats
                remCalculRes = pMenu->addAction(QIcon(QPixmap(IMG("id_icon_bin"))), TR("id_contextmenu_remcalculresults"));
                pMenu->addSeparator();

                // Modeler
                showModeler = pMenu->addAction(QIcon(QPixmap(IMG("id_icon_modeler"))), TR("id_contextmenu_modeler"));
                pMenu->addSeparator();

                // Changer le site racine
                changeSiteRoot = pMenu->addAction(TR("id_contextmenu_changesiteroot"));
            }
            else if (eltItem->getElement()->isA("TYPointControl"))
            {
                // On recupere l'objet graphique de l'element
                pGraphicObject = eltItem->getElement()->getGraphicObject()._pObj;
                if (pGraphicObject)
                {
                    highlight = pMenu->addAction(TR("id_contextmenu_highlight"));
                    highlight->setCheckable(true);
                    highlight->setChecked(pGraphicObject->getHighlightState());
                    pMenu->addSeparator();
                }

                // Export
                exportInt = pMenu->addAction(QIcon(QPixmap(IMG("id_icon_export"))), TR("id_contextmenu_export"));
                pMenu->addSeparator();
                duplicatePtControl = pMenu->addAction(QIcon(QPixmap(IMG("id_icon_duplicate"))), TR("id_contextmenu_dupptcontrol"));
                pMenu->addSeparator();
                remPtControl = pMenu->addAction(QIcon(QPixmap(IMG("id_icon_del"))), TR("id_contextmenu_remptcontrol"));
                pMenu->addSeparator();
            }
            else if (dynamic_cast<TYMaillage*>(eltItem->getElement()._pObj) != nullptr) // Maillage
            {
                // Si calcul parent bloque impossible de changer etat ou supprimer un point de controle
                LPTYCalcul pCalc = TYCalcul::safeDownCast(eltItem->getElement()->getParent());
                if (pCalc && pCalc->getState() == TYCalcul::Actif)
                {
                    mail_actif = pMenu->addAction(TR("id_contextmenu_ptactif"));
                    if (((TYMaillage*) eltItem->getElement().getRealPointer())->getState() == TYMaillage::Actif)
                    {
                        mail_actif->setCheckable(true);
                        mail_actif->setChecked(true);
                    }

                    mail_inactif = pMenu->addAction(TR("id_contextmenu_ptinactif"));
                    if (((TYMaillage*) eltItem->getElement().getRealPointer())->getState() == TYMaillage::Inactif)
                    {
                        mail_inactif->setCheckable(true);
                        mail_inactif->setChecked(true);
                    }
                    pMenu->addSeparator();
                    exportCsv = pMenu->addAction(TR("id_contextmenu_exportcsv"));
                    pMenu->addSeparator();
                    remMaillage = pMenu->addAction(QIcon(QPixmap(IMG("id_icon_del"))), TR("id_contextmenu_remmaillage"));
                    pMenu->addSeparator();
                }
            }

            // AFFICHAGE DU MENU
            QAction* ret = pMenu->exec(_pListView->mapToGlobal(point));

            // ANALYSE DU CHOIX DE L'UTILISATEUR
            if (ret)
            {
                if (ret == prop)
                {
                    LPTYElement pElt = eltItem->getElement();

                    if (pElt->edit(this) == QDialog::Accepted)
                    {
                        eltItem->updateContent();
                    }

                    if (pElt->isA("TYRectangularMaillage"))
                    {
                        TYRectangularMaillage* pRectMaillage = (LPTYRectangularMaillage&) pElt;
                        LPTYElementGraphic pMGr = pRectMaillage->getGraphicObject();
                        pRectMaillage->updateGraphic();
                    }
                    else
                    {
                        pElt->updateGraphic();
                    }

                    getTYMainWnd()->updateModelers(true, true, true);
                }
                else if (ret == showModeler)
                {

                    if (dynamic_cast<TYProjet*>(eltItem->getElement()._pObj) != nullptr)
                    {
                        getTYMainWnd()->makeProjetModeler((TYProjet*) eltItem->getElement().getRealPointer());
                    }
                }
                else if (ret == remPtControl)
                {
                    if ((dynamic_cast<TYPointControl*>(eltItem->getElement()._pObj) != nullptr)
                        && (dynamic_cast<TYProjet*>(eltItem->getElement()->getParent()) != nullptr))
                    {
                        LPTYProjet pProjet = (TYProjet*) eltItem->getElement()->getParent();

                        if (getTYApp()->getCalculManager()->askForResetResultat())
                        {
                            if (QMessageBox::warning(this, "Tympan", TR("id_warning_suppr").arg(eltItem->getElement()->getName()), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
                            {
                                if (pProjet->remPointControl((TYPointControl*) eltItem->getElement().getRealPointer()))
                                {
                                    // Reset du tableau des resultats des calculs
                                    TYTabLPCalcul listCalcul = pProjet->getListCalcul();
                                    for (int i = 0; i < listCalcul.size(); i++)
                                    {
                                        listCalcul[i]->getResultat()->purge();
                                    }
                                    pProjet->updateGraphic();
                                    getTYMainWnd()->updateModelers(false, false);
                                    updateList();
                                    TYElement::setIsSavedOk(true);
                                }
                            }
                        }
                    }
                }
                else if (ret == duplicatePtControl)
                {
                    TYPointControl* pPointCtrl = dynamic_cast<TYPointControl*>(
                            eltItem->getElement()._pObj);
                    if (pPointCtrl != nullptr)
                    {
                        TYProjet* pProjet = dynamic_cast<TYProjet*>(
                                eltItem->getElement()->getParent());
                        if (pProjet != nullptr)
                        {
                        if (getTYApp()->getCalculManager()->askForResetResultat())
                        {
                            TYPointControl* pPoint = pProjet->duplicatePointControl(pPointCtrl);
                            if (pPoint)
                            {
                                pPoint->edit();
                            }

                            pProjet->updateGraphic();
                            getTYMainWnd()->updateModelers(false, false);
                            updateList();
                            TYElement::setIsSavedOk(true);
                        }
                        }
                    }

                }
                else if (ret == actif)
                {
                    ((TYPointControl*) eltItem->getElement().getRealPointer())->setEtat(true, _pCurrentCalcul);
                    getTYMainWnd()->updateModelers(true, true, true);
                }
                else if (ret == inactif)
                {
                    ((TYPointControl*) eltItem->getElement().getRealPointer())->setEtat(false, _pCurrentCalcul);
                    getTYMainWnd()->updateModelers(true, true, true);
                }
                else if (ret == highlight)
                {
                    if (pGraphicObject)
                    {
                        bool bCenterOnLocate = false;
                        if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "CenterOnLocate"))
                        {
                            bCenterOnLocate = TYPreferenceManager::getBool(TYDIRPREFERENCEMANAGER, "CenterOnLocate");
                        }
                        // Toggle le highlight
                        pGraphicObject->highlight(!pGraphicObject->getHighlightState());

                        if (pGraphicObject->getHighlightState())
                        {
                            QList<QMdiSubWindow*> windows =    getTYMainWnd()->getWorkspace()->subWindowList();

                            for (int i = 0; i < int(windows.count()); ++i)
                            {
                                QWidget* internal_window = windows.at(i)->widget();

                                QString qClassName = internal_window->metaObject()->className();
                                if (qClassName == QString("TYSiteModelerFrame"))
                                {
                                    TYRenderWindowInteractor* pView = ((TYModelerFrame*) internal_window)->getView();
                                    pView->getRenderer()->addSelectedElement(eltItem->getElement());
                                }
                            }
                        }

                        if (bCenterOnLocate && pGraphicObject->getHighlightState())
                        {
                            pGraphicObject->computeBoundingBox();
                            TYElement* pElement = eltItem->getElement()._pObj;
                            TYGeometryNode* pGeoNode = TYGeometryNode::GetGeoNode(pElement);
                            if (pGeoNode)
                            {
                                ORepere3D repere = pGeoNode->getORepere3D();
                                NxVec3 repereCenter = NxVec3(repere._origin._x, repere._origin._z, -repere._origin._y);
                                OBox oBox = pGraphicObject->GetBox();
                                NxVec3 oBoxMin = NxVec3(oBox._min._x, oBox._min._z, -oBox._min._y);
                                NxVec3 oBoxMax = NxVec3(oBox._max._x, oBox._max._z, -oBox._max._y);
                                NxVec3 oBoxCenter = oBoxMin + ((oBoxMax - oBoxMin) / 2);
                                NxVec3 center = repereCenter + oBoxCenter;

                                QList<QMdiSubWindow*> windows =    getTYMainWnd()->getWorkspace()->subWindowList();

                                for (int i = 0; i < int(windows.count()); ++i)
                                {
                                    QWidget* internal_window = windows.at(i)->widget();

                                    if (dynamic_cast<TYModelerFrame*>(internal_window) != nullptr)
                                    {
                                        TYRenderWindowInteractor* pView = ((TYModelerFrame*) internal_window)->getView();
                                        pView->getRenderer()->getActiveCamera()->setTo(center.get());
                                    }
                                }
                            }
                        }

                        getTYMainWnd()->updateModelers();//az++ pour la fonction localiser

                        //il faut un update avant d'enlever l'etat de "locate" sur l'objet
                        if (!pGraphicObject->getHighlightState())
                        {
                            QList<QMdiSubWindow*> windows =    getTYMainWnd()->getWorkspace()->subWindowList();

                            for (int i = 0; i < int(windows.count()); ++i)
                            {
                                QWidget* internal_window = windows.at(i)->widget();

                                QString qClassName = internal_window->metaObject()->className();
                                if (qClassName == QString("TYSiteModelerFrame"))
                                {
                                    getTYMainWnd()->updateModelers();//az++ pour localiser
                                    TYRenderWindowInteractor* pView = ((TYModelerFrame*) internal_window)->getView();
                                    pView->getRenderer()->removeSelectedElement(eltItem->getElement());
                                }
                            }
                        }
                    }
                }
                else if (ret == remMaillage)
                {
                    TYMaillage* pMail = dynamic_cast<TYMaillage*>(eltItem->getElement()._pObj);
                    if (pMail != nullptr)
                    {
                        LPTYCalcul pCalcul = dynamic_cast<TYCalcul*>(
                                eltItem->getElement()->getParent());

                    if (pCalcul._pObj != nullptr)
                    {
                        if (pCalcul->remMaillage(pMail))
                        {
                            pCalcul->updateGraphic();
                            getTYMainWnd()->updateModelers(false, false);
                            updateList();
                            TYElement::setIsSavedOk(true);
                        }
                    }
                    }

                }
                else if (ret == mail_actif)
                {
                    ((TYMaillage*) eltItem->getElement().getRealPointer())->setState(TYMaillage::Actif);
                    getTYMainWnd()->updateModelers(true, true, true);
                }
                else if (ret == mail_inactif)
                {
                    ((TYMaillage*) eltItem->getElement().getRealPointer())->setState(TYMaillage::Inactif);
                    getTYMainWnd()->updateModelers(true, true, true);
                }
                else if (ret == exportCsv)
                {
                    LPTYMaillage pMaillage = TYMaillage::safeDownCast(eltItem->getElement());
                    exportMaillageCSV(pMaillage);
                }
                else if (ret == changeSiteRoot)
                {
                    vector<LPTYElement> tabElem;
                    getTYMainWnd()->open(tabElem);

                    if (tabElem.size() > 0)
                    {
                        LPTYElement pElt = tabElem[0];
                        if ((pElt != NULL) && (pElt->isA("TYSiteNode")))
                        {
                            LPTYSiteNode pSite = TYSiteNode::safeDownCast(pElt);
                            pSite->setRoot(true); // Le nouveau site est racine
                            _pProjet->setSite(pSite);

                            // On met a jour l'arborescence du site frame
                            getTYMainWnd()->getSiteFrame()->setSiteNodeRoot(pSite);
                        }

                        pElt = NULL;
                    }

                    tabElem.clear();
                }
                else if (ret == exportInt)
                {
                    // Sauvegarde securisee
                    getTYMainWnd()->saveAs(eltItem->getElement());
                }
                else if (ret == delCalcul)
                {
                    TYElement* pElem = eltItem->getElement();
                    if (pElem == _pProjet->getCurrentCalcul())
                    {
                        QMessageBox::warning(this, "Tympan", TR("id_warning_def_calc_suppr"));
                    }
                    else if (QMessageBox::warning(this, "Tympan", TR("id_warning_suppr").arg(eltItem->getElement()->getName()), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
                    {
                        if (remFromList(eltItem))
                        {
                            eltItem->remove();
                            delete eltItem;
                            eltItem = NULL;
                            TYElement::setIsSavedOk(true);
                        }
                        else
                        {
                            QMessageBox::warning(this, "Tympan", TR("id_warning_notsuppr").arg(eltItem->getElement()->getName()));
                        }
                    }
                }
                else if (ret == remCalculRes)
                {
                    LPTYProjet pProjet = dynamic_cast<TYProjet*>(eltItem->getElement()._pObj);
                    if (pProjet._pObj != nullptr)
                    {
                        for (unsigned int i = 0; i < pProjet->getListCalcul().size(); i++)
                        {
                            LPTYCalcul pCalcul = pProjet->getListCalcul().at(i);
                            pCalcul->clearResult();
                            pCalcul->updateGraphic();
                        }

                    }
                    else
                    {
                        LPTYCalcul pCalcul = dynamic_cast<TYCalcul*>(eltItem->getElement()._pObj);
                        if (pCalcul._pObj != nullptr)
                        {
                            pCalcul->clearResult();
                            pCalcul->updateGraphic();
                        }
                    }
                    _pProjet->updateGraphicTree();
                    getTYMainWnd()->updateModelers(false, false);
                    updateList();
                }
                else if (ret == curCalcul)
                {
                    TYCalcul* pCalcul = (TYCalcul*) eltItem->getElement().getRealPointer();

                    // Si le calcul courant (avant changement), conserve les contributions partielles, on les efface
                    if (_pCurrentCalcul && (pCalcul != _pCurrentCalcul))
                    {
                        _pCurrentCalcul->setStatusPartialResult(false);
                    }

                    _pProjet->setCurrentCalcul(pCalcul);
                    _pCurrentCalcul = pCalcul;

                    LPTYSiteNode pSite = _pProjet->getSite();
                    if (pSite && pCalcul) {
                        pSite->getInfrastructure()->updateAcoustic(pCalcul);
                    }

                    emit changeCurrentCalcul(pCalcul);

                    _pProjet->updateGraphicTree();
                    getTYMainWnd()->updateModelers(false, false);
                    updateList();

                    setCalculDone(true);
                }
                else if (ret == goCalcul)
                {
                    if ((LPTYElement&) _pCurrentCalcul == eltItem->getElement())
                    {
                        getTYApp()->getCalculManager()->launch((TYCalcul*) eltItem->getElement().getRealPointer());
                    }
                }
                else if (ret == duplicateCalcul)
                {
                    LPTYCalcul pCalcul = (TYCalcul*) eltItem->getElement().getRealPointer();

                    // Duplication et ajout
                    _pProjet->duplicateCalcul(pCalcul);

                    updateList();
                    TYElement::setIsSavedOk(true);
                }
                else if (ret == calc_actif) // DT 16/08/2005 un calcul peut etre actif ou bloque
                {
                    ((TYCalcul*) eltItem->getElement().getRealPointer())->setState(TYCalcul::Actif);
                    getTYMainWnd()->updateModelers(false, false);
                    TYElement::setIsSavedOk(true);
                }
                else if (ret == calc_locked) // DT 16/08/2005 un calcul peut etre actif ou bloque
                {
                    ((TYCalcul*) eltItem->getElement().getRealPointer())->setState(TYCalcul::Locked);
                    getTYMainWnd()->updateModelers(false, false);
                    TYElement::setIsSavedOk(true);
                }
                else if (ret == addCalcul)
                {
                    LPTYCalcul calcul = new TYCalcul();
                    _pProjet->addCalcul(calcul);

                    if (calcul->edit(this) == QDialog::Accepted)
                    {
                        updateList();
                        TYElement::setIsSavedOk(true);
                    }
                }

                delete pMenu;
            }
        }
        else if (eltItem->text(0) == TR("id_list_pointscontrol_item"))
        {
            LPTYProjet pProjet = dynamic_cast<TYProjet*>(
                    ((TYElementListItem*) eltItem->parent())->getElement()._pObj);
            // Le sous menu ne doit apparaitre que si le calcul est actif
            if (pProjet._pObj != nullptr)
            {
                    QMenu* pMenu = new QMenu(this);
                    QAction* addPtControl = pMenu->addAction(TR("id_contextmenu_addptcontrol"));
                    QAction* selectAllPts = pMenu->addAction(TR("id_contextmenu_selectAllPts"));
                    QAction* unselectAllPts = pMenu->addAction(TR("id_contextmenu_unselectAllPts"));

                    // Affiche popup
                    QAction* ret = pMenu->exec(_pListView->mapToGlobal(point));

                    if (ret)
                    {
                        if (ret == addPtControl)
                        {
                            if (getTYApp()->getCalculManager()->askForResetResultat())
                            {
                                LPTYPointControl pPtControl = new TYPointControl();
                                pPtControl->setParent(_pProjet);

                                if (pPtControl->edit(this) == QDialog::Accepted)
                                {
                                    TYProjet* pProjet = (TYProjet*)((TYElementListItem*) eltItem->parent())->getElement().getRealPointer();

                                    if (pProjet->addPointControl(pPtControl))
                                    {
                                        pProjet->updateGraphic();
                                        getTYMainWnd()->updateModelers(false, false);
                                        updateList();
                                        TYElement::setIsSavedOk(true);
                                    }
                                }
                            }
                        }
                        else if (ret == selectAllPts)
                        {
                            selectOrUnselectAll(static_cast<TYElementListItem*>(item), true);
                        }
                        else if (ret == unselectAllPts)
                        {
                            selectOrUnselectAll(static_cast<TYElementListItem*>(item), false);
                        }
                    }
            }
        }
        else if (eltItem->text(0) == "Etats")
        {
            QMenu* pMenu = new QMenu(this);
            QAction* prop = NULL;

            prop = pMenu->addAction(TR("id_contextmenu_properties"));

            // Affiche popup
            QAction* ret = pMenu->exec(_pListView->mapToGlobal(point));
            if (ret)
            {
                if (ret == prop)
                {
                    LPTYCalcul pCalcul = (TYCalcul*)((TYElementListItem*) eltItem->parent())->getElement().getRealPointer();
                    TYEtatsWidget* pEtatWidget = new TYEtatsWidget(pCalcul, this);
                    pEtatWidget->exec();
                    pEtatWidget = NULL;
                }
            }
        }
    }
}

TYElementListItem* TYProjetFrame::addEltToList(LPTYElement pElement, TYElementListItem* parent /*=NULL*/)
{
    TYElementListItem* pItem  = NULL;

    if (!parent)
    {
        pItem = new TYElementListItem(_pListView, pElement);
    }
    else
    {
        pItem = new TYElementListItem(parent, pElement);
    }

    return pItem;
}

bool TYProjetFrame::remFromList(TYElementListItem* item)
{
    bool res = false;

    if (item->getElement()->isA("TYCalcul"))
    {
        res = _pProjet->remCalcul(item->getElement()->getID().toString());
        return res;
    }

    return res;
}

void TYProjetFrame::keyPressEvent(QKeyEvent* e)
{
    TYElementListItem* pItem  = (TYElementListItem*) _pListView->currentItem();

    if (pItem != 0)
    {
        switch (e->key())
        {
            case Key_Delete :
                remFromList(pItem);
                break;
        }
        e->accept();
    }
}

void TYProjetFrame::doubleClic(QTreeWidgetItem* item, int column)
{
    TYElementListItem* eltItem = (TYElementListItem*) item;

    if (eltItem->isElementItem())
    {
        if (dynamic_cast<TYProjet*>(eltItem->getElement()._pObj) != nullptr)
        {
            getTYMainWnd()->makeProjetModeler((TYProjet*) eltItem->getElement().getRealPointer());
        }
        else // Pour tous les autres, affichage des proprietes
        {
            LPTYElement pElt = eltItem->getElement();
            if (pElt->edit(this) == QDialog::Accepted)
            {
                eltItem->updateContent();
                pElt->updateGraphic();
            }
        }
    }
    else if (eltItem->text(0) == "Etats")
    {
        LPTYCalcul pCalcul = (TYCalcul*)((TYElementListItem*) eltItem->parent())->getElement().getRealPointer();
        TYEtatsWidget* pEtatWidget = new TYEtatsWidget(pCalcul, this);
        pEtatWidget->exec();
        pEtatWidget = NULL;
    }

    getTYMainWnd()->updateModelers(true, true, true);
    _pListView->collapseItem(item);
}

void TYProjetFrame::apply(QTreeWidgetItem* item, int col)
{
    if (item)
    {
        TYElementListItem* pCheckItem = (TYElementListItem*) item;
        pCheckItem->setCheckState(0, pCheckItem->checkState(0));
    }
}

void TYProjetFrame::exportMaillageCSV(const LPTYMaillage pMaillage)
{
    assert(pMaillage);

    QString fileName = QFileDialog::getSaveFileName(this, "", "", "CSV (*.csv)");

    if (!fileName.isEmpty())
    {
        if (!fileName.endsWith(".csv"))
        {
            fileName += ".csv";
        }
    }

    QFile f(fileName);

    if (f.open(QIODevice::WriteOnly))
    {
        QTextStream s(&f);

        s << "X" << ";" << "Y" << ";" << "Lp" << "\n";

        TYTabLPPointCalcul tabPoints = pMaillage->getPtsCalcul();
        double value = 0.0;
        LPTYPointCalcul pPtCalcul = NULL;
        TYSpectre spectre;

        for (int i = 0; i < tabPoints.size(); i++)
        {
            pPtCalcul = tabPoints[i];

            // Valeur du scalaire pour la color map
            switch (pMaillage->getDataType())
            {
                case TYMaillage::ValGlobalDBA:
                default:
                    value = pPtCalcul->getValA();
                    break;
                case TYMaillage::ValGlobalDBLin:
                    value = pPtCalcul->getValLin();
                    break;
                case TYMaillage::DataFreq:
                    spectre = *pPtCalcul->getSpectre();
                    value = spectre.getValueReal(pMaillage->getDataFreq());
                    break;
            }

            s << pPtCalcul->getCoordSIG()._x << ";" << pPtCalcul->getCoordSIG()._y << ";" << value << "\n";
        }

        f.close();
    }
}

void TYProjetFrame::selectOrUnselectAll(TYElementListItem* item, const bool& bSelect)
{
    if (getTYApp()->getCalculManager()->askForResetResultat())
    {
        bool on = bSelect;

        for (int i = 0; (i < item->childCount()) && _pCurrentCalcul; i++)
        {
            static_cast<TYElementListItem*>(item->child(i))->setOn(on, false);
        }

        _pProjet->getGraphicObject()->update(true);
        getTYMainWnd()->updateModelers(false, false);
    }
}
