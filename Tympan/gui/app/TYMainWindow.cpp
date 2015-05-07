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
 * \file TYMainWindow.cpp
 * \brief Fenetre principale de l'application Tympan
 */


//Added by qt3to4:
#include <QPixmap>
#include <QCloseEvent>
#include <qaction.h>
#include <qdir.h>
#include <qslider.h>
#include <qstatusbar.h>
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qfileinfo.h>
#include <qtoolbutton.h>
#include <qbuttongroup.h>
#include <qmessagebox.h>
#include <qcursor.h>
#include <qmainwindow.h>
#include <qprocess.h>
#include <qtextbrowser.h>
#include <qtextstream.h>
#include <QDockWidget>
#include <QMdiArea>
#include <QMdiSubWindow>

#include "Tympan/core/config.h"
#include "Tympan/models/business/acoustic/TYSpectre.h"
#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/TYPreferenceManager.h"
#include "Tympan/models/business/TYXMLManager.h"
#include "Tympan/models/business/TYPluginManager.h"
#include "Tympan/gui/widgets/TYSpectreWidget.h"
#include "Tympan/gui/gl/TYPickingTable.h"
#include "Tympan/gui/app/TYCustomPopupMenu.h"
#include "Tympan/gui/app/TYCalculToolbar.h"
#include "Tympan/gui/app/TYInfraToolbar.h"
#include "Tympan/gui/app/TYTopoToolbar.h"
#include "Tympan/gui/app/TYBatimentToolbar.h"
#include "Tympan/gui/app/TYBatimentModelerFrame.h"
#include "Tympan/gui/app/TYFaceModelerFrame.h"
#include "Tympan/gui/app/TYFaceToolbar.h"
#include "Tympan/gui/app/TYMachineModelerFrame.h"
#include "Tympan/gui/app/TYMachineToolbar.h"

#include "Tympan/gui/app/TYModelerFrame.h"
#include "Tympan/gui/app/TYModelerToolbar.h"
#include "Tympan/gui/app/TYPreferenceDialog.h"
#include "Tympan/gui/app/TYOpenElementDialog.h"
#include "Tympan/gui/app/TYPluginDialog.h"
#include "Tympan/gui/app/TYCreateElementDialog.h"
#include "Tympan/gui/app/TYProjetFrame.h"
#include "Tympan/gui/app/TYSiteModelerFrame.h"
#include "Tympan/gui/app/TYSiteFrame.h"
#include "Tympan/gui/app/TYOutputFrame.h"
#include "Tympan/gui/app/TYApplication.h"
#include "TYMainWindow.h"

#define TR(id) OLocalizator::getString("TYMainWindow", (id))
#define IMG(id) OLocalizator::getPicture("TYMainWindow", (id))


using namespace Qt;

// to avoid X11 macro conflit
#ifdef Always
#undef Always
#endif



