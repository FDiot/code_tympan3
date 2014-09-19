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
 * \file TYSiteFrame.cpp
 * \brief Frame pour la gestion de site
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
#include <QPixmap>
#include <QVBoxLayout>
#include <QBoxLayout>
#include <QHeaderView>
#include <QMdiArea>
#include <QMdiSubWindow>

#include "Tympan/models/business/TYXMLManager.h"
#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/TYPreferenceManager.h"
#include "Tympan/gui/tools/OGLLineElement.h"
#include "Tympan/gui/tools/NxVec3.h"
#include "Tympan/gui/app/TYElementListItem.h"
#include "Tympan/gui/app/TYGetLibraryDialog.h"
#include "Tympan/gui/app/TYRenderWindowInteractor.h"
#include "Tympan/gui/app/TYModelerFrame.h"
#include "Tympan/gui/app/TYRotationDialog.h"
#include "Tympan/gui/app/TYPositionDialog.h"
#include "Tympan/gui/app/TYAddLibraryDialog.h"
#include "Tympan/gui/app/TYLibraryWidget.h"
#include "Tympan/gui/app/TYApplication.h"
#include "Tympan/gui/app/TYMainWindow.h"
#include "TYSiteFrame.h"

#define TR(id) OLocalizator::getString("TYSiteFrame", (id))
#define IMG(id) OLocalizator::getPicture("TYSiteFrame", (id))


TYSiteFrame::TYSiteFrame(QWidget* parent, const char* name, Qt::WFlags f):
    QWidget(parent, f)
{
    setObjectName(name);
    QBoxLayout* pLayout = new QVBoxLayout(this);
    pLayout->setMargin(0);
    setLayout(pLayout);

    // List view
    _pListView = new QTreeWidget(this);
    QStringList stringList;
    stringList.append(TR("id_column_site"));
    _pListView->setColumnCount(1);
    _pListView->setHeaderLabels(stringList);
    _pListView->resizeColumnToContents(0);
    _pListView->setSortingEnabled(false);

    pLayout->addWidget(_pListView);

    _pSiteNodeRoot = NULL;
    _pCurrentCalcul = NULL;

    // Signaux
    QObject::connect(_pListView, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
                     this, SLOT(apply(QTreeWidgetItem*, int)));
    QObject::connect(_pListView, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
                     this, SLOT(openModeler(QTreeWidgetItem*, int)));
}

TYSiteFrame::~TYSiteFrame()
{
    _pListView->disconnect();
}

void TYSiteFrame::setSiteNodeRoot(LPTYSiteNode pSiteNode)
{
    clear();

    if (_pSiteNodeRoot && _pSiteNodeRoot->getGraphicObject())
    {
        _pSiteNodeRoot->getGraphicObject()->disconnectUpdateSignal(this, SLOT(updateList()));
    }

    _pSiteNodeRoot = pSiteNode;

    if (_pSiteNodeRoot)
    {
        // Ce site est root
        _pSiteNodeRoot->setRoot(true);
        updateList();

        if (_pSiteNodeRoot->getGraphicObject())
        {
            _pSiteNodeRoot->getGraphicObject()->connectUpdateSignal(this, SLOT(updateList()));
        }
    }
}

void TYSiteFrame::updateList()
{
    clear();

    if (_pSiteNodeRoot == NULL) { return; }

    // Le site node root est l'item root de la liste
    TYElementListItem* pRootItem = addToList(_pSiteNodeRoot);

    _pListView->setItemExpanded(pRootItem, true);
    _pListView->resizeColumnToContents(0);
}

TYElementListItem* TYSiteFrame::addToList(LPTYSiteNode pElement, TYElementListItem* parent /*=NULL*/)
{
    TYElementListItem* pRootItem  = NULL;
    if (pElement->getRoot())
    {
        pRootItem  = addEltToList(pElement, parent);
    }
    else
    {
        pRootItem  = addEltToList(pElement, parent, true);
    }

    _pListView->setItemExpanded(pRootItem, true);

    // Infrastrucure du site
    addToList(pElement->getInfrastructure(), pRootItem);
    // Topographie du site
    addToList(pElement->getTopographie(), pRootItem);

    // Listes des sites enfants
    TYElementListItem* pListSitesItem = new TYElementListItem(pRootItem, QStringList(TR("id_list_sites_item")));
    for (unsigned int i = 0; i < pElement->getListSiteNode().size(); i++)
    {
        addToList(TYSiteNode::safeDownCast(pElement->getListSiteNode()[i]->getElement()), pListSitesItem);
    }
    _pListView->setItemExpanded(pListSitesItem, true);

    return pRootItem;
}

TYElementListItem* TYSiteFrame::addToList(LPTYTopographie pElement, TYElementListItem* parent /*=NULL*/)
{
    unsigned int i = 0;

    // Topographie
    TYElementListItem* pRootItem  = addEltToList(pElement, parent);
    _pListView->setItemExpanded(pRootItem, true);

    // Liste des plans d'eau
    TYElementListItem* pListPlanEauItem = new TYElementListItem(pRootItem, QStringList(TR("id_list_planeau_item")));
    for (i = 0; i < pElement->getListPlanEau().size(); i++)
    {
        new TYElementListItem(pListPlanEauItem, pElement->getPlanEau(i));
    }

    // Liste des cours d'eau
    TYElementListItem* pListCoursEauItem = new TYElementListItem(pRootItem, QStringList(TR("id_list_crseau_item")));
    for (i = 0; i < pElement->getListCrsEau().size(); i++)
    {
        new TYElementListItem(pListCoursEauItem, pElement->getCrsEau(i));
    }

    // Liste des courbes de niveau
    TYElementListItem* pListCourbeNivItem = new TYElementListItem(pRootItem, QStringList(TR("id_list_courbes_item")));
    for (i = 0; i < pElement->getListCrbNiv().size(); i++)
    {
        new TYElementListItem(pListCourbeNivItem, pElement->getCrbNiv(i));
    }

    // Liste des terrains
    TYElementListItem* pListTerrainItem = new TYElementListItem(pRootItem, QStringList(TR("id_list_terrains_item")));
    TYElementListItem* pTerrainItem;
    for (i = 0; i < pElement->getListTerrain().size(); i++)
    {
        pTerrainItem = new TYElementListItem(pListTerrainItem, pElement->getTerrain(i));
        if (pElement->getTerrain(i) == pElement->getDefTerrain())
        {
            pTerrainItem->setIcon(0, QPixmap(IMG("id_icon_check")));
        }
        else
        {
            pTerrainItem->setIcon(0, QPixmap(IMG("id_icon_no_check")));
        }
    }

    return pRootItem;
}

