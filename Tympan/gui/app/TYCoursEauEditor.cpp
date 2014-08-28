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
 * \file TYCoursEauEditor.cpp
 * \brief Construit un CoursEau a partir des points saisis
 */

#include <qdialog.h>

#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/infrastructure/TYSiteNode.h"
#include "Tympan/models/business/topography/TYCoursEau.h"
#include "Tympan/gui/app/TYModelerFrame.h"
#include "Tympan/gui/app/TYSiteModelerFrame.h"
#include "Tympan/gui/app/TYActions.h"
#include "Tympan/gui/app/TYApplication.h"
#include "Tympan/gui/app/TYMainWindow.h"
#include "TYCoursEauEditor.h"



#define TR(id) OLocalizator::getString("TYCoursEauEditor", (id))


TYCoursEauEditor::TYCoursEauEditor(TYModelerFrame* pModeler) :
    TYPolyLineEditor(pModeler)
{
    QObject::connect(this, SIGNAL(endedSavingPoints()), this, SLOT(endCoursEau()));
}

TYCoursEauEditor::~TYCoursEauEditor()
{
}

void TYCoursEauEditor::endCoursEau()
{
    if (!_pModeler->askForResetResultat())
    {
        return;
    }

    LPTYCoursEau pCoursEau = new TYCoursEau();

    if (pCoursEau->edit(_pModeler) == QDialog::Accepted)
    {
        pCoursEau->setTabPoint(this->getSavedPoints());

        TYSiteNode* pSite = ((TYSiteModelerFrame*)_pModeler)->getSite();

        // Make sure altimetry was initialized before using it
        if (pSite->getTopographie()->getAltimetrie()->containsData())
        {
            for (unsigned int i = 0; i < pCoursEau->getTabPoint().size(); i++)
            {
                pCoursEau->getTabPoint()[i]._z = 0.0;
                pSite->getTopographie()->getAltimetrie()->updateAltitude(pCoursEau->getTabPoint()[i]);
            }
        }

        if (pSite->getTopographie()->addCrsEau(pCoursEau))
        {
            TYAction* pAction = new TYAddElementToTopoAction((LPTYElement&) pCoursEau, pSite->getTopographie(), _pModeler, TR("id_action_addcrseau"));
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