TYMainWindow::TYMainWindow():
    QMainWindow(0)
{
    //  QApplication::setStyle(new QGtkStyle());

    TYPickingTable _pPickingTable;

    setWindowTitle(TR("id_caption") + "-" + TY_CURRENT_RELEASE_ + " (" + TY_PRODUCT_PLATFORM_ + ")");

    _closeAndQuit = true;

    // Gestion du workspace
    _pWorkspace = new QMdiArea();

    setCentralWidget(_pWorkspace);


    // CLM-NT35 : Empecher de deplacer/tourner en vue perspective/mobile
    _pCurrentModeler = NULL;

    // Settings du docking
    //setDockWindowsMovable(true);

    // Navigateur d'aide
    _pHelpBrowser = new QTextBrowser(NULL);
    _pHelpBrowser->resize(800, 600);
    _pHelpBrowser->setSource(TYApplication::tympanAppDir() + "/" + TR("id_help_index_path"));
    _pHelpBrowser->hide();


    // Modes des Modelers
    _curSiteMode = TYModelerFrame::CameraMode;
    _curMachineMode = TYModelerFrame::CameraMode;
    _curBatimentMode = TYModelerFrame::CameraMode;
    _curFaceMode = TYModelerFrame::CameraMode;


    // Actions
    _pCreateNewAction = new QAction(QPixmap(IMG("id_icon_createnew")), TR("id_menuitem_createnew"), this);
    _pCreateNewAction->setShortcut(CTRL + Key_N);
    _pCreateNewAction->setStatusTip(TR("id_status_createnew"));
    QObject::connect(_pCreateNewAction, SIGNAL(activated()), this, SLOT(createNew()));

    _pOpenAction = new QAction(QPixmap(IMG("id_icon_open")), TR("id_menuitem_open"), this);
    _pOpenAction->setShortcut(CTRL + Key_O);
    _pOpenAction->setStatusTip(TR("id_status_open"));
    QObject::connect(_pOpenAction, SIGNAL(activated()), this, SLOT(open()));

    _pCloseAction = new QAction(QPixmap(IMG("id_icon_close")), TR("id_menuitem_close"), this);
    _pCloseAction->setStatusTip(TR("id_status_close"));
    QObject::connect(_pCloseAction, SIGNAL(activated()), this, SLOT(close()));
    _pCloseAction->setEnabled(false);

    _pSaveAction = new QAction(QPixmap(IMG("id_icon_save")), TR("id_menuitem_save"), this);
    _pSaveAction->setShortcut(CTRL + Key_S);
    _pSaveAction->setStatusTip(TR("id_status_save"));
    QObject::connect(_pSaveAction, SIGNAL(activated()), this, SLOT(save()));

    _pSaveAsAction = new QAction(QPixmap(IMG("id_icon_save")), TR("id_menuitem_saveas"), this);
    _pSaveAsAction->setStatusTip(TR("id_status_saveas"));
    QObject::connect(_pSaveAsAction, SIGNAL(activated()), this, SLOT(saveAs()));

    _pSaveAsNoResultAction = new QAction(QPixmap(IMG("id_icon_save")), TR("id_menuitem_saveas_noresult"), this);
    _pSaveAsNoResultAction->setStatusTip(TR("id_status_saveas_noresult"));
    QObject::connect(_pSaveAsNoResultAction, SIGNAL(activated()), this, SLOT(saveAsNoResult()));

    _pShowPluginManagerAction = new QAction(QPixmap(IMG("id_icon_plugin")), TR("id_menuitem_plugin"), this);
    _pShowPluginManagerAction->setStatusTip(TR("id_status_plugin"));
    QObject::connect(_pShowPluginManagerAction, SIGNAL(activated()), this, SLOT(showPluginManager()));


    // XXX TODO Add a entry to the Languages_**.xml with the 'TR()' macro.
    _pImportDXFBdTopoAction = new QAction(QPixmap(IMG("id_icon_importxml")),
                                          "Import BD TOPO (DXF)", this);
    _pImportDXFBdTopoAction->setShortcut(CTRL + Key_B);
    _pImportDXFBdTopoAction->setStatusTip("Import BD TOPO (DXF)");
    QObject::connect(_pImportDXFBdTopoAction, SIGNAL(activated()), this,
                     SLOT(importFromBD_TOPO()));

    // XXX What does it do?
#ifndef TY_USE_DXF
    // _pImportDXFBdTopoAction->setEnabled(false);
#endif // TY_USE_DXF

    //az++:
    _pPrintAction = new QAction(QPixmap(IMG("id_icon_print")), TR("id_menuitem_print"), this);
    _pPrintAction->setShortcut(CTRL + Key_P);
    _pPrintAction->setStatusTip(TR("id_status_print"));
    QObject::connect(_pPrintAction, SIGNAL(activated()), this, SLOT(print()));
    _pPrintAction->setEnabled(false);

    _pUndoAction = new QAction(QPixmap(IMG("id_icon_undo")), TR("id_menuitem_undo_na"), this);
    _pUndoAction->setShortcut(CTRL + Key_Z);
    _pUndoAction->setStatusTip(TR("id_status_undo"));
    QObject::connect(_pUndoAction, SIGNAL(activated()), this, SLOT(undo()));
    _pUndoAction->setEnabled(false);

    _pRedoAction = new QAction(QPixmap(IMG("id_icon_redo")), TR("id_menuitem_redo_na"), this);
    _pRedoAction->setShortcut(CTRL + Key_Y);
    _pRedoAction->setStatusTip(TR("id_status_redo"));
    QObject::connect(_pRedoAction, SIGNAL(activated()), this, SLOT(redo()));
    _pRedoAction->setEnabled(false);

    _pCutAction = new QAction(QPixmap(IMG("id_icon_cut")), TR("id_menuitem_cut"), this);
    _pCutAction->setShortcut(CTRL + Key_X);
    _pCutAction->setStatusTip(TR("id_status_cut"));
    QObject::connect(_pCutAction, SIGNAL(activated()), this, SLOT(cut()));
    _pCutAction->setEnabled(false);

    _pCopyAction = new QAction(QPixmap(IMG("id_icon_copy")), TR("id_menuitem_copy"), this);
    _pCopyAction->setShortcut(CTRL + Key_C);
    _pCopyAction->setStatusTip(TR("id_status_cut"));
    QObject::connect(_pCopyAction, SIGNAL(activated()), this, SLOT(copy()));
    _pCopyAction->setEnabled(false);

    _pPasteAction = new QAction(QPixmap(IMG("id_icon_paste")), TR("id_menuitem_paste"), this);
    _pPasteAction->setShortcut(CTRL + Key_V);
    _pPasteAction->setStatusTip(TR("id_status_paste"));
    QObject::connect(_pPasteAction, SIGNAL(activated()), this, SLOT(paste()));
    _pPasteAction->setEnabled(false);

    _pHideOrShowSpectreMngrAction = new QAction(QPixmap(IMG("id_icon_show_spectremngr")), TR("id_menuitem_show_spectremngr"), this);
    _pHideOrShowSpectreMngrAction->setCheckable(false); // DT 18-05-04 (true change en false)
    _pHideOrShowSpectreMngrAction->setStatusTip(TR("id_status_show_spectremngr"));
    _pHideOrShowSpectreMngrAction->setEnabled(false); // Inactive au depart
    QObject::connect(_pHideOrShowSpectreMngrAction, SIGNAL(toggled(bool)), this, SLOT(showSpectreManager(bool)));

    _pHideOrShowMaillageMngrAction = new QAction(QPixmap(IMG("id_icon_show_maillagemngr")), TR("id_menuitem_show_maillagemngr"), this);
    _pHideOrShowMaillageMngrAction->setCheckable(false);// DT 18-05-04 (true change en false)
    _pHideOrShowMaillageMngrAction->setStatusTip(TR("id_status_show_maillagemngr"));
    _pHideOrShowMaillageMngrAction->setEnabled(false); // Inactive au depart
    QObject::connect(_pHideOrShowMaillageMngrAction, SIGNAL(toggled(bool)), this, SLOT(showMaillageManager(bool)));

    _pHideOrShowBiblioAction = new QAction(QPixmap(IMG("id_icon_show_biblio")), TR("id_menuitem_show_biblio"), this);
    _pHideOrShowBiblioAction->setCheckable(true);
    _pHideOrShowBiblioAction->setStatusTip(TR("id_status_show_biblio"));
    QObject::connect(_pHideOrShowBiblioAction, SIGNAL(toggled(bool)), this, SLOT(showBiblio(bool)));
    _pHideOrShowBiblioAction->setEnabled(false);

    _pHideOrShowOutputAction = new QAction(QPixmap(IMG("id_icon_show_output")), TR("id_menuitem_show_output"), this);
    _pHideOrShowOutputAction->setCheckable(true);
    _pHideOrShowOutputAction->setStatusTip(TR("id_status_show_output"));
    QObject::connect(_pHideOrShowOutputAction, SIGNAL(toggled(bool)), this, SLOT(showOutput(bool)));

    _pEditCurCalculAction = new QAction(QPixmap(IMG("id_icon_edit_curcalcul")), TR("id_menuitem_edit_curcalcul"), this);
    _pEditCurCalculAction->setStatusTip(TR("id_status_edit_curcalcul"));
    QObject::connect(_pEditCurCalculAction, SIGNAL(activated()), this, SLOT(editCurCalcul()));
    _pEditCurCalculAction->setEnabled(false);

    _pGoCurCalculAction = new QAction(QPixmap(IMG("id_icon_go_curcalcul")), TR("id_menuitem_go_curcalcul"), this);
    _pGoCurCalculAction->setStatusTip(TR("id_status_go_curcalcul"));
    QObject::connect(_pGoCurCalculAction, SIGNAL(activated()), this, SLOT(goCurCalcul()));
    _pGoCurCalculAction->setEnabled(false);

    QAction* pPrefDialogAction = new QAction(QPixmap(IMG("id_icon_prefdlg")), TR("id_menuitem_prefdlg"), this);
    QObject::connect(pPrefDialogAction, SIGNAL(activated()), this, SLOT(showPreferenceDialog()));

    // Barre de menu
    QMenu* pFileMenu = new QMenu(TR("id_menu_file"), this);
    menuBar()->addMenu(pFileMenu);

    QMenu* pNewMenu = new QMenu(TR("id_menuitem_new"), this);
    pNewMenu->setTearOffEnabled(true);
    pNewMenu->addAction(TR("id_menuitem_newprojet"), this, SLOT(createNewProjet()));
    pNewMenu->addAction(TR("id_menuitem_newsite"), this, SLOT(createNewSite()));
    pNewMenu->addAction(TR("id_menuitem_newbatiment"), this, SLOT(createNewBatiment()));
    pNewMenu->addAction(TR("id_menuitem_newmachine"), this, SLOT(createNewMachine()));
    pNewMenu->addAction(_pCreateNewAction);

    pFileMenu->addMenu(pNewMenu);
    pFileMenu->addAction(_pOpenAction);
    pFileMenu->addAction(_pCloseAction);
    pFileMenu->addSeparator();
    pFileMenu->addAction(_pSaveAction);
    pFileMenu->addAction(_pSaveAsAction);
    pFileMenu->addAction(_pSaveAsNoResultAction);
    pFileMenu->addSeparator();
    pFileMenu->addAction(_pShowPluginManagerAction);
    pFileMenu->addSeparator();
    pFileMenu->addAction(_pImportDXFBdTopoAction);
    // _pImportDXFBdTopoAction->addTo(pFileMenu); // XXX difference with the previous line?
    pFileMenu->addSeparator();
    pFileMenu->addAction(_pPrintAction);
    pFileMenu->addSeparator();
    pFileMenu->addAction(TR("id_menuitem_quit"), this, SLOT(quitte()), QKeySequence("Ctrl+Q"));

    QMenu* pEditMenu = new QMenu(TR("id_menu_edit"), this);
    menuBar()->addMenu(pEditMenu);
    pEditMenu->addAction(_pUndoAction);
    pEditMenu->addAction(_pRedoAction);
    pEditMenu->addSeparator();
    pEditMenu->addAction(_pCutAction);
    pEditMenu->addAction(_pCopyAction);
    pEditMenu->addAction(_pPasteAction);
    pEditMenu->addSeparator();
    pEditMenu->addAction(QPixmap(IMG("id_icon_prefdlg")), TR("id_menuitem_prefdlg"), this, SLOT(showPreferenceDialog()));

    _pWindowsMenu = new QMenu(TR("id_menu_windows"), this);
    connect(_pWindowsMenu, SIGNAL(aboutToShow()), this, SLOT(windowsMenuAboutToShow()));
    menuBar()->addMenu(_pWindowsMenu);

    menuBar()->addSeparator();

    QMenu* pHelpMenu = new QMenu(TR("id_menu_help"), this);
    menuBar()->addMenu(pHelpMenu);
    pHelpMenu->addAction(TR("id_menuitem_help"), this, SLOT(showHelp()), QKeySequence("F1"));
    pHelpMenu->addSeparator();
    pHelpMenu->addAction(TR("id_menuitem_about"), this, SLOT(about()));


    // Barre d'outils fichier
    _pMainToolbar = new QToolBar(TR("id_toolbar_file"), this);
    addToolBar(_pMainToolbar);
    QToolButton* pToolNewFile = new QToolButton(this);
    pToolNewFile->setIcon(QPixmap(IMG("id_icon_new")));
    pToolNewFile->setToolTip(TR("id_menuitem_new"));
    pToolNewFile->setMenu(pNewMenu);
    pToolNewFile->setPopupMode(QToolButton::InstantPopup);
    _pMainToolbar->addWidget(pToolNewFile);
    _pMainToolbar->addSeparator();
    _pMainToolbar->addAction(_pOpenAction);
    _pMainToolbar->addAction(_pSaveAction);
    _pMainToolbar->addSeparator();
    _pMainToolbar->addAction(_pPrintAction);
    _pMainToolbar->addSeparator();
    _pMainToolbar->addAction(_pCutAction);
    _pMainToolbar->addAction(_pCopyAction);
    _pMainToolbar->addAction(_pPasteAction);
    _pMainToolbar->addSeparator();
    _pMainToolbar->addAction(_pUndoAction);
    _pMainToolbar->addAction(_pRedoAction);
    _pMainToolbar->setObjectName("MainToolbar");

    // Barre d'outils des fenetres
    _pWindowToolbar = new QToolBar(TR("id_toolbar_window"), this);
    addToolBar(_pWindowToolbar);
    _pWindowToolbar->addAction(_pHideOrShowSpectreMngrAction);
    _pWindowToolbar->addAction(_pHideOrShowMaillageMngrAction);
    _pWindowToolbar->addAction(_pHideOrShowBiblioAction);
    _pWindowToolbar->addSeparator();
    _pWindowToolbar->addAction(_pHideOrShowOutputAction);
    _pWindowToolbar->addSeparator();
    _pWindowToolbar->addAction(pPrefDialogAction);
    _pWindowToolbar->setObjectName("WindowToolbar");

    // Barre d'outils de calcul acoustique
    _pCalculAcousticToolbar = new QToolBar(TR("id_toolbar_calculacoustic"), this);
    addToolBar(_pCalculAcousticToolbar);
    _pCalculAcousticToolbar->addAction(_pEditCurCalculAction);
    _pCalculAcousticToolbar->addAction(_pGoCurCalculAction);
    _pCalculAcousticToolbar->setObjectName("CalculAcousticToolbar");

    // Barre d'outils pour les modelers
    _pToolbarModeler = new TYModelerToolbar(0, this, TR("id_toolbar_modeler"));
    addToolBar(_pToolbarModeler);
    _pToolbarModeler->setObjectName("ToolbarModeler");

    // Btn group pour Site
    _pSiteBtnGroup = new QButtonGroup(this);
    //  _pSiteBtnGroup->hide();
    _pSiteBtnGroup->setExclusive(true);

    // Barre d'outils Topographie
    _pToolbarTopo = new TYTopoToolbar(_pSiteBtnGroup, this,  TR("id_toolbar_topo"));
    addToolBar(_pToolbarTopo);
    _pToolbarTopo->setObjectName("ToolbarTopo");

    // Barre d'outils Calcul
    _pToolbarCalcul = new TYCalculToolbar(_pSiteBtnGroup, this, TR("id_toolbar_calcul"));
    addToolBar(_pToolbarCalcul);
    _pToolbarCalcul->setObjectName("ToolbarCalcul");

    // Barre d'outils Infrastructure
    _pToolbarInfra = new TYInfraToolbar(_pSiteBtnGroup, this, TR("id_toolbar_infra"));
    addToolBar(_pToolbarInfra);
    _pToolbarInfra->setObjectName("ToolbarInfra");


    // Btn group pour Machine
    _pMachineBtnGroup = new QButtonGroup(this);
    //  _pMachineBtnGroup->hide();
    _pMachineBtnGroup->setExclusive(true);

    // Barre d'outils Machine
    _pToolbarMachine = new TYMachineToolbar(_pMachineBtnGroup, this, TR("id_toolbar_machine"));
    addToolBar(_pToolbarMachine);
    _pToolbarMachine->setObjectName("ToolbarMachine");

    // Btn group pour Face
    _pFaceBtnGroup = new QButtonGroup(this);
    //  _pFaceBtnGroup->hide();
    _pFaceBtnGroup->setExclusive(true);

    // Barre d'outils Face
    _pToolbarFace = new TYFaceToolbar(_pFaceBtnGroup, this, TR("id_toolbar_Face"));
    addToolBar(_pToolbarFace);
    _pToolbarFace->setObjectName("ToolbarFace");

    // Btn group pour Batiment
    _pBatimentBtnGroup = new QButtonGroup(this);
    //  _pBatimentBtnGroup->hide();
    _pBatimentBtnGroup->setExclusive(true);

    // Barre d'outils Batiment
    _pToolbarBatiment = new TYBatimentToolbar(_pBatimentBtnGroup, this, TR("id_toolbar_batiment"));
    addToolBar(_pToolbarBatiment);
    _pToolbarBatiment->setObjectName("ToolbarBatiment");

    // Fenetre Projet
    _pProjetDockWnd = new QDockWidget(TR("id_toolbar_projet"), this);
    _pProjetDockWnd->setFloating(false);
    _pProjetFrame = new TYProjetFrame(_pProjetDockWnd);
    _pProjetDockWnd->setWidget(_pProjetFrame);
    _pProjetDockWnd->setObjectName("ProjetDockWnd");
    _pProjetDockWnd->setContentsMargins(0, 0, 0, 0);
    addDockWidget(Qt::LeftDockWidgetArea, _pProjetDockWnd);

    // Fenetre Site
    _pSiteDockWnd = new QDockWidget(TR("id_toolbar_site"), this);
    _pSiteDockWnd->setFloating(false);
    _pSiteFrame = new TYSiteFrame(_pSiteDockWnd);
    _pSiteDockWnd->setWidget(_pSiteFrame);
    _pSiteDockWnd->setObjectName("SiteDockWnd");
    addDockWidget(Qt::LeftDockWidgetArea, _pSiteDockWnd);

    // Fenetre Output
    _pOutputDockWnd = new QDockWidget(TR("id_toolbar_output"), this);
    _pOutputDockWnd->setFloating(false);
    _pOutputFrame = new TYOutputFrame(_pOutputDockWnd);
    _pOutputDockWnd->setWidget(_pOutputFrame);
    _pOutputDockWnd->setObjectName("OutputDockWnd");
    addDockWidget(Qt::LeftDockWidgetArea, _pOutputDockWnd);
    QObject::connect(_pOutputDockWnd, SIGNAL(visibilityChanged(bool)), _pHideOrShowOutputAction, SLOT(setOn(bool)));

    // Connexion des signaux
    QObject::connect(getTYApp(), SIGNAL(curProjetChanged(LPTYProjet)), this, SLOT(setCurProjet(LPTYProjet)));
    QObject::connect(getTYApp(), SIGNAL(curSiteNodeChanged(LPTYSiteNode)), this, SLOT(setCurSiteNode(LPTYSiteNode)));
    QObject::connect(_pProjetFrame, SIGNAL(changeCurrentCalcul(LPTYCalcul)), _pSiteFrame, SLOT(setCurrentCalcul(LPTYCalcul)));
    QObject::connect(_pProjetFrame, SIGNAL(changeSite(LPTYSiteNode)), _pSiteFrame, SLOT(setSiteNodeRoot(LPTYSiteNode)));
    QObject::connect(_pProjetFrame, SIGNAL(changeCurrentCalcul(LPTYCalcul)), this, SLOT(updateCurCalcul()));
    QObject::connect(_pWorkspace, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(subWindowActivated()));

    // Connecte l'Action Manager de l'Application
    connectDefaultActionManager();

    // Desactive tous les boutons
    subWindowActivated();

    // Ready to work !!
    statusBar()->showMessage(TR("id_status_ready"));

    // Indique que les TYElement sont ok (rien a sauvegarder)
    TYElement::setIsSavedOk(false);
}

