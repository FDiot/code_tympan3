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
 * \file TYCalculManager.cpp
 * \brief Gestionnaire des calculs acoustiques. Il fait l'interface entre l'IHM et le gestionnaire de donnees
 * pour la partie calcul
 */

#include <qcursor.h>
#include <qmessagebox.h>
#include <QTemporaryFile>

#include "Tympan/core/config.h"
#include "Tympan/core/chrono.h"
#include "Tympan/core/defines.h"
#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/xml_project_util.h"
#include "Tympan/models/business/subprocess_util.h"
#include "Tympan/gui/app/os.h"
#include "Tympan/gui/app/TYApplication.h"
#include "Tympan/gui/app/TYProjetFrame.h"
#include "Tympan/gui/app/TYSiteFrame.h"
#include "Tympan/gui/app/TYMessageManager.h"
#include "Tympan/gui/app/TYMainWindow.h"
#include "TYCalculManager.h"


#define TR(id) OLocalizator::getString("TYCalculManager", (id))

using namespace tympan;

TYCalculManager::TYCalculManager()
{
    _pCurrentCalcul = NULL;
}

TYCalculManager::~TYCalculManager()
{
    _pCurrentCalcul = NULL;
}

void TYCalculManager::setCurrent(LPTYCalcul pCalcul)
{
    _pCurrentCalcul = pCalcul;
}

bool TYCalculManager::launchCurrent()
{
    return launch(_pCurrentCalcul);
}

bool TYCalculManager::launch(LPTYCalcul pCalcul)
{
    if (!pCalcul)
    {
        return false;
    }

    TYProjet *pProject = pCalcul->getProjet();
    OMessageManager& logger =  *OMessageManager::get();

    // Is the debug option "TYMPAN_DEBUG=keep_tmp_files" enabled?
    bool keep_tmp_files = must_keep_tmp_files();
    // Temporary XML files to give the current acoustic problem to the python
    // script and get the results
    QTemporaryFile problemfile;
    problemfile.setFileTemplate(QDir::tempPath() + QString("/XXXXXX.xml"));
    QTemporaryFile resultfile;
    resultfile.setFileTemplate(QDir::tempPath() + QString("/XXXXXX.xml"));
    QTemporaryFile meshfile;
    meshfile.setFileTemplate(QDir::tempPath() + QString("/XXXXXX.ply"));
    if(!init_tmp_file(problemfile, keep_tmp_files)
            || !init_tmp_file(resultfile, keep_tmp_files)
            || !init_tmp_file(meshfile, keep_tmp_files))
    {
        logger.error("Creation de fichier temporaire impossible. Veuillez verifier l'espace disque disponible.");
        return false;
    }
    // Serialize current project
    try
    {
        save_project(problemfile.fileName().toUtf8().data(), pProject);
    }
    catch(const tympan::invalid_data& exc)
    {
        ostringstream msg;
        msg << boost::diagnostic_information(exc);
        logger.error(
                "Could not export current project. Computation won't be done");
        logger.debug(msg.str().c_str());
        return false;
    }
    if(keep_tmp_files)
    {
        logger.debug(
                "Le calcul va s'executer en mode debug.\nLes fichiers temporaires ne seront pas supprimes une fois le calcul termine.\nProjet courant non calcule: %s. Projet avec les resultats du calcul: %s. Mesh de l'altimetrie au format ply: %s",
                problemfile.fileName().toStdString().c_str(),
                resultfile.fileName().toStdString().c_str(),
                meshfile.fileName().toStdString().c_str());
    }

    // Call python script "solve_tympan_project.py" with: the name of the file
    // containing the problem, the name of the file where to record
    // the result and the directory containing the solver plugin to use
    // to solve the acoustic problem
    QStringList args;
    QString absolute_plugins_path (QCoreApplication::applicationDirPath());
    absolute_plugins_path.append("/");
    absolute_plugins_path.append(PLUGINS_PATH);
    QString absolute_pyscript_path (QCoreApplication::applicationDirPath());
    absolute_pyscript_path.append("/");
    absolute_pyscript_path.append(SOLVE_PYSCRIPT);
    args << absolute_pyscript_path << problemfile.fileName() << resultfile.fileName()
        <<  meshfile.fileName() << absolute_plugins_path;

    logger.info(TR("id_msg_go_calcul"));

    if (!python_gui(args))
    {
        return false;
    }
    // Then read the result to update the internal model
    LPTYProjet result;
    try
    {
        result = load_project(resultfile.fileName().toUtf8().data());
    }
    catch(const tympan::invalid_data& exc)
    {
        ostringstream msg;
        msg << boost::diagnostic_information(exc);
        logger.error("Could not import computed project. No results available.");
        logger.debug(msg.str().c_str());
        QMessageBox msgBox;
        msgBox.setText("Le fichier de resultats n'a pas pu etre lu.");
        msgBox.exec();
        return false;
    }
    // Update the current project with the results of the current acoustic
    // problem
    TYCalcul* pOldComp = pCalcul;
    pProject = result.getRealPointer();
    pCalcul = pProject->getCurrentCalcul();
    getTYApp()->getCurProjet()->setCurrentCalcul(pCalcul);
    // Can't remove current computation so first assign it and then remove
    // the previous one from the project
    getTYApp()->getCurProjet()->remCalcul(pOldComp);
    getTYMainWnd()->getProjetFrame()->setProjet(pProject);
    // Update site altimetry with the mesh retrieved from the ply file
    LPTYSiteNode pSite = pProject->getSite();
    std::deque<OPoint3D> points;
    std::deque<OTriangle> triangles;
    std::deque<LPTYSol> materials;
    pSite->readMesh(points, triangles, materials, meshfile.fileName());
    pSite->getTopographie()->getAltimetrie()->plugBackTriangulation(points,
            triangles, materials);
    pSite->updateAltiInfra();
    pSite->updateAcoustique();
    pProject->updateAltiRecepteurs();
    // Update graphics
    pCalcul->getParent()->updateGraphicTree();
    pCalcul->updateGraphicTree();
    getTYMainWnd()->updateModelers(false, false);
    TYElement::setIsSavedOk(true);
    // Update projet frame
    getTYMainWnd()->getProjetFrame()->setCalculDone(true);
    // Computation achieved with success
    logger.info(TR("id_msg_calcul_done"));
    return true;
}