TYElementListItem* TYSiteFrame::addToList(LPTYInfrastructure pElement, TYElementListItem* parent /*=NULL*/)
{
    unsigned int i = 0;

    // Infrastructure
    TYElementListItem* pRootItem  = addEltToList(pElement, parent);
    _pListView->setItemExpanded(pRootItem, true);

    // Liste des sources
    TYElementListItem* pListSourcesItem = new TYElementListItem(pRootItem, QStringList(TR("id_list_sources_item")));
    for (i = 0; i < pElement->getSrcs().size(); i++)
    {
        LPTYUserSourcePonctuelle pSource = TYUserSourcePonctuelle::safeDownCast(pElement->getSrc(i)->getElement());
        new TYElementListItem(pListSourcesItem, pSource, _pCurrentCalcul, QStringList(), true);
    }

    // Liste des batiments
    TYElementListItem* pListBatimentItem = new TYElementListItem(pRootItem, QStringList(TR("id_list_batiments_item")));
    for (i = 0; i < pElement->getListBatiment().size(); i++)
    {
        LPTYBatiment pBatiment = TYBatiment::safeDownCast(pElement->getBatiment(i)->getElement());
        addToList(pBatiment, pListBatimentItem); // DT_24-09-03
    }

    // Liste des machines
    TYElementListItem* pListMachineItem = new TYElementListItem(pRootItem, QStringList(TR("id_list_machines_item")));
    for (i = 0; i < pElement->getListMachine().size(); i++)
    {
        LPTYMachine pMachine = TYMachine::safeDownCast(pElement->getMachine(i)->getElement());
        new TYElementListItem(pListMachineItem, pMachine, _pCurrentCalcul, QStringList(), true);
    }

    // Liste des reseaux de transport
    TYElementListItem* pListResTranspItem = new TYElementListItem(pRootItem, QStringList(TR("id_list_restransps_item")));
    for (i = 0; i < pElement->getListResTrans().size(); i++)
    {
        new TYElementListItem(pListResTranspItem, pElement->getResTrans(i), _pCurrentCalcul, QStringList(), true);
    }

    // Liste des routes
    TYElementListItem* pListRouteItem = new TYElementListItem(pRootItem, QStringList(TR("id_list_routes_item")));
    for (i = 0; i < pElement->getListRoute().size(); i++)
    {
        new TYElementListItem(pListRouteItem, pElement->getRoute(i), _pCurrentCalcul, QStringList(), true);
    }

    return pRootItem;
}

TYElementListItem* TYSiteFrame::addToList(LPTYMateriauConstruction pElement, TYElementListItem* parent /*=NULL*/)
{
    // Materiau de construction
    TYElementListItem* pRootItem  = addEltToList(pElement, parent);
    new TYElementListItem(pRootItem, &pElement->getSpectreTransm());
    new TYElementListItem(pRootItem, &pElement->getSpectreAbso());
    return pRootItem;
}

TYElementListItem* TYSiteFrame::addToList(LPTYSol pElement, TYElementListItem* parent /*=NULL*/)
{
    // Sol
    TYElementListItem* pRootItem  = addEltToList(pElement, parent);
    if (pElement->getVegetActive())
    {
        new TYElementListItem(pRootItem, pElement->getVegetation());
    }
    return pRootItem;
}

TYElementListItem* TYSiteFrame::addToList(LPTYSpectre pElement, TYElementListItem* parent /*=NULL*/)
{
    // Spectre
    TYElementListItem* pRootItem  = addEltToList(pElement, parent);
    return pRootItem;
}

TYElementListItem* TYSiteFrame::addToList(LPTYSourcePonctuelle pElement, TYElementListItem* parent /*=NULL*/)
{
    // Source ponctuelle
    TYElementListItem* pRootItem  = addEltToList(pElement, parent);
    return pRootItem;
}

TYElementListItem* TYSiteFrame::addToList(LPTYMachine pElement, TYElementListItem* parent /*=NULL*/)
{
    int i = 0;

    // Machine
    TYElementListItem* pRootItem  = addEltToList(pElement, parent);

    TYElementListItem* pListAccVolItem = new TYElementListItem(pRootItem, QStringList(TR("id_list_accvols_item")));
    for (i = 0; i < pElement->getTabAcousticVol().size(); i++)
    {
        new TYElementListItem(pListAccVolItem, pElement->getAcousticVol(i));
    }

    new TYElementListItem(pRootItem, QStringList(TR("id_list_spectres_item")));

    if (pElement->getUseAtt())
    {
        new TYElementListItem(pRootItem, pElement->getAtt());
    }

    return pRootItem;
}

TYElementListItem* TYSiteFrame::addToList(LPTYBatiment pElement, TYElementListItem* parent /*=NULL*/)
{
    unsigned int i, j;

    // batiment
    TYElementListItem* pRootItem = new TYElementListItem(parent, pElement, _pCurrentCalcul, QStringList(), true);
    TYElementListItem* pEtageItem;
    for (i = 0; i < pElement->getTabAcousticVol().size(); i++)
    {
        // Etage
        LPTYEtage pEtage = dynamic_cast<TYEtage*>(pElement->getAcousticVol(i)._pObj);
        if (pEtage != nullptr)
        {
            if (pEtage->getClosed())
            {
                // Ferme = etage
                pEtageItem = new TYElementListItem(pRootItem, pEtage);
            }
            else
            {
                // Non ferme = ecran
                pEtageItem = new TYElementListItem(pRootItem, pEtage, NULL, QStringList(TR("id_ecran_item")));
            }

            // Machines dans l'etage
            for (j = 0; j < pEtage->getTabMachine().size(); j++)
            {
                new TYElementListItem(pEtageItem, pEtage->getMachine(j));
            }

            // Source dans l'etage
            for (j = 0; j < pEtage->getTabSources().size(); j++)
            {
                new TYElementListItem(pEtageItem, pEtage->getSource(j));
            }
        }
        else // Ou Silos
        {
            LPTYAcousticVolume pAccVol = pElement->getAcousticVol(i);
            (void) new TYElementListItem(pRootItem, pAccVol);
        }
    }

    return NULL;
}