TYMainWindow::~TYMainWindow()
{
    delete _pHelpBrowser;
    _pHelpBrowser = NULL;
}

QMenu* TYMainWindow::createPopupMenu()
{
    QMenu* returnedMenu =  QMainWindow::createPopupMenu();
    TYCustomPopupMenu* myMenu = new TYCustomPopupMenu();
    myMenu->addActions(returnedMenu->actions());
    delete returnedMenu;
    return myMenu;
}

void TYMainWindow::reloadSlots()
{
}

bool TYMainWindow::loadSettings(const QString& fileName)
{
    bool res = false;

    statusBar()->showMessage(TR("id_status_load_settings"), 3000);

    QFile* inFile = new QFile(QString(fileName) + QString("MainWindowIHM.bin"));
    if (inFile->open(QIODevice::ReadOnly))
    {
        QByteArray byteArray = inFile->readAll();
        if (byteArray.isEmpty())
        {
            inFile->close();
            delete inFile;
            return false;
        }

        res = restoreState(byteArray);
        inFile->close();
        delete inFile;
    }
    else
    {
        return false;
    }

    QString finalFileName = QString(fileName) + QString(".xml");

    // Cas d'une premiere utilisation de TYMPAN 3.6
    // Verification de l'existence du fichier
    QFileInfo fi(finalFileName);
    // Si le fichier existe , on demande confirmation a l'utilisateur
    if (!fi.exists())
    {
        finalFileName = QDir::convertSeparators(getTYApp()->tympanUserDir() + "/Settings" + ".xml");
    }

    TYPreferenceManager::init(finalFileName);

    // Chargement des settings
    res = TYPreferenceManager::read();

    // Apply settings
    TYPreferenceManager::loadGeometryFromPreferences(metaObject()->className(), this);

    // Dock windows
    QObjectList objectsList = children();//dockWindows();
    QWidget* pDockWnd = NULL;
    QObject* pObject = NULL;

    for (int _i = 0; _i < objectsList.size(); _i++)
    {
        pObject = objectsList[_i];
        if ((pObject->objectName() == "ProjetDockWnd")
            || (pObject->objectName() == "SiteDockWnd")
            || (pObject->objectName() == "OutputDockWnd")
            || (pObject->metaObject()->className() == "QToolBar")
            || (dynamic_cast<QToolBar*>(pObject) != nullptr))
        {
            pDockWnd = (QWidget*)pObject;
            QString key = pDockWnd->objectName();
            // Geometry
            TYPreferenceManager::loadGeometryFromPreferences(key, pDockWnd);
        }
    }

    return res;
}

