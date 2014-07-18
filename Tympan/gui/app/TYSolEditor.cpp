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
 * \file TYSolEditor.cpp
 * \brief Construit un sol a partir des points saisis
 */


#include <qinputdialog.h>

#include "Tympan/core/logging.h"
#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/infrastructure/TYSiteNode.h"
#include "Tympan/models/business/material/TYSol.h"
#include "Tympan/models/business/topography/TYTerrain.h"
#include "Tympan/gui/widgets/TYSolWidget.h"
#include "Tympan/gui/app/TYActions.h"
#include "Tympan/gui/app/TYApplication.h"
#include "Tympan/gui/app/TYMainWindow.h"
#include "Tympan/gui/app/TYModelerFrame.h"
#include "Tympan/gui/app/TYSiteModelerFrame.h"
#include "TYSolEditor.h"

#if defined(WIN32)
  #include <crtdbg.h>
  #ifdef _DEBUG
    #undef THIS_FILE
    static char THIS_FILE[] = __FILE__;
    #define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
  #endif
#endif

#define TR(id) OLocalizator::getString("TYSolEditor", (id))


TYSolEditor::TYSolEditor(TYModelerFrame* pModeler) :
    TYPolyLineEditor(pModeler)
{
    QObject::connect(this, SIGNAL(endedSavingPoints()), this, SLOT(endSol()));
}

TYSolEditor::~TYSolEditor()
{
}

void TYSolEditor::endSol()
{
    if (!_pModeler->askForResetResultat())
    {
        return;
    }

    LPTYTerrain pTerrain = new TYTerrain();
    LPTYSol pSol = new TYSol();
    pTerrain->setSol(pSol);

    if (pTerrain->edit(_pModeler) == QDialog::Accepted)
    {
        //LPTYTerrain pTerrain = new TYTerrain();
        //pTerrain->setSol(pSol);
        TYTabPoint tabPts = this->getSavedPoints();

        TYSiteNode* pSite = ((TYSiteModelerFrame*)_pModeler)->getSite();

        // XXX See ticket https://extranet.logilab.fr/ticket/1484180
        // This hardly readable cascade of if is better than no reporting
        // but should instead be handled with exceptions.
        if (pSite->getTopographie()->addTerrain(pTerrain))
        {
            if (pSite->updateAltimetrie(true))
            {
                bool alti_update_ok = true;

                for (unsigned int i = 0; i < tabPts.size(); i++)
                {
                    tabPts[i]._z = 0.0;
                    alti_update_ok &= pSite->getTopographie()->getAltimetrie()->updateAltitude(tabPts[i]);
                }
                if (!alti_update_ok)
                {
                    OMessageManager::get()->error(
                        "The ground material area is invalid : "
                        "probably out of the area of well defined altimetry: "
                        "it is going to be removed");
                    pSite->getTopographie()->remTerrain(pTerrain);
                    return;
                } // if (!alti_update_ok)
            }
            else
            {
                OMessageManager::get()->error(
                    "The altimetry update failed: "
                    "the new material area will be left with invalid altitude!");
            } // if (pSite->updateAltimetrie(true))

            TYAction* pAction = new TYAddElementToTopoAction(
                (LPTYElement&) pTerrain, pSite->getTopographie(), _pModeler,
                TR("id_action_addsol"));
            _pModeler->getActionManager()->addAction(pAction);

            pTerrain->setListPoints(tabPts);

            pSite->getTopographie()->updateGraphicTree();
            updateSiteFrame();
            _pModeler->getView()->getRenderer()->updateDisplayList();
            _pModeler->updateView();
        } // (pSite->getTopographie()->addTerrain(pTerrain))

        // repasse en mode camera selection
        getTYMainWnd()->setDefaultCameraMode();
    }
}
