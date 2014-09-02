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
 * \file TYSiteModelerFrame.cpp
 * \brief Classe Modeler specialisee pour l'edition des sites
 */



#include <qmessagebox.h>
#include <qtooltip.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qcursor.h>
#include <qtoolbutton.h>
#include <qpixmap.h>

#include "Tympan/models/business/TYRectangularMaillage.h"
#include "Tympan/models/business/TYBoundaryNoiseMap.h"
#include "Tympan/models/business/TYXMLManager.h"
#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/TYPreferenceManager.h"
#include "Tympan/gui/gl/TYRectangularMaillageGraphic.h"
#include "Tympan/gui/gl/TYBoundaryNoiseMapGraphic.h"
#include "Tympan/gui/app/TYPickEditor.h"
#include "Tympan/gui/app/TYAbstractSceneEditor.h"
#include "Tympan/gui/app/TYEchelleEditor.h"
#include "Tympan/gui/app/TYPointRefEditor.h"
#include "Tympan/gui/app/TYOrientationEditor.h"
#include "Tympan/gui/app/TYEmpriseEditor.h"
#include "Tympan/gui/app/TYCourbeNiveauEditor.h"
#include "Tympan/gui/app/TYPlanEauEditor.h"
#include "Tympan/gui/app/TYSolEditor.h"
#include "Tympan/gui/app/TYRouteEditor.h"
#include "Tympan/gui/app/TYReseauTransportEditor.h"
#include "Tympan/gui/app/TYEtageEditor.h"
#include "Tympan/gui/app/TYSilosEditor.h"
#include "Tympan/gui/app/TYCoursEauEditor.h"
#include "Tympan/gui/app/TYVegetationEditor.h"
#include "Tympan/gui/app/TYSourceEditor.h"
#include "Tympan/gui/app/TYPointCalculEditor.h"
#include "Tympan/gui/app/TYRectangularMaillageEditor.h"
#include "Tympan/gui/app/TYLinearMaillageEditor.h"
#include "Tympan/gui/app/TYVerticalMaillageEditor.h"
#include "Tympan/gui/app/TYBoundaryNoiseMapEditor.h"
#include "Tympan/gui/app/TYApplication.h"
#include "TYSiteModelerFrame.h"


#define TR(id) OLocalizator::getString("TYSiteModelerFrame", (id))
#define IMG(id) OLocalizator::getPicture("TYSiteModelerFrame", (id))


int TYSiteModelerFrame::_nbInstance = 0;


TYSiteModelerFrame::TYSiteModelerFrame(QWidget* parent, const char* name, Qt::WFlags f) :
    TYModelerFrame(parent, name, f)
{
    init();
    setProjet(new TYProjet());

    setSelectMaillageBox(TR("id_none_select_maillage"));
    updateSelectMaillageBox();
    updateVisibilityElementSite();
}

TYSiteModelerFrame::TYSiteModelerFrame(LPTYProjet pProjet, QWidget* parent, const char* name, Qt::WFlags f) :
    TYModelerFrame(parent, name, f)
{
    init();

    assert(pProjet);
    setProjet(pProjet);

    setSelectMaillageBox(TR("id_none_select_maillage"));
    updateSelectMaillageBox();
    updateVisibilityElementSite();
}

TYSiteModelerFrame::TYSiteModelerFrame(LPTYSiteNode pSite, QWidget* parent, const char* name, Qt::WFlags f) :
    TYModelerFrame(parent, name, f)
{
    init();

    assert(pSite);
    setSite(pSite);

    setSelectMaillageBox(TR("id_none_select_maillage"));
    updateSelectMaillageBox();
    updateVisibilityElementSite();
}

