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
 * \file TYMainWindow.h
 * \brief Fenetre principale de l'application Tympan (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_MAIN_WINDOW__
#define __TY_MAIN_WINDOW__

#include <qmainwindow.h>
//Added by qt3to4:
#include <QCloseEvent>
#include <QMenu>

#include "Tympan/models/business/TYProjet.h"
#include "Tympan/gui/app/TYAppDefines.h"
#include "TYMainWindow.h"

class QAction;
class QToolBar;
class QMenu;
class QDockWidget;
class QToolButton;
class QButtonGroup;
class QTextBrowser;
// CLM-NT35: Gestion MDI avec QT4.7
#ifdef USE_QMDIAREA
class QMdiArea;
class QMdiSubWindow;
#else
class QWorkspace;
#endif
// CLM-NT35 End


class TYSpectreManager;
class TYMaillageManager;

/**
 * \class TYMainWindow
 * \brief Fenetre principale de l'application Tympan
 */
class TYMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * Constructeur par defaut.
     */
    TYMainWindow();
    /**
     * Destructeur.
     */
    virtual ~TYMainWindow();

    // CLM-NT35: Gestion MDI avec QT4.7
#ifdef USE_QMDIAREA
    QMdiArea* getWorkspace() { return _pWorkspace; }
#else
    QWorkspace* getWorkspace() { return _pWorkspace; }
#endif
    // CLM-NT35 End

    TYModelerFrame* getCurrentModeler() { return _pCurrentModeler; }

    TYProjetFrame* getProjetFrame() { return _pProjetFrame; }
    TYSiteFrame* getSiteFrame() { return _pSiteFrame; }
    TYOutputFrame* getOutputFrame() { return _pOutputFrame; }

    TYFaceToolbar* getFaceToolbar() { return _pToolbarFace; }
    TYModelerToolbar* getModelerToolbar() { return _pToolbarModeler; }

    void updateCurrentFileName(const QString& fileName);

    /// Retourne l'etat de la requete de sauvegarde
    bool getSaveStatus() { return _closeAndQuit; }
    void initSaveStatus() { _closeAndQuit = true; }

    virtual QMenu* createPopupMenu();