void TYSiteFrame::contextMenuEvent(QContextMenuEvent* e)
{
    //si on trouve un item associe
    QPoint point = _pListView->mapFrom(this, e->pos());
    QTreeWidgetItem* item = _pListView->itemAt(point.x(), point.y() - _pListView->header()->height());

    if (item != NULL)
    {
        QMenu* pMenu = new QMenu(this);

        int editState = QDialog::Rejected;

        QAction* prop = NULL; // Boite de dialogue de proprietes
        QAction* showModeler = NULL;
        QAction* calculer = NULL;
        QAction* position = NULL;
        QAction* rotation = NULL;
        QAction* exportXML = NULL;
        QAction* exportCSV = NULL;
        QAction* exportLib = NULL;
        QAction* importLib = NULL;
        QAction* del = NULL;
        QAction* highlight = NULL;
        QAction* selectAll = NULL;
        QAction* unselectAll = NULL;
        QAction* addMachineXML = NULL;
        QAction* newMachine = NULL;
        QAction* addSrcXML = NULL;
        QAction* newSrc = NULL;
        QAction* addBatimentXML = NULL;
        QAction* newBatiment = NULL;
        QAction* addSiteXML = NULL;
        QAction* newSite = NULL;
        QAction* resetDMax = NULL;

        QAction* ret = NULL; // Action selectionnee par l'utilisateur

        QMenu* pSubMenuAdd = new QMenu(this);
        pSubMenuAdd->setIcon(QIcon(QPixmap(IMG("id_icon_open"))));
        pSubMenuAdd->setTitle(TR("id_contextmenu_submenu_add"));

        QMenu* pSubMenuExport = new QMenu(this);
        pSubMenuExport->setIcon(QIcon(QPixmap(IMG("id_icon_save"))));
        pSubMenuExport->setTitle(TR("id_contextmenu_submenu_export"));
        exportLib = pSubMenuExport->addAction(QIcon(QPixmap(IMG("id_icon_export"))), TR("id_contextmenu_export_lib"));
        pSubMenuExport->addSeparator();
        exportXML = pSubMenuExport->addAction(QIcon(QPixmap(IMG("id_icon_export"))), TR("id_contextmenu_export_xml"));
        pSubMenuExport->addSeparator();
        exportCSV = pSubMenuExport->addAction(QIcon(QPixmap(IMG("id_icon_export"))), TR("id_contextmenu_export_csv"));

        TYElement* pElement = NULL;
        TYElementGraphic* pGraphicObject = NULL;

        TYElementListItem* eltItem = (TYElementListItem*) item;

        // Composition du menu
        if (eltItem->isElementItem()) // L'item selectionne est un element du site
        {
            pElement = eltItem->getElement().getRealPointer();
            pGraphicObject = pElement->getGraphicObject().getRealPointer();

            // Composition du menu
            prop = pMenu->addAction(QIcon(QPixmap(IMG("id_icon_editeelt"))), TR("id_contextmenu_properties"));
            pMenu->addSeparator();

            TYSiteNode* pSiteNode = dynamic_cast<TYSiteNode*>(pElement);
            if (pSiteNode != nullptr)
            {
                showModeler = pMenu->addAction(QIcon(QPixmap(IMG("id_icon_modeler"))), TR("id_contextmenu_modeler"));

                bool bNotSiteRoot = (pSiteNode && !pSiteNode->getRoot());

                if (bNotSiteRoot) // Actions possibles si ce n'est pas le site racine
                {
                    pMenu->addSeparator();
                    if (pGraphicObject)
                    {
                        highlight = pMenu->addAction(TR("id_contextmenu_highlight"));
                        highlight->setCheckable(true);
                        highlight->setChecked(pGraphicObject->getHighlightState());
                        pMenu->addSeparator();
                    }
                    position = pMenu->addAction(TR("id_context_menu_position"));
                    rotation = pMenu->addAction(TR("id_context_menu_rotation"));
                    pMenu->addSeparator();
                }

                // Definition du sous menu d'ajout d'elements
                addSiteXML = pSubMenuAdd->addAction(TR("id_contextmenu_site_xml"));
                newSite = pSubMenuAdd->addAction(TR("id_contextmenu_newsite"));
                pSubMenuAdd->addSeparator();
                addBatimentXML = pSubMenuAdd->addAction(TR("id_contextmenu_addbatiment_xml"));
                newBatiment = pSubMenuAdd->addAction(TR("id_contextmenu_newbatiment"));
                pSubMenuAdd->addSeparator();
                addMachineXML = pSubMenuAdd->addAction(TR("id_contextmenu_addmachine_xml"));
                newMachine = pSubMenuAdd->addAction(TR("id_contextmenu_newmachine"));
                pSubMenuAdd->addSeparator();
                addSrcXML = pSubMenuAdd->addAction(TR("id_contextmenu_addsource_xml"));
                newSrc = pSubMenuAdd->addAction(TR("id_contextmenu_newsource"));
                pSubMenuAdd->addSeparator();
                importLib = pSubMenuAdd->addAction(QIcon(QPixmap(IMG("id_icon_import_lib"))), TR("id_contextmenu_import_lib"));

                pMenu->addMenu(pSubMenuAdd);

                // Ajout du sous-menu d'export
                pMenu->addSeparator();
                pMenu->addMenu(pSubMenuExport);

                // Retour au menu principal
                if (bNotSiteRoot) // Action possible si ce n'est pas le site racine
                {
                    pMenu->addSeparator();
                    del = pMenu->addAction(QIcon(QPixmap(IMG("id_icon_del"))), TR("id_contextmenu_delete"));
                }
            }
            else if (dynamic_cast<TYInfrastructure*>(pElement) != nullptr)
            {
                // Definition du sous menu d'ajout d'elements
                addBatimentXML = pSubMenuAdd->addAction(TR("id_contextmenu_addbatiment_xml"));
                newBatiment = pSubMenuAdd->addAction(TR("id_contextmenu_newbatiment"));
                pSubMenuAdd->addSeparator();
                addMachineXML = pSubMenuAdd->addAction(TR("id_contextmenu_addmachine_xml"));
                newMachine = pSubMenuAdd->addAction(TR("id_contextmenu_newmachine"));
                pSubMenuAdd->addSeparator();

                addSrcXML = pSubMenuAdd->addAction(TR("id_contextmenu_addsource_xml"));
                newSrc = pSubMenuAdd->addAction(TR("id_contextmenu_newsource"));
                pSubMenuAdd->addSeparator();

                importLib = pSubMenuAdd->addAction(QIcon(QPixmap(IMG("id_icon_import_lib"))), TR("id_contextmenu_import_lib"));

                pMenu->addMenu(pSubMenuAdd);
            }
            else if (dynamic_cast<TYAcousticVolumeNode*>(pElement) != nullptr)
            {
                showModeler = pMenu->addAction(QIcon(QPixmap(IMG("id_icon_modeler"))), TR("id_contextmenu_modeler"));
                pMenu->addSeparator();
                if (pGraphicObject)
                {
                    highlight = pMenu->addAction(TR("id_contextmenu_highlight"));
                    highlight->setCheckable(true);
                    highlight->setChecked(pGraphicObject->getHighlightState());
                    pMenu->addSeparator();
                }
                calculer = pMenu->addAction(QIcon(QPixmap(IMG("id_icon_calcul"))), TR("id_contextmenu_calculer"));
                pMenu->addSeparator();
                position = pMenu->addAction(TR("id_context_menu_position"));
                rotation = pMenu->addAction(TR("id_context_menu_rotation"));
                pMenu->addSeparator();
                pMenu->addMenu(pSubMenuExport);
                pMenu->addSeparator();
                del = pMenu->addAction(QIcon(QPixmap(IMG("id_icon_del"))), TR("id_contextmenu_delete"));
            }
            else if (dynamic_cast<TYAcousticVolume*>(pElement) != nullptr)
               // Cas de l'etage
            {
                // Definition du sous menu d'ajout d'elements
                addMachineXML = pSubMenuAdd->addAction(TR("id_contextmenu_addmachine_xml"));
                newMachine = pSubMenuAdd->addAction(TR("id_contextmenu_newmachine"));
                pSubMenuAdd->addSeparator();
                addSrcXML = pSubMenuAdd->addAction(TR("id_contextmenu_addsource_xml"));
                newSrc = pSubMenuAdd->addAction(TR("id_contextmenu_newsource"));
                pSubMenuAdd->addSeparator();

                importLib = pSubMenuAdd->addAction(QIcon(QPixmap(IMG("id_icon_import_lib"))), TR("id_contextmenu_import_lib"));

                pMenu->addMenu(pSubMenuAdd);
            }
            else if (dynamic_cast<TYAcousticLine*>(pElement) != nullptr)
            {
                if (pGraphicObject)
                {
                    highlight = pMenu->addAction(TR("id_contextmenu_highlight"));
                    highlight->setCheckable(true);
                    highlight->setChecked(pGraphicObject->getHighlightState());
                    pMenu->addSeparator();
                }
            }
            else if (dynamic_cast<TYSourcePonctuelle*>(pElement) != nullptr)
            {
                if (pGraphicObject)
                {
                    highlight = pMenu->addAction(TR("id_contextmenu_highlight"));
                    highlight->setCheckable(true);
                    highlight->setChecked(pGraphicObject->getHighlightState());
                    pMenu->addSeparator();
                }

                pMenu->addMenu(pSubMenuExport);

                pMenu->addSeparator();
                del = pMenu->addAction(QIcon(QPixmap(IMG("id_icon_del"))), TR("id_contextmenu_delete"));
            }
            else if (dynamic_cast<TYTopographie*>(pElement->getParent()) != nullptr)
                // Pour les objets de topo en general
            {
                del = pMenu->addAction(QIcon(QPixmap(IMG("id_icon_del"))), TR("id_contextmenu_delete"));
            }
        }
        else // L'item selectionne est un element de decors (liste de ...)
        {
            TYElementListItem* pItemParent = (TYElementListItem*) item->parent();
            pElement = pItemParent->getElement();
            assert(pElement);

            if ((item->text(0) == TR("id_list_sources_item"))   ||
                (item->text(0) == TR("id_list_batiments_item")) ||
                (item->text(0) == TR("id_list_machines_item"))  ||
                (item->text(0) == TR("id_list_sites_item")))
            {
                selectAll = pMenu->addAction(TR("id_contextmenu_selectall"));
                unselectAll = pMenu->addAction(TR("id_contextmenu_unselectall"));
                pMenu->addSeparator();
            }

            if ((item->text(0) == TR("id_list_sites_item")))
            {
                // Definition du sous menu d'ajout d'elements
                addSiteXML = pSubMenuAdd->addAction(TR("id_contextmenu_site_xml"));
                pSubMenuAdd->addSeparator();
                newSite = pSubMenuAdd->addAction(TR("id_contextmenu_newsite"));
                pSubMenuAdd->addSeparator();
                importLib = pSubMenuAdd->addAction(QIcon(QPixmap(IMG("id_icon_import_lib"))), TR("id_contextmenu_import_lib"));

                pMenu->addMenu(pSubMenuAdd);
                pMenu->addSeparator();
            }
            else if ((item->text(0) == TR("id_list_batiments_item")))
            {
                // Definition du sous menu d'ajout d'elements
                addBatimentXML = pSubMenuAdd->addAction(TR("id_contextmenu_addbatiment_xml"));
                pSubMenuAdd->addSeparator();
                newBatiment = pSubMenuAdd->addAction(TR("id_contextmenu_newbatiment"));
                pSubMenuAdd->addSeparator();
                importLib = pSubMenuAdd->addAction(QIcon(QPixmap(IMG("id_icon_import_lib"))), TR("id_contextmenu_import_lib"));

                pMenu->addMenu(pSubMenuAdd);
                pMenu->addSeparator();
            }
            else if ((item->text(0) == TR("id_list_machines_item")))
            {
                // Definition du sous menu d'ajout d'elements
                addMachineXML = pSubMenuAdd->addAction(TR("id_contextmenu_addmachine_xml"));
                pSubMenuAdd->addSeparator();
                newMachine = pSubMenuAdd->addAction(TR("id_contextmenu_newmachine"));
                pSubMenuAdd->addSeparator();
                importLib = pSubMenuAdd->addAction(QIcon(QPixmap(IMG("id_icon_import_lib"))), TR("id_contextmenu_import_lib"));

                pMenu->addMenu(pSubMenuAdd);
                pMenu->addSeparator();
            }
            else if ((item->text(0) == TR("id_list_sources_item")))
            {
                // Definition du sous menu d'ajout d'elements
                addSrcXML = pSubMenuAdd->addAction(TR("id_contextmenu_addsource_xml"));
                pSubMenuAdd->addSeparator();
                newSrc = pSubMenuAdd->addAction(TR("id_contextmenu_newsource"));
                pSubMenuAdd->addSeparator();
                importLib = pSubMenuAdd->addAction(QIcon(QPixmap(IMG("id_icon_import_lib"))), TR("id_contextmenu_import_lib"));

                pMenu->addMenu(pSubMenuAdd);
                pMenu->addSeparator();
            }
            else if ((item->text(0) == TR("id_list_courbes_item")) ||
                     (item->text(0) == TR("id_list_planeau_item")))
            {
                if (item->childCount() > 0)
                {
                    resetDMax = pMenu->addAction(TR("id_contextmenu_DMax")); // Reinitialisation de la distance max
                }
            }
        }

        ret = pMenu->exec(_pListView->mapToGlobal(point));

        // Traitement de la selection
        if (ret)
        {
            if (ret == prop)
            {
                editState = pElement->edit(this);

                if (editState == QDialog::Accepted)
                {
                    eltItem->updateContent();

                    TYProjet* pProjet = getTYApp()->getCurProjet();
                    if (pProjet)
                    {
                        TYSiteNode* pSite = pProjet->getSite();
                        if (pSite)
                        {
                            pSite->update(pElement);
                        }

                        pProjet->updateGraphicTree();
                    }
                }

                pElement->updateGraphicTree();
            }
            else if (pElement && ret == showModeler)
            {
                if (dynamic_cast<TYSiteNode*>(pElement) != nullptr)
                {
                    getTYMainWnd()->makeSiteModeler((TYSiteNode*) pElement);
                }
                else if (dynamic_cast<TYBatiment*>(eltItem->getElement()._pObj) != nullptr)
                {
                    getTYMainWnd()->makeBatimentModeler((TYBatiment*) pElement);
                }
                else if (dynamic_cast<TYMachine*>(eltItem->getElement()._pObj) != nullptr)
                {
                    getTYMainWnd()->makeMachineModeler((TYMachine*) pElement);
                }
            }
            else if (ret == highlight)
            {
                localise(pElement, pGraphicObject);
            }
            else if (ret == calculer)
            {
                getTYApp()->getCalculManager()->updateAcoustic(pElement);
            }
            else if (ret == position)
            {
                changePos(pElement);
            }
            else if (ret == rotation)
            {
                changeRotation(pElement);
            }
            else if (ret == exportCSV)
            {
                exportEXCEL(pElement);
            }
            else if (ret == exportXML)
            {
                getTYMainWnd()->saveAs(pElement);
            }
            else if (ret == exportLib)
            {
                TYAddLibraryDialog* pDlg = new TYAddLibraryDialog(this);
                pDlg->setElement(pElement);
                pDlg->exec();
            }
            else if (ret == importLib)
            {
                unsigned int filter = 0;
                if (dynamic_cast<TYSiteNode*>(pElement) != nullptr)
                {
                    filter = eSiteFilter;
                }
                else if (dynamic_cast<TYInfrastructure*>(pElement) != nullptr)
                {
                    filter = eInfraFilter;
                }
                else if (dynamic_cast<TYBatiment*>(pElement) != nullptr)
                {
                    filter = eConstructionFilter;
                }
                else if (dynamic_cast<TYEtage*>(pElement) != nullptr)
                {
                    filter = eEtageFilter;
                }
                else if (dynamic_cast<TYMachine*>(pElement) != nullptr)
                {
                    filter = eMachineFilter;
                }
                else if (dynamic_cast<TYSource*>(pElement) != nullptr)
                {
                    filter = eSourceFilter;
                }

                importFromLib(filter, pElement);
            }
            else if (ret == addSiteXML)
            {
                addEltXML("TYSiteNode", pElement);
            }
            else if (ret == newSite)
            {
                newElt("TYSiteNode", pElement);
            }
            else if (ret == addBatimentXML)
            {
                addEltXML("TYBatiment", pElement);
            }
            else if (ret == newBatiment)
            {
                newElt("TYBatiment", pElement);
            }
            else if (ret == addMachineXML)
            {
                addEltXML("TYMachine", pElement);
            }
            else if (ret == newMachine)
            {
                newElt("TYMachine", pElement);
            }
            else if (ret == addSrcXML)
            {
                addEltXML("TYUserSourcePonctuelle", pElement);
            }
            else if (ret == newSrc)
            {
                newElt("TYUserSourcePonctuelle", pElement);
            }
            else if (ret == del)
            {
                if (eltItem && !supprime(eltItem))
                {
                    QMessageBox::warning(this, "Tympan", TR("id_warning_notsuppr"));
                }
            }
            else if (ret == selectAll)
            {
                selectOrUnselectAll(static_cast<TYElementListItem*>(item), true);
            }
            else if (ret == unselectAll)
            {
                selectOrUnselectAll(static_cast<TYElementListItem*>(item), false);
            }
            else if (ret == resetDMax)
            {
                setDMaxDefault(static_cast<TYElementListItem*>(item));
            }

            _pSiteNodeRoot->getGraphicObject()->updateTree();
            getTYMainWnd()->updateModelers(true, true, true);
        }
    }
}

