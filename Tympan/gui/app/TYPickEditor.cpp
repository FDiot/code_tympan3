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
 * \file TYPickEditor.cpp
 * \brief gestion de l'element actionne par picking
 */


#include <qdialog.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qmenu.h>
#include <qpainter.h>
#include <qcursor.h>
#include <qinputdialog.h>
#include <QMessageBox>
#include <QBoxLayout>
#include <QPixmap>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QMdiArea>
#include <QMdiSubWindow>

#include "Tympan/core/logging.h"
#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/business/TYProjet.h"
#include "Tympan/models/business/TYCalcul.h"
#include "Tympan/models/business/infrastructure/TYSiteNode.h"
#include "Tympan/models/business/infrastructure/TYBatiment.h"
#include "Tympan/models/business/infrastructure/TYEcran.h"
#include "Tympan/models/business/infrastructure/TYInfrastructure.h"
#include "Tympan/models/business/TYRectangularMaillage.h"
#include "Tympan/gui/gl/TYPickHandler.h"
#include "Tympan/gui/gl/TYPanelGraphic.h"
#include "Tympan/gui/gl/TYRectangularMaillageGraphic.h"
#include "Tympan/gui/widgets/TYWidget.h"
#include "Tympan/gui/app/TYMenuItem.h"
#include "Tympan/gui/app/TYSiteFrame.h"
#include "Tympan/gui/app/TYModelerFrame.h"
#include "Tympan/gui/app/TYFaceModelerFrame.h"
#include "Tympan/gui/app/TYSiteModelerFrame.h"
#include "Tympan/gui/app/TYRenderWindowInteractor.h"
#include "Tympan/gui/app/TYRotationDialog.h"
#include "Tympan/gui/app/TYPositionDialog.h"
#include "Tympan/gui/app/TYDimensionDialog.h"
#include "Tympan/gui/app/TYActions.h"
#include "Tympan/gui/app/TYApplication.h"
#include "Tympan/gui/app/TYMainWindow.h"
#include "TYPickEditor.h"

#define TR(id) OLocalizator::getString("TYPickEditor", (id))
#define IMG(id) OLocalizator::getPicture("TYPickEditor", (id))

using namespace Qt;


TYPickEditor::TYPickEditor(TYModelerFrame* pModeler) :
    TYAbstractSceneEditor(pModeler)
{
    _useHighlight = true;
    _usePopup = true;

    // On recupere le pointeur sur le picker du modeler
    _pPicker = _pModeler->getElementPicker();

    setPickTolerance(3.0f);
    setRolloverTolerance(0.0f);

    _pLastRolloverElt = 0;
}

TYPickEditor::~TYPickEditor()
{
}

