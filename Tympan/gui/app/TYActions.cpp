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
 * \file TYActions.cpp
 * \brief action qui necessite l'acces au modeler
 */

#include "Tympan/models/business/TYProjet.h"
#include "Tympan/models/business/TYCalcul.h"
#include "Tympan/models/business/geoacoustic/TYAcousticSurfaceNode.h"
#include "Tympan/models/business/infrastructure/TYSiteNode.h"
#include "Tympan/gui/app/TYPickEditor.h"
#include "TYActions.h"


TYAddAccSurfToAccSurfNodeAction::TYAddAccSurfToAccSurfNodeAction(LPTYAcousticSurfaceGeoNode pAccSurfGeoNode,
                                                                 LPTYAcousticSurfaceNode pAccSurfNode,
                                                                 TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pAccSurfGeoNode(pAccSurfGeoNode), _pAccSurfNode(pAccSurfNode)
{
}

TYAddAccSurfToAccSurfNodeAction::TYAddAccSurfToAccSurfNodeAction(LPTYAcousticSurface pAccSurf,
                                                                 LPTYAcousticSurfaceNode pAccSurfNode,
                                                                 TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pAccSurfNode(pAccSurfNode)
{
    _pAccSurfGeoNode = _pAccSurfNode->findAcousticSurf(pAccSurf);
}

TYAddAccSurfToAccSurfNodeAction::~TYAddAccSurfToAccSurfNodeAction()
{
}

void TYAddAccSurfToAccSurfNodeAction::undo()
{
    if (_pAccSurfNode->remAcousticSurf(_pAccSurfGeoNode))
    {
        _pAccSurfNode->updateGraphicTree();
        _pInteractor->updateGL();
    }
}

void TYAddAccSurfToAccSurfNodeAction::redo()
{
    if (_pAccSurfNode->addAcousticSurf(_pAccSurfGeoNode))
    {
        _pAccSurfNode->updateGraphicTree();
        _pInteractor->updateGL();
    }
}


/***************************************************************************/


TYRemAccSurfToAccSurfNodeAction::TYRemAccSurfToAccSurfNodeAction(LPTYAcousticSurfaceGeoNode pAccSurfGeoNode,
                                                                 LPTYAcousticSurfaceNode pAccSurfNode,
                                                                 TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pAccSurfGeoNode(pAccSurfGeoNode), _pAccSurfNode(pAccSurfNode)
{
}

TYRemAccSurfToAccSurfNodeAction::TYRemAccSurfToAccSurfNodeAction(LPTYAcousticSurface pAccSurf,
                                                                 LPTYAcousticSurfaceNode pAccSurfNode,
                                                                 TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pAccSurfNode(pAccSurfNode)
{
    _pAccSurfGeoNode = _pAccSurfNode->findAcousticSurf(pAccSurf);
}

TYRemAccSurfToAccSurfNodeAction::~TYRemAccSurfToAccSurfNodeAction()
{
}

void TYRemAccSurfToAccSurfNodeAction::undo()
{
    if (_pAccSurfNode->addAcousticSurf(_pAccSurfGeoNode))
    {
        _pAccSurfNode->updateGraphicTree();
        _pInteractor->updateGL();
    }
}

void TYRemAccSurfToAccSurfNodeAction::redo()
{
    if (_pAccSurfNode->remAcousticSurf(_pAccSurfGeoNode))
    {
        _pAccSurfNode->updateGraphicTree();
        _pInteractor->updateGL();
    }
}


/***************************************************************************/

TYAddAccVolToAccVolNodeAction::TYAddAccVolToAccVolNodeAction(LPTYAcousticVolumeGeoNode pAccVolGeoNode,
                                                             LPTYAcousticVolumeNode pAccVolNode,
                                                             TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pAccVolGeoNode(pAccVolGeoNode), _pAccVolNode(pAccVolNode)
{
}

TYAddAccVolToAccVolNodeAction::TYAddAccVolToAccVolNodeAction(LPTYAcousticVolume pAccVol,
                                                             LPTYAcousticVolumeNode pAccVolNode,
                                                             TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pAccVolNode(pAccVolNode)
{
    _pAccVolGeoNode = _pAccVolNode->findAcousticVol(pAccVol);
}

TYAddAccVolToAccVolNodeAction::~TYAddAccVolToAccVolNodeAction()
{
}

void TYAddAccVolToAccVolNodeAction::undo()
{
    if (_pAccVolNode->remAcousticVol(_pAccVolGeoNode))
    {
        _pAccVolNode->updateGraphicTree();
        _pInteractor->updateGL();
    }
}

void TYAddAccVolToAccVolNodeAction::redo()
{
    if (_pAccVolNode->addAcousticVol(_pAccVolGeoNode))
    {
        _pAccVolNode->updateGraphicTree();
        _pInteractor->updateGL();
    }
}


/***************************************************************************/


TYRemAccVolToAccVolNodeAction::TYRemAccVolToAccVolNodeAction(LPTYAcousticVolumeGeoNode pAccVolGeoNode,
                                                             LPTYAcousticVolumeNode pAccVolNode,
                                                             TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pAccVolGeoNode(pAccVolGeoNode), _pAccVolNode(pAccVolNode)
{
}

TYRemAccVolToAccVolNodeAction::TYRemAccVolToAccVolNodeAction(LPTYAcousticVolume pAccVol,
                                                             LPTYAcousticVolumeNode pAccVolNode,
                                                             TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pAccVolNode(pAccVolNode)
{
    _pAccVolGeoNode = _pAccVolNode->findAcousticVol(pAccVol);
}

TYRemAccVolToAccVolNodeAction::~TYRemAccVolToAccVolNodeAction()
{
}

void TYRemAccVolToAccVolNodeAction::undo()
{
    if (_pAccVolNode->addAcousticVol(_pAccVolGeoNode))
    {
        _pAccVolNode->updateGraphicTree();
        _pInteractor->updateGL();
    }
}

void TYRemAccVolToAccVolNodeAction::redo()
{
    if (_pAccVolNode->remAcousticVol(_pAccVolGeoNode))
    {
        _pAccVolNode->updateGraphicTree();
        _pInteractor->updateGL();
    }
}


/***************************************************************************/


TYAddMachineToEtageAction::TYAddMachineToEtageAction(LPTYMachineGeoNode pMachineGeoNode, LPTYEtage pEtage,
                                                     TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pMachineGeoNode(pMachineGeoNode), _pEtage(pEtage)
{
}

TYAddMachineToEtageAction::TYAddMachineToEtageAction(LPTYMachine pMachine, LPTYEtage pEtage,
                                                     TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pEtage(pEtage)
{
    _pMachineGeoNode = _pEtage->findMachine(pMachine);
}

TYAddMachineToEtageAction::~TYAddMachineToEtageAction()
{
}

void TYAddMachineToEtageAction::undo()
{
    if (_pEtage->remMachine(_pMachineGeoNode))
    {
        _pEtage->updateGraphicTree();
        _pInteractor->updateGL();
    }
}

void TYAddMachineToEtageAction::redo()
{
    if (_pEtage->addMachine(_pMachineGeoNode))
    {
        _pEtage->updateGraphicTree();
        _pInteractor->updateGL();
    }
}


/***************************************************************************/


TYRemMachineToEtageAction::TYRemMachineToEtageAction(LPTYMachineGeoNode pMachineGeoNode, LPTYEtage pEtage,
                                                     TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pMachineGeoNode(pMachineGeoNode), _pEtage(pEtage)
{
}

TYRemMachineToEtageAction::TYRemMachineToEtageAction(LPTYMachine pMachine, LPTYEtage pEtage,
                                                     TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pEtage(pEtage)
{
}

TYRemMachineToEtageAction::~TYRemMachineToEtageAction()
{
}

void TYRemMachineToEtageAction::undo()
{
    if (_pEtage->addMachine(_pMachineGeoNode))
    {
        _pEtage->updateGraphicTree();
        _pInteractor->updateGL();
    }
}

void TYRemMachineToEtageAction::redo()
{
    if (_pEtage->remMachine(_pMachineGeoNode))
    {
        _pEtage->updateGraphicTree();
        _pInteractor->updateGL();
    }
}







/****************************************************************************/
//DEBUT
TYAddElementToEtageAction::TYAddElementToEtageAction(SmartPtr<TYGeometryNode > pElementGeoNode, LPTYEtage pEtage,
                                                     TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pElementGeoNode(pElementGeoNode), _pEtage(pEtage)
{
    _pSrcPonct = NULL;
}

TYAddElementToEtageAction::TYAddElementToEtageAction(LPTYElement pElement, LPTYEtage pEtage,
                                                     TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pEtage(pEtage)
{
    _pElementGeoNode = NULL;
    _pSrcPonct = NULL;

    if (pElement->isA("TYMachine"))
    {
        _pElementGeoNode =  _pEtage->findMachine((LPTYMachine&) pElement);
    }
    else if (pElement->isA("TYUserSourcePonctuelle"))
    {
        _pSrcPonct = (LPTYUserSourcePonctuelle&) pElement;
    }
}

TYAddElementToEtageAction::~TYAddElementToEtageAction()
{
}

void TYAddElementToEtageAction::undo()
{
    if (_pElementGeoNode)
    {
        if (_pElementGeoNode->getElement()->isA("TYMachine"))
        {
            _pEtage->remMachine((LPTYMachineGeoNode&) _pElementGeoNode);
        }
    }
    else if (_pSrcPonct)
    {
        _pEtage->remSource(_pSrcPonct);
    }

    _pEtage->updateGraphicTree();
    _pInteractor->updateGL();
}

void TYAddElementToEtageAction::redo()
{
    if (_pElementGeoNode)
    {
        if (_pElementGeoNode->getElement()->isA("TYMachine"))
        {
            _pEtage->addMachine((LPTYMachineGeoNode&) _pElementGeoNode);
        }
    }
    else if (_pSrcPonct)
    {
        _pSrcPonct->setIsGeometryModified(true);
        _pEtage->addSource(_pSrcPonct);
    }

    _pEtage->updateGraphicTree();
    _pInteractor->updateGL();
}


/***************************************************************************/


TYRemElementToEtageAction::TYRemElementToEtageAction(SmartPtr<TYGeometryNode > pElementGeoNode, LPTYEtage pEtage,
                                                     TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pElementGeoNode(pElementGeoNode), _pEtage(pEtage)
{
    _pSrcPonct = NULL;
}

TYRemElementToEtageAction::TYRemElementToEtageAction(LPTYElement pElement, LPTYEtage pEtage,
                                                     TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pEtage(pEtage)
{
    _pElementGeoNode = NULL;
    _pSrcPonct = NULL;

    if (pElement->isA("TYMachine"))
    {
        _pElementGeoNode =  _pEtage->findMachine((LPTYMachine&) pElement);
    }
    else if (pElement->isA("TYUserSourcePonctuelle"))
    {
        _pSrcPonct = (LPTYUserSourcePonctuelle&) pElement;
    }
}

TYRemElementToEtageAction::~TYRemElementToEtageAction()
{
}

void TYRemElementToEtageAction::undo()
{
    if (_pElementGeoNode)
    {
        if (_pElementGeoNode->getElement()->isA("TYMachine"))
        {
            _pEtage->addMachine((LPTYMachineGeoNode&) _pElementGeoNode);
        }
    }
    else if (_pSrcPonct)
    {
        _pSrcPonct->setIsGeometryModified(true);
        _pEtage->addSource(_pSrcPonct);
    }

    _pEtage->updateGraphicTree();
    _pInteractor->updateGL();
}

void TYRemElementToEtageAction::redo()
{
    if (_pElementGeoNode)
    {
        if (_pElementGeoNode->getElement()->isA("TYMachine"))
        {
            _pEtage->remMachine((LPTYMachineGeoNode&) _pElementGeoNode);
        }
    }
    else if (_pSrcPonct)
    {
        _pEtage->remSource(_pSrcPonct);
    }

    _pEtage->updateGraphicTree();
    _pInteractor->updateGL();
}

//FIN
/***************************************************************************/























/***************************************************************************/


TYAddElementToTopoAction::TYAddElementToTopoAction(SmartPtr<TYGeometryNode > pElementGeoNode, LPTYTopographie pTopo,
                                                   TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pElementGeoNode(pElementGeoNode), _pTopo(pTopo)
{
    if (_pTopo) { _pTopo->incRef(); }
}

TYAddElementToTopoAction::TYAddElementToTopoAction(LPTYElement pElement, LPTYTopographie pTopo,
                                                   TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pTopo(pTopo)
{
    if (_pTopo) { _pTopo->incRef(); }

    if (pElement->isA("TYCourbeNiveau"))
    {
        _pElementGeoNode =  _pTopo->findCrbNiv((LPTYCourbeNiveau&) pElement);
    }
    else if (pElement->isA("TYTerrain"))
    {
        _pElementGeoNode = _pTopo->findTerrain((LPTYTerrain&) pElement);
    }
    else if (pElement->isA("TYCoursEau"))
    {
        _pElementGeoNode =  _pTopo->findCrsEau((LPTYCoursEau&) pElement);
    }
    else if (pElement->isA("TYPlanEau"))
    {
        _pElementGeoNode =  _pTopo->findPlanEau((LPTYPlanEau&) pElement);
    }
}

TYAddElementToTopoAction::~TYAddElementToTopoAction()
{
}

void TYAddElementToTopoAction::undo()
{
    if (_pElementGeoNode->getElement()->isA("TYCourbeNiveau"))
    {
        _pTopo->remCrbNiv((LPTYCourbeNiveauGeoNode&) _pElementGeoNode);
    }
    else if (_pElementGeoNode->getElement()->isA("TYTerrain"))
    {
        _pTopo->remTerrain((LPTYTerrainGeoNode&) _pElementGeoNode);
    }
    else if (_pElementGeoNode->getElement()->isA("TYCoursEau"))
    {
        _pTopo->remCrsEau((LPTYCoursEauGeoNode&) _pElementGeoNode);
    }
    else if (_pElementGeoNode->getElement()->isA("TYPlanEau"))
    {
        _pTopo->remPlanEau((LPTYPlanEauGeoNode&) _pElementGeoNode);
    }

    _pTopo->updateGraphicTree();
    _pInteractor->updateGL();
}

void TYAddElementToTopoAction::redo()
{
    if (_pElementGeoNode->getElement()->isA("TYCourbeNiveau"))
    {
        _pTopo->addCrbNiv((LPTYCourbeNiveauGeoNode&) _pElementGeoNode);
    }
    else if (_pElementGeoNode->getElement()->isA("TYTerrain"))
    {
        _pTopo->addTerrain((LPTYTerrainGeoNode&) _pElementGeoNode);
    }
    else if (_pElementGeoNode->getElement()->isA("TYCoursEau"))
    {
        _pTopo->addCrsEau((LPTYCoursEauGeoNode&) _pElementGeoNode);
    }
    else if (_pElementGeoNode->getElement()->isA("TYPlanEau"))
    {
        _pTopo->addPlanEau((LPTYPlanEauGeoNode&) _pElementGeoNode);
    }

    _pTopo->updateGraphicTree();
    _pInteractor->updateGL();
}


/***************************************************************************/


TYRemElementToTopoAction::TYRemElementToTopoAction(SmartPtr<TYGeometryNode > pElementGeoNode, LPTYTopographie pTopo,
                                                   TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pElementGeoNode(pElementGeoNode), _pTopo(pTopo)
{
}

TYRemElementToTopoAction::TYRemElementToTopoAction(LPTYElement pElement, LPTYTopographie pTopo,
                                                   TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pTopo(pTopo)
{
    if (pElement->isA("TYCourbeNiveau"))
    {
        _pElementGeoNode =  _pTopo->findCrbNiv((LPTYCourbeNiveau&) pElement);
    }
    else if (pElement->isA("TYTerrain"))
    {
        _pElementGeoNode =  _pTopo->findTerrain((LPTYTerrain&) pElement);
    }
    else if (pElement->isA("TYCoursEau"))
    {
        _pElementGeoNode =  _pTopo->findCrsEau((LPTYCoursEau&) pElement);
    }
    else if (pElement->isA("TYPlanEau"))
    {
        _pElementGeoNode =  _pTopo->findPlanEau((LPTYPlanEau&) pElement);
    }
}

TYRemElementToTopoAction::~TYRemElementToTopoAction()
{
}

void TYRemElementToTopoAction::undo()
{
    if (_pElementGeoNode->getElement()->isA("TYCourbeNiveau"))
    {
        _pTopo->addCrbNiv((LPTYCourbeNiveauGeoNode&) _pElementGeoNode);
    }
    else if (_pElementGeoNode->getElement()->isA("TYTerrain"))
    {
        _pTopo->addTerrain((LPTYTerrainGeoNode&) _pElementGeoNode);
    }
    else if (_pElementGeoNode->getElement()->isA("TYCoursEau"))
    {
        _pTopo->addCrsEau((LPTYCoursEauGeoNode&) _pElementGeoNode);
    }
    else if (_pElementGeoNode->getElement()->isA("TYPlanEau"))
    {
        _pTopo->addPlanEau((LPTYPlanEauGeoNode&) _pElementGeoNode);
    }

    _pTopo->updateGraphicTree();
    _pInteractor->updateGL();
}

void TYRemElementToTopoAction::redo()
{
    if (_pElementGeoNode->getElement()->isA("TYCourbeNiveau"))
    {
        _pTopo->remCrbNiv((LPTYCourbeNiveauGeoNode&) _pElementGeoNode);
    }
    else if (_pElementGeoNode->getElement()->isA("TYTerrain"))
    {
        _pTopo->remTerrain((LPTYTerrainGeoNode&) _pElementGeoNode);
    }
    else if (_pElementGeoNode->getElement()->isA("TYCoursEau"))
    {
        _pTopo->remCrsEau((LPTYCoursEauGeoNode&) _pElementGeoNode);
    }
    else if (_pElementGeoNode->getElement()->isA("TYPlanEau"))
    {
        _pTopo->remPlanEau((LPTYPlanEauGeoNode&) _pElementGeoNode);
    }

    _pTopo->updateGraphicTree();
    _pInteractor->updateGL();
}


/***************************************************************************/


TYAddElementToInfraAction::TYAddElementToInfraAction(SmartPtr<TYGeometryNode > pElementGeoNode, LPTYInfrastructure pInfra,
                                                     TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pElementGeoNode(pElementGeoNode), _pInfra(pInfra)
{
    _pSrcPonct = NULL;
}

TYAddElementToInfraAction::TYAddElementToInfraAction(LPTYElement pElement, LPTYInfrastructure pInfra,
                                                     TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pInfra(pInfra)
{
    _pElementGeoNode = NULL;
    _pSrcPonct = NULL;

    if (pElement->isA("TYRoute"))
    {
        _pElementGeoNode =  _pInfra->findRoute((LPTYRoute&) pElement);
    }
    else if (pElement->isA("TYReseauTransport"))
    {
        _pElementGeoNode =  _pInfra->findResTrans((LPTYReseauTransport&) pElement);
    }
    else if (pElement->isA("TYBatiment"))
    {
        _pElementGeoNode =  _pInfra->findBatiment((LPTYBatiment&) pElement);
    }
    else if (pElement->isA("TYMachine"))
    {
        _pElementGeoNode =  _pInfra->findMachine((LPTYMachine&) pElement);
    }
    else if (pElement->isA("TYUserSourcePonctuelle"))
    {
        _pSrcPonct = (LPTYUserSourcePonctuelle&) pElement;
    }
}

TYAddElementToInfraAction::~TYAddElementToInfraAction()
{
}

void TYAddElementToInfraAction::undo()
{
    if (_pElementGeoNode)
    {
        if (_pElementGeoNode->getElement()->isA("TYRoute"))
        {
            _pInfra->remRoute((LPTYRouteGeoNode&) _pElementGeoNode);
        }
        else if (_pElementGeoNode->getElement()->isA("TYReseauTransport"))
        {
            _pInfra->remResTrans((LPTYReseauTransportGeoNode&) _pElementGeoNode);
        }
        else if (_pElementGeoNode->getElement()->isA("TYBatiment"))
        {
            _pInfra->remBatiment((LPTYBatimentGeoNode&) _pElementGeoNode);
        }
        else if (_pElementGeoNode->getElement()->isA("TYMachine"))
        {
            _pInfra->remMachine((LPTYMachineGeoNode&) _pElementGeoNode);
        }
    }
    else if (_pSrcPonct)
    {
        _pInfra->remSrc(_pSrcPonct);
    }

    _pInfra->updateGraphicTree();
    _pInteractor->updateGL();
}

void TYAddElementToInfraAction::redo()
{
    if (_pElementGeoNode)
    {
        if (_pElementGeoNode->getElement()->isA("TYRoute"))
        {
            _pInfra->addRoute((LPTYRouteGeoNode&) _pElementGeoNode);
        }
        else if (_pElementGeoNode->getElement()->isA("TYReseauTransport"))
        {
            _pInfra->addResTrans((LPTYReseauTransportGeoNode&) _pElementGeoNode);
        }
        else if (_pElementGeoNode->getElement()->isA("TYBatiment"))
        {
            _pInfra->addBatiment((LPTYBatimentGeoNode&) _pElementGeoNode);
        }
        else if (_pElementGeoNode->getElement()->isA("TYMachine"))
        {
            _pInfra->addMachine((LPTYMachineGeoNode&) _pElementGeoNode);
        }
    }
    else if (_pSrcPonct)
    {
        _pSrcPonct->setIsGeometryModified(true);
        _pInfra->addSrc(_pSrcPonct);
    }

    _pInfra->updateGraphicTree();
    _pInteractor->updateGL();
}


/***************************************************************************/


TYRemElementToInfraAction::TYRemElementToInfraAction(SmartPtr<TYGeometryNode > pElementGeoNode, LPTYInfrastructure pInfra,
                                                     TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pElementGeoNode(pElementGeoNode), _pInfra(pInfra)
{
    _pSrcPonct = NULL;
}

TYRemElementToInfraAction::TYRemElementToInfraAction(LPTYElement pElement, LPTYInfrastructure pInfra,
                                                     TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pInfra(pInfra)
{
    _pElementGeoNode = NULL;
    _pSrcPonct = NULL;

    if (pElement->isA("TYRoute"))
    {
        _pElementGeoNode =  _pInfra->findRoute((LPTYRoute&) pElement);
    }
    else if (pElement->isA("TYReseauTransport"))
    {
        _pElementGeoNode =  _pInfra->findResTrans((LPTYReseauTransport&) pElement);
    }
    else if (pElement->isA("TYBatiment"))
    {
        _pElementGeoNode =  _pInfra->findBatiment((LPTYBatiment&) pElement);
    }
    else if (pElement->isA("TYMachine"))
    {
        _pElementGeoNode =  _pInfra->findMachine((LPTYMachine&) pElement);
    }
    else if (pElement->isA("TYUserSourcePonctuelle"))
    {
        _pSrcPonct = (LPTYUserSourcePonctuelle&) pElement;
    }
}

TYRemElementToInfraAction::~TYRemElementToInfraAction()
{
}

void TYRemElementToInfraAction::undo()
{
    if (_pElementGeoNode)
    {
        if (_pElementGeoNode->getElement()->isA("TYRoute"))
        {
            _pInfra->addRoute((LPTYRouteGeoNode&) _pElementGeoNode);
        }
        else if (_pElementGeoNode->getElement()->isA("TYReseauTransport"))
        {
            _pInfra->addResTrans((LPTYReseauTransportGeoNode&) _pElementGeoNode);
        }
        else if (_pElementGeoNode->getElement()->isA("TYBatiment"))
        {
            _pInfra->addBatiment((LPTYBatimentGeoNode&) _pElementGeoNode);
        }
        else if (_pElementGeoNode->getElement()->isA("TYMachine"))
        {
            _pInfra->addMachine((LPTYMachineGeoNode&) _pElementGeoNode);
        }
    }
    else if (_pSrcPonct)
    {
        _pSrcPonct->setIsGeometryModified(true);
        _pInfra->addSrc(_pSrcPonct);
    }

    _pInfra->updateGraphicTree();
    _pInteractor->updateGL();
}

void TYRemElementToInfraAction::redo()
{
    if (_pElementGeoNode)
    {
        if (_pElementGeoNode->getElement()->isA("TYRoute"))
        {
            _pInfra->remRoute((LPTYRouteGeoNode&) _pElementGeoNode);
        }
        else if (_pElementGeoNode->getElement()->isA("TYReseauTransport"))
        {
            _pInfra->remResTrans((LPTYReseauTransportGeoNode&) _pElementGeoNode);
        }
        else if (_pElementGeoNode->getElement()->isA("TYBatiment"))
        {
            _pInfra->remBatiment((LPTYBatimentGeoNode&) _pElementGeoNode);
        }
        else if (_pElementGeoNode->getElement()->isA("TYMachine"))
        {
            _pInfra->remMachine((LPTYMachineGeoNode&) _pElementGeoNode);
        }
    }
    else if (_pSrcPonct)
    {
        _pInfra->remSrc(_pSrcPonct);
    }

    _pInfra->updateGraphicTree();
    _pInteractor->updateGL();
}


/***************************************************************************/


TYResizeRectAction::TYResizeRectAction(LPTYRectangle pRect, const float& newSizeX, const float& newSizeY,
                                       TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pRect(pRect)
{
    _newSize[0] = newSizeX;
    _newSize[1] = newSizeY;

    pRect->getSize(_oldSize[0], _oldSize[1]);
}

TYResizeRectAction::~TYResizeRectAction()
{
}

void TYResizeRectAction::undo()
{
    _pRect->setSize(_oldSize[0], _oldSize[1]);

    _pRect->updateGraphicTree();
    _pInteractor->updateGL();
}

void TYResizeRectAction::redo()
{
    _pRect->setSize(_newSize[0], _newSize[1]);

    _pRect->updateGraphicTree();
    _pInteractor->updateGL();
}


/***************************************************************************/


TYResizeAccBoxAction::TYResizeAccBoxAction(LPTYAcousticBox pBox,
                                           const float& newSizeX, const float& newSizeY, const float& newSizeZ,
                                           TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pBox(pBox)
{
    _newSize[0] = newSizeX;
    _newSize[1] = newSizeY;
    _newSize[2] = newSizeZ;

    pBox->getDimension(_oldSize[0], _oldSize[1], _oldSize[2]);
}

TYResizeAccBoxAction::~TYResizeAccBoxAction()
{
}

void TYResizeAccBoxAction::undo()
{
    _pBox->setDimension(_oldSize[0], _oldSize[1], _oldSize[2]);
    _pBox->updateGraphic();
    _pInteractor->updateGL();
}

void TYResizeAccBoxAction::redo()
{
    _pBox->setDimension(_newSize[0], _newSize[1], _newSize[2]);
    _pBox->updateGraphic();
    _pInteractor->updateGL();
}


/***************************************************************************/


TYResizeAccCylAction::TYResizeAccCylAction(LPTYAcousticCylinder pCyl,
                                           const double& newDiameter, const double& newHauteur,
                                           TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pCyl(pCyl)
{
    _newDiameter = newDiameter;
    _newHauteur = newHauteur;

    _oldDiameter = _pCyl->getDiameter();
    _oldHauteur = _pCyl->getHauteur();
}

TYResizeAccCylAction::~TYResizeAccCylAction()
{
}

void TYResizeAccCylAction::undo()
{
    _pCyl->setDiameter(_oldDiameter);
    _pCyl->setHauteur(_oldHauteur);

    _pCyl->updateGraphic();
    _pInteractor->updateGL();
}

void TYResizeAccCylAction::redo()
{
    _pCyl->setDiameter(_newDiameter);
    _pCyl->setHauteur(_newHauteur);

    _pCyl->updateGraphic();
    _pInteractor->updateGL();
}


/***************************************************************************/


TYResizeAccSemiCylAction::TYResizeAccSemiCylAction(LPTYAcousticSemiCylinder pSemiCyl,
                                                   const double& newDiameter, const double& newHauteur,
                                                   TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pSemiCyl(pSemiCyl)
{
    _newDiameter = newDiameter;
    _newHauteur = newHauteur;

    _oldDiameter = _pSemiCyl->getDiameter();
    _oldHauteur = _pSemiCyl->getHauteur();
}

TYResizeAccSemiCylAction::~TYResizeAccSemiCylAction()
{
}

void TYResizeAccSemiCylAction::undo()
{
    _pSemiCyl->setDiameter(_oldDiameter);
    _pSemiCyl->setHauteur(_oldHauteur);

    _pSemiCyl->updateGraphic();
    _pInteractor->updateGL();
}

void TYResizeAccSemiCylAction::redo()
{
    _pSemiCyl->setDiameter(_newDiameter);
    _pSemiCyl->setHauteur(_newHauteur);

    _pSemiCyl->updateGraphic();
    _pInteractor->updateGL();
}


/***************************************************************************/


TYMoveGeoNodeAction::TYMoveGeoNodeAction(SmartPtr< TYGeometryNode > pGeoNode,
                                         const TYPoint& oldPos, const TYPoint& newPos,
                                         TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pGeoNode(pGeoNode), _oldPos(oldPos), _newPos(newPos)
{
}

TYMoveGeoNodeAction::~TYMoveGeoNodeAction()
{
}

void TYMoveGeoNodeAction::undo()
{
    _pGeoNode->getORepere3D()._origin = _oldPos;

    _pGeoNode->setIsGeometryModified(true);
    _pGeoNode->getElement()->getParent()->setIsGeometryModified(true);
    _pGeoNode->getElement()->getParent()->updateGraphic();
    _pInteractor->updateGL();
}

void TYMoveGeoNodeAction::redo()
{
    _pGeoNode->getORepere3D()._origin = _newPos;

    _pGeoNode->setIsGeometryModified(true);
    _pGeoNode->getElement()->getParent()->setIsGeometryModified(true);
    _pGeoNode->getElement()->getParent()->updateGraphic();
    _pInteractor->updateGL();
}


/***************************************************************************/


TYRotateGeoNodeAction::TYRotateGeoNodeAction(SmartPtr< TYGeometryNode > pGeoNode,
                                             const TYRepere& oldRepere, const TYRepere& newRepere,
                                             TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pGeoNode(pGeoNode), _oldRepere(oldRepere), _newRepere(newRepere)
{
}

TYRotateGeoNodeAction::~TYRotateGeoNodeAction()
{
}

void TYRotateGeoNodeAction::undo()
{
    _pGeoNode->setRepere(_oldRepere);

    _pGeoNode->setIsGeometryModified(true);
    _pGeoNode->getElement()->getParent()->setIsGeometryModified(true);
    _pGeoNode->getElement()->getParent()->updateGraphic();
    _pInteractor->updateGL();
}

void TYRotateGeoNodeAction::redo()
{
    _pGeoNode->setRepere(_newRepere);

    _pGeoNode->setIsGeometryModified(true);
    _pGeoNode->getElement()->getParent()->setIsGeometryModified(true);
    _pGeoNode->getElement()->getParent()->updateGraphic();
    _pInteractor->updateGL();
}

/***************************************************************************/


TYMovePointEtageAction::TYMovePointEtageAction(LPTYEtage pEtage, const int& ptId,
                                               const TYPoint& oldPos, const TYPoint& newPos,
                                               TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pEtage(pEtage), _ptId(ptId), _oldPos(oldPos), _newPos(newPos)
{
}

TYMovePointEtageAction::~TYMovePointEtageAction()
{
}

void TYMovePointEtageAction::undo()
{
    TYTabPoint contour = _pEtage->getContour();
    contour[_ptId] = _oldPos;

    _pEtage->setMurs(contour, _pEtage->getHauteur(), _pEtage->getClosed());

    _pEtage->updateGraphicTree();
    _pInteractor->updateGL();
}

void TYMovePointEtageAction::redo()
{
    TYTabPoint contour = _pEtage->getContour();
    contour[_ptId] = _newPos;

    _pEtage->setMurs(contour, _pEtage->getHauteur(), _pEtage->getClosed());

    _pEtage->updateGraphicTree();
    _pInteractor->updateGL();
}


/***************************************************************************/


TYMoveSrcPonctAction::TYMoveSrcPonctAction(LPTYSourcePonctuelle pSrcPonct,
                                           const TYPoint& oldPos, const TYPoint& newPos,
                                           TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pSrcPonct(pSrcPonct), _oldPos(oldPos), _newPos(newPos)
{
}

TYMoveSrcPonctAction::~TYMoveSrcPonctAction()
{
}

void TYMoveSrcPonctAction::undo()
{
    *_pSrcPonct->getPos() = _oldPos;

    _pSrcPonct->setIsGeometryModified(true);
    _pSrcPonct->updateGraphic();
    _pInteractor->updateGL();
}

void TYMoveSrcPonctAction::redo()
{
    *_pSrcPonct->getPos() = _newPos;

    _pSrcPonct->setIsGeometryModified(true);
    _pSrcPonct->updateGraphic();
    _pInteractor->updateGL();
}


/***************************************************************************/


TYOrienteSrcPonctAction::TYOrienteSrcPonctAction(LPTYSourcePonctuelle pSrcPonct,
                                                 const OVector3D& oldOrient, const OVector3D& newOrient,
                                                 TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pSrcPonct(pSrcPonct), _oldOrient(oldOrient), _newOrient(newOrient)
{
}

TYOrienteSrcPonctAction::~TYOrienteSrcPonctAction()
{
}

void TYOrienteSrcPonctAction::undo()
{
    _pSrcPonct->setOrientation(_oldOrient);

    _pSrcPonct->setIsGeometryModified(true);
    _pSrcPonct->updateGraphic();
    _pInteractor->updateGL();
}

void TYOrienteSrcPonctAction::redo()
{
    _pSrcPonct->setOrientation(_newOrient);

    _pSrcPonct->setIsGeometryModified(true);
    _pSrcPonct->updateGraphic();
    _pInteractor->updateGL();
}


/***************************************************************************/


TYMovePointControlAction::TYMovePointControlAction(LPTYPointControl pPointControl,
                                                   const TYPoint& oldPos, const TYPoint& newPos,
                                                   TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pPointControl(pPointControl), _oldPos(oldPos), _newPos(newPos)
{
}

TYMovePointControlAction::~TYMovePointControlAction()
{
}

void TYMovePointControlAction::undo()
{
    *_pPointControl = _oldPos;

    _pPointControl->setIsGeometryModified(true);
    _pPointControl->updateGraphic();
    _pInteractor->updateGL();
}

void TYMovePointControlAction::redo()
{
    *_pPointControl = _newPos;

    _pPointControl->setIsGeometryModified(true);
    _pPointControl->updateGraphic();
    _pInteractor->updateGL();
}


/***************************************************************************/


TYSetPointRefSiteAction::TYSetPointRefSiteAction(LPTYSiteNode& pSite, const TYPoint& oldPos,
                                                 const TYPoint& newPos, TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pSite(pSite), _oldPos(oldPos), _newPos(newPos)
{
}

TYSetPointRefSiteAction::~TYSetPointRefSiteAction()
{
}

void TYSetPointRefSiteAction::undo()
{
    _pSite->setPosition(_oldPos);

    _pSite->updateGraphic();
    _pInteractor->updateGL();
}

void TYSetPointRefSiteAction::redo()
{
    _pSite->setPosition(_newPos);

    _pSite->updateGraphic();
    _pInteractor->updateGL();
}


/***************************************************************************/


TYSetOrientationSiteAction::TYSetOrientationSiteAction(LPTYSiteNode& pSite, const TYSegment& oldOrient,
                                                       const TYSegment& newOrient, TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pSite(pSite), _oldOrient(oldOrient), _newOrient(newOrient)
{
}

TYSetOrientationSiteAction::~TYSetOrientationSiteAction()
{
}

void TYSetOrientationSiteAction::undo()
{
    _pSite->setOrientation(_oldOrient);

    _pSite->updateGraphic();
    _pInteractor->updateGL();
}

void TYSetOrientationSiteAction::redo()
{
    _pSite->setOrientation(_newOrient);

    _pSite->updateGraphic();
    _pInteractor->updateGL();
}


/***************************************************************************/


TYSetEchelleSiteAction::TYSetEchelleSiteAction(LPTYSiteNode& pSite, float oldEchelle, float newEchelle,
                                               TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pSite(pSite), _oldEchelle(oldEchelle), _newEchelle(newEchelle)
{
}

TYSetEchelleSiteAction::~TYSetEchelleSiteAction()
{
}

void TYSetEchelleSiteAction::undo()
{
    _pSite->setEchelle(_oldEchelle);

    _pSite->updateGraphic();
    _pInteractor->updateGL();
}

void TYSetEchelleSiteAction::redo()
{
    _pSite->setEchelle(_newEchelle);

    _pSite->updateGraphic();
    _pInteractor->updateGL();
}


/***************************************************************************/


TYSetEmpriseTopoAction::TYSetEmpriseTopoAction(LPTYTopographie pTopo, const TYTabPoint& newEmprise,
                                               TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pTopo(pTopo), _newEmprise(newEmprise)
{
    _oldEmprise = pTopo->getEmprise();
}

TYSetEmpriseTopoAction::~TYSetEmpriseTopoAction()
{
}

void TYSetEmpriseTopoAction::undo()
{
    _pTopo->setEmprise(_oldEmprise);

    _pTopo->updateGraphic();
    _pInteractor->updateGL();
}

void TYSetEmpriseTopoAction::redo()
{
    _pTopo->setEmprise(_newEmprise);

    _pTopo->updateGraphic();
    _pInteractor->updateGL();
}


/***************************************************************************/


TYAddPointControlAction::TYAddPointControlAction(LPTYProjet pProjet, LPTYPointControl pPtControl,
                                                 TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pProjet(pProjet), _pPtControl(pPtControl)
{
}

TYAddPointControlAction::~TYAddPointControlAction()
{
}

void TYAddPointControlAction::undo()
{
    if (_pProjet->remPointControl(_pPtControl))
    {
        _pProjet->updateGraphicTree();
        _pInteractor->updateGL();
    }
}

void TYAddPointControlAction::redo()
{
    if (_pProjet->addPointControl(_pPtControl))
    {
        _pProjet->updateGraphicTree();
        _pInteractor->updateGL();
    }
}


/***************************************************************************/


TYRemPointControlAction::TYRemPointControlAction(LPTYProjet pProjet, LPTYPointControl pPtControl,
                                                 TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pProjet(pProjet), _pPtControl(pPtControl)
{
}

TYRemPointControlAction::~TYRemPointControlAction()
{
}

void TYRemPointControlAction::undo()
{
    if (_pProjet->addPointControl(_pPtControl))
    {
        _pProjet->updateGraphicTree();
        _pInteractor->updateGL();
    }
}

void TYRemPointControlAction::redo()
{
    if (_pProjet->remPointControl(_pPtControl))
    {
        _pProjet->updateGraphicTree();
        _pInteractor->updateGL();
    }
}


/***************************************************************************/


TYAddMaillageToProjetAction::TYAddMaillageToProjetAction(   LPTYMaillageGeoNode pMaillageGeoNode, LPTYProjet pProjet,
                                                            TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pProjet(pProjet), _pMaillageGeoNode(pMaillageGeoNode)
{
}

TYAddMaillageToProjetAction::TYAddMaillageToProjetAction(   LPTYMaillage pMaillage, LPTYProjet pProjet,
                                                            TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pProjet(pProjet)
{
    _pMaillageGeoNode = pProjet->findMaillage(pMaillage);
}

TYAddMaillageToProjetAction::~TYAddMaillageToProjetAction()
{
}

void TYAddMaillageToProjetAction::undo()
{
    if (_pProjet->remMaillage(_pMaillageGeoNode))
    {
        _pProjet->updateGraphicTree();
        _pInteractor->updateGL();
    }
}

void TYAddMaillageToProjetAction::redo()
{
    if (_pProjet->addMaillage(_pMaillageGeoNode))
    {
        _pProjet->updateGraphicTree();
        _pInteractor->updateGL();
    }
}


/***************************************************************************/


TYRemMaillageToProjetAction::TYRemMaillageToProjetAction(LPTYMaillageGeoNode pMaillageGeoNode,
                                                         LPTYProjet pProjet, TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pProjet(pProjet), _pMaillageGeoNode(pMaillageGeoNode)
{
}

TYRemMaillageToProjetAction::TYRemMaillageToProjetAction(LPTYMaillage pMaillage,
                                                         LPTYProjet pProjet, TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pProjet(pProjet)
{
    _pMaillageGeoNode = _pProjet->findMaillage(pMaillage);
}

TYRemMaillageToProjetAction::~TYRemMaillageToProjetAction()
{
}

void TYRemMaillageToProjetAction::undo()
{
    if (_pProjet->addMaillage(_pMaillageGeoNode))
    {
        _pProjet->updateGraphicTree();
        _pInteractor->updateGL();
    }
}

void TYRemMaillageToProjetAction::redo()
{
    if (_pProjet->remMaillage(_pMaillageGeoNode))
    {
        _pProjet->updateGraphicTree();
        _pInteractor->updateGL();
    }
}


/***************************************************************************/


TYAddSiteNodeToSiteNodeAction::TYAddSiteNodeToSiteNodeAction(LPTYSiteNodeGeoNode pSiteNodeGeoNode,
                                                             LPTYSiteNode pSiteNodeParent, TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pSiteNodeParent(pSiteNodeParent), _pSiteNodeGeoNode(pSiteNodeGeoNode)
{
}

TYAddSiteNodeToSiteNodeAction::TYAddSiteNodeToSiteNodeAction(LPTYSiteNode pSiteNode,
                                                             LPTYSiteNode pSiteNodeParent, TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pSiteNodeParent(pSiteNodeParent)
{
    _pSiteNodeGeoNode = _pSiteNodeParent->findSiteNode(pSiteNode);
}

TYAddSiteNodeToSiteNodeAction::~TYAddSiteNodeToSiteNodeAction()
{
}

void TYAddSiteNodeToSiteNodeAction::undo()
{
    if (_pSiteNodeParent->remSiteNode(_pSiteNodeGeoNode))
    {
        _pSiteNodeParent->updateGraphicTree();
        _pInteractor->updateGL();
    }
}

void TYAddSiteNodeToSiteNodeAction::redo()
{
    if (_pSiteNodeParent->addSiteNode(_pSiteNodeGeoNode))
    {
        _pSiteNodeParent->updateGraphicTree();
        _pInteractor->updateGL();
    }
}


/***************************************************************************/


TYRemSiteNodeToSiteNodeAction::TYRemSiteNodeToSiteNodeAction(LPTYSiteNodeGeoNode pSiteNodeGeoNode,
                                                             LPTYSiteNode pSiteNodeParent, TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pSiteNodeParent(pSiteNodeParent), _pSiteNodeGeoNode(pSiteNodeGeoNode)
{
}

TYRemSiteNodeToSiteNodeAction::TYRemSiteNodeToSiteNodeAction(LPTYSiteNode pSiteNode,
                                                             LPTYSiteNode pSiteNodeParent, TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pSiteNodeParent(pSiteNodeParent)
{
    _pSiteNodeGeoNode = _pSiteNodeParent->findSiteNode(pSiteNode);
}

TYRemSiteNodeToSiteNodeAction::~TYRemSiteNodeToSiteNodeAction()
{
}

void TYRemSiteNodeToSiteNodeAction::undo()
{
    if (_pSiteNodeParent->addSiteNode(_pSiteNodeGeoNode))
    {
        _pSiteNodeParent->updateGraphicTree();
        _pInteractor->updateGL();
    }
}

void TYRemSiteNodeToSiteNodeAction::redo()
{
    if (_pSiteNodeParent->remSiteNode(_pSiteNodeGeoNode))
    {
        _pSiteNodeParent->updateGraphicTree();
        _pInteractor->updateGL();
    }
}

/***************************************************************************/


TYMovePointPolyLineAction::TYMovePointPolyLineAction(LPTYElement pElt, TYTabPoint& polyLine, int ptId,
                                                     const TYPoint& oldPos, const TYPoint& newPos,
                                                     TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pElt(pElt), _polyLine(polyLine), _ptId(ptId), _oldPos(oldPos), _newPos(newPos)
{
}

TYMovePointPolyLineAction::~TYMovePointPolyLineAction()
{
}

void TYMovePointPolyLineAction::undo()
{
    _polyLine[_ptId] = _oldPos;

    _pElt->setIsGeometryModified(true);
    _pElt->updateGraphic();

    _pModeler->getPickEditor()->resetPicker();
}

void TYMovePointPolyLineAction::redo()
{
    _polyLine[_ptId] = _newPos;

    _pElt->setIsGeometryModified(true);
    _pElt->updateGraphic();

    _pModeler->getPickEditor()->resetPicker();
}


/***************************************************************************/


TYInsertPointPolyLineAction::TYInsertPointPolyLineAction(LPTYElement pElt, TYTabPoint& polyLine, int ptId,
                                                         const TYPoint& pt, TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pElt(pElt), _polyLine(polyLine), _ptId(ptId), _pt(pt)
{
}

TYInsertPointPolyLineAction::~TYInsertPointPolyLineAction()
{
}

void TYInsertPointPolyLineAction::undo()
{
    _polyLine.erase(_polyLine.begin() + _ptId);

    _pElt->setIsGeometryModified(true);
    _pElt->updateGraphic();
    _pModeler->getPickEditor()->resetPicker();
}

void TYInsertPointPolyLineAction::redo()
{
    _polyLine.insert(_polyLine.begin() + _ptId, _pt);

    _pElt->setIsGeometryModified(true);
    _pElt->updateGraphic();
    _pModeler->getPickEditor()->resetPicker();
}

/***************************************************************************/


TYRemovePointPolyLineAction::TYRemovePointPolyLineAction(LPTYElement pElt, TYTabPoint& polyLine, int ptId,
                                                         const TYPoint& pt, TYModelerFrame* pModeler, const QString& actionName)
    : TYModelerAction(pModeler, actionName), _pElt(pElt), _polyLine(polyLine), _ptId(ptId), _pt(pt)
{
}

TYRemovePointPolyLineAction::~TYRemovePointPolyLineAction()
{
}

void TYRemovePointPolyLineAction::undo()
{
    _polyLine.insert(_polyLine.begin() + _ptId, _pt);

    _pElt->setIsGeometryModified(true);
    _pElt->updateGraphic();
    _pModeler->getPickEditor()->resetPicker();
}

void TYRemovePointPolyLineAction::redo()
{
    _polyLine.erase(_polyLine.begin() + _ptId);

    _pElt->setIsGeometryModified(true);
    _pElt->updateGraphic();
    _pModeler->getPickEditor()->resetPicker();
}