TYElementListItem* TYSiteFrame::addEltToList(LPTYElement pElement, TYElementListItem* parent, const bool& checkable)
{
    TYElementListItem* pItem  = NULL;

    if (parent == NULL)
    {
        pItem = new TYElementListItem(_pListView, pElement, _pCurrentCalcul, QStringList(), checkable);
    }
    else
    {
        pItem = new TYElementListItem(parent, pElement, _pCurrentCalcul, QStringList(), checkable);
    }

    return pItem;
}


bool TYSiteFrame::remInInfra(LPTYSiteNode _siteNode, TYElementListItem* item, const char* className)
{
    bool res = false;

    TYInfrastructure* infra = (TYInfrastructure*)(item->getElement()->getParent());

    for (unsigned int i = 0; i < _siteNode->getListSiteNode().size(); i++)
    {
        LPTYSiteNode pSiteNode = TYSiteNode::safeDownCast(_siteNode->getListSiteNode()[i]->getElement());
        TYInfrastructure* pInfraChild = pSiteNode->getInfrastructure();

        if (pInfraChild == infra)
        {
            if (strcmp(className, "TYMachine") == 0)
            {
                res = pInfraChild->remMachine((TYMachine*) item->getElement().getRealPointer());
            }
            else if (strcmp(className, "TYBatiment") == 0)
            {
                res = pInfraChild->remBatiment((TYBatiment*) item->getElement().getRealPointer());
            }
            else if (strcmp(className, "TYRoute") == 0)
            {
                res = pInfraChild->remRoute((TYRoute*) item->getElement().getRealPointer());
            }
            else if (strcmp(className, "TYReseauTransport") == 0)
            {
                res = pInfraChild->remResTrans((TYReseauTransport*) item->getElement().getRealPointer());
            }
            else if (strcmp(className, "TYUserSourcePonctuelle") == 0)
            {
                res = pInfraChild->remSrc((TYUserSourcePonctuelle*) item->getElement().getRealPointer());
            }

            return res;
        }

        res = remInInfra(pSiteNode, item, className);
        if (res) { return true; }
    }

    return res;
}

