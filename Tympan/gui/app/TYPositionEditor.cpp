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
 * \file TYPositionEditor.cpp
 * \brief gestion de la position selon les modes 'moving', 'rotation', 'edition
 *
 *
 */



#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include <qtimer.h>

#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/GraphicIHM/ToolsGraphic/OGLLineElement.h"

#include "Tympan/models/business/infrastructure/TYEcran.h"

#include <qcursor.h>
#include <qstatusbar.h>

#if defined(WIN32)
#include <crtdbg.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif


#define TR(id) OLocalizator::getString("TYPositionEditor", (id))

static bool bTYPositionEditorFirstMove = true;

TYPositionEditor::TYPositionEditor(TYModelerFrame* pModeler, int mode /*=Moving*/) :
    TYAbstractSceneEditor(pModeler)
{
    setMode(mode);

    setAngleStep(15.0);
    setPickPointPrecision(1.0);

    // On recupere le pointeur sur le picker du modeler
    _pPicker = _pModeler->getElementPicker();

    // Camera
    _pCamEditor = new TYCameraEditor(pModeler);
    _pCamEditor->setLeftButtonFunction2D(SLOT(doNothing()));
    _pCamEditor->setMiddleButtonFunction2D(SLOT(cameraTranslate()));
    _pCamEditor->setRightButtonFunction2D(SLOT(doNothing()));
    _pCamEditor->setShiftLeftButtonFunction2D(SLOT(cameraTranslate()));
    _pCamEditor->setShiftRightButtonFunction2D(SLOT(cameraZoom()));

    _shiftOn = false;
    _keyAOn = false;
    _keyDOn = false;
    _active = true;
    _firstCall = false;
    _viewOnlyHighlight = true;
    _angleInfo = 0.0;
    _pLastAction = NULL;
    _inUse = false;
    _bSiteModelerFrame = false;

    _pStepTimer = new QTimer(this);
    _mouseEventActive = false;

    setInteractionTime(20);
    setSensitivity(20.0);

    _pEditElement = NULL;
    _pEditGeoNode = NULL;
    _pEditPoint = NULL;

    // Line 1
    _pOGLLineElementMoving1 = new OGLLineElement();
    _pOGLLineElementMoving1->setColor(OColor(1.0, 0.0, 0.0));
    _pOGLLineElementMoving1->setVisibility(false);
    _pOGLLineElementMoving1->setIs3D(true);

    _pOGLLineElementMoving2 = new OGLLineElement();
    _pOGLLineElementMoving2->setColor(OColor(1.0, 0.0, 0.0));
    _pOGLLineElementMoving2->setVisibility(false);
    _pOGLLineElementMoving2->setIs3D(true);

    _pInteractor->getRenderer()->addOGLElement(_pOGLLineElementMoving1);
    _pInteractor->getRenderer()->addOGLElement(_pOGLLineElementMoving2);
}

TYPositionEditor::~TYPositionEditor()
{
    delete _pCamEditor;

    if (_pLastAction)
    {
        delete _pLastAction;
        _pLastAction = NULL;
    }

    _pInteractor->getRenderer()->removeOGLElement(_pOGLLineElementMoving1);
    delete _pOGLLineElementMoving1;
    _pInteractor->getRenderer()->removeOGLElement(_pOGLLineElementMoving2);
    delete _pOGLLineElementMoving2;

}

void TYPositionEditor::connect()
{
    TYAbstractSceneEditor::connect();

    // En fonction du type de modeler on edite pas le meme parent
    if (QString(_pModeler->metaObject()->className()).compare("TYMachineModelerFrame") == 0)
    {
        // On edite les volumes
        QObject::connect(_pPicker, SIGNAL(acousticCylinderPicked(TYElement*)), this, SLOT(initEditVolume(TYElement*)));
        QObject::connect(_pPicker, SIGNAL(acousticSemiCylinderPicked(TYElement*)), this, SLOT(initEditVolume(TYElement*)));
        QObject::connect(_pPicker, SIGNAL(acousticBoxPicked(TYElement*)), this, SLOT(initEditVolume(TYElement*)));
    }
    else if (QString(_pModeler->metaObject()->className()).compare("TYBatimentModelerFrame") == 0)
    {
        // On edite les silos
        QObject::connect(_pPicker, SIGNAL(acousticCylinderPicked(TYElement*)), this, SLOT(initEditVolume(TYElement*)));
        // On edite les machines
        QObject::connect(_pPicker, SIGNAL(machinePicked(TYElement*)), this, SLOT(initEditMachine(TYElement*)));
        // Et les Sources ponctuelles
        QObject::connect(_pPicker, SIGNAL(sourcePonctuellePicked(TYElement*)), this, SLOT(initEditSrcPonct(TYElement*)));
        // Et les etages
        QObject::connect(_pPicker, SIGNAL(etagePicked(TYElement*)), this, SLOT(initEditVolume(TYElement*)));
        // Et les ecrans
        QObject::connect(_pPicker, SIGNAL(ecranPicked(TYElement*)), this, SLOT(initEditEcran(TYElement*)));
    }
    else if (QString(_pModeler->metaObject()->className()).compare("TYSiteModelerFrame") == 0)
    {
        _bSiteModelerFrame = true;

        // On edite les batiments
        QObject::connect(_pPicker, SIGNAL(batimentPicked(TYElement*)), this, SLOT(initEditBatiment(TYElement*)));
        // Et les machines "en plein air"
        QObject::connect(_pPicker, SIGNAL(machinePicked(TYElement*)), this, SLOT(initEditMachine(TYElement*)));

        // Les elements de topo et d'infra
        QObject::connect(_pPicker, SIGNAL(routePicked(TYElement*)), this, SLOT(initEditRoute(TYElement*)));
        QObject::connect(_pPicker, SIGNAL(reseauTransportPicked(TYElement*)), this, SLOT(initEditResTrans(TYElement*)));
        QObject::connect(_pPicker, SIGNAL(courbeNiveauPicked(TYElement*)), this, SLOT(initEditCrbNiv(TYElement*)));
        QObject::connect(_pPicker, SIGNAL(coursEauPicked(TYElement*)), this, SLOT(initEditCrsEau(TYElement*)));
        QObject::connect(_pPicker, SIGNAL(planEauPicked(TYElement*)), this, SLOT(initEditPlanEau(TYElement*)));
        QObject::connect(_pPicker, SIGNAL(terrainPicked(TYElement*)), this, SLOT(initEditTerrain(TYElement*)));
        QObject::connect(_pPicker, SIGNAL(sourcePonctuellePicked(TYElement*)), this, SLOT(initEditSrcPonct(TYElement*)));
        QObject::connect(_pPicker, SIGNAL(ecranPicked(TYElement*)), this, SLOT(initEditEcran(TYElement*)));

        // Les maillages et elements propres aux calculs
        QObject::connect(_pPicker, SIGNAL(pointControlPicked(TYElement*)), this, SLOT(initEditPointControl(TYElement*)));
        QObject::connect(_pPicker, SIGNAL(rectangularMaillagePicked(TYElement*)), this, SLOT(initEditMaillage(TYElement*)));
        QObject::connect(_pPicker, SIGNAL(linearMaillagePicked(TYElement*)), this, SLOT(initEditMaillage(TYElement*)));
    }

    // On connect la camera aussi
    _pCamEditor->connect();

    // Preferences
    static const char dirName[] = "UserPreferences";

    if (TYPreferenceManager::getInstance().exists(dirName, "ViewOnlyHighlight"))
    {
        _viewOnlyHighlight = TYPreferenceManager::getInstance().getBool(dirName, "ViewOnlyHighlight");
    }
    else
    {
        TYPreferenceManager::getInstance().setBool(dirName, "ViewOnlyHighlight", _viewOnlyHighlight);
    }
}

void TYPositionEditor::disconnect()
{
    TYAbstractSceneEditor::disconnect();

    _pPicker->disconnect(this);
    _pCamEditor->disconnect();
}

void TYPositionEditor::init()
{
}

void TYPositionEditor::cancel()
{
    _pPicker->reset();
    _inUse = false;

    showText(false);
    _pInteractor->updateGL();
}

void TYPositionEditor::close()
{
    cancel();
}