bool TYCalculManager::askForResetResultat()
{
    bool ret = true;
    TYCalcul* pCalcul = NULL;
    TYProjet* pProjet = getTYApp()->getCurProjet().getRealPointer();
    if (pProjet)
    {
        pCalcul = pProjet->getCurrentCalcul();
    }
    else
    {
        return true;
    }

    if (pCalcul)
    {
        ret = true;
        getTYMainWnd()->getProjetFrame()->setCalculDone(false);

        /*// Question
        int del = QMessageBox::Yes;
        if (pCalcul->getIsAcousticModified())
        {
            del = QMessageBox::warning(getTYMainWnd(), getTYMainWnd()->windowTitle(), TR("id_msg_del_resultat"), QMessageBox::Yes, QMessageBox::No);
        }
        if (del == QMessageBox::Yes)
        {
            // Ok pour effacer les resultats (uniquement si c'est un resultat calcule);
            if (pCalcul->getState() == TYCalcul::Actif)
            {
            // Reset du resultat
            pCalcul->getResultat()->purge();
            for (unsigned int i=0 ; i<pProjet->getPointsControl().size() ; i++)
            {
                pProjet->getPointsControl()[i]->purge(pCalcul);
            }

                pCalcul->setIsAcousticModified(false); // La calcul est a jour
            }

            ret = true;
        }
        else
        {
            ret = false;
        }*/
    }

    return ret;
}

bool TYCalculManager::updateAcoustic(TYElement* pElement)
{
    bool ret = false;
    TYSiteNode* pSite = dynamic_cast<TYSiteNode*>(pElement);
    if (pSite != nullptr)
    {
        pSite->updateAcoustique();
    }
    LPTYAcousticVolumeNode pAccVolNode = dynamic_cast<TYAcousticVolumeNode*>(pElement);
    if (pAccVolNode._pObj != nullptr)
    {
            TYMessageManager::get()->info(TR("id_msg_go_updateacoustic"));

            TYApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

            // Calcul acoustique
            ret = pAccVolNode->updateAcoustic();

            if (ret)
            {
                TYMessageManager::get()->info(TR("id_msg_updateacoustic_done"));
            }
            else
            {
                TYMessageManager::get()->info(TR("id_msg_updateacoustic_failed"));
            }

            // MaJ graphique
            pAccVolNode->getGraphicObject()->update(true);
            getTYMainWnd()->updateModelers(false, false);

            TYApplication::restoreOverrideCursor();
    }
    else
    {
        LPTYAcousticLine pLine = dynamic_cast<TYAcousticLine*>(pElement);
        if (pLine._pObj != nullptr)
        {
            TYMessageManager::get()->info(TR("id_msg_go_updateacoustic"));

            TYApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

            // Calcul acoustique
            ret = pLine->updateAcoustic();

            if (ret)
            {
                TYMessageManager::get()->info(TR("id_msg_updateacoustic_done"));
            }
            else
            {
                TYMessageManager::get()->info(TR("id_msg_updateacoustic_failed"));
            }

            // MaJ graphique
            pLine->getGraphicObject()->update(true);
            getTYMainWnd()->updateModelers(false, false);

            TYApplication::restoreOverrideCursor();
        }
    }

    return ret;
}