void TYSiteModelerFrame::init()
{
    _nbInstance++;
    setWindowTitle(TR("id_caption") + " " + QString("%1").arg(_nbInstance));

    // Btn Show CrbNiv
    _showCrbNiv = true;
    _pShowCrbNivBtn = new QToolButton(this);
    _pShowCrbNivBtn->setIcon(QPixmap(IMG("id_icon_crbniv")));
    _pShowCrbNivBtn->setText(TR("id_show_crbniv_btn"));
    _pShowCrbNivBtn->setToolTip(TR("id_show_crbniv_btn"));
    _pShowCrbNivBtn->setCheckable(true);
    _pShowCrbNivBtn->setChecked(_showCrbNiv);
    _pShowCrbNivBtn->setFixedSize(24, 24);
    _pCtrlLayout->addWidget(_pShowCrbNivBtn, 0);
    connect(_pShowCrbNivBtn, SIGNAL(toggled(bool)), this, SLOT(showCrbNiv(bool)));

    // Btn Show Altimetrie
    _showAltimetrie = false;
    _pShowAltimetrieBtn = new QToolButton(this);
    _pShowAltimetrieBtn->setIcon(QPixmap(IMG("id_icon_altimetrie")));
    _pShowAltimetrieBtn->setText(TR("id_show_altimetrie_btn"));
    _pShowAltimetrieBtn->setToolTip(TR("id_show_altimetrie_btn"));
    _pShowAltimetrieBtn->setCheckable(true);
    _pShowAltimetrieBtn->setChecked(_showAltimetrie);
    _pShowAltimetrieBtn->setFixedSize(24, 24);
    _pCtrlLayout->addWidget(_pShowAltimetrieBtn, 0);
    connect(_pShowAltimetrieBtn, SIGNAL(toggled(bool)), this, SLOT(showAltimetrie(bool)));

    // Btn Show ImageSite
    _showImageSite = true;
    _pShowImageSiteBtn = new QToolButton(this);
    _pShowImageSiteBtn->setIcon(QPixmap(IMG("id_icon_imagesite")));
    _pShowImageSiteBtn->setText(TR("id_show_imagesite_btn"));
    _pShowImageSiteBtn->setToolTip(TR("id_show_imagesite_btn"));
    _pShowImageSiteBtn->setCheckable(true);
    _pShowImageSiteBtn->setChecked(_showImageSite);
    _pShowImageSiteBtn->setFixedSize(24, 24);
    _pCtrlLayout->addWidget(_pShowImageSiteBtn, 0);
    connect(_pShowImageSiteBtn, SIGNAL(toggled(bool)), this, SLOT(showImageSite(bool)));

    // Editors
    _pEchelleEditor = new TYEchelleEditor(this);
    _pPointRefEditor = new TYPointRefEditor(this);
    _pOrientationEditor = new TYOrientationEditor(this);
    _pEmpriseEditor = new TYEmpriseEditor(this);
    _pCourbeNiveauEditor = new TYCourbeNiveauEditor(this);
    _pPlanEauEditor = new TYPlanEauEditor(this);
    _pSolEditor = new TYSolEditor(this);
    _pRouteEditor = new TYRouteEditor(this);
    _pReseauTransportEditor = new TYReseauTransportEditor(this);
    _pEtageEditor = new TYEtageEditor(this);
    _pSilosEditor = new TYSilosEditor(this);
    _pCoursEauEditor = new TYCoursEauEditor(this);
    _pVegetationEditor = new TYVegetationEditor(this);
    _pSourceEditor = new TYSourceEditor(this);
    _pPointCalculEditor = new TYPointCalculEditor(this);
    _pRectangularMaillageEditor = new TYRectangularMaillageEditor(this);
    _pLinearMaillageEditor = new TYLinearMaillageEditor(this);
    _pVerticalMaillageEditor = new TYVerticalMaillageEditor(this);
    _pBoundaryNoiseMapEditor = new TYBoundaryNoiseMapEditor(this);

    _pCtrlLayout->addSpacing(10);

    // Combo box mode de Rendu
    _pRenderMaillageModeBox = new QComboBox(this);
    _pCtrlLayout->addWidget(_pRenderMaillageModeBox, 0);
    _pRenderMaillageModeBox->setToolTip(TR("id_render_maillage_mode"));
    _pRenderMaillageModeBox->insertItem(Surface, TR("id_surface_maillage_mode"));
    _pRenderMaillageModeBox->insertItem(Lines, TR("id_lines_maillage_mode"));
    _pRenderMaillageModeBox->insertItem(Mixed, TR("id_mixed_maillage_mode"));
    connect(_pRenderMaillageModeBox, SIGNAL(activated(int)), this, SLOT(setRenderMaillageModeSlot(int)));

    _pCtrlLayout->addSpacing(10);

    _pSelectMaillageBox = new QComboBox(this);
    _pCtrlLayout->addWidget(_pSelectMaillageBox, 0);
    _pSelectMaillageBox->setToolTip(TR("id_select_maillage"));
    connect(_pSelectMaillageBox, SIGNAL(activated(const QString&)), this, SLOT(setSelectMaillageBox(const QString&)));

    _pCtrlLayout->addSpacing(10);

    // Btn MaJ Altimetrie
    _pAltiBtn = new QToolButton(this);
    _pAltiBtn->setIcon(QPixmap(IMG("id_icon_calculalti_btn")));
    _pAltiBtn->setText(TR("id_calculalti_btn"));
    _pAltiBtn->setToolTip(TR("id_calculalti_btn"));
    _pCtrlLayout->addWidget(_pAltiBtn, 0);
    _pCtrlLayout->addStretch(1);
    _pAltiBtn->setFixedSize(24, 24);
    // Check if altimetry is up to date on the root site node.
    // If it is, no need to enable the button triggering its computation
    if (_pSite != nullptr &&_pSite->getProjet() != nullptr)
    {
        LPTYSiteNode rootSite = _pSite->getProjet()->getSite();
        if (rootSite->getAltimetryUpToDate())
        {
            _pAltiBtn->setEnabled(false);
        }
    }

    connect(_pAltiBtn, SIGNAL(clicked()), this, SLOT(calculAltimetrie()));

    // Vue de dessus
    setViewType(TopView);

    updatePreferences();
}

