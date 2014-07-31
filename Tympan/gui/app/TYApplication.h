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
 * \file TYApplication.h
 * \brief pour l'application Tympan (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_APPLICATION__
#define __TY_APPLICATION__

#include <qapplication.h>

#include "TYAppDefines.h"
#include "TYCalculManager.h"
#include "TYActionManager.h"

class TYImageManager;
class TYPluginManager;

/**
 * \class TYApplication
 * \brief Classe principale pour l'application Tympan
 */
class TYApplication : public QApplication
{
    Q_OBJECT

public:
    /**
     * \fn TYApplication(int &argc, char **argv)
     * \brief Constructeur par defaut.
     */
    TYApplication(int& argc, char** argv);
    /**
     * \fn virtual ~TYApplication()
     * \brief Destructeur.
     */
    virtual ~TYApplication();

    /**
     * \fn bool init()
     * \brief Initialise l'application et charges toutes les ressources.
     */
    bool init();
    /**
     * \fn bool run()
     * \brief Lance l'application.
     */
    bool run();
    /**
     * \fn bool close()
     * \brief Ferme l'application et libere toutes les ressources.
     */
    bool close();

    /**
     * \fn TYMainWindow * getMainWnd()
     * \brief Get de la fenetre principale.
     */
    TYMainWindow* getMainWnd() { return _pMainWnd; }

    /**
     * \fn TYActionManager * getActionManager()
     * \brief Get du gestionnaire d'actions (historique pour les undos).
     */
    TYActionManager* getActionManager() { return &_actionManager; }

    /**
     * \fn TYCalculManager * getCalculManager()
     * \brief Get du gestionnaire de calculs.
     */
    TYCalculManager* getCalculManager() { return _pCalculManager; }

    /**
     * \fn LPTYProjet getCurProjet()
     * \brief Set/Get du projet courant.
     */
    LPTYProjet getCurProjet() { return _pCurProjet; }
    /**
     * \fn LPTYSiteNode getCurSiteNode()
     * \brief Set/Get du site node courant.
     */
    LPTYSiteNode getCurSiteNode() { return _pCurSiteNode; }

    /**
     * \fn static QString tympanUserDir()
     * \brief Retourne le chemin du dossier de sauvegarde de l'utilisateur courant.
     * Celui-ci est donne par la variable d'environnement TYMPANUSERDIR.
     * Si la variable n'est pas definie, c'est le dossier courant de
     * l'application qui est utilise.
     */
    static QString tympanUserDir();

    /**
     * \fn static const QString& getCurrentFileName()
     * \brief Retourne le nom du fichier en cours d'utilisation
     */
    static const QString& getCurrentFileName();

    /**
     * \fn static void setCurrentFileName(const QString& fileName)
     * \brief Enregistre le nom du ficgier de travail courant
     */
    static void setCurrentFileName(const QString& fileName);

    /// Get/Set du repertoire de travail courant
    static void setCurrentDirName(const QString& dirName);
    static const QString& getCurrentDirName();

    /**
     * \fn static QString tympanAppDir()
     * \brief Retourne le chemin du dossier de l'application.
     */
    static QString tympanAppDir();

    /**
     * \fn bool getIsCurrentFileSaved()
     * \brief Get/Set de l'indicateur d'etat de sauvegarde du fichier courant
     */
    bool getIsCurrentFileSaved() { return _saved; }

    /**
     * \fn void setIsCurrentFileSaved(const bool& saved)
     * \brief Get/Set de l'indicateur d'etat de sauvegarde du fichier courant
     */
    void setIsCurrentFileSaved(const bool& saved) { _saved = saved; }

public slots:
    /**
     * \fn void setCurProjet(LPTYProjet pProjet)
     * \brief Set/Get du projet courant.
     */
    void setCurProjet(LPTYProjet pProjet);
    /**
     * \fn void setCurSiteNode(LPTYSiteNode pSiteNode)
     * \brief Set/Get du site node courant.
     */
    void setCurSiteNode(LPTYSiteNode pSiteNode);

signals:
    /**
     * \fn void curProjetChanged(LPTYProjet pCurProjet)
     * \brief Signal que le projet courant a change.
     */
    void curProjetChanged(LPTYProjet pCurProjet);
    /**
     * \fn void curSiteNodeChanged(LPTYSiteNode pCurSiteNode)
     * \brief Signal que le site node courant a change.
     */
    void curSiteNodeChanged(LPTYSiteNode pCurSiteNode);

protected:
    ///Splash screen.
    OSplashScreen* _pSplash;

    ///La fenetre principale de l'application.
    TYMainWindow* _pMainWnd;

    ///Le gestionnaire d'actions (historique pour les undos).
    TYActionManager _actionManager;

    ///Le gestionnaire de calculs.
    TYCalculManager* _pCalculManager;

    ///Pointeur sur le projet courant.
    LPTYProjet _pCurProjet;

    ///Pointeur sur le site node courant.
    LPTYSiteNode _pCurSiteNode;

    /// Le gestionnaire de messages d'erreurs
    TYMessageManager* _pMessageManager;

    /// Le gestionnaire d'image
    TYImageManager* _pImageManager;

    /// Le gestionnaire de plugins
    TYPluginManager* _pPluginManager;

    ///Indicateur de l'etat de sauvegarde du fichier courant
    bool _saved;

public :
    ///Get Chemin d'origine de l'application. (singleton)
    static const QString& getOriginalCurrentDirPath();

    ///Set Chemin d'origine de l'application.
    static void setOriginalCurrentDirPath(const QString&);

    /// Get chemin des preferences utilisateur
    static const QString& getSettingsDir();

    /// Set chemin des preferences utilisateur
    static void setSettingsDir(const QString&);

private :
    bool createMainWnd();
    bool createProgressMngr();
    bool createMessageMngr();
    bool loadTextures();
    bool loadPlugins();
    bool createCalculMngr();
    bool loadSettings();
    bool createCacheDir();
    bool createWorkDir();

private:
    ///Chemin d'origine de l'application.
    static QString* _originalCurrentDirPath;

    ///Chemin des parametres utilisateur de l'application
    static QString* _settingsDir;

    ///Nom du fichier de travail courant
    static QString* _currentFileName;

    /// Nom du repertoire de travail courant
    static QString* _currentDirName;
};

///Retourne le pointeur sur l'application.
TYApplication*  getTYApp();
///Retourne le pointeur sur la fenetre principale.
TYMainWindow*   getTYMainWnd();

///Affiche un message dans la fenetre de sortie.
void writeOutputMsg(QString msg);

///Affiche un message de debug dans la fenetre de sortie.
void writeDebugMsg(QString msg);


#endif //__TY_APPLICATION__
