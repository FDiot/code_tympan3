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
 * \file TYReseauTransportEditor.cpp
 * \brief Construit un ReseauTransport a partir des points saisis
 *
 *
 */





#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/Tools/OLocalizator.h"


#include "Tympan/models/business/infrastructure/TYReseauTransport.h"

#include <qinputdialog.h>


#include "TYRenderWindowInteractor.h"
#include "TYModelerFrame.h"
#include "TYSiteModelerFrame.h"
#include "TYActions.h"

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


#define TR(id) OLocalizator::getString("TYReseauTransportEditor", (id))


TYReseauTransportEditor::TYReseauTransportEditor(TYModelerFrame* pModeler) :
    TYPolyLineEditor(pModeler)
{
    QObject::connect(this, SIGNAL(endedSavingPoints()), this, SLOT(endReseauTransport()));
}

TYReseauTransportEditor::~TYReseauTransportEditor()
{
}

void TYReseauTransportEditor::endReseauTransport()
{
    if (!_pModeler->askForResetResultat())
    {
        return;
    }

    LPTYReseauTransport pReseauTransport = new TYReseauTransport();

    if (pReseauTransport->edit(_pModeler) == QDialog::Accepted)
    {
        pReseauTransport->setTabPoint(this->getSavedPoints());

        TYSiteNode* pSite = ((TYSiteModelerFrame*)_pModeler)->getSite();

        for (unsigned int i = 0; i < pReseauTransport->getTabPoint().size(); i++)
        {
            pReseauTransport->getTabPoint()[i]._z = 0.0;

            // Altitude
            pSite->getTopographie()->getAltimetrie()->updateAltitude(pReseauTransport->getTabPoint()[i]);

            // Ajout de la hauteur moyenne
            pReseauTransport->getTabPoint()[i]._z += pReseauTransport->getHauteurMoyenne();
        }

        if (pSite->getInfrastructure()->addResTrans(pReseauTransport))
        {
            // On ajoute ce reseau a la selection du calcul courant
            if (getTYApp()->getCurProjet() && (getTYApp()->getCurProjet()->getSite() == pSite))
            {
                LPTYCalcul pCalcul = getTYApp()->getCurProjet()->getCurrentCalcul();

                if (pCalcul)
                {
                    pCalcul->addToSelection(pReseauTransport);
                }
            }

            TYAction* pAction = new TYAddElementToInfraAction((LPTYElement&) pReseauTransport, pSite->getInfrastructure(), _pModeler, TR("id_action_addrestrans"));
            _pModeler->getActionManager()->addAction(pAction);

            pSite->getInfrastructure()->updateGraphicTree();
            refreshSiteFrame();
            _pModeler->getView()->getRenderer()->updateDisplayList();
            _pModeler->updateView();
        }

        // repasse en mode camera selection
        getTYMainWnd()->setDefaultCameraMode();
    }
}
