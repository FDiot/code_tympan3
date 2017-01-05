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
 * \file TYTerrainEditor.cpp
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
#include "TYTerrainEditor.h"


#define TR(id) OLocalizator::getString("TYTerrainEditor", (id))


TYTerrainEditor::TYTerrainEditor(TYModelerFrame* pModeler) :
    TYPolyLineEditor(pModeler)
{
    QObject::connect(this, SIGNAL(endedSavingPoints()), this, SLOT(endTerrain()));
}

TYTerrainEditor::~TYTerrainEditor()
{
}

void TYTerrainEditor::endTerrain()
{
    if ( !(getSavedPoints().size() > 2) || (!_pModeler->askForResetResultat()) )
    {
        return;
    }

    LPTYTerrain pTerrain = new TYTerrain();
    LPTYSol pSol = new TYSol();
    pTerrain->setSol(pSol);
    pTerrain->setListPoints(getSavedPoints());

    if (pTerrain->edit(_pModeler) == QDialog::Accepted)
    {
        TYSiteNode* pSite = ((TYSiteModelerFrame*)_pModeler)->getSite();

        // This hardly readable cascade of if is better than no reporting
        // but should instead be handled with exceptions.
        if (pSite->getTopographie()->addTerrain(pTerrain))
        {
            TYAction* pAction = new TYAddElementToTopoAction(
                (LPTYElement&) pTerrain, pSite->getTopographie(), _pModeler,
                TR("id_action_addsol"));
            _pModeler->getActionManager()->addAction(pAction);

            pSite->getTopographie()->updateGraphicTree();
            updateSiteFrame();
            _pModeler->getView()->getRenderer()->updateDisplayList();
            _pModeler->updateView();
        }

        // repasse en mode camera selection
        getTYMainWnd()->setDefaultCameraMode();
    }
}
