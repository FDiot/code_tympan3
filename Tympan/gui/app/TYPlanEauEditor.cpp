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
 * \file TYPlanEauEditor.cpp
 * \brief Construit un plan d'eau a partir des points saisis
 */


#include <qinputdialog.h>

#include "Tympan/models/business/topography/TYPlanEau.h"
#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/gui/app/TYRenderWindowInteractor.h"
#include "Tympan/gui/app/TYModelerFrame.h"
#include "Tympan/gui/app/TYSiteModelerFrame.h"
#include "Tympan/gui/app/TYActions.h"
#include "Tympan/gui/app/TYApplication.h"
#include "Tympan/gui/app/TYMainWindow.h"
#include "TYPlanEauEditor.h"


#define TR(id) OLocalizator::getString("TYPlanEauEditor", (id))


TYPlanEauEditor::TYPlanEauEditor(TYModelerFrame* pModeler) :
    TYPolyLineEditor(pModeler)
{
    _pPlanEau = NULL;
    QObject::connect(this, SIGNAL(endedSavingPoints()), this, SLOT(endPlanEau()));
}

TYPlanEauEditor::~TYPlanEauEditor()
{
}

void TYPlanEauEditor::slotKeyPressed(int key)
{
    switch (key)
    {
        case Qt::Key_Space:
            if (_active)
            {
                ((TYSiteModelerFrame*)_pModeler)->getSite()->altimetryNeedsUpdate();
                ((TYSiteModelerFrame*)_pModeler)->getSite()->updateGraphicTree();
                _pInteractor->updateGL();
            }
            break;
        default:
            TYPolyLineEditor::slotKeyPressed(key);
    }
}

void TYPlanEauEditor::endPlanEau()
{
    if (!_pModeler->askForResetResultat())
    {
        return;
    }

    bool ok = false;
    double alti = QInputDialog::getDouble(getTYMainWnd(), getTYMainWnd()->windowTitle(), TR("id_msg_getaltitude"), 0, -10000, 10000, 2, &ok);

    if (ok && (this->getSavedPoints().size() != 0))
    {
        _pPlanEau = new TYPlanEau();
        _pPlanEau->setListPoints(this->getSavedPoints());
        _pPlanEau->setAltitude(alti);

        TYSiteNode* pSite = ((TYSiteModelerFrame*) _pModeler)->getSite();

        if (pSite->getTopographie()->addPlanEau(_pPlanEau))
        {
            TYAction* pAction = new TYAddElementToTopoAction((LPTYElement&) _pPlanEau, pSite->getTopographie(), _pModeler, TR("id_action_addplaneau"));
            _pModeler->getActionManager()->addAction(pAction);

            // On demande la mise à jour de l'altimetrie globale du site
            TYProjet* pProjet = getTYApp()->getCurProjet();
            if (pProjet)
            {
                pProjet->getSite()->altimetryNeedsUpdate();
                pProjet->getSite()->getTopographie()->updateGraphicTree();
            }

            refreshSiteFrame();
            _pModeler->getView()->getRenderer()->updateDisplayList();
            _pModeler->updateView();
        }

        // repasse en mode camera selection
        getTYMainWnd()->setDefaultCameraMode();
    }
}
