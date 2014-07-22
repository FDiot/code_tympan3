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
 * \file TYApplication.cpp
 * \brief pour l'application Tympan
 */


#include <qdir.h>
#include <qtimer.h>
#include <qmessagebox.h>

#include "Tympan/core/config.h"
#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/TYXMLManager.h"
#include "Tympan/models/business/TYProgressManager.h"
#include "Tympan/models/business/TYPluginManager.h"
#include "Tympan/models/business/infrastructure/TYSiteNode.h"
#include "Tympan/gui/tools/TYImageManager.h"
#include "Tympan/gui/app/OSplashScreen.h"
#include "Tympan/gui/app/TYMessageManager.h"
#include "Tympan/gui/app/TYOutputFrame.h"
#include "Tympan/gui/app/TYProjetFrame.h"
#include "Tympan/gui/app/TYMainWindow.h"
#include "TYApplication.h"


using namespace Qt;

#define TR(id) OLocalizator::getString("TYApplication", (id))


TYApplication*  getTYApp() { return (TYApplication*) qApp; }

//Chemin d'origine de l'application.
QString* TYApplication::_originalCurrentDirPath = NULL;

// Repertoire des settings utilisateur
QString* TYApplication::_settingsDir = NULL;

// Nom du repertoire de travail
QString* TYApplication::_currentDirName = NULL;

// Nom du fichier de travail
QString* TYApplication::_currentFileName = NULL;

TYMainWindow*   getTYMainWnd()
{
    if (getTYApp())
    {
        return getTYApp()->getMainWnd();
    }
    else { return NULL; }
}

void writeOutputMsg(QString msg)
{
    if (getTYMainWnd())
    {
        getTYMainWnd()->getOutputFrame()->appendMsg(msg);
    }
}

void writeDebugMsg(QString msg)
{
#ifdef _DEBUG
    writeOutputMsg("Debug : " + msg + "\n");
#endif
}

const QString& TYApplication::getOriginalCurrentDirPath()
{
    if (!_originalCurrentDirPath)
    {
        _originalCurrentDirPath = new QString(QDir::currentPath());
        return *_originalCurrentDirPath;
    }

    return *_originalCurrentDirPath;
}

void TYApplication::setOriginalCurrentDirPath(const QString& newValue)
{
    if (_originalCurrentDirPath)
    {
        *_originalCurrentDirPath = newValue;
    }
    else
    {
        _originalCurrentDirPath = new QString(newValue);
    }
}

const QString& TYApplication::getCurrentFileName()
{
    if (_currentFileName)
    {
        return *_currentFileName;
    }
    else
    {
        _currentFileName = new QString("");
        return *_currentFileName;
    }
}

void TYApplication::setCurrentFileName(const QString& newValue)
{
    if (_currentFileName)
    {
        *_currentFileName = newValue;
    }
    else
    {
        _currentFileName = new QString(newValue);
    }
}

const QString& TYApplication::getCurrentDirName()
{
    if (_currentDirName)
    {
        return *_currentDirName;
    }
    else
    {
        *_currentDirName = tympanUserDir();
        return *_currentDirName;
    }
}


void TYApplication::setCurrentDirName(const QString& newValue)
{
    if (_currentDirName)
    {
        *_currentDirName = newValue;
    }
    else
    {
        _currentDirName = new QString(newValue);
    }
}


TYApplication::TYApplication(int& argc, char** argv)
    : QApplication(argc, argv), _actionManager(10)
{
    _pSplash = NULL;
    _pMainWnd = NULL;
    _pCurProjet = NULL;
    _pCurSiteNode = NULL;
    _pCalculManager = NULL;
    _saved = false;
    QStringList args = this->arguments();
    _usePython = true;
    if (args.count() > 1)
    {
        // We want to solve the acoustic problems using the legacy version of
        // Tympan (no python/cython used)
        if (args.contains("--legacy-computation"))
        {
            _usePython = false;
        }
    }
}

TYApplication::~TYApplication()
{
    if (_pSplash)
    {
        delete _pSplash;
        _pSplash = NULL;
    }
}