TYSiteModelerFrame::~TYSiteModelerFrame()
{
    _nbInstance--;

    if (_pSite)
    {
        _pSite->drawGraphic(false);
    }

    delete _pEchelleEditor;
    delete _pPointRefEditor;
    delete _pOrientationEditor;
    delete _pEmpriseEditor;
    delete _pCourbeNiveauEditor;
    delete _pPlanEauEditor;
    delete _pSolEditor;
    delete _pRouteEditor;
    delete _pReseauTransportEditor;
    delete _pEtageEditor;
    delete _pSilosEditor;
    delete _pCoursEauEditor;
    delete _pVegetationEditor;
    delete _pSourceEditor;
    delete _pPointCalculEditor;
    delete _pRectangularMaillageEditor;
    delete _pLinearMaillageEditor;
    delete _pVerticalMaillageEditor;
    delete _pBoundaryNoiseMapEditor;
}

bool TYSiteModelerFrame::close()
{
    return TYModelerFrame::close();
}

void TYSiteModelerFrame::setSite(LPTYSiteNode pSite)
{
    QString caption(TR("id_caption") + " " + QString("%1").arg(_nbInstance));

    if (_pSite)
    {
        _pSite->drawGraphic(false);
    }

    _pSite = pSite;
    _pElement = pSite;

    if (_pSite)
    {
        _pProjet = TYProjet::safeDownCast(_pSite->getParent());

        if (_pProjet)
        {
            _pElement = _pProjet;
        }

        if (!_pElement->getName().isEmpty())
        {
            caption += QString(" : %1").arg(_pElement->getName());
        }

        // On affiche le site dans ce renderer
        _pElement->drawGraphic();
    }

    setWindowTitle(caption);

    _pView->getRenderer()->setElement((LPTYElement&)_pSite);
    getView()->getRenderer()->updateDisplayList();

    updateView();
    //  fit();//az++
}

void TYSiteModelerFrame::setProjet(LPTYProjet pProjet)
{
    if (_pProjet)
    {
        _pProjet->drawGraphic(false);
    }

    if (pProjet)
    {
        _pProjet = pProjet;
    }

    setSite(pProjet->getSite());
}

void TYSiteModelerFrame::setViewType(int view)
{
    TYModelerFrame::setViewType(view);
}

void TYSiteModelerFrame::setRenderMode(int mode, bool bUpdateGL)
{
    TYModelerFrame::setRenderMode(mode, bUpdateGL);
}