public slots:
    /**
     * Raffraichit le titre de la fenetre
     */
    void refreshWindowTitle();
    /**
     * Chargement des settings.
     */
    bool loadSettings(const QString& fileName);
    /**
     * Sauvegarde des settings.
     */
    bool saveSettings(const QString& fileName);

    /**
     * Set du projet courant.
     */
    void setCurProjet(LPTYProjet pProjet);
    /**
     * Set du site node courant.
     */
    void setCurSiteNode(LPTYSiteNode pSiteNode);
    /**
     * Met a jour le calcul courant.
     */
    void updateCurCalcul();

    /**
     * Affiche la boite de dialogue de creation d'un nouvel element.
     */
    void createNew();
    /**
     * Creer un nouveau Projet.
     */
    void createNewProjet();
    /**
     * Creer un nouveau Site.
     */
    void createNewSite();
    /**
     * Creer un nouveau Bâtiment.
     */
    void createNewBatiment();
    /**
     * Creer une nouvelle Machine.
     */
    void createNewMachine();

    /**
     * Ouvre un fichier XML
     */
    void open();

    /**
     * Ouvre un fichier XML (utilise pour faire l'import de composants)
     */
    void open(std::vector<LPTYElement>& tabElem, const bool& bRegenerate = false);

    /**
     *
     */
    bool save(QString dirName, QString& fileName, TYElement* pElement);

    /**
     *
     */
    void formatFileName(QString& fileName) { if (!fileName.endsWith(".xml")) { fileName += ".xml"; } }

    /**
     *
     */
    void updateCurrentAppFile(const QString& dirName, const QString& fileName);

    /**
     * Enregistre l'element de la fenetre activee.
     */
    bool save();

    /**
     * Enregistre le projet (ou l'element actif) sous un autre nom
     */
    bool saveAs();

    /**
     * Enregistre le projet (ou l'element actif) sous un autre nom sans les resultats de calcul
     */
    bool saveAsNoResult();

    /**
     * Enregistre l'element passe en parametre
     */
    bool saveAs(LPTYElement pElement);

    /**
     *
     */
    TYElement* elementToSave();

    /**
     * Cree une copie du fichier passe en parametre
     */
    bool backupFile(const QString& fileName);

    /**
     * Affiche le gestionnaire de plug-ins
     */
    void showPluginManager();

    /**
     * Handle BDTOPO DXF files format.
     */
    void importFromBD_TOPO();

    /**
     * Ferme le projet courant ou le site courant.
     */
    void close();

    /**
     * Quitte l'application
     */
    void quitte();

    /**
     * Affiche la boite de dialogue d'impression.
     */
    void print();

    /**
     * Annule la derniere action.
     */
    void undo();
    /**
     * Effectue a nouveau la derniere action annulee.
     */
    void redo();

    /**
     * Couper-Copier-Coller.
     */
    void cut();
    /**
     * Couper-Copier-Coller.
     */
    void copy();
    /**
     * Couper-Copier-Coller.
     */
    void paste();

    /**
     * Affiche le navigateur d'aide.
     */
    void showHelp();

    /**
     * Affiche la fenetre d'information sur Tympan.
     */
    void about();

    /**
     * Affiche ou cache le gestionnaire de spectre.
     */
    void showSpectreManager(bool show);
    /**
     * Affiche ou cache le gestionnaire de maillage.
     */
    void showMaillageManager(bool show);
    /**
     * Affiche ou cache a bibliotheque.
     */
    void showBiblio(bool show);
    /**
     * Affiche la boite de dialogue des preferences.
     */
    void showPreferenceDialog();
    /**
     * Affiche la fenetre de sortie des messages.
     */
    void showOutput(bool show);

    /**
     * Ouvre l'Importer (conversion Tympan II vers Tympan 3)
     * dans un nouveau process.
     */
    void launchImporter();

    /**
     * Creation d'un modeleur a partir d'un type
     */
    void makeModeler(TYElement* pElt);

    /**
     * Instancie une novuelle fenetre Batiment Modeler.
     */
    void makeBatimentModeler(LPTYBatiment pBatiment = NULL);
    /**
     * Instancie une novuelle fenetre Machine Modeler.
     */
    void makeMachineModeler(LPTYMachine pMachine = NULL);
    /**
     * Instancie une novuelle fenetre Site Modeler.
     */
    void makeSiteModeler(LPTYSiteNode pSite = NULL);
    /**
     * Instancie une novuelle fenetre Site Modeler.
     */
    void makeProjetModeler(LPTYProjet pProjet = NULL);

    /**
     * Connecte un ActionManager a cette MainWindow.
     * Ainsi les actions undo et redo seront branchees a ce nouvel
     * ActionManager, et le precedent sera deconnecte.
     *
     * @param pActionManager L'ActionManager a connecter.
     */
    void connectActionManager(TYActionManager* pActionManager);
    /**
     * Connecte l'ActionManager de l'Application a cette MainWindow.
     */
    void connectDefaultActionManager();

    /**
     * Mets a jour les vues graphiques des modelers ouverts.
     *
     * @param clipping Pour mettre a jour le clipping range de la camera.
     * @param axesAndGrid Pour mettre a jour les axes et la grille.
     */
    void updateModelers(bool clipping = true, bool axesAndGrid = true, bool displayList = true);
    /**
     * Mets a jour la structure graphique de l'element associe aux modelers
     * ouverts.
     *
     * @param force Force la mise a jour meme si l'element n'est pas marque
     *              comme modifie.
     */
    void updateModelersElementGraphic(bool force = false);

    /**
     * Ferme le modeleur d'un element
     * @param pElement Objet dont le modeleur est ouvert
     */
    void closeModeler(const TYElement* pElement);

    /**
     * Affiche la widget du calcul courant.
     */
    void editCurCalcul();
    /**
     * Lance le calcul courant.
     */
    void goCurCalcul();

    void reloadSlots();

    /**
     * Repositionne l'interface en mode camera par defaut
     * (utilise automatiquement apres les creations d'elements)
     */
    void setDefaultCameraMode();


private slots:
    /**
     * Gere l'affichage des fenetres.
     */
    void windowsMenuAboutToShow();
    /**
     * Gere l'affichage des fenetres.
     */
    void windowsMenuActivated();
    /**
     * Appeller lorsque une fenetre est actrivee.
     */
    void subWindowActivated();

    /**
     * Pour conserver le mode courant des fenetres SiteModeler.
     */
    void saveCurSiteMode(int mode);
    /**
     * Pour conserver le mode courant des fenetres MachineModeler.
     */
    void saveCurMachineMode(int mode);
    /**
     * Pour conserver le mode courant des fenetres BatimentModeler.
     */
    void saveCurBatimentMode(int mode);
    /**
     * Pour conserver le mode courant des fenetres FaceModeler.
     */
    void saveCurFaceMode(int mode);

    /**
     * Mets a jour les menus et boutons d'undo et redo.
     */
    void updateUndoRedo(bool undoAvailable, bool redoAvailable,
                        const QString& undoCmd, const QString& redoCmd);

    void notifyAltimetryOutdated();


signals:
    /**
     * Signal qu'un undo est demande.
     */
    void onUndo();
    /**
     * Signal qu'un redo est demande.
     */
    void onRedo();


protected:
    virtual void closeEvent(QCloseEvent* pEvent);

    /// Teste l'existence et la possibilite d'ecrire le fichier
    bool testFile(const QString& fileName);

    /// Produit un nom de fichier valide (ou rien !)
    QString getFileName(const QString& filter, QString& dirName, const bool& forceNewName = false);


protected:
    ///Workspace pour la gestion MDI.
    // CLM-NT35: Gestion MDI avec QT4.7
#ifdef USE_QMDIAREA
    QMdiArea* _pWorkspace;
#else
    QWorkspace* _pWorkspace;