bool TYMainWindow::saveSettings(const QString& fileName)
{
    bool res = false;

    QByteArray byteArray = saveState();
    QFile* outFile = new QFile(QString(fileName) + QString("MainWindowIHM.bin"));
    if (outFile->open(QIODevice::WriteOnly))
    {
        outFile->write(byteArray);
        outFile->close();
    }

    delete outFile;

    // Sauvegarde des settings
    QString finalFileName = QString(fileName) + QString(".xml");
    TYPreferenceManager::setFileName(finalFileName);
    res = TYPreferenceManager::write();

    return res;
}

void TYMainWindow::showHelp()
{
    _pHelpBrowser->show();
}

void TYMainWindow::about()
{
    // Dlg "A propos de Tympan"
    QMessageBox* pAboutTympan = new QMessageBox(NULL);
    pAboutTympan->setWindowTitle(windowTitle());
    QString msg = TR("id_about_msg");
    msg = msg.arg(TY_PRODUCT_NAME_);
    msg = msg.arg(TY_CURRENT_RELEASE_);
    msg = msg.arg(TY_PRODUCT_PLATFORM_);
    msg = msg.arg(TY_PRODUCT_COPYRIGHT_);
    msg = msg.arg(TY_BUILD_DATE_);
    msg = msg.arg(TY_BUILD_TIME_);
    msg = msg.arg(TY_CUSTOMER_);
    msg = msg.arg(TY_LICENCE_NUMBER_);
    msg = msg.arg(TY_TIME_LIMIT_);
    pAboutTympan->setText(msg);
    pAboutTympan->setIconPixmap(IMG("id_logo_about"));

    // Affichage
    pAboutTympan->exec();

    // Nettoyage
    delete pAboutTympan;
    pAboutTympan = NULL;
}

void TYMainWindow::makeModeler(TYElement* pElt)
{
    if (!pElt) { return; }

    TYProjet* pProj = dynamic_cast<TYProjet*>(pElt);
    if (pProj != nullptr)
    {
        makeProjetModeler(pProj);
    }
    else if (pElt->isA("TYSiteNode"))
    {
        makeSiteModeler(static_cast<TYSiteNode*>(pElt));
    }
    else if (pElt->isA("TYBatiment"))
    {
        makeBatimentModeler(static_cast<TYBatiment*>(pElt));
    }
    else if (pElt->isA("TYMachine"))
    {
        makeMachineModeler(static_cast<TYMachine*>(pElt));
    }
}

void TYMainWindow::makeBatimentModeler(LPTYBatiment pBatiment)
{
    TYBatimentModelerFrame* pBatimentModeler = new TYBatimentModelerFrame(NULL, _pWorkspace, "TYBatimentModelerFrame");
    pBatimentModeler->setAttribute(WA_DeleteOnClose);
    _pWorkspace->addSubWindow(pBatimentModeler)->setObjectName("TYBatimentModelerFrame");

    QObject::connect(pBatimentModeler, SIGNAL(editorModeChanged(int)), this, SLOT(saveCurBatimentMode(int)));
    QObject::connect(pBatimentModeler, SIGNAL(frameResized()), this, SLOT(refreshWindowTitle()));

    if (!pBatiment) { pBatiment = new TYBatiment(); }
    pBatimentModeler->setBatiment(pBatiment);

    pBatimentModeler->showMaximized();
    pBatimentModeler->fit();
}

void TYMainWindow::makeMachineModeler(LPTYMachine pMachine)
{
    TYMachineModelerFrame* pMachineModeler = new TYMachineModelerFrame(NULL, _pWorkspace, "TYMachineModelerFrame");
    pMachineModeler->setAttribute(WA_DeleteOnClose);

    _pWorkspace->addSubWindow(pMachineModeler)->setObjectName("TYMachineModelerFrame");

    QObject::connect(pMachineModeler, SIGNAL(editorModeChanged(int)), this, SLOT(saveCurMachineMode(int)));
    QObject::connect(pMachineModeler, SIGNAL(frameResized()), this, SLOT(refreshWindowTitle()));

    if (!pMachine) { pMachine = new TYMachine(); }
    pMachineModeler->setMachine(pMachine);

    pMachineModeler->showMaximized();
    pMachineModeler->fit();
}

void TYMainWindow::makeSiteModeler(LPTYSiteNode pSite)
{
    TYSiteModelerFrame* pSiteModeler = new TYSiteModelerFrame(pSite, _pWorkspace, "TYSiteModelerFrame");
    pSiteModeler->setAttribute(WA_DeleteOnClose);

    _pWorkspace->addSubWindow(pSiteModeler)->setObjectName("TYSiteModelerFrame");

    QObject::connect(pSiteModeler, SIGNAL(editorModeChanged(int)), this, SLOT(saveCurSiteMode(int)));
    QObject::connect(pSiteModeler, SIGNAL(frameResized()), this, SLOT(refreshWindowTitle()));

    pSiteModeler->showMaximized();
    pSiteModeler->fit();
}

void TYMainWindow::makeProjetModeler(LPTYProjet pProjet)
{
    TYSiteModelerFrame* pSiteModeler = new TYSiteModelerFrame(pProjet, _pWorkspace, "TYSiteModelerFrame");
    pSiteModeler->setAttribute(WA_DeleteOnClose);

    _pWorkspace->addSubWindow(pSiteModeler)->setObjectName("TYSiteModelerFrame");

    QObject::connect(pSiteModeler, SIGNAL(editorModeChanged(int)), this, SLOT(saveCurSiteMode(int)));
    QObject::connect(pSiteModeler, SIGNAL(frameResized()), this, SLOT(refreshWindowTitle()));

    pSiteModeler->showMaximized();
    pSiteModeler->fit();
}

void TYMainWindow::connectActionManager(TYActionManager* pActionManager)
{
    QObject::disconnect(this, SIGNAL(onUndo()), 0, 0);
    QObject::disconnect(this, SIGNAL(onRedo()), 0, 0);

    if (pActionManager)
    {
        QObject::connect(this, SIGNAL(onUndo()), pActionManager, SLOT(undo()));
        QObject::connect(this, SIGNAL(onRedo()), pActionManager, SLOT(redo()));

        QObject::connect(pActionManager, SIGNAL(undoRedoChanged(bool, bool, const QString&, const QString&)),
                         this, SLOT(updateUndoRedo(bool, bool, const QString&, const QString&)));
        pActionManager->emitUndoRedo();
    }
    else
    {
        connectDefaultActionManager();
    }
}

void TYMainWindow::connectDefaultActionManager()
{
    connectActionManager(getTYApp()->getActionManager());
}

void TYMainWindow::updateModelers(bool clipping /*=true*/, bool axesAndGrid /*=true*/, bool displayList/*=true*/)
{
    QList<QMdiSubWindow*> windows = _pWorkspace->subWindowList();

    for (int i = 0; i < int(windows.count()); ++i)
    {
        QWidget* internal_window = windows.at(i)->widget();

        TYModelerFrame* pInW = dynamic_cast<TYModelerFrame*>(internal_window);
        if (pInW != nullptr)
        {
            pInW->updateView(clipping, axesAndGrid); //az--
            if (displayList)
            {
                TYRenderWindowInteractor* pView = ((TYModelerFrame*) internal_window)->getView();
                TYOpenGLRenderer* pOpenGLRenderer = pView->getRenderer();

                pOpenGLRenderer->updateDisplayList(); // az-- test
                ((TYModelerFrame*) internal_window)->updateView(clipping, axesAndGrid); //az--
                pView->updateGL();
            }
        }
    }
}


void TYMainWindow::updateModelersElementGraphic(bool force /* = false */)
{
    QList<QMdiSubWindow*> windows = _pWorkspace->subWindowList();

    for (int i = 0; i < int(windows.count()); ++i)
    {
        QWidget* internal_window = windows.at(i)->widget();

        TYModelerFrame* pInW = dynamic_cast<TYModelerFrame*>(internal_window);
        if (pInW != nullptr)
        {
            pInW->updateElementGraphic(force);
        }
    }
}