void TYSiteModelerFrame::setRenderMaillageModeSlot(int mode)
{
    if ((mode < 0) || (mode >= NbOfRenderMaillageMode)) { return; }   //sanity check

    bool updateNeeded = false;

    // Les maillages ont leur propre type d'interpolation
    if (getProjet() && getProjet()->getCurrentCalcul())
    {
        LPTYCalcul pCalcul = getProjet()->getCurrentCalcul();

        // Pour chaque maillage
        for (unsigned int i = 0; i < pCalcul->getMaillages().size(); i++)
        {
            // Recuperation de l'objet graphique du maillage
            TYMaillage* pMaillage = pCalcul->getMaillage(i);
            TYElementGraphic* pMGraphic = pMaillage->getGraphicObject().getRealPointer();

            if (TYRectangularMaillageGraphic* pRectGraphic = dynamic_cast<TYRectangularMaillageGraphic*>(pMGraphic))
            {
                switch (mode)
                {
                    case Surface:
                        pRectGraphic->setRendering(TYRectangularMaillageGraphic::RENDER_SURF);
                        break;
                    case Lines:
                        pRectGraphic->setRendering(TYRectangularMaillageGraphic::RENDER_LINES);
                        break;
                    case Mixed:
                        pRectGraphic->setRendering(TYRectangularMaillageGraphic::RENDER_MIXED);
                        break;
                }

                pMaillage->updateGraphic();
                updateNeeded = true;
            }
            else if (TYBoundaryNoiseMapGraphic* pBoundGraphic = dynamic_cast<TYBoundaryNoiseMapGraphic*>(pMGraphic))
            {
                switch (mode)
                {
                    case Surface:
                        pBoundGraphic->setRendering(TYBoundaryNoiseMapGraphic::RENDER_SURF);
                        break;
                    case Lines:
                        pBoundGraphic->setRendering(TYBoundaryNoiseMapGraphic::RENDER_LINES);
                        break;
                    case Mixed:
                        pBoundGraphic->setRendering(TYBoundaryNoiseMapGraphic::RENDER_MIXED);
                        break;
                }

                pMaillage->updateGraphic();
                updateNeeded = true;
            }
        }
    }

    if (updateNeeded)
    {
        _pView->getRenderer()->updateDisplayList();
        updateView();
    }
}

void TYSiteModelerFrame::setEditorMode(int mode)
{
    if (!_editorModeAccepted)
    {
        getPickEditor()->usePopup(true);
        getPickEditor()->useHighlight(false);

        if (_pCurrentEditor)
        {
            _pCurrentEditor->disconnect();
            _pCurrentEditor->close();
        }

        _editorModeAccepted = true;

        switch (mode)
        {
            case EchelleMode:
                _pCurrentEditor = _pEchelleEditor;
                break;
            case PointRefMode:
                _pCurrentEditor = _pPointRefEditor;
                break;
            case OrientationMode:
                _pCurrentEditor = _pOrientationEditor;
                break;
            case EmpriseMode:
                _pCurrentEditor = _pEmpriseEditor;
                break;
            case CourbeNiveauMode:
                _pCurrentEditor = _pCourbeNiveauEditor;
                break;
            case TerrainMode:
                _pCurrentEditor = _pSolEditor;
                break;
            case VegetationMode:
                _pCurrentEditor = _pVegetationEditor;
                break;
            case PlanEauMode:
                _pCurrentEditor = _pPlanEauEditor;
                break;
            case CoursEauMode:
                _pCurrentEditor = _pCoursEauEditor;
                break;
            case SrcPonctMode:
                _pCurrentEditor = _pSourceEditor;
                break;
            case RecepteurMode:
                _pCurrentEditor = _pPointCalculEditor;
                break;
            case RouteMode:
                _pCurrentEditor = _pRouteEditor;
                break;
            case ReseauTransportMode:
                _pCurrentEditor = _pReseauTransportEditor;
                break;
            case EtageMode:
                _pCurrentEditor = _pEtageEditor;
                break;
            case SilosMode:
                _pCurrentEditor = _pSilosEditor;
                break;
            case MaillageRectMode:
                _pCurrentEditor = _pRectangularMaillageEditor;
                break;
            case MaillageLinearMode:
                _pCurrentEditor = _pLinearMaillageEditor;
                break;
            case MaillageVerticalMode:
                _pCurrentEditor = _pVerticalMaillageEditor;
                break;
            case BoundaryNoiseMapMode:
                _pCurrentEditor = _pBoundaryNoiseMapEditor;
                break;
            case PointsCtrlMode:
                _pCurrentEditor = _pPointCalculEditor;
                break;
            default:
                _editorModeAccepted = false;
        }
    }

    TYModelerFrame::setEditorMode(mode);
}

void TYSiteModelerFrame::updateView(bool clipping /*=true*/, bool axesAndGrid /*=true*/)
{
    // Appel de la methode parent
    TYModelerFrame::updateView(clipping, axesAndGrid);

    updateSelectMaillageBox();
}

void TYSiteModelerFrame::showCrbNiv(bool show)
{
    if (_showCrbNiv == show)
    {
        return;
    }

    _showCrbNiv = show;

    updateVisibilityElementSite();
    _pView->getRenderer()->updateDisplayList();
    updateView();

    _pShowCrbNivBtn->setChecked(_showCrbNiv);
}