#endif
    // CLM-NT35 End

    ///Modeler en cours d'affichage
    TYModelerFrame* _pCurrentModeler;

    ///Action pour creer un nouvel element.
    QAction* _pCreateNewAction;
    ///Action pour ouvrir le gestionnaire de bibliotheques.
    QAction* _pOpenAction;
    ///Action pour fermer le projet ou site courant.
    QAction* _pCloseAction;

    ///Action pour sauvegarder un element.
    QAction* _pSaveAction;

    /// Action pour sauvegarder un element sous un nom specifique
    QAction* _pSaveAsAction;

    /// Action pour sauvegarder un element sous un nom specifique sans les resultats de calcul
    QAction* _pSaveAsNoResultAction;

    /// Action pour afficher le gestionnaire de plug-ins
    QAction* _pShowPluginManagerAction;

    ///Action pour sauvegarder tous les elements.
    QAction* _pSaveAllAction;

    ///Action pour importer un element depuis un fichier XML.
    QAction* _pImportXMLAction;
    ///Action pour exporter l'element du modeler actif en XML.
    QAction* _pExportXMLAction;

    /// Action pour importer un element depuis un fichier DXF BD-TOPO.
    QAction* _pImportDXFBdTopoAction;

    ///Action pour imprimer.
    QAction* _pPrintAction;

    ///Action pour realiser un undo.
    QAction* _pUndoAction;
    ///Action pour realiser un redo.
    QAction* _pRedoAction;

    ///Action pour realiser un couper.
    QAction* _pCutAction;
    ///Action pour realiser un copier.
    QAction* _pCopyAction;
    ///Action pour realiser un coller.
    QAction* _pPasteAction;

    ///Affiche ou cache le Spectre Manager.
    QAction* _pHideOrShowSpectreMngrAction;
    ///Affiche ou cache le Maillage Manager.
    QAction* _pHideOrShowMaillageMngrAction;
    ///Affiche ou cache la Bibliotheque d'elements.
    QAction* _pHideOrShowBiblioAction;
    ///Affiche ou cache la fenetre de sortie des messages.
    QAction* _pHideOrShowOutputAction;

    ///Action pour editer les preferences du calcul courant.
    QAction* _pEditCurCalculAction;
    ///Action pour lancer le calcul courant.
    QAction* _pGoCurCalculAction;

    ///Toolbar principale.
    QToolBar* _pMainToolbar;

    ///Toolbar pour la gestion des fenetres.
    QToolBar* _pWindowToolbar;

    ///Toolbar pour les calculs acoustiques.
    QToolBar* _pCalculAcousticToolbar;

    ///Toolbar generale pour les modelers.
    TYModelerToolbar* _pToolbarModeler;

    ///Button group pour les toolbars site.
    QButtonGroup* _pSiteBtnGroup;
    ///Conserve le mode courant des fenetres SiteModeler.
    int _curSiteMode;
    ///Toolbar topographie.
    TYTopoToolbar* _pToolbarTopo;
    ///Toolbar infrastructure.
    TYInfraToolbar* _pToolbarInfra;
    ///Toolbar calcul.
    TYCalculToolbar* _pToolbarCalcul;

    ///Button group pour la toolbar machine.
    QButtonGroup* _pMachineBtnGroup;
    ///Conserve le mode courant des fenetres MachineModeler.
    int _curMachineMode;
    ///Toolbar machine.
    TYMachineToolbar* _pToolbarMachine;

    ///Button group pour la toolbar face.
    QButtonGroup* _pFaceBtnGroup;
    ///Conserve le mode courant des fenetres FaceModeler.
    int _curFaceMode;
    ///Toolbar machine.
    TYFaceToolbar* _pToolbarFace;

    ///Button group pour la toolbar batiment.
    QButtonGroup* _pBatimentBtnGroup;
    ///Conserve le mode courant des fenetres BatimentModeler.
    int _curBatimentMode;
    ///Toolbar batiment.
    TYBatimentToolbar* _pToolbarBatiment;

    ///Frame pour la gestion de projets.
    TYProjetFrame* _pProjetFrame;
    ///Fenetre dockable pour la gestion de projets.
    QDockWidget* _pProjetDockWnd;

    ///Frame pour la gestion de sites.
    TYSiteFrame* _pSiteFrame;
    ///Fenetre dockable pour la gestion de sites.
    QDockWidget* _pSiteDockWnd;

    ///Frame pour les messages de retour.
    TYOutputFrame* _pOutputFrame;
    ///Fenetre dockable pour les messages de retour.
    QDockWidget* _pOutputDockWnd;

    ///Gestionnaire de spectres.
    //  TYSpectreManager * _pSpectreManager;

    ///Gestionnaire de maillages.
    //  TYMaillageManager * _pMaillageManager;
    ///Modeler de maillage pour le gestionnaire de maillage.
    //TYMaillageModelerFrame * _pMaillageModeler;//az--

    ///Menu de gestion des fenetres.
    QMenu* _pWindowsMenu;

    ///Navigateur d'aide.
    QTextBrowser* _pHelpBrowser;

    /// Indicateur d'annulation de commande de sortie
    bool _closeAndQuit;
};

#endif //__TY_MAIN_WINDOW__