bool TYSiteFrame::remInTopo(LPTYSiteNode _siteNode, TYElementListItem* item, const char* className)
{
    bool res = false;

    TYTopographie* topo = (TYTopographie*)(item->getElement()->getParent());

    for (unsigned int i = 0; i < _siteNode->getListSiteNode().size(); i++)
    {
        LPTYSiteNode pSiteNode = TYSiteNode::safeDownCast(_siteNode->getListSiteNode()[i]->getElement());
        TYTopographie* pTopoChild = pSiteNode->getTopographie();

        if (pTopoChild == topo)
        {
            if (strcmp(className, "TYCourbeNiveau") == 0)
            {
                res = pTopoChild->remCrbNiv((TYCourbeNiveau*) item->getElement().getRealPointer());
            }
            else if (strcmp(className, "TYCoursEau") == 0)
            {
                res = pTopoChild->remCrsEau((TYCoursEau*) item->getElement().getRealPointer());
            }
            else if (strcmp(className, "TYPlanEau") == 0)
            {
                res = pTopoChild->remPlanEau((TYPlanEau*) item->getElement().getRealPointer());
            }
            else if (strcmp(className, "TYTerrain") == 0)
            {
                res = pTopoChild->remTerrain((TYTerrain*) item->getElement().getRealPointer());
            }

            return res;
        }

        res = remInTopo(pSiteNode, item, className);
        if (res) { return true; }
    }

    return res;
}

bool TYSiteFrame::remFromList(TYElementListItem* item)
{
    TYElement* pElement = item->getElement();
    if (!pElement) { return false; }

    return remFromSite(pElement);
}