void TYSiteModelerFrame::showAltimetrie(bool show)
{
    if (_showAltimetrie == show)
    {
        return;
    }

    _showAltimetrie = show;

    updateVisibilityElementSite();
    _pView->getRenderer()->updateDisplayList();
    updateView();

    _pShowAltimetrieBtn->setChecked(_showAltimetrie);
}

void TYSiteModelerFrame::showImageSite(bool show)
{
    _showImageSite = show;

    updateVisibilityElementSite();
    _pView->getRenderer()->updateDisplayList();
    updateView();

    _pShowImageSiteBtn->setChecked(_showImageSite);
}

void TYSiteModelerFrame::updateVisibilityElementSite()
{
    if (!_pSite)
    {
        return;
    }

    TYTabCourbeNiveauGeoNode* pTabCrbNivGeoNode = NULL;
    TYTabTerrainGeoNode* pTabTerrainGeoNode = NULL;
    TYTabPlanEauGeoNode* pTabPlanEauGeoNode = NULL;

    // Collecte les site nodes de facon recursive
    TYTabSiteNodeGeoNode sites;

    // On collecte l'ensemble des sites
    sites = _pSite->collectSites();

    // Pour chaque site
    for (unsigned int i = 0; i < sites.size(); i++)
    {
        // Collecte des courbes
        pTabCrbNivGeoNode = &(TYSiteNode::safeDownCast(sites[i]->getElement())->getTopographie()->getListCrbNiv());

        // On recupere tous les actors constituant les courbes de niveau
        for (unsigned int j = 0; j < pTabCrbNivGeoNode->size(); j++)
        {
            pTabCrbNivGeoNode->at(j)->getElement()->getGraphicObject()->setVisible(_showCrbNiv);
        }

        // Collecte des terrains et des plans d'eau
        pTabTerrainGeoNode = &(TYSiteNode::safeDownCast(sites[i]->getElement())->getTopographie()->getListTerrain());
        for (unsigned int j = 0; j < pTabTerrainGeoNode->size(); j++)
        {
            pTabTerrainGeoNode->at(j)->getElement()->getGraphicObject()->setVisible(_showAltimetrie);
        }

        pTabPlanEauGeoNode = &(TYSiteNode::safeDownCast(sites[i]->getElement())->getTopographie()->getListPlanEau());
        for (unsigned int j = 0; j < pTabPlanEauGeoNode->size(); j++)
        {
            pTabPlanEauGeoNode->at(j)->getElement()->getGraphicObject()->setVisible(_showAltimetrie);
        }
        sites[i]->getElement()->getGraphicObject()->setVisible(false);
    }

    // Pour l'altimetrie (seul le site root a une alti)
    _pSite->getAltimetry()->getGraphicObject()->setVisible(_showAltimetrie);

    // Mise a jour de la transparence de l'altimetrie
    updateAltiOpacity();

    _pSite->getGraphicObject()->setVisible(_showImageSite);
}

void TYSiteModelerFrame::enableAltimetryComputation()
{
    _pAltiBtn->setEnabled(true);
}
void TYSiteModelerFrame::disableAltimetryComputation()
{
    _pAltiBtn->setEnabled(false);
}
void TYSiteModelerFrame::calculAltimetrie()
{
    // Called when _pAltiBtn button is pressed
    if (!_pSite) { return; }

    TYApplication::setOverrideCursor(Qt::WaitCursor);

    writeOutputMsg(TR("id_msg_calculalti"));

    // Retrieve the root SiteNode to compute altimetry
    LPTYSiteNode rootSite = _pSite->getProjet()->getSite();
    // One global altimetry update for all the sites
    rootSite->updateAltimetrie();
    // This method uses the altimetry above-computed to update the infrastructure
    // as well as the receptors and sources and recurses on the current site and
    // all its subsites
    rootSite->update();

    if (_pProjet)
    {
        writeOutputMsg(TR("id_msg_updatealtimaillages"));
        _pProjet->updateAltiRecepteurs();
    }

    _pSite->updateGraphicTree();
    getView()->getRenderer()->updateDisplayList();
    updateView();

    _pAltiBtn->setEnabled(false);
    TYApplication::restoreOverrideCursor();
}

void TYSiteModelerFrame::updateAltiOpacity(int view /*=-1*/)
{
    if (!_pSite) { return; }
}

