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
 * \file TYEmpriseEditor.cpp
 * \brief Construit l'emprise a partir des points saisis
 */


#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/TYPointControl.h"
#include "Tympan/models/business/TYMaillage.h"
#include "Tympan/models/business/infrastructure/TYSiteNode.h"
#include "Tympan/models/business/infrastructure/TYTopographie.h"
#include "Tympan/models/business/infrastructure/TYMachine.h"
#include "Tympan/models/business/geoacoustic/TYAcousticSurface.h"
#include "Tympan/models/business/acoustic/TYUserSourcePonctuelle.h"
#include "Tympan/gui/app/TYActions.h"
#include "Tympan/gui/app/TYApplication.h"
#include "Tympan/gui/app/TYMainWindow.h"
#include "Tympan/gui/app/TYRenderWindowInteractor.h"
#include "Tympan/gui/app/TYSiteModelerFrame.h"
#include "Tympan/gui/app/TYModelerFrame.h"
#include "TYEmpriseEditor.h"



#define TR(id) OLocalizator::getString("TYEmpriseEditor", (id))


TYEmpriseEditor::TYEmpriseEditor(TYModelerFrame* pModeler) :
    TYPolyLineEditor(pModeler)
{
    QObject::connect(this, SIGNAL(endedSavingPoints()), this, SLOT(endEmprise()));
}

TYEmpriseEditor::~TYEmpriseEditor()
{
}

void TYEmpriseEditor::slotKeyPressed(int key)
{
    switch (key)
    {
        case Qt::Key_Space:
            if (_active)
            {
                ((TYSiteModelerFrame*)_pModeler)->getSite()->updateGraphic();
                _pInteractor->updateGL();
            }
            break;
        default:
            TYPolyLineEditor::slotKeyPressed(key);
    }
}

void TYEmpriseEditor::endEmprise()
{
    if (getSavedPoints().size() != 0)
    {
        if (_pModeler->askForResetResultat())
        {
            // On met les points a altitude 0
            for (unsigned int i = 0; i < getSavedPoints().size(); i++)
            {
                getSavedPoints()[i]._z = 0.0;
            }

            // Le site en edition
            LPTYSiteNode pSite = ((TYSiteModelerFrame*)_pModeler)->getSite();
            LPTYTopographie pTopo = pSite->getTopographie();

            // Action
            TYAction* pAction = new TYSetEmpriseTopoAction(pTopo, getSavedPoints(), _pModeler, TR("id_action_setemprise"));
            _pModeler->getActionManager()->addAction(pAction);

            // On assigne les points saisies pour l'emprise de la topo
            pTopo->setEmprise(getSavedPoints());
            pSite->getAltimetry()->setIsGeometryModified(true);

            // Update
            pTopo->updateGraphicTree();

            _pModeler->getView()->getRenderer()->updateDisplayList();
            _pModeler->updateView();

            // repasse en mode camera selection
            getTYMainWnd()->setDefaultCameraMode();
        }
    }
}