bool TYSiteFrame::remFromSite(TYElement* pElement)
{
    if (!pElement) { return false; }

    bool res = false;

    const char* className = pElement->getClassName();
    TYElement* pParent = pElement->getParent();

    if (pParent)
    {
        // Supression d'un site imbrique
        if (strcmp(pParent->getClassName(), "TYSiteNode") == 0)
        {
            TYSiteNode* pSite = TYSiteNode::safeDownCast(pParent);
            if (pSite) { res = pSite->remSiteNode((LPTYSiteNode&) pElement); }
        }
        // Suppression d'un element d'infrastructure
        else if (strcmp(pParent->getClassName(), "TYInfrastructure") == 0)
        {
            TYInfrastructure* pInfra = TYInfrastructure::safeDownCast(pParent);

            if (pInfra)
            {
                if (strcmp(className, "TYMachine") == 0) { res = pInfra->remMachine((LPTYMachine&) pElement); }
                else if (strcmp(className, "TYBatiment") == 0) { res = pInfra->remBatiment((LPTYBatiment&) pElement); }
                else if (strcmp(className, "TYRoute") == 0) { res = pInfra->remRoute((LPTYRoute&) pElement); }
                else if (strcmp(className, "TYReseauTransport") == 0) { res = pInfra->remResTrans((LPTYReseauTransport&) pElement); }
                else if (strcmp(className, "TYUserSourcePonctuelle") == 0) { res = pInfra->remSrc((LPTYUserSourcePonctuelle&) pElement); }
            }
            else
            {
                res = false;
            }
        }
        // Suppression d'un element de topographie
        else if (strcmp(pParent->getClassName(), "TYTopographie") == 0)
        {
            TYTopographie* pTopo = TYTopographie::safeDownCast(pParent);

            if (pTopo)
            {
                if (strcmp(className, "TYCourbeNiveau") == 0) { res = pTopo->remCrbNiv((LPTYCourbeNiveau&) pElement); }
                else if (strcmp(className, "TYCoursEau") == 0) { res = pTopo->remCrsEau((LPTYCoursEau&) pElement); }
                else if (strcmp(className, "TYPlanEau") == 0) { res = pTopo->remPlanEau((LPTYPlanEau&) pElement); }
                else if (strcmp(className, "TYTerrain") == 0) { res = pTopo->remTerrain((LPTYTerrain&) pElement); }
            }
            else
            {
                res = false;
            }
        }
        // Suppression d'un etage de batiment
        else if (strcmp(pParent->getClassName(), "TYBatiment") == 0)
        {
            TYBatiment* pBatiment = TYBatiment::safeDownCast(pParent);
            if (pBatiment)
            {
                res = pBatiment->remAcousticVol((LPTYAcousticVolume&) pElement);
            }
            else
            {
                res = false;
            }
        }
        else if (strcmp(pParent->getClassName(), "TYEtage") == 0)
        {
            TYEtage* pEtage = dynamic_cast<TYEtage*>(pParent);

            if (pEtage != nullptr)
            {
                if (dynamic_cast<TYMachine*>(pElement) != nullptr)
                {
                    res = pEtage->remMachine((LPTYMachine&)pElement);
                }
                else if (dynamic_cast<TYSource*>(pElement) != nullptr)
                {
                    res = pEtage->remSource((LPTYUserSourcePonctuelle&) pElement);
                }
            }
            else
            {
                res = false;
            }
        }
    }

    return res;
}

void TYSiteFrame::apply(QTreeWidgetItem* item, int col)
{
    if (item)
    {
        TYElementListItem* pCheckItem = (TYElementListItem*) item;
        pCheckItem->setCheckState(0, pCheckItem->checkState(0));
    }
}

void TYSiteFrame::openModeler(QTreeWidgetItem* item, int column)
{
    TYElementListItem* eltItem = (TYElementListItem*) item;
    if ((eltItem) && (eltItem->isElementItem()))
    {
        if (dynamic_cast<TYSiteNode*>(eltItem->getElement()._pObj) != nullptr)
        {
            getTYMainWnd()->makeSiteModeler((TYSiteNode*) eltItem->getElement().getRealPointer());
        }
        else if (dynamic_cast<TYBatiment*>(eltItem->getElement()._pObj) != nullptr)
        {
            getTYMainWnd()->makeBatimentModeler((TYBatiment*) eltItem->getElement().getRealPointer());
        }
        else if (dynamic_cast<TYMachine*>(eltItem->getElement()._pObj) != nullptr)
        {
            getTYMainWnd()->makeMachineModeler((TYMachine*) eltItem->getElement().getRealPointer());
        }
        else // Pour tous les autres, affichage des proprietes
        {
            LPTYElement pElt = eltItem->getElement();
            pElt->edit(this);
            eltItem->updateContent();
        }
    }

    _pListView->collapseItem(item);
}

void TYSiteFrame::exportEXCEL(TYElement* pElement)
{
    assert(pElement);

    QString fileName = QFileDialog::getSaveFileName(this, "", getTYApp()->getCurrentDirName(), "CSV (*.csv)");

    if (!fileName.isEmpty())
    {
        if (!fileName.endsWith(".csv"))
        {
            fileName += ".csv";
        }

        std::ofstream ofs;
        ofs.open(fileName.toAscii().constData(), std::ios_base::out);

        if (ofs.is_open())
        {
            ofs.setf(std::ios::fixed, std::ios::floatfield);
            ofs.precision(2);

            // Sauvegarde
            // Adaptation au type
            LPTYAcousticVolumeNode pVolNode = dynamic_cast<TYAcousticVolumeNode*>(pElement);
            if (pVolNode != nullptr)
            {
                pVolNode->exportCSV(ofs);
            }
            else
            {
                TYAcousticLine* pAcLine = dynamic_cast<TYAcousticLine*>(pElement);
                if (pAcLine != nullptr)
                {
                    pAcLine->exportCSV(ofs);
                }
                else
                {
                    TYUserSourcePonctuelle* pSource = dynamic_cast<TYUserSourcePonctuelle*>(pElement);
                    if (pSource != nullptr)
                    {
                        pSource->exportCSV(ofs);
                    }
                    else
                    {
                        TYSiteNode* pSite = dynamic_cast<TYSiteNode*>(pElement);
                        if (pSite != nullptr)
                        {
                            pSite->exportCSV(ofs);
                        }
                    }
                }
            }
        }

        ofs.close();
    }
}

void TYSiteFrame::changePos(TYElement* pElement)
{
    assert(pElement);

    TYGeometryNode* pGeoNode = TYGeometryNode::GetGeoNode(pElement);
    if (pGeoNode)
    {
        TYGeometryNode* pTempNode = new TYGeometryNode();
        pTempNode->deepCopy(pGeoNode, true);

        TYPositionDialog* pDlg = new TYPositionDialog(pGeoNode, this);
        if (pElement->isA("TYSiteNode")) { pDlg->setHauteurEnabled(false); }

        int ret = pDlg->exec();

        // Applique les modificatins si necessaire
        if ((ret == QDialog::Accepted) &&
            !(getTYApp()->getCalculManager()->askForResetResultat()))
        {
            pGeoNode->deepCopy(pTempNode, true);
        }

        pDlg = NULL;
        delete pTempNode;
        pTempNode = NULL;
    }

    // Si la machine est dans un batiment, il faut indiquer au batiment qu'il n'est plus a jour
    TYElement* pParent = pElement->getParent();
    if (pParent && pParent->isA("TYEtage"))
    {
        pParent = pParent->getParent();
        if (pParent)
        {
            pParent->setIsAcousticModified(true);
            pParent->setIsGeometryModified(true);
        }
    }
}