bool TYSiteModelerFrame::computeCurPos(int x, int y, float* ret)
{
    int view = getCurrentView();
    if ((view != PerspView) && (view != FreeView))
    {
        NxVec3 pos = OGLCamera::displayToWorld(NxVec3(x, _pView->height() - y, 0));
        ret[0] = pos.x;
        ret[1] = pos.y;
        ret[2] = pos.z;

        switch (getCurrentView())
        {
            case TopView:
                ret[1] = 0.0;
                break;
            case LeftView:  ret[0] = 0.0; break;
            case FrontView: ret[2] = 0.0; break;
        }

        return true;
    }

    return false;
}

void TYSiteModelerFrame::updatePreferences()
{
    // Grille
    _gridDimX = 4000.0f;
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "GridDimXSite"))
    {
        _gridDimX = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, "GridDimXSite");
    }
    else
    {
        TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, "GridDimXSite", _gridDimX);
    }

    _gridDimY = 4000.0f;
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "GridDimYSite"))
    {
        _gridDimY = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, "GridDimYSite");
    }
    else
    {
        TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, "GridDimYSite", _gridDimY);
    }

    _gridStep = 500.0f;
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "GridStepSite"))
    {
        _gridStep = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, "GridStepSite");
    }
    else
    {
        TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, "GridStepSite", _gridStep);
    }

    _gridMagnStep = 5.0f;
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "GridMagnStepSite"))
    {
        _gridMagnStep = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, "GridMagnStepSite");
    }
    else
    {
        TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, "GridMagnStepSite", _gridMagnStep);
    }

    resizeGrid();
    setGridLinesActorsVisibility((_curViewType == FrontView), (_curViewType == TopView) || (_curViewType == PerspView), (_curViewType == LeftView));

    TYModelerFrame::updatePreferences();
}

void TYSiteModelerFrame::updateSelectMaillageBox()
{
    _pSelectMaillageBox->clear();

    // Compteur
    unsigned int counter = 0;

    // Pas d'element clicke
    bool isNull = true;

    // Premier element vide
    _pSelectMaillageBox->insertItem(counter++, TR("id_none_select_maillage"));

    // Les maillages ont leur propre type d'interpolation
    if (getProjet() && getProjet()->getCurrentCalcul())
    {
        LPTYCalcul pCalcul = getProjet()->getCurrentCalcul();

        // Pour chaque maillage
        for (unsigned int i = 0; i < pCalcul->getMaillages().size(); ++i)
        {
            // Recuperation du maillage
            TYMaillage* pMaillage = pCalcul->getMaillage(i);

            // Si il est actif, on le fait apparaitre dans la liste
            if (pMaillage->getState() == TYMaillage::Actif)
            {
                _pSelectMaillageBox->insertItem(counter++, pMaillage->getName());
            }

            // Si l'element est picke, on le selectionne dans la liste
            if (pMaillage->getGraphicObject()->getHighlightState())
            {
                _pSelectMaillageBox->setCurrentIndex(counter - 1);
                isNull = false;
            }
        }

        if (isNull)
        {
            setSelectMaillageBox(TR("id_none_select_maillage"));
        }
    }
}

void TYSiteModelerFrame::setSelectMaillageBox(const QString& name)
{
    bool isNull = false;
    if (name == TR("id_none_select_maillage"))
    {
        isNull = true;
    }

    if (getProjet() && getProjet()->getCurrentCalcul())
    {
        LPTYCalcul pCalcul = getProjet()->getCurrentCalcul();

        // Pour chaque maillage
        for (unsigned int i = 0; i < pCalcul->getMaillages().size(); i++)
        {
            // Recuperation de l'objet graphique du maillage
            TYMaillage* pMaillage = pCalcul->getMaillage(i);

            if (isNull)
            {
                if (pMaillage->getState() == TYMaillage::Actif)
                {
                    pMaillage->getGraphicObject()->setVisible(true);
                    if (pMaillage->getGraphicObject()->getHighlightState())
                    {
                        getPickEditor()->getPicker()->reset();
                        pMaillage->getGraphicObject()->highlight(false);
                        getView()->getRenderer()->updateDisplayList();
                    }
                }
            }
            else if (pMaillage->getName() == name)
            {
                pMaillage->getGraphicObject()->setVisible(true);
                getPickEditor()->getPicker()->highlightElement(pMaillage);
                _pView->getRenderer()->updateDisplayList();
            }
            else
            {
                if (pMaillage->getState() == TYMaillage::Actif)
                {
                    pMaillage->getGraphicObject()->setVisible(false);
                    getView()->getRenderer()->updateDisplayList();
                }
            }
        }
    }
}
