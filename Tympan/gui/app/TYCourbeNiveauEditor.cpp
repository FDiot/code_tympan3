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
 * \file TYCourbeNiveauEditor.cpp
 * \brief Construit une courbe de niveau a partir des points saisis
 */


#include <qinputdialog.h>

#include "Tympan/models/business/topography/TYCourbeNiveau.h"
#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/TYProjet.h"
#include "Tympan/models/business/infrastructure/TYSiteNode.h"
#include "Tympan/gui/gl/TYCourbeNiveauGraphic.h"
#include "Tympan/gui/app/TYModelerFrame.h"
#include "Tympan/gui/app/TYSiteFrame.h"
#include "Tympan/gui/app/TYSiteModelerFrame.h"
#include "Tympan/gui/app/TYRenderWindowInteractor.h"
#include "Tympan/gui/app/TYActions.h"
#include "Tympan/gui/app/TYApplication.h"
#include "Tympan/gui/app/TYMainWindow.h"
#include "TYCourbeNiveauEditor.h"



#define TR(id) OLocalizator::getString("TYCourbeNiveauEditor", (id))


TYCourbeNiveauEditor::TYCourbeNiveauEditor(TYModelerFrame* pModeler) :
    TYPolyLineEditor(pModeler)
{
    _pCrbNiv = NULL;
    QObject::connect(this, SIGNAL(endedSavingPoints()), this, SLOT(endCourbeNiveau()));
}

TYCourbeNiveauEditor::~TYCourbeNiveauEditor()
{
}

void TYCourbeNiveauEditor::init()
{
    ((TYSiteModelerFrame*) _pModeler)->showCrbNiv(true);

    TYPolyLineEditor::init();
}

void TYCourbeNiveauEditor::slotKeyPressed(int key)
{
    switch (key)
    {
        case Qt::Key_Space:
            if (_active)
            {
                getTYMainWnd()->getSiteFrame()->altimetryNeedsUpdate();
                ((TYSiteModelerFrame*)_pModeler)->getSite()->updateGraphic();
                _pInteractor->updateGL();
            }
            break;
        default:
            TYPolyLineEditor::slotKeyPressed(key);
    }
}

void TYCourbeNiveauEditor::endCourbeNiveau()
{
    bool ok = false;

    if (!_pModeler->askForResetResultat())
    {
        return;
    }

    double alti = QInputDialog::getDouble(getTYMainWnd(), getTYMainWnd()->windowTitle(), TR("id_msg_getaltitude"), 0, -10000, 10000, 2, &ok);

    if (ok && (this->getSavedPoints().size() != 0))
    {
        _pCrbNiv = new TYCourbeNiveau();
        _pCrbNiv->setListPoints(this->getSavedPoints());
        _pCrbNiv->setAltitude(alti);

        TYSiteNode* pSite = ((TYSiteModelerFrame*) _pModeler)->getSite();

        if (pSite->getTopographie()->addCrbNiv(_pCrbNiv))
        {
            TYAction* pAction = new TYAddElementToTopoAction((LPTYElement&) _pCrbNiv, pSite->getTopographie(), _pModeler, TR("id_action_addcrbniv"));
            _pModeler->getActionManager()->addAction(pAction);

            // On demande la mise à jour de l'altimetrie globale du site
            getTYMainWnd()->getSiteFrame()->altimetryNeedsUpdate();
            TYProjet* pProjet = getTYApp()->getCurProjet();
            if (pProjet)
            {
                pProjet->getSite()->getTopographie()->updateGraphicTree();
            }

            updateSiteFrame();
            _pModeler->getView()->getRenderer()->updateDisplayList();

            _pModeler->updateView();
        }

        // repasse en mode camera selection
        getTYMainWnd()->setDefaultCameraMode();
    }
}