void TYMainWindow::closeModeler(const TYElement* pElement)
{
    QList<QMdiSubWindow*> windows = _pWorkspace->subWindowList();

    for (int i = 0; i < int(windows.count()); ++i)
    {
        QWidget* internal_window = windows.at(i)->widget();
        QWidget* container_window = windows.at(i);

        if (dynamic_cast<TYModelerFrame*>(internal_window) != nullptr)
        {
            if (static_cast<TYModelerFrame*>(internal_window)->getElement() == pElement)
            {
                //              internal_window->close();
                container_window->close();
            }
        }
    }
}

void TYMainWindow::editCurCalcul()
{
    if (_pProjetFrame->getProjet() && _pProjetFrame->getProjet()->getCurrentCalcul())
    {
        _pProjetFrame->getProjet()->getCurrentCalcul()->edit(this);
    }
}

void TYMainWindow::goCurCalcul()
{
    getTYApp()->getCalculManager()->launchCurrent();
}

void TYMainWindow::windowsMenuAboutToShow()
{
    _pWindowsMenu->clear();

    QAction* cascadeId = _pWindowsMenu->addAction(TR("id_menuitem_cascade"), _pWorkspace, SLOT(cascade()));
    QAction* tileId = _pWindowsMenu->addAction(TR("id_menuitem_tile"), _pWorkspace, SLOT(tile()));

    QList<QMdiSubWindow*> windows = _pWorkspace->subWindowList();

    if (windows.isEmpty())
    {
        cascadeId->setEnabled(false);
        tileId->setEnabled(false);
    }

    _pWindowsMenu->addSeparator();

    QMenu* viewMenu = createPopupMenu();
    viewMenu->setTitle(TR("id_menuitem_views"));
    _pWindowsMenu->addMenu(viewMenu);

    _pWindowsMenu->addSeparator();

    for (int i = 0; i < int(windows.count()); ++i)
    {
        QWidget* internal_window = windows.at(i)->widget();

        if (!internal_window) { break; } // Securite

        QAction* id = _pWindowsMenu->addAction(internal_window->windowTitle(),
                                               this, SLOT(windowsMenuActivated()));
        id->setData(i);
        id->setChecked(_pWorkspace->activeSubWindow() == windows.at(i));
    }
}

void TYMainWindow::windowsMenuActivated()
{
    QAction* action = (QAction*)sender();
    int id = action->data().toInt();

    QWidget* pWidget = _pWorkspace->subWindowList().at(id)->widget();

    if (pWidget)
    {
        pWidget->showNormal();
        pWidget->setFocus();

        if (QString(pWidget->metaObject()->className()).compare("TYSpectreManager") == 0)
        {
            _pHideOrShowSpectreMngrAction->setChecked(true);
        }
        else if (QString(pWidget->metaObject()->className()).compare("TYDataBaseFrame") == 0)
        {
            _pHideOrShowBiblioAction->setChecked(true);
        }
        else if (QString(pWidget->metaObject()->className()).compare("TYMaillageManager") == 0)
        {
            _pHideOrShowMaillageMngrAction->setChecked(true);
        }
    }
}

void TYMainWindow::setDefaultCameraMode()
{
    if (_pCurrentModeler != NULL)
    {
        _pCurrentModeler->setEditorModeToCamera();
    }
}

void TYMainWindow::subWindowActivated()
{
    QWidget* pW = NULL;
    QMdiSubWindow* subWindow = _pWorkspace->activeSubWindow();
    if (_pWorkspace->subWindowList().count() > 0 && subWindow)
    {
        pW = subWindow->widget();
    }

    _pPrintAction->setEnabled(false);

    // Toolbars modeler
    _pToolbarCalcul->activeButtons(false);
    _pToolbarInfra->activeButtons(false);
    _pToolbarTopo->activeButtons(false);
    _pSiteBtnGroup->disconnect();

    _pToolbarMachine->activeButtons(false);
    _pMachineBtnGroup->disconnect();

    _pToolbarFace->activeButtons(false);
    _pFaceBtnGroup->disconnect();

    _pToolbarBatiment->activeButtons(false);
    _pBatimentBtnGroup->disconnect();

    _pToolbarModeler->activeButtons(false);
    _pBatimentBtnGroup->disconnect();

    // TODO Check this work around : it seems we were emptying the BtnGroups here,
    // which is a priori doubtful
    /*
    _pToolbarModeler->removeFromButtonGroup(_pSiteBtnGroup);
    _pToolbarModeler->removeFromButtonGroup(_pMachineBtnGroup);
    _pToolbarModeler->removeFromButtonGroup(_pFaceBtnGroup);
    _pToolbarModeler->removeFromButtonGroup(_pBatimentBtnGroup);
    */

    connectDefaultActionManager(); // On commence par desactiver tous les boutons

    if (pW == NULL)
    {
        connectDefaultActionManager();
        return;
    }

    if (dynamic_cast<TYModelerFrame*>(pW) != nullptr)
    {
        TYModelerFrame* pModeler = (TYModelerFrame*) pW;

        connectActionManager(pModeler->getActionManager());

        _pPrintAction->setEnabled(true);

        QObject::connect(pW, SIGNAL(viewTypeChanged(int)), _pToolbarModeler, SLOT(viewTypeChanged(int)));

        // On desactive toutes les toolbars...
        _pToolbarCalcul->activeButtons(false);
        _pToolbarInfra->activeButtons(false);
        _pToolbarTopo->activeButtons(false);
        _pToolbarMachine->activeButtons(false);
        _pToolbarBatiment->activeButtons(false);
        _pToolbarFace->activeButtons(false);
        // ...Puis on active celles qui nous sont utiles

        if (QString(pW->metaObject()->className()).compare("TYSiteModelerFrame") == 0)
        {
            TYSiteModelerFrame* pS = (TYSiteModelerFrame*) pW;

            QObject::connect(_pSiteBtnGroup, SIGNAL(buttonClicked(int)), this, SLOT(saveCurSiteMode(int)));
            QObject::connect(_pSiteBtnGroup, SIGNAL(buttonClicked(int)), pS, SLOT(setEditorMode(int)));

            // On fait en sorte de n'activer la toolbar de calcul que pour le modeler pointant
            // sur le site racine ou le projet
            bool bActiveCalculToolBar = false;

            // On recupere le site node represente
            if (pS->getElement()->isA("TYProjet"))
            {
                bActiveCalculToolBar = true;
            }
            else
            {
                if (TYSiteNode::safeDownCast(pS->getElement())->getRoot())
                {
                    bActiveCalculToolBar = true;
                }
            }

            // Si le calcul est bloque, on ne peut modifier le  projet // DTn 20110915 : Meme comportement dans tous les cas
            if (_pProjetFrame->getProjet() && _pProjetFrame->getProjet()->getCurrentCalcul() &&
                (_pProjetFrame->getProjet()->getCurrentCalcul()->getState() == TYCalcul::Locked))
            {
                _pToolbarCalcul->activeButtons(bActiveCalculToolBar);
                _pToolbarInfra->activeButtons(true);
                _pToolbarTopo->activeButtons(true);
                _pToolbarModeler->activeButtons(true);
            }
            else
            {
                _pToolbarCalcul->activeButtons(bActiveCalculToolBar);
                _pToolbarInfra->activeButtons(true);
                _pToolbarTopo->activeButtons(true);
                _pToolbarModeler->activeButtons(true);
            }

            _pToolbarModeler->addToButtonGroup(_pSiteBtnGroup);

            pS->setEditorMode(_curSiteMode);
        }
        else if (QString(pW->metaObject()->className()).compare("TYMachineModelerFrame") == 0)
        {
            TYMachineModelerFrame* pM = (TYMachineModelerFrame*) pW;

            QObject::connect(_pMachineBtnGroup, SIGNAL(buttonClicked(int)), this, SLOT(saveCurMachineMode(int)));
            QObject::connect(_pMachineBtnGroup, SIGNAL(buttonClicked(int)), pM, SLOT(setEditorMode(int)));

            // Si le calcul est bloque, on ne peut modifier le  projet
            if (_pProjetFrame->getProjet() &&
                (_pProjetFrame->getProjet()->getCurrentCalcul()->getState() == TYCalcul::Locked)) // DTn : Neutralisation : 20110915
            {
                _pToolbarMachine->activeButtons(true);
                _pToolbarModeler->activeButtons(true);
            }
            else
            {
                _pToolbarMachine->activeButtons(true);
                _pToolbarModeler->activeButtons(true);
            }

            _pToolbarModeler->addToButtonGroup(_pMachineBtnGroup);

            pM->setEditorMode(_curMachineMode);
        }
        else if (QString(pW->metaObject()->className()).compare("TYBatimentModelerFrame") == 0)
        {
            TYBatimentModelerFrame* pB = (TYBatimentModelerFrame*) pW;

            QObject::connect(_pBatimentBtnGroup, SIGNAL(buttonClicked(int)), this, SLOT(saveCurBatimentMode(int)));
            QObject::connect(_pBatimentBtnGroup, SIGNAL(buttonClicked(int)), pB, SLOT(setEditorMode(int)));

            if (_pProjetFrame->getProjet() &&
                (_pProjetFrame->getProjet()->getCurrentCalcul()->getState() == TYCalcul::Locked)) // DTn : Neutralisation : 20110915
            {
                _pToolbarModeler->activeModif(false);
                _pToolbarBatiment->activeButtons(false);
            }
            else
            {
                _pToolbarModeler->activeButtons(true);
                _pToolbarBatiment->activeButtons(true);
            }

            _pToolbarModeler->addToButtonGroup(_pBatimentBtnGroup);

            pB->setEditorMode(_curBatimentMode);
        }
        else if (QString(pW->metaObject()->className()).compare("TYFaceModelerFrame") == 0)
        {
            TYFaceModelerFrame* pF = (TYFaceModelerFrame*) pW;

            QObject::disconnect(pF, SIGNAL(editorModeChanged(int)), this, SLOT(saveCurFaceMode(int)));
            QObject::connect(pF, SIGNAL(editorModeChanged(int)), this, SLOT(saveCurFaceMode(int)));

            QObject::connect(_pFaceBtnGroup, SIGNAL(buttonClicked(int)), this, SLOT(saveCurFaceMode(int)));
            QObject::connect(_pFaceBtnGroup, SIGNAL(buttonClicked(int)), pF, SLOT(setEditorMode(int)));

            if (_pProjetFrame->getProjet() &&
                (_pProjetFrame->getProjet()->getCurrentCalcul()->getState() == TYCalcul::Locked)) // DTn : Neutralisation : 20110915
            {
                _pToolbarModeler->activeModif(false);
                _pToolbarFace->activeButtons(false);
            }
            else
            {
                _pToolbarModeler->activeButtons(true);
                _pToolbarFace->activeButtons(true);
            }

            _pToolbarModeler->addToButtonGroup(_pFaceBtnGroup);

            pF->setEditorMode(_curFaceMode);
        }

        _pToolbarModeler->activeCameraButtons(pModeler->getCurrentView());

        pModeler->getView()->getRenderer()->updateDisplayList();

        _pCurrentModeler = pModeler;

    }
    else
    {
        connectDefaultActionManager();
        _pCurrentModeler = NULL;
    }
}

