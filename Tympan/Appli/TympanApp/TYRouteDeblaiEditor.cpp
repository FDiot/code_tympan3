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
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/Tools/OLocalizator.h"


#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYRouteDeblai.h"
#include "Tympan/GraphicIHM/DataManagerIHM/TYRouteDeblaiWidget.h"

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


#define TR(id) OLocalizator::getString("TYRouteDeblaiEditor", (id))


TYRouteDeblaiEditor::TYRouteDeblaiEditor(TYModelerFrame* pModeler) :
    TYPolyLineEditor(pModeler)
{
    QObject::connect(this, SIGNAL(endedSavingPoints()), this, SLOT(endRouteDeblai()));
}

TYRouteDeblaiEditor::~TYRouteDeblaiEditor()
{
}

void TYRouteDeblaiEditor::endRouteDeblai()
{
    if (!_pModeler->askForResetResultat())
    {
        return;
    }

    LPTYRouteDeblai pRoute = new TYRouteDeblai();

    if (pRoute->edit(_pModeler) == QDialog::Accepted)
    {
        pRoute->setTabPoint(this->getSavedPoints());

        TYSiteNode* pSite = ((TYSiteModelerFrame*)_pModeler)->getSite();

        for (unsigned int i = 0; i < pRoute->getTabPoint().size(); i++)
        {
            pRoute->getTabPoint()[i]._z = 0.0;
            pSite->getTopographie()->getAltimetrie()->updateAltitude(pRoute->getTabPoint()[i]);
        }

        if (pSite->getInfrastructure()->addRoute(pRoute))
        {
            // On ajoute cette route a la selection du calcul courant
            if (getTYApp()->getCurProjet() && (getTYApp()->getCurProjet()->getSite() == pSite))
            {
                LPTYCalcul pCalcul = getTYApp()->getCurProjet()->getCurrentCalcul();

                if (pCalcul)
                {
                    pCalcul->addToSelection(pRoute);
                }
            }

            TYAction* pAction = new TYAddElementToInfraAction((LPTYElement&) pRoute, pSite->getInfrastructure(), _pModeler, TR("id_action_addroutedeblai"));
            _pModeler->getActionManager()->addAction(pAction);

            pSite->getInfrastructure()->updateGraphicTree();
            refreshSiteFrame();
            _pModeler->getView()->getRenderer()->updateDisplayList();
            _pModeler->updateView();
        }
    }
    else
    {
        delete pRoute;
    }
}
