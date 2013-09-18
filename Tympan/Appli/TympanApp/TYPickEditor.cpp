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
 *
 *
 */



#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYBatiment.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYEcran.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYPickHandler.h"
#include "Tympan/GraphicIHM/DataManagerIHM/TYWidget.h"

#include "Tympan/Tools/OLocalizator.h"


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

//Added by qt3to4:
#include <QBoxLayout>
#include <QPixmap>
#include <QGridLayout>
#include <QHBoxLayout>
// CLM-NT35: Gestion MDI avec QT4.7
#ifdef USE_QMDIAREA
#include <QMdiArea>
#include <QMdiSubWindow>
#else
#include <QWorkspace>
#endif
// CLM-NT35 End


#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYRectangularMaillage.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYRectangularMaillageGraphic.h"
#include "TYMenuItem.h"

using namespace Qt;

#if defined(WIN32)
#include <crtdbg.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif


#define TR(id) OLocalizator::getString("TYPickEditor", (id))
#define IMG(id) OLocalizator::getPicture("TYPickEditor", (id))


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
    if (_pModeler->hasFocus() && _pLastRolloverElt && _pLastRolloverElt->inherits("TYMaillage"))
    {
        hidePanel(_pLastRolloverElt);
        _pModeler->getView()->updateGL();
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
            QObject::connect(_pPicker, SIGNAL(elementCollectionPicked(LPTYElementCollection)), this, SLOT(showPopupMenu(LPTYElementCollection)));
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
            QObject::disconnect(_pPicker, SIGNAL(elementCollectionPicked(LPTYElementCollection)), this, SLOT(showPopupMenu(LPTYElementCollection)));
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
    if (_pModeler->hasFocus() && _pLastRolloverElt && _pLastRolloverElt->inherits("TYMaillage"))
    {
        hidePanel(_pLastRolloverElt);
        _pModeler->getView()->updateGL();
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
    if (_pModeler->hasFocus() && _pLastRolloverElt && _pLastRolloverElt->inherits("TYMaillage"))
    {
        hidePanel(_pLastRolloverElt);
        _pModeler->getView()->updateGL();
    }
}

void TYPickEditor::showPopupMenu(LPTYElementCollection pElts)
{
    if (!pElts)
    {
        return;
    }

    QMenu* pPopup = new QMenu(NULL);

    std::map<QAction*, int> retCodes;
    std::map<QAction*, TYGeometryNode*> posRetCodes;
    std::map<QAction*, TYGeometryNode*> rotRetCodes;
    std::map<QAction*, TYAcousticVolume*> dimVolRetCodes;
    std::map<QAction*, TYAcousticVolume*> remVolRetCodes;
    std::map<QAction*, TYAcousticVolume*> copyVolRetCodes;
    std::map<QAction*, TYMachine*> remMachineRetCodes;
    std::map<QAction*, TYMachine*> copyMachineRetCodes;
    std::map<QAction*, TYElement*> remTopoRetCodes;
    std::map<QAction*, TYElement*> copyTopoRetCodes;
    std::map<QAction*, TYElement*> remInfraRetCodes;
    std::map<QAction*, TYElement*> copyInfraRetCodes;
    std::map<QAction*, TYMaillage*> remMaillageRetCodes;
    std::map<QAction*, TYMaillage*> copyMaillageRetCodes;
    std::map<QAction*, TYPointControl*> copyPtControlRetCodes;
    std::map<QAction*, TYPointControl*> remPtControlRetCodes;
    std::map<QAction*, TYSiteNode*> remSiteNodeRetCodes;
    std::map<QAction*, TYSiteNode*> copySiteNodeRetCodes;
    std::map<QAction*, TYAcousticVolumeNode*> calculVolNodeRetCodes;
    LPTYElement pRemovedEltParent = NULL;
    LPTYElement pAddedElt = NULL;
    TYElement* pSelected = NULL;
    QAction* hauteurEtage = NULL;
    QAction* hauteurEcran = NULL;
    QAction* epaisseurEcran = NULL;
    QAction* editFace = NULL;
    QAction* showModelerSite = NULL;
    QAction* showModelerMachine = NULL;
    QAction* showModelerBatiment = NULL;
    QAction* code = NULL;
    QAction* inverseNormales = NULL;
    int etageFound = -2, ecranFound = -2, rectFound = -2, volumeFound = -2;

    for (unsigned int i = 0; i < pElts->size(); i++)
    {
        if (i > 0)
        {
            // On separe l'element picke de ses parents
            pPopup->addSeparator();
        }

        QFont font = pPopup->font();
        font.setBold(true);
        if (TYSiteNode::safeDownCast(pElts->at(i)) && TYSiteNode::safeDownCast(pElts->at(i))->getRoot())
        {
            code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_editeelt"))), TR("id_popup_siteroot"));
            code->setFont(font);
        }
        else
        {
            code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_editeelt"))), TYWidget::getDisplayName(pElts->at(i)));
            code->setFont(font);
        }
        retCodes[code] = i;

        if (pElts->at(i)->isA("TYBatiment"))
        {
            // Calcul acoustique
            code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_calcul"))), TR("id_popup_calculer"));
            calculVolNodeRetCodes[code] = (LPTYAcousticVolumeNode&) pElts->at(i);

            // Si on est dans un modeleur de site/projet, on permet d'ouvrir un modeleur
            if (QString(_pModeler->metaObject()->className()).compare("TYSiteModelerFrame") == 0)
            {
                pSelected = pElts->at(i);
            }

            if ((i + 1 < pElts->size()) && pElts->at(i + 1)->inherits("TYInfrastructure"))
            {
                if ((QString(_pModeler->metaObject()->className()).compare("TYBatimentModelerFrame") != 0) &&
                    (QString(_pModeler->metaObject()->className()).compare("TYMachineModelerFrame") != 0))
                {
                    // Si le batiment est dans le modeler de son site parent on a acces a sa position
                    if (((i + 2) < pElts->size()) && (pElts->at(i + 2)->inherits("TYSiteNode")))
                    {
                        LPTYSiteNode pSite = TYSiteNode::safeDownCast(pElts->at(i + 2));
                        TYSiteModelerFrame* pmodelerFrame = (TYSiteModelerFrame*) _pModeler;

                        // GeoNode
                        TYGeometryNode* pEltGeoNode = (TYGeometryNode*)((LPTYInfrastructure&) pElts->at(i + 1))->findBatiment((LPTYBatiment&) pElts->at(i)).getRealPointer();

                        if ((pSite) && (pSite == pmodelerFrame->getSite()))
                        {
                            // Position via le parent (GeoNode)
                            code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_moving"))), TR("id_popup_position"));
                            posRetCodes[code] = pEltGeoNode;

                            // Rotation via le parent (GeoNode)
                            code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_rotation"))), TR("id_popup_rotation"));
                            rotRetCodes[code] = pEltGeoNode;
                        }
                    }
                }
            }
        }
        else if ((pElts->at(i)->inherits("TYAcousticRectangleNode")) &&
                 ((QString(_pModeler->metaObject()->className()).compare("TYBatimentModelerFrame") == 0) ||
                  (QString(_pModeler->metaObject()->className()).compare("TYMachineModelerFrame") == 0)))
        {
            // On permet d'editer une face uniquement dans un modeleur de batiment ou de machine
            rectFound = i;
            editFace = pPopup->addAction(TR("id_popup_editface"));
        }
        else if (pElts->at(i)->inherits("TYAcousticVolume"))
        {
            // Dans tous les cas, on offre la possibilite d'inverser la normales des faces
            volumeFound = i;
            inverseNormales = pPopup->addAction(TR("id_popup_normales"));

            // Possibilite de modification du volume node uniquement dans un modeleur adapte
            if ((QString(_pModeler->metaObject()->className()).compare("TYBatimentModelerFrame") == 0) ||
                (QString(_pModeler->metaObject()->className()).compare("TYMachineModelerFrame") == 0))
            {
                if ((pElts->at(i)->isA("TYEtage")) &&
                    (QString(_pModeler->metaObject()->className()).compare("TYBatimentModelerFrame") == 0))
                {
                    // Hauteur etage
                    etageFound = i;
                    hauteurEtage = pPopup->addAction(TR("id_popup_hauteur"));
                }
                else if ((pElts->at(i)->isA("TYEcran")) &&
                         (QString(_pModeler->metaObject()->className()).compare("TYBatimentModelerFrame") == 0))
                {
                    // Hauteur ecran
                    ecranFound = i;
                    hauteurEcran = pPopup->addAction(TR("id_popup_hauteur"));
                    epaisseurEcran = pPopup->addAction(TR("id_popup_epaisseur"));
                }
                // Deux solutions (sous reserve qu'il existe un element i+1):
                //          1/ On est dans un modeleur de batiment et l'objet suivant est un batiment
                //          2/ On est dans un modeleur de machine et l'objet suivant est une machine
                if ((i + 1 < pElts->size())                                   &&
                    (((QString(_pModeler->metaObject()->className()).compare("TYBatimentModelerFrame") == 0)           &&
                      (pElts->at(i + 1)->isA("TYBatiment")))           ||
                     ((QString(_pModeler->metaObject()->className()).compare("TYMachineModelerFrame") == 0)           &&
                      (pElts->at(i + 1)->isA("TYMachine")))))
                {
                    // GeoNode
                    TYGeometryNode* pEltGeoNode = (TYGeometryNode*)((LPTYAcousticVolumeNode&) pElts->at(i + 1))->findAcousticVol((LPTYAcousticVolume&) pElts->at(i)).getRealPointer();

                    // Position via le parent (GeoNode)
                    code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_moving"))), TR("id_popup_position"));
                    posRetCodes[code] = pEltGeoNode;

                    // Rotation via le parent (GeoNode)
                    code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_rotation"))), TR("id_popup_rotation"));
                    rotRetCodes[code] = pEltGeoNode;

                    if ((pElts->at(i)->isA("TYAcousticBox"))            ||
                        (pElts->at(i)->isA("TYAcousticCylinder"))      ||
                        (pElts->at(i)->isA("TYAcousticSemiCylinder")))
                    {
                        // Dimensions box, cyl et demi cyl
                        code = pPopup->addAction(TR("id_popup_dimension"));
                        dimVolRetCodes[code] = (LPTYAcousticVolume&) pElts->at(i);
                    }

                    // Duplication
                    code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_duplicate"))), TR("id_popup_duplicate"));
                    copyVolRetCodes[code] = (LPTYAcousticVolume&) pElts->at(i);

                    // Suppression
                    code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_del"))), TR("id_popup_remove"));
                    remVolRetCodes[code] = (LPTYAcousticVolume&) pElts->at(i);
                }
            }
        }
        else if (pElts->at(i)->inherits("TYMachine"))
        {
            // Calcul acoustique
            code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_calcul"))), TR("id_popup_calculer"));
            calculVolNodeRetCodes[code] = (LPTYAcousticVolumeNode&) pElts->at(i);

            // Machine
            bool bMachineDansUnEtage = (i + 1 < pElts->size()) && pElts->at(i + 1)->inherits("TYEtage"); //az++
            bool bMachineDansUneInfrastructure = (i + 1 < pElts->size()) && pElts->at(i + 1)->inherits("TYInfrastructure");
            bool bBatimentModeler = QString(_pModeler->metaObject()->className()).compare("TYBatimentModelerFrame") == 0;
            bool bSiteModeler = QString(_pModeler->metaObject()->className()).compare("TYSiteModelerFrame") == 0;
            if ((bMachineDansUnEtage && bBatimentModeler) ||
                (bMachineDansUneInfrastructure && bSiteModeler))
            {
                pSelected = pElts->at(i);

                // GeoNode
                TYGeometryNode* pEltGeoNode;

                bool bCanBeMoved = false;

                if (bMachineDansUnEtage) // et donc dans un modeleur de batiment
                {
                    pEltGeoNode = (TYGeometryNode*)((LPTYEtage&) pElts->at(i + 1))->findMachine((LPTYMachine&) pElts->at(i)).getRealPointer();
                    bCanBeMoved = true; // Catch me if you can !
                }
                else // Machine dans un modeleur de site
                {
                    pEltGeoNode = (TYGeometryNode*)((LPTYInfrastructure&) pElts->at(i + 1))->findMachine((LPTYMachine&) pElts->at(i)).getRealPointer();

                    // Si la machine est dans le modeler de son site parent on a acces a sa position
                    if (((i + 2) < pElts->size()) && (pElts->at(i + 2)->inherits("TYSiteNode")))
                    {
                        LPTYSiteNode pSite = TYSiteNode::safeDownCast(pElts->at(i + 2));
                        TYSiteModelerFrame* pmodelerFrame = (TYSiteModelerFrame*) _pModeler;

                        // GeoNode
                        TYGeometryNode* pEltGeoNode = (TYGeometryNode*)((LPTYInfrastructure&) pElts->at(i + 1))->findBatiment((LPTYBatiment&) pElts->at(i)).getRealPointer();

                        if ((pSite) && (pSite == pmodelerFrame->getSite()))
                        {
                            bCanBeMoved = true;
                        }
                        else
                        {
                            bCanBeMoved = false;
                        }
                    }
                }

                if (bCanBeMoved)
                {
                    // Position via le parent (GeoNode)
                    code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_moving"))), TR("id_popup_position"));
                    posRetCodes[code] = pEltGeoNode;

                    // Rotation via le parent (GeoNode)
                    code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_rotation"))), TR("id_popup_rotation"));
                    rotRetCodes[code] = pEltGeoNode;
                }
            }
        }
        else if (pElts->at(i)->inherits("TYMaillage"))
        {
            // Calcul parent
            if ((i + 1 < pElts->size()) && pElts->at(i + 1)->inherits("TYCalcul"))
            {
                LPTYMaillage pMaillage = (LPTYMaillage&) pElts->at(i);

                // Si le maillage est bien present dans le calcul
                LPTYMaillageGeoNode pMaillageGeoNode = ((LPTYCalcul&) pElts->at(i + 1))->findMaillage(pMaillage);
                if (pMaillageGeoNode)
                {
                    // Position via le parent (GeoNode)
                    code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_moving"))), TR("id_popup_position"));
                    posRetCodes[code] = (TYGeometryNode*) pMaillageGeoNode.getRealPointer();

                    // Rotation via le parent (GeoNode)
                    code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_rotation"))), TR("id_popup_rotation"));
                    rotRetCodes[code] = (TYGeometryNode*) pMaillageGeoNode.getRealPointer();

                    // Duplication
                    code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_duplicate"))), TR("id_popup_duplicate"));
                    copyMaillageRetCodes[code] = pMaillage;

                    // Suppression
                    code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_del"))), TR("id_popup_remove"));
                    remMaillageRetCodes[code] = pMaillage;
                }
            }
        }
        else if (pElts->at(i)->inherits("TYPointControl"))
        {
            // Projet parent
            if ((i + 1 < pElts->size()) && pElts->at(i + 1)->inherits("TYProjet"))
            {
                // Duplication
                code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_duplicate"))), TR("id_popup_duplicate"));
                copyPtControlRetCodes[code] = (LPTYPointControl&) pElts->at(i);

                // Suppression
                code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_del"))), TR("id_popup_remove"));
                remPtControlRetCodes[code] = (LPTYPointControl&) pElts->at(i);
            }
        }
        // Site
        else if (pElts->at(i)->inherits("TYSiteNode")) // Donc on est forcemment dans un modelerSiteFrame
        {
            LPTYSiteNode pSiteNode = (LPTYSiteNode&) pElts->at(i);

            if ((i + 1 < pElts->size()) && pElts->at(i + 1)->inherits("TYSiteNode")) // On est bien dans un site imbrique dans un autre
            {
                LPTYSiteNode pSite = TYSiteNode::safeDownCast(pElts->at(i + 1));

                if (QString(_pModeler->metaObject()->className()).compare("TYSiteModelerFrame") == 0) // On est bien dans un modeleur de site
                {
                    TYSiteModelerFrame* pmodelerFrame = (TYSiteModelerFrame*) _pModeler;

                    if ((pSite) && (pSite == pmodelerFrame->getSite()))
                    {
                        // Les actions suivantes ne peuvent etre faites que dans le siteframe du site englobant le site picke
                        // GeoNode
                        TYGeometryNode* pEltGeoNode = (TYGeometryNode*)((LPTYSiteNode&) pElts->at(i + 1))->findSiteNode(pSiteNode).getRealPointer();

                        // Ouverture d'un modeleur sur le site
                        pSelected = pElts->at(i);

                        // Position via le parent (GeoNode)
                        code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_moving"))), TR("id_popup_position"));
                        posRetCodes[code] = pEltGeoNode;

                        // Rotation via le parent (GeoNode)
                        code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_rotation"))), TR("id_popup_rotation"));
                        rotRetCodes[code] = pEltGeoNode;

                        // Duplication
                        code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_duplicate"))), TR("id_popup_duplicate"));
                        copySiteNodeRetCodes[code] = pSiteNode;

                        // Suppression
                        code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_del"))), TR("id_popup_remove"));
                        remSiteNodeRetCodes[code] = pSiteNode;
                    }
                }
            }
        }
        // Elts de Topo
        if ((i + 1 < pElts->size()) && (pElts->at(i + 1)->isA("TYTopographie")))
        {

            if (((i + 2) < pElts->size()) && (pElts->at(i + 2)->inherits("TYSiteNode")))
            {
                LPTYSiteNode pSite = TYSiteNode::safeDownCast(pElts->at(i + 2));
                TYSiteModelerFrame* pmodelerFrame = (TYSiteModelerFrame*) _pModeler;

                // On ne peut dupliquer ou supprimer que dans le modeler de son site
                if ((pSite) && (pSite == pmodelerFrame->getSite()))
                {
                    // Duplication
                    code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_duplicate"))), TR("id_popup_duplicate"));
                    copyTopoRetCodes[code] = pElts->at(i);

                    // Suppression
                    code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_del"))), TR("id_popup_remove"));
                    remTopoRetCodes[code] = pElts->at(i);
                }
            }
        }
        // Elts d'Infra
        else if ((i + 1 < pElts->size()) && (pElts->at(i + 1)->isA("TYInfrastructure")) && (QString(_pModeler->metaObject()->className()).compare("TYSiteModelerFrame") == 0))
        {
            if (((i + 2) < pElts->size()) && (pElts->at(i + 2)->inherits("TYSiteNode")))
            {
                LPTYSiteNode pSite = TYSiteNode::safeDownCast(pElts->at(i + 2));
                TYSiteModelerFrame* pmodelerFrame = (TYSiteModelerFrame*) _pModeler;

                // On ne peut dupliquer ou supprimer que dans le modeler de son site
                if ((pSite) && (pSite == pmodelerFrame->getSite()))
                {
                    // Duplication
                    code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_duplicate"))), TR("id_popup_duplicate"));
                    copyInfraRetCodes[code] = pElts->at(i);

                    // Suppression
                    code = pPopup->addAction(QIcon(QPixmap(IMG("id_icon_del"))), TR("id_popup_remove"));
                    remInfraRetCodes[code] = pElts->at(i);
                }
            }
        }
    }

    // Gestion du choix de l'utilisateur

    pPopup->setMouseTracking(true);
    bool bUpdateDisplayList = false;
    QAction* popupRet = pPopup->exec(QCursor::pos());
    if (popupRet == NULL) { return; }
    qApp->processEvents();

    if (popupRet == hauteurEtage)
    {
        if (_pModeler->askForResetResultat())
        {
            TYEtage* pEtage = (LPTYEtage&) pElts->at(etageFound);

            bool ok = false;
            double hauteur = QInputDialog::getDouble(_pModeler, "", TR("id_msg_gethauteur"), pEtage->getHauteur(), 0, 1000, 2, &ok);

            if (ok)
            {
                pEtage->setHauteur(hauteur);
                pEtage->updateGraphic();
                bUpdateDisplayList = true;
                _pInteractor->updateGL();
            }
        }
    }
    if (popupRet == hauteurEcran)
    {
        if (_pModeler->askForResetResultat())
        {
            TYEcran* pEcran = (LPTYEcran&) pElts->at(ecranFound);

            bool ok = false;
            double hauteur = QInputDialog::getDouble(_pModeler, "", TR("id_msg_gethauteur_ecran"), pEcran->getHauteur(), 0, 1000, 2, &ok);

            if (ok)
            {
                pEcran->setHauteur(hauteur);
                pEcran->updateGeometry();
                pEcran->updateGraphic();
                bUpdateDisplayList = true;
                _pInteractor->updateGL();
            }
        }
    }
    if (popupRet == epaisseurEcran)
    {
        if (_pModeler->askForResetResultat())
        {
            TYEcran* pEcran = (LPTYEcran&) pElts->at(ecranFound);

            bool ok = false;
            double epaisseur = QInputDialog::getDouble(_pModeler, "", TR("id_msg_getepaisseur_ecran"), pEcran->getEpaisseur(), 0, 1000, 2, &ok);

            if (ok)
            {
                pEcran->setEpaisseur(epaisseur);
                pEcran->updateGeometry();
                pEcran->updateGraphic();
                bUpdateDisplayList = true;
                _pInteractor->updateGL();
            }
        }
    }
    else if (calculVolNodeRetCodes.find(popupRet) != calculVolNodeRetCodes.end())
    {
        if (_pModeler->askForResetResultat())
        {
            getTYApp()->getCalculManager()->updateAcoustic(calculVolNodeRetCodes[popupRet]);
        }
    }
    else if (popupRet == editFace)
    {
        bUpdateDisplayList = true;
        TYFaceModelerFrame* pFaceMdF;

        if (strcmp(pElts->at(rectFound)->getClassName(), "TYMur") == 0)
        {
            TYMur* pMur = (LPTYMur&) pElts->at(rectFound);
            pFaceMdF = new TYFaceModelerFrame(pMur, getTYMainWnd()->getWorkspace(), "face modeler");
            pFaceMdF->setAttribute(Qt::WA_DeleteOnClose);
            // CLM-NT35: Gestion MDI avec QT4.7
#ifdef USE_QMDIAREA
            getTYMainWnd()->getWorkspace()->addSubWindow(pFaceMdF)->setObjectName("TYFaceModelerFrame");
#else
            getTYMainWnd()->getWorkspace()->addWindow(pFaceMdF)->setObjectName("TYFaceModelerFrame");
#endif
            // CLM-NT35 End
        }
        else
        {
            TYAcousticRectangleNode* pAccRectNode = (LPTYAcousticRectangleNode&) pElts->at(rectFound);
            pFaceMdF = new TYFaceModelerFrame(pAccRectNode, getTYMainWnd()->getWorkspace(), "face modeler");
            pFaceMdF->setAttribute(Qt::WA_DeleteOnClose);
            // CLM-NT35: Gestion MDI avec QT4.7
#ifdef USE_QMDIAREA
            getTYMainWnd()->getWorkspace()->addSubWindow(pFaceMdF)->setObjectName("TYFaceModelerFrame");
#else
            getTYMainWnd()->getWorkspace()->addWindow(pFaceMdF)->setObjectName("TYFaceModelerFrame");
#endif
            // CLM-NT35 End
        }

        pFaceMdF->showMaximized();
        pFaceMdF->fit();
    }
    else if (popupRet == showModelerMachine)
    {
        getTYMainWnd()->makeMachineModeler((TYMachine*) pSelected);
        getTYMainWnd()->updateModelers();
    }
    else if (popupRet == showModelerBatiment)
    {
        getTYMainWnd()->makeBatimentModeler((LPTYBatiment&) pSelected);
    }
    else if (popupRet == showModelerSite)
    {
        getTYMainWnd()->makeSiteModeler((TYSiteNode*) pSelected);
    }
    else if (popupRet == inverseNormales)
    {
        TYAcousticVolume* pVol = TYAcousticVolume::safeDownCast(pElts->at(volumeFound));
        if (pVol) { pVol->inverseNormales(); }
        pVol->setNormalStatus();
    }
    else if (retCodes.find(popupRet) != retCodes.end())
    {
        // Affiche le widget de l'element
        int ret = pElts->at(retCodes[popupRet])->edit(_pModeler);

        if (ret == QDialog::Accepted)
        {
            // Mise a jour du projet
            TYProjet* pProjet = getTYApp()->getCurProjet();
            if (pProjet)
            {
                TYSiteNode* pSite = pProjet->getSite();
                if (pSite)
                {
                    pSite->update(pElts->at(retCodes[popupRet]));
					getTYMainWnd()->getSiteFrame()->updateList();
                }
            }

            // Mise a jour graphic
            if ((pElts->at(retCodes[popupRet])->inherits("TYAcousticSurface")) ||
                (pElts->at(retCodes[popupRet])->inherits("TYAcousticSurfaceNode")) ||
                (pElts->at(retCodes[popupRet])->inherits("TYAcousticCylinder")) ||
                (pElts->at(retCodes[popupRet])->inherits("TYAcousticSemiCylinder")))
            {
                pElts->at(retCodes[popupRet])->updateGraphicTree();
                bUpdateDisplayList = false;
            }
            else if (pElts->at(retCodes[popupRet])->isA("TYRectangularMaillage"))
            {
                TYRectangularMaillage* pRectMaillage = (LPTYRectangularMaillage&) pElts->at(retCodes[popupRet]);
                LPTYElementGraphic pMGr = pRectMaillage->getGraphicObject();
                pRectMaillage->updateGraphic();
                bUpdateDisplayList = true; // DT++
            }
            else if (pElts->at(retCodes[popupRet])->isA("TYCourbeNiveau") ||
                     pElts->at(retCodes[popupRet])->isA("TYPlanEau"))
            {
                bUpdateDisplayList = true; // DT++
            }
            else
            {
                pElts->at(retCodes[popupRet])->updateGraphic();
            }
        }
    }
    else if (dimVolRetCodes.find(popupRet) != dimVolRetCodes.end())
    {
        // On preserve la dimension en hauteur de l'objet
        TYAcousticVolume* pVol = TYAcousticVolume::safeDownCast(dimVolRetCodes[popupRet]);
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
        showDimensionsDialog(dimVolRetCodes[popupRet]);

        // Modification de la position pour tenir compte des nouvelles dimensions
        LPTYAcousticVolumeNode pParent = (TYAcousticVolumeNode*) pVol->getParent();
        LPTYAcousticVolumeGeoNode pGeoNode = pParent->findAcousticVol(pVol);

        if (pGeoNode)
        {
            ORepere3D repere = pGeoNode->getRepere();
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


        if ((pElts->at(retCodes[popupRet])->inherits("TYAcousticSurface")) ||
            (pElts->at(retCodes[popupRet])->inherits("TYAcousticSurfaceNode")) ||
            (pElts->at(retCodes[popupRet])->inherits("TYAcousticCylinder")))
        {
            pElts->at(retCodes[popupRet])->updateGraphicTree();
        }
        else if (pElts->at(retCodes[popupRet])->isA("TYRectangularMaillage"))
        {
            TYRectangularMaillage* pRectMaillage = (LPTYRectangularMaillage&) pElts->at(retCodes[popupRet]);
            LPTYElementGraphic pMGr = pRectMaillage->getGraphicObject();
            pRectMaillage->updateGraphic();
        }
        else
        {
            pElts->at(retCodes[popupRet])->updateGraphic();
        }
        bUpdateDisplayList = true;
    }
    else if (posRetCodes.find(popupRet) != posRetCodes.end())
    {
        showPositionDialog(posRetCodes[popupRet]);
        bUpdateDisplayList = true;
        getTYMainWnd()->updateModelers(false);
    }
    else if (rotRetCodes.find(popupRet) != rotRetCodes.end())
    {
        showRotationDialog(rotRetCodes[popupRet]);
        bUpdateDisplayList = true;
        getTYMainWnd()->updateModelers(false);
    }
    else if (remVolRetCodes.find(popupRet) != remVolRetCodes.end())
    {
        if (_pModeler->askForResetResultat())
        {
            LPTYAcousticVolumeNode pParent = (TYAcousticVolumeNode*) remVolRetCodes[popupRet]->getParent();
            LPTYAcousticVolumeGeoNode pGeoNode = pParent->findAcousticVol(remVolRetCodes[popupRet]);

            if (pParent->remAcousticVol(remVolRetCodes[popupRet]))
            {
                TYAction* pAction = new TYRemAccVolToAccVolNodeAction(pGeoNode, (TYAcousticVolumeNode*) remVolRetCodes[popupRet]->getParent(), _pModeler, TR("id_action_remvol"));
                _pModeler->getActionManager()->addAction(pAction);

                pRemovedEltParent = pParent;
            }

            updateSiteFrame();
            getTYMainWnd()->updateModelers(false);

            // La scene a ete modifiee
            TYElement::setIsSavedOk(true);
        }
    }
    else if (copyVolRetCodes.find(popupRet) != copyVolRetCodes.end())
    {
        if (_pModeler->askForResetResultat())
        {
            LPTYAcousticVolumeGeoNode pGeoNode = ((TYAcousticVolumeNode*) copyVolRetCodes[popupRet]->getParent())->findAcousticVol(copyVolRetCodes[popupRet]);

            // Nouvel element du meme type que l'objet a dupliquer (clone)
            LPTYAcousticVolumeGeoNode pCopy = new TYAcousticVolumeGeoNode((TYAcousticVolume*) copyVolRetCodes[popupRet]->clone());

            // Duplication
            pCopy->deepCopy(pGeoNode, false);

            // Ajout
            if (((TYAcousticVolumeNode*) copyVolRetCodes[popupRet]->getParent())->addAcousticVol(pCopy))
            {
                // Offset
                ORepere3D repere = pCopy->getRepere();

                if (pCopy->getElement()->inherits("TYEtage"))
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
                TYAction* pAction = new TYAddAccVolToAccVolNodeAction(pCopy, (TYAcousticVolumeNode*) copyVolRetCodes[popupRet]->getParent(), _pModeler, TR("id_action_addvol"));
                _pModeler->getActionManager()->addAction(pAction);

                pAddedElt = pCopy->getElement();
            }

            updateSiteFrame();
            bUpdateDisplayList = true; // DT++

            // La scene a ete modifiee
            TYElement::setIsSavedOk(true);
        }
    }
    else if (copyMachineRetCodes.find(popupRet) != copyMachineRetCodes.end())
    {
        if (_pModeler->askForResetResultat())
        {
            LPTYMachineGeoNode pGeoNode = ((TYEtage*) copyMachineRetCodes[popupRet]->getParent())->findMachine(copyMachineRetCodes[popupRet]);

            // Nouvel element du meme type que l'objet a dupliquer (clone)
            LPTYMachineGeoNode pCopy = new TYMachineGeoNode((TYMachine*) copyMachineRetCodes[popupRet]->clone());

            // Duplication
            pCopy->deepCopy(pGeoNode, false);

            // Ajout
            if (((TYEtage*) copyMachineRetCodes[popupRet]->getParent())->addMachine(pCopy))
            {
                ORepere3D repere = pCopy->getRepere();
                // Offset
                repere._origin._x += 5.0;
                repere._origin._y -= 5.0;

                pCopy->setRepere(repere);

                // Update Graphic
                pCopy->updateGraphicTree();

                // Action
                TYAction* pAction = new TYAddMachineToEtageAction(pCopy, (TYEtage*) copyMachineRetCodes[popupRet]->getParent(), _pModeler, TR("id_action_addmachine"));
                _pModeler->getActionManager()->addAction(pAction);

                pAddedElt = pCopy->getElement();
            }

            updateSiteFrame();
            getTYMainWnd()->updateModelers(false);

            // La scene a ete modifiee
            TYElement::setIsSavedOk(true);
        }
    }
    else if (remMachineRetCodes.find(popupRet) != remMachineRetCodes.end())
    {
        if (_pModeler->askForResetResultat())
        {
            LPTYMachineGeoNode pGeoNode = ((TYEtage*) remMachineRetCodes[popupRet]->getParent())->findMachine(remMachineRetCodes[popupRet]);
            LPTYEtage pParent = (TYEtage*) remMachineRetCodes[popupRet]->getParent();

            if (pParent->remMachine(remMachineRetCodes[popupRet]))
            {
                TYAction* pAction = new TYRemMachineToEtageAction(pGeoNode, (TYEtage*) remMachineRetCodes[popupRet]->getParent(), _pModeler, TR("id_action_remmachine"));
                _pModeler->getActionManager()->addAction(pAction);

                pRemovedEltParent = pParent;
            }

            updateSiteFrame();
            getTYMainWnd()->updateModelers(false);

            // La scene a ete modifiee
            TYElement::setIsSavedOk(true);
        }
    }
    else if (remTopoRetCodes.find(popupRet) != remTopoRetCodes.end())
    {
        if (_pModeler->askForResetResultat())
        {
            TYTopographie* pTopo = (TYTopographie*) remTopoRetCodes[popupRet]->getParent();
            // On recupere le site parent
            TYSiteNode* pSiteParent = TYSiteNode::safeDownCast(pTopo->getParent());

            TYAction* pAction = new TYRemElementToTopoAction(remTopoRetCodes[popupRet], pTopo, _pModeler, TR("id_action_remelttopo"));
            _pModeler->getActionManager()->addAction(pAction);

            if (remTopoRetCodes[popupRet]->isA("TYCourbeNiveau"))
            {
                pTopo->remCrbNiv((LPTYCourbeNiveau&) remTopoRetCodes[popupRet]);
            }
            else if (remTopoRetCodes[popupRet]->isA("TYTerrain"))
            {
                pTopo->remTerrain((LPTYTerrain&) remTopoRetCodes[popupRet]);
            }
            else if (remTopoRetCodes[popupRet]->isA("TYCoursEau"))
            {
                pTopo->remCrsEau((LPTYCoursEau&) remTopoRetCodes[popupRet]);
            }
            else if (remTopoRetCodes[popupRet]->isA("TYPlanEau"))
            {
                pTopo->remPlanEau((LPTYPlanEau&) remTopoRetCodes[popupRet]);
            }

            pRemovedEltParent = pTopo;

            // On altimetrise aussi le sous-site (si s'en est un !)
            if (pSiteParent && !pSiteParent->getRoot()) { pSiteParent->updateAltimetrie(true); }

            // On met a jour l'altimetrie globale du site
            TYProjet* pProjet = getTYApp()->getCurProjet();
            if (pProjet)
            {
                pProjet->getSite()->updateAltimetrie(true);
                pProjet->getSite()->updateAltiInfra(true);
                pProjet->updateAltiRecepteurs();

                pProjet->getSite()->getTopographie()->updateGraphicTree();
            }

            bUpdateDisplayList = true; // DT++

            updateSiteFrame();
            getTYMainWnd()->updateModelers(true, true, true);

            // La scene a ete modifiee
            TYElement::setIsSavedOk(true);
        }
    }
    else if (copyTopoRetCodes.find(popupRet) != copyTopoRetCodes.end())
    {
        if (_pModeler->askForResetResultat())
        {
            TYTopographie* pTopo = (TYTopographie*) copyTopoRetCodes[popupRet]->getParent();
            SmartPtr<TYGeometryNode > pCopyTmp = NULL;

            if (copyTopoRetCodes[popupRet]->isA("TYCourbeNiveau"))
            {
                // Nouvelle element
                LPTYCourbeNiveauGeoNode pCopy = new TYCourbeNiveauGeoNode(new TYCourbeNiveau());
                pCopyTmp = (SmartPtr<TYGeometryNode >&) pCopy;

                // Duplication
                pCopy->deepCopy(pTopo->findCrbNiv((LPTYCourbeNiveau&) copyTopoRetCodes[popupRet]), false);

                // Ajout
                pTopo->addCrbNiv(pCopy);
            }
            else if (copyTopoRetCodes[popupRet]->isA("TYTerrain"))
            {
                // Nouvelle element
                LPTYTerrainGeoNode pCopy = new TYTerrainGeoNode(new TYTerrain);
                pCopyTmp = (SmartPtr<TYGeometryNode >&) pCopy;

                // Duplication
                pCopy->deepCopy(pTopo->findTerrain((LPTYTerrain&) copyTopoRetCodes[popupRet]), false);

                // Ajout
                pTopo->addTerrain(pCopy);
            }
            else if (copyTopoRetCodes[popupRet]->isA("TYCoursEau"))
            {
                // Nouvelle element
                LPTYCoursEauGeoNode pCopy = new TYCoursEauGeoNode(new TYCoursEau);
                pCopyTmp = (SmartPtr<TYGeometryNode >&) pCopy;

                // Duplication
                pCopy->deepCopy(pTopo->findCrsEau((LPTYCoursEau&) copyTopoRetCodes[popupRet]), false);

                // Ajout
                pTopo->addCrsEau(pCopy);
            }
            else if (copyTopoRetCodes[popupRet]->isA("TYPlanEau"))
            {
                // Nouvelle element
                LPTYPlanEauGeoNode pCopy = new TYPlanEauGeoNode(new TYPlanEau);
                pCopyTmp = (SmartPtr<TYGeometryNode >&) pCopy;

                // Duplication
                pCopy->deepCopy(pTopo->findPlanEau((LPTYPlanEau&) copyTopoRetCodes[popupRet]), false);

                // Ajout
                pTopo->addPlanEau(pCopy);
            }

            if (pCopyTmp)
            {
                ORepere3D repere = pCopyTmp->getRepere();
                // Offset
                repere._origin._x += 50.0;
                repere._origin._y -= 50.0;
                pCopyTmp->setRepere(repere);

                // Action
                TYAction* pAction = new TYAddElementToTopoAction(pCopyTmp, pTopo, _pModeler, TR("id_action_addelttopo"));
                _pModeler->getActionManager()->addAction(pAction);

                // Update graphic
                pCopyTmp->updateGraphicTree();

                pAddedElt = pCopyTmp->getElement();
            }

            updateSiteFrame();
            getTYMainWnd()->updateModelers(false);

            // La scene a ete modifiee
            TYElement::setIsSavedOk(true);
        }
    }
    else if (remInfraRetCodes.find(popupRet) != remInfraRetCodes.end())
    {
        if (_pModeler->askForResetResultat())
        {
            TYInfrastructure* pInfra = (TYInfrastructure*) remInfraRetCodes[popupRet]->getParent();

            TYAction* pAction = new TYRemElementToInfraAction(remInfraRetCodes[popupRet], pInfra, _pModeler, TR("id_action_remeltinfra"));
            _pModeler->getActionManager()->addAction(pAction);

            if (remInfraRetCodes[popupRet]->isA("TYRoute"))
            {
                pInfra->remRoute((LPTYRoute&) remInfraRetCodes[popupRet]);
            }
            else if (remInfraRetCodes[popupRet]->isA("TYReseauTransport"))
            {
                pInfra->remResTrans((LPTYReseauTransport&) remInfraRetCodes[popupRet]);
            }
            else if (remInfraRetCodes[popupRet]->isA("TYBatiment"))
            {
                pInfra->remBatiment((LPTYBatiment&) remInfraRetCodes[popupRet]);
            }
            else if (remInfraRetCodes[popupRet]->isA("TYMachine"))
            {
                pInfra->remMachine((LPTYMachine&) remInfraRetCodes[popupRet]);
            }
            else if (remInfraRetCodes[popupRet]->isA("TYUserSourcePonctuelle"))
            {
                pInfra->remSrc((LPTYUserSourcePonctuelle&) remInfraRetCodes[popupRet]);
            }

            pRemovedEltParent = pInfra;

            updateSiteFrame(); // Mise a jour de l'arborescence du site
            getTYMainWnd()->updateModelers(false);

            // La scene a ete modifiee
            TYElement::setIsSavedOk(true);
        }
    }
    else if (copyInfraRetCodes.find(popupRet) != copyInfraRetCodes.end())
    {
        if (_pModeler->askForResetResultat())
        {
            TYInfrastructure* pInfra = (TYInfrastructure*) copyInfraRetCodes[popupRet]->getParent();
            SmartPtr<TYGeometryNode > pCopyTmp = NULL;
            TYBox boundBox;

            if (copyInfraRetCodes[popupRet]->isA("TYRoute"))
            {
                // Nouvelle element
                LPTYRouteGeoNode pCopy = new TYRouteGeoNode(new TYRoute);
                pCopyTmp = (SmartPtr<TYGeometryNode >&) pCopy;

                // Duplication
                pCopy->deepCopy(pInfra->findRoute((LPTYRoute&) copyInfraRetCodes[popupRet]), false);

                // Ajout
                pInfra->addRoute(pCopy);

                updateSiteFrame(); // Mise a jour de l'arborescence du site
                getTYMainWnd()->updateModelers(false);

                // On l'ajoute dans le calcul courant (si calcul courant il y a !)
                if (getTYApp()->getCurProjet() && getTYApp()->getCurProjet()->getCurrentCalcul())
                {
                    getTYApp()->getCurProjet()->getCurrentCalcul()->addToSelection(pCopy->getElement());
                }
            }
            else if (copyInfraRetCodes[popupRet]->isA("TYReseauTransport"))
            {
                // Nouvelle element
                LPTYReseauTransportGeoNode pCopy = new TYReseauTransportGeoNode(new TYReseauTransport);
                pCopyTmp = (SmartPtr<TYGeometryNode >&) pCopy;

                // Duplication
                pCopy->deepCopy(pInfra->findResTrans((LPTYReseauTransport&) copyInfraRetCodes[popupRet]), false);

                // Ajout
                pInfra->addResTrans(pCopy);

                updateSiteFrame(); // Mise a jour de l'arborescence du site
                getTYMainWnd()->updateModelers(false);

                // On l'ajoute dans le calcul courant (si calcul courant il y a !)
                if (getTYApp()->getCurProjet() && getTYApp()->getCurProjet()->getCurrentCalcul())
                {
                    getTYApp()->getCurProjet()->getCurrentCalcul()->addToSelection(pCopy->getElement());
                }
            }
            else if (copyInfraRetCodes[popupRet]->isA("TYBatiment"))
            {
                // Nouvelle element
                LPTYBatimentGeoNode pCopy = new TYBatimentGeoNode(new TYBatiment);
                pCopyTmp = (SmartPtr<TYGeometryNode >&) pCopy;

                // Duplication
                pCopy->deepCopy(pInfra->findBatiment((LPTYBatiment&) copyInfraRetCodes[popupRet]), false);

                boundBox = TYBatiment::safeDownCast(pCopy->getElement())->volEnglob();

                // Ajout
                pInfra->addBatiment(pCopy);

                updateSiteFrame(); // Mise a jour de l'arborescence du site
                getTYMainWnd()->updateModelers(false);

                // On l'ajoute dans le calcul courant (si calcul courant il y a !)
                if (getTYApp()->getCurProjet() && getTYApp()->getCurProjet()->getCurrentCalcul())
                {
                    getTYApp()->getCurProjet()->getCurrentCalcul()->addToSelection(pCopy->getElement());
                }

                // Message d'alerte sur le fonctionnement des machine dupliquees
                OMessageManager::get()->info("**********************************************************************\n");
                OMessageManager::get()->info("*                      ATTENTION !!!                                 *\n\n\n");
                OMessageManager::get()->info("* Les machines a l'interieur du batiment duplique ne sont pas active *\n\n");
                OMessageManager::get()->info("*                PENSEZ A LES REACTIVER                              *\n\n");
                OMessageManager::get()->info("**********************************************************************\n");

            }
            else if (copyInfraRetCodes[popupRet]->isA("TYMachine"))
            {
                // Nouvelle element
                LPTYMachineGeoNode pCopy = new TYMachineGeoNode(new TYMachine);
                pCopyTmp = (SmartPtr<TYGeometryNode >&) pCopy;

                // Duplication
                pCopy->deepCopy(pInfra->findMachine((LPTYMachine&) copyInfraRetCodes[popupRet]), false);

                boundBox = TYMachine::safeDownCast(pCopy->getElement())->volEnglob();

                // Ajout
                pInfra->addMachine(pCopy);

                updateSiteFrame(); // Mise a jour de l'arborescence du site
                getTYMainWnd()->updateModelers(false);

                // On l'ajoute dans le calcul courant (si calcul courant il y a !)
                if (getTYApp()->getCurProjet() && getTYApp()->getCurProjet()->getCurrentCalcul())
                {
                    getTYApp()->getCurProjet()->getCurrentCalcul()->addToSelection(pCopy->getElement());
                }
            }
            else if (copyInfraRetCodes[popupRet]->isA("TYUserSourcePonctuelle"))
            {
                // Nouvelle element
                LPTYSourcePonctuelleGeoNode pCopy = new TYSourcePonctuelleGeoNode(new TYSourcePonctuelle);
                pCopyTmp = (SmartPtr<TYGeometryNode >&) pCopy;

                // Duplication
                pCopy->deepCopy(pInfra->findSrc((LPTYUserSourcePonctuelle&) copyInfraRetCodes[popupRet]), false);

                // Ajout
                pInfra->addSrc(pCopy);

                // Offset
                TYUserSourcePonctuelle::safeDownCast(pCopy->getElement())->getPos()->_x += 10;
                TYUserSourcePonctuelle::safeDownCast(pCopy->getElement())->getPos()->_y -= 10;

                // Update graphic
                pCopy->updateGraphicTree();

                updateSiteFrame(); // Mise a jour de l'arborescence du site
                getTYMainWnd()->updateModelers(false);

                // On l'ajoute dans le calcul courant (si calcul courant il y a !)
                if (getTYApp()->getCurProjet() && getTYApp()->getCurProjet()->getCurrentCalcul())
                {
                    getTYApp()->getCurProjet()->getCurrentCalcul()->addToSelection(pCopy->getElement());
                }
            }

            if (pCopyTmp)
            {
                if (!copyInfraRetCodes[popupRet]->isA("TYUserSourcePonctuelle"))
                {
                    ORepere3D repere = pCopyTmp->getRepere();

                    // Offset proportionnel a la taille de l'objet
                    repere._origin._x += boundBox._sizeX; //10.0;
                    repere._origin._y -= boundBox._sizeY; //10.0;

                    pCopyTmp->setRepere(repere);
                }

                // Action
                TYAction* pAction = new TYAddElementToInfraAction(pCopyTmp, pInfra, _pModeler, TR("id_action_addeltinfra"));
                _pModeler->getActionManager()->addAction(pAction);

                // Update graphic
                pCopyTmp->updateGraphicTree();

                pAddedElt = pCopyTmp->getElement();
                updateSiteFrame(); // Mise a jour de l'arborescence du site
                getTYMainWnd()->updateModelers(false);
            }

            // La scene a ete modifiee
            TYElement::setIsSavedOk(true);
        }
    }
    else if (copyMaillageRetCodes.find(popupRet) != copyMaillageRetCodes.end())
    {
        if (_pModeler->askForResetResultat())
        {
            LPTYMaillageGeoNode pGeoNode = ((TYCalcul*) copyMaillageRetCodes[popupRet]->getParent())->findMaillage(copyMaillageRetCodes[popupRet]);

            // Nouvel element du meme type que l'objet a dupliquer (clone)
            LPTYMaillageGeoNode pCopy = new TYMaillageGeoNode((TYMaillage*) copyMaillageRetCodes[popupRet]->clone());

            // Duplication
            pCopy->deepCopy(pGeoNode, false);

            // Ajout
            if (((TYCalcul*) copyMaillageRetCodes[popupRet]->getParent())->addMaillage(pCopy))
            {
                ORepere3D repere = pCopy->getRepere();
                // Offset
                repere._origin._x += 10.0;
                repere._origin._y -= 10.0;

                pCopy->setRepere(repere);

                // Update Graphic
                pCopy->updateGraphicTree();

                // Action
                TYAction* pAction = new TYAddMaillageToCalculAction(pCopy, (TYCalcul*) copyMaillageRetCodes[popupRet]->getParent(), _pModeler, TR("id_action_addmaillage"));
                _pModeler->getActionManager()->addAction(pAction);

                refreshProjectFrame();
                getTYMainWnd()->updateModelers(false);
                bUpdateDisplayList = true;
            }

            // La scene a ete modifiee
            TYElement::setIsSavedOk(true);
        }
    }
    else if (remMaillageRetCodes.find(popupRet) != remMaillageRetCodes.end())
    {
        if (_pModeler->askForResetResultat())
        {
            LPTYMaillageGeoNode pGeoNode = ((TYCalcul*) remMaillageRetCodes[popupRet]->getParent())->findMaillage(remMaillageRetCodes[popupRet]);
            LPTYCalcul pParent = (TYCalcul*) remMaillageRetCodes[popupRet]->getParent();

            if (pParent->remMaillage(remMaillageRetCodes[popupRet]))
            {
                TYAction* pAction = new TYRemMaillageToCalculAction(pGeoNode, (TYCalcul*) remMaillageRetCodes[popupRet]->getParent(), _pModeler, TR("id_action_remmaillage"));
                _pModeler->getActionManager()->addAction(pAction);

                pRemovedEltParent = pParent;
            }

            refreshProjectFrame();
            getTYMainWnd()->updateModelers(false);
            bUpdateDisplayList = true;

            // La scene a ete modifiee
            TYElement::setIsSavedOk(true);
        }
    }
    else if (copyPtControlRetCodes.find(popupRet) != copyPtControlRetCodes.end())
    {
        if (_pModeler->askForResetResultat())
        {
            // Nouvel element du meme type que l'objet a dupliquer (clone)
            LPTYPointControl pCopy = (TYPointControl*) copyPtControlRetCodes[popupRet]->clone();

            // Duplication
            pCopy->deepCopy((TYPointControl*) copyPtControlRetCodes[popupRet], false);

            // Ajout
            if (((TYProjet*) copyPtControlRetCodes[popupRet]->getParent())->addPointControl(pCopy))
            {
                // Offset
                pCopy->_x += 10.0;
                pCopy->_y -= 10.0;

                // Update Graphic
                pCopy->updateGraphicTree();

                // Action
                TYAction* pAction = new TYAddPointControlAction((TYProjet*) copyPtControlRetCodes[popupRet]->getParent(), pCopy, _pModeler, TR("id_action_addptcontrol"));
                _pModeler->getActionManager()->addAction(pAction);
            }

            refreshProjectFrame();
            getTYMainWnd()->updateModelers(false);

            // La scene a ete modifiee
            TYElement::setIsSavedOk(true);
        }
    }
    else if (remPtControlRetCodes.find(popupRet) != remPtControlRetCodes.end())
    {
        if (_pModeler->askForResetResultat())
        {
            LPTYProjet pParent = (TYProjet*) remPtControlRetCodes[popupRet]->getParent();

            if (pParent->remPointControl(remPtControlRetCodes[popupRet]))
            {
                TYAction* pAction = new TYRemPointControlAction((TYProjet*) remPtControlRetCodes[popupRet]->getParent(), (LPTYPointControl&) remPtControlRetCodes[popupRet], _pModeler, TR("id_action_remptcontrol"));
                _pModeler->getActionManager()->addAction(pAction);

                pRemovedEltParent = pParent;
            }

            refreshProjectFrame();
            getTYMainWnd()->updateModelers(false);

            // La scene a ete modifiee
            TYElement::setIsSavedOk(true);
        }
    }
    else if (copySiteNodeRetCodes.find(popupRet) != copySiteNodeRetCodes.end())
    {
        if (_pModeler->askForResetResultat())
        {
            LPTYSiteNodeGeoNode pGeoNode = ((TYSiteNode*) copySiteNodeRetCodes[popupRet]->getParent())->findSiteNode(copySiteNodeRetCodes[popupRet]);

            // Nouvel element du meme type que l'objet a dupliquer (clone)
            LPTYSiteNodeGeoNode pCopy = new TYSiteNodeGeoNode((TYSiteNode*) copySiteNodeRetCodes[popupRet]->clone());

            // Duplication
            pCopy->deepCopy(pGeoNode, false);

            // Ajout
            if (((TYSiteNode*) copySiteNodeRetCodes[popupRet]->getParent())->addSiteNode(pCopy))
            {
                ORepere3D repere = pCopy->getRepere();
                // Offset
                repere._origin._x += 100.0;
                repere._origin._y -= 100.0;

                pCopy->setRepere(repere);

				LPTYCalcul pCalc = NULL;
				TYSiteNode *pSite = NULL;
				pSite = dynamic_cast<TYSiteNode*>( pCopy->getElement() ) ;
				if (getTYApp()->getCurProjet() && pSite)
				{
						pCalc = getTYApp()->getCurProjet()->getCurrentCalcul();
						if (pCalc) { pCalc->getCalculElements(pSite); }
				}

                // Update Graphic
                pCopy->updateGraphicTree();

                // Action
                TYAction* pAction = new TYAddSiteNodeToSiteNodeAction(pCopy, (TYSiteNode*) copySiteNodeRetCodes[popupRet]->getParent(), _pModeler, TR("id_action_addsitenode"));
                _pModeler->getActionManager()->addAction(pAction);

                pAddedElt = pCopy->getElement();
            }

            updateSiteFrame();
            getTYMainWnd()->updateModelers(false);

            // La scene a ete modifiee
            TYElement::setIsSavedOk(true);
        }
    }
    else if (remSiteNodeRetCodes.find(popupRet) != remSiteNodeRetCodes.end())
    {
        if (_pModeler->askForResetResultat())
        {
            LPTYSiteNodeGeoNode pGeoNode = ((TYSiteNode*) remSiteNodeRetCodes[popupRet]->getParent())->findSiteNode(remSiteNodeRetCodes[popupRet]);
            LPTYSiteNode pParent = (TYSiteNode*) remSiteNodeRetCodes[popupRet]->getParent();

            if (pParent->remSiteNode(remSiteNodeRetCodes[popupRet]))
            {
                TYAction* pAction = new TYRemSiteNodeToSiteNodeAction(pGeoNode, (TYSiteNode*) remSiteNodeRetCodes[popupRet]->getParent(), _pModeler, TR("id_action_remsitenode"));
                _pModeler->getActionManager()->addAction(pAction);

                pRemovedEltParent = pParent;
            }

            updateSiteFrame();
            getTYMainWnd()->updateModelers(false);

            // La scene a ete modifiee
            TYElement::setIsSavedOk(true);
        }
    }

    // Deselection
    resetPicker();

    // Si un element a ete supprime
    if (pRemovedEltParent)
    {
        pRemovedEltParent->updateGraphicTree();
    }

    // Si un element a ete ajoute
    if (pAddedElt && _pModeler->isElementInCurrentCalcul())
    {
        getTYApp()->getCurProjet()->getCurrentCalcul()->addToSelection(pAddedElt);
        pAddedElt->updateGraphicTree();
    }

    // Update
    getTYMainWnd()->updateModelers(true, true, bUpdateDisplayList);//xbh++

    delete pPopup;
}

void TYPickEditor::showPositionDialog(TYGeometryNode* pGeoNode)
{
    assert(pGeoNode);

    //On doit presenter la position de l'origine du GeoNode selectionne.
    //Cette position doit etre dans le repere du modeler frame.
    TYElement* pRootTYElement = _pModeler->getElement();
    TYGeometryNode* pRootGeometryNode = TYGeometryNode::GetGeoNode(pRootTYElement);

    // On a besoin du type d'element point par le geonode
    TYElement* pElement = pGeoNode->getElement();

    // On cree un geonode temporaire copie du geonode transmis (uniquement les elements geometrique)
    TYGeometryNode* pTempGeoNode = new TYGeometryNode();
//	pTempGeoNode->deepCopy(pGeoNode, true);
	pTempGeoNode->setRepere(pGeoNode->getRepere()) ;
	pTempGeoNode->setElement(pElement);
 

    TYPoint oldZero(0, 0, 0);
    oldZero = pGeoNode->localToGlobal(oldZero);
    if (pRootGeometryNode)
    {
        oldZero = pRootGeometryNode->globalToLocal(oldZero);//oldZero est maintenant exprime dans le repere pRootGeometryNode

        // Affectation de la nouvelle origine au geonode
        pTempGeoNode->getRepere()._origin = oldZero;
    }

    // Affiche la boite de dialogue
    TYPositionDialog* pDlg = new TYPositionDialog(pTempGeoNode, _pModeler);

    if ((QString(_pModeler->metaObject()->className()).compare("TYBatimentModelerFrame") == 0) &&
        (pElement->isA("TYMachine")))
    {
        pDlg->setHauteurEnabled(true); // Activation de la hauteur pour les elements de volumes nodes
    }
    else if ((QString(_pModeler->metaObject()->className()).compare("TYBatimentModelerFrame") == 0) ||
             (QString(_pModeler->metaObject()->className()).compare("TYMachineModelerFrame") == 0)  ||
             (pElement->isA("TYSiteNode")))
    {
        pDlg->setHauteurEnabled(false); // Desactivation de la hauteur pour les elements de volumes nodes
    }

    int ret = pDlg->exec();

    // Applique les modificatins si necessaire
    if (ret == QDialog::Accepted)
    {
        if (_pModeler->askForResetResultat())
        {
            //On a bouge l'origine (exprime dans le repere root)
            OPoint3D newZero = pTempGeoNode->getRepere()._origin;

            // On passe newZero dans le repere local s'il y a lieu
            if (pRootGeometryNode)
            {
                newZero = pRootGeometryNode->localToGlobal(newZero);
            }

            //exprimons newZero dans le repere du GeoNode:
            newZero = pGeoNode->globalToLocal(newZero);//newZero est maintenant exprime dans le repere global

            //on modifie l'origine du GeoNode de newZero:
            ORepere3D repere = pGeoNode->getRepere();
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
    if (pGeoNode->getElement()->inherits("TYMaillage") || pGeoNode->getElement()->inherits("TYSiteNode"))
    {
        pDlg->lockXY();
    }

    int ret = pDlg->exec();

    // Applique les modifications si necessaire
    if (ret == QDialog::Accepted)
    {
        if (_pModeler->askForResetResultat())
        {
            ORepere3D Repere = pGeoNode->getRepere();
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

            tyMatTmpX.setRotationOx(-M_PI * dRotateX / 180);
            tyMatTmpY.setRotationOy(-M_PI * dRotateY / 180);
            tyMatTmpZ.setRotationOz(M_PI * dRotateZ / 180);

            if (pDlg->getConcatenateStatus()) //az--
            {
                Repere.getMatChangeRep(tyMat);//ne fait que retourner la matrice associe a ce repere
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

    QDialog* pDlg = new QDialog(_pModeler);
    pDlg->setWindowTitle(TR("id_popup_dimension"));

    QGridLayout* pLayout = new QGridLayout();
    pDlg->setLayout(pLayout);

    QBoxLayout* pEditLayout = new QHBoxLayout();
    pEditLayout->setMargin(10);
    pLayout->addLayout(pEditLayout, 0, 1);

    QLineEdit* pXLineEdit = NULL;
    QLineEdit* pYLineEdit = NULL;
    QLineEdit* pZLineEdit = NULL;
    QLineEdit* pDiamLineEdit = NULL;
    QLineEdit* pHauteurLineEdit = NULL;

    if (pAccVol->isA("TYAcousticBox"))
    {
        TYAcousticBox* pAccBox = (TYAcousticBox*) pAccVol;
        float sizeX, sizeY, sizeZ;
        pAccBox->getDimension(sizeX, sizeY, sizeZ);

        // Size X
        QLabel* pXLabelName = new QLabel(pDlg);
        pXLabelName->setText(TR("id_x_label"));
        pEditLayout->addWidget(pXLabelName);
        pXLineEdit = new QLineEdit(pDlg);
        pXLineEdit->setFixedWidth(60);
        pXLineEdit->setText(QString().setNum(sizeX, 'f', 2));
        pEditLayout->addWidget(pXLineEdit);

        // Size Y
        pEditLayout->addSpacing(10);
        QLabel* pYLabelName = new QLabel(pDlg);
        pYLabelName->setText(TR("id_y_label"));
        pEditLayout->addWidget(pYLabelName);
        pYLineEdit = new QLineEdit(pDlg);
        pYLineEdit->setFixedWidth(60);
        pYLineEdit->setText(QString().setNum(sizeY, 'f', 2));
        pEditLayout->addWidget(pYLineEdit);

        // Size Z
        pEditLayout->addSpacing(10);
        QLabel* pZLabelName = new QLabel(pDlg);
        pZLabelName->setText(TR("id_z_label"));
        pEditLayout->addWidget(pZLabelName);
        pZLineEdit = new QLineEdit(pDlg);
        pZLineEdit->setFixedWidth(60);
        pZLineEdit->setText(QString().setNum(sizeZ, 'f', 2));
        pEditLayout->addWidget(pZLineEdit);
    }
    else if (pAccVol->isA("TYAcousticCylinder") || pAccVol->isA("TYAcousticSemiCylinder"))
    {
        // Diametre
        QLabel* pDiamLabelName = new QLabel(pDlg);
        pDiamLabelName->setText(TR("id_diameter_label"));
        pEditLayout->addWidget(pDiamLabelName);
        pDiamLineEdit = new QLineEdit(pDlg);
        pDiamLineEdit->setFixedWidth(60);
        pEditLayout->addWidget(pDiamLineEdit);

        // Hauteur
        pEditLayout->addSpacing(10);
        QLabel* pHauteurLabelName = new QLabel(pDlg);
        pHauteurLabelName->setText(TR("id_hauteur_label"));
        pEditLayout->addWidget(pHauteurLabelName);
        pHauteurLineEdit = new QLineEdit(pDlg);
        pHauteurLineEdit->setFixedWidth(60);
        pEditLayout->addWidget(pHauteurLineEdit);

        if (pAccVol->isA("TYAcousticCylinder"))
        {
            pDiamLineEdit->setText(QString().setNum(((TYAcousticCylinder*) pAccVol)->getDiameter(), 'f', 2));
            pHauteurLineEdit->setText(QString().setNum(((TYAcousticCylinder*) pAccVol)->getHauteur(), 'f', 2));
        }
        else if (pAccVol->isA("TYAcousticSemiCylinder"))
        {
            pDiamLineEdit->setText(QString().setNum(((TYAcousticSemiCylinder*) pAccVol)->getDiameter(), 'f', 2));
            pHauteurLineEdit->setText(QString().setNum(((TYAcousticSemiCylinder*) pAccVol)->getHauteur(), 'f', 2));
        }
    }

    QBoxLayout* pBtnLayout = new QHBoxLayout();
    pLayout->addLayout(pBtnLayout, 1, 1);

    pBtnLayout->addStretch(1);

    QPushButton* pButtonOK = new QPushButton(TR("id_ok_btn"), pDlg);
    pButtonOK->setDefault(true);
    QObject::connect(pButtonOK, SIGNAL(clicked()), pDlg, SLOT(accept()));
    pBtnLayout->addWidget(pButtonOK);

    QPushButton* pButtonCancel = new QPushButton(TR("id_cancel_btn"), pDlg);
    pButtonCancel->setShortcut(Qt::Key_Escape);
    QObject::connect(pButtonCancel, SIGNAL(clicked()), pDlg, SLOT(reject()));
    pBtnLayout->addWidget(pButtonCancel);

    // Affiche la boite de dialogue
    int ret = pDlg->exec();

    // Applique les modificatins si necessaire
    if (ret == QDialog::Accepted)
    {
        if (_pModeler->askForResetResultat())
        {
            if (pAccVol->isA("TYAcousticBox"))
            {
                float sizeX = pXLineEdit->text().toDouble();
                float sizeY = pYLineEdit->text().toDouble();
                float sizeZ = pZLineEdit->text().toDouble();

                TYAction* pAction = new TYResizeAccBoxAction((TYAcousticBox*) pAccVol,
                                                             sizeX, sizeY, sizeZ, _pModeler, TR("id_action_resizebox"));
                _pModeler->getActionManager()->addAction(pAction);

                ((TYAcousticBox*) pAccVol)->setDimension(sizeX, sizeY, sizeZ);
            }
            else if (pAccVol->isA("TYAcousticCylinder"))
            {
                double diameter = pDiamLineEdit->text().toDouble();
                double hauteur = pHauteurLineEdit->text().toDouble();

                TYAction* pAction = new TYResizeAccCylAction((TYAcousticCylinder*) pAccVol,
                                                             diameter, hauteur, _pModeler, TR("id_action_resizecyl"));
                _pModeler->getActionManager()->addAction(pAction);

                ((TYAcousticCylinder*) pAccVol)->setDiameter(diameter);
                ((TYAcousticCylinder*) pAccVol)->setHauteur(hauteur);
            }
            else if (pAccVol->isA("TYAcousticSemiCylinder"))
            {
                double diameter = pDiamLineEdit->text().toDouble();
                double hauteur = pHauteurLineEdit->text().toDouble();

                TYAction* pAction = new TYResizeAccSemiCylAction((TYAcousticSemiCylinder*) pAccVol,
                                                                 diameter, hauteur, _pModeler, TR("id_action_resizesemicyl"));
                _pModeler->getActionManager()->addAction(pAction);

                ((TYAcousticSemiCylinder*) pAccVol)->setDiameter(diameter);
                ((TYAcousticSemiCylinder*) pAccVol)->setHauteur(hauteur);
            }

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

    delete pBtnLayout;
    delete pEditLayout;
}

void TYPickEditor::showPanel(TYElement* pElt)
{
    hidePanel(_pLastRolloverElt);

    if (!pElt)
    {
        return;
    }

    if (pElt->inherits("TYMaillage"))
    {
        double x = _lastMovedCurPos.x();
        double y = _pInteractor->height() - _lastMovedCurPos.y();

        LPTYMaillage pMaillage = TYMaillage::safeDownCast(pElt);
        LPTYPanel pPanel = pMaillage->getPanel();

        // Position du curseur
        TYElementGraphic* pTYElementGraphic = pPanel->getGraphicObject();
        ((TYPanelGraphic*)pTYElementGraphic)->setPosX(x);
        ((TYPanelGraphic*)pTYElementGraphic)->setPosY(y);
        pTYElementGraphic->setVisible();

        // Position dans la scene 3D
        double worldPos[4];
        displayToWorld(x, y, 0.0, worldPos);
        switch (_pModeler->getCurrentView())
        {
            case TYModelerFrame::TopView:
                pPanel->setFirstPos("X : " + doubleToStrPre(worldPos[0], 1));
                pPanel->setSecondPos("Y : " + doubleToStrPre(-worldPos[2], 1));
                break;
            case TYModelerFrame::LeftView:
                pPanel->setFirstPos("Y : " + doubleToStrPre(-worldPos[2], 1));
                pPanel->setSecondPos("Z : " + doubleToStrPre(worldPos[1], 1));
                break;
            case TYModelerFrame::FrontView:
                pPanel->setFirstPos("X : " + doubleToStrPre(worldPos[0], 1));
                pPanel->setSecondPos("Z : " + doubleToStrPre(worldPos[1], 1));
                break;
        }

        // Valeur
        pTYElementGraphic = pMaillage->getGraphicObject();
        LPTYMaillageGeoNode pMaillageGeoNode = TYCalcul::safeDownCast(pMaillage->getParent())->findMaillage(pMaillage);

        // On recupere la premiere distance
        OCoord3D coord;
        double minDistSquare = 0.0;
        LPTYPointCalcul pPtCalcul = pMaillage->getPtsCalcul().front();
        LPTYPointCalcul pResult = 0;
        if (pPtCalcul)
        {
            coord = pMaillageGeoNode->localToGlobal(OCoord3D(pPtCalcul->_x, pPtCalcul->_y, 0.0));
            switch (_pModeler->getCurrentView())
            {
                case TYModelerFrame::TopView:
                    minDistSquare = std::sqrt(std::pow(coord._x - worldPos[0], 2) + std::pow(coord._y + worldPos[2], 2));
                    break;
                case TYModelerFrame::LeftView:
                    minDistSquare = std::sqrt(std::pow(coord._y + worldPos[2], 2) + std::pow(coord._z - worldPos[1], 2));
                    break;
                case TYModelerFrame::FrontView:
                    minDistSquare = std::sqrt(std::pow(coord._x - worldPos[0], 2) + std::pow(coord._z - worldPos[1], 2));
                    break;
            }

            pResult = pPtCalcul;
        }

        // On teste les suivantes pour trouver le point de calcul le plus proche
        for (unsigned int i = 1; i < pMaillage->getPtsCalcul().size(); i++)
        {
            pPtCalcul = pMaillage->getPtsCalcul()[i];
            coord = pMaillageGeoNode->localToGlobal(OCoord3D(pPtCalcul->_x, pPtCalcul->_y, 0.0));

            double distSquare = 0.0;
            switch (_pModeler->getCurrentView())
            {
                case TYModelerFrame::TopView:
                    distSquare = std::sqrt(std::pow(coord._x - worldPos[0], 2) + std::pow(coord._y + worldPos[2], 2));
                    break;
                case TYModelerFrame::LeftView:
                    distSquare = std::sqrt(std::pow(coord._y + worldPos[2], 2) + std::pow(coord._z - worldPos[1], 2));
                    break;
                case TYModelerFrame::FrontView:
                    distSquare = std::sqrt(std::pow(coord._x - worldPos[0], 2) + std::pow(coord._z - worldPos[1], 2));
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

    if (pElt->inherits("TYMaillage"))
    {
        LPTYMaillage pMaillage = TYMaillage::safeDownCast(pElt);
        pMaillage->getPanel()->getGraphicObject()->setVisible(false);

        // On met a jour uniquement le maillage
        _pModeler->getView()->getRenderer()->updateDisplayList();
    }
}