void TYSiteFrame::changeRotation(TYElement* pElement)
{
    assert(pElement);
    TYGeometryNode* pGeoNode = TYGeometryNode::GetGeoNode(pElement);
    if (pGeoNode)
    {
        OMatrix oldTyMat = pGeoNode->getMatrix();

        // Get rotations from transform matrix
        OPoint3D vec;
        vec._x = oldTyMat._m[0][1];
        vec._y = oldTyMat._m[1][1];
        vec._z = oldTyMat._m[2][1];

        // Get X-vector for roll calculation
        OPoint3D xv;
        xv._x = oldTyMat._m[0][0];
        xv._y = oldTyMat._m[1][0];
        xv._z = oldTyMat._m[2][0];

        // Calculate PRH (x = pitch, y = roll, z = heading)
        OPoint3D rotTmp(-atan2(vec._z, sqrt(vec._x * vec._x + vec._y * vec._y)), xv._z, -atan2(-vec._x, vec._y));

        // Set up vars
        double pitch = RADTODEG(rotTmp._x);     // Pitch
        double yaw = -RADTODEG(rotTmp._z);       // Heading
        double roll = RADTODEG(rotTmp._y);      // Roll

        OPoint3D rot(pitch, roll, yaw);

        TYRotationDialog* pDlg = new TYRotationDialog(&rot, this);
        int ret = pDlg->exec();

        // Applique les modificatins si necessaire
        if (ret == QDialog::Accepted)
        {
            if (getTYApp()->getCalculManager()->askForResetResultat())
            {
                ORepere3D Repere = pGeoNode->getORepere3D();
                ORepere3D oldRepere = Repere;
                OMatrix tyMat;
                OMatrix tyMatTmpX;
                OMatrix tyMatTmpY;
                OMatrix tyMatTmpZ;
                OMatrix tyMatTmpConcat;

                // On applique la rotation
                double dRotateX = rot._x;
                double dRotateY = rot._y;
                double dRotateZ = rot._z;
                if (pDlg->getConcatenateStatus()) //(!pConcatenateCheckBox->isChecked()) //az++
                {
                    dRotateX -= pitch;
                    dRotateY -= roll;
                    dRotateZ -= yaw;
                }

                tyMatTmpX.setRotationOx(-M_PI * dRotateX / 180);
                tyMatTmpY.setRotationOy(-M_PI * dRotateY / 180);
                tyMatTmpZ.setRotationOz(M_PI * dRotateZ / 180);

                if (pDlg->getConcatenateStatus()) //az--
                {
                    tyMat = Repere.asMatrix();
                    tyMatTmpConcat = tyMat;
                }

                tyMat = tyMat * tyMatTmpZ * tyMatTmpY * tyMatTmpX * tyMatTmpConcat;

                OPoint3D org = Repere._origin; // On conserve l'origine de depart
                Repere.set(tyMat);
                Repere._origin = org;

                pGeoNode->setRepere(Repere);
            }
        }

        pDlg = NULL;
    }
}
void TYSiteFrame::localise(TYElement* pElement, TYElementGraphic* pGraphicObject)
{
    if (pGraphicObject && pElement)
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
            QList<QMdiSubWindow*> windows = getTYMainWnd()->getWorkspace()->subWindowList();

            for (int i = 0; i < int(windows.count()); ++i)
            {
                QWidget* internal_window = windows.at(i)->widget();

                QString qClassName = internal_window->metaObject()->className();
                if (qClassName == QString("TYSiteModelerFrame"))
                {
                    TYRenderWindowInteractor* pView = ((TYModelerFrame*) internal_window)->getView();
                    pView->getRenderer()->addSelectedElement(pElement);
                }
            }
        }

        if (bCenterOnLocate && pGraphicObject->getHighlightState())
        {
            pGraphicObject->computeBoundingBox();
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

                QList<QMdiSubWindow*> windows = getTYMainWnd()->getWorkspace()->subWindowList();

                for (int i = 0; i < int(windows.count()); ++i)
                {
                    QWidget* internal_window = windows.at(i)->widget();

                    if (dynamic_cast<TYModelerFrame*>(internal_window) != nullptr)
                    {
                        TYRenderWindowInteractor* pView = ((TYModelerFrame*) internal_window)->getView();
                        pView->getRenderer()->getActiveCamera()->setTo(center.get());

                        TYElement* pRootTYElement = ((TYModelerFrame*) internal_window)->getElement();
                        TYGeometryNode* pRootGeometryNode = TYGeometryNode::GetGeoNode(pRootTYElement);
                        pView->getRenderer()->updateDisplayListOverlay(pGeoNode, pRootGeometryNode);
                    }
                }
            }
        }

        getTYMainWnd()->updateModelers();//az++ pour localiser

        //il faut un update avant d'enlever l'etat de "locate" sur l'objet
        if (!pGraphicObject->getHighlightState())
        {
            QList<QMdiSubWindow*> windows = getTYMainWnd()->getWorkspace()->subWindowList();

            for (int i = 0; i < int(windows.count()); ++i)
            {
                QWidget* internal_window = windows.at(i)->widget();

                QString qClassName = internal_window->metaObject()->className();
                if (qClassName == QString("TYSiteModelerFrame"))
                {
                    getTYMainWnd()->updateModelers();//az++ pour localiser
                    TYRenderWindowInteractor* pView = ((TYModelerFrame*) internal_window)->getView();
                    pView->getRenderer()->removeSelectedElement(pElement);
                }
            }
        }
    }
}

bool TYSiteFrame::supprime(TYElementListItem* eltItem)
{
    bool ret = false;

    if (getTYApp()->getCalculManager()->askForResetResultat())
    {
        LPTYElement pElt = eltItem->getElement()->getParent();

        if (remFromList(eltItem))
        {
            // Fermeture des modeleurs ouverts
            getTYMainWnd()->closeModeler(eltItem->getElement());

            // Mise a jour graphique du parent
            pElt->updateGraphicTree();
            TYElement::setIsSavedOk(true);

            // Suppression de l'entree dans l'arborescence
            eltItem->remove();
            delete eltItem;
            eltItem = NULL;

            // Mise a jour de l'arborescence
            updateList();

            ret = true;
        }
        else
        {
            ret = false;
        }
    }
    return ret;
}
void TYSiteFrame::importFromLib(const unsigned int& filter, TYElement* pElement)
{
    assert(pElement);
    TYGetLibraryDialog* pDlg = new TYGetLibraryDialog(filter, this);
    pDlg->exec();
    LPTYElementArray elements = pDlg->getElements();

    if (getTYApp()->getCalculManager()->askForResetResultat())
    {
        LPTYElement pElt = NULL;
        for (unsigned int i = 0; i < elements.size(); ++i)
        {
            pElt = elements[i];
            addElt(pElement, pElt);
        }
    }
}