bool TYApplication::init()
{
    // Resources path
    QString ressourcePath(getOriginalCurrentDirPath() + "/resources/");
    ressourcePath = QDir::convertSeparators(ressourcePath);

    // Splash screen
#ifndef _DEBUG
    QPixmap pix(ressourcePath + "Splash.png");

    if (pix.isNull())
    {
        pix = QPixmap(230, 368);
        pix.fill();
    }

    QString version = TY_PRODUCT_VERSION_;
    _pSplash = new OSplashScreen(pix, "<font color=\"#09367A\"><i><b>TYMPAN " + version + "</i></b></font>");

    QColor splashColor(4, 34, 245);
    _pSplash->setStatus("Initialisation...", AlignLeft, splashColor);

    // Resources
    _pSplash->setStatus("Chargement des ressources...", AlignLeft, splashColor);
#endif

    // TODO Check consistency of this coherent with the resources distribution.
    if (!((OLocalizator::setRessourceFile(ressourcePath + "Language.xml") == true)
          || (OLocalizator::setRessourceFile(ressourcePath + "Language_FR.xml") == true)
          || (OLocalizator::setRessourceFile(ressourcePath + "Language_EN.xml") == true)))
    {
#ifndef _DEBUG
        _pSplash->hide();
#endif
        QMessageBox::critical(NULL, "Tympan fatal error", "Resources file not found : Language.xml");
        return false;
    }
    OLocalizator::setRessourcePath(ressourcePath);

    // Creation de la fenetre principale, contenant le workspace
    bool  bStatus = createMainWnd();
    // Initialisation du Progress Manager
    bStatus &= createProgressMngr();
    // Initialisation du Message Manager
    bStatus &= createMessageMngr();
    // Chargement des textures
    bStatus &= loadTextures();
    // Chargement des plugins
    bStatus &= loadPlugins();
    // Initialisation du Calcul Manager
    bStatus &= createCalculMngr();
    // Chargement des settings
    bStatus &= loadSettings();
    // Definition du chemin pour la sauvegarde du fichier topographique temporaire
    bStatus &= createCacheDir();
    // Creation d'un dossier de travail/d'echange
    bStatus &= createWorkDir();

#ifndef _DEBUG
    _pSplash->setStatus("Chargement termine.", AlignLeft, splashColor);
#endif

    return bStatus;

}

bool TYApplication::createMainWnd()
{
#ifndef _DEBUG
    QColor splashColor(4, 34, 245);
    _pSplash->setStatus("Creation de l'espace de travail...", AlignLeft, splashColor);
#endif
    _pMainWnd = new TYMainWindow();

    return true;
}

bool TYApplication::createProgressMngr()
{
#ifndef _DEBUG
    QColor splashColor(4, 34, 245);
    _pSplash->setStatus("Initialisation du gestionnaire de progression...", AlignLeft, splashColor);
#endif
    TYProgressManager::create(_pMainWnd, "progress manager");

    return true;
}

bool TYApplication::createMessageMngr()
{
#ifndef _DEBUG
    QColor splashColor(4, 34, 245);
    _pSplash->setStatus("Initialisation du gestionnaire de message...", AlignLeft, splashColor);
#endif
    _pMessageManager = new TYMessageManager();
    _pMessageManager->setAsSingleton();

    return true;
}

bool TYApplication::loadTextures()
{
#ifndef _DEBUG
    QColor splashColor(4, 34, 245);
    _pSplash->setStatus("Chargement des textures...", AlignLeft, splashColor);
#endif
    _pImageManager = new TYImageManager();
    _pImageManager->setAsSingleton();
    if (!_pImageManager->loadImages())
    {
#ifndef _DEBUG
        _pSplash->hide();
#endif
        QMessageBox::critical(NULL, "Tympan fatal error", "Image loading failed!");
        return false;
    }

    return true;
}

bool TYApplication::loadPlugins()
{
#ifndef _DEBUG
    QColor splashColor(4, 34, 245);
    _pSplash->setStatus("Chargement des plugins...", AlignLeft, splashColor);
#endif
    _pPluginManager = new TYPluginManager();
    _pPluginManager->setAsSingleton();
    QString pluginsPath;
#ifndef _DEBUG
    pluginsPath = getOriginalCurrentDirPath() + "/plugins/";
#else
    pluginsPath = getOriginalCurrentDirPath() + "/pluginsd/";
#endif
    if (!_pPluginManager->loadPlugins(pluginsPath))
    {
#ifndef _DEBUG
        _pSplash->hide();
#endif
        QMessageBox::critical(NULL, "Tympan fatal error", "Plugin loading failed!");
        return false;
    }

    return true;
}

bool TYApplication::createCalculMngr()
{
#ifndef _DEBUG
    QColor splashColor(4, 34, 245);
    _pSplash->setStatus("Initialisation du gestionnaire de calcul...", AlignLeft, splashColor);
#endif
    _pCalculManager = new TYCalculManager();
    QObject::connect(_pMainWnd->getProjetFrame(), SIGNAL(changeCurrentCalcul(LPTYCalcul)), _pCalculManager, SLOT(setCurrent(LPTYCalcul)));

    return true;
}

bool TYApplication::loadSettings()
{
#ifndef _DEBUG
    QColor splashColor(4, 34, 245);
    _pSplash->setStatus("Chargement des preferences utilisateur...", AlignLeft, splashColor);
#endif
    _pMainWnd->loadSettings(QDir::convertSeparators(tympanUserDir() + "/Settings" + TY_PRODUCT_VERSION_));

    return true;
}