void TYMainWindow::saveCurSiteMode(int mode)
{
    _curSiteMode = mode;
    assert(_pSiteBtnGroup->button(mode)); // XXX This is a bug point : mode does not match a button.
    _pSiteBtnGroup->button(mode)->setChecked(true);
}

void TYMainWindow::saveCurMachineMode(int mode)
{
    _curMachineMode = mode;
    assert(_pMachineBtnGroup->button(mode));
    _pMachineBtnGroup->button(mode)->setChecked(true);
}

void TYMainWindow::saveCurBatimentMode(int mode)
{
    _curBatimentMode = mode;
    assert(_pBatimentBtnGroup->button(mode)); // XXX This is a bug point : mode does not match a button.
    _pBatimentBtnGroup->button(mode)->setChecked(true);
}

void TYMainWindow::saveCurFaceMode(int mode)
{
    _curFaceMode = mode;
    assert(_pFaceBtnGroup->button(mode));
    _pFaceBtnGroup->button(mode)->setChecked(true);
}

void TYMainWindow::updateUndoRedo(bool undoAvailable, bool redoAvailable,
                                  const QString& undoCmd, const QString& redoCmd)
{
    _pUndoAction->setEnabled(undoAvailable);
    _pRedoAction->setEnabled(redoAvailable);

    if (!undoCmd.isEmpty())
    {
        _pUndoAction->setText(TR("id_menuitem_undo_name").arg(undoCmd));
    }
    else
    {
        _pUndoAction->setText(TR("id_menuitem_undo_na"));
    }

    if (!redoCmd.isEmpty())
    {
        _pRedoAction->setText(TR("id_menuitem_redo_name").arg(redoCmd));
    }
    else
    {
        _pRedoAction->setText(TR("id_menuitem_redo_na"));
    }

    _pUndoAction->setToolTip(_pUndoAction->text());
    _pRedoAction->setToolTip(_pRedoAction->text());
}

void TYMainWindow::setCurProjet(LPTYProjet pProjet)
{
    _pProjetFrame->setProjet(pProjet);

    //if (pProjet) {
    //  getTYApp()->setCurSiteNode(pProjet->getSite());
    //} else {
    //  getTYApp()->setCurSiteNode(NULL);
    //}

    _pCloseAction->setEnabled(pProjet ? true : false);
    //  updateCurCalcul();
}

void TYMainWindow::setCurSiteNode(LPTYSiteNode pSiteNode)
{
    _pSiteFrame->setSiteNodeRoot(pSiteNode);
    _pCloseAction->setEnabled(pSiteNode ? true : false);
}

