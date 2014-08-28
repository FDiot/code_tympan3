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
 * \file TYVegetationEditor.cpp
 * \brief Construit un Vegetation a partir des points saisis
 */


#include <qinputdialog.h>

#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/material/TYSol.h"
#include "Tympan/models/business/infrastructure/TYSiteNode.h"
#include "Tympan/models/business/topography/TYTerrain.h"
#include "Tympan/gui/widgets/TYSolWidget.h"
#include "Tympan/gui/app/TYModelerFrame.h"
#include "Tympan/gui/app/TYSiteModelerFrame.h"
#include "Tympan/gui/app/TYActions.h"
#include "Tympan/gui/app/TYApplication.h"
#include "Tympan/gui/app/TYMainWindow.h"
#include "TYVegetationEditor.h"



#define TR(id) OLocalizator::getString("TYVegetationEditor", (id))


TYVegetationEditor::TYVegetationEditor(TYModelerFrame* pModeler) :
    TYPolyLineEditor(pModeler)
{
    QObject::connect(this, SIGNAL(endedSavingPoints()), this, SLOT(endVegetation()));
}

TYVegetationEditor::~TYVegetationEditor()
{
}

void TYVegetationEditor::endVegetation()
{
    if (!_pModeler->askForResetResultat())
    {
        return;
    }

    LPTYTerrain pTerrain = new TYTerrain();
    LPTYSol pSol = new TYSol();
    pSol->useVegetation(true);
    pTerrain->setSol(pSol);

    if (pTerrain->edit(_pModeler) == QDialog::Accepted)
    {
        //LPTYTerrain pTerrain = new TYTerrain();
        //pTerrain->setSol(pSol);
        TYTabPoint tabPts = this->getSavedPoints();

        TYSiteNode* pSite = ((TYSiteModelerFrame*)_pModeler)->getSite();
        // Make sure altimetry was initialized before using it
        if (pSite->getTopographie()->getAltimetrie()->containsData())
        {
            for (unsigned int i = 0; i < tabPts.size(); i++)
            {
                tabPts[i]._z = 0.0;
                pSite->getTopographie()->getAltimetrie()->updateAltitude(tabPts[i]);
            }
        }

        pTerrain->setListPoints(tabPts);

        if (pSite->getTopographie()->addTerrain(pTerrain))
        {
            TYAction* pAction = new TYAddElementToTopoAction((LPTYElement&) pTerrain, pSite->getTopographie(), _pModeler, TR("id_action_addvegetation"));
            _pModeler->getActionManager()->addAction(pAction);

            pSite->getTopographie()->updateGraphicTree();
            refreshSiteFrame();
            _pModeler->getView()->getRenderer()->updateDisplayList();
            _pModeler->updateView();
        }

        // repasse en mode camera selection
        getTYMainWnd()->setDefaultCameraMode();
    }
}