void TYSiteFrame::addEltXML(const char* className, TYElement* pElement)
{
    assert(pElement);

    if (getTYApp()->getCalculManager()->askForResetResultat())
    {
        vector<LPTYElement> tabElem;
        getTYMainWnd()->open(tabElem, true);// Avec regeneration de l'ID de l'element

        if (tabElem.size() > 0)
        {
            TYElement* pElt = NULL;
            for (unsigned int i = 0; i < tabElem.size(); i++)
            {
                pElt = tabElem[i];
                if ((pElt != NULL) && (pElt->isA(className)))
                {
                    addElt(pElement, pElt);
                }
            }
        }

        tabElem.clear();
    }

}

void TYSiteFrame::addElt(TYElement* pElement, TYElement* pElt)
{
    assert(pElement);
    assert(pElt);

    TYSiteNode* pSiteNode = NULL;
    TYInfrastructure* pInfra = NULL;
    bool reallyAdd = true;  // Eviter qu'une machine dans un batiment soit connue du calcul (fenetre "etats")

    TYSiteNode* pSite = dynamic_cast<TYSiteNode*>(pElt);
    if (pSite != nullptr)
    {
        pSiteNode = TYSiteNode::safeDownCast(pElement);

        pSite->setRoot(false); // Le site ajoute n'est pas racine

        pSiteNode->addSiteNode(pSite);
        if (_pCurrentCalcul) { _pCurrentCalcul->getCalculElements(pSite); }

        pSiteNode->updateGraphicTree();
        pSiteNode->setIsGeometryModified(true);
    }
    else if (dynamic_cast<TYBatiment*>(pElt) != nullptr)
    {
        TYBatiment* pBatiment = TYBatiment::safeDownCast(pElt);

        if (dynamic_cast<TYSiteNode*>(pElement) != nullptr)
        {
            pInfra = TYSiteNode::safeDownCast(pElement)->getInfrastructure();
        }
        else
        {
            pInfra = TYInfrastructure::safeDownCast(pElement);
        }

        if (pInfra)
        {
            pInfra->addBatiment(pBatiment, TYRepere());
        }
    }
    else if (dynamic_cast<TYMachine*>(pElt) != nullptr)
    {
        TYMachine* pMachine = TYMachine::safeDownCast(pElt);

        if (dynamic_cast<TYEtage*>(pElement) != nullptr)
        {
            TYEtage* pEtage = TYEtage::safeDownCast(pElement);
            if (pEtage)
            {
                pEtage->addMachine(pMachine);
                pEtage->updateGraphicTree();
                reallyAdd = false;
            }
        }
        else
        {
            if (dynamic_cast<TYSiteNode*>(pElement) != nullptr)
            {
                pInfra = TYSiteNode::safeDownCast(pElement)->getInfrastructure();
            }
            else
            {
                pInfra = TYInfrastructure::safeDownCast(pElement);
            }

            if (pInfra)
            {
                pInfra->addMachine(pMachine, TYRepere());
            }
        }
    }
    else if (dynamic_cast<TYUserSourcePonctuelle*>(pElt) != nullptr)
    {
        TYUserSourcePonctuelle* pSource = TYUserSourcePonctuelle::safeDownCast(pElt);

        if (dynamic_cast<TYEtage*>(pElement) != nullptr)
        {
            TYEtage* pEtage = TYEtage::safeDownCast(pElement);
            if (pEtage)
            {
                pEtage->addSource(pSource);
                pEtage->updateGraphicTree();
                reallyAdd = false;
            }
        }
        else
        {
            if (dynamic_cast<TYSiteNode*>(pElement) != nullptr)
            {
                pInfra = TYSiteNode::safeDownCast(pElement)->getInfrastructure();
            }
            else
            {
                pInfra = TYInfrastructure::safeDownCast(pElement);
            }

            if (pInfra)
            {
                pInfra->addSrc(pSource);
            }
        }
    }

    if (_pCurrentCalcul)
    {
        if (reallyAdd) { _pCurrentCalcul->addToSelection(pElt); }
        _pSiteNodeRoot->updateAcoustique(true);
        _pSiteNodeRoot->getGraphicObject()->update(true);
        updateList();
        TYElement::setIsSavedOk(true);
    }
}

void TYSiteFrame::newElt(const char* className, TYElement* pElement)
{
    TYElement* pElt = NULL;
    try
    {
        pElt = dynamic_cast<TYElement*>(TYElement::findAndClone(className));
    }
    catch(tympan::invalid_data& exc) {}

    assert (pElt);
    if (pElement) { addElt(pElement, pElt); }

    if ((dynamic_cast<TYSiteNode*>(pElt) != nullptr) ||
        (dynamic_cast<TYAcousticVolumeNode*>(pElt) != nullptr))
    {
        getTYMainWnd()->makeModeler(pElt);
    }
    else
    {
        pElt->edit(this);
    }

    if ((dynamic_cast<TYSiteNode*>(pElt) != nullptr) && _pCurrentCalcul)
    {
        _pCurrentCalcul->addToSelection(pElt);
        _pSiteNodeRoot->updateAcoustique();
        _pSiteNodeRoot->getGraphicObject()->update(true);
        updateList();
        TYElement::setIsSavedOk(true);
    }
}

void TYSiteFrame::selectOrUnselectAll(TYElementListItem* item, const bool& bSelect)
{
    if (getTYApp()->getCalculManager()->askForResetResultat())
    {
        bool on = bSelect;

        for (int i = 0; (i < item->childCount()) && _pCurrentCalcul; i++)
        {
            static_cast<TYElementListItem*>(item->child(i))->setOn(on, false);
        }

        _pSiteNodeRoot->getGraphicObject()->update(true);
        getTYMainWnd()->updateModelers(false, false);
    }
}

void TYSiteFrame::setDMaxDefault(TYElementListItem* item)
{
    TYElement* pElement = NULL;
    TYCourbeNiveau* pCourbe = NULL;
    TYPlanEau* pPlanEau = NULL;

    for (int i = 0; i < item->childCount(); i++)
    {
        pElement = static_cast<TYElementListItem*>(item->child(i))->getElement();

        if (pElement->isA("TYCourbeNiveau"))
        {
            pCourbe = TYCourbeNiveau::safeDownCast(pElement);
            pCourbe->setIsDMaxDefault(false);
        }
        else if (pElement->isA("TYPlanEau"))
        {
            pPlanEau = TYPlanEau::safeDownCast(pElement);
            pPlanEau->setIsDMaxDefault(false);
        }
    }

    _pSiteNodeRoot->getGraphicObject()->update(true);
    getTYMainWnd()->updateModelers(false, false);
    TYElement::setIsSavedOk(true);
}