void TYPositionEditor::slotKeyPressed(int key)
{
    switch (key)
    {
        case Qt::Key_Shift:
            _shiftOn = true;
            break;
        case Qt::Key_A:
            _keyAOn = true;
            break;
        case Qt::Key_D:
            _keyDOn = true;
            break;
        case Qt::Key_Escape:
            cancel();
            break;
    }
}

void TYPositionEditor::slotKeyReleased(int key)
{
    switch (key)
    {
        case Qt::Key_Shift:
            _shiftOn = false;
            break;
        case Qt::Key_A:
            _keyAOn = false;
            break;
        case Qt::Key_D:
            _keyDOn = false;
            break;
    }
}

void TYPositionEditor::slotViewTypeChanged(int view)
{
    showText(false);
    _pInteractor->updateGL();

    // On relaie le signal au CameraEditor
    _pCamEditor->slotViewTypeChanged(view);
}

void TYPositionEditor::slotMousePressed(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
{
    if (_mouseEventActive)
    {
        return;
    }

    if (_pLastAction)
    {
        delete _pLastAction;
        _pLastAction = NULL;
    }

    if (_active && !_shiftOn)
    {
        if (button == Qt::LeftButton)
        {
            _pPicker->reset();

            // On met a true car la "pick" emet des signaux.
            _mouseEventActive = true;

            // On effectue le picking
            if (!_pPicker->pick(x, _pInteractor->height() - y))
            {
                // Rien de picker, on peut remettre a false, normalement
                // aucun signaux n'ont ete emis par "pick"
                _mouseEventActive = false;
            }
        }
    }

    // Si un element a ete picke et si l'utilisateur accepte d'effacer
    // les resultats courant le cas echeant
    if (_mouseEventActive) // && _pModeler->askForResetResultat())  // DT -- le 22/08/2005
    {
        // On deconnecte la camera le temps de l'edition
        _pCamEditor->disconnect();

        _currentMousePos[0] = x;
        _currentMousePos[1] = y;
        _lastMousePos[0] = x;
        _lastMousePos[1] = y;
        _firstCall = true;
        _angleInfo = 0.0;
        _pStepTimer->start(_interactionTime);

        if (_mode == Moving)
        {
            TYApplication::setOverrideCursor(Qt::SizeAllCursor);
        }
        else
        {
            TYApplication::setOverrideCursor(Qt::BlankCursor);
        }
    }
}

void TYPositionEditor::slotMouseReleased(int x, int y, Qt::MouseButton button, Qt::KeyboardModifiers state)
{
    bTYPositionEditorFirstMove = true;
    if (_mouseEventActive)
    {
        _pStepTimer->stop();
        _mouseEventActive = false;
        QObject::disconnect(_pStepTimer, 0, this, 0);

        if (_pLastAction)
        {
            _pModeler->getActionManager()->addAction(_pLastAction);
            _pLastAction = NULL;
        }

        if (_pEditGeoNode && _viewOnlyHighlight)
        {
            _pEditGeoNode->getGraphicObject()->viewOnlyHighlight(false);
        }

        _inUse = false;
        _pEditElement = NULL;
        _pEditPoint = NULL;

        showText(false);
        _pOGLLineElementMoving1->setVisibility(false);
        _pOGLLineElementMoving2->setVisibility(false);

        // On arrete le highlight dans la scene 3D
        _pPicker->highlightElement(NULL);//az++

        _pInteractor->getRenderer()->updateDisplayListOverlay(NULL, NULL);
        _pInteractor->getRenderer()->updateDisplayList();//az--
        _pEditGeoNode = NULL;//az++

        _pModeler->updateView(true, true); // DT++ le 22/08/2005

        _pModeler->askForResetResultat(); // CLM++ le 10/01/2012

        // L'edition est terminee on reconnecte la camera
        _pCamEditor->connect();

        // La scene a ete modifiee
        TYElement::setIsSavedOk(true);

        TYApplication::restoreOverrideCursor();
    }
}

void TYPositionEditor::slotMouseMoved(int x, int y, Qt::MouseButtons button, Qt::KeyboardModifiers state)
{
    _currentMousePos[0] = x;
    _currentMousePos[1] = y;
}

void TYPositionEditor::initEditSiteNode(TYElement* pElt)
{
    if (!_mouseEventActive || _inUse)
    {
        return;
    }

    // Pour traiter le cas d'une imbrication verticale
    // (site in site in site in ...in siteroot), on va rechercher
    // le site qui est directement enfant du site pointe par le modeleur

    // On recupere le site pointe par le modeleur
    TYElement* pModelerTarget = NULL;
    if (_pModeler->getElement()->isA("TYProjet"))
    {
        pModelerTarget = TYProjet::safeDownCast(_pModeler->getElement())->getSite();
    }
    else
    {
        pModelerTarget = _pModeler->getElement();
    }

    // Ensuite on boucle pour retrouver son fils
    TYElement* pParent = pElt->getParent();

    while (pParent && (pParent != pModelerTarget))
    {
        pElt = pParent;
        pParent = pParent->getParent();
    }

    if (!pElt) { return; } // Au cas ou on serait parti dans les choux, on sort ...

    // On positionne l'element courant
    _pEditElement = pElt;

    // On le highlight dans la scene 3D
    _pPicker->highlightElement(_pEditElement);

    TYSiteNode* pSiteNodeChild = TYSiteNode::safeDownCast(_pEditElement);
    TYSiteNode* pSiteNodeParent = TYSiteNode::safeDownCast(pSiteNodeChild->getParent());

    // On ne peut rien faire sans le parent...
    if (!pSiteNodeParent) { return; }

    // On recupere le GeoNode associe
    _pEditGeoNode = (TYGeometryNode*) pSiteNodeParent->findSiteNode(pSiteNodeChild).getRealPointer();
    if (!_pEditGeoNode) { return; }

    // Visu du highlight seulement
    if (_viewOnlyHighlight)
    {
        _pEditGeoNode->getGraphicObject()->viewOnlyHighlight(_viewOnlyHighlight);
    }

    if (_mode != Edition)
    {
        // On connecte le timer
        QObject::connect(_pStepTimer, SIGNAL(timeout()), this, SLOT(editGeoNode()));
    }
}

void TYPositionEditor::initEditVolume(TYElement* pElt)
{
    if (!_mouseEventActive)
    {
        return;
    }

    // On positionne l'element courant
    _pEditElement = pElt;

    // On le highlight dans la scene 3D
    _pPicker->highlightElement(_pEditElement);

    TYAcousticVolume* pAccVol = TYAcousticVolume::safeDownCast(_pEditElement);
    TYAcousticVolumeNode* pParent = TYAcousticVolumeNode::safeDownCast(pAccVol->getParent());

    // On ne peut rien faire sans le parent...
    if (!pParent) { return; }

    // On recupere le GeoNode associe
    _pEditGeoNode = (TYGeometryNode*) pParent->findAcousticVol(pAccVol).getRealPointer();
    if (!_pEditGeoNode) { return; }


    // Visu du highlight seulement
    if (_viewOnlyHighlight)
    {
        _pEditGeoNode->getGraphicObject()->viewOnlyHighlight(_viewOnlyHighlight);
    }

    if (_mode != Edition)
    {
        // On connecte le timer
        QObject::connect(_pStepTimer, SIGNAL(timeout()), this, SLOT(editGeoNode()));
    }
}

void TYPositionEditor::initEditMachine(TYElement* pElt)
{
    if (!_mouseEventActive)
    {
        return;
    }

    if (_bSiteModelerFrame)
    {
        // Si on est dans un siteModelerFrame, on s'assure que le site
        // represente par le modeleur est le parent direct de l'element picke
        // Si oui, on peut bouger l'element,
        // Si non, on renvoie sur initEditSiteNode qui entrainera le mouvement
        // du site imbrique de niveau juste inferieur au site represente par le modeleur
        if (selectElement(pElt)) { return; }
    }

    // On positionne l'element courant
    _pEditElement = pElt;

    // On le highlight dans la scene 3D
    _pPicker->highlightElement(_pEditElement);

    TYMachine* pMachine = TYMachine::safeDownCast(_pEditElement);
    TYElement* pParent = TYElement::safeDownCast(pMachine->getParent());

    // On ne peut rien faire sans le parent...
    if (!pParent) { return; }

    // Le parent d'une Machine peut etre un Etage ou Infrastructure
    if (pParent->isA("TYEtage"))
    {
        _pEditGeoNode = (TYGeometryNode*)((TYEtage*) pParent)->findMachine(pMachine).getRealPointer();
    }
    else if (pParent->isA("TYInfrastructure"))
    {
        _pEditGeoNode = (TYGeometryNode*)((TYInfrastructure*) pParent)->findMachine(pMachine).getRealPointer();
    }
    else
    {
        _pEditGeoNode = NULL;
    }

    if (!_pEditGeoNode) { return; }

    // Visu du highlight seulement
    if (_viewOnlyHighlight)
    {
        _pEditGeoNode->getGraphicObject()->viewOnlyHighlight(_viewOnlyHighlight);
    }

    if (_mode != Edition)
    {
        // On connecte le timer
        QObject::connect(_pStepTimer, SIGNAL(timeout()), this, SLOT(editGeoNode()));

        // On "prend la main"
        _inUse = true;
    }
}

/*void TYPositionEditor::initEditEtage(TYElement* pElt)
{
    if (!_mouseEventActive || _inUse) {
        return;
    }

    if (_pModeler->getCurrentView() != TYModelerFrame::TopView) {
        // L'edition d'un etage n'est possible qu'en vue de dessus
        return;
    }

    // On positionne l'element courant
    _pEditElement = pElt;

    TYEtage * pEtage = TYEtage::safeDownCast(_pEditElement);
    TYBatiment * pParent = TYBatiment::safeDownCast(pEtage->getParent());

    // On ne peut rien faire sans le parent...
    if (!pParent)   return;

    // On recupere le GeoNode associe
    _pEditGeoNode = (TYGeometryNode *) pParent->findAcousticVol(pEtage).getRealPointer();
    if (!_pEditGeoNode) return;
}*/

void TYPositionEditor::initEditEcran(TYElement* pElt)
{
    if (!_mouseEventActive || _inUse)
    {
        return;
    }

    if (_pModeler->getCurrentView() != TYModelerFrame::TopView)
    {
        // L'edition d'un etage n'est possible qu'en vue de dessus
        return;
    }

    if (_bSiteModelerFrame)
    {
        // Si on est dans un siteModelerFrame, on s'assure que le site
        // represente par le modeleur est le parent direct de l'element picke
        // Si oui, on peut bouger l'element,
        // Si non, on renvoie sur initEditSiteNode qui entrainera le mouvement
        // du site imbrique de niveau juste inferieur au site represente par le modeleur
        if (selectElement(pElt)) { return; }
    }

    // On positionne l'element courant
    _pEditElement = pElt;

    TYEcran* pEcran = TYEcran::safeDownCast(_pEditElement);
    TYBatiment* pParent = TYBatiment::safeDownCast(pEcran->getParent());

    // On ne peut rien faire sans le parent...
    if (!pParent) { return; }

    // On recupere le GeoNode associe
    _pEditGeoNode = (TYGeometryNode*) pParent->findAcousticVol(pEcran).getRealPointer();
    if (!_pEditGeoNode) { return; }
}

void TYPositionEditor::initEditBatiment(TYElement* pElt)
{
    if (!_mouseEventActive)
    {
        return;
    }


    if (_bSiteModelerFrame)
    {
        // Si on est dans un siteModelerFrame, on s'assure que le site
        // represente par le modeleur est le parent direct de l'element picke
        // Si oui, on peut bouger l'element,
        // Si non, on renvoie sur initEditSiteNode qui entrainera le mouvement
        // du site imbrique de niveau juste inferieur au site represente par le modeleur
        if (selectElement(pElt)) { return; }
    }

    // On positionne l'element courant
    _pEditElement = pElt;

    // On le highlight dans la scene 3D
    _pPicker->highlightElement(_pEditElement);

    TYBatiment* pBatiment = TYBatiment::safeDownCast(_pEditElement);
    TYInfrastructure* pParent = TYInfrastructure::safeDownCast(pBatiment->getParent());

    // On ne peut rien faire sans le parent...
    if (!pParent) { return; }

    // On recupere le GeoNode associe
    _pEditGeoNode = (TYGeometryNode*) pParent->findBatiment(pBatiment).getRealPointer();
    if (!_pEditGeoNode) { return; }

    // Visu du highlight seulement
    if (_viewOnlyHighlight)
    {
        _pEditGeoNode->getGraphicObject()->viewOnlyHighlight(_viewOnlyHighlight);
    }

    if (_mode != Edition)
    {
        // On connecte le timer
        QObject::connect(_pStepTimer, SIGNAL(timeout()), this, SLOT(editGeoNode()));

        // On "prend la main"//az++
        _inUse = true;//az++
    }
}

void TYPositionEditor::initEditRoute(TYElement* pElt)
{
    if (!_mouseEventActive)
    {
        return;
    }

    if (_bSiteModelerFrame)
    {
        // Si on est dans un siteModelerFrame, on s'assure que le site
        // represente par le modeleur est le parent direct de l'element picke
        // Si oui, on peut bouger l'element,
        // Si non, on renvoie sur initEditSiteNode qui entrainera le mouvement
        // du site imbrique de niveau juste inferieur au site represente par le modeleur
        if (selectElement(pElt)) { return; }
    }

    // On positionne l'element courant
    _pEditElement = pElt;

    TYRoute* pRoute = TYRoute::safeDownCast(_pEditElement);
    TYInfrastructure* pParent = TYInfrastructure::safeDownCast(pRoute->getParent());

    // On ne peut rien faire sans le parent...
    if (!pParent) { return; }

    // On recupere le GeoNode associe
    _pEditGeoNode = (TYGeometryNode*) pParent->findRoute(pRoute).getRealPointer();
    if (!_pEditGeoNode) { return; }

    if (_mode == Edition)
    {
        TYTabPoint& pts = pRoute->getTabPoint();
        editPolyLine(pRoute, pts, false);
    }
    else
    {
        _pPicker->highlightElement(_pEditElement);
        // On connecte le timer
        QObject::connect(_pStepTimer, SIGNAL(timeout()), this, SLOT(editGeoNode()));
        // On "prend la main"
        _inUse = true;
    }
}

void TYPositionEditor::initEditResTrans(TYElement* pElt)
{
    if (!_mouseEventActive)
    {
        return;
    }

    if (_bSiteModelerFrame)
    {
        // Si on est dans un siteModelerFrame, on s'assure que le site
        // represente par le modeleur est le parent direct de l'element picke
        // Si oui, on peut bouger l'element,
        // Si non, on renvoie sur initEditSiteNode qui entrainera le mouvement
        // du site imbrique de niveau juste inferieur au site represente par le modeleur
        if (selectElement(pElt)) { return; }
    }

    // On positionne l'element courant
    _pEditElement = pElt;

    TYReseauTransport* pResTrans = TYReseauTransport::safeDownCast(_pEditElement);
    TYInfrastructure* pParent = TYInfrastructure::safeDownCast(pResTrans->getParent());

    // On ne peut rien faire sans le parent...
    if (!pParent) { return; }

    // On recupere le GeoNode associe
    _pEditGeoNode = (TYGeometryNode*) pParent->findResTrans(pResTrans).getRealPointer();
    if (!_pEditGeoNode) { return; }

    if (_mode == Edition)
    {
        TYTabPoint& pts = pResTrans->getTabPoint();
        editPolyLine(pResTrans, pts, false);
    }
    else
    {
        _pPicker->highlightElement(_pEditElement);
        // On connecte le timer
        QObject::connect(_pStepTimer, SIGNAL(timeout()), this, SLOT(editGeoNode()));
        // On "prend la main"
        _inUse = true;
    }
}

void TYPositionEditor::initEditCrbNiv(TYElement* pElt)
{
    if (!_mouseEventActive)
    {
        return;
    }

    if (_bSiteModelerFrame)
    {
        // Si on est dans un siteModelerFrame, on s'assure que le site
        // represente par le modeleur est le parent direct de l'element picke
        // Si oui, on peut bouger l'element,
        // Si non, on renvoie sur initEditSiteNode qui entrainera le mouvement
        // du site imbrique de niveau juste inferieur au site represente par le modeleur
        if (selectElement(pElt)) { return; }
    }

    // On positionne l'element courant
    _pEditElement = pElt;

    TYCourbeNiveau* pCrbNiv = TYCourbeNiveau::safeDownCast(_pEditElement);
    TYTopographie* pParent = TYTopographie::safeDownCast(pCrbNiv->getParent());

    // On ne peut rien faire sans le parent...
    if (!pParent) { return; }

    // On recupere le GeoNode associe
    _pEditGeoNode = (TYGeometryNode*) pParent->findCrbNiv(pCrbNiv).getRealPointer();
    if (!_pEditGeoNode) { return; }

    // On marque l'alti comme modifiee ici car on ne peut plus le faire apres
    pParent->getAltimetrie()->setIsGeometryModified(true);

    if (_mode == Edition)
    {
        TYTabPoint& pts = pCrbNiv->getListPoints();
        editPolyLine(pCrbNiv, pts, false);
    }
    else
    {
        _pPicker->highlightElement(_pEditElement);
        // On connecte le timer
        QObject::connect(_pStepTimer, SIGNAL(timeout()), this, SLOT(editGeoNode()));
        // On "prend la main"
        _inUse = true;
    }
}

void TYPositionEditor::initEditCrsEau(TYElement* pElt)
{

    if (!_mouseEventActive)
    {
        return;
    }

    if (_bSiteModelerFrame)
    {
        // Si on est dans un siteModelerFrame, on s'assure que le site
        // represente par le modeleur est le parent direct de l'element picke
        // Si oui, on peut bouger l'element,
        // Si non, on renvoie sur initEditSiteNode qui entrainera le mouvement
        // du site imbrique de niveau juste inferieur au site represente par le modeleur
        if (selectElement(pElt)) { return; }
    }

    // On positionne l'element courant
    _pEditElement = pElt;

    TYCoursEau* pCrsEau = TYCoursEau::safeDownCast(_pEditElement);
    TYTopographie* pParent = TYTopographie::safeDownCast(pCrsEau->getParent());

    // On ne peut rien faire sans le parent...
    if (!pParent) { return; }

    // On recupere le GeoNode associe
    _pEditGeoNode = (TYGeometryNode*) pParent->findCrsEau(pCrsEau).getRealPointer();
    if (!_pEditGeoNode) { return; }

    // We tag the altimetry as modified since we can't do it later
    pParent->getAltimetrie()->setIsGeometryModified(true);

    if (_mode == Edition)
    {
        TYTabPoint& pts = pCrsEau->getTabPoint();
        editPolyLine(pCrsEau, pts, false, pParent->getAltimetrie());
    }
    else
    {
        _pPicker->highlightElement(_pEditElement);
        // On connecte le timer
        QObject::connect(_pStepTimer, SIGNAL(timeout()), this, SLOT(editGeoNode()));
        // On "prend la main"
        _inUse = true;
    }
}

void TYPositionEditor::initEditPlanEau(TYElement* pElt)
{
    if (!_mouseEventActive)
    {
        return;
    }

    if (_bSiteModelerFrame)
    {
        // Si on est dans un siteModelerFrame, on s'assure que le site
        // represente par le modeleur est le parent direct de l'element picke
        // Si oui, on peut bouger l'element,
        // Si non, on renvoie sur initEditSiteNode qui entrainera le mouvement
        // du site imbrique de niveau juste inferieur au site represente par le modeleur
        if (selectElement(pElt)) { return; }
    }

    // On positionne l'element courant
    _pEditElement = pElt;

    TYPlanEau* pPlanEau = TYPlanEau::safeDownCast(_pEditElement);
    TYTopographie* pParent = TYTopographie::safeDownCast(pPlanEau->getParent());

    // On ne peut rien faire sans le parent...
    if (!pParent) { return; }

    // On recupere le GeoNode associe
    _pEditGeoNode = (TYGeometryNode*) pParent->findPlanEau(pPlanEau).getRealPointer();
    if (!_pEditGeoNode) { return; }

    // On marque l'alti comme modifiee ici car on ne peut plus le faire apres
    pParent->getAltimetrie()->setIsGeometryModified(true);

    if (_mode == Edition)
    {
        TYTabPoint& pts = pPlanEau->getListPoints();
        editPolyLine(pPlanEau, pts, true, pParent->getAltimetrie());
    }
    else
    {
        _pPicker->highlightElement(_pEditElement);
        // On connecte le timer
        QObject::connect(_pStepTimer, SIGNAL(timeout()), this, SLOT(editGeoNode()));
        // On "prend la main"
        _inUse = true;
    }
}

void TYPositionEditor::initEditTerrain(TYElement* pElt)
{
    if (!_mouseEventActive)
    {
        return;
    }

    if (_bSiteModelerFrame)
    {
        // Si on est dans un siteModelerFrame, on s'assure que le site
        // represente par le modeleur est le parent direct de l'element picke
        // Si oui, on peut bouger l'element,
        // Si non, on renvoie sur initEditSiteNode qui entrainera le mouvement
        // du site imbrique de niveau juste inferieur au site represente par le modeleur
        if (selectElement(pElt)) { return; }
    }

    // On positionne l'element courant
    _pEditElement = pElt;

    TYTerrain* pTerrain = TYTerrain::safeDownCast(_pEditElement);
    TYTopographie* pParent = TYTopographie::safeDownCast(pTerrain->getParent());

    // On ne peut rien faire sans le parent...
    if (!pParent) { return; }

    // On recupere le GeoNode associe
    _pEditGeoNode = (TYGeometryNode*) pParent->findTerrain(pTerrain).getRealPointer();
    if (!_pEditGeoNode) { return; }

    // We tag the altimetry as modified since we can't do it later
    pParent->getAltimetrie()->setIsGeometryModified(true);

    if (_mode == Edition)
    {
        TYTabPoint& pts = pTerrain->getListPoints();
        editPolyLine(pTerrain, pts, true, pParent->getAltimetrie());
    }
    else
    {
        _pPicker->highlightElement(_pEditElement);
        // On connecte le timer
        QObject::connect(_pStepTimer, SIGNAL(timeout()), this, SLOT(editGeoNode()));
        // On "prend la main"
        _inUse = true;
    }
}

void TYPositionEditor::initEditSrcPonct(TYElement* pElt)
{
    if (!_mouseEventActive)
    {
        return;
    }

    if (_bSiteModelerFrame)
    {
        // Si on est dans un siteModelerFrame, on s'assure que le site
        // represente par le modeleur est le parent direct de l'element picke
        // Si oui, on peut bouger l'element,
        // Si non, on renvoie sur initEditSiteNode qui entrainera le mouvement
        // du site imbrique de niveau juste inferieur au site represente par le modeleur
        if (selectElement(pElt)) { return; }
    }

    // On positionne l'element courant
    _pEditElement = pElt;

    TYUserSourcePonctuelle* pSrcPonct = TYUserSourcePonctuelle::safeDownCast(_pEditElement);

    if (!pSrcPonct)
    {
        return;
    }

    TYInfrastructure* pParent = TYInfrastructure::safeDownCast(pSrcPonct->getParent()); //az--
    if (pParent)  //az--
    {
        _pEditGeoNode = (TYGeometryNode*) pParent->findSrc(pSrcPonct).getRealPointer(); //az--
    }//az--

    if (_mode == Moving)
    {
        _pEditPoint = pSrcPonct->getPos();

        _pLastAction = new TYMoveSrcPonctAction(pSrcPonct, *_pEditPoint, TYPoint(), _pModeler, TR("id_action_movesrcponct"));

        _pPicker->highlightElement(_pEditPoint);

        // On connecte le timer
        QObject::connect(_pStepTimer, SIGNAL(timeout()), this, SLOT(editPoint()));    // DT++ 20051201
        // On "prend la main"
        _inUse = true;
    }
    else if (_mode == Rotation)
    {
        // On le highlight dans la scene 3D
        _pPicker->highlightElement(_pEditElement);

        _pLastAction = new TYOrienteSrcPonctAction(pSrcPonct, pSrcPonct->getOrientation(), OVector3D(), _pModeler, TR("id_action_orientsrcponct"));

        // On connecte le timer
        QObject::connect(_pStepTimer, SIGNAL(timeout()), this, SLOT(orienteSrcPonct()));    //az--

        // On "prend la main"
        _inUse = true;
    }

}

void TYPositionEditor::initEditPointControl(TYElement* pElt)
{
    if (!_mouseEventActive)
    {
        return;
    }

    // On positionne l'element courant
    _pEditElement = pElt;

    TYPointControl* pPointControl = TYPointControl::safeDownCast(_pEditElement);

    if (!pPointControl)
    {
        return;
    }

    if (_mode == Moving)
    {
        _pEditPoint = (LPTYPoint&) pPointControl;

        _pLastAction = new TYMovePointControlAction(pPointControl, *_pEditPoint, TYPoint(), _pModeler, TR("id_action_movepointcontrol"));

        _pPicker->highlightElement(_pEditPoint);

        // On connecte le timer
        QObject::connect(_pStepTimer, SIGNAL(timeout()), this, SLOT(editPoint()));
    }
}

void TYPositionEditor::initEditMaillage(TYElement* pElt)
{
    if (!_mouseEventActive)
    {
        return;
    }

    // On positionne l'element courant
    _pEditElement = pElt;

    // On le highlight dans la scene 3D
    _pPicker->highlightElement(_pEditElement);

    TYMaillage* pMaillage = TYMaillage::safeDownCast(_pEditElement);
    TYCalcul* pParent = TYCalcul::safeDownCast(pMaillage->getParent());

    // On ne peut rien faire sans le parent...
    if (!pParent) { return; }

    // On recupere le GeoNode associe
    _pEditGeoNode = (TYGeometryNode*) pParent->findMaillage(pMaillage).getRealPointer();
    if (!_pEditGeoNode) { return; }

    // Visu du highlight seulement
    if (_viewOnlyHighlight)
    {
        _pEditGeoNode->getGraphicObject()->viewOnlyHighlight(_viewOnlyHighlight);
    }

    if (_mode != Edition)
    {
        // On connecte le timer
        QObject::connect(_pStepTimer, SIGNAL(timeout()), this, SLOT(editGeoNode()));
    }
}

void TYPositionEditor::PointOfGeoNodeInCurrentSiteCoordinates(TYGeometryNode* pGeoNode, OPoint3D& centralPointGN)
{
    PointOfGeoNodeInCurrentSiteCoordinatesOrPointOfCurrentSiteInGeoNodeCoordinates(pGeoNode, centralPointGN, true);
}

void TYPositionEditor::PointOfCurrentSiteInGeoNodeCoordinates(TYGeometryNode* pGeoNode, OPoint3D& centralPointGN)
{
    PointOfGeoNodeInCurrentSiteCoordinatesOrPointOfCurrentSiteInGeoNodeCoordinates(pGeoNode, centralPointGN, false);
}

void TYPositionEditor::PointOfGeoNodeInCurrentSiteCoordinatesOrPointOfCurrentSiteInGeoNodeCoordinates(TYGeometryNode* pGeoNode, OPoint3D& centralPointGN, bool bPointOfGeoNodeInCurrentSiteCoordinates)
{
    TYPoint centralPoint(centralPointGN);
    OMatrix currentMatrix;
    OMatrix totalMatrix;
    if (pGeoNode)
    {
        //Trouvons le GeoNode du Site courant (NULL si c'est le site root)
        TYSiteModelerFrame* pTYSiteModelerFrame = (TYSiteModelerFrame*)_pModeler;
        TYSiteNode* pRootSite = pTYSiteModelerFrame->getSite();
        TYGeometryNode* pRootGeometryNode = TYGeometryNode::GetGeoNode(pRootSite);

        TYListPtrGeoNode GeoNodeParents;
        pGeoNode->GetGeoNodeParentList(GeoNodeParents);

        TYListPtrGeoNode::iterator ite = GeoNodeParents.end();
        int i, rootIndex = 0;
        if (pRootGeometryNode)
        {
            for (i = 0; i < (int)GeoNodeParents.size(); i++)
            {
                ite--;
                if ((*ite) == pRootGeometryNode)
                {
                    rootIndex = i + 1;
                }
            }
        }

        for (i = rootIndex; i < ((int)GeoNodeParents.size() - rootIndex); i++)
        {
            ite--;
            TYGeometryNode* pCurrentTYGeometryNode = (TYGeometryNode*)(*ite);
            currentMatrix = pCurrentTYGeometryNode->getMatrix();
            if (!bPointOfGeoNodeInCurrentSiteCoordinates)
            {
                currentMatrix = currentMatrix.getInvert();
            }
            totalMatrix = totalMatrix * currentMatrix;
        }
        centralPoint = totalMatrix * centralPoint;
    }
    centralPointGN = centralPoint;
}

void TYPositionEditor::editGeoNode()
{
    bool modified = false;
    TYPoint oldPos;
    TYRepere oldRepere;
    bool enableX = true;
    bool enableY = true;
    bool enableZ = true;

    if (_mode == Moving)
    {
        // Copie de l'ancienne pos
        oldPos = _pEditGeoNode->getORepere3D()._origin;

        if ((dynamic_cast<TYCourbeNiveau*>(_pEditGeoNode->getElement()) != nullptr)
            || (dynamic_cast<TYPlanEau*>(_pEditGeoNode->getElement()) != nullptr))
        {
            enableZ = false;
        }

        ORepere3D repere = _pEditGeoNode->getORepere3D();

        // On applique le deplacement
        modified = move(&repere._origin, enableX, enableY, enableZ);//az--

        _pEditGeoNode->setRepere(repere);
    }
    else if (_mode == Rotation)
    {
        // Copie de l'ancien repere
        oldRepere = _pEditGeoNode->getORepere3D();

        if ((dynamic_cast<TYMaillage*>(_pEditGeoNode->getElement()) != nullptr)
            || (dynamic_cast<TYCourbeNiveau*>(_pEditGeoNode->getElement()) != nullptr)
            || (dynamic_cast<TYPlanEau*>(_pEditGeoNode->getElement()) != nullptr)
            || (dynamic_cast<TYSiteNode*>(_pEditGeoNode->getElement()) != nullptr))
        {
            enableX = false;
            enableY = false;
        }

        ORepere3D repere = _pEditGeoNode->getORepere3D();

        // On applique la rotation
        modified = rotate(&repere, enableX, enableY, enableZ);

        _pEditGeoNode->setRepere(repere);

    }

    if (modified && bTYPositionEditorFirstMove)
    {
        //az++
        bTYPositionEditorFirstMove = false;
        //Magouille infâme pour faire disparaitre temporairement le GeoNode car le setVisible ne marche plus vraiment
        ORepere3D repere = _pEditGeoNode->getORepere3D();
        ORepere3D AlOuest;
        AlOuest._origin = OPoint3D(999999, 999999, 999999);
        _pEditGeoNode->setRepere(AlOuest);
        _pEditGeoNode->getGraphicObject()->viewOnlyHighlight(true);
        _pInteractor->getRenderer()->updateDisplayList();//az++ : pour rendre invisible l'objet en mouvement
        _pInteractor->updateGL();//az++
        _pEditGeoNode->setRepere(repere);
    }

    if (modified)
    {
        if (_mode == Moving)
        {
            if (_firstCall)
            {
                _pLastAction = new TYMoveGeoNodeAction(_pEditGeoNode, oldPos, _pEditGeoNode->getORepere3D()._origin, _pModeler, TR("id_action_movegeonode"));
                _firstCall = false;
            }
            else
            {
                ((TYMoveGeoNodeAction*) _pLastAction)->setNewPos(_pEditGeoNode->getORepere3D()._origin);
            }
        }
        else if (_mode == Rotation)
        {
            if (_firstCall)
            {
                _pLastAction = new TYRotateGeoNodeAction(_pEditGeoNode, oldRepere, _pEditGeoNode->getORepere3D(), _pModeler, TR("id_action_rotategeonode"));
                _firstCall = false;
            }
            else
            {
                ((TYRotateGeoNodeAction*) _pLastAction)->setNewRepere(_pEditGeoNode->getORepere3D());
            }
        }

        // Refresh
        _pEditGeoNode->setIsGeometryModified(true);
        _pEditGeoNode->updateGraphic();
        _pModeler->updateView(false, false);

        //Il faut maintenant afficher le GeoNode dans le repere adequat (celui choisit lors de l'ouverture du modeler);
        //Le repere peut etre:
        //* celui du repere du monde    (TYSiteModelerFrame)
        //* celui d'un TYSiteNode fils  (TYSiteModelerFrame)
        //* celui d'un TYBatiment       (TYBatimentModelerFrame)
        //* celui d'un                  (TYMachineModelerFrame)
        //* celui d'un                  (TYFaceModelerFrame)
        TYElement* pRootTYElement = _pModeler->getElement();
        TYGeometryNode* pRootGeometryNode = TYGeometryNode::GetGeoNode(pRootTYElement);

        _pInteractor->getRenderer()->updateDisplayListOverlay(_pEditGeoNode, pRootGeometryNode);
        _pInteractor->getRenderer()->updateDisplayList();
        _pInteractor->updateGL();
    }
}

void TYPositionEditor::editPoint()
{
    bool modified = false;
    double z = 0.0;

    if (_pModeler->getCurrentView() == TYModelerFrame::PerspView)
    {
        // En vue 3D on conserve l'altitude du point
        z = _pEditPoint->_z;
    }

    modified = move(_pEditPoint);

    if (modified)
    {
        if (_pModeler->getCurrentView() == TYModelerFrame::PerspView)
        {
            // En vue 3D on restitue l'altitude du point
            _pEditPoint->_z = z;
        }

        if (_pEditElement->isA("TYCourbeNiveau"))
        {
            ((TYMovePointPolyLineAction*) _pLastAction)->setNewPos(*_pEditPoint);
        }
        else if (dynamic_cast<TYAcousticLine*>(_pEditElement) != nullptr)
        {
            ((TYMovePointPolyLineAction*) _pLastAction)->setNewPos(*_pEditPoint);
        }
        else if (_pEditElement->isA("TYTerrain"))
        {
            ((TYMovePointPolyLineAction*) _pLastAction)->setNewPos(*_pEditPoint);
        }
        else if (_pEditElement->isA("TYPlanEau"))
        {
            ((TYMovePointPolyLineAction*) _pLastAction)->setNewPos(*_pEditPoint);
        }
        else if (dynamic_cast<TYSourcePonctuelle*>(_pEditElement) != nullptr)
        {
            ((TYMoveSrcPonctAction*) _pLastAction)->setNewPos(*_pEditPoint);
        }
        else if (dynamic_cast<TYPointControl*>(_pEditElement) != nullptr)
        {
            ((TYMovePointControlAction*) _pLastAction)->setNewPos(*_pEditPoint);
        }

        _pEditElement->setIsGeometryModified(true);

        if (_pEditElement->getParent())
        {
            _pEditElement->getParent()->setIsGeometryModified(true);
        }

        // Refresh
        _pEditElement->updateGraphic();
        _pModeler->updateView(false, false);

        // Highlight point
        TYPoint worldPoint(*_pEditPoint);
        if (_pEditGeoNode) { worldPoint = _pEditGeoNode->getMatrix() * worldPoint; }
        _pPicker->highlightElement(&worldPoint);    // Fait un updateGL
    }
}

void TYPositionEditor::editPointEtage()
{
    bool modified = false;

    modified = move(_pEditPoint);

    if (modified)
    {
        _pEditElement->setIsGeometryModified(true);
        _pEditElement->getParent()->setIsGeometryModified(true);

        ((TYMovePointEtageAction*) _pLastAction)->setNewPos(*_pEditPoint);

        // Mise a jour de l'etage
        TYEtage* pEtage = (TYEtage*) _pEditElement;
        pEtage->setMurs(_editContour, pEtage->getHauteur(), pEtage->getClosed());

        // Refresh
        if (_pEditElement->getParent())
        {
            _pEditElement->getParent()->updateGraphic();
        }
        else
        {
            _pEditElement->updateGraphic();
        }

        // Highlight point
        TYPoint worldPoint = _pEditGeoNode->getMatrix() * (*_pEditPoint);
        _pPicker->highlightElement(&worldPoint);    // Fait un updateGL
    }
}

void TYPositionEditor::orienteSrcPonct()
{
    // On ne fait rien si la souris n'a pas bougee
    if ((_currentMousePos[0] == _lastMousePos[0]) &&
        (_currentMousePos[1] == _lastMousePos[1]))
    {
        return;
    }

    TYUserSourcePonctuelle* pSrcPonct = TYUserSourcePonctuelle::safeDownCast(_pEditElement);

    //Il nous faut la position du point dans le repere global
    TYPoint pos(0, 0, 0);
    OPoint3D SrcePos(pos);
    OPoint3D opos = _pEditGeoNode->localToGlobal() * SrcePos;//repere le + haut dans la hierarchie, mais pas forcement celui du modelerFrame courant
    pos = opos;

    // Calcul du vecteur orientation : il est constitue de 2 points:
    //- celui donnant la position de la source ponctuelle
    //- celui donnant la position de la souris a l'instant present
    //1. Passage de coordonnees ecrans a coordonnees du repere du modeler frame
    double displaySrcePos[4];
    worldToDisplay(pos._x, pos._z, -pos._y, displaySrcePos);
    double ptCur[4];
    displayToWorld(_currentMousePos[0], _pInteractor->height() - _currentMousePos[1], displaySrcePos[2], ptCur);
    OPoint3D optCur(ptCur[0], -ptCur[2], ptCur[1]);
    //optCur represente maintenant les coordonnees du second point dans le repere du ModelerFrame courant
    //2. Passage de coordonnees du repere du modeler frame a coordonnees globales
    TYElement* pRootTYElement = _pModeler->getElement();
    TYGeometryNode* pRootGeometryNode = TYGeometryNode::GetGeoNode(pRootTYElement);
    if (pRootGeometryNode)
    {
      optCur = pRootGeometryNode->localToGlobal() * optCur;
    }
    //optCur represente maintenant les coordonnees du second point dans le repere le plus haut dans la hierarchie:
    optCur = _pEditGeoNode->globalToLocal() * optCur;
    pos = _pEditGeoNode->globalToLocal() * pos;
    OVector3D vecOrient(pos, optCur);

    switch (_pModeler->getCurrentView())
    {
        case TYModelerFrame::TopView :
            pSrcPonct->getOrientation()._x = vecOrient._x;
            pSrcPonct->getOrientation()._y = vecOrient._y;
            break;
        case TYModelerFrame::LeftView :
            pSrcPonct->getOrientation()._y = vecOrient._y;
            pSrcPonct->getOrientation()._z = vecOrient._z;
            break;
        case TYModelerFrame::FrontView :
            pSrcPonct->getOrientation()._x = vecOrient._x;
            pSrcPonct->getOrientation()._z = vecOrient._z;
            break;
    }

    ((TYOrienteSrcPonctAction*) _pLastAction)->setNewOrient(pSrcPonct->getOrientation());

    _pEditElement->setIsGeometryModified(true);
    _pEditElement->updateGraphic();

    // Save mouse pos car il y a eu un deplacement
    _lastMousePos[0] = _currentMousePos[0];
    _lastMousePos[1] = _currentMousePos[1];
}

bool TYPositionEditor::move(OPoint3D* pPt, bool enableX /*=true*/, bool enableY /*=true*/, bool enableZ /*=true*/)
{
    Q_CHECK_PTR(pPt);
    bool ret = false;

    if (!enableX && !enableY && !enableZ) { return false; }

    // On ne fait rien si la souris n'a pas bougee
    if ((_currentMousePos[0] == _lastMousePos[0]) &&
        (_currentMousePos[1] == _lastMousePos[1]))
    {
        return false;
    }

    //Chgmt de repere si besoin (si SiteNode pere)
    if (QString(_pModeler->metaObject()->className()).compare("TYSiteModelerFrame") == 0)
    {
        PointOfGeoNodeInCurrentSiteCoordinates(_pEditGeoNode, *pPt);
    }

    // On garde une copie
    OPoint3D ptBis = *pPt;

    // Calcul du delta de deplacement
    double pt1[4];
    double pt2[4];
    double org[4];
    worldToDisplay(ptBis._x, ptBis._z, -ptBis._y, org);
    displayToWorld(_currentMousePos[0], _pInteractor->height() - _currentMousePos[1], org[2], pt1);
    displayToWorld(_lastMousePos[0], _pInteractor->height() - _lastMousePos[1], org[2], pt2);
    OVector3D delta(pt1[0] - pt2[0], -(pt1[2] - pt2[2]), pt1[1] - pt2[1]);

    OPoint3D deltap = delta;

    // Calcul de la nouvelle position
    ptBis = OVector3D(ptBis) + deltap;//az++

    // Si la grille magnetique est activee
    if (_pModeler->getSnapGridActive())
    {
        snapToGrid(ptBis._x, ptBis._y, ptBis._z);
    }

    double H = 0.0; //  Hauteur par rapport au sol DT++ 20060322
    // Si le deplacement n'est pas nul
    if (ptBis != *pPt)
    {
        // Calcul de la nouvelle hauteur par rapport au sol DT++ 20060322
        // Sauf a se compliquer la vie, ca ne marchera pas si l'altimetrie n'a pas ete calculee
        if (QString(_pModeler->metaObject()->className()).compare("TYSiteModelerFrame") == 0)
        {
            if (_pEditGeoNode)
            {
                H = ptBis._z - pPt->_z + _pEditGeoNode->getHauteur();
                _pEditGeoNode->setHauteur(H);
            }
            else
            {
                H = ptBis._z - pPt->_z;
                pPt->_z = H;
            }
        }

        // On applique le deplacement
        if (enableX) { pPt->_x = ptBis._x; }
        if (enableY) { pPt->_y = ptBis._y; }
        if (enableZ) { pPt->_z = ptBis._z; }

        // Mets a jour les moving lines
        updateMovingPoint(*pPt);

        // Mets a jour le texte d'information sur la nouvelle position
        if (_mode == Edition)
        {
            // Si la grille magnetique est activee
            if (_pModeler->getSnapGridActive())
            {
                snapToGrid(pt1[0], pt1[1], pt1[2]);
            }

            // On affiche la position globale du curseur
            updateText(QString(TR("id_pos_info")).arg(pPt->_x, 0, 'f', 2).arg(pPt->_y, 0, 'f', 2).arg(pPt->_z, 0, 'f', 2),
                       _currentMousePos[0], _pInteractor->height() - _currentMousePos[1]);
        }
        else
        {
            // On affiche la position globale du point
            updateText(QString(TR("id_pos_info")).arg(pPt->_x, 0, 'f', 2).arg(pPt->_y, 0, 'f', 2).arg(pPt->_z, 0, 'f', 2),
                       (int)org[0], (int)org[1]);
        }

        // Save mouse pos car il y a eu un deplacement
        _lastMousePos[0] = _currentMousePos[0];
        _lastMousePos[1] = _currentMousePos[1];

        // Le point a ete deplace
        ret = true;
    }

    //Chgmt de repere si besoin (si SiteNode pere)
    if (QString(_pModeler->metaObject()->className()).compare("TYSiteModelerFrame") == 0)
    {
        PointOfCurrentSiteInGeoNodeCoordinates(_pEditGeoNode, *pPt);
    }

    return ret;
}

bool TYPositionEditor::rotate(ORepere3D* pRepere, bool enableX /*=true*/, bool enableY /*=true*/, bool enableZ /*=true*/)
{
    Q_CHECK_PTR(pRepere);
    bool ret = false;

    if (!enableX && !enableY && !enableZ) { return false; }

    // On ne fait rien si la souris n'a pas bougee
    if ((_currentMousePos[0] == _lastMousePos[0]) &&
        (_currentMousePos[1] == _lastMousePos[1]))
    {
        return false;
    }

    //Chgmt de repere si besoin (si SiteNode pere)
    if (QString(_pModeler->metaObject()->className()).compare("TYSiteModelerFrame") == 0)
    {
        PointOfGeoNodeInCurrentSiteCoordinates(_pEditGeoNode, pRepere->_origin);
    }

    double center[4];
    worldToDisplay(pRepere->_origin._x, pRepere->_origin._z, -pRepere->_origin._y, center);

    int diffX1 = _currentMousePos[0] - int(center[0]);
    int diffY1 = _currentMousePos[1] - int(center[1]);
    int diffX2 = _lastMousePos[0] - int(center[0]);
    int diffY2 = _lastMousePos[1] - int(center[1]);

    double a1 = atan2(double(diffY1), double(diffX1));
    double a2 = atan2(double(diffY2), double(diffX2));
    float angle = (a2 - a1) / (2.0 * 3.1415926535) * 360.0 / 10.0 * _trackballFactor;

    if (_pModeler->getSnapGridActive())
    {
        angle = ROUND(angle / _angleStep) * _angleStep;
    }

    // Si l'angle n'est pas nul
    if (angle != 0)
    {
        // On applique la rotation

        OMatrix tyMat;
        OMatrix tyMatTmpX;
        OMatrix tyMatTmpY;
        OMatrix tyMatTmpZ;

        double vpNormal[3];
        vpNormal[0] = _pInteractor->getRenderer()->getActiveCamera()->front.x;
        vpNormal[1] = _pInteractor->getRenderer()->getActiveCamera()->front.y;
        vpNormal[2] = _pInteractor->getRenderer()->getActiveCamera()->front.z;

        if (!enableX) { vpNormal[0] = 0.0; }
        if (!enableY) { vpNormal[2] = 0.0; }
        if (!enableZ) { vpNormal[1] = 0.0; }

        if (vpNormal[0] != 0) { tyMatTmpX.setRotationOx(-vpNormal[0]*M_PI * angle / 180); }
        if (vpNormal[2] != 0) { tyMatTmpY.setRotationOy(vpNormal[2]*M_PI * angle / 180); }
        if (vpNormal[1] != 0) { tyMatTmpZ.setRotationOz(-vpNormal[1]*M_PI * angle / 180); }

        tyMat = pRepere->asMatrix() * tyMatTmpZ * tyMatTmpY * tyMatTmpX;

        OPoint3D org = pRepere->_origin; // On conserve l'origine de depart
        pRepere->set(tyMat);
        pRepere->_origin = org;

        // Save mouse pos car il y a eu une rotation
        _lastMousePos[0] = _currentMousePos[0];
        _lastMousePos[1] = _currentMousePos[1];

        // On affiche la somme de l'angle que l'on applique durant cette rotation
        _angleInfo = ((int)(_angleInfo + angle)) % 360;
        updateText(QString(TR("id_angle_info")).arg(_angleInfo, 0, 'f', 2), (int)center[0], (int)center[1]);

        // Le repere a ete modifie
        ret = true;
    }

    //Chgmt de repere si besoin (si SiteNode pere)
    if (QString(_pModeler->metaObject()->className()).compare("TYSiteModelerFrame") == 0)
    {
        PointOfCurrentSiteInGeoNodeCoordinates(_pEditGeoNode, pRepere->_origin);
    }

    return ret;
}

int TYPositionEditor::findPoint(float* ptToFind, TYTabPoint* pInPoints)
{
    int ptId = -1;

    for (size_t i = 0; i < pInPoints->size(); i++)
    {
        TYPoint curPt = pInPoints->at(i);

        if ((ptToFind[0] >= curPt._x - _pickPointPrecision) &&
            (ptToFind[0] <= curPt._x + _pickPointPrecision) &&
            (-ptToFind[2] >= curPt._y - _pickPointPrecision) &&
            (-ptToFind[2] <= curPt._y + _pickPointPrecision))
        {
            // Point trouve !
            ptId = i;

            updateMovingLines(pInPoints, ptId);

            break;
        }
    }

    return ptId;
}

void TYPositionEditor::updateMovingLines(TYTabPoint* pPoints, int centralPointId)
{
    TYPoint centralPoint = pPoints->at(centralPointId);
    if (_pEditGeoNode) { centralPoint = _pEditGeoNode->getMatrix() * centralPoint; }

    if (centralPointId - 1 >= 0)
    {
        TYPoint prevPoint = pPoints->at(centralPointId - 1);
        if (_pEditGeoNode) { prevPoint = _pEditGeoNode->getMatrix() * prevPoint; }

        _pOGLLineElementMoving1->setPoint1(OPoint3D(prevPoint._x, prevPoint._z, -prevPoint._y));
        _pOGLLineElementMoving1->setPoint2(OPoint3D(centralPoint._x, centralPoint._z, -centralPoint._y));
        _pOGLLineElementMoving1->setVisibility(true);
    }

    if (centralPointId + 1 < (int)pPoints->size())
    {
        TYPoint nextPoint = pPoints->at(centralPointId + 1);
        if (_pEditGeoNode) { nextPoint = _pEditGeoNode->getMatrix() * nextPoint; }

        _pOGLLineElementMoving2->setPoint1(OPoint3D(centralPoint._x, centralPoint._z, -centralPoint._y));
        _pOGLLineElementMoving2->setPoint2(OPoint3D(nextPoint._x, nextPoint._z, -nextPoint._y));
        _pOGLLineElementMoving2->setVisibility(true);
    }
}


void TYPositionEditor::updateMovingPoint(const TYPoint& point)
{
    TYPoint centralPoint(point);
    if (_pEditGeoNode) { centralPoint = _pEditGeoNode->getMatrix() * centralPoint; }

    _pOGLLineElementMoving1->setPoint2(OPoint3D(centralPoint._x, centralPoint._z, -centralPoint._y));
    _pOGLLineElementMoving2->setPoint1(OPoint3D(centralPoint._x, centralPoint._z, -centralPoint._y));
}

bool TYPositionEditor::selectElement(TYElement* pElt)
{
    bool stop = false;

    // Recherche du siteNodeParent
    TYElement* pParent = pElt->getParent();

    while (pParent && (dynamic_cast<TYSiteNode*>(pParent) == nullptr))
    {
        pParent = pParent->getParent();
    }

    // Si le modeleur point un projet on prend le sitenode racine
    TYElement* pModelerTarget = NULL;
    if (_pModeler->getElement()->isA("TYProjet"))
    {
        pModelerTarget = TYProjet::safeDownCast(_pModeler->getElement())->getSite();
    }
    else
    {
        pModelerTarget = _pModeler->getElement();
    }

    // Si le site parent n'est pas le meme que le site pointe par le modeleur
    if (pModelerTarget && (pParent != pModelerTarget))
    {
        // On initialise sur lui
        initEditSiteNode(pParent);
        stop = true; // La methode appelante devra se terminer
    }

    return stop;
}

void TYPositionEditor::editPolyLine(LPTYElement pElt, TYTabPoint& pts, bool closed, LPTYAltimetrie pAlti)
{
    int ptIdToEdit = 0;
    if (_keyAOn) // Insert a point
    {
        TYPoint ptInsert;
        insertNewPoint(pts, ptInsert, ptIdToEdit, pAlti);
        _pLastAction = new TYInsertPointPolyLineAction(pElt, pts, ptIdToEdit, ptInsert, _pModeler, TR("id_action_insertptpolyline"));
    }
    else if (_keyDOn) // Delete a point
    {
        size_t nbPts = pts.size();
        if (nbPts < 3 || (closed && nbPts < 4))
        {
            return;
        }
        TYPoint ptDelete;
        deletePoint(pts, ptDelete, ptIdToEdit);
        _pLastAction = new TYRemovePointPolyLineAction(pElt, pts, ptIdToEdit, ptDelete, _pModeler, TR("id_action_delptpolyline"));
    }
    else // Move a point
    {
        // We find the id of the closest point to the mouse
        ptIdToEdit = findClosestPointToMouse(pts);
        _pEditPoint = &pts[ptIdToEdit];
        _pLastAction = new TYMovePointPolyLineAction(pElt, pts, ptIdToEdit, *_pEditPoint, TYPoint(), _pModeler, TR("id_action_moveptpolyline"));
        // Is the highlight working...?
        TYPoint worldPoint = _pEditGeoNode->getMatrix() * (*_pEditPoint);
        _pPicker->highlightElement(&worldPoint);
        // Connecte the timer
        QObject::connect(_pStepTimer, SIGNAL(timeout()), this, SLOT(editPoint()));
        _inUse = true;
    }
}

void TYPositionEditor::insertNewPoint(TYTabPoint& pts, TYPoint& newPoint, int& idInsert, LPTYAltimetrie pAlti)
{
    idInsert = findClosestSegmentToMouse(pts);
    double ptProj[3];
    displayToWorld(_currentMousePos[0], _pInteractor->height() - _currentMousePos[1], 0.5, ptProj);
    newPoint = TYPoint(ptProj[0], -ptProj[2], pts[idInsert]._z);

    // Si la grille magnetique est activee
    if (_pModeler->getSnapGridActive())
    {
        snapToGrid(newPoint._x, newPoint._y, newPoint._z);
    }

    ++idInsert;
    if (pAlti && !pAlti->altitude(newPoint))
    {
        newPoint._z = std::numeric_limits<double>::quiet_NaN();
    }
    pts.insert(pts.begin() + idInsert, newPoint);
}

void TYPositionEditor::deletePoint(TYTabPoint& pts, TYPoint& deletedPoint, int& idDelete)
{
    idDelete = findClosestPointToMouse(pts);
    deletedPoint = pts[idDelete];
    pts.erase(pts.begin() + idDelete);
}

int TYPositionEditor::findClosestSegmentToMouse(const TYTabPoint& pts)
{
    int idPt = 0;
    size_t nbPts = pts.size();
    double ptOnDisplay[3];
    OVector3D pt(0, 0, 0);
    const OMatrix& matrixNode = _pEditGeoNode->getMatrix();

    std::vector<OVector3D> ptsDisplay;
    //We project on the display the points of our polyline
    for (size_t i = 0; i < nbPts; ++i)
    {
        //Is the transformation really necessary?
        pt = matrixNode * pts[i];
        worldToDisplay(pt._x, pt._z, -pt._y, ptOnDisplay);
        ptsDisplay.push_back(OVector3D(ptOnDisplay[0], ptOnDisplay[1], 0.0));
    }

    double distSqrMin = (std::numeric_limits<double>::max)();
    double distSqr = 0;
    double mx = _currentMousePos[0];
    double my = _pInteractor->height() - _currentMousePos[1];

    //Find the closest line to our mouse position
    int j = 0;
    for (size_t i = 0; i < nbPts; ++i)
    {
        j = i < (nbPts - 1) ? i + 1 : 0;
        const OVector3D& v = ptsDisplay[i];
        const OVector3D& w = ptsDisplay[j];

        distSqr = distSegmentSqr(mx, my, v._x, v._y, w._x, w._y);
        if (distSqr < distSqrMin)
        {
            distSqrMin = distSqr;
            idPt = i;
        }
    }
    return idPt;
}

int TYPositionEditor::findClosestPointToMouse(const TYTabPoint& pts)
{
    int ptId = -1;
    size_t nbPts = pts.size();
    double distSqrMin = (std::numeric_limits<double>::max)();
    double ptOnDisplay[3];
    double diffx = 0, diffy = 0, distSqr = 0;
    OVector3D pt(0, 0, 0);
    const OMatrix& matrixNode = _pEditGeoNode->getMatrix();

    for (size_t i = 0; i < nbPts; ++i)
    {
        // Is the transformation really necessary?
        pt = matrixNode * pts[i];
        worldToDisplay(pt._x, pt._z, -pt._y, ptOnDisplay);
        diffx = ptOnDisplay[0] - _currentMousePos[0];
        diffy = ptOnDisplay[1] - _pInteractor->height() + _currentMousePos[1];
        distSqr = diffx * diffx + diffy * diffy;
        if (distSqr < distSqrMin)
        {
            distSqrMin = distSqr;
            ptId = i;
        }
    }
    return ptId;
}

double TYPositionEditor::distSegmentSqr(double mx, double my, double vx, double vy, double wx, double wy) const
{
    double l2 = (vx - wx) * (vx - wx) + (vy - wy) * (vy - wy) ;
    if (l2 != 0)
    {
        double t = ((mx - vx) * (wx - vx) + (my - vy) * (wy - vy)) / l2;
        if (t < 0)
        {
            return (mx - vx) * (mx - vx) + (my - vy) * (my - vy);
        }
        else if (t > 1)
        {
            return (mx - wx) * (mx - wx) + (my - wy) * (my - wy);
        }
        else
        {
            return (mx - (vx + t * (wx - vx))) * (mx - (vx + t * (wx - vx))) + (my - (vy + t * (wy - vy))) * (my - (vy + t * (wy - vy)));
        }
    }
    else
    {
        return (mx - vx) * (mx - vx) + (my - vy) * (my - vy);
    }
}

