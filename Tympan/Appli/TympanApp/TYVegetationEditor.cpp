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
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/Tools/OLocalizator.h"

#include "Tympan/models/business/material/TYSol.h"
#include "Tympan/GraphicIHM/DataManagerIHM/TYSolWidget.h"

#include <qinputdialog.h>


#if defined(WIN32)
#include <crtdbg.h>
#endif

#if defined(WIN32)
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif


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

        for (unsigned int i = 0; i < tabPts.size(); i++)
        {
            tabPts[i]._z = 0.0;
            pSite->getTopographie()->getAltimetrie()->updateAltitude(tabPts[i]);
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