void TYMainWindow::updateCurCalcul()
{
    if (_pProjetFrame->getProjet() && _pProjetFrame->getProjet()->getCurrentCalcul())
    {
        if (!_pProjetFrame->getProjet()->getStatusSolver()) // Le solveur du calcul n'est pas disponible
        {
            if (QMessageBox::warning(this, "Tympan", TR("id_msg_solver_out"), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
            {
                // L'utilisateur accepte d'utiliser le solveur par defaut
                _pProjetFrame->getProjet()->getCurrentCalcul()->setSolverId(OGenID(QString(DEFAULT_SOLVER_UUID)));
                _pProjetFrame->getProjet()->getCurrentCalcul()->setState(TYCalcul::Actif);
                _pProjetFrame->getProjet()->setStatusSolver(true);   // On est ok sur le solveur
            }
            else
            {
                // Le calcul est bloque. Si l'utilisateur le debloque, lancer le calcul appelera le solveur courant
                _pProjetFrame->getProjet()->getCurrentCalcul()->setState(TYCalcul::Locked);
            }
        }

    }

    _pGoCurCalculAction->setEnabled(true);
    _pEditCurCalculAction->setEnabled(true);
}

void TYMainWindow::createNew()
{
    close(); // Fermeture de l'lement precedemment actif
    TYCreateElementDialog* pDlg = new TYCreateElementDialog(this);
    pDlg->exec();
    TYElement::setIsSavedOk(true);
}

void TYMainWindow::createNewProjet()
{
    close(); // Fermeture de l'lement precedemment actif
    TYCreateElementDialog* pDlg = new TYCreateElementDialog(this);
    pDlg->createElement("TYProjet");
    TYElement::setIsSavedOk(true);

    // CLM-NT33 - Ouverture automatique du modeler projet
    // DTn 20111003 Plus la peine : ouverture assuree par le openElementDialog appele par le createElementDialog
    //  makeProjetModeler(_pProjetFrame->getProjet());
}

void TYMainWindow::createNewSite()
{
    close(); // Fermeture de l'lement precedemment actif
    TYCreateElementDialog* pDlg = new TYCreateElementDialog(this);
    pDlg->createElement("TYSiteNode");
    TYElement::setIsSavedOk(true);
}

void TYMainWindow::createNewBatiment()
{
    close(); // Fermeture de l'lement precedemment actif
    TYCreateElementDialog* pDlg = new TYCreateElementDialog(this);
    pDlg->createElement("TYBatiment");
    TYElement::setIsSavedOk(true);
}

void TYMainWindow::createNewMachine()
{
    close(); // Fermeture de l'lement precedemment actif
    TYCreateElementDialog* pDlg = new TYCreateElementDialog(this);
    pDlg->createElement("TYMachine");
    TYElement::setIsSavedOk(true);
}

void TYMainWindow::open()
{
    close(); // Ferme l'element en cours en demandant la sauvegarde si necessaire
    if (!_closeAndQuit) // L'utilisateur ne souhaite pas fermer son projet
    {
        _closeAndQuit = true;
        return;
    }

    TYOpenElementDialog* pDlg = new TYOpenElementDialog(this);
    pDlg->setMultiSelect(false); // Un seul element peut etre selectionne
    pDlg->setDirName(getTYApp()->getCurrentDirName());
    pDlg->open();

    if (pDlg->result() == QDialog::Accepted) // si le choix du fichier XML est OK
    {
        // CLM-NT33 - Choix automatique
        if (pDlg->getAvailableElements().size() == 1)
        {
            pDlg->openElement(pDlg->getAvailableElements()[0]);
        }
        else
        {
            pDlg->exec();
        }

        TYApplication::setOverrideCursor(Qt::WaitCursor);

        QString strExtend = "";
        if (pDlg->isFileReadOnly())
        {
            strExtend = TR("id_status_file_ro");
            _pSaveAction->setEnabled(false); // Desactivation de la fnction de sauvegarde
        }

        QString fileName = pDlg->getFileName(); // Nom du fichier lu
        setWindowTitle(TR("id_caption") + "-" + TY_CURRENT_RELEASE_ + " (" + TY_PRODUCT_PLATFORM_ + ") : " + fileName + ' ' + strExtend);

        TYApplication::setCurrentFileName(fileName);
        getTYApp()->setCurrentDirName(pDlg->getDirName());

        TYElement::setIsSavedOk(false); // Tous les elements sont up to date apres le chargement

        TYApplication::restoreOverrideCursor();
    }
}

void TYMainWindow::open(vector<LPTYElement>& tabElem, const bool& bRegenerate /*=false*/)
{
    TYElement::setRegenerateID(bRegenerate);

    TYOpenElementDialog* pDlg = new TYOpenElementDialog(this);
    pDlg->setMultiSelect(true); // Plusieurs elements peuvent etre selectionnes
    pDlg->setDirName(getTYApp()->getCurrentDirName());
    pDlg->open();

    if (pDlg->result() == QDialog::Accepted)
    {
        if (pDlg->getAvailableElements().size() == 1)
        {
            tabElem.push_back(pDlg->getAvailableElements()[0]);
        }
        else
        {
            pDlg->exec();
            tabElem = pDlg->getTabElem();
        }
    }

    TYElement::setRegenerateID(false);
}

void TYMainWindow::close()
{
    if (TYElement::getIsSavedOk() == true) // Si un element a ete modifie, il faut sauvegarder
    {
        switch (QMessageBox::warning(this, "Attention", TR("id_file_not_saved"), QMessageBox::Yes, QMessageBox::No))
        {
            case QMessageBox::Yes:
                if (_pSaveAction->isEnabled())
                {
                    _closeAndQuit = save();
                }
                else
                {
                    _closeAndQuit = saveAs();
                }

                break;
            case QMessageBox::No:
                _closeAndQuit = true;
                break;
                //case QMessageBox::Cancel:
                //    _closeAndQuit = false;
                //    break;
        }
    }

    if (_closeAndQuit)
    {
        // Fermeture de toutes les fenetres
        QList<QMdiSubWindow*> windows = _pWorkspace->subWindowList();

        for (int i = 0; i < int(windows.count()); ++i)
        {
            windows.at(i)->close();
        }

        // On reinitialise le projet et le site courant
        getTYApp()->setCurProjet(NULL);
        getTYApp()->setCurSiteNode(NULL);
        // Par securite, on inhibe le calcul courant connu du calculManager (pourrait expliquer des plantages ...)
        getTYApp()->getCalculManager()->setCurrent(NULL);

        // On efface le nom de fichier en cours
        QString fileName = "";
        updateCurrentFileName(fileName);

        _pSaveAction->setEnabled(true); // Plus de fichier ouvert, remise en etat de save

        // plus rien en memoire (theoriquement) donc rien a sauvegarder
        TYElement::setIsSavedOk(false);
    }
}

void TYMainWindow::quitte()
{
    _closeAndQuit = true; // A priori

    // Ferme le projet courant et demande si necessaire la sauvegarde du fichier.
    close();

    // Ferme l'application
    if (_closeAndQuit)
    {
        // Sauvegarde des settings
        saveSettings(QDir::convertSeparators(getTYApp()->getSettingsDir() + "/Settings" + TY_PRODUCT_VERSION_));
        // Fermeture de toutes les fenetres
        getTYApp()->closeAllWindows();
    }
}

TYElement* TYMainWindow::elementToSave()
{
    TYElement* pElement = NULL;


    if (_pProjetFrame->getProjet()) // Si on est dans un projet, on le sauve (avec son site)
    {
        pElement = (TYElement*) _pProjetFrame->getProjet();
    }
    else if (_pSiteFrame->getSiteNodeRoot()) // On sauve que le site si pas de projet mais un site
    {
        pElement = (TYElement*) _pSiteFrame->getSiteNodeRoot();
    }
    else // Sinon  on sauve ce qu'on peut, l'element actif
    {
        QWidget* pW = this->focusWidget();
        if (pW && (dynamic_cast<TYModelerFrame*>(pW) != nullptr))
        {
            pElement = ((TYModelerFrame*) pW)->getElement();

            if (pElement && (dynamic_cast<TYAcousticVolume*>(pElement) != nullptr))
                        { return pElement; }

            // Recherche d'un parent de type "acousticVolume"
            TYElement* pParent = pElement->getParent();
            while (pParent && (dynamic_cast<TYAcousticVolumeNode*>(pParent) == nullptr))
            {
                pParent = pParent->getParent();
            };

            if (pParent && (dynamic_cast<TYAcousticVolumeNode*>(pParent) != nullptr))
                    { return pParent; }
        }
    }

    return pElement;
}

bool TYMainWindow::save()
{
    bool bRet = false;

    TYElement* pElement = elementToSave();

    if (!pElement)
    {
        QMessageBox::warning(this, "Tympan", TR("id_nothingtoexport"));
        return false;
    }

    QString dirName;
    QString fileName = getFileName("XML (*.xml)", dirName); // Recuperation du nom du fichier courant

    bRet = save(dirName, fileName, pElement); // Sauvegarde effective de l'objet

    if (bRet)
    {
        QMessageBox::information(this, "Tympan", TR("id_export_ok").arg(fileName));

        updateCurrentAppFile(dirName, fileName);

        // Indique que les TYElement sont ok (rien a sauvegarder)
        TYElement::setIsSavedOk(false);
    }
    else
    {
        QMessageBox::warning(this, "Tympan", TR("id_export_failed").arg(fileName));
    }

    return bRet;
}

bool TYMainWindow::save(QString dirName, QString& fileName, TYElement* pElement)
{
    // Controle des parametres
    if (!pElement || fileName.isEmpty()) { return false; }
    if (dirName.isEmpty())
    {
        dirName = getTYApp()->getCurrentDirName();
    }

    bool bRet = false;

    formatFileName(fileName); // Ajoute l'extension si necessaire

    // Creation du fichier XML
    TYXMLManager xmlManager;

    QString version(TY_CURRENT_RELEASE_);
    QString tiret(" - ");
    QString licencie(TY_CUSTOMER_);
    QString licenceNumber(TY_LICENCE_NUMBER_);

    QString messageVersion = version + tiret + licencie + tiret + licenceNumber;

    TYXMLManager::setSavedFileName(fileName);

    xmlManager.createDoc(TY_PRODUCT_XMLTAG_, messageVersion); //TY_PRODUCT_VERSION_);
    xmlManager.addElement(pElement);

    if (xmlManager.save(fileName) == 0) { return true; }

    return bRet;
}

bool TYMainWindow::saveAs()
{
    bool bRet = false;

    TYElement* pElement = elementToSave();

    if (!pElement)
    {
        QMessageBox::warning(this, "Tympan", TR("id_nothingtoexport"));
        return false;
    }

    QString dirName;
    QString fileName = getFileName("XML (*.xml)", dirName, true);


    if (testFile(fileName))
    {
        bRet = save(dirName, fileName, pElement); // On fait la sauvegarde en ecrasant l'ancienne version

        if (bRet)
        {
            QMessageBox::information(this, "Tympan", TR("id_export_ok").arg(fileName));

            updateCurrentAppFile(dirName, fileName);

            // Indique que les TYElement sont ok (rien a sauvegarder)
            TYElement::setIsSavedOk(false);
        }
        else
        {
            QMessageBox::warning(this, "Tympan", TR("id_export_failed").arg(fileName));
        }
    }

    return bRet;
}

bool TYMainWindow::saveAsNoResult()
{
    bool bRet = false;

    TYProjet* pProjet = getTYApp()->getCurProjet();

    if (!pProjet)
    {
        QMessageBox::warning(this, "Tympan", TR("id_nothingtoexport"));
        return false;
    }

    QString dirName;
    QString fileName = getFileName("XML (*.xml)", dirName, true);

    TYProjet::gSaveValues = false;
    if (testFile(fileName))
    {
        bRet = save(dirName, fileName, pProjet); // On fait la sauvegarde en ecrasant l'ancienne version

        if (bRet)
        {
            QMessageBox::information(this, "Tympan", TR("id_export_ok").arg(fileName));
        }
        else
        {
            QMessageBox::warning(this, "Tympan", TR("id_export_failed").arg(fileName));
        }
    }

    TYProjet::gSaveValues = true;

    return bRet;
}

bool TYMainWindow::saveAs(LPTYElement pElement)
{
    bool bRet = false;

    if (pElement == NULL) { return bRet; }

    QString dirName;
    QString fileName = getFileName("XML (*.xml)", dirName, true);

    if (testFile(fileName))
    {
        bRet = save(dirName, fileName, pElement);

        if (bRet)
        {
            QMessageBox::information(this, "Tympan", TR("id_export_ok").arg(fileName));
        }
        else
        {
            QMessageBox::warning(this, "Tympan", TR("id_export_failed").arg(fileName));
        }
    }

    return bRet;
}

void TYMainWindow::updateCurrentAppFile(const QString& dirName, const QString& fileName)
{
    getTYApp()->setCurrentDirName(dirName);
    getTYApp()->setCurrentFileName(fileName);

    setWindowTitle(TR("id_caption") + "-" + TY_CURRENT_RELEASE_ + " (" + TY_PRODUCT_PLATFORM_ + ") : " + fileName);
}


bool TYMainWindow::backupFile(const QString& fileName)
{
    // Creation du nom du fichier
    QFileInfo fi(fileName);

    QString nomFichier = fi.absolutePath() + '/' + fi.baseName() + ".bak";

    // Ouverture du fichier d'origine
    QFile fileToSave(fileName);
    bool bOpen = fileToSave.open(QIODevice::ReadOnly);

    if (!bOpen) { return false; }

    // Ouverture du fichier backup
    QFile backupFile(nomFichier);
    bOpen = backupFile.open(QIODevice::WriteOnly);

    if (!bOpen)
    {
        fileToSave.close();
        return false;
    }

    // Copie en bloc de l'un dans l'autre
    backupFile.write(fileToSave.readAll());

    // Fermeture des fichier
    fileToSave.close();
    backupFile.close();

    return true;
}

void TYMainWindow::showPluginManager()
{
    TYPluginDialog* pDlg = new TYPluginDialog(this);
    pDlg->exec();

    // Mise a jour du solveur pour le calcul courant si calcul pas bloque
    if (_pProjetFrame->getProjet())
        if (_pProjetFrame->getProjet()->getCurrentCalcul())
            if (_pProjetFrame->getProjet()->getCurrentCalcul()->getState() == TYCalcul::Actif)
            {
                _pProjetFrame->getProjet()->getCurrentCalcul()->setSolverId(pDlg->current_solver);
            }
}

bool TYMainWindow::testFile(const QString& fileName)
{
    // Verification de l'etat du fichier
    QFileInfo fi(fileName);
    // Si le fichier existe , on demande confirmation a l'utilisateur
    if (fi.exists())
    {
        // On ecrase le fichier, puis on sauve
        QFile* file = new QFile(fileName);
        switch (QMessageBox::warning(this, "Attention", TR("id_file_exist"), QMessageBox::Yes, QMessageBox::No))
        {
            case QMessageBox::Yes:
                if (!file->remove())
                {
                    QMessageBox::warning(this, "Attention", TR("id_file_cannot_be_clear"));
                    return false;
                }

                break;
            case QMessageBox::No:
                return false; // On sort sans sauvegarder
                break;
        }

        delete file;
        file = NULL;
    }

    return true;
}

QString TYMainWindow::getFileName(const QString& filter, QString& dirName, const bool& forceNewName /*=false*/)
{
    QString fileName = getTYApp()->getCurrentFileName(); // Recuperation du nom du fichier courant

    if (!forceNewName && !fileName.isEmpty())
    {
        return fileName;
    }

    // Recuperation de la terminaison de fichier a creer
    short index = filter.indexOf(".");
    QString end = filter.mid(index, 4);

    // NOUVELLE VERSION POUR RECUPERER LE REPERTOIRE
    dirName = getTYApp()->getCurrentDirName();

    QFileDialog* pDialog = new QFileDialog(parentWidget(), "Choose a file" , dirName,
                                           filter);
    pDialog->setFileMode(QFileDialog::AnyFile);
    pDialog->setAcceptMode(QFileDialog::AcceptSave);
    pDialog->exec();

    QStringList list = pDialog->selectedFiles();
    if ((pDialog->result() == QDialog::Accepted) && (!list.isEmpty()))
    {
        fileName = list.first();

        QDir dir = pDialog->directory();
        dirName = dir.absolutePath();

        pDialog = NULL;


        if (!fileName.endsWith(end))
        {
            fileName += end;
        }
    }
    else
    {
        fileName = "";
    }

    return fileName;
}

void TYMainWindow::importFromBD_TOPO()
{
    // TYPreferenceDialog * pPrfDlg = new TYPreferenceDialog(this);
    // pPrfDlg->exec();
    TYOpenElementDialog* pDlg = new TYOpenElementDialog(this);
    pDlg->openBDTopo();
    pDlg->exec();
}

void TYMainWindow::print()
{
    QWidget* pW = this->focusWidget();

    if (pW && (dynamic_cast<TYModelerFrame*>(pW) != nullptr))
    {
        ((TYModelerFrame*) pW)->print();
    }
}

void TYMainWindow::undo()
{
    emit onUndo();
}

void TYMainWindow::redo()
{
    emit onRedo();
}

void TYMainWindow::cut()
{
    about();
}

void TYMainWindow::copy()
{
    about();
}

void TYMainWindow::paste()
{
    about();
}

void TYMainWindow::showSpectreManager(bool show)
{
}

void TYMainWindow::showMaillageManager(bool show)
{
}


void TYMainWindow::showBiblio(bool show)
{
}

void TYMainWindow::showPreferenceDialog()
{
    TYPreferenceDialog* pDlg = new TYPreferenceDialog(this);
    pDlg->exec();
}

void TYMainWindow::showOutput(bool show)
{
    if (show)
    {
        _pOutputDockWnd->show();
    }
    else
    {
        _pOutputDockWnd->hide();
    }

    _pHideOrShowOutputAction->setChecked(show);
}

void TYMainWindow::launchImporter()
{
    QProcess* pImporterLauncher = new QProcess(this);

    // On travail dans le repertoire utilisateur
    pImporterLauncher->setWorkingDirectory(TYApplication::tympanUserDir());

    // Le nom de l'executable
    //pImporterLauncher->addArgument("Importer");

    // Lancement
    pImporterLauncher->start("Importer");

    if (pImporterLauncher->state() == QProcess::Running)
    {
        writeOutputMsg(TR("id_msg_importer_ok"));
    }
    else
    {
        writeOutputMsg(TR("id_msg_importer_fail"));
    }
}

void TYMainWindow::closeEvent(QCloseEvent* pEvent)
{
    _closeAndQuit = true; // A priori

    // Avant tout, on sauvegarde le fichier (si necessaire)
    close();

    if (!_closeAndQuit) { return; } // Annulation de l'action

    // Main window
    TYPreferenceManager::saveGeometryToPreferences(metaObject()->className(), this);

    // Dock windows
    QObjectList objectsList = children();//dockWindows();
    QWidget* pDockWnd = NULL;
    QObject* pObject = NULL;

    for (int _i = 0; _i < objectsList.size(); _i++)
    {
        pObject = objectsList[_i];
        if ((pObject->objectName() == "ProjetDockWnd")
            || (pObject->objectName() == "SiteDockWnd")
            || (pObject->objectName() == "OutputDockWnd")
            || (pObject->metaObject()->className() == "QToolBar")
            || (dynamic_cast<QToolBar*>(pObject) != nullptr))
        {

            pDockWnd = (QWidget*)pObject;
            QString key = pDockWnd->objectName();

            // Geometry
            TYPreferenceManager::saveGeometryToPreferences(key, pDockWnd);
        }
    }

    // Sauvegarde des settings
    saveSettings(QDir::convertSeparators(getTYApp()->getSettingsDir() + "/Settings" + TY_PRODUCT_VERSION_));

    QMainWindow::closeEvent(pEvent);
}

void TYMainWindow::updateCurrentFileName(const QString& fileName)
{
    // Informe l'application du nom du fichier courant
    TYApplication::setCurrentFileName(fileName);

    // Change le titre de la fenetre pour afficher le nom du fichier
    setWindowTitle(TR("id_caption") + "-" + TY_CURRENT_RELEASE_ + " (" + TY_PRODUCT_PLATFORM_ + ") : " + fileName);
}

// CLM-NT33 : Raffraichissement du nom de fichier en vue modeleur
void TYMainWindow::refreshWindowTitle()
{
    // Change le titre de la fenetre pour afficher le nom du fichier
    setWindowTitle(TR("id_caption") + "-" + TY_CURRENT_RELEASE_ + " (" + TY_PRODUCT_PLATFORM_ + ") : " + TYApplication::getCurrentFileName());
}
// CLM-NT33 : Fin

