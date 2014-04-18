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
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "TYCalculManager.h"

#include "TYMessageManager.h"
#include "Tympan/Config.h"
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/Tools/OChrono.h"
#include "Tympan/MetierSolver/DataManagerMetier/xml_project_util.hpp"

#include <qcursor.h>
#include <qmessagebox.h>

#if defined(WIN32)
#include <crtdbg.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif


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
    bool ret = false;

    if (pCalcul)
    {
        TYMessageManager::get()->info(TR("id_msg_go_calcul"));

        // On rend inactif l'IHM
        getTYMainWnd()->setEnabled(false);

        TYApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

        // Lance le calcul
        OChronoTime startTime; //xbh: analyse du temps de calcul

        TYProjet *pProject = pCalcul->getProjet();

        // Serialize model
        // XXX this code is temporary. We should use QTemporaryFile objects (TBD)
        const char *problemfile = "problem.xml";
        const char *resultfile = "result.xml";

        try
        {
            save_project(problemfile, pProject);
        }
        catch(tympan::invalid_data)
        {
            // reactivate HMI
            TYApplication::restoreOverrideCursor();
            getTYMainWnd()->setEnabled(true);
            return false;
        }


            // Call python module to do the computation
            // XXX we should define a work environment so as to know where to record
            // the xml files, where to look for them, where are the python scripts
            // we want to call, where are the libraries, etc.
            // waiting for a decision, for now keep it dirty
            QProcess python;
            QStringList env = QProcess::systemEnvironment();
            // XXX because for now we install the tympan project in the home
            // directory. To be fixed
            env.replaceInStrings(QRegExp("^LD_LIBRARY_PATH=(.*)"),
                    "LD_LIBRARY_PATH=Tympan/lib");
            python.setEnvironment(env);

            QStringList args;
            args << "Tympan/pymodules/tympan.py" << problemfile << resultfile
                << "Tympan/pluginsd";
            python.start("python", args);
            while (!python.waitForFinished()); // wait for the script to execute
            int pystatus = python.exitStatus();
            if (pystatus == 0)
            {
                // Then read the result to update the internal model
                LPTYProjet result;
                try
                {
                    result = load_project(resultfile);
                }
                catch(tympan::invalid_data)
                {
                    // reactivate HMI
                    TYApplication::restoreOverrideCursor();
                    getTYMainWnd()->setEnabled(true);
                    return false;
                }
                    pProject = result.getRealPointer();
                    getTYApp()->getCurProjet()->setCurrentCalcul(
                            pProject->getCurrentCalcul());
                    pCalcul = pProject->getCurrentCalcul();
                    ret = true;
            }

        OChronoTime endTime; //xbh: analyse du temps de calcul
        OChronoTime duration = endTime - startTime;
        unsigned long second  = duration.getTime() / 1000;
        unsigned long millisecond = duration.getTime() - second * 1000;
        TYMessageManager::get()->info("Temps de calcul : %02ld,%03ld sec.  (%ld msec.)", second, millisecond, duration.getTime());

        TYApplication::restoreOverrideCursor();

        // On reactive l'IHM
        getTYMainWnd()->setEnabled(true);


        if (ret)
        {
            // Update graphic
            pCalcul->getParent()->updateGraphicTree();
            pCalcul->updateGraphicTree();
            getTYMainWnd()->updateModelers(false, false);
            TYElement::setIsSavedOk(true);

            // Update projet frame
            getTYMainWnd()->getProjetFrame()->setCalculDone(true);

            // Calcul termine et reussi
            TYMessageManager::get()->info(TR("id_msg_calcul_done"));
        }
        else
        {
            // Calcul termine et echec
            TYMessageManager::get()->info(TR("id_msg_calcul_failed"));
        }
    }

    return ret;
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
    if (pElement->inherits("TYSiteNode"))
    {
        TYSiteNode* pSite = TYSiteNode::safeDownCast(pElement);
        if (pSite) { pSite->update(); }
    }
    if (pElement->inherits("TYAcousticVolumeNode"))
    {
        LPTYAcousticVolumeNode pAccVolNode = TYAcousticVolumeNode::safeDownCast(pElement);
        if (pAccVolNode)
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
    }
    else if (pElement->inherits("TYAcousticLine"))
    {
        LPTYAcousticLine pLine = TYAcousticLine::safeDownCast(pElement);
        if (pLine)
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