bool TYApplication::createCacheDir()
{
#ifndef _DEBUG
    QColor splashColor(4, 34, 245);
    _pSplash->setStatus("Definition du dossier temporaire de travail...", AlignLeft, splashColor);
#endif
    QString tympanCachePath(tympanUserDir() + "/cache/");
    tympanCachePath = QDir::convertSeparators(tympanCachePath);
    QDir tympanCacheDir(tympanCachePath);
    if (!tympanCacheDir.exists())
    {
        tympanCacheDir.mkdir(tympanCachePath);
    }
    TYSiteNode::setTopoFilePath(tympanCachePath.toLatin1().data());

    return true;
}

bool TYApplication::createWorkDir()
{
#ifndef _DEBUG
    QColor splashColor(4, 34, 245);
    _pSplash->setStatus("Initialisation du dossier de travail...", AlignLeft, splashColor);
#endif
    QString tympanExchangePath(tympanUserDir());
    tympanExchangePath = QDir::convertSeparators(tympanExchangePath);
    setCurrentDirName(tympanExchangePath);
    QDir tympanExchangeDir(tympanExchangePath);
    if (!tympanExchangeDir.exists())
    {
        tympanExchangeDir.mkdir(tympanExchangePath);
    }
    // Le dossier d'echange devient le dossier courant de l'application
    QDir::setCurrent(tympanExchangePath);

    return true;
}


bool TYApplication::run()
{
    // Affichage
    _pMainWnd->show();

    if (_pSplash)
    {
        _pSplash->finish(_pMainWnd);
        delete _pSplash;
        _pSplash = NULL;
    }

    // Execution de la boucle principale Qt
    exec();

    return true;
}

bool TYApplication::close()
{
    // Suppression des fichiers topographiques temporaires
    QString tympanCachePath(tympanUserDir() + "/cache/");
    tympanCachePath = QDir::convertSeparators(tympanCachePath);
    QDir tympanCacheDir(tympanCachePath);
    for (unsigned int i = 0; i < tympanCacheDir.count(); i++)
    {
        tympanCacheDir.remove(tympanCacheDir[i]);
    }

    if (_pCalculManager != NULL)
    {
        delete _pCalculManager;
        _pCalculManager = NULL;
    }
    //  setMainWidget(NULL);
    delete _pMainWnd;
    _pMainWnd = NULL;
    _pMessageManager = NULL;
    _pImageManager = NULL;
    _pPluginManager = NULL;
    _pCurProjet = NULL;
    _pCurSiteNode = NULL;

    return true;
}

void TYApplication::setCurProjet(LPTYProjet pProjet)
{
    _pCurProjet = pProjet;
    emit curProjetChanged(_pCurProjet);
}

void TYApplication::setCurSiteNode(LPTYSiteNode pSiteNode)
{
    _pCurSiteNode = pSiteNode;
    emit curSiteNodeChanged(_pCurSiteNode);
}

/*static*/ QString TYApplication::tympanUserDir()
{
    QString workingDir(getSettingsDir());
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "WorkDirPath"))
    {
        workingDir = TYPreferenceManager::getString(TYDIRPREFERENCEMANAGER, "WorkDirPath");
    }
    else
    {
        TYPreferenceManager::setString(TYDIRPREFERENCEMANAGER, "WorkDirPath", workingDir);
    }

    return workingDir;
}

const QString& TYApplication::getSettingsDir()
{
    if (!_settingsDir)
    {
        _settingsDir = new QString();
        QString dir;

        dir = QString::fromLatin1(getenv("TYMPANUSERDIR"));

        // Si TYMPANUSERDIR n'est pas defini, on essaye HOME
        if (dir.isEmpty() || !QFile::exists(dir))
        {
            dir = QDir::home().path() + "/TYMPAN";

            QDir tympanDir(dir);
            if (!tympanDir.exists())
            {
                tympanDir.mkdir(dir);
            }
        }
        // Si vide, on essaye de prendre la variable TEMP qui est generalement definie
        if (dir.isEmpty() || !QFile::exists(dir))
        {
            dir = QString::fromLatin1(getenv("TEMP"));
        }

        // Si TEMP est vide, on essaye TMP
        if (dir.isEmpty() || !QFile::exists(dir))
        {
            dir = QString::fromLatin1(getenv("TMP"));
        }
        // En desespoir de cause on se rabat sur le repertoire de l'application
        if (dir.isEmpty() || !QFile::exists(dir))
        {
            dir = getOriginalCurrentDirPath();
        }

        *_settingsDir = dir;
    }

    return *_settingsDir;
}

/*static*/ QString TYApplication::tympanAppDir()
{
    return getOriginalCurrentDirPath();
}