void TYPickEditor::slotMousePressed(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
{
    if (_pModeler->hasFocus() && _pLastRolloverElt)
    {
        if(dynamic_cast<TYMaillage*>(_pLastRolloverElt) != nullptr)
        {
            hidePanel(_pLastRolloverElt);
            _pModeler->getView()->updateGL();
        }
    }


    _lastPressedCurPos.setX(x);
    _lastPressedCurPos.setY(y);
}

void TYPickEditor::slotMouseReleased(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
{
    if ((x >= _lastPressedCurPos.x() + _pickTolerance) || (x <= _lastPressedCurPos.x() - _pickTolerance) ||
        (y >= _lastPressedCurPos.y() + _pickTolerance) || (y <= _lastPressedCurPos.y() - _pickTolerance))
    {
        return;
    }

    bool doPick = false;

    if ((button == Qt::RightButton) || (button == Qt::LeftButton))
    {
        // Connections
        if (_useHighlight || (button == Qt::RightButton))
        {
            QObject::connect(_pPicker, SIGNAL(elementPicked(TYElement*)), _pPicker, SLOT(highlightElement(TYElement*)));
            doPick = true;
        }
        if ((button == Qt::RightButton) && _usePopup)
        {
            QObject::connect(_pPicker, SIGNAL(elementCollectionPicked(std::shared_ptr<LPTYElementArray>)), this, SLOT(showPopupMenu(std::shared_ptr<LPTYElementArray>)));
            doPick = true;
        }

        // Picking
        if (doPick)
        {
            if (!_pPicker->pick(x, _pInteractor->height() - y))
            {
                resetPicker();
            }

            // Deconnections
            QObject::disconnect(_pPicker, SIGNAL(elementPicked(TYElement*)), _pPicker, SLOT(highlightElement(TYElement*)));
            QObject::disconnect(_pPicker, SIGNAL(elementCollectionPicked(std::shared_ptr<LPTYElementArray>)), this, SLOT(showPopupMenu(std::shared_ptr<LPTYElementArray>)));
        }
    }
}

void TYPickEditor::slotMouseMoved(int x, int y, Qt::MouseButtons button, Qt::KeyboardModifiers state)
{
    // Si un bouton est enfonce, on retourne
    if ((button == Qt::RightButton) || (button == Qt::LeftButton))
    {
        return;
    }

    _lastMovedCurPos.setX(x);
    _lastMovedCurPos.setY(y);

    // Si le modeleur peut contenir un maillage (TYSiteModelerFrame)
    if ((QString(_pModeler->metaObject()->className()).compare("TYSiteModelerFrame") == 0) && _pModeler->hasFocus())
    {
        // On cherche a savoir si au moins un maillage est visible
        bool hasMaillage = false;
        LPTYCalcul pCalc = NULL;
        if (getTYApp()->getCurProjet() && getTYApp()->getCurProjet()->getCurrentCalcul())
        {
            pCalc = getTYApp()->getCurProjet()->getCurrentCalcul();
            TYTabMaillageGeoNode& pMaillages = pCalc->getMaillages();
            for (int i = 0; i < pMaillages.size(); ++i)
            {
                if (pMaillages[i]->getGraphicObject()->getVisible())
                {
                    hasMaillage = true;
                    break;
                }
            }
        }

        // Si au moins un maillage est visible
        if (hasMaillage)
        {
            // Si le modeleur n'est pas en vue en perspective
            if (_pModeler->getCurrentView() != TYModelerFrame::PerspView)
            {
                // Si aucun bouton n'est enfonce
                if (state == Qt::NoButton)
                {
                    // Connection
                    QObject::connect(_pPicker, SIGNAL(elementPicked(TYElement*)), this, SLOT(showPanel(TYElement*)));

                    // Picking
                    if (!_pPicker->pick(x, _pInteractor->height() - y))
                    {
                        hidePanel(_pLastRolloverElt);
                    }

                    // Deconnection
                    QObject::disconnect(_pPicker, SIGNAL(elementPicked(TYElement*)), this, SLOT(showPanel(TYElement*)));

                    // On met a jour la vue
                    _pModeler->getView()->updateGL();
                }
            }
        }
    }
}

void TYPickEditor::slotWheeled(int x, int y, int delta, Qt::KeyboardModifiers state)
{
    if (_pModeler->hasFocus() && _pLastRolloverElt)
    {
        if(dynamic_cast<TYMaillage*>(_pLastRolloverElt) != nullptr)
        {
            hidePanel(_pLastRolloverElt);
            _pModeler->getView()->updateGL();
        }
    }
}

void TYPickEditor::slotKeyPressed(int key)
{
    if (key == Qt::Key_Escape)
    {
        // (un updateGL est fait dans le reset)
        resetPicker();
    }
}

void TYPickEditor::slotMouseLeave()
{
    if (_pModeler->hasFocus() && _pLastRolloverElt)
    {
        if(dynamic_cast<TYMaillage*>(_pLastRolloverElt) != nullptr)
        {
            hidePanel(_pLastRolloverElt);
            _pModeler->getView()->updateGL();
        }
    }
}

// TODO : What's this monster ?!? Circa 1300 lines of code full of if ?!?
void TYPickEditor::showPopupMenu(std::shared_ptr<LPTYElementArray> pElts)
{
    if (!pElts)
    {
        return;
    }

    if (QString(_pModeler->metaObject()->className()).compare("TYSiteModelerFrame") == 0)
    {
        siteModelerPopupMenu(pElts);
    }
    else if (QString(_pModeler->metaObject()->className()).compare("TYMachineModelerFrame") == 0)
    {
        machineModelerPopupMenu(pElts);
    }
    else if (QString(_pModeler->metaObject()->className()).compare("TYBatimentModelerFrame") == 0)
    {
        batimentModelerPopupMenu(pElts);
    }

    // Deselection
    resetPicker();

    TYProjet* pProjet = getTYApp()->getCurProjet();
    if (pProjet)
    {
        pProjet->getSite()->getTopographie()->updateGraphicTree();
    }
    // Update
    getTYMainWnd()->updateModelers(true, true, true);//xbh++

}

void TYPickEditor::siteModelerPopupMenu(std::shared_ptr<LPTYElementArray> pElts)
{
    // Check site existance
    TYSiteModelerFrame* pModelerFrame = dynamic_cast<TYSiteModelerFrame*> (_pModeler);
    TYSiteNode* pModelerSite = nullptr;
    if (pModelerFrame) 
    {
        pModelerSite = pModelerFrame->getSite();
    }
    if ( pModelerSite == nullptr ) { return; }

    QMenu* pPopup = new QMenu(NULL);

    std::map<QAction*, int> retCodes;
    std::map<QAction*, TYGeometryNode*> posRetCodes;
    std::map<QAction*, TYGeometryNode*> rotRetCodes;
    std::map<QAction*, TYSiteNode*> copySiteNodeRetCodes;
    std::map<QAction*, TYSiteNode*> remSiteNodeRetCodes;
    std::map<QAction*, TYAcousticVolumeNode*> calculVolNodeRetCodes;
    QAction* inverseNormales = NULL;
    std::map<QAction*, TYMaillage*> copyMaillageRetCodes;
    std::map<QAction*, TYMaillage*> remMaillageRetCodes;
    std::map<QAction*, TYPointControl*> copyPtControlRetCodes;
    std::map<QAction*, TYPointControl*> remPtControlRetCodes;
    QAction* split = NULL;
    std::map<QAction*, TYElement*> copyTopoRetCodes;
    std::map<QAction*, TYElement*> remTopoRetCodes;
    std::map<QAction*, TYElement*> copyInfraRetCodes;
    std::map<QAction*, TYElement*> remInfraRetCodes;
    QAction* code = NULL;

    LPTYElement pRemovedEltParent = NULL;
    LPTYElement pAddedElt = NULL;

    int volumeFound = -2, levelCurveFound = -2;

    QFont font = pPopup->font();
    font.setBold(true);
    TYElement *elem_0 = nullptr; // Currrent element
    TYElement *elem_1 = nullptr; // next in queue
    TYElement *elem_2 = nullptr; // 2nd level next in queue
    for (unsigned int i = 0; i < pElts->size(); i++)
    {
        elem_0 = pElts->at(i); // Currrent element
        if ( (i+1) < pElts->size() ) { elem_1 = pElts->at(i + 1)._pObj; }
        if ( (i+2) < pElts->size() ) { elem_2 = pElts->at(i + 2)._pObj; }


        TYSiteNode *pCurrentSite = dynamic_cast<TYSiteNode*>(elem_0);

        // Edition des proprietes de l'element (quelque soit l'element)
        QString labelTxt;
        if ( (pCurrentSite != nullptr) && (pCurrentSite->getRoot()) )
        {
            labelTxt = TR("id_popup_siteroot");
        }
        else
        {
            labelTxt = TYWidget::getDisplayName(pElts->at(i));
        }
        code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_editeelt"))), labelTxt);
        code->setFont(font);
        retCodes[code] = i;

        if (pCurrentSite != nullptr)
        {
            // Recherche du site parent
            TYSiteNode *pCurrentSiteParent = nullptr;
            if (i+1 < pElts->size())
            {
                pCurrentSiteParent = dynamic_cast<TYSiteNode*>(elem_1);
            }

            if (pCurrentSiteParent == pModelerSite)
            {
                // GeoNode
                TYGeometryNode* pEltGeoNode = TYGeometryNode::GetGeoNode(pCurrentSite); 

                // Position
                code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_moving"))), TR("id_popup_position"));
                posRetCodes[code] = pEltGeoNode;

                // Rotation
                code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_rotation"))), TR("id_popup_rotation"));
                rotRetCodes[code] = pEltGeoNode;

                // Duplication
                code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_duplicate"))), TR("id_popup_duplicate"));
                copySiteNodeRetCodes[code] = pCurrentSite;

                // Suppression
                code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_del"))), TR("id_popup_remove"));
                remSiteNodeRetCodes[code] = pCurrentSite;
            }
        }
        
        if (dynamic_cast<TYAcousticVolume*>(elem_0) != nullptr)
        {
            // Dans tous les cas, on offre la possibilite d'inverser la normales des faces
            volumeFound = i;
            inverseNormales = pPopup->addAction(TR("id_popup_normales"));
        }
        
        TYMaillage *pMaillage = dynamic_cast<TYMaillage*>(elem_0);        
        if ( pMaillage != nullptr )
        {
            // Calcul parent
            if ( dynamic_cast<TYCalcul*>(elem_1) != nullptr )
            {
                // Si le maillage est bien present dans le calcul
                TYMaillageGeoNode* pMaillageGeoNode = TYGeometryNode::GetGeoNode(elem_0);
                if (pMaillageGeoNode != nullptr)
                {
                    // Position via le parent (GeoNode)
                    code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_moving"))), TR("id_popup_position"));
                    posRetCodes[code] = dynamic_cast<TYGeometryNode*>(pMaillageGeoNode);

                    // Rotation via le parent (GeoNode)
                    code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_rotation"))), TR("id_popup_rotation"));
                    rotRetCodes[code] = dynamic_cast<TYGeometryNode*>(pMaillageGeoNode);

                    // Duplication
                    code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_duplicate"))), TR("id_popup_duplicate"));
                    copyMaillageRetCodes[code] = pMaillage;

                    // Suppression
                    code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_del"))), TR("id_popup_remove"));
                    remMaillageRetCodes[code] = pMaillage;
                }
            }
        }

        TYPointControl *pPointCtrl = dynamic_cast<TYPointControl*>(pElts->at(i)._pObj);
        if (pPointCtrl != nullptr)
        {
            // Projet parent
            if ( dynamic_cast<TYProjet*>(elem_1) != nullptr )
            {
                // Duplication
                code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_duplicate"))), TR("id_popup_duplicate"));
                copyPtControlRetCodes[code] = pPointCtrl;

                // Suppression
                code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_del"))), TR("id_popup_remove"));
                remPtControlRetCodes[code] = pPointCtrl;
            }
        }

        if (dynamic_cast<TYTopographie*>(elem_1) != nullptr)
        {

            TYSiteNode* pSite = dynamic_cast<TYSiteNode*>(elem_2);
            if (pSite == pModelerSite)
            {
                // On ne peut dupliquer ou supprimer que dans le modeler de son site
                if (dynamic_cast<TYCourbeNiveau*>(pElts->at(i)._pObj) != nullptr)
                {
                    split = pPopup->addAction(TR("id_popup_split"));
                    levelCurveFound = i;
                }
                // Duplication
                code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_duplicate"))), TR("id_popup_duplicate"));
                copyTopoRetCodes[code] = pElts->at(i);

                // Suppression
                code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_del"))), TR("id_popup_remove"));
                remTopoRetCodes[code] = pElts->at(i);
            }
        }

        if ( dynamic_cast<TYInfrastructure*>(elem_1) != nullptr )
        {
            // Acoustic computation is always possible
            if ( dynamic_cast<TYAcousticVolumeNode*>( elem_0 ) )
            {
                // Calcul acoustique
                code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_calcul"))), TR("id_popup_calculer"));
                calculVolNodeRetCodes[code] = dynamic_cast<TYAcousticVolumeNode*>( elem_0 );
            }

            // Geometric changes only possible in parent site meodeler
            TYSiteNode* pSite = dynamic_cast<TYSiteNode*>(elem_2);
            if (pSite == pModelerSite)
            {
                // GeoNode
                TYGeometryNode* pEltGeoNode = TYGeometryNode::GetGeoNode( elem_0 );

                // Position via le parent (GeoNode)
                code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_moving"))), TR("id_popup_position"));
                posRetCodes[code] = pEltGeoNode;

                // Rotation via le parent (GeoNode)
                code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_rotation"))), TR("id_popup_rotation"));
                rotRetCodes[code] = pEltGeoNode;

                // Duplication
                code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_duplicate"))), TR("id_popup_duplicate"));
                copyInfraRetCodes[code] = elem_0;

                // Suppression
                code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_del"))), TR("id_popup_remove"));
                remInfraRetCodes[code] = elem_0;
            }
        }
        // Ajoute un separateur entre les elements
        pPopup->addSeparator();
    }

    // Gestion du choix de l'utilisateur
    pPopup->setMouseTracking(true);
    bool bUpdateDisplayList = false;
    QAction* popupRet = pPopup->exec(QCursor::pos());
    if (popupRet == NULL) { return; }
    qApp->processEvents();

    if (retCodes.find(popupRet) != retCodes.end())
    {
        TYElement *pElement = pElts->at(retCodes[popupRet]);
        manageProperties(pElement);
    }
    else if (posRetCodes.find(popupRet) != posRetCodes.end())
    {
        bool bHeight = true;
        if ( dynamic_cast<TYGeometryNode*>(posRetCodes[popupRet])->getElement()->isA("TYSiteNode") ) 
        { 
            bHeight = false; 
        }
        showPositionDialog(posRetCodes[popupRet], bHeight);
    }
    else if (rotRetCodes.find(popupRet) != rotRetCodes.end())
    {
        showRotationDialog(rotRetCodes[popupRet]);
    }
    else if (popupRet == inverseNormales)
    {
        inverseNormal(pElts->at(volumeFound)._pObj);
    }
    else if (_pModeler->askForResetResultat() )
    {
        if (copySiteNodeRetCodes.find(popupRet) != copySiteNodeRetCodes.end())
        {
            copySite(copySiteNodeRetCodes[popupRet]);
        }
        else if (remSiteNodeRetCodes.find(popupRet) != remSiteNodeRetCodes.end())
        {
            remSite(remSiteNodeRetCodes[popupRet]);
        }
        else if (calculVolNodeRetCodes.find(popupRet) != calculVolNodeRetCodes.end())
        {
            getTYApp()->getCalculManager()->updateAcoustic(calculVolNodeRetCodes[popupRet]);
        }
        else if (popupRet == split)
        {
            splitCurve(pElts->at(levelCurveFound)._pObj);
        }
        else if (copyMaillageRetCodes.find(popupRet) != copyMaillageRetCodes.end())
        {
            copyMaillage(copyMaillageRetCodes[popupRet]);
        }
        else if (remMaillageRetCodes.find(popupRet) != remMaillageRetCodes.end())
        {
            remMaillage(remMaillageRetCodes[popupRet]);
        }
        else if (copyPtControlRetCodes.find(popupRet) != copyPtControlRetCodes.end())
        {
            copyPtCtrl(copyPtControlRetCodes[popupRet]);
        }
        else if (remPtControlRetCodes.find(popupRet) != remPtControlRetCodes.end())
        {
            remPtCtrl(remPtControlRetCodes[popupRet]);
        }
        else if (remTopoRetCodes.find(popupRet) != remTopoRetCodes.end())
        {
            remTopoElmt(remTopoRetCodes[popupRet]); 
        }
        else if (copyTopoRetCodes.find(popupRet) != copyTopoRetCodes.end())
        {
            copyTopoElmt(copyTopoRetCodes[popupRet]);
        }
        else if (remInfraRetCodes.find(popupRet) != remInfraRetCodes.end())
        {
            remInfraElmt(remInfraRetCodes[popupRet]);
        }
        else if (copyInfraRetCodes.find(popupRet) != copyInfraRetCodes.end())
        {
            copyInfraElmt(copyInfraRetCodes[popupRet]);
        }
    }

    delete pPopup;
}

void TYPickEditor::machineModelerPopupMenu(std::shared_ptr<LPTYElementArray> pElts)
{
    QMenu* pPopup = new QMenu(NULL);

    std::map<QAction*, int> retCodes;
    std::map<QAction*, TYAcousticVolumeNode*> calculVolNodeRetCodes;
    QAction* inverseNormales = NULL;
    std::map<QAction*, TYGeometryNode*> posRetCodes;
    std::map<QAction*, TYGeometryNode*> rotRetCodes;
    std::map<QAction*, TYAcousticVolume*> dimVolRetCodes;
    std::map<QAction*, TYAcousticVolume*> remVolRetCodes;
    std::map<QAction*, TYAcousticVolume*> copyVolRetCodes;
    QAction* editFace = NULL;
    QAction* code = NULL;

    LPTYElement pRemovedEltParent = NULL;
    LPTYElement pAddedElt = NULL;

    int rectFound = -2, volumeFound = -2;

    QFont font = pPopup->font();
    font.setBold(true);
    for (unsigned int i = 0; i < pElts->size(); i++)
    {
        // Edition des proprietes de l'element
        code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_editeelt"))), TYWidget::getDisplayName(pElts->at(i)));
        code->setFont(font);
        retCodes[code] = i;

        // Calcul des proprietes acoustiques
        if ( dynamic_cast<TYAcousticVolumeNode*>( pElts->at(i)._pObj ) )
        {
            // Calcul acoustique
            code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_calcul"))), TR("id_popup_calculer"));
            calculVolNodeRetCodes[code] = (LPTYAcousticVolumeNode&) pElts->at(i);
        }
        else if (dynamic_cast<TYAcousticVolume*>(pElts->at(i)._pObj) != nullptr)
        {
            // Dans tous les cas, on offre la possibilite d'inverser la normales des faces
            volumeFound = i;
            inverseNormales = pPopup->addAction(TR("id_popup_normales"));
            
            // Gestion position / rotation 
            TYGeometryNode* pEltGeoNode = TYGeometryNode::GetGeoNode( pElts->at(i)._pObj);
            code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_moving"))), TR("id_popup_position"));
            posRetCodes[code] = pEltGeoNode;

            code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_rotation"))), TR("id_popup_rotation"));
            rotRetCodes[code] = pEltGeoNode;

            // Dimensions
            code = pPopup->addAction(TR("id_popup_dimension"));
            dimVolRetCodes[code] = (LPTYAcousticVolume&) pElts->at(i);

            // Duplication
            code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_duplicate"))), TR("id_popup_duplicate"));
            copyVolRetCodes[code] = (LPTYAcousticVolume&) pElts->at(i);

            // Suppression
            code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_del"))), TR("id_popup_remove"));
            remVolRetCodes[code] = (LPTYAcousticVolume&) pElts->at(i);
        }
        else if ( dynamic_cast<TYAcousticRectangleNode*>(pElts->at(i)._pObj) != nullptr )
        {
            // On permet d'editer une face uniquement dans un modeleur de batiment ou de machine
            rectFound = i;
            editFace = pPopup->addAction(TR("id_popup_editface"));
        }

        // Ajoute un separateur entre les elements
        pPopup->addSeparator();
    }

    // Gestion du choix de l'utilisateur
    pPopup->setMouseTracking(true);
    bool bUpdateDisplayList = false;
    QAction* popupRet = pPopup->exec(QCursor::pos());
    if (popupRet == NULL) { return; }
    qApp->processEvents();

    if (retCodes.find(popupRet) != retCodes.end())
    {
        TYElement *pElement = pElts->at(retCodes[popupRet]);
        manageProperties(pElement);
    }
    else if (posRetCodes.find(popupRet) != posRetCodes.end())
    {
        showPositionDialog(posRetCodes[popupRet], false);
    }
    else if (rotRetCodes.find(popupRet) != rotRetCodes.end())
    {
        showRotationDialog(rotRetCodes[popupRet]);
    }
    else if (dimVolRetCodes.find(popupRet) != dimVolRetCodes.end())
    {
        TYElement *pElement = dimVolRetCodes[popupRet];
        setVolumeSize(pElement);
    }
    else if (popupRet == editFace)
    {
        TYElement *pElement = pElts->at(rectFound)._pObj;
        openFaceModeler(pElement);
    }
    else if (popupRet == inverseNormales)
    {
        inverseNormal(pElts->at(volumeFound)._pObj);
    }
    else if (_pModeler->askForResetResultat())
    {
        if (calculVolNodeRetCodes.find(popupRet) != calculVolNodeRetCodes.end())
        {
            getTYApp()->getCalculManager()->updateAcoustic(calculVolNodeRetCodes[popupRet]);
        }
        else if (remVolRetCodes.find(popupRet) != remVolRetCodes.end())
        {
            removeVolume(remVolRetCodes[popupRet]);
        }
        else if (copyVolRetCodes.find(popupRet) != copyVolRetCodes.end())
        {
            copyVolume(copyVolRetCodes[popupRet]);
        }
    }

    delete pPopup;
}

void TYPickEditor::batimentModelerPopupMenu(std::shared_ptr<LPTYElementArray> pElts)
{
    QMenu* pPopup = new QMenu(NULL);

    QAction* code = NULL;
    std::map<QAction*, int> retCodes;
    std::map<QAction*, TYAcousticVolumeNode*> calculVolNodeRetCodes;
    QAction* inverseNormales = NULL;
    std::map<QAction*, TYGeometryNode*> posRetCodes;
    std::map<QAction*, TYGeometryNode*> rotRetCodes;
    QAction* hauteurEtage = NULL;
    QAction* hauteurEcran = NULL;
    QAction* epaisseurEcran = NULL;
    QAction* editFace = NULL;
    std::map<QAction*, TYAcousticVolume*> remVolRetCodes;
    std::map<QAction*, TYAcousticVolume*> copyVolRetCodes;
    std::map<QAction*, TYAcousticVolume*> dimVolRetCodes;

    LPTYElement pRemovedEltParent = NULL;
    LPTYElement pAddedElt = NULL;

    int etageFound = -2, ecranFound = -2, rectFound = -2, volumeFound = -2;

    QFont font = pPopup->font();
    font.setBold(true);
    for (unsigned int i = 0; i < pElts->size(); i++)
    {
        // Edition des proprietes de l'element
        code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_editeelt"))), TYWidget::getDisplayName(pElts->at(i)));
        code->setFont(font);
        retCodes[code] = i;

        // Calcul des proprietes acoustiques
        if ( dynamic_cast<TYAcousticVolumeNode*>( pElts->at(i)._pObj ) )
        {
            // Calcul acoustique
            code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_calcul"))), TR("id_popup_calculer"));
            calculVolNodeRetCodes[code] = (LPTYAcousticVolumeNode&) pElts->at(i);
        }    
        else if (dynamic_cast<TYAcousticVolume*>(pElts->at(i)._pObj) != nullptr)
        {
            // Dans tous les cas, on offre la possibilite d'inverser la normales des faces
            volumeFound = i;
            inverseNormales = pPopup->addAction(TR("id_popup_normales"));

            // Gestion position / rotation 
            TYGeometryNode* pEltGeoNode = TYGeometryNode::GetGeoNode( pElts->at(i)._pObj );

            code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_moving"))), TR("id_popup_position"));
            posRetCodes[code] = pEltGeoNode;

            code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_rotation"))), TR("id_popup_rotation"));
            rotRetCodes[code] = pEltGeoNode;            
            
            // Dimensions
            if ( pElts->at(i)->isA("TYEtage") )
            {
                etageFound = i;
                hauteurEtage = pPopup->addAction(TR("id_popup_hauteur"));
            }
            else if ( pElts->at(i)->isA("TYEcran") )
            {
                ecranFound = i;
                hauteurEcran = pPopup->addAction(TR("id_popup_hauteur"));
                epaisseurEcran = pPopup->addAction(TR("id_popup_epaisseur"));
            }

            // Duplication
            code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_duplicate"))), TR("id_popup_duplicate"));
            copyVolRetCodes[code] = (LPTYAcousticVolume&) pElts->at(i);

            // Suppression
            code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_del"))), TR("id_popup_remove"));
            remVolRetCodes[code] = (LPTYAcousticVolume&) pElts->at(i);
        }
        else if ( dynamic_cast<TYAcousticRectangleNode*>(pElts->at(i)._pObj) != nullptr )
        {
            // On permet d'editer une face uniquement dans un modeleur de batiment ou de machine
            rectFound = i;
            editFace = pPopup->addAction(TR("id_popup_editface"));
        }

        // Ajoute un separateur entre les elements
        pPopup->addSeparator();
    }

    // Gestion du choix de l'utilisateur
    pPopup->setMouseTracking(true);
    bool bUpdateDisplayList = false;
    QAction* popupRet = pPopup->exec(QCursor::pos());
    if (popupRet == NULL) { return; }
    qApp->processEvents();

    if (retCodes.find(popupRet) != retCodes.end())
    {
        TYElement *pElement = pElts->at(retCodes[popupRet]);
        manageProperties(pElement);
    }
    else if (posRetCodes.find(popupRet) != posRetCodes.end())
    {
        bool bHeight = false;
        if (posRetCodes[popupRet]->isA("TYMachine")) { bHeight = true; }
        showPositionDialog(posRetCodes[popupRet], bHeight);
    }
    else if (rotRetCodes.find(popupRet) != rotRetCodes.end())
    {
        showRotationDialog(rotRetCodes[popupRet]);
    }
    else if (popupRet == editFace)
    {
        TYElement *pElement = pElts->at(rectFound)._pObj;
        openFaceModeler(pElement);
    }
    else if (popupRet == hauteurEtage)
    {
        floorHeight(pElts->at(etageFound)._pObj);
    }
    else if (popupRet == hauteurEcran)
    {
        screenHeight(pElts->at(ecranFound)._pObj);
    }
    else if (popupRet == epaisseurEcran)
    {
        screenThick(pElts->at(ecranFound)._pObj);
    }
    else if (popupRet == inverseNormales)
    {
        inverseNormal(pElts->at(volumeFound)._pObj);
    }
    else if (dimVolRetCodes.find(popupRet) != dimVolRetCodes.end())
    {
        TYElement *pElement = dimVolRetCodes[popupRet];
        setVolumeSize(pElement);
    }
    else if (_pModeler->askForResetResultat())
    {
        if (calculVolNodeRetCodes.find(popupRet) != calculVolNodeRetCodes.end())
        {
            getTYApp()->getCalculManager()->updateAcoustic(calculVolNodeRetCodes[popupRet]);
        }
        else if (remVolRetCodes.find(popupRet) != remVolRetCodes.end())
        {
            removeVolume(remVolRetCodes[popupRet]);
        }
        else if (copyVolRetCodes.find(popupRet) != copyVolRetCodes.end())
        {
            copyVolume(copyVolRetCodes[popupRet]);
        }
    }

    delete pPopup;
}


void TYPickEditor::showPositionDialog(TYGeometryNode* pGeoNode, bool activeHeight)
{
    assert(pGeoNode);

    // Recuperation de la hauteur de l'element
    double hauteur = pGeoNode->getHauteur();

    //On doit presenter la position de l'origine du GeoNode selectionne.
    //Cette position doit etre dans le repere du modeler frame.
    TYElement* pRootTYElement = _pModeler->getElement();
    TYGeometryNode* pRootGeometryNode = TYGeometryNode::GetGeoNode(pRootTYElement);

    // On a besoin du type d'element point par le geonode
    TYElement* pElement = pGeoNode->getElement();

    // On cree un geonode temporaire copie du geonode transmis (uniquement les elements geometrique)
    TYGeometryNode* pTempGeoNode = new TYGeometryNode();
    //  pTempGeoNode->deepCopy(pGeoNode, true);
    pTempGeoNode->setRepere(pGeoNode->getORepere3D()) ;
    pTempGeoNode->setElement(pElement);


    TYPoint oldZero(0, 0, 0);
    oldZero = pGeoNode->localToGlobal() * oldZero;
    if (pRootGeometryNode)
    {
        oldZero = pRootGeometryNode->globalToLocal() * oldZero;
        pTempGeoNode->getORepere3D()._origin = oldZero;
    }

    // Affectation de la hauteur au geoNode temporaire
    pTempGeoNode->setHauteur(hauteur);

    // Affiche la boite de dialogue
    TYPositionDialog* pDlg = new TYPositionDialog(pTempGeoNode, _pModeler);
    pDlg->setHauteurEnabled(activeHeight); // Activation ou non de la hauteur pour les elements de volumes nodes

    int ret = pDlg->exec();

    // Applique les modificatins si necessaire
    if (ret == QDialog::Accepted)
    {
        if (_pModeler->askForResetResultat())
        {
            //On a bouge l'origine (exprime dans le repere root)
            OPoint3D newZero = pTempGeoNode->getORepere3D()._origin;

            // On passe newZero dans le repere local s'il y a lieu
            if (pRootGeometryNode)
            {
	            newZero = pRootGeometryNode->localToGlobal() * newZero;
            }

            //exprimons newZero dans le repere du GeoNode:
            newZero = pGeoNode->globalToLocal() * newZero;

            //on modifie l'origine du GeoNode de newZero:
            ORepere3D repere = pGeoNode->getORepere3D();
            TYPoint oldOrg = repere._origin;
            repere._origin._x = repere._origin._x + newZero._x;
            repere._origin._y = repere._origin._y + newZero._y;
            repere._origin._z = repere._origin._z + newZero._z;
            pGeoNode->setRepere(repere);

            pGeoNode->setHauteur(pTempGeoNode->getHauteur());

            TYAction* pAction = new TYMoveGeoNodeAction(pGeoNode, oldOrg, repere._origin, _pModeler, TR("id_action_setposition"));
            _pModeler->getActionManager()->addAction(pAction);

            // Refresh
            pGeoNode->setIsGeometryModified(true);
            pGeoNode->updateGraphicTree();
            // (un updateGL est fait dans le reset)
            resetPicker();

            // La scene a ete modifiee
            TYElement::setIsSavedOk(true);
        }
    }

    delete pTempGeoNode;
    pTempGeoNode = NULL;
    delete pDlg;
    pDlg = NULL;
}

void TYPickEditor::showRotationDialog(TYGeometryNode* pGeoNode)
{
    if (!pGeoNode) { return; }

    //On doit une vue de la matrice du GeoNode selectionne, dans le repere du modeler frame.
    TYElement* pRootTYElement = _pModeler->getElement();
    TYGeometryNode* pRootGeometryNode = TYGeometryNode::GetGeoNode(pRootTYElement);

    OMatrix oldTyMat  = pGeoNode->localToGlobal();
    if (pRootGeometryNode)
    {
        oldTyMat = oldTyMat * pRootGeometryNode->globalToLocal(); //origin est maintenant exprime dans le repere pRootGeometryNode
    }

    // Get rotations from transform matrix
    OPoint3D vec;
    vec._x = oldTyMat._m[0][1];
    vec._y = oldTyMat._m[1][1];
    vec._z = oldTyMat._m[2][1];

    // Get X-vector for roll calculation
    OPoint3D xv;
    xv._x = oldTyMat._m[0][0];
    xv._y = oldTyMat._m[1][0];
    xv._z = oldTyMat._m[2][0];

    // Calculate PRH (x = pitch, y = roll, z = heading)
    OPoint3D rotTmp(-atan2(vec._z, sqrt(vec._x * vec._x + vec._y * vec._y)), xv._z, -atan2(-vec._x, vec._y));

    // Set up vars
    double pitch = RADTODEG(rotTmp._x);     // Pitch
    double yaw = -RADTODEG(rotTmp._z);       // Heading
    double roll = RADTODEG(rotTmp._y);      // Roll

    // Affiche la boite de dialogue
    OPoint3D rot(pitch, roll, yaw);
    TYRotationDialog* pDlg = new TYRotationDialog(&rot, _pModeler);
    if ((dynamic_cast<TYMaillage*>(pGeoNode->getElement()) != nullptr)
            || (dynamic_cast<TYSiteNode*>(pGeoNode->getElement()) != nullptr))
    {
        pDlg->lockXY();
    }

    int ret = pDlg->exec();

    // Applique les modifications si necessaire
    if (ret == QDialog::Accepted)
    {
        if (_pModeler->askForResetResultat())
        {
            ORepere3D Repere = pGeoNode->getORepere3D();
            ORepere3D oldRepere = Repere;
            OMatrix tyMat;
            OMatrix tyMatTmpX;
            OMatrix tyMatTmpY;
            OMatrix tyMatTmpZ;
            OMatrix tyMatTmpConcat;

            // On applique la rotation
            double dRotateX = rot._x;
            double dRotateY = rot._y;
            double dRotateZ = rot._z;
            if (pDlg->getConcatenateStatus())
            {
                dRotateX -= pitch;
                dRotateY -= roll;
                dRotateZ -= yaw;
            }

            tyMatTmpX.setRotationOx(-DEGTORAD(dRotateX));
            tyMatTmpY.setRotationOy(-DEGTORAD(dRotateY));
            tyMatTmpZ.setRotationOz(DEGTORAD(dRotateZ));

            if (pDlg->getConcatenateStatus()) //az--
            {
                tyMat = Repere.asMatrix();
                tyMatTmpConcat = tyMat;
            }

            tyMat = tyMat * tyMatTmpZ * tyMatTmpY * tyMatTmpX * tyMatTmpConcat;

            OPoint3D org = Repere._origin; // On conserve l'origine de depart
            Repere.set(tyMat);
            Repere._origin = org;

            pGeoNode->setRepere(Repere);

            TYAction* pAction = new TYRotateGeoNodeAction(pGeoNode, oldRepere, Repere, _pModeler, TR("id_action_setrotation"));
            _pModeler->getActionManager()->addAction(pAction);

            pGeoNode->setIsGeometryModified(true);
            pGeoNode->getElement()->getParent()->setIsGeometryModified(true);

            // Refresh
            pGeoNode->getElement()->getParent()->updateGraphic();
            // (un updateGL est fait dans le reset)
            resetPicker();

            // La scene a ete modifiee
            TYElement::setIsSavedOk(true);

        }
    }

    delete pDlg;
    pDlg = NULL;
}

void TYPickEditor::showDimensionsDialog(TYAcousticVolume* pAccVol)
{
    if (!pAccVol) { return; }

    TYAcousticVolume *pBackupVolume = dynamic_cast<TYAcousticVolume*>(pAccVol->clone());

    TYDimensionDialog *pDlg = new TYDimensionDialog(pAccVol, _pModeler);

    // Affiche la boite de dialogue
    int ret = pDlg->exec();

    // Applique les modificatins si necessaire
    if (ret == QDialog::Accepted)
    {
        if (! _pModeler->askForResetResultat())
        {
            *pAccVol = *pBackupVolume;
        }
        else
        {
            pAccVol->setIsGeometryModified(true);
            pAccVol->getParent()->setIsGeometryModified(true);

            // Refresh
            pAccVol->getParent()->updateGraphic();
            // (un updateGL est fait dans le reset)
            _pPicker->reset();

            // La scene a ete modifiee
            TYElement::setIsSavedOk(true);
        }
    }
}

void TYPickEditor::showPanel(TYElement* pElt)
{
    hidePanel(_pLastRolloverElt);

    if (!pElt)
    {
        return;
    }

    LPTYMaillage pMaillage = dynamic_cast<TYMaillage*>(pElt);
    if (pMaillage != nullptr)
    {
        LPTYPanel pPanel = pMaillage->getPanel();

        // Position du curseur (repere modeleur)
        double x = _lastMovedCurPos.x();
        double y = _pInteractor->height() - _lastMovedCurPos.y();
        TYElementGraphic* pTYElementGraphic = pPanel->getGraphicObject();
        ((TYPanelGraphic*)pTYElementGraphic)->setPosX(x);
        ((TYPanelGraphic*)pTYElementGraphic)->setPosY(y);
        pTYElementGraphic->setVisible();

        // Position dans le repère "monde"
        OPoint3D pt;
        if ( realWorldPosition(pt) == false ) { return ; }
        double X=pt._x, Y=pt._y, Z=pt._z;
        
        switch (_pModeler->getCurrentView())
        {
            case TYModelerFrame::TopView:
                pPanel->setFirstPos("X : " + doubleToStrPre(X, 1));
                pPanel->setSecondPos("Y : " + doubleToStrPre(Y, 1));
                break;
            case TYModelerFrame::LeftView:
                pPanel->setFirstPos("Y : " + doubleToStrPre(Y, 1));
                pPanel->setSecondPos("Z : " + doubleToStrPre(Z, 1));
                break;
            case TYModelerFrame::FrontView:
                pPanel->setFirstPos("X : " + doubleToStrPre(X, 1));
                pPanel->setSecondPos("Z : " + doubleToStrPre(Z, 1));
                break;
        }

        // Valeur
        LPTYMaillageGeoNode pMaillageGeoNode = TYCalcul::safeDownCast(pMaillage->getParent())->findMaillage(pMaillage);

        // On recupere la premiere distance
        OPoint3D coord;
        double minDistSquare = 0.0;
        LPTYPointCalcul pPtCalcul = pMaillage->getPtsCalcul().front();
        LPTYPointCalcul pResult = 0;
        if (pPtCalcul)
        {
	        //coord = pMaillageGeoNode->localToGlobal() * OCoord3D(pPtCalcul->_x, pPtCalcul->_y, 0.0);
 	        coord = pMaillageGeoNode->localToGlobal() * OPoint3D(pPtCalcul->_x, pPtCalcul->_y, 0.0);
           switch (_pModeler->getCurrentView())
            {
                case TYModelerFrame::TopView:
                    minDistSquare = std::sqrt(std::pow(coord._x - X, 2) + std::pow(coord._y - Y, 2));
                    break;
                case TYModelerFrame::LeftView:
                    minDistSquare = std::sqrt(std::pow(coord._y - Y, 2) + std::pow(coord._z - Z, 2));
                    break;
                case TYModelerFrame::FrontView:
                    minDistSquare = std::sqrt(std::pow(coord._x - X, 2) + std::pow(coord._z - Z, 2));
                    break;
            }

            pResult = pPtCalcul;
        }

        // On teste les suivantes pour trouver le point de calcul le plus proche
        for (unsigned int i = 1; i < pMaillage->getPtsCalcul().size(); i++)
        {
            pPtCalcul = pMaillage->getPtsCalcul()[i];
            coord = pMaillageGeoNode->localToGlobal() * OPoint3D(pPtCalcul->_x, pPtCalcul->_y, 0.0);

            double distSquare = 0.0;
            switch (_pModeler->getCurrentView())
            {
                case TYModelerFrame::TopView:
                    distSquare = std::sqrt(std::pow(coord._x - X, 2) + std::pow(coord._y - Y, 2));
                    break;
                case TYModelerFrame::LeftView:
                    distSquare = std::sqrt(std::pow(coord._y - Y, 2) + std::pow(coord._z - Z, 2));
                    break;
                case TYModelerFrame::FrontView:
                    distSquare = std::sqrt(std::pow(coord._x - X, 2) + std::pow(coord._z - Z, 2));
                    break;
            }

            if (distSquare < minDistSquare)
            {
                pResult = pPtCalcul;
                minDistSquare = distSquare;
            }
        }

        std::string value = "dB : N/A";
        if (pResult)
        {
            switch (pMaillage->getDataType())
            {
                case TYMaillage::ValGlobalDBA:
                    value = "dB(A) : " + doubleToStrPre(pResult->getValA(), 1);
                    break;
                case TYMaillage::ValGlobalDBLin:
                    value = "dB(Lin) : " + doubleToStrPre(pResult->getValLin(), 1);
                    break;
                case TYMaillage::DataFreq:
                    value = "dB(Freq) : " + doubleToStrPre(pResult->getSpectre()->getValueReal(pMaillage->getDataFreq()), 1);
                    break;
            }
        }

        pPanel->setValue(value);

        // On met a jour uniquement le maillage
        _pModeler->getView()->getRenderer()->updateDisplayList();
    }
    else
    {
        hidePanel(_pLastRolloverElt);
    }

    _pLastRolloverElt = pElt;
}

void TYPickEditor::hidePanel(TYElement* pElt)
{
    if (!pElt)
    {
        return;
    }

    LPTYMaillage pMaillage = dynamic_cast<TYMaillage*>(pElt);
    if (pMaillage != nullptr)
    {
        pMaillage->getPanel()->getGraphicObject()->setVisible(false);

        // On met a jour uniquement le maillage
        _pModeler->getView()->getRenderer()->updateDisplayList();
    }
}

bool TYPickEditor::realWorldPosition(OPoint3D& pt)
{
    dynamic_cast<TYModelerFrame*>(_pModeler)->updateView();

    // Position dans la scene 3D
    QPoint curPos = _lastPressedCurPos;

    // Calcul des coords
    float* pos = new float[3];

    if ( !(_pModeler->computeCurPos(curPos.x(), curPos.y(), pos)) ) 
    { 
        delete [] pos;
        return false; 
    }

    pt._x = pos[0];
    pt._y = -pos[2]; 
    pt._z = pos[1];
    
    delete [] pos;

    return true;
}

void TYPickEditor::manageProperties(TYElement *pElement)
{
    if ( (pElement != nullptr) && (pElement->edit(_pModeler) == QDialog::Accepted) )
    {
        // Mise a jour du projet
        TYProjet* pProjet = getTYApp()->getCurProjet();
        if (pProjet)
        {
            TYSiteNode* pSite = pProjet->getSite();
            if (pSite)
            {
                pSite->update(pElement);
                getTYMainWnd()->getSiteFrame()->updateList();
            }
        }

        pElement->updateGraphicTree();
        pElement->updateGraphic();
    }
}

void TYPickEditor::copySite(TYElement* pElement)
{
    if (pElement == nullptr) { return; }

    TYElement *pParent = pElement->getParent();

    if (pParent == nullptr) { return; }

    TYSiteNodeGeoNode *pGeoNode = TYGeometryNode::GetGeoNode(pElement);

    // Nouvel element du meme type que l'objet a dupliquer (clone)
    LPTYSiteNodeGeoNode pCopy = new TYSiteNodeGeoNode( dynamic_cast<TYSiteNode*>(pElement->clone()) );

    // Duplication
    pCopy->deepCopy(pGeoNode, false);
    pCopy->setParent( pElement->getParent() );

    // Ajout
    if (dynamic_cast<TYSiteNode*>(pParent)->addSiteNode(pCopy))
    {
        ORepere3D repere = pCopy->getORepere3D();
        // Offset
        repere._origin._x += 100.0;
        repere._origin._y -= 100.0;

        pCopy->setRepere(repere);

        LPTYCalcul pCalc = NULL;
        TYSiteNode* pSite = NULL;
        pSite = dynamic_cast<TYSiteNode*>(pCopy->getElement()) ;
        if (getTYApp()->getCurProjet() && pSite)
        {
            pCalc = getTYApp()->getCurProjet()->getCurrentCalcul();
            if (pCalc) { pCalc->getCalculElements(pSite); }
        }

        // Update Graphic
        pCopy->updateGraphicTree();

        // Action
        TYAction* pAction = new TYAddSiteNodeToSiteNodeAction(pCopy, dynamic_cast<TYSiteNode*>(pParent), _pModeler, TR("id_action_addsitenode"));
        _pModeler->getActionManager()->addAction(pAction);
    }

    updateSiteFrame();

    // La scene a ete modifiee
    TYElement::setIsSavedOk(true);
}

void TYPickEditor::remSite(TYElement *pElement)
{
    if (pElement == nullptr) { return; }
    TYElement *pParent = pElement->getParent();
    if (pParent == nullptr) { return; }

    LPTYSiteNode pSiteParent = dynamic_cast<TYSiteNode*> (pParent);
    LPTYSiteNodeGeoNode pGeoNode = TYGeometryNode::GetGeoNode(pElement);

    if ( pSiteParent->remSiteNode( (LPTYSiteNode&)pElement ) )
    {
        TYAction* pAction = new TYRemSiteNodeToSiteNodeAction(pGeoNode, pSiteParent, _pModeler, TR("id_action_remsitenode"));
        _pModeler->getActionManager()->addAction(pAction);
    }

    updateSiteFrame();

    // La scene a ete modifiee
    TYElement::setIsSavedOk(true);
}

void TYPickEditor::splitCurve(TYElement *pElement)
{
    OPoint3D pt;
    if ( realWorldPosition(pt) )
    {
        TYCourbeNiveau *pCurrentCurve = dynamic_cast<TYCourbeNiveau*>( pElement );
        TYTopographie* pTopo = dynamic_cast<TYTopographie*>(pCurrentCurve->getParent());

        if (pCurrentCurve != nullptr)
        {
            LPTYCourbeNiveau newCurve = pCurrentCurve->split(pt);
            if (newCurve._pObj != nullptr)
            {
                // Copie du geonode de l'actuelle courbe
                TYGeometryNode *pCurrentGeoNode = TYGeometryNode::GetGeoNode(pCurrentCurve);
                LPTYGeometryNode pNewGeoNode = new TYGeometryNode();
                pNewGeoNode->deepCopy(pCurrentGeoNode, false);

                // Association du geonode avec la nouvelle courbe
                pNewGeoNode->setElement( (LPTYElement) newCurve);

                // Ajout de la nouvelle courbe au projet
                if (pTopo != nullptr)
                {
                    pTopo->addCrbNiv(pNewGeoNode);
                }
            }

            pTopo->updateGraphicTree();
            pTopo->updateGraphic();
            updateSiteFrame();
            // La scene a ete modifiee
            TYElement::setIsSavedOk(true);
        }
    }
}

void TYPickEditor::copyMaillage(TYElement *pElement)
{
    if (pElement == nullptr) { return; }
    TYElement *pParent = pElement->getParent();
    if (pParent == nullptr) { return; }

    TYMaillageGeoNode *pGeoNode = TYGeometryNode::GetGeoNode(pElement);

    // Nouvel element du meme type que l'objet a dupliquer (clone)
    LPTYMaillageGeoNode pCopy = new TYMaillageGeoNode(dynamic_cast<TYMaillage*>(pElement->clone()));

    // Duplication
    pCopy->deepCopy(pGeoNode, false);
    pCopy->setParent(pParent);

    // Ajout
    if ((dynamic_cast<TYCalcul*>(pParent))->addMaillage(pCopy))
    {
        TYRectangularMaillage *pMaillage = dynamic_cast<TYRectangularMaillage*>(pCopy->getElement());
        double x = 10., y = 10.;
        if (pMaillage != nullptr)
        {
            LPTYRectangle pRect = pMaillage->getRectangle();
            x = pRect->getSizeX() / 2.;
            y = pRect->getSizeY() / 2.;
        }
        
        ORepere3D repere = pCopy->getORepere3D();
        // Offset
        repere._origin._x += x;
        repere._origin._y -= y;

        pCopy->setRepere(repere);

        // Update Graphic
        pCopy->updateGraphicTree();

        // Action
        TYAction* pAction = new TYAddMaillageToCalculAction(pCopy, (dynamic_cast<TYCalcul*>(pParent)), _pModeler, TR("id_action_addmaillage"));
        _pModeler->getActionManager()->addAction(pAction);

        refreshProjectFrame();
    }

    // La scene a ete modifiee
    TYElement::setIsSavedOk(true);
}

void TYPickEditor::remMaillage(TYElement *pElement)
{
    if (pElement == nullptr) { return; }
    TYElement *pParent = pElement->getParent();
    if (pParent == nullptr) { return; }

    LPTYMaillageGeoNode pGeoNode = TYGeometryNode::GetGeoNode(pElement);
    LPTYCalcul pCalculParent = dynamic_cast<TYCalcul*>(pParent);

    if ( pCalculParent->remMaillage(dynamic_cast<TYMaillage*>(pElement)) )
    {
        TYAction* pAction = new TYRemMaillageToCalculAction(pGeoNode, dynamic_cast<TYCalcul*>(pParent), _pModeler, TR("id_action_remmaillage"));
        _pModeler->getActionManager()->addAction(pAction);
    }

    refreshProjectFrame();
    getTYMainWnd()->updateModelers(false);

    // La scene a ete modifiee
    TYElement::setIsSavedOk(true);
}

void TYPickEditor::copyPtCtrl(TYElement *pElement)
{
    if (pElement == nullptr) { return; }
    TYElement *pParent = pElement->getParent();
    if (pParent == nullptr) { return; }

    // Nouvel element du meme type que l'objet a dupliquer (clone)
    LPTYPointControl pCopy = dynamic_cast<TYPointControl*>(pElement->clone());

    // Duplication
    pCopy->deepCopy(dynamic_cast<TYPointControl*>(pElement), false);
    pCopy->setParent(pElement->getParent());

    // Ajout
    if ( (dynamic_cast<TYProjet*>(pParent))->addPointControl(pCopy) )
    {
        // Offset
        pCopy->_x += 10.0;
        pCopy->_y -= 10.0;

        // Update Graphic
        pCopy->updateGraphicTree();

        // Action
        TYAction* pAction = new TYAddPointControlAction(dynamic_cast<TYProjet*>(pParent), pCopy, _pModeler, TR("id_action_addptcontrol"));
        _pModeler->getActionManager()->addAction(pAction);
    }

    refreshProjectFrame();
    getTYMainWnd()->updateModelers(false);

    // La scene a ete modifiee
    TYElement::setIsSavedOk(true);
}
void TYPickEditor::remPtCtrl(TYElement *pElement)
{
    if (pElement == nullptr) { return; }
    LPTYProjet pParent = dynamic_cast<TYProjet*>(pElement->getParent());
    if (pParent == nullptr) { return; }

    if ( pParent->remPointControl( dynamic_cast<TYPointControl*>(pElement) ) )
    {
        TYAction* pAction = new TYRemPointControlAction(pParent, dynamic_cast<TYPointControl*>(pElement), _pModeler, TR("id_action_remptcontrol"));
        _pModeler->getActionManager()->addAction(pAction);
    }

    refreshProjectFrame();
    getTYMainWnd()->updateModelers(false);

    // La scene a ete modifiee
    TYElement::setIsSavedOk(true);
}
void TYPickEditor::remTopoElmt(TYElement *pElement)
{
    if (pElement == nullptr) { return; }
    TYTopographie* pTopo = dynamic_cast<TYTopographie*>(pElement->getParent());
    if (pTopo == nullptr) { return; }

    // On recupere le site parent
    TYSiteNode* pSiteParent = TYSiteNode::safeDownCast(pTopo->getParent());

    TYAction* pAction = new TYRemElementToTopoAction(pElement, pTopo, _pModeler, TR("id_action_remelttopo"));
    _pModeler->getActionManager()->addAction(pAction);

    if (pElement->isA("TYCourbeNiveau"))
    {
        pTopo->remCrbNiv((LPTYCourbeNiveau&) pElement);
    }
    else if (pElement->isA("TYTerrain"))
    {
        pTopo->remTerrain((LPTYTerrain&) pElement);
    }
    else if (pElement->isA("TYCoursEau"))
    {
        pTopo->remCrsEau((LPTYCoursEau&) pElement);
    }
    else if (pElement->isA("TYPlanEau"))
    {
        pTopo->remPlanEau((LPTYPlanEau&) pElement);
    }

    updateSiteFrame();

    // La scene a ete modifiee
    TYElement::setIsSavedOk(true);
}
void TYPickEditor::copyTopoElmt(TYElement *pElement)
{
    if (pElement == nullptr) { return; }
    TYTopographie* pTopo = dynamic_cast<TYTopographie*>(pElement->getParent());
    if (pTopo == nullptr) { return; }

    SmartPtr<TYGeometryNode > pCopyTmp = NULL;

    if (pElement->isA("TYCourbeNiveau"))
    {
        // Nouvelle element
        LPTYCourbeNiveauGeoNode pCopy = new TYCourbeNiveauGeoNode(new TYCourbeNiveau());
        pCopyTmp = (SmartPtr<TYGeometryNode >&) pCopy;

        // Duplication
        pCopy->deepCopy(pTopo->findCrbNiv((LPTYCourbeNiveau&) pElement), false);
        pCopy->setParent(pElement->getParent());

        // Ajout
        pTopo->addCrbNiv(pCopy);
    }
    else if (pElement->isA("TYTerrain"))
    {
        // Nouvelle element
        LPTYTerrainGeoNode pCopy = new TYTerrainGeoNode(new TYTerrain);
        pCopyTmp = (SmartPtr<TYGeometryNode >&) pCopy;

        // Duplication
        pCopy->deepCopy(pTopo->findTerrain((LPTYTerrain&) pElement), false);
        pCopy->setParent(pElement->getParent());

        // Ajout
        pTopo->addTerrain(pCopy);
    }
    else if (pElement->isA("TYCoursEau"))
    {
        // Nouvelle element
        LPTYCoursEauGeoNode pCopy = new TYCoursEauGeoNode(new TYCoursEau);
        pCopyTmp = (SmartPtr<TYGeometryNode >&) pCopy;

        // Duplication
        pCopy->deepCopy(pTopo->findCrsEau((LPTYCoursEau&) pElement), false);
        pCopy->setParent(pElement->getParent());

        // Ajout
        pTopo->addCrsEau(pCopy);
    }
    else if (pElement->isA("TYPlanEau"))
    {
        // Nouvelle element
        LPTYPlanEauGeoNode pCopy = new TYPlanEauGeoNode(new TYPlanEau);
        pCopyTmp = (SmartPtr<TYGeometryNode >&) pCopy;

        // Duplication
        pCopy->deepCopy(pTopo->findPlanEau((LPTYPlanEau&) pElement), false);
        pCopy->setParent(pElement->getParent());

        // Ajout
        pTopo->addPlanEau(pCopy);
    }

    //if (pCopyTmp)
    //{
    //    ORepere3D repere = pCopyTmp->getORepere3D();
    //    // Offset
    //    repere._origin._x += 50.0;
    //    repere._origin._y -= 50.0;
    //    pCopyTmp->setRepere(repere);

    //    // Action
    //    TYAction* pAction = new TYAddElementToTopoAction(pCopyTmp, pTopo, _pModeler, TR("id_action_addelttopo"));
    //    _pModeler->getActionManager()->addAction(pAction);

    //    // Update graphic
    //    pCopyTmp->updateGraphicTree();
    //}

    updateSiteFrame();

    // La scene a ete modifiee
    TYElement::setIsSavedOk(true);
}
void TYPickEditor::remInfraElmt(TYElement *pElement)
{
    if (pElement == nullptr) { return; }
    TYInfrastructure* pInfra = dynamic_cast<TYInfrastructure*>(pElement->getParent());
    if (pInfra == nullptr) { return; }

    TYAction* pAction = new TYRemElementToInfraAction(pElement, pInfra, _pModeler, TR("id_action_remeltinfra"));
    _pModeler->getActionManager()->addAction(pAction);

    if (pElement->isA("TYRoute"))
    {
        pInfra->remRoute((LPTYRoute&) pElement);
    }
    else if (pElement->isA("TYReseauTransport"))
    {
        pInfra->remResTrans((LPTYReseauTransport&) pElement);
    }
    else if (pElement->isA("TYBatiment"))
    {
        pInfra->remBatiment((LPTYBatiment&) pElement);
    }
    else if (pElement->isA("TYMachine"))
    {
        pInfra->remMachine((LPTYMachine&) pElement);
    }
    else if (pElement->isA("TYUserSourcePonctuelle"))
    {
        pInfra->remSrc((LPTYUserSourcePonctuelle&) pElement);
    }
	
	

    updateSiteFrame(); // Mise a jour de l'arborescence du site

    // La scene a ete modifiee
    TYElement::setIsSavedOk(true);
}
void TYPickEditor::copyInfraElmt(TYElement *pElement)
{
    if (pElement == nullptr) { return; }
    TYInfrastructure* pInfra = dynamic_cast<TYInfrastructure*>( pElement->getParent() );
    if (pInfra == nullptr) { return; }

    SmartPtr<TYGeometryNode > pCopyTmp = NULL;
    TYBox boundBox;

    LPTYGeometryNode pCopy = nullptr;

    if (pElement->isA("TYRoute"))
    {
        // Nouvelle element
        pCopy = new TYRouteGeoNode(new TYRoute);
        pCopyTmp = (SmartPtr<TYGeometryNode >&) pCopy;

        // Duplication
        pCopy->deepCopy(pInfra->findRoute((LPTYRoute&) pElement), false);
        pCopy->setParent(pElement->getParent());

        // Ajout
        pInfra->addRoute(pCopy);
    }
    else if (pElement->isA("TYReseauTransport"))
    {
        // Nouvel element
        pCopy = new TYReseauTransportGeoNode(new TYReseauTransport);
        pCopyTmp = (SmartPtr<TYGeometryNode >&) pCopy;

        // Duplication
        pCopy->deepCopy(pInfra->findResTrans((LPTYReseauTransport&) pElement), false);
        pCopy->setParent(pElement->getParent());

        // Ajout
        pInfra->addResTrans(pCopy);
    }
    else if (pElement->isA("TYBatiment"))
    {
        // Nouvelle element
        pCopy = new TYBatimentGeoNode(new TYBatiment);
        pCopyTmp = (SmartPtr<TYGeometryNode >&) pCopy;

        // Duplication
        pCopy->deepCopy(pInfra->findBatiment((LPTYBatiment&) pElement), false);
        pCopy->setParent(pElement->getParent());
        updateCopyPosition(pCopy);

        // Ajout
        pInfra->addBatiment(pCopy);

        // Message d'alerte sur le fonctionnement des machine dupliquees
        OMessageManager::get()->info("**********************************************************************\n");
        OMessageManager::get()->info("*                      ATTENTION !!!                                 *\n\n\n");
        OMessageManager::get()->info("* Les machines a l'interieur du batiment duplique ne sont pas active *\n\n");
        OMessageManager::get()->info("*                PENSEZ A LES REACTIVER                              *\n\n");
        OMessageManager::get()->info("**********************************************************************\n");

    }
    else if (pElement->isA("TYMachine"))
    {
        // Nouvelle element
        pCopy = new TYMachineGeoNode(new TYMachine);
        pCopyTmp = (SmartPtr<TYGeometryNode >&) pCopy;

        // Duplication
        pCopy->deepCopy(pInfra->findMachine((LPTYMachine&) pElement), false);
        pCopy->setParent(pElement->getParent());
        updateCopyPosition(pCopy);
        
        // Ajout
        pInfra->addMachine(pCopy);
    }
    else if (pElement->isA("TYUserSourcePonctuelle"))
    {
        // Nouvelle element
        pCopy = new TYSourcePonctuelleGeoNode(new TYSourcePonctuelle);
        pCopyTmp = (SmartPtr<TYGeometryNode >&) pCopy;

        // Duplication
        pCopy->deepCopy(pInfra->findSrc((LPTYUserSourcePonctuelle&) pElement), false);
        pCopy->setParent(pElement->getParent());

        // Ajout
        pInfra->addSrc(pCopy);

        // Offset
        TYUserSourcePonctuelle::safeDownCast(pCopy->getElement())->getPos()->_x += 10;
        TYUserSourcePonctuelle::safeDownCast(pCopy->getElement())->getPos()->_y -= 10;
    }

    //if (pCopyTmp)
    //{
    //    if (!pElement->isA("TYUserSourcePonctuelle"))
    //    {
    //        ORepere3D repere = pCopyTmp->getORepere3D();

    //        // Offset proportionnel a la taille de l'objet
    //        repere._origin._x += boundBox._sizeX; //10.0;
    //        repere._origin._y -= boundBox._sizeY; //10.0;

    //        pCopyTmp->setRepere(repere);
    //    }

    //    // Action
    //    TYAction* pAction = new TYAddElementToInfraAction(pCopyTmp, pInfra, _pModeler, TR("id_action_addeltinfra"));
    //    _pModeler->getActionManager()->addAction(pAction);
    //}

    // On l'ajoute dans le calcul courant (si calcul courant il y a !)
    if (pCopy && getTYApp()->getCurProjet() && getTYApp()->getCurProjet()->getCurrentCalcul())
    {
        getTYApp()->getCurProjet()->getCurrentCalcul()->addToSelection(pCopy->getElement());
    }    
    
    updateSiteFrame(); // Mise a jour de l'arborescence du site

    // La scene a ete modifiee
    TYElement::setIsSavedOk(true);
}
void TYPickEditor::setVolumeSize(TYElement *pElement)
{
    if (pElement == nullptr) { return; }
    // On preserve la dimension en hauteur de l'objet
    TYAcousticVolume* pVol = dynamic_cast<TYAcousticVolume*>(pElement);
    if (pVol == nullptr) { return; }

    float sizeX = 0, sizeY = 0, sizeZ = 0;

    if (pVol != NULL)
    {
        if (pVol->isA("TYAcousticBox"))
        {
            TYAcousticBox* pBox = (TYAcousticBox*) pVol;
            pBox->getDimension(sizeX, sizeY, sizeZ);
        }
        else if (pVol->isA("TYAcousticCylinder"))
        {
            sizeZ = ((TYAcousticCylinder*) pVol)->getHauteur();
        }
        else if (pVol->isA("TYAcousticSemiCylinder"))
        {
            sizeZ = ((TYAcousticSemiCylinder*) pVol)->getHauteur();
        }
    }

    // Affichage de la boite de dialogue
    showDimensionsDialog(pVol);

    // Modification de la position pour tenir compte des nouvelles dimensions
    LPTYAcousticVolumeNode pParent = (TYAcousticVolumeNode*) pVol->getParent();
    LPTYAcousticVolumeGeoNode pGeoNode = pParent->findAcousticVol(pVol);

    if (pGeoNode)
    {
        ORepere3D repere = pGeoNode->getORepere3D();
        TYPoint org = repere._origin;
        // On retire l'ancienne hauteur
        org._z -= sizeZ / 2;

        // on ajoute la nouvelle
        if (pVol->isA("TYAcousticBox"))
        {
            TYAcousticBox* pAccBox = (TYAcousticBox*) pVol;
            pAccBox->getDimension(sizeX, sizeY, sizeZ);
        }
        else if (pVol->isA("TYAcousticCylinder"))
        {
            sizeZ = ((TYAcousticCylinder*) pVol)->getHauteur();
        }
        else if (pVol->isA("TYAcousticSemiCylinder"))
        {
            sizeZ = ((TYAcousticSemiCylinder*) pVol)->getHauteur();
        }

        org._z += sizeZ / 2;

        repere._origin = org;
        pGeoNode->setRepere(repere);
    }
}
void TYPickEditor::removeVolume(TYElement *pElement)
{
    if (pElement == nullptr) { return; }
    TYAcousticVolumeNode *pParent = dynamic_cast<TYAcousticVolumeNode*>(pElement->getParent());
    if (pParent == nullptr) { return; }
    TYAcousticVolumeGeoNode *pGeoNode = pParent->findAcousticVol(dynamic_cast<TYAcousticVolume*>(pElement));
    if (pGeoNode == nullptr) { return; }

	// The action is added to the ActionManager before the object is deleted
	TYAction* pAction = new TYRemAccVolToAccVolNodeAction(pGeoNode, pParent, _pModeler, TR("id_action_remvol"));
    _pModeler->getActionManager()->addAction(pAction);
    pParent->remAcousticVol(dynamic_cast<TYAcousticVolume*>(pElement));
    updateSiteFrame();

    // La scene a ete modifiee
    TYElement::setIsSavedOk(true);
}
void TYPickEditor::copyVolume(TYElement *pElement)
{
    if (pElement == nullptr) { return; }
    TYAcousticVolumeNode *pVolParent = dynamic_cast<TYAcousticVolumeNode*>(pElement->getParent());
    if (pVolParent == nullptr) { return; }
    TYAcousticVolumeGeoNode *pGeoNode = TYGeometryNode::GetGeoNode(pElement);
    if (pGeoNode == nullptr) { return; }

    // Nouvel element du meme type que l'objet a dupliquer (clone)
    LPTYAcousticVolumeGeoNode pCopy = new TYAcousticVolumeGeoNode(dynamic_cast<TYAcousticVolume*> (pElement->clone()));

    // Duplication
    pCopy->deepCopy(pGeoNode, false);

    // Ajout
    if (pVolParent->addAcousticVol(pCopy))
    {
        // Offset
        ORepere3D repere = pCopy->getORepere3D();

        if (dynamic_cast<TYEtage*>(pCopy->getElement()) != nullptr)
        {
            TYElement* pElt = pCopy->getElement();
            repere._origin._z += ((TYEtage*)pElt)->getHauteur();
        }
        else
        {
            repere._origin._x += 20.0;
            repere._origin._y -= 20.0;
        }

        pCopy->setRepere(repere);
        // Update Graphic
        pCopy->updateGraphicTree();

        // Action
        TYAction* pAction = new TYAddAccVolToAccVolNodeAction(pCopy, pVolParent, _pModeler, TR("id_action_addvol"));
        _pModeler->getActionManager()->addAction(pAction);
    }

    updateSiteFrame();

    // La scene a ete modifiee
    TYElement::setIsSavedOk(true);
}
void TYPickEditor::openFaceModeler(TYElement *pElement)
{
    if (pElement == nullptr) { return; }
    TYAcousticRectangleNode* pAccRectNode = dynamic_cast<TYAcousticRectangleNode*>(pElement);
    if (pAccRectNode == nullptr) { return; }

    TYFaceModelerFrame* pFaceMdF;
    pFaceMdF = new TYFaceModelerFrame(pAccRectNode, getTYMainWnd()->getWorkspace(), "face modeler");
    pFaceMdF->setAttribute(Qt::WA_DeleteOnClose);
    getTYMainWnd()->getWorkspace()->addSubWindow(pFaceMdF)->setObjectName("TYFaceModelerFrame");

    pFaceMdF->showMaximized();
    pFaceMdF->fit();
}
void TYPickEditor::inverseNormal(TYElement *pElement)
{
    if (pElement == nullptr) { return; }
    TYAcousticVolume* pVol = dynamic_cast<TYAcousticVolume*>(pElement);

    if (pVol) { pVol->inverseNormales(); }
    pVol->setNormalStatus();
}
void TYPickEditor::floorHeight(TYElement *pElement)
{
    if (_pModeler->askForResetResultat())
    {
        if (pElement == nullptr) { return; }
        TYEtage* pEtage = dynamic_cast<TYEtage*>(pElement);
        if (pEtage == nullptr) { return; }

        bool ok = false;
        double hauteur = QInputDialog::getDouble(_pModeler, "", TR("id_msg_gethauteur"), pEtage->getHauteur(), 0, 1000, 2, &ok);

        if (ok)
        {
            pEtage->setHauteur(hauteur);
            pEtage->updateGraphic();
        }
    }
}
void TYPickEditor::screenHeight(TYElement *pElement)
{
    if (_pModeler->askForResetResultat())
    {
        TYEcran* pEcran = dynamic_cast<TYEcran*>(pElement);

        bool ok = false;
        double hauteur = QInputDialog::getDouble(_pModeler, "", TR("id_msg_gethauteur_ecran"), pEcran->getHauteur(), 0, 1000, 2, &ok);

        if (ok)
        {
            pEcran->setHauteur(hauteur);
            pEcran->updateGeometry();
            pEcran->updateGraphic();
        }
    }
}
void TYPickEditor::screenThick(TYElement *pElement)
{
    if (_pModeler->askForResetResultat())
    {
        TYEcran* pEcran = dynamic_cast<TYEcran*>(pElement);

        bool ok = false;
        double epaisseur = QInputDialog::getDouble(_pModeler, "", TR("id_msg_getepaisseur_ecran"), pEcran->getEpaisseur(), 0, 1000, 2, &ok);

        if (ok)
        {
            pEcran->setEpaisseur(epaisseur);
            pEcran->updateGeometry();
            pEcran->updateGraphic();
        }
    }
}

void TYPickEditor::updateCopyPosition(LPTYGeometryNode &pNode)
{
    TYBox boundBox = dynamic_cast<TYAcousticVolumeNode*>(pNode->getElement())->volEnglob();

    pNode->getORepere3D()._origin = pNode->getORepere3D()._origin + OCoord3D(boundBox._sizeX/2., boundBox._sizeY/2., 0.);
}
