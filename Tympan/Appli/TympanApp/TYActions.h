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
 * \file TYActions.h
 * \brief fichier contenant differents types d'actions (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_ACTIONS__
#define __TY_ACTIONS__

#include "TYAppDefines.h"
#include "TYAction.h"
#include "TYModelerFrame.h"

/**
 * \class TYModelerAction
 * \brief Classe pour les actions qui necessite l'acces au modeler.
 */
class TYModelerAction : public TYAction
{
public:
    /**
     * \fn TYModelerAction(TYModelerFrame * pModeler, const QString& actionName)
     * \brief Constructeur.
     */
    TYModelerAction(TYModelerFrame* pModeler, const QString& actionName)
        : TYAction(actionName), _pModeler(pModeler)
    {
        _pInteractor = _pModeler->getView();
    }

    /**
     * \fn virtual ~TYModelerAction()
     * \brief Destructeur.
     */
    virtual ~TYModelerAction() {}

    virtual void undo() = 0;
    virtual void redo() = 0;

protected:
    TYRenderWindowInteractor* _pInteractor;
    TYModelerFrame* _pModeler;
};


/**
 * \class TYAddAccSurfToAccSurfNodeAction
 * \brief ajout d'une surface acoustique a un ensemble de surfaces acoustiques
 */
class TYAddAccSurfToAccSurfNodeAction : public TYModelerAction
{
public:
    /**
     * \fn TYAddAccSurfToAccSurfNodeAction(LPTYAcousticSurfaceGeoNode pAccSurfGeoNode,
            LPTYAcousticSurfaceNode pAccSurfNode,
            TYModelerFrame * pModeler, const QString& actionName)
     * \brief Constructeur.
     */
    TYAddAccSurfToAccSurfNodeAction(LPTYAcousticSurfaceGeoNode pAccSurfGeoNode,
                                    LPTYAcousticSurfaceNode pAccSurfNode,
                                    TYModelerFrame* pModeler, const QString& actionName);
    /**
     * \fn TYAddAccSurfToAccSurfNodeAction(LPTYAcousticSurface pAccSurf,
            LPTYAcousticSurfaceNode pAccSurfNode,
            TYModelerFrame * pModeler, const QString& actionName)
     * \brief Constructeur.
     */
    TYAddAccSurfToAccSurfNodeAction(LPTYAcousticSurface pAccSurf,
                                    LPTYAcousticSurfaceNode pAccSurfNode,
                                    TYModelerFrame* pModeler, const QString& actionName);
    /**
     * \fn virtual ~TYAddAccSurfToAccSurfNodeAction()
     * \brief Destructeur.
     */
    virtual ~TYAddAccSurfToAccSurfNodeAction();

    virtual void undo();
    virtual void redo();

protected:
    LPTYAcousticSurfaceGeoNode _pAccSurfGeoNode;
    LPTYAcousticSurfaceNode _pAccSurfNode;
};


/**
 * \class TYRemAccSurfToAccSurfNodeAction
 * \brief suppression d'une surface acoustique a un ensemble de surfaces acoustiques
 */
class TYRemAccSurfToAccSurfNodeAction : public TYModelerAction
{
public:
    /**
     * \fn TYRemAccSurfToAccSurfNodeAction(LPTYAcousticSurfaceGeoNode pAccSurfGeoNode,
            LPTYAcousticSurfaceNode pAccSurfNode,
            TYModelerFrame * pModeler, const QString& actionName)
     * \brief Constructeur.
     */
    TYRemAccSurfToAccSurfNodeAction(LPTYAcousticSurfaceGeoNode pAccSurfGeoNode,
                                    LPTYAcousticSurfaceNode pAccSurfNode,
                                    TYModelerFrame* pModeler, const QString& actionName);
    /**
     * \fn TYRemAccSurfToAccSurfNodeAction(LPTYAcousticSurface pAccSurf,
            LPTYAcousticSurfaceNode pAccSurfNode,
            TYModelerFrame * pModeler, const QString& actionName)
     * \brief Constructeur.
     */
    TYRemAccSurfToAccSurfNodeAction(LPTYAcousticSurface pAccSurf,
                                    LPTYAcousticSurfaceNode pAccSurfNode,
                                    TYModelerFrame* pModeler, const QString& actionName);
    /**
     * \fn virtual ~TYRemAccSurfToAccSurfNodeAction()
     * \brief Destructeur.
     */
    virtual ~TYRemAccSurfToAccSurfNodeAction();

