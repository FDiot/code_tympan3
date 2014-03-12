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
 * \file TYSourceEditor.cpp
 * \brief gestion de l'edition d'une source
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYUserSourcePonctuelle.h"

#include "Tympan/Tools/OLocalizator.h"

#include <qinputdialog.h>

#if defined(WIN32)
#include <crtdbg.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif


#define TR(id) OLocalizator::getString("TYSourceEditor", (id))


TYSourceEditor::TYSourceEditor(TYModelerFrame* pModeler) :
    TYAbstractSceneEditor(pModeler)
{
    _active = false;

    slotViewTypeChanged(_pModeler->getCurrentView());
}

TYSourceEditor::~TYSourceEditor()
{
}

void TYSourceEditor::init()
{
}

void TYSourceEditor::close()
{
}

void TYSourceEditor::slotViewTypeChanged(int view)
{
    if (view == TYModelerFrame::TopView)
    {
        _active = true;
    }
    else
    {
        _active = false;
    }
}

void TYSourceEditor::slotMousePressed(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
{
    if ((button == Qt::LeftButton) && _active)
    {
    }
}

void TYSourceEditor::slotMouseMoved(int x, int y, Qt::MouseButtons button, Qt::KeyboardModifiers state)
{
    if ((button == Qt::LeftButton) && _active)
    {
    }
}

void TYSourceEditor::slotMouseReleased(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
{
    if ((button == Qt::LeftButton) && _active)
    {
        if (!_pModeler->askForResetResultat())
        {
            return;
        }

        // Hauteur pour ce point de controle
        bool ok = false;
        double hauteur = QInputDialog::getDouble(_pModeler, "", TR("id_msg_gethauteur"), 2.0, -1000.0, 1000.0, 2, &ok);

        if (ok)
        {
            double point[4];
            TYUserSourcePonctuelle* pSrcPonct = new TYUserSourcePonctuelle();

            // Pos
            displayToWorld(x, _pInteractor->height() - y, 0.0, point);

            // Si la grille magnetique est activee
            if (_pModeler->getSnapGridActive())
            {
                snapToGrid(point[0], point[1], point[2]);
            }

            // Init source
            pSrcPonct->setHauteur(hauteur);
            pSrcPonct->getPos()->setFromOGL(point);
            pSrcPonct->getPos()->_z = 0.0;
            pSrcPonct->getOrientation()._x = 1.0;

            if (QString(_pModeler->metaObject()->className()).compare("TYSiteModelerFrame") == 0)
            {
                TYSiteNode* pSite = ((TYSiteModelerFrame*) _pModeler)->getSite();

                // Ajout
                if (pSite->getInfrastructure()->addSrc(pSrcPonct))
                {
                    // On ajoute cette source a la selection du calcul courant
                    if (getTYApp()->getCurProjet() && (getTYApp()->getCurProjet()->getSite() == pSite))
                    {
                        LPTYCalcul pCalcul = getTYApp()->getCurProjet()->getCurrentCalcul();

                        if (pCalcul)
                        {
                            pCalcul->addToSelection(pSrcPonct);
                        }
                    }

                    // Action
                    TYAction* pAction = new TYAddElementToInfraAction((LPTYElement&) pSrcPonct, pSite->getInfrastructure(), _pModeler, TR("id_action_addsource"));
                    _pModeler->getActionManager()->addAction(pAction);

                    _pModeler->getView()->getRenderer()->updateDisplayList();

                    // Update
                    pSite->getInfrastructure()->updateGraphicTree();
                    _pModeler->updateView();
                    updateSiteFrame(); // Mise a jour de l'arborescence de site
                }
            }
            else if (QString(_pModeler->metaObject()->className()).compare("TYBatimentModelerFrame") == 0)
            {
                pSrcPonct->getPos()->_z = hauteur;

                TYBatiment* pBat = ((TYBatimentModelerFrame*) _pModeler)->getBatiment();
                LPTYEtage pEtage = (TYEtage*) pBat->getAcousticVol(0).getRealPointer();

                if (pEtage)
                {
                    bool addOk = pEtage->addSource(pSrcPonct);

                    if (addOk) // Si l'action d'ajouter a ete effectuee sans probleme
                    {
                        // Action
                        TYAction* pAction = new TYAddElementToEtageAction((LPTYElement&) pSrcPonct, pEtage, _pModeler, TR("id_action_addsource"));
                        _pModeler->getActionManager()->addAction(pAction);
                    }

                    //Update
                    pEtage->updateGraphicTree();
                    updateSiteFrame();
                    _pModeler->getView()->getRenderer()->updateDisplayList();
                    _pModeler->updateView();
                }
            }

            TYElement::setIsSavedOk(true);
        }
    }
}