    virtual void undo();
    virtual void redo();

protected:
    LPTYAcousticSurfaceGeoNode _pAccSurfGeoNode;
    LPTYAcousticSurfaceNode _pAccSurfNode;
};


/**
 * \class TYAddAccVolToAccVolNodeAction
 * \brief ajout d'un volume acoustique a un ensemble de volumes acoustiques
 */
class TYAddAccVolToAccVolNodeAction : public TYModelerAction
{
public:
    /**
     * Constructeur.
     */
    TYAddAccVolToAccVolNodeAction(LPTYAcousticVolumeGeoNode pAccVolGeoNode,
                                  LPTYAcousticVolumeNode pAccVolNode,
                                  TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Constructeur.
     */
    TYAddAccVolToAccVolNodeAction(LPTYAcousticVolume pAccVol,
                                  LPTYAcousticVolumeNode pAccVolNode,
                                  TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Destructeur.
     */
    virtual ~TYAddAccVolToAccVolNodeAction();

    virtual void undo();
    virtual void redo();

protected:
    LPTYAcousticVolumeGeoNode _pAccVolGeoNode;
    LPTYAcousticVolumeNode _pAccVolNode;
};


/**
 * \class TYRemAccVolToAccVolNodeAction
 * \brief suppression d'un volume acoustique a un ensemble de volumes acoustiques
 */
class TYRemAccVolToAccVolNodeAction : public TYModelerAction
{
public:
    /**
     * Constructeur.
     */
    TYRemAccVolToAccVolNodeAction(LPTYAcousticVolumeGeoNode pAccVolGeoNode,
                                  LPTYAcousticVolumeNode pAccVolNode,
                                  TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Constructeur.
     */
    TYRemAccVolToAccVolNodeAction(LPTYAcousticVolume pAccVol,
                                  LPTYAcousticVolumeNode pAccVolNode,
                                  TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Destructeur.
     */
    virtual ~TYRemAccVolToAccVolNodeAction();

    virtual void undo();
    virtual void redo();

protected:
    LPTYAcousticVolumeGeoNode _pAccVolGeoNode;
    LPTYAcousticVolumeNode _pAccVolNode;
};


/**
 * \class TYAddMachineToEtageAction
 * \brief ajout d'une machine a un etage
 */
class TYAddMachineToEtageAction : public TYModelerAction
{
public:
    /**
     * Constructeur.
     */
    TYAddMachineToEtageAction(LPTYMachineGeoNode pMachineGeoNode, LPTYEtage pEtage,
                              TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Constructeur.
     */
    TYAddMachineToEtageAction(LPTYMachine pMachine, LPTYEtage pEtage,
                              TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Destructeur.
     */
    virtual ~TYAddMachineToEtageAction();

    virtual void undo();
    virtual void redo();

protected:
    LPTYMachineGeoNode _pMachineGeoNode;
    LPTYEtage _pEtage;
};


/**
 * \class TYRemMachineToEtageAction
 * \brief suppression d'une machine a un etage
 */
class TYRemMachineToEtageAction : public TYModelerAction
{
public:
    /**
     * Constructeur.
     */
    TYRemMachineToEtageAction(LPTYMachineGeoNode pMachineGeoNode,
                              LPTYEtage pEtage,
                              TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Constructeur.
     */
    TYRemMachineToEtageAction(LPTYMachine pMachine, LPTYEtage pEtage,
                              TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Destructeur.
     */
    virtual ~TYRemMachineToEtageAction();

    virtual void undo();
    virtual void redo();

protected:
    LPTYMachineGeoNode _pMachineGeoNode;
    LPTYEtage _pEtage;
};

// BEGIN
/**
 * \class TYAddElementToEtageAction
 * \brief ajout d'un element a un etage
 * \author Projet_Tympan.
 */
class TYAddElementToEtageAction : public TYModelerAction
{
public:
    /**
     * Constructeur.
     */
    TYAddElementToEtageAction(SmartPtr<TYGeometryNode > pElementGeoNode, LPTYEtage pEtage,
                              TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Constructeur.
     */
    TYAddElementToEtageAction(LPTYElement pElement, LPTYEtage pEtage,
                              TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Destructeur.
     */
    virtual ~TYAddElementToEtageAction();

    virtual void undo();
    virtual void redo();

protected:
    SmartPtr<TYGeometryNode > _pElementGeoNode;
    LPTYUserSourcePonctuelle _pSrcPonct;
    LPTYEtage _pEtage;
};


/**
 * \class TYRemElementToEtageAction
 * \brief suppression d'un element a un etage
 * \author Projet_Tympan.
 */
class TYRemElementToEtageAction : public TYModelerAction
{
public:
    /**
     * Constructeur.
     */
    TYRemElementToEtageAction(SmartPtr<TYGeometryNode > pElementGeoNode, LPTYEtage pEtage,
                              TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Constructeur.
     */
    TYRemElementToEtageAction(LPTYElement pElement, LPTYEtage pEtage,
                              TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Destructeur.
     */
    virtual ~TYRemElementToEtageAction();

    virtual void undo();
    virtual void redo();

protected:
    SmartPtr<TYGeometryNode > _pElementGeoNode;
    LPTYUserSourcePonctuelle _pSrcPonct;
    LPTYEtage _pEtage;
};

//END

/**
 * \class TYAddElementToTopoAction
 * \brief Ajout d'un element a la topographie
 * \author Projet_Tympan.
 */
class TYAddElementToTopoAction : public TYModelerAction
{
public:
    /**
     * Constructeur.
     */
    TYAddElementToTopoAction(SmartPtr<TYGeometryNode > pElementGeoNode, LPTYTopographie pTopo,
                             TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Constructeur.
     */
    TYAddElementToTopoAction(LPTYElement pElement, LPTYTopographie pTopo,
                             TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Destructeur.
     */
    virtual ~TYAddElementToTopoAction();

    virtual void undo();
    virtual void redo();

protected:
    SmartPtr<TYGeometryNode > _pElementGeoNode;
    LPTYTopographie _pTopo;
};

/**
 * \class TYRemElementToTopoAction
 * \brief Suppression d'un element a la topographie
 * \author Projet_Tympan.
 */
class TYRemElementToTopoAction : public TYModelerAction
{
public:
    /**
     * Constructeur.
     */
    TYRemElementToTopoAction(SmartPtr<TYGeometryNode > pElementGeoNode, LPTYTopographie pTopo,
                             TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Constructeur.
     */
    TYRemElementToTopoAction(LPTYElement pElement, LPTYTopographie pTopo,
                             TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Destructeur.
     */
    virtual ~TYRemElementToTopoAction();

    virtual void undo();
    virtual void redo();

protected:
    SmartPtr<TYGeometryNode > _pElementGeoNode;
    LPTYTopographie _pTopo;
};


/**
 * \class TYAddElementToInfraAction
 * \brief Ajout d'un element a une infrastructure
 * \author Projet_Tympan.
 */
class TYAddElementToInfraAction : public TYModelerAction
{
public:
    /**
     * Constructeur.
     */
    TYAddElementToInfraAction(SmartPtr<TYGeometryNode > pElementGeoNode, LPTYInfrastructure pInfra,
                              TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Constructeur.
     */
    TYAddElementToInfraAction(LPTYElement pElement, LPTYInfrastructure pInfra,
                              TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Destructeur.
     */
    virtual ~TYAddElementToInfraAction();

    virtual void undo();
    virtual void redo();

protected:
    SmartPtr<TYGeometryNode > _pElementGeoNode;
    LPTYUserSourcePonctuelle _pSrcPonct;
    LPTYInfrastructure _pInfra;
};

/**
 * \class TYRemElementToInfraAction
 * \brief Suppression d'un element a une infrastructure
 * \author Projet_Tympan.
 */
class TYRemElementToInfraAction : public TYModelerAction
{
public:
    /**
     * Constructeur.
     */
    TYRemElementToInfraAction(SmartPtr<TYGeometryNode > pElementGeoNode, LPTYInfrastructure pInfra,
                              TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Constructeur.
     */
    TYRemElementToInfraAction(LPTYElement pElement, LPTYInfrastructure pInfra,
                              TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Destructeur.
     */
    virtual ~TYRemElementToInfraAction();

    virtual void undo();
    virtual void redo();

protected:
    SmartPtr<TYGeometryNode > _pElementGeoNode;
    LPTYUserSourcePonctuelle _pSrcPonct;
    LPTYInfrastructure _pInfra;
};


/**
 * \class TYResizeRectAction
 * \brief Action de redimensionnement d'un rectangle
 * \author Projet_Tympan.
 */
class TYResizeRectAction : public TYModelerAction
{
public:
    /**
     * Constructeur.
     */
    TYResizeRectAction(LPTYRectangle pRect, const float& newSizeX, const float& newSizeY,
                       TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Destructeur.
     */
    virtual ~TYResizeRectAction();

    virtual void undo();
    virtual void redo();

protected:
    LPTYRectangle _pRect;
    float _oldSize[2];
    float _newSize[2];
};


/**
 * \class TYResizeAccBoxAction
 * \brief Action de redimensionnement d'un boite acoustique
 * \author Projet_Tympan
 */
class TYResizeAccBoxAction : public TYModelerAction
{
public:
    /**
     * Constructeur.
     */
    TYResizeAccBoxAction(LPTYAcousticBox pBox,
                         const float& newSizeX, const float& newSizeY, const float& newSizeZ,
                         TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Destructeur.
     */
    virtual ~TYResizeAccBoxAction();

    virtual void undo();
    virtual void redo();

protected:
    LPTYAcousticBox _pBox;
    float _oldSize[3];
    float _newSize[3];
};

/**
 * \class TYResizeAccCylAction
 * \brief Action de redimensionnement d'un cylindre acoustique
 * \author Projet_Tympan
 */
class TYResizeAccCylAction : public TYModelerAction
{
public:
    /**
     * Constructeur.
     */
    TYResizeAccCylAction(LPTYAcousticCylinder pCyl,
                         const double& newDiameter, const double& newHauteur,
                         TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Destructeur.
     */
    virtual ~TYResizeAccCylAction();

    virtual void undo();
    virtual void redo();

protected:
    LPTYAcousticCylinder _pCyl;
    double _oldDiameter;
    double _oldHauteur;
    double _newDiameter;
    double _newHauteur;
};

/**
 * \class TYResizeAccSemiCylAction
 * \brief Action de redimensionnement d'un 1/2 cylindre acoustique
 * \author Projet_Tympan
 */

class TYResizeAccSemiCylAction : public TYModelerAction
{
public:
    /**
     * Constructeur.
     */
    TYResizeAccSemiCylAction(LPTYAcousticSemiCylinder pSemiCyl,
                             const double& newDiameter, const double& newHauteur,
                             TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Destructeur.
     */
    virtual ~TYResizeAccSemiCylAction();

    virtual void undo();
    virtual void redo();

protected:
    LPTYAcousticSemiCylinder _pSemiCyl;
    double _oldDiameter;
    double _oldHauteur;
    double _newDiameter;
    double _newHauteur;
};

/**
 * \class TYMoveGeoNodeAction
 * \brief Actions de deplacement d'un GeometryNode
 * \author Projet_Tympan
 */
class TYMoveGeoNodeAction : public TYModelerAction
{
public:
    /**
     * Constructeur.
     */
    TYMoveGeoNodeAction(SmartPtr< TYGeometryNode > pGeoNode,
                        const TYPoint& oldPos, const TYPoint& newPos,
                        TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Destructeur.
     */
    virtual ~TYMoveGeoNodeAction();

    void setNewPos(const TYPoint& newPos) { _newPos = newPos; }

    virtual void undo();
    virtual void redo();

protected:
    SmartPtr< TYGeometryNode > _pGeoNode;
    TYPoint _oldPos;
    TYPoint _newPos;
};


/**
 * \class TYRotateGeoNodeAction
 * \brief Actions de rotation d'un GeometryNode
 * \author Projet_Tympan
 */
class TYRotateGeoNodeAction : public TYModelerAction
{
public:
    /**
     * Constructeur.
     */
    TYRotateGeoNodeAction(SmartPtr< TYGeometryNode > pGeoNode,
                          const TYRepere& oldRepere, const TYRepere& newRepere,
                          TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Destructeur.
     */
    virtual ~TYRotateGeoNodeAction();

    void setNewRepere(const TYRepere& newRepere) { _newRepere = newRepere; }

    virtual void undo();
    virtual void redo();

protected:
    SmartPtr< TYGeometryNode > _pGeoNode;
    TYRepere _oldRepere;
    TYRepere _newRepere;
};

/**
 * \class TYMovePointEtageAction
 * \brief Action de deplacement d'un point d'un etage
 * @author Projet_Tympan
 */
class TYMovePointEtageAction : public TYModelerAction
{
public:
    /**
     * Constructeur.
     */
    TYMovePointEtageAction(LPTYEtage pEtage, const int& ptId,
                           const TYPoint& oldPos, const TYPoint& newPos,
                           TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Destructeur.
     */
    virtual ~TYMovePointEtageAction();

    void setNewPos(const TYPoint& newPos) { _newPos = newPos; }

    virtual void undo();
    virtual void redo();

protected:
    LPTYEtage _pEtage;
    int _ptId;
    TYPoint _oldPos;
    TYPoint _newPos;
};

/**
 * \class TYMoveSrcPonctAction
 * \brief Action de deplacement d'un source ponctuelle
 * \author Projet_Tympan
 */
class TYMoveSrcPonctAction : public TYModelerAction
{
public:
    /**
     * Constructeur.
     */
    TYMoveSrcPonctAction(LPTYSourcePonctuelle pSrcPonct,
                         const TYPoint& oldPos, const TYPoint& newPos,
                         TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Destructeur.
     */
    virtual ~TYMoveSrcPonctAction();

    void setNewPos(const TYPoint& newPos) { _newPos = newPos; }

    virtual void undo();
    virtual void redo();

protected:
    LPTYSourcePonctuelle _pSrcPonct;
    TYPoint _oldPos;
    TYPoint _newPos;
};

/**
 * \class TYOrienteSrcPonctAction
 * \brief Action d'orientation d'une source ponctuelle
 * \author Projet_Tympan
 */
class TYOrienteSrcPonctAction : public TYModelerAction
{
public:
    /**
     * Constructeur.
     */
    TYOrienteSrcPonctAction(LPTYSourcePonctuelle pSrcPonct,
                            const OVector3D& oldOrient, const OVector3D& newOrient,
                            TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Destructeur.
     */
    virtual ~TYOrienteSrcPonctAction();

    void setNewOrient(const OVector3D& newOrient) { _newOrient = newOrient; }

    virtual void undo();
    virtual void redo();

protected:
    LPTYSourcePonctuelle _pSrcPonct;
    OVector3D _oldOrient;
    OVector3D _newOrient;
};


/**
 * \class TYMovePointControlAction
 * \brief Action de deplacement d'un point de controle
 * \author Projet_Tympan
 */
class TYMovePointControlAction : public TYModelerAction
{
public:
    /**
     * Constructeur.
     */
    TYMovePointControlAction(LPTYPointControl pPointControl,
                             const TYPoint& oldPos, const TYPoint& newPos,
                             TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Destructeur.
     */
    virtual ~TYMovePointControlAction();

    void setNewPos(const TYPoint& newPos) { _newPos = newPos; }

    virtual void undo();
    virtual void redo();

protected:
    LPTYPointControl _pPointControl;
    TYPoint _oldPos;
    TYPoint _newPos;
};

/**
 * \class TYSetPointRefSiteAction
 * \brief Action de positionnement d'un point de reference du site par rapport a une image de fond
 * \author Projet_Tympan
 */
class TYSetPointRefSiteAction : public TYModelerAction
{
public:
    /**
     * Constructeur.
     */
    TYSetPointRefSiteAction(LPTYSiteNode& pSite, const TYPoint& oldPos, const TYPoint& newPos,
                            TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Destructeur.
     */
    virtual ~TYSetPointRefSiteAction();

    virtual void undo();
    virtual void redo();

protected:
    LPTYSiteNode _pSite;
    TYPoint _oldPos;
    TYPoint _newPos;
};


/**
 * \class TYSetOrientationSiteAction
 * \brief Action d'orientation du site
 * \author Projet_Tympan.
 */
class TYSetOrientationSiteAction : public TYModelerAction
{
public:
    /**
     * Constructeur.
     */
    TYSetOrientationSiteAction(LPTYSiteNode& pSite, const TYSegment& oldOrient, const TYSegment& newOrient,
                               TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Destructeur.
     */
    virtual ~TYSetOrientationSiteAction();

    virtual void undo();
    virtual void redo();

protected:
    LPTYSiteNode _pSite;
    TYSegment _oldOrient;
    TYSegment _newOrient;
};


/**
 * \class TYSetEchelleSiteAction
 * \brief Action de mise a l'echelle d'un site
 * \author Projet_Tympan
 */
class TYSetEchelleSiteAction : public TYModelerAction
{
public:
    /**
     * Constructeur.
     */
    TYSetEchelleSiteAction(LPTYSiteNode& pSite, float oldEchelle, float newEchelle,
                           TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Destructeur.
     */
    virtual ~TYSetEchelleSiteAction();

    virtual void undo();
    virtual void redo();

protected:
    LPTYSiteNode _pSite;
    float _oldEchelle;
    float _newEchelle;
};


/**
 * \class TYSetEmpriseTopoAction
 * \brief Action de positionnement de l'emprise de la topographie
 * \author Projet_Tympan
 */
class TYSetEmpriseTopoAction : public TYModelerAction
{
public:
    /**
     * Constructeur.
     */
    TYSetEmpriseTopoAction(LPTYTopographie pTopo, const TYTabPoint& newEmprise,
                           TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Destructeur.
     */
    virtual ~TYSetEmpriseTopoAction();

    virtual void undo();
    virtual void redo();

protected:
    LPTYTopographie _pTopo;
    TYTabPoint _oldEmprise;
    TYTabPoint _newEmprise;
};


/**
 * \class TYAddPointControlAction
 * \brief Action d'ajout d'un point de controle
 * \author Projet_Tympan
 */
class TYAddPointControlAction : public TYModelerAction
{
public:
    /**
     * Constructeur.
     */
    TYAddPointControlAction(LPTYProjet pProjet, LPTYPointControl pPtControl,
                            TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Destructeur.
     */
    virtual ~TYAddPointControlAction();

    virtual void undo();
    virtual void redo();

protected:
    LPTYProjet _pProjet;
    LPTYPointControl _pPtControl;
};

/**
 * \class TYRemPointControlAction
 * \brief Action de suppression d'un point de controle
 * \author Projet_Tympan
 */
class TYRemPointControlAction : public TYModelerAction
{
public:
    /**
     * Constructeur.
     */
    TYRemPointControlAction(LPTYProjet pProjet, LPTYPointControl pPtControl,
                            TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Destructeur.
     */
    virtual ~TYRemPointControlAction();

    virtual void undo();
    virtual void redo();

protected:
    LPTYProjet _pProjet;
    LPTYPointControl _pPtControl;
};


/**
 * \class TYAddMaillageToCalculAction
 * \brief Action d'ajout d'un maillage au calcul
 * \author Projet_Tympan
 */
class TYAddMaillageToCalculAction : public TYModelerAction
{
public:
    /**
     * Constructeur.
     */
    TYAddMaillageToCalculAction(LPTYMaillageGeoNode pMaillageGeoNode, LPTYCalcul pCalcul,
                                TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Constructeur.
     */
    TYAddMaillageToCalculAction(LPTYMaillage pMaillage, LPTYCalcul pCalcul,
                                TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Destructeur.
     */
    virtual ~TYAddMaillageToCalculAction();

    virtual void undo();
    virtual void redo();

protected:
    LPTYCalcul _pCalcul;
    LPTYMaillageGeoNode _pMaillageGeoNode;
};


/**
 * \class TYRemMaillageToCalculAction
 * \brief Action de suppression d'un maillage au calcul
 * \author Projet_Tympan
 */
class TYRemMaillageToCalculAction : public TYModelerAction
{
public:
    /**
     * Constructeur.
     */
    TYRemMaillageToCalculAction(LPTYMaillageGeoNode pMaillageGeoNode, LPTYCalcul pCalcul,
                                TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Constructeur.
     */
    TYRemMaillageToCalculAction(LPTYMaillage pMaillage, LPTYCalcul pCalcul,
                                TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Destructeur.
     */
    virtual ~TYRemMaillageToCalculAction();

    virtual void undo();
    virtual void redo();

protected:
    LPTYCalcul _pCalcul;
    LPTYMaillageGeoNode _pMaillageGeoNode;
};


/**
 * \class TYAddSiteNodeToSiteNodeAction
 * \brief Action d'ajout d'un site a un ensemble de sites
 * \author Projet_Tympan
 */
class TYAddSiteNodeToSiteNodeAction : public TYModelerAction
{
public:
    /**
     * Constructeur.
     */
    TYAddSiteNodeToSiteNodeAction(LPTYSiteNodeGeoNode pSiteNodeGeoNode, LPTYSiteNode pSiteNodeParent,
                                  TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Constructeur.
     */
    TYAddSiteNodeToSiteNodeAction(LPTYSiteNode pSiteNode, LPTYSiteNode pSiteNodeParent,
                                  TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Destructeur.
     */
    virtual ~TYAddSiteNodeToSiteNodeAction();

    virtual void undo();
    virtual void redo();

protected:
    LPTYSiteNode _pSiteNodeParent;
    LPTYSiteNodeGeoNode _pSiteNodeGeoNode;
};

/**
 * \class TYRemSiteNodeToSiteNodeAction
 * \brief Action de suppression d'un site a un ensemble de sites
 * \author Projet_Tympan
 */
class TYRemSiteNodeToSiteNodeAction : public TYModelerAction
{
public:
    /**
     * Constructeur.
     */
    TYRemSiteNodeToSiteNodeAction(LPTYSiteNodeGeoNode pSiteNodeGeoNode, LPTYSiteNode pSiteNodeParent,
                                  TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Constructeur.
     */
    TYRemSiteNodeToSiteNodeAction(LPTYSiteNode pSiteNode, LPTYSiteNode pSiteNodeParent,
                                  TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Destructeur.
     */
    virtual ~TYRemSiteNodeToSiteNodeAction();

    virtual void undo();
    virtual void redo();

protected:
    LPTYSiteNode _pSiteNodeParent;
    LPTYSiteNodeGeoNode _pSiteNodeGeoNode;
};

/**
 * \class TYMovePointPolyLineAction
 * \brief Action editing for a polyline (when moving point)
 * \author Projet_Tympan
 */
class TYMovePointPolyLineAction : public TYModelerAction
{
public:
    /**
     * \brief Constructor.
     * \param pElt The element which owns the edited polyline
     * \param polyLine The edited polyline
     * \param ptId The index of the edited point
     * \param oldPos The old position (undo)
     * \param newPos The new position (redo)
     * \param pModeler The modeler
     * \param actionName The action name
     */
    TYMovePointPolyLineAction(LPTYElement pElt, TYTabPoint& polyLine, int ptId,
                              const TYPoint& oldPos, const TYPoint& newPos,
                              TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Destructor.
     */
    virtual ~TYMovePointPolyLineAction();

    /**
     * \brief Function to be called to update the new position of the edited point
     * \param newPos The new position of the edited point
     */
    void setNewPos(const TYPoint& newPos) { _newPos = newPos; }

    /**
     * \brief The undo action
     */
    virtual void undo();

    /**
     * \brief The redo action
     */
    virtual void redo();

protected:
    /// The element which owns the edited polyline
    LPTYElement _pElt;
    /// The edited polyline
    TYTabPoint& _polyLine;
    /// Id of the point
    int _ptId;
    /// The old position of the point (undo)
    TYPoint _oldPos;
    /// The new position of the point (redo)
    TYPoint _newPos;
};

/**
 * \class TYInsertPointPolyLineAction
 * \brief Action editing for a polyline (when inserting point)
 * \author Projet_Tympan
 */
class TYInsertPointPolyLineAction : public TYModelerAction
{
public:
    /**
     * \brief Constructor.
     * \param pElt The element which owns the edited polyline
     * \param polyLine The edited polyline
     * \param ptId The index of the new inserted point
     * \param pt The new inserted point
     * \param pModeler The modeler
     * \param actionName The action name
     */
    TYInsertPointPolyLineAction(LPTYElement pElt, TYTabPoint& polyLine, int ptId,
                                const TYPoint& pt, TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Destructor.
     */
    virtual ~TYInsertPointPolyLineAction();

    /**
     * \brief The undo action
     */
    virtual void undo();

    /**
     * \brief The redo action
     */
    virtual void redo();

protected:
    /// The element which owns the edited polyline
    LPTYElement _pElt;
    /// The edited polyline
    TYTabPoint& _polyLine;
    /// Id of the point
    int _ptId;
    /// The new inserted point
    TYPoint _pt;
};

/**
 * \class TYInsertPointPolyLineAction
 * \brief Action editing for a polyline (when deleting point)
 * \author Projet_Tympan
 */
class TYRemovePointPolyLineAction : public TYModelerAction
{
public:
    /**
     * \brief Constructor.
     * \param pElt The element which owns the edited polyline
     * \param polyLine The edited polyline
     * \param ptId The index of the deleted point
     * \param pt The deleted point
     * \param pModeler The modeler
     * \param actionName The action name
     */
    TYRemovePointPolyLineAction(LPTYElement pElt, TYTabPoint& polyLine, int ptId,
                                const TYPoint& pt, TYModelerFrame* pModeler, const QString& actionName);
    /**
     * Destructor.
     */
    virtual ~TYRemovePointPolyLineAction();

    /**
     * \brief The undo action
     */
    virtual void undo();

    /**
     * \brief The redo action
     */
    virtual void redo();

protected:
    /// The element which owns the edited polyline
    LPTYElement _pElt;
    /// The edited polyline
    TYTabPoint& _polyLine;
    /// Id of the point
    int _ptId;
    /// The new inserted point
    TYPoint _pt;
};

#endif // __TY_ACTIONS__
